#include <src/pch.h>

#include <src/cheat.h>

#include "../localization.h"
#include "../widgets.h"
#include "../theme.h"

#define LINE_ID (HAX_LINE * 567)

namespace Cheat
{
    static void HotkeyLine(size_t id, int& key, LocKey label)
    {
        if (HotkeyEx(id, key, G->Loc[label]))
            Hax::IniSave(G->Config);
    }

    void DrawSettingsTab()
    {
        BeginColumns(Hax::Hash("SettingsColumns"), 3);
        {
            BeginPanel(LINE_ID);
            PanelHeader(G->Loc[LocKey_INTERFACE], {}, {.Icon = Icon::Sliders});
            {
                const Hax::WStringView languages[] = {L"English", L"Русский"};
                const int picked = SegmentedRow(LINE_ID, G->Loc[LocKey_Language], languages, 2, G->Language == Lang_Ru ? 1 : 0);
                if (picked == 0) { G->Loc = g_LocDict[Lang_Eng]; G->Language = Lang_Eng; }
                if (picked == 1) { G->Loc = g_LocDict[Lang_Ru]; G->Language = Lang_Ru; }
                HorizontalLine(1_px);
                ToggleEx(LINE_ID, G->DarkenBackground, G->Loc[LocKey_DarkenBackground]);
            }
            EndPanel();
        }
        NextColumn();
        {
            BeginPanel(LINE_ID);
            PanelHeader(G->Loc[LocKey_HOTKEYS], {}, {.Icon = Icon::Keyboard});
            {
                HotkeyLine(Hax::Hash(L"MenuHotkeyEditor"), G->VkOpenClose, LocKey_OpenCloseMenu);
                HorizontalLine(1_px);
                HotkeyLine(Hax::Hash(L"ToggleFlightHotkeyEditorSettings"), G->VkToggleFlight, LocKey_ToggleFlight);
                HorizontalLine(1_px);
                HotkeyLine(Hax::Hash(L"ToggleEyeLasersHotkeyEditorSettings"), G->VkToggleEyeLasers, LocKey_ToggleEyeLasers);
                HorizontalLine(1_px);
                HotkeyLine(Hax::Hash(L"PlayerToCameraHotkeyEditorSettings"), G->VkTeleportPlayerToCamera, LocKey_PlayerToCamera);
                HorizontalLine(1_px);
                HotkeyLine(Hax::Hash(L"ReviveSelfHotkeyEditorSettings"), G->VkReviveSelf, LocKey_ReviveSelf);
                HorizontalLine(1_px);
                HotkeyLine(Hax::Hash(L"SpawnItemHotkeyEditorSettings"), G->VkSpawnItem, LocKey_Spawn);
                HorizontalLine(1_px);
                HotkeyLine(Hax::Hash(L"DeleteObjectHotkeyEditorSettings"), G->VkDeleteObject, LocKey_ObjectRemover);
                HorizontalLine(1_px);
                HotkeyLine(Hax::Hash(L"LockTargetHotkeyEditorSettings"), G->VkLockGodTarget, LocKey_LockTarget);
            }
            EndPanel();
        }
        NextColumn();
        EndColumns();
    }
}
