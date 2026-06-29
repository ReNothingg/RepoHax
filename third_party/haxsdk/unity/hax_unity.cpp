#include "hax_unity.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>

#include "hax_unity_uvm.h"
#include "modules/hax_unity_mscorlib.h"

static bool GetInternalCallsReturnHandles()
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        UvmMethod* method = uvm_class_get_method_from_name(uvm_appdomain_get_class(), "getRootDomain", {});
        HAX_ASSERT(method != nullptr);

        using Func = void*(*)(MonoError*);
        Func icall = (Func)uvm_lookup_internal_call(method);
        HAX_ASSERT(icall != nullptr);

        MonoError err{};
        UvmAppDomain* variant1 = (UvmAppDomain*)icall(&err);
        UvmAppDomain** variant2 = (UvmAppDomain**)variant1;

        UvmDomain* domain = uvm_get_root_domain();
        if (variant1->UvmDomain == domain)
            return false;

        HAX_ASSERT((*variant2)->UvmDomain == domain);
        return true;
    }
    else
    {
        return false;
    }
}

static Hax::Vector<Hax::Unity::TypeRequest>& GetTypeRequests()
{
    static Hax::Vector<Hax::Unity::TypeRequest> s_Vector;
    return s_Vector;
}

static Hax::Vector<Hax::Unity::MethodRequest>& GetMethodRequests()
{
    static Hax::Vector<Hax::Unity::MethodRequest> s_Vector;
    return s_Vector;
}

static Hax::Vector<Hax::Unity::MethodWrapperRequest>& GetMethodWrapperRequests()
{
    static Hax::Vector<Hax::Unity::MethodWrapperRequest> s_Vector;
    return s_Vector;
}

static Hax::Vector<Hax::Unity::FieldOffsetRequest>& GetFieldOffsetRequests()
{
    static Hax::Vector<Hax::Unity::FieldOffsetRequest> s_Vector;
    return s_Vector;
}

static Hax::Vector<Hax::Unity::StaticFieldRequest>& GetStaticFieldRequests()
{
    static Hax::Vector<Hax::Unity::StaticFieldRequest> s_Vector;
    return s_Vector;
}

static Hax::Vector<Hax::Unity::EnumRequest>& GetEnumRequests()
{
    static Hax::Vector<Hax::Unity::EnumRequest> s_Vector;
    return s_Vector;
}

namespace Hax::Unity
{
    bool g_ICallsReturnHandle;
    Hax::Vector<void*> g_BoxesPool;

    Hax::LogFile* g_Logger;

    void Initialize(LogFile* logFile)
    {
        HAX_ASSERT(GetUvmHandle() != 0 && "Not a unity game");
        g_Logger = logFile;

        if (logFile != nullptr)
            Hax::LogDebug(*logFile, L"Initializing predefined data...");

        UvmThread* thread = uvm_thread_attach(uvm_get_root_domain());
        g_ICallsReturnHandle = GetInternalCallsReturnHandles();
        for (TypeRequest& request : GetTypeRequests())
            *request.Out = System::Type::GetType(request.Assembly, request.Namespace, request.Name, true);
        for (MethodRequest& request : GetMethodRequests())
            *request.Out = request.Type->GetMethod(request.Name, request.Sig, true);
        for (MethodWrapperRequest& request : GetMethodWrapperRequests())
            *request.Out = request.Type->GetMethod(request.Name, request.Sig, true).Wrap(request.Flags);
        for (FieldOffsetRequest& request : GetFieldOffsetRequests())
            *request.Out = request.Type->GetField(request.Name, true).GetFieldOffset();
        for (StaticFieldRequest& request : GetStaticFieldRequests())
            *request.Out = request.Type->GetField(request.Name, true).GetStaticAddress();
        for (EnumRequest& request : GetEnumRequests())
        {
            System::FieldInfo field = request.Type->GetField(request.Name, true);
            HAX_ASSERT(field.IsLiteral());
            field.GetStaticValue(request.Out);
        }
        uvm_thread_detach(thread);
    }

    bool IsUnityProcess()
    {
        static int s_Cached = -1;
        if (s_Cached >= 0)
            return (bool)s_Cached;

        DWORD processId = ::GetProcessId(::GetCurrentProcess());
        HANDLE hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);

        MODULEENTRY32W me = {};
        me.dwSize = sizeof(MODULEENTRY32W);
        if (::Module32FirstW(hSnapshot, &me))
        {
            do
            {
                if (_wcsicmp(me.szModule, L"UnityPlayer.dll") == 0)
                {
                    ::CloseHandle(hSnapshot);
                    s_Cached = 1;
                    return true;
                }
            } while (::Module32NextW(hSnapshot, &me));
        }

