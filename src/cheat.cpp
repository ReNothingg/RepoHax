#include "pch.h"

#include "cheat.h"
#include <third_party/haxsdk/unity/hax_unity_uvm.h>

namespace Cheat
{
    extern void HookMonoRuntimeInvoke(); // game_hooks.cpp
    extern void HookWinProcs(); // win_hooks.cpp
    extern void HookGraphics(); // graphics_hooks.cpp
    extern void HookGameFuncs(); // game_hooks.cpp
    extern void HookPlayerLoop(); // game_hooks.cpp

    Context* G;

    static void SetupConfig(Hax::IniFile& ini);
    static void WaitForUnityToLoad();

    void Initialize(HMODULE hCheatDll)
    {
        if (!Hax::Unity::IsUnityProcess())
            return;

        G = new Context();
        G->Handle = hCheatDll;

        SetupConfig(G->Config);
        Hax::InitLogFile(G->Logger, L"haxsdk_logs.txt", G->UseConsole);

        WaitForUnityToLoad();
        Hax::Unity::Initialize(&G->Logger);

        HookWinProcs();

        HMODULE hDirectX11 = 0;
        do 
        { 
            Sleep(200); 
            hDirectX11 = ::GetModuleHandleW(L"d3d11.dll"); 
        } while (hDirectX11 == 0);

        HookGraphics();
        HookGameFuncs();

        while (!G->PlayerLoop.ReadyToHook) 
            Sleep(200);
        HookPlayerLoop();
    }

    template <int Default, int Min, int Max>
    static void IniFileRead_IntClamped(void* data, const char* str)
    {
        int value = Default;
        if (sscanf_s(str, "%d", &value) != 1)
            value = Default;

        *(int*)data = Hax::Clamp(value, Min, Max);
    }

