#include <src/pch.h>

#include <src/cheat.h>

#include "../localization.h"
#include "../widgets.h"
#include "../theme.h"

#define LINE_ID (HAX_LINE * 891)

namespace Cheat
{
    void DrawVisionTab()
    {
        BeginColumns(Hax::Hash("VisionColumns"), 3);
        {
            BeginPanel(LINE_ID);
            PanelHeader(G->Loc[LocKey_VIEW], {}, {.Icon = Icon::Eye});
            {
                ToggleEx(LINE_ID, G->BetterVision, G->Loc[LocKey_BetterVision]);
                HorizontalLine(1_px);
                ToggleEx(LINE_ID, G->NoFog, G->Loc[LocKey_NoFog], G->Loc[LocKey_NoFogDesc]);
                HorizontalLine(1_px);
                ToggleEx(LINE_ID, G->ThirdPerson, G->Loc[LocKey_ThirdPerson]);
                HorizontalLine(1_px);

                Hax::char16 buf[32] = {};
                swprintf_s(buf, _countof(buf), G->Fov < 61 ? G->Loc[LocKey_Default].Data() : G->Loc[LocKey_Degrees].Data(), G->Fov);
                SliderEx(LINE_ID, G->Loc[LocKey_FieldOfView], buf, &G->Fov, 60, 140, SliderConvertInt);
            }
            EndPanel();

            BeginPanel(LINE_ID);
            PanelHeader(G->Loc[LocKey_DisableOcclusion], G->Loc[LocKey_DisableOcclusionDesc],
                {.Icon = Icon::Box, .Toggle = &G->DisableOcclusionCulling});
            {
                Hax::char16 buf[32] = {};
                swprintf_s(buf, _countof(buf), G->Loc[LocKey_Meters].Data(), G->RenderDistance);
                SliderEx(LINE_ID, G->Loc[LocKey_RenderDistance], buf, &G->RenderDistance, 32, 500, SliderConvertInt);
            }
            EndPanel();
        }
        NextColumn();
        {
            BeginPanel(LINE_ID);
            PanelHeader(G->Loc[LocKey_FLASHLIGHT], {}, {.Icon = Icon::Sun});
            {
                int& intensity = G->Flashlight.Intensity;
                Hax::char16 buf[16] = {};
                swprintf_s(buf, _countof(buf), intensity < 11 ? G->Loc[LocKey_Default].Data() : L"×%.1f", intensity / 10.f);
                SliderEx(LINE_ID, G->Loc[LocKey_LightIntensity], buf, &intensity, 10, 100, SliderConvertInt);
                HorizontalLine(1_px);

                int& angle = G->Flashlight.Angle;
                swprintf_s(buf, _countof(buf), G->Loc[LocKey_Degrees].Data(), angle);
                SliderEx(LINE_ID, G->Loc[LocKey_IlluminationAngle], buf, &angle, 60, 120, SliderConvertInt);
                HorizontalLine(1_px);

                ToggleEx(LINE_ID, G->Flashlight.InCrouch, G->Loc[LocKey_WorksWhileCrouching]);
            }
            EndPanel();
        }
        NextColumn();
        {
            BeginPanel(LINE_ID);
            PanelHeader(G->Loc[LocKey_DEADHEAD], {}, {.Icon = Icon::Battery});
            {
                ToggleEx(LINE_ID, G->MaxHeadBattery, G->Loc[LocKey_MaxBatteryHead]);
            }
            EndPanel();

            BeginPanel(LINE_ID);
            PanelHeader(G->Loc[LocKey_SPECTATOR], G->Loc[LocKey_SpectatorDesc],
                {.Icon = Icon::Ghost, .Toggle = &G->SpectatorEnabled});
            {
                const Hax::WStringView views[SpectatorView_Count] =
                {
                    G->Loc[LocKey_SpecViewGameShort], G->Loc[LocKey_SpecViewEyesShort], G->Loc[LocKey_SpecViewFreeShort]
                };
                const int picked = SegmentedRow(LINE_ID, G->Loc[LocKey_SpectatorView], views, SpectatorView_Count,
                    G->SpectatorViewMode, G->SpectatorEnabled, true);
                if (picked >= 0)
                    G->SpectatorViewMode = picked;
                HorizontalLine(1_px);
                HotkeyEx(Hax::Hash(L"SpectatorViewHotkeyEditor"), G->VkSpectatorView, G->Loc[LocKey_SpectatorViewKey]);
                HorizontalLine(1_px);
                ToggleEx(LINE_ID, G->SpectatorSkipDeathCam, G->Loc[LocKey_SpectatorSkipDeathCam], G->Loc[LocKey_SpectatorSkipDeathCamDesc],
                    {.Disabled = !G->SpectatorEnabled});
                HorizontalLine(1_px);

                Hax::char16 buf[32] = {};
                swprintf_s(buf, _countof(buf), L"%d m/s", G->SpectatorFreeSpeed);
                SliderEx(LINE_ID, G->Loc[LocKey_SpectatorFreeSpeed], buf, &G->SpectatorFreeSpeed, 2, 40, SliderConvertInt);
            }
            EndPanel();
        }
        EndColumns();
    }
}
