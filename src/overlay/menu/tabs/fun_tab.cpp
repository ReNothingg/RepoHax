#include <src/pch.h>

#include <src/cheat.h>

#include "../localization.h"
#include "../theme.h"
#include "../widgets.h"
#include "tabs.h"

#define LINE_ID (HAX_LINE * 2741)

namespace Cheat
{
    static void DrawOrbitalPanel(bool host)
    {
        Hax::char16 note[32]{};
        if (G->OrbitalsActive > 0)
            swprintf_s(note, L"%ls: %d", G->Loc[LocKey_DronesInFlight].Data(), G->OrbitalsActive);

        BeginPanel(LINE_ID);
        PanelHeader(G->Loc[LocKey_ORBITAL], G->Loc[LocKey_OrbitalDesc], {.Icon = Icon::Target, .HostOnly = true, .Note = note});
        {
            Hax::char16 buf[16]{};
            swprintf_s(buf, L"%d", G->OrbitalPower);
            SliderEx(LINE_ID, G->Loc[LocKey_OrbitalPower], buf, &G->OrbitalPower, 3, 24, SliderConvertInt);
            HorizontalLine(1_px);

            const float gap = 8_px;
            const float btnW = (Hax::Gui::GetContentRegionAvail().X - gap) / 2.f;
            Hax::Gui::BeginHorizontal(gap);
            {
                if (Button(LINE_ID, G->Loc[LocKey_OrbitalAtCrosshair], {},
                    {.Enabled = host, .MinW = btnW, .Style = ButtonStyle::Danger, .Icon = Icon::Crosshair}))
                    G->OrbitalRequested = true;
                const bool hasTarget = G->DroneTargetPlayer || G->DroneTargetEnemy;
                if (Button(LINE_ID, G->Loc[LocKey_OrbitalAtTarget], {}, {.Enabled = host && hasTarget, .MinW = btnW}))
                    G->OrbitalAtTargetRequested = true;
            }
            Hax::Gui::EndHorizontal();

            HorizontalLine(1_px);
            HotkeyEx(Hax::Hash(L"OrbitalHotkeyEditor"), G->VkOrbitalStrike, G->Loc[LocKey_OrbitalHotkey]);
        }
        EndPanel();
    }

    static void DrawBlackHolePanel(bool host)
    {
        BeginPanel(LINE_ID);
        PanelHeader(G->Loc[LocKey_BLACKHOLE], G->Loc[LocKey_BlackHoleDesc], {.Icon = Icon::Globe, .HostOnly = true});
        {
            Hax::char16 buf[16]{};
            swprintf_s(buf, G->Loc[LocKey_Meters].Data(), G->BlackHoleRadius);
            SliderEx(LINE_ID, G->Loc[LocKey_BlackHoleRadius], buf, &G->BlackHoleRadius, 4, 40, SliderConvertInt);
            HorizontalLine(1_px);
            swprintf_s(buf, G->Loc[LocKey_SecondsFmt].Data(), G->BlackHoleDuration);
            SliderEx(LINE_ID, G->Loc[LocKey_BlackHoleDuration], buf, &G->BlackHoleDuration, 2, 20, SliderConvertInt);
            HorizontalLine(1_px);
            swprintf_s(buf, L"%d", G->BlackHoleStrength);
            SliderEx(LINE_ID, G->Loc[LocKey_BlackHoleStrength], buf, &G->BlackHoleStrength, 3, 40, SliderConvertInt);
            HorizontalLine(1_px);
            ToggleEx(LINE_ID, G->BlackHoleIncludeSelf, G->Loc[LocKey_BlackHoleIncludeSelf]);
            HorizontalLine(1_px);

            const float gap = 8_px;
            const float btnW = (Hax::Gui::GetContentRegionAvail().X - gap) / 2.f;
            Hax::Gui::BeginHorizontal(gap);
            {
                if (Button(LINE_ID, G->Loc[LocKey_BlackHoleOpen], {},
                    {.Enabled = host, .MinW = btnW, .Style = ButtonStyle::Danger, .Icon = Icon::Crosshair}))
                    G->BlackHoleRequested = true;
                if (Button(LINE_ID, G->Loc[LocKey_BlackHoleCollapse], {}, {.Enabled = host && G->BlackHoleActive, .MinW = btnW}))
                    G->BlackHoleStopRequested = true;
            }
            Hax::Gui::EndHorizontal();

            HorizontalLine(1_px);
            HotkeyEx(Hax::Hash(L"BlackHoleHotkeyEditor"), G->VkBlackHole, G->Loc[LocKey_BlackHoleHotkey]);
        }
        EndPanel();
    }

    static void DrawChaosPanel(bool host)
    {
        Hax::char16 note[48]{};
        if (G->ChaosEnabled && G->ChaosNextIn > 0)
            swprintf_s(note, G->Loc[LocKey_ChaosNextFmt].Data(), G->ChaosNextIn);

        BeginPanel(LINE_ID);
        PanelHeader(G->Loc[LocKey_CHAOS], G->Loc[LocKey_ChaosDesc],
            {.Icon = Icon::Sparkle, .HostOnly = true, .Toggle = &G->ChaosEnabled, .ToggleDisabled = !host, .Note = note});
        {
            Hax::char16 buf[16]{};
            swprintf_s(buf, G->Loc[LocKey_SecondsFmt].Data(), G->ChaosInterval);
            SliderEx(LINE_ID, G->Loc[LocKey_ChaosInterval], buf, &G->ChaosInterval, 5, 120, SliderConvertInt);
            HorizontalLine(1_px);
            ToggleEx(LINE_ID, G->ChaosSpareMe, G->Loc[LocKey_ChaosSpareMe]);
            HorizontalLine(1_px);
            if (Button(LINE_ID, G->Loc[LocKey_ChaosNow], {},
                {.Enabled = host && G->ChaosEnabled, .MinW = Hax::Gui::GetContentRegionAvail().X, .Icon = Icon::Zap}))
                G->ChaosTriggerNow = true;
        }
        EndPanel();
    }

    void DrawFunTab()
    {
        const bool host = G->IsInGame && !G->IsClient;

        static constexpr float weights[] = {1.f, 1.f};
        BeginColumns(Hax::Hash("FunColumns"), 2, weights);
        {
            DrawDroneStrikePanel();
        }
        NextColumn();
        {
            DrawOrbitalPanel(host);
            DrawBlackHolePanel(host);
            DrawChaosPanel(host);
        }
        EndColumns();
    }
}
