#include "pch.h"

namespace Cheat
{
    extern void Initialize(HMODULE hDll);
}

static unsigned int WINAPI Start(void* hDll)
{
    Cheat::Initialize((HMODULE)hDll);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE handle, DWORD reason, LPVOID reserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        ::DisableThreadLibraryCalls(handle);
        
        HANDLE hThread = (HANDLE)_beginthreadex(nullptr, 0, Start, (void*)handle, 0, nullptr);
        if (hThread == nullptr)
            return FALSE;

        ::CloseHandle(hThread);
    }

    return TRUE;
}

#define SYS_PATH "\\\\.\\GLOBALROOT\\SystemRoot\\System32\\dwmapi."
#pragma comment(linker, "/export:DwmpDxGetWindowSharedSurface=" SYS_PATH "DwmpDxGetWindowSharedSurface,@100")
#pragma comment(linker, "/export:DwmpDxUpdateWindowSharedSurface=" SYS_PATH "DwmpDxUpdateWindowSharedSurface,@101")
#pragma comment(linker, "/export:DwmEnableComposition=" SYS_PATH "DwmEnableComposition,@102")
#pragma comment(linker, "/export:DllCanUnloadNow=" SYS_PATH "DllCanUnloadNow,PRIVATE")
#pragma comment(linker, "/export:DllGetClassObject=" SYS_PATH "DllGetClassObject,PRIVATE")
#pragma comment(linker, "/export:DwmAttachMilContent=" SYS_PATH "DwmAttachMilContent,@116")
#pragma comment(linker, "/export:DwmDefWindowProc=" SYS_PATH "DwmDefWindowProc,@117")
#pragma comment(linker, "/export:DwmDetachMilContent=" SYS_PATH "DwmDetachMilContent,@118")
#pragma comment(linker, "/export:DwmEnableBlurBehindWindow=" SYS_PATH "DwmEnableBlurBehindWindow,@119")
#pragma comment(linker, "/export:DwmEnableMMCSS=" SYS_PATH "DwmEnableMMCSS,@120")
#pragma comment(linker, "/export:DwmExtendFrameIntoClientArea=" SYS_PATH "DwmExtendFrameIntoClientArea,@121")
#pragma comment(linker, "/export:DwmFlush=" SYS_PATH "DwmFlush,@122")
#pragma comment(linker, "/export:DwmGetColorizationColor=" SYS_PATH "DwmGetColorizationColor,@123")
#pragma comment(linker, "/export:DwmGetCompositionTimingInfo=" SYS_PATH "DwmGetCompositionTimingInfo,@125")
#pragma comment(linker, "/export:DwmGetGraphicsStreamClient=" SYS_PATH "DwmGetGraphicsStreamClient,@126")
#pragma comment(linker, "/export:DwmpGetColorizationParameters=" SYS_PATH "DwmpGetColorizationParameters,@127")
#pragma comment(linker, "/export:DwmpDxgiIsThreadDesktopComposited=" SYS_PATH "DwmpDxgiIsThreadDesktopComposited,@128")
#pragma comment(linker, "/export:DwmGetGraphicsStreamTransformHint=" SYS_PATH "DwmGetGraphicsStreamTransformHint,@129")
#pragma comment(linker, "/export:DwmGetTransportAttributes=" SYS_PATH "DwmGetTransportAttributes,@130")
#pragma comment(linker, "/export:DwmpSetColorizationParameters=" SYS_PATH "DwmpSetColorizationParameters,@131")
#pragma comment(linker, "/export:DwmGetUnmetTabRequirements=" SYS_PATH "DwmGetUnmetTabRequirements,@133")
#pragma comment(linker, "/export:DwmGetWindowAttribute=" SYS_PATH "DwmGetWindowAttribute,@134")
#pragma comment(linker, "/export:DwmpRenderFlick=" SYS_PATH "DwmpRenderFlick,@135")
#pragma comment(linker, "/export:DwmpAllocateSecurityDescriptor=" SYS_PATH "DwmpAllocateSecurityDescriptor,@136")
#pragma comment(linker, "/export:DwmpFreeSecurityDescriptor=" SYS_PATH "DwmpFreeSecurityDescriptor,@137")
#pragma comment(linker, "/export:DwmpEnableDDASupport=" SYS_PATH "DwmpEnableDDASupport,@143")
#pragma comment(linker, "/export:DwmInvalidateIconicBitmaps=" SYS_PATH "DwmInvalidateIconicBitmaps,@149")
#pragma comment(linker, "/export:DwmTetherTextContact=" SYS_PATH "DwmTetherTextContact,@156")
#pragma comment(linker, "/export:DwmpUpdateProxyWindowForCapture=" SYS_PATH "DwmpUpdateProxyWindowForCapture,@183")
#pragma comment(linker, "/export:DwmIsCompositionEnabled=" SYS_PATH "DwmIsCompositionEnabled,@184")
#pragma comment(linker, "/export:DwmModifyPreviousDxFrameDuration=" SYS_PATH "DwmModifyPreviousDxFrameDuration,@185")
#pragma comment(linker, "/export:DwmQueryThumbnailSourceSize=" SYS_PATH "DwmQueryThumbnailSourceSize,@186")
#pragma comment(linker, "/export:DwmRegisterThumbnail=" SYS_PATH "DwmRegisterThumbnail,@187")
#pragma comment(linker, "/export:DwmRenderGesture=" SYS_PATH "DwmRenderGesture,@188")
#pragma comment(linker, "/export:DwmSetDxFrameDuration=" SYS_PATH "DwmSetDxFrameDuration,@189")
#pragma comment(linker, "/export:DwmSetIconicLivePreviewBitmap=" SYS_PATH "DwmSetIconicLivePreviewBitmap,@190")
#pragma comment(linker, "/export:DwmSetIconicThumbnail=" SYS_PATH "DwmSetIconicThumbnail,@191")
#pragma comment(linker, "/export:DwmSetPresentParameters=" SYS_PATH "DwmSetPresentParameters,@192")
#pragma comment(linker, "/export:DwmSetWindowAttribute=" SYS_PATH "DwmSetWindowAttribute,@193")
#pragma comment(linker, "/export:DwmShowContact=" SYS_PATH "DwmShowContact,@194")
#pragma comment(linker, "/export:DwmTetherContact=" SYS_PATH "DwmTetherContact,@195")
#pragma comment(linker, "/export:DwmTransitionOwnedWindow=" SYS_PATH "DwmTransitionOwnedWindow,@196")
#pragma comment(linker, "/export:DwmUnregisterThumbnail=" SYS_PATH "DwmUnregisterThumbnail,@197")
#pragma comment(linker, "/export:DwmUpdateThumbnailProperties=" SYS_PATH "DwmUpdateThumbnailProperties,@198")