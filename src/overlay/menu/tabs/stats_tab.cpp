#include <src/pch.h>

#include <src/cheat.h>

#include "../localization.h"
#include "../widgets.h"
#include "../theme.h"

#define LINE_ID (HAX_LINE * 678)

namespace Cheat
{
    static bool IsPlayerAlive()
    {
        PlayerAvatar avatar = PlayerAvatar::instance();
        if (!avatar)
            return false;

        return !avatar.deadSet() && !avatar.isDisabled();
    }

    static bool IsPlayerFullHp()
    {
        if (!IsPlayerAlive())
            return true;

        PlayerHealth health = PlayerAvatar::instance().playerHealth();
        return health.health() >= health.maxHealth();
    }

    static void SpeedSlider(size_t id, LocKey label, int* value)
    {
        Hax::char16 buf[16]{};
        if (*value == 1)
            swprintf_s(buf, _countof(buf), L"%.*ls", (int)G->Loc[LocKey_Default].Size(), G->Loc[LocKey_Default].Data());
        else
            swprintf_s(buf, _countof(buf), L"×%d", *value);
        SliderEx(id, G->Loc[label], buf, value, 1, 5, SliderConvertInt);
    }

    static void DrawHealthCard()
    {
        PlayerAvatar avatar = PlayerAvatar::instance();
        PlayerHealth health = avatar ? avatar.playerHealth() : nullptr;
        const bool canHeal = G->IsInGame && avatar && health && health.health() < health.maxHealth();

        BeginPanel(LINE_ID);
        if (PanelHeader(G->Loc[LocKey_HEALTH], {}, {.Icon = Icon::Heart, .Action = G->Loc[LocKey_HealShort], .ActionEnabled = canHeal}))
            G->HealToMax = true;
        {
            ToggleEx(LINE_ID, G->Godmode, G->Loc[LocKey_Godmode], G->Loc[LocKey_GodmodeHint], {.Disabled = !G->IsInGame});
            HorizontalLine(1_px);

            // Health packs lying around the level, applied to the local player.
            MainLabel(G->Loc[LocKey_AidKitsHint], Theme::Muted);
            Hax::Gui::BeginVertical(6_px);
            Hax::Gui::BeginHorizontal(6_px);
            {
                size_t shown = 0;
                const Hax::Vector<ItemHealthPack>& aids = G->AidsPool.GetFront();
                for (size_t i = 0; i < aids.Size(); ++i)
                {
                    ItemHealthPack aid = aids[i];
                    if (!aid || !aid.itemAttributes() || !aid.itemToggle())
                        continue;

                    AidData& data = G->AidsData[(int)AidType::N];
                    Hax::WStringView name = aid.itemAttributes().itemAssetName().ToHaxView();
                    for (size_t j = 0; j < (size_t)AidType::N; ++j)
                    {
                        if (name.EndsWith(G->AidsData[j].InternalNameEnd))
                        {
                            data = G->AidsData[j];
                            break;
                        }
                    }

                    const bool disabled = aid.itemToggle().toggleState() == true || IsPlayerFullHp();
                    if (AutoLayoutButton(LINE_ID + i * 5542, G->Loc[data.LocKey], {.Disabled = disabled, .Icon = data.Icon}))
                        G->AidToToggle = aid;
                    ++shown;
                }
                if (shown == 0)
                    HintLabel(G->Loc[LocKey_NoneAvailable]);
            }
            Hax::Gui::EndHorizontal();
            Hax::Gui::EndVertical();
        }
        EndPanel();
    }

    static void DrawMovementCard()
    {
        BeginPanel(LINE_ID);
        PanelHeader(G->Loc[LocKey_MOVEMENT], {}, {.Icon = Icon::Move});
        {
            ToggleEx(LINE_ID, G->InfStamina, G->Loc[LocKey_InfiniteStamina], {}, {.Disabled = !G->IsInGame});
            HorizontalLine(1_px);
            ToggleEx(LINE_ID, G->InfJumps, G->Loc[LocKey_InfiniteJumps]);
            HorizontalLine(1_px);
            SpeedSlider(LINE_ID, LocKey_WalkingSpeed, &G->Accel.Walking);
            HorizontalLine(1_px);
            SpeedSlider(LINE_ID, LocKey_RunningSpeed, &G->Accel.Sprinting);
            HorizontalLine(1_px);
            SpeedSlider(LINE_ID, LocKey_CrouchingSpeed, &G->Accel.Crouching);
        }
        EndPanel();
    }

