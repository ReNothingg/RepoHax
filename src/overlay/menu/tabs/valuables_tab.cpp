#include <src/pch.h>

#include <src/cheat.h>

#include "../localization.h"
#include "../widgets.h"

#define LINE_ID (HAX_LINE * 789)

namespace Cheat
{
    void DrawValuablesTab()
    {
        const Hax::Vector2 mainAreaSize = Hax::Gui::GetContentRegionAvail();
        const float spacing = 20_px;
        const Hax::Vector2 columnSize = {(mainAreaSize.X - 3 * spacing) / 2, mainAreaSize.Y};

        // Column 1
        Hax::Gui::Space(spacing);
        Hax::Gui::BeginContainer(Hax::Hash("ValuablesColumnLeft"), {.W = columnSize.X, .FitY = true});
        Hax::Gui::BeginVertical(spacing);
        Hax::Gui::Dummy({0.f, 0.f});
        {
            BeginPanel(LINE_ID);
            PanelHeader(G->Loc[LocKey_VISUALS]);
            {
                {
                    ToggleEx(LINE_ID, G->ValuablesEsp, G->Loc[LocKey_DisplayThroughWalls], G->Loc[LocKey_WithinSelectedDistance]);
                }

                HorizontalLine(1_px);

                {
                    Hax::WStringBuilder<16> sb;
                    sb.AppendF(L"%dm", G->ValuablesEspRange);
                    SliderEx(LINE_ID, G->Loc[LocKey_DisplayDistance], sb.View(), &G->ValuablesEspRange, 5, 500, SliderConvertInt);
                }

                HorizontalLine(1_px);

                {
                    ToggleEx(LINE_ID, G->ValuablesChams, G->Loc[LocKey_XrayHighlight], G->Loc[LocKey_NotAllHighlighted]);
                }
            }
            EndPanel();

            BeginPanel(LINE_ID);
            PanelHeader(G->Loc[LocKey_VALUE]);
            {
                {
                    ToggleEx(LINE_ID, G->Unbreakable, G->Loc[LocKey_NoDamageValuables], G->Loc[LocKey_AvailableIfHost], {.Disabled = G->IsClient});
                }

                HorizontalLine(1_px);

                {
                    MainLabel(G->Loc[LocKey_ChangeValue]);
                    Hax::Gui::BeginHorizontal(5_px);
                    {
                        const float w = CalcWidgetEqWidth(2);
                        if (Button(LINE_ID, G->Loc[LocKey_ToZero], G->Loc[LocKey_HostOnly], {.Enabled = G->IsInGame && !G->IsClient, .MinW = w}))
                            G->SetToZero = true;
                        if (Button(LINE_ID, G->Loc[LocKey_ToMax], G->Loc[LocKey_HostOnly], {.Enabled = G->IsInGame  && !G->IsClient, .MinW = w}))
                            G->SetToMax = true;
                    }
                    Hax::Gui::EndHorizontal();
                }
            }
            EndPanel();
        }
        Hax::Gui::Dummy({0.f, 0.f});
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();

        // Column 2
        Hax::Gui::Space(spacing);
        Hax::Gui::BeginContainer(Hax::Hash("ValuablesColumnRight"), {.W = columnSize.X, .FitY = true});
        Hax::Gui::BeginVertical(spacing);
        Hax::Gui::Dummy({0.f, 0.f});
        {
            BeginPanel(LINE_ID);
            PanelHeader(G->Loc[LocKey_EXTRACTION]);
            {
                {
                    ToggleEx(LINE_ID, G->ExtrPointsEsp, G->Loc[LocKey_DisplayThroughWalls]);
                }

                HorizontalLine(1_px);

                RoundDirector dir = RoundDirector::instance();
                bool disabled = !G->IsInGame || !dir || dir.extractionPointActive();
                if (Button(LINE_ID, G->Loc[LocKey_ActivateNext], {}, {.Enabled = !disabled, .MinW = Hax::Gui::GetContentRegionAvail().X}))
                    G->ActivateNextPoint = true;
            }
            EndPanel();
        }
        Hax::Gui::Dummy({0.f, 0.f});
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();
    }

}
