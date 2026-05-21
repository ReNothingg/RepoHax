#include <src/pch.h>

#include <src/cheat.h>

#include "../localization.h"
#include "../widgets.h"

#define LINE_ID (HAX_LINE * 567)

namespace Cheat
{
    void DrawSettingsTab()
    {
        const Hax::Vector2 mainAreaSize = Hax::Gui::GetContentRegionAvail();
        const float spacing = 20_px;
        const Hax::Vector2 columnSize = {(mainAreaSize.X - 3 * spacing) / 2, mainAreaSize.Y};

        // Column 1
        Hax::Gui::Space(spacing);
        Hax::Gui::BeginContainer(0, {.W = columnSize.X, .H = columnSize.Y});
        Hax::Gui::BeginVertical(spacing);
        Hax::Gui::Dummy({0.f, 0.f});
        {
            BeginPanel(HAX_LINE);
            PanelHeader(G->Loc[LocKey_SETTINGS]);
            {
                {
                    MainLabel(G->Loc[LocKey_Language]);

                    constexpr size_t dropListId = Hax::Hash(L"LanguageSelect");
                    static Hax::WStringView s_Languages[] = { L"English", L"Русский" };
                    const float w = Hax::Gui::GetContentRegionAvail().X;
                    Hax::Gui::BeginVertical(3_px);
                    if (DropdownBtn(HAX_LINE, s_Languages[G->Language], w))
                        OpenPopup(dropListId, Hax::Gui::GetCursorPos());
                    Hax::Gui::EndVertical();

                    const float selectableH = CalcButtonHeight();
                    const float dropListH = selectableH * Hax::Max(1ULL, Hax::Min(_countof(s_Languages), 5ULL)) + 5_px * 2.f + 1.f;
                    if (BeginDropList(dropListId, {w, dropListH}))
                    {
                        if (Selectable(HAX_LINE, s_Languages[0], G->Language == Lang_Eng, {.MinW = w})) { G->Loc = g_LocDict[Lang_Eng]; G->Language = Lang_Eng; }
                        if (Selectable(HAX_LINE, s_Languages[1], G->Language == Lang_Ru, {.MinW = w})) { G->Loc = g_LocDict[Lang_Ru]; G->Language = Lang_Ru; }
                        EndDropList();
                    }
                }

                HorizontalLine(1_px);

                {
                    HotkeyEx(HAX_LINE, G->VkOpenClose, G->Loc[LocKey_Hotkey], G->Loc[LocKey_OpenCloseMenu]);
                }

                HorizontalLine(1_px);

                {
                    ToggleEx(HAX_LINE, G->DarkenBackground, G->Loc[LocKey_DarkenBackground]);
                }
            }
            EndPanel();
        }
        Hax::Gui::Dummy({0.f, 0.f});
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();
    }
}