    static void DrawNoclipCard()
    {
        PlayerAvatar avatar = PlayerAvatar::instance();
        const bool playerReady = G->IsInGame && avatar && !avatar.deadSet() && !avatar.isDisabled();

        BeginPanel(LINE_ID);
        PanelHeader(G->Loc[LocKey_NOCLIP], G->Loc[LocKey_FlightControls],
            {.Icon = Icon::ArrowUp, .Toggle = &G->FlightEnabled, .ToggleDisabled = !G->IsInGame,
             .Hotkey = &G->VkToggleFlight, .HotkeyId = Hax::Hash(L"ToggleFlightHotkeyEditor")});
        {
            Hax::char16 buf[16]{};
            swprintf_s(buf, _countof(buf), L"%d", G->FlightSpeed);
            SliderEx(LINE_ID, G->Loc[LocKey_FlightSpeed], buf, &G->FlightSpeed, 1, 30, SliderConvertInt);
            HorizontalLine(1_px);
            swprintf_s(buf, _countof(buf), L"×%d", G->FlightSprintBoost);
            SliderEx(LINE_ID, G->Loc[LocKey_SprintBoost], buf, &G->FlightSprintBoost, 1, 6, SliderConvertInt);
            HorizontalLine(1_px);
            ToggleEx(LINE_ID, G->NoTumble, G->Loc[LocKey_DontTumble], G->Loc[LocKey_DontTumbleHint]);
            HorizontalLine(1_px);
            if (ActionRow(LINE_ID, G->Loc[LocKey_PlayerToCamera], {}, G->Loc[LocKey_MoveShort],
                {.Enabled = playerReady, .Hotkey = &G->VkTeleportPlayerToCamera, .HotkeyId = Hax::Hash(L"PlayerToCameraHotkeyEditor")}))
                G->TeleportAction = TeleportQuickAction::PlayerToCamera;
        }
        EndPanel();
    }

    static void DrawGrabCard()
    {
        BeginPanel(LINE_ID);
        PanelHeader(G->Loc[LocKey_GRAB], {}, {.Icon = Icon::Hand});
        {
            ToggleEx(LINE_ID, G->EasyGrab, G->Loc[LocKey_HighStrength], {}, {.Disabled = G->IsClient, .HostOnly = true});
            HorizontalLine(1_px);
            ToggleEx(LINE_ID, G->UnlimGrabRange, G->Loc[LocKey_UnlimitedRange]);
            HorizontalLine(1_px);
            ToggleEx(LINE_ID, G->NoOvercharge, G->Loc[LocKey_NoOvercharge]);
        }
        EndPanel();
    }

    static void DrawLasersCard()
    {
        PlayerAvatar avatar = PlayerAvatar::instance();
        const Hax::WStringView laserDescription = G->IsClient
            ? G->Loc[G->EyeLaserHostConnected ? LocKey_EyeLaserHostConnected : LocKey_EyeLaserHostRequired]
            : G->Loc[LocKey_EyeLasersDesc];

        BeginPanel(LINE_ID);
        PanelHeader(G->Loc[LocKey_EYELASERS], laserDescription,
            {.Icon = Icon::Zap, .Toggle = &G->EyeLasersEnabled, .ToggleDisabled = !G->IsInGame || !avatar,
             .Hotkey = &G->VkToggleEyeLasers, .HotkeyId = Hax::Hash(L"ToggleEyeLasersHotkeyEditor")});
        {
            Hax::char16 buf[16]{};
            swprintf_s(buf, _countof(buf), G->Loc[LocKey_Meters].Data(), G->EyeLaserRange);
            SliderEx(LINE_ID, G->Loc[LocKey_EyeLaserRange], buf, &G->EyeLaserRange, 10, 200, SliderConvertInt);
            HorizontalLine(1_px);
            swprintf_s(buf, _countof(buf), L"%d", G->EyeLaserDamage);
            SliderEx(LINE_ID, G->Loc[LocKey_EyeLaserDamage], buf, &G->EyeLaserDamage, 10, 2000, SliderConvertInt);
            HorizontalLine(1_px);
            ToggleEx(LINE_ID, G->EyeLaserInstantKill, G->Loc[LocKey_EyeLaserInstantKill]);
            HorizontalLine(1_px);
            ToggleEx(LINE_ID, G->EyeLaserDestroyObjects, G->Loc[LocKey_EyeLaserDestroyObjects], {},
                {.Disabled = G->IsClient && !G->EyeLaserHostConnected, .HostOnly = true});
            HorizontalLine(1_px);
            ToggleEx(LINE_ID, G->EyeLaserHostRelayEnabled, G->Loc[LocKey_EyeLaserHostRelay], G->Loc[LocKey_EyeLaserHostRelayDesc],
                {.Disabled = G->IsClient, .HostOnly = true});
        }
        EndPanel();
    }