    static void SetupConfig(Hax::IniFile& ini)
    {
        Hax::IniAddEntry(ini, "Settings",   "UseConsole",          &G->UseConsole,          Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Settings",   "VkOpenClose",         &G->VkOpenClose,         Hax::IniFileWrite_Int,  IniFileRead_IntClamped<VK_OEM_3, 1, 255>);
        Hax::IniAddEntry(ini, "Settings",   "Language",            &G->Language,            Hax::IniFileWrite_Int,  IniFileRead_IntClamped<0, 0, Lang_Count - 1>);
        Hax::IniAddEntry(ini, "Settings",   "DarkenBackground",    &G->DarkenBackground,    Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);

        Hax::IniAddEntry(ini, "Stats",      "Godmode",             &G->Godmode,             Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Stats",      "InfiniteStamina",     &G->InfStamina,          Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Stats",      "WalkingSpeed",        &G->Accel.Walking,       Hax::IniFileWrite_Int,  IniFileRead_IntClamped<1, 1, 5>);
        Hax::IniAddEntry(ini, "Stats",      "SprintingSpeed",      &G->Accel.Sprinting,     Hax::IniFileWrite_Int,  IniFileRead_IntClamped<1, 1, 5>);
        Hax::IniAddEntry(ini, "Stats",      "CrouchingSpeed",      &G->Accel.Crouching,     Hax::IniFileWrite_Int,  IniFileRead_IntClamped<1, 1, 5>);
        Hax::IniAddEntry(ini, "Stats",      "InfiniteJumps",       &G->InfJumps,            Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Stats",      "NoTumble",            &G->NoTumble,            Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Stats",      "VkToggleFlight",      &G->VkToggleFlight,      Hax::IniFileWrite_Int,  IniFileRead_IntClamped<VK_F7, 0, 255>);
        Hax::IniAddEntry(ini, "Stats",      "FlightSpeed",         &G->FlightSpeed,         Hax::IniFileWrite_Int,  IniFileRead_IntClamped<10, 1, 30>);
        Hax::IniAddEntry(ini, "Stats",      "FlightSprintBoost",    &G->FlightSprintBoost,   Hax::IniFileWrite_Int,  IniFileRead_IntClamped<3, 1, 6>);
        Hax::IniAddEntry(ini, "Stats",      "VkTeleportPlayerToCamera", &G->VkTeleportPlayerToCamera, Hax::IniFileWrite_Int, IniFileRead_IntClamped<VK_F9, 0, 255>);
        Hax::IniAddEntry(ini, "Stats",      "EyeLasersEnabled",    &G->EyeLasersEnabled,    Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Stats",      "VkToggleEyeLasers",   &G->VkToggleEyeLasers,   Hax::IniFileWrite_Int,  IniFileRead_IntClamped<VK_F8, 0, 255>);
        Hax::IniAddEntry(ini, "Stats",      "EyeLaserRange",       &G->EyeLaserRange,       Hax::IniFileWrite_Int,  IniFileRead_IntClamped<80, 10, 200>);
        Hax::IniAddEntry(ini, "Stats",      "EyeLaserDamage",      &G->EyeLaserDamage,      Hax::IniFileWrite_Int,  IniFileRead_IntClamped<250, 10, 2000>);
        Hax::IniAddEntry(ini, "Stats",      "EyeLaserInstantKill", &G->EyeLaserInstantKill, Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Stats",      "EyeLaserDestroyObjects", &G->EyeLaserDestroyObjects, Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Stats",      "EasyGrab",            &G->EasyGrab,            Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Stats",      "UnlimitedGrabRange",  &G->UnlimGrabRange,      Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Stats",      "NoOverCharge",        &G->NoOvercharge,        Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Stats",      "AutoUseUpgrades",     &G->AutoUseUpgrs,        Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);

        static constexpr const char* s_Names[(int)(UpgradeType::N)] = 
        {
            "CrouchRest", "ExtraJump", "Range", "SprintSpeed", "Stamina", "Strength",
            "Launch", "Wings", "Health", "PlayersCount", "Climb", "HeadBattery"
        };

        for (int i = 0; i < (int)UpgradeType::N; ++i)
            Hax::IniAddEntry(ini, "UpgradeAutoUse", s_Names[i], &G->UpgradesData[i].AutoUse, Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);

        Hax::IniAddEntry(ini, "Entities",   "NoGrabMaxTime",       &G->NoGrabMaxTime,       Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Entities",   "EnemiesEsp",          &G->EnemiesEsp,          Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Entities",   "PlayersEsp",          &G->PlayersEsp,          Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Entities",   "PlayersChams",        &G->PlayersChams,        Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);

        Hax::IniAddEntry(ini, "Vision",     "BetterVision",        &G->BetterVision,        Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Vision",     "NoFog",               &G->NoFog,               Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Vision",     "DisableOcclusion",    &G->DisableOcclusionCulling, Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Vision",     "RenderDistance",      &G->RenderDistance,      Hax::IniFileWrite_Int,  IniFileRead_IntClamped<200, 32, 500>);
        Hax::IniAddEntry(ini, "Vision",     "ThirdPerson",         &G->ThirdPerson,         Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Vision",     "Fov",                 &G->Fov,                 Hax::IniFileWrite_Int,  IniFileRead_IntClamped<60, 60, 140>);
        Hax::IniAddEntry(ini, "Vision",     "FlashlightIntensity", &G->Flashlight.Intensity,Hax::IniFileWrite_Int,  IniFileRead_IntClamped<10, 10, 20>);
        Hax::IniAddEntry(ini, "Vision",     "FlashAngle",          &G->Flashlight.Angle,    Hax::IniFileWrite_Int,  IniFileRead_IntClamped<60, 60, 120>);
        Hax::IniAddEntry(ini, "Vision",     "FlashInCrouch",       &G->Flashlight.InCrouch, Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Vision",     "MaxHeadBattery",      &G->MaxHeadBattery,      Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);

        Hax::IniAddEntry(ini, "Items",      "ObjectRemoverEnabled", &G->ObjectRemoverEnabled, Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Items",      "VkDeleteObject",      &G->VkDeleteObject,      Hax::IniFileWrite_Int,  IniFileRead_IntClamped<VK_F10, 0, 255>);
        Hax::IniAddEntry(ini, "Items",      "ObjectRemoverRange",  &G->ObjectRemoverRange,  Hax::IniFileWrite_Int,  IniFileRead_IntClamped<25, 3, 100>);
        Hax::IniAddEntry(ini, "GodTools",   "VkLockTarget",       &G->VkLockGodTarget,      Hax::IniFileWrite_Int,  IniFileRead_IntClamped<VK_F11, 0, 255>);
        Hax::IniAddEntry(ini, "GodTools",   "HoldDistance",       &G->GodHoldDistance,      Hax::IniFileWrite_Int,  IniFileRead_IntClamped<5, 1, 25>);
        Hax::IniAddEntry(ini, "GodTools",   "ThrowForce",         &G->GodThrowForce,        Hax::IniFileWrite_Int,  IniFileRead_IntClamped<35, 5, 150>);
        Hax::IniAddEntry(ini, "GodTools",   "TimeScale",          &G->WorldTimeScalePercent,Hax::IniFileWrite_Int,  IniFileRead_IntClamped<100, 0, 300>);
        Hax::IniAddEntry(ini, "GodTools",   "GravityY",           &G->WorldGravityY,        Hax::IniFileWrite_Int,  IniFileRead_IntClamped<-10, -30, 30>);

        Hax::IniAddEntry(ini, "Valuables",  "ValuablesEsp",        &G->ValuablesEsp,        Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Valuables",  "ValuablesEspRange",   &G->ValuablesEspRange,   Hax::IniFileWrite_Int,  IniFileRead_IntClamped<50, 5, 500>);
        Hax::IniAddEntry(ini, "Valuables",  "ValuablesChams",      &G->ValuablesChams,      Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Valuables",  "Unbreakable",         &G->Unbreakable,         Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Valuables",  "ExtrPointsEsp",       &G->ExtrPointsEsp,       Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);

        Hax::IniAddEntry(ini, "Items",      "InfBattery",          &G->InfBattery,          Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Items",      "UseLaser",            &G->UseLaser,            Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Items",      "VkSpawnItem",         &G->VkSpawnItem,         Hax::IniFileWrite_Int,  IniFileRead_IntClamped<VK_F6, 0, 255>);

        Hax::IniAddEntry(ini, "Misc",       "TruckEsp",            &G->TruckEsp,            Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Misc",       "PreserveSaveOnDeath", &G->PreserveSaveOnDeath, Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Misc",       "SessionSafetyEnabled", &G->SessionSafetyEnabled, Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Misc",       "AutoCancelClientUnsafe", &G->AutoCancelClientUnsafe, Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Misc",       "DisableAllPlayersDeadCheck", &G->DisableAllPlayersDeadCheck, Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Misc",       "CurrencyChangeAmount", &G->CurrencyChangeAmount, Hax::IniFileWrite_Int, IniFileRead_IntClamped<1000, 1, 100000>);

        Hax::IniLoad(G->Config);
        G->Loc = g_LocDict[G->Language];
    }

    static void WaitForUnityToLoad()
    {
        Hax::Log(G->Logger, L"Waiting for unity virtual machine...");
        while (!Hax::Unity::GetUvmHandle())
            ::Sleep(200);

        HANDLE hEvent = ::CreateEvent(0, TRUE, FALSE, nullptr);
        HAX_PANIC(hEvent != nullptr, &G->Logger, L"%zu", HAX_LINE);

        G->UnityLoadedEvent = hEvent;
        HookMonoRuntimeInvoke();

        Hax::Log(G->Logger, L"Waiting for unity...");
        ::WaitForSingleObject(hEvent, INFINITE);
        ::CloseHandle(hEvent);
    }
}
