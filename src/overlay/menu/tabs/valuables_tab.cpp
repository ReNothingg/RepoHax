#include <src/pch.h>

#include <src/cheat.h>

#include "../localization.h"
#include "../widgets.h"
#include "../theme.h"
#include "tabs.h"

#define LINE_ID (HAX_LINE * 789)

namespace Cheat
{
    static void MoneyDisplay(Hax::WStringView amount)
    {
        const Hax::Vector2 size = {Hax::Gui::GetContentRegionAvail().X, 52_px};
        const Hax::Rect bounds = Hax::Rect::FromPosSize(Hax::Gui::GetCursorPos(), size);
        Hax::Gui::PlaceItem(size);
        if (!Hax::Gui::IsItemVisible(bounds))
            return;

        Hax::Gui::DrawRect(bounds.Min, bounds.Max,
            {.BorderColor = Theme::Line, .BorderTh = 1_px, .FillColor = Theme::Surface2, .Rounding = 10_px});

        const float amountH = Hax::Gui::Scale(24.f);
        const Hax::Vector2 amountSize = Hax::Gui::CalcTextSize(G->NunitoSans_ExtraBold, amount, amountH);
        Hax::Gui::DrawString(G->NunitoSans_ExtraBold, amount,
            {bounds.Min.X + 16_px, bounds.GetCenter().Y - amountSize.Y / 2.f}, amountH, {.Color = Theme::Text});
    }

    static void DrawEconomyTiles()
    {
        int valuables = 0;
        if (ValuableDirector dir = ValuableDirector::instance(); dir && dir.valuableList() != null)
            valuables = dir.valuableList().Count();

        Hax::char16 balance[32]{}, quota[32]{}, count[16]{};
        swprintf_s(balance, L"$%dK", G->CurrentRunCurrency);
        swprintf_s(quota, L"$%dK", G->CurrentQuota);
        swprintf_s(count, L"%d", valuables);

        BeginColumns(Hax::Hash("EconomyTiles"), 3);
        StatTile(G->Loc[LocKey_RunBalance], G->IsInGame ? Hax::WStringView(balance) : Hax::WStringView(L"—"), G->Loc[LocKey_BalanceSyncHint]);
        NextColumn();
        StatTile(G->Loc[LocKey_QuotaTile], G->IsInGame ? Hax::WStringView(quota) : Hax::WStringView(L"—"), G->Loc[LocKey_QuotaTileHint], -1.f, Theme::Accent);
        NextColumn();
        StatTile(G->Loc[LocKey_ValuablesOnMap], G->IsInGame ? Hax::WStringView(count) : Hax::WStringView(L"—"), G->Loc[LocKey_OnMapHint]);
        EndColumns();
    }

    static void DrawQuotaCard()
    {
        BeginPanel(LINE_ID);
        PanelHeader(G->Loc[LocKey_QUOTA], G->Loc[LocKey_QuotaDesc], {.Icon = Icon::Target, .HostOnly = true});
        {
            Hax::char16 current[32]{}, target[32]{};
            swprintf_s(current, _countof(current), L"$%dK", G->CurrentQuota);
            swprintf_s(target, _countof(target), L"$%dK", G->QuotaTarget);
            InfoRow(G->Loc[LocKey_CurrentQuota], G->IsInGame ? Hax::WStringView(current) : Hax::WStringView(L"—"),
                G->IsInGame ? Theme::Accent : Theme::Faint);
            HorizontalLine(1_px);
            SliderEx(LINE_ID, G->Loc[LocKey_QuotaTarget], target, &G->QuotaTarget, 0, 1000000, SliderConvertInt);
            const bool canApply = G->IsInGame && !G->IsClient && RoundDirector::instance();
            if (Button(LINE_ID, G->Loc[LocKey_ApplyQuota], {}, {.Enabled = canApply, .MinW = Hax::Gui::GetContentRegionAvail().X, .Style = ButtonStyle::Primary}))
                G->QuotaApplyRequested = true;
        }
        EndPanel();
    }