        ::CloseHandle(hSnapshot);
        s_Cached = 0;
        return false;
    }

    Handle GetUvmHandle()
    {
        static Handle s_UvmHandle;
        if (s_UvmHandle != 0)
            return s_UvmHandle;

        MODULEENTRY32W me = {.dwSize = sizeof(MODULEENTRY32W)};
        HANDLE hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetCurrentProcessId());
        if (::Module32FirstW(hSnapshot, &me))
        {
            do
            {
                if (me.hModule == 0)
                    continue;

                if (::GetProcAddress(me.hModule, "il2cpp_init") != nullptr)
                {
                    if constexpr (!HAX_UNITY_IS_MONO)
                    {
                        s_UvmHandle = (Handle)me.hModule;
                        break;
                    }
                    else
                        HAX_ASSERT(false); //! TODO: add msg
                }
                if (::GetProcAddress(me.hModule, "mono_init") != nullptr)
                {
                    if constexpr (HAX_UNITY_IS_MONO)
                    {
                        s_UvmHandle = (Handle)me.hModule;
                        break;
                    }
                    else
                        HAX_ASSERT(false); //! TODO: add msg
                }
            } while (::Module32NextW(hSnapshot, &me));
        }
        ::CloseHandle(hSnapshot);

        return s_UvmHandle;
    }

    Handle AttachThread()
    {
        return (Handle)uvm_thread_attach(uvm_get_root_domain());
    }

    void DetachThread(Handle hThread)
    {
        uvm_thread_detach((UvmThread*)hThread);
    }

    TypeRequest& RequestType(const char* assembly, const char* namespaze, const char* name)
    {
        auto& requests = GetTypeRequests();
        requests.PushBack(TypeRequest{.Assembly = assembly,
            .Namespace = namespaze, 
            .Name = name});
        return requests.Last();
    }

    MethodRequest& RequestMethod(System::Type& type, const char* name, const char* sig)
    {
        auto& requests = GetMethodRequests();
        requests.PushBack(MethodRequest{.Type = &type,
            .Name = name,
            .Sig = sig});
        return requests.Last();
    }

    MethodWrapperRequest& RequestMethodWrapper(System::Type& type, const char* name, const char* sig, int flags)
    {
        auto& requests = GetMethodWrapperRequests();//!
        requests.PushBack(MethodWrapperRequest{.Type = &type,
            .Name = name,
            .Sig = sig,
            .Flags = flags});
        return requests.Last();
    }

    FieldOffsetRequest& RequestFieldOffset(System::Type& type, const char* name)
    {
        auto& requests = GetFieldOffsetRequests();
        requests.PushBack(FieldOffsetRequest{.Type = &type,
            .Name = name});
        return requests.Last();
    }

    StaticFieldRequest& RequestStaticField(System::Type& type, const char* name)
    {
        auto& requests = GetStaticFieldRequests();
        requests.PushBack(StaticFieldRequest{.Type = &type,
            .Name = name});
        return requests.Last();
    }

    EnumRequest& RequestEnum(System::Type& type, const char* name)
    {
        auto& requests = GetEnumRequests();
        requests.PushBack(EnumRequest{.Type = &type,
            .Name = name});
        return requests.Last();
    }

    void* Internal::UvmMethodGetThunk(void* uvmMethod)
    {
        return uvm_method_get_unmanaged_thunk((UvmMethod*)uvmMethod);
    }

    void* Internal::UvmMethodGetAddress(void* uvmMethod)
    {
        return uvm_method_get_pointer((UvmMethod*)uvmMethod);
    }

    void* Internal::Invoke(void* method, void* __this, void** args, void** ex)
    {
        return uvm_runtime_invoke((UvmMethod*)method, __this, args, ex);
    }

    void Internal::GCSetField(const System::Object& __this, const System::Object* field, const System::Object& value)
    {
        uvm_gc_wbarrier_set_field(std::bit_cast<const UvmObject*>(__this), std::bit_cast<UvmObject**>(field), std::bit_cast<UvmObject*>(value));
    }

    size_t Internal::CreateBoxPool(::System::Type type)
    {
        g_BoxesPool.Reserve(HAX_UNITY_BOX_POOL_SIZE * 25);
        size_t poolOffset = g_BoxesPool.Size();

        UvmClass* klass = uvm_class_from_system_type(std::bit_cast<UvmReflectionType*>(type));

        for (size_t i = 0; i < HAX_UNITY_BOX_POOL_SIZE; ++i)
        {
            UvmObject* object = uvm_object_new(klass);
            uvm_gchandle_new(object, true);
            g_BoxesPool.PushBack(object);
        }

        return poolOffset;
    }

    void* Internal::GetBoxedObject(size_t index)
    {
        return g_BoxesPool[index];
    }

}
