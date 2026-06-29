#pragma once

#include "../hax.h"

inline constexpr size_t HAX_UNITY_BOX_POOL_SIZE = 4;
inline constexpr bool HAX_UNITY_IS_MONO = true;

namespace System
{
    class Type;
    class Object;
    class MethodInfo;
    struct Int32;
    struct IntPtr;
    struct UInt32;
    struct MethodInfoWrapper;

    enum MethodInfoWrapperFlags
    {
        Address = 1 << 0,
        Thunk = 1 << 1,
        ICall = 1 << 2,
        Each = (1 << 3) - 1
    };
}

namespace Hax::Unity
{
    void Initialize(LogFile* logFile);

    bool IsUnityProcess();
    Handle GetUvmHandle();
    Handle AttachThread();
    void DetachThread(Handle hThread);

    struct TypeRequest { const char *Assembly, *Namespace, *Name; System::Type* Out; };
    struct MethodRequest { System::Type* Type; const char *Name, *Sig; System::MethodInfo* Out; };
    struct MethodWrapperRequest { System::Type* Type; const char *Name, *Sig; int Flags; System::MethodInfoWrapper* Out; };
    struct FieldOffsetRequest { System::Type* Type; const char* Name; System::Int32* Out; };
    struct StaticFieldRequest { System::Type* Type; const char* Name; System::IntPtr* Out; };
    struct EnumRequest { System::Type* Type; const char* Name; System::UInt32* Out; };

    TypeRequest& RequestType(const char* assembly, const char* namespaze, const char* name);
    MethodRequest& RequestMethod(System::Type& type, const char* name, const char* sig = nullptr);
    MethodWrapperRequest& RequestMethodWrapper(System::Type& type, const char* name, const char* sig = nullptr, int flags = System::MethodInfoWrapperFlags::Each);
    FieldOffsetRequest& RequestFieldOffset(System::Type& type, const char* name);
    StaticFieldRequest& RequestStaticField(System::Type& type, const char* name);
    EnumRequest& RequestEnum(System::Type& type, const char* name);

    namespace Internal
    {
        void*   UvmMethodGetThunk(void* uvmMethod);
        void*   UvmMethodGetAddress(void* uvmMethod);
        void*   Invoke(void* method, void* __this, void** args, void** ex);
        void    GCSetField(const System::Object& __this, const System::Object* field, const System::Object& value);
        size_t  CreateBoxPool(System::Type type);
        void*   GetBoxedObject(size_t index);
    }
}

#ifndef HAX_UNITY_SRC

#include "modules/hax_unity_mscorlib.h"
#include "modules/hax_unity_core.h"
#include "modules/hax_unity_ai.h"
#include "modules/hax_unity_photon.h"
#include "modules/hax_unity_physics.h"
#include "modules/hax_unity_physics2d.h"

#define META(a, ns, c)\
private: inline static System::Type s_Type = Hax::Unity::RequestType(a, ns, c);\
public: static System::Type typeof() { return s_Type; }

#define STATIC_FIELD(n, t)                            \
private: static inline System::IntPtr s_ ## n = Hax::Unity::RequestStaticField(s_Type, #n);\
public: static t& n() { return *(t*)s_ ## n ## .Value; }

#define FIELD(n, t)                                   \
private: static inline System::Int32 s_ ## n = Hax::Unity::RequestFieldOffset(s_Type, #n);     \
public: t& n() const { THROW_IF_NULL(); return *(t*)((char*)m_Ptr + s_ ## n ## .Value); }

#define FIELD_EX(f, n, t)                                   \
private: static inline System::Int32 s_ ## f = Hax::Unity::RequestFieldOffset(s_Type, n); \
public: t& f() const { THROW_IF_NULL(); return *(t*)((char*)m_Ptr + s_ ## f ## .Value); }

#define STATIC_PROPERTY(n, t)                            \
private: static inline System::IntPtr s_ ## n = Hax::Unity::RequestStaticField(s_Type, "<" ## #n ## ">k__BackingField");        \
public: static t& n() { return *(t*)s_ ## n ## .Value; }

#define PROPERTY(n, t) \
private: static inline System::Int32 s_ ## n = Hax::Unity::RequestFieldOffset(s_Type, "<" ## #n ## ">k__BackingField"); \
public: t& n() const { THROW_IF_NULL(); return *(t*)((char*)m_Ptr + s_ ## n ## .Value); }

#define ENUM_VALUE(n)                                 \
private: static inline System::UInt32 s_ ## n = Hax::Unity::RequestEnum(s_Type, #n); \
public: static uint32_t n() { return s_ ## n ## .Value; }

#define METHOD(n)\
static inline System::MethodInfo s_ ## n = Hax::Unity::RequestMethod(s_Type, #n, nullptr)

#define METHOD_EX(n, sig)\
static inline System::MethodInfo s_ ## n = Hax::Unity::RequestMethod(s_Type, #n, sig)

#define METHOD_WRAPPER(n)\
static inline System::MethodInfoWrapper s_ ## n = Hax::Unity::RequestMethodWrapper(s_Type, #n, nullptr, System::MethodInfoWrapperFlags::Thunk | System::MethodInfoWrapperFlags::Address)

#define METHOD_WRAPPER_EX(n, sig)\
static inline System::MethodInfoWrapper s_ ## n = Hax::Unity::RequestMethodWrapper(s_Type, #n, sig, System::MethodInfoWrapperFlags::Thunk | System::MethodInfoWrapperFlags::Address)

#define ICALL_WRAPPER(n)\
static inline System::MethodInfoWrapper s_ ## n = Hax::Unity::RequestMethodWrapper(s_Type, #n, nullptr, System::MethodInfoWrapperFlags::ICall)

#define ICALL_WRAPPER_EX(n, sig)\
static inline System::MethodInfoWrapper s_ ## n = Hax::Unity::RequestMethodWrapper(s_Type, #n, sig, System::MethodInfoWrapperFlags::ICall)

#define COMMA ,

#endif