    static void DrawPowerupsCard()
    {
        constexpr size_t settingsPopupId = Hax::Hash("PowerupPopup");

        BeginPanel(LINE_ID);
        PanelHeader(G->Loc[LocKey_POWERUPS], {}, {.Icon = Icon::Sparkle});
        {
            ToggleEx(LINE_ID, G->AutoUseUpgrs, G->Loc[LocKey_AutoApplyUpgrades], {}, {.SettingsId = settingsPopupId});

            if (BeginModal(settingsPopupId))
            {
                const float pad = 14_px;
                Hax::Gui::BeginHorizontal(18_px);
                {
                    const size_t half = ((size_t)UpgradeType::N + 1) / 2;
                    Hax::Gui::Dummy({0.f, 0.f});
                    for (int col = 0; col < 2; ++col)
                    {
                        Hax::Gui::BeginVertical(8_px);
                        Hax::Gui::Dummy({0.f, pad - 8_px});
                        const size_t from = col == 0 ? 0 : half;
                        const size_t to = col == 0 ? half : (size_t)UpgradeType::N;
                        for (size_t i = from; i < to; ++i)
                        {
                            auto& upgrInfo = G->UpgradesData[i];
                            CheckboxEx((size_t)&upgrInfo, upgrInfo.AutoUse, G->Loc[upgrInfo.LocKey]);
                        }
                        Hax::Gui::Dummy({0.f, pad - 8_px});
                        Hax::Gui::EndVertical();
                    }
                    Hax::Gui::Dummy({0.f, 0.f});
                }
                Hax::Gui::EndHorizontal();
                EndModal();
            }

            HorizontalLine(1_px);
            MainLabel(G->Loc[LocKey_ManualApplication], Theme::Muted);

            Hax::Gui::BeginVertical(6_px);
            Hax::Gui::BeginHorizontal(6_px);
            {
                size_t shown = 0;
                const Hax::Vector<ItemUpgrade>& upgrades = G->UpgradesPool.GetFront();
                for (size_t i = 0; i < upgrades.Size(); ++i)
                {
                    ItemUpgrade item = upgrades[i];
                    if (!item || !item.itemAttributes() || !item.itemToggle())
                        continue;

                    UpgradeData& data = G->UpgradesData[(int)UpgradeType::N];
                    Hax::WStringView name = item.itemAttributes().itemAssetName().ToHaxView();
                    for (size_t j = 0; j < (size_t)UpgradeType::N; ++j)
                    {
                        if (name.EndsWith(G->UpgradesData[j].InternalNameEnd))
                        {
                            data = G->UpgradesData[j];
                            break;
                        }
                    }

                    const bool disabled = item.itemToggle().toggleState() == true;
                    if (AutoLayoutButton(LINE_ID + i * 5542, G->Loc[data.LocKey], {.Disabled = disabled, .Icon = data.Icon}))
                        G->UpgradeToToggle = item;
                    ++shown;
                }
                if (shown == 0)
                    HintLabel(G->Loc[LocKey_NoneAvailable]);
            }
            Hax::Gui::EndHorizontal();
            Hax::Gui::EndVertical();
        }
        EndPanel();
    }

    static void DrawUpgradesCard()
    {
        static constexpr LocKey labels[(int)PlayerUpgradeType::N] =
        {
            LocKey_Health,
            LocKey_Stamina,
            LocKey_ExtraJump,
            LocKey_PlayersCount,
            LocKey_Launch,
            LocKey_Climb,
            LocKey_HeadBattery,
            LocKey_Wings,
            LocKey_SprintSpeed,
            LocKey_CrouchRest,
            LocKey_Strength,
            LocKey_ThrowForce,
            LocKey_Range
        };

        const bool ready = G->IsInGame && G->PlayerUpgradeToChange == PlayerUpgradeType::N;

        BeginPanel(LINE_ID);
        PanelHeader(G->Loc[LocKey_PlayerUpgrades], {}, {.Icon = Icon::ChevronsUp, .Note = G->Loc[LocKey_UpgradesLocal], .NoDivider = true});
        {
            constexpr int perRow = 4;
            const float gap = 8_px;
            const float cellW = Hax::Floor((Hax::Gui::GetContentRegionAvail().X - gap * (perRow - 1)) / perRow);

            Hax::Gui::BeginVertical(gap);
            for (int i = 0; i < (int)PlayerUpgradeType::N; i += perRow)
            {
                Hax::Gui::BeginHorizontal(gap);
                for (int j = i; j < Hax::Min((int)PlayerUpgradeType::N, i + perRow); ++j)
                {
                    const int level = G->PlayerUpgradeLevels[j];
                    const int delta = UpgradeCell(LINE_ID + (size_t)j * 10, G->Loc[labels[j]], level, ready, cellW);
                    if (delta != 0 && ready && level + delta >= 0)
                    {
                        G->PlayerUpgradeToChange = (PlayerUpgradeType)j;
                        G->PlayerUpgradeDelta = delta;
                    }
                }
                Hax::Gui::EndHorizontal();
            }
            Hax::Gui::EndVertical();
        }
        EndPanel();
    }

    void DrawStatsTab()
    {
        BeginColumns(Hax::Hash("StatsColumns"), 3);
        {
            DrawHealthCard();
            DrawMovementCard();
        }
        NextColumn();
        {
            DrawNoclipCard();
            DrawGrabCard();
        }
        NextColumn();
        {
            DrawLasersCard();
            DrawPowerupsCard();
        }
        EndColumns();

        DrawUpgradesCard();
    }
}
