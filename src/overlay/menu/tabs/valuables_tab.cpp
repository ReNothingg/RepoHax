#include <src/pch.h>

#include <src/cheat.h>

#include "../localization.h"
#include "../widgets.h"
#include "tabs.h"

#define LINE_ID (HAX_LINE * 789)

namespace Cheat
{
    static void StatusLine(Hax::WStringView name, Hax::WStringView value, Hax::Gui::Color valueColor = 0xD8D0ACFF)
    {
        Hax::Gui::BeginHorizontal();
        MainLabel(name, 0x978D67FF);
        const Hax::Vector2 valueSize = CalcMainLabelSize(value);
        Hax::Gui::Space(Hax::Max(0.f, Hax::Gui::GetContentRegionAvail().X - valueSize.X));
        MainLabel(value, valueColor);
        Hax::Gui::EndHorizontal();
    }

    static void RepoMoneyDisplay(Hax::WStringView amount)
    {
        const Hax::Vector2 size = {Hax::Gui::GetContentRegionAvail().X, 52_px};
        const Hax::Rect bounds = Hax::Rect::FromPosSize(Hax::Gui::GetCursorPos(), size);
        Hax::Gui::PlaceItem(size);
        if (!Hax::Gui::IsItemVisible(bounds))
            return;

        constexpr Hax::Gui::Color rust = 0xD88813FF;
        Hax::Gui::DrawRect(bounds.Min, bounds.Max,
            {.BorderColor = 0x725A27FF, .BorderTh = 1_px, .FillColor = 0x101108FF, .Rounding = 2_px});
        Hax::Gui::DrawRect(bounds.Min, {bounds.Min.X + 5_px, bounds.Max.Y}, {.FillColor = rust, .Rounding = Hax::Vector4(3_px, 0.f, 0.f, 3_px)});

        const float amountH = Hax::Gui::Scale(25.f);
        const Hax::Vector2 amountSize = Hax::Gui::CalcTextSize(G->NunitoSans_ExtraBold, amount, amountH);
        Hax::Gui::DrawString(G->NunitoSans_ExtraBold, amount,
            {bounds.Max.X - amountSize.X - 13_px, bounds.GetCenter().Y - amountSize.Y / 2.f}, amountH, {.Color = 0x9ED52AFF});

        // A pair of worn terminal marks gives the control the same industrial
        // readout language as the extraction machine without changing the global theme.
        Hax::Gui::DrawLine({bounds.Max.X - 27_px, bounds.Min.Y + 7_px}, {bounds.Max.X - 19_px, bounds.Min.Y + 7_px}, {.FillColor = rust, .Th = 2_px});
        Hax::Gui::DrawLine({bounds.Max.X - 15_px, bounds.Min.Y + 7_px}, {bounds.Max.X - 9_px, bounds.Min.Y + 7_px}, {.FillColor = 0x63502AFF, .Th = 2_px});
    }

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
            PanelHeader(G->Loc[LocKey_QUOTA], G->Loc[LocKey_QuotaDesc]);
            {
                Hax::char16 current[32]{};
                Hax::char16 target[32]{};
                swprintf_s(current, _countof(current), L"$%dK", G->CurrentQuota);
                swprintf_s(target, _countof(target), L"$%dK", G->QuotaTarget);
                StatusLine(G->Loc[LocKey_CurrentQuota], G->IsInGame ? Hax::WStringView(current) : Hax::WStringView(L"-"),
                    G->IsInGame ? 0xE8B93FFF : 0x978D67FF);
                HorizontalLine(1_px);
                SliderEx(LINE_ID, G->Loc[LocKey_QuotaTarget], target, &G->QuotaTarget, 0, 1000000, SliderConvertInt);
                const bool canApply = G->IsInGame && !G->IsClient && RoundDirector::instance();
                if (Button(LINE_ID, G->Loc[LocKey_ApplyQuota], G->Loc[LocKey_HostOnly],
                    {.Enabled = canApply, .MinW = Hax::Gui::GetContentRegionAvail().X}))
                    G->QuotaApplyRequested = true;
            }
            EndPanel();

            BeginPanel(LINE_ID);
            PanelHeader(G->Loc[LocKey_SURPLUS_BAG], G->Loc[LocKey_SurplusBagDesc]);
            {
                Hax::char16 amount[32]{};
                swprintf_s(amount, _countof(amount), L"$%dK", G->SurplusBagAmount);
                RepoMoneyDisplay(amount);
                Slider(LINE_ID, &G->SurplusBagAmount, 1, 100000, SliderConvertInt,
                    Hax::Gui::GetContentRegionAvail().X);

                Hax::Gui::BeginHorizontal(5_px);
                {
                    constexpr int presets[] = {1000, 5000, 10000, 25000};
                    const float presetW = CalcWidgetEqWidth(_countof(presets));
                    for (size_t i = 0; i < _countof(presets); ++i)
                    {
                        Hax::char16 presetLabel[16]{};
                        swprintf_s(presetLabel, _countof(presetLabel), L"$%dK", presets[i]);
                        if (Button(LINE_ID + i * 10000, presetLabel, {}, {.MinW = presetW}))
                            G->SurplusBagAmount = presets[i];
                    }
                }
                Hax::Gui::EndHorizontal();

                const bool canSpawn = G->IsInGame && !G->IsClient && AssetManager::instance() && SemiFunc::MainCamera();
                if (Button(LINE_ID, G->Loc[LocKey_SpawnSurplusBag], G->Loc[LocKey_SurplusBagHostHint],
                    {.Enabled = canSpawn, .MinW = Hax::Gui::GetContentRegionAvail().X}))
                    G->SurplusBagSpawnRequested = true;
            }
            EndPanel();

            DrawBalancePanel();

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

            DrawLootControlPanel();
        }
        Hax::Gui::Dummy({0.f, 0.f});
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();
    }

}