    static void DrawSurplusBagCard()
    {
        BeginPanel(LINE_ID);
        PanelHeader(G->Loc[LocKey_SURPLUS_BAG], G->Loc[LocKey_SurplusBagDesc], {.Icon = Icon::Coin, .HostOnly = true});
        {
            Hax::char16 amount[32]{};
            swprintf_s(amount, _countof(amount), L"$%dK", G->SurplusBagAmount);
            MoneyDisplay(amount);
            Slider(LINE_ID, &G->SurplusBagAmount, 1, 100000, SliderConvertInt, Hax::Gui::GetContentRegionAvail().X);

            constexpr int presets[] = {1000, 5000, 10000, 25000};
            Hax::char16 labels[4][16]{};
            bool clicked[4]{};
            GridButton buttons[4];
            for (int i = 0; i < 4; ++i)
            {
                swprintf_s(labels[i], L"$%dK", presets[i]);
                buttons[i] = {.Label = labels[i], .Clicked = &clicked[i], .Style = G->SurplusBagAmount == presets[i] ? ButtonStyle::Primary : ButtonStyle::Secondary};
            }
            ButtonGrid(LINE_ID, buttons, 4, 4);
            for (int i = 0; i < 4; ++i)
                if (clicked[i])
                    G->SurplusBagAmount = presets[i];

            const bool canSpawn = G->IsInGame && !G->IsClient && AssetManager::instance() && SemiFunc::MainCamera();
            if (Button(LINE_ID, G->Loc[LocKey_SpawnSurplusBag], G->Loc[LocKey_SurplusBagHostHint],
                {.Enabled = canSpawn, .MinW = Hax::Gui::GetContentRegionAvail().X}))
                G->SurplusBagSpawnRequested = true;
        }
        EndPanel();
    }

    static void DrawValuablesCard()
    {
        const bool host = G->IsInGame && !G->IsClient;

        BeginPanel(LINE_ID);
        PanelHeader(G->Loc[LocKey_VISUALS], {}, {.Icon = Icon::Gem});
        {
            ToggleEx(LINE_ID, G->ValuablesEsp, G->Loc[LocKey_DisplayThroughWalls], G->Loc[LocKey_WithinSelectedDistance]);
            HorizontalLine(1_px);
            {
                Hax::WStringBuilder<16> sb;
                sb.AppendF(G->Loc[LocKey_Meters].Data(), G->ValuablesEspRange);
                SliderEx(LINE_ID, G->Loc[LocKey_DisplayDistance], sb.View(), &G->ValuablesEspRange, 5, 500, SliderConvertInt);
            }
            HorizontalLine(1_px);
            ToggleEx(LINE_ID, G->ValuablesChams, G->Loc[LocKey_XrayHighlight], G->Loc[LocKey_NotAllHighlighted]);
            HorizontalLine(1_px);
            ToggleEx(LINE_ID, G->Unbreakable, G->Loc[LocKey_NoDamageValuables], {}, {.Disabled = G->IsClient, .HostOnly = true});
            HorizontalLine(1_px);

            MainLabel(G->Loc[LocKey_PricesAll]);
            bool zero = false, max = false;
            const GridButton buttons[] =
            {
                {.Label = G->Loc[LocKey_ToZero], .Clicked = &zero, .Enabled = host},
                {.Label = G->Loc[LocKey_ToMax], .Clicked = &max, .Enabled = host},
            };
            ButtonGrid(LINE_ID, buttons, _countof(buttons), 2);
            if (zero) G->SetToZero = true;
            if (max) G->SetToMax = true;
        }
        EndPanel();
    }

    static void DrawExtractionCard()
    {
        RoundDirector dir = RoundDirector::instance();
        const bool disabled = !G->IsInGame || !dir || dir.extractionPointActive();

        BeginPanel(LINE_ID);
        PanelHeader(G->Loc[LocKey_EXTRACTION], {}, {.Icon = Icon::Extract});
        {
            ToggleEx(LINE_ID, G->ExtrPointsEsp, G->Loc[LocKey_DisplayThroughWalls]);
            HorizontalLine(1_px);
            if (ActionRow(LINE_ID, G->Loc[LocKey_ActivateNext], {}, G->Loc[LocKey_ActivateShort], {.Enabled = !disabled}))
                G->ActivateNextPoint = true;
        }
        EndPanel();
    }

    void DrawValuablesTab()
    {
        DrawEconomyTiles();

        BeginColumns(Hax::Hash("EconomyColumns"), 3);
        {
            DrawQuotaCard();
            DrawBalancePanel();
        }
        NextColumn();
        {
            DrawValuablesCard();
            DrawExtractionCard();
        }
        NextColumn();
        {
            DrawLootControlPanel();
            DrawSurplusBagCard();
        }
        EndColumns();
    }
}
