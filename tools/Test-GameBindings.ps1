#requires -Version 5.1
<#
.SYNOPSIS
Checks native game bindings against installed managed assemblies without running game code.
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string] $ManagedDirectory
)

$ErrorActionPreference = 'Stop'
if ($PSVersionTable.PSEdition -eq 'Core') {
    & "$env:SystemRoot\System32\WindowsPowerShell\v1.0\powershell.exe" -NoProfile -NonInteractive -ExecutionPolicy Bypass `
        -File $PSCommandPath -ManagedDirectory $ManagedDirectory
    exit $LASTEXITCODE
}

$managed = (Resolve-Path -LiteralPath $ManagedDirectory).Path
$root = Split-Path -Parent $PSScriptRoot
$header = Get-Content -Raw -LiteralPath (Join-Path $root 'src\game_classes.h')
$hooks = Get-Content -Raw -LiteralPath (Join-Path $root 'src\hooks\game_hooks.cpp')
$resolver = [ResolveEventHandler] {
    param($sender, $eventArgs)
    $name = New-Object System.Reflection.AssemblyName($eventArgs.Name)
    $path = Join-Path $managed ($name.Name + '.dll')
    if (Test-Path -LiteralPath $path) {
        return [Reflection.Assembly]::ReflectionOnlyLoadFrom($path)
    }
    return [Reflection.Assembly]::ReflectionOnlyLoad($eventArgs.Name)
}
[AppDomain]::CurrentDomain.add_ReflectionOnlyAssemblyResolve($resolver)

$flags = [Reflection.BindingFlags]'Instance,Static,Public,NonPublic,DeclaredOnly'
$failures = New-Object 'System.Collections.Generic.List[string]'
$checked = 0

function Find-Field($type, $name) {
    while ($null -ne $type) {
        $field = $type.GetField($name, $flags)
        if ($null -ne $field) { return $field }
        $type = $type.BaseType
    }
}

function Find-Methods($type, $name) {
    while ($null -ne $type) {
        $methods = @($type.GetMethods($flags) | Where-Object Name -eq $name)
        if ($methods.Count -gt 0) { return $methods }
        $type = $type.BaseType
    }
}

function Get-Signature($method) {
    $parameters = @($method.GetParameters() | ForEach-Object { $_.ParameterType.FullName })
    return $method.ReturnType.FullName + '(' + ($parameters -join ',') + ')'
}

function Get-ArgumentCount([string] $arguments) {
    if ([string]::IsNullOrWhiteSpace($arguments)) { return 0 }
    $depth = 0
    $count = 1
    foreach ($ch in $arguments.ToCharArray()) {
        if ($ch -eq '(' -or $ch -eq '<') { $depth++ }
        elseif ($ch -eq ')' -or $ch -eq '>') { $depth-- }
        elseif ($ch -eq ',' -and $depth -eq 0) { $count++ }
    }
    return $count
}

try {
    $assemblies = @{}
    foreach ($struct in [regex]::Matches($header, 'struct (\w+)[^{;]*\{([\s\S]*?)(?=\r?\n\};)')) {
        $nativeName = $struct.Groups[1].Value
        $body = $struct.Groups[2].Value
        $meta = [regex]::Match($body, 'META\("([^"]+)",\s*"([^"]*)",\s*"([^"]+)"\)')
        if (-not $meta.Success) { continue }
        $assemblyName = $meta.Groups[1].Value
        if (-not $assemblies.ContainsKey($assemblyName)) {
            $assemblies[$assemblyName] = [Reflection.Assembly]::ReflectionOnlyLoadFrom((Join-Path $managed ($assemblyName + '.dll')))
        }
        $typeName = $meta.Groups[3].Value.Replace('/', '+')
        if ($meta.Groups[2].Value) { $typeName = $meta.Groups[2].Value + '.' + $typeName }
        $type = $assemblies[$assemblyName].GetType($typeName, $true)
        $checked++

        foreach ($entry in [regex]::Matches($body, '\b(STATIC_FIELD|FIELD|PROPERTY|ENUM_VALUE)\((\w+)[,)]')) {
            $kind = $entry.Groups[1].Value
            $name = $entry.Groups[2].Value
            if ($kind -eq 'PROPERTY') { $name = '<' + $name + '>k__BackingField' }
            $field = Find-Field $type $name
            $checked++
            if ($null -eq $field) { $failures.Add("Missing field: $typeName.$name"); continue }
            if (($kind -eq 'STATIC_FIELD' -or $kind -eq 'ENUM_VALUE') -and -not $field.IsStatic) {
                $failures.Add("Expected static field: $typeName.$name")
            }
            if ($kind -eq 'FIELD' -and $field.IsStatic) { $failures.Add("Expected instance field: $typeName.$name") }
        }

        $boundMethods = @{}
        $entries = @([regex]::Matches($body, 'METHOD_WRAPPER(?:_EX)?\((\w+)(?:,\s*"([^"]+)")?\)'))
        $entries += @([regex]::Matches($body, 'PLAYER_UPGRADE_METHOD\((\w+)\)'))
        foreach ($entry in $entries) {
            $name = $entry.Groups[1].Value
            if ($name -eq 'name') { continue } # Macro definition, not a binding.
            $methods = @(Find-Methods $type $name)
            $signature = $entry.Groups[2].Value
            if ($signature) { $methods = @($methods | Where-Object { (Get-Signature $_) -eq $signature }) }
            $checked++
            if ($methods.Count -ne 1) { $failures.Add("Expected one matching method: $typeName.$name $signature (found $($methods.Count))"); continue }
            $boundMethods[$name] = $methods[0]
        }

        # Check actual native calls, including explicitly supplied optional arguments.
        foreach ($call in [regex]::Matches($body, '\bs_(\w+)\.Call<[^;{}]*?>\(([^;]*?)\);')) {
            $name = $call.Groups[1].Value
            if (-not $boundMethods.ContainsKey($name)) { continue }
            $method = $boundMethods[$name]
            $actual = Get-ArgumentCount $call.Groups[2].Value
            if (-not $method.IsStatic) { $actual-- }
            $expected = $method.GetParameters().Count
            $checked++
            if ($actual -ne $expected) { $failures.Add("Native call $typeName.$name supplies $actual arguments; game requires $expected") }
        }

        $hookPattern = 'static \w+ Hooked__' + [regex]::Escape($nativeName) + '_(\w+)\(([^;{}]*)\);'
        foreach ($hook in [regex]::Matches($hooks, $hookPattern)) {
            $name = $hook.Groups[1].Value
            if (-not $boundMethods.ContainsKey($name)) { continue }
            $method = $boundMethods[$name]
            $actual = Get-ArgumentCount $hook.Groups[2].Value
            if (-not $method.IsStatic) { $actual-- }
            $expected = $method.GetParameters().Count
            $checked++
            if ($actual -ne $expected) { $failures.Add("Native hook $typeName.$name receives $actual arguments; game supplies $expected") }
        }
    }
    # The relay writes one enum field and boxes one native value type. Field existence
    # alone would miss a four-byte write over ReceiverGroup's one-byte storage.
    $receiverType = $assemblies['PhotonRealtime'].GetType('Photon.Realtime.ReceiverGroup', $true)
    $deliveryType = $assemblies['Photon3Unity3D'].GetType('ExitGames.Client.Photon.DeliveryMode', $true)
    $checked += 2
    if ($receiverType.GetEnumUnderlyingType().FullName -ne 'System.Byte') { $failures.Add('ReceiverGroup must use one-byte storage') }
    if ($deliveryType.GetEnumUnderlyingType().FullName -ne 'System.Int32') { $failures.Add('DeliveryMode must use four-byte storage') }
    $sendType = $assemblies['Photon3Unity3D'].GetType('ExitGames.Client.Photon.SendOptions', $true)
    $sendFields = @($sendType.GetFields($flags) | Where-Object { -not $_.IsStatic } | Sort-Object MetadataToken)
    $sendShape = @($sendFields | ForEach-Object { $_.Name + ':' + $_.FieldType.FullName }) -join ','
    $checked++
    if (-not $sendType.IsLayoutSequential -or $sendShape -ne 'DeliveryMode:ExitGames.Client.Photon.DeliveryMode,Encrypt:System.Boolean,Channel:System.Byte') {
        $failures.Add('SendOptions no longer matches the native relay layout')
    }

    # Unity engine bindings have a separate ABI from Assembly-CSharp. In Unity 6
    # injected calls receive Object.m_CachedPtr as IntPtr, not the Mono object.
    foreach ($module in @('core', 'physics', 'physics2d', 'ai')) {
        $sdkPath = Join-Path $root ("third_party\haxsdk\unity\modules\hax_unity_$module.cpp")
        $sdk = Get-Content -Raw -LiteralPath $sdkPath
        $types = @{}
        foreach ($entry in [regex]::Matches($sdk, 'System::Type (\w+)::typeof\(\)\s*\{([^}]+)\}')) {
            $meta = [regex]::Match($entry.Groups[2].Value, 'System::Type::GetType\("([^"]+)",\s*"([^"]+)",\s*"([^"]+)"')
            if (-not $meta.Success) { continue }
            $assemblyName = $meta.Groups[1].Value
            if (-not $assemblies.ContainsKey($assemblyName)) {
                $assemblies[$assemblyName] = [Reflection.Assembly]::ReflectionOnlyLoadFrom((Join-Path $managed ($assemblyName + '.dll')))
            }
            $types[$entry.Groups[1].Value] = $assemblies[$assemblyName].GetType(($meta.Groups[2].Value + '.' + $meta.Groups[3].Value), $true)
        }
        foreach ($function in [regex]::Matches($sdk, '(?ms)^    [\w:<>]+ (\w+)::(\w+)\([^;{]*\)(?: const)?\s*\{(.*?)^    \}')) {
            $body = $function.Groups[3].Value
            $call = [regex]::Match($body, 's_Method\.InternalCall<([^>]+)>\(([^;]*?)\);')
            if (-not $call.Success) { continue }
            $lookup = [regex]::Match($body, 'GetMethod\("([^"]+)"')
            $type = $types[$function.Groups[1].Value]
            $methods = @(Find-Methods $type $lookup.Groups[1].Value)
            $checked++
            if ($methods.Count -ne 1) { $failures.Add("Missing/ambiguous SDK internal call: $($function.Groups[1].Value).$($lookup.Groups[1].Value)"); continue }
            $method = $methods[0]
            $parameters = @($method.GetParameters())
            $actual = Get-ArgumentCount $call.Groups[2].Value
            if (-not $method.IsStatic) { $actual-- }
            if ($actual -ne $parameters.Count) { $failures.Add("SDK internal call $($method.Name) supplies $actual arguments; engine requires $($parameters.Count)") }
            if ($parameters.Count -gt 0 -and $parameters[0].ParameterType.FullName -eq 'System.IntPtr' -and
                $call.Groups[2].Value -notmatch '^GetCachedPtr\(\)\s*,') {
                $failures.Add("SDK internal call $($function.Groups[1].Value).$($method.Name) requires a native pointer from GetCachedPtr()")
            }
            if ($method.ReturnType.FullName -eq 'System.IntPtr' -and $call.Groups[1].Value -notmatch '^(void\*|System::IntPtr)$') {
                $failures.Add("SDK internal call $($method.Name) returns a native pointer, not a managed $($call.Groups[1].Value)")
            }
        }
    }
    if ($failures.Count -gt 0) { throw ($failures -join [Environment]::NewLine) }
    Write-Host "PASS: $checked type, field, method-signature, native-call, hook and Unity engine ABI checks against $managed"
} finally {
    [AppDomain]::CurrentDomain.remove_ReflectionOnlyAssemblyResolve($resolver)
}
