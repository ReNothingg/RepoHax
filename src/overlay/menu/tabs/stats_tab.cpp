#include <src/pch.h>

#include <src/cheat.h>

#include "../localization.h"
#include "../widgets.h"

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

    static void DrawPlayerUpgradeRow(size_t id, PlayerUpgradeType type, Hax::WStringView label)
    {
        int level = G->PlayerUpgradeLevels[(int)type];
        bool ready = G->IsInGame && G->PlayerUpgradeToChange == PlayerUpgradeType::N;

        const Hax::Vector2 buttonSize = CalcRepeatBtnSize(L"+");
        constexpr float valueWidth = 38.f;

        Hax::Gui::BeginHorizontal(5_px);
        {
            MainLabelAlignedByH(label, buttonSize.Y);
            const float controlsWidth = buttonSize.X * 2.f + valueWidth + 10_px;
            Hax::Gui::Space(Hax::Max(0.f, Hax::Gui::GetContentRegionAvail().X - controlsWidth));

            if (RepeatBtn(id, L"-", ready && level > 0))
            {
                G->PlayerUpgradeToChange = type;
                G->PlayerUpgradeDelta = -1;
            }

            Hax::char16 value[16]{};
            swprintf_s(value, _countof(value), L"%d", level);
            MainLabelAlignedByH(value, buttonSize.Y);

            if (RepeatBtn(id + 1, L"+", ready))
            {
                G->PlayerUpgradeToChange = type;
                G->PlayerUpgradeDelta = 1;
            }
        }
        Hax::Gui::EndHorizontal();
    }

    void DrawStatsTab()
    {
        const Hax::Vector2 mainAreaSize = Hax::Gui::GetContentRegionAvail();
        const float spacing = 20_px;
        const Hax::Vector2 columnSize = {(mainAreaSize.X - 3 * spacing) / 2, mainAreaSize.Y};

        PlayerAvatar avatar = PlayerAvatar::instance();
        PlayerHealth health = avatar ? avatar.playerHealth() : nullptr;

        // Column 1
        Hax::Gui::Space(spacing);
        Hax::Gui::BeginContainer(Hax::Hash("StatsColumnLeft"), {.W = columnSize.X, .FitY = true});
        Hax::Gui::BeginVertical(spacing);
        Hax::Gui::Dummy({0.f, 0.f});
        {
            BeginPanel(LINE_ID);
            PanelHeader(G->Loc[LocKey_HEALTH]);
            {
                ToggleEx(LINE_ID, G->Godmode, G->Loc[LocKey_Godmode]);

                HorizontalLine(1_px);

                bool disabled = !G->IsInGame || !avatar || !health || health.health() >= health.maxHealth();
                if (Button(LINE_ID, G->Loc[LocKey_HealToMax], {}, {.Enabled = !disabled, .MinW = Hax::Gui::GetContentRegionAvail().X}))
                    G->HealToMax = true;
            }
            EndPanel();

            BeginPanel(LINE_ID);
            PanelHeader(G->Loc[LocKey_MOVEMENT]);
            {
                ToggleEx(LINE_ID, G->InfStamina, G->Loc[LocKey_InfiniteStamina]);

                HorizontalLine(1_px);

                {
                    Hax::char16 buf[16]{};
                    swprintf_s(buf, _countof(buf), G->Accel.Walking == 1 ? G->Loc[LocKey_Default].Data() : L"%dx", G->Accel.Walking);

                    SliderEx(LINE_ID, G->Loc[LocKey_WalkingSpeed], buf, &G->Accel.Walking, 1, 5, SliderConvertInt);
                }

                HorizontalLine(1_px);

                {
                    Hax::char16 buf[16]{};
                    swprintf_s(buf, _countof(buf), G->Accel.Sprinting == 1 ? G->Loc[LocKey_Default].Data() : L"%dx", G->Accel.Sprinting);

                    SliderEx(LINE_ID, G->Loc[LocKey_RunningSpeed], buf, &G->Accel.Sprinting, 1, 5, SliderConvertInt);
                }

                HorizontalLine(1_px);

                {
                    Hax::char16 buf[16]{};
                    swprintf_s(buf, _countof(buf), G->Accel.Crouching == 1 ? G->Loc[LocKey_Default].Data() : L"%dx", G->Accel.Crouching);

                    SliderEx(LINE_ID, G->Loc[LocKey_CrouchingSpeed], buf, &G->Accel.Crouching, 1, 5, SliderConvertInt);
                }

                HorizontalLine(1_px);

                ToggleEx(LINE_ID, G->InfJumps, G->Loc[LocKey_InfiniteJumps]);

                HorizontalLine(1_px);

                ToggleEx(LINE_ID, G->FlightEnabled, G->Loc[LocKey_Flight], G->Loc[LocKey_FlightControls], {.Disabled = !G->IsInGame});

                {
                    Hax::char16 buf[16]{};
                    swprintf_s(buf, _countof(buf), L"%d", G->FlightSpeed);
                    SliderEx(LINE_ID, G->Loc[LocKey_FlightSpeed], buf, &G->FlightSpeed, 1, 30, SliderConvertInt);
                }

                HotkeyEx(Hax::Hash(L"ToggleFlightHotkeyEditor"), G->VkToggleFlight, G->Loc[LocKey_Hotkey], G->Loc[LocKey_ToggleFlight]);

                HorizontalLine(1_px);

                ToggleEx(LINE_ID, G->NoTumble, G->Loc[LocKey_DontTumble], G->Loc[LocKey_DisableTumbling]);
            }
            EndPanel();
        }
        Hax::Gui::Dummy({0.f, 0.f});
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();

        // Column 2
        Hax::Gui::Space(spacing);
        Hax::Gui::BeginContainer(Hax::Hash("StatsColumnRight"), {.W = columnSize.X, .FitY = true});
        Hax::Gui::BeginVertical(spacing);
        Hax::Gui::Dummy({0.f, 0.f});
        {
            BeginPanel(LINE_ID);
            PanelHeader(G->Loc[LocKey_GRAB]);
            {
                ToggleEx(LINE_ID, G->EasyGrab, G->Loc[LocKey_HighStrength], G->Loc[LocKey_AvailableIfHost], {.Disabled = G->IsClient});

                HorizontalLine(1_px);

                ToggleEx(LINE_ID, G->UnlimGrabRange, G->Loc[LocKey_UnlimitedRange]);

                HorizontalLine(1_px);

                ToggleEx(LINE_ID, G->NoOvercharge, G->Loc[LocKey_NoOvercharge]);
            }
            EndPanel();

            BeginPanel(LINE_ID);
            PanelHeader(G->Loc[LocKey_PlayerUpgrades]);
            {
                static const Hax::WStringView labels[(int)PlayerUpgradeType::N] =
                {
                    L"Health Upgrade",
                    L"Stamina Upgrade",
                    L"Extra Jump Upgrade",
                    L"Map Player Count Upgrade",
                    L"Tumble Launch Upgrade",
                    L"Tumble Climb Upgrade",
                    L"Death Head Battery Upgrade",
                    L"Tumble Wings Upgrade",
                    L"Sprint Speed Upgrade",
                    L"Crouch Rest Upgrade",
                    L"Strength Upgrade",
                    L"Throw Strength Upgrade",
                    L"Range Upgrade"
                };

                for (int i = 0; i < (int)PlayerUpgradeType::N; ++i)
                {
                    if (i > 0)
                        HorizontalLine(1_px);
                    DrawPlayerUpgradeRow(LINE_ID + (size_t)i * 10, (PlayerUpgradeType)i, labels[i]);
                }
            }
            EndPanel();

            BeginPanel(LINE_ID);
            PanelHeader(G->Loc[LocKey_POWERUPS]);
            {
                size_t id = LINE_ID;
                constexpr size_t settingsPopupId = Hax::Hash("PowerupPopup");
                ToggleEx(id, G->AutoUseUpgrs, G->Loc[LocKey_AutoApplyUpgrades], {}, {.SettingsId = settingsPopupId});

                if (BeginModal(settingsPopupId))
                {
                    const float px10 = 10_px;
                    Hax::Gui::BeginHorizontal(15_px);
                    {
                        size_t half = ((size_t)UpgradeType::N + 1) / 2;

                        Hax::Gui::Dummy(Hax::Vector2(px10, 0.f));
                        Hax::Gui::BeginVertical(5_px);
                        {
                            {
                                Hax::Gui::Dummy(Hax::Vector2(0.f, px10));
                                for (size_t i = 0; i < half; ++i)
                                {
                                    auto& upgrInfo = G->UpgradesData[i];
                                    CheckboxEx((size_t)&upgrInfo, upgrInfo.AutoUse, G->Loc[upgrInfo.LocKey]);
                                }
                                Hax::Gui::Dummy(Hax::Vector2(0.f, px10));
                            }
                        }
                        Hax::Gui::EndVertical();

                        Hax::Gui::BeginVertical(5_px);
                        {
                            Hax::Gui::Dummy(Hax::Vector2(0.f, px10));
                            for (size_t i = half; i < (size_t)UpgradeType::N; ++i)
                            {
                                auto& upgrInfo = G->UpgradesData[i];
                                CheckboxEx((size_t)&upgrInfo, upgrInfo.AutoUse, G->Loc[upgrInfo.LocKey]);
                            }
                            Hax::Gui::Dummy(Hax::Vector2(0.f, px10));
                        }

                        Hax::Gui::EndVertical();
                        Hax::Gui::Dummy(Hax::Vector2(px10, 0.f));
                    }
                    Hax::Gui::EndHorizontal();

                    EndModal();
                }

                HorizontalLine(1_px);

                {
                    MainLabel(G->Loc[LocKey_ManualApplication]);

                    Hax::Gui::BeginVertical(5_px);
                    Hax::Gui::BeginHorizontal(5_px);
                    {
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

                            bool disabled = item.itemToggle().toggleState() == true;
                            if (AutoLayoutButton(LINE_ID + i * 5542, G->Loc[data.LocKey], {.Disabled = disabled, .Icon = data.Icon}))
                                G->UpgradeToToggle = item;
                        }
                    }
                    Hax::Gui::EndHorizontal();
                    Hax::Gui::EndVertical();
                }
            }
            EndPanel();

            BeginPanel(LINE_ID);
            PanelHeader(G->Loc[LocKey_HEALTHPACKS], G->Loc[LocKey_ApplyAidKit]);
            {
                Hax::Gui::BeginVertical(5_px);
                Hax::Gui::BeginHorizontal(5_px);
                {
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

                        bool disabled = aid.itemToggle().toggleState() == true || IsPlayerFullHp();
                        if (AutoLayoutButton(LINE_ID + i * 5542, G->Loc[data.LocKey], {.Disabled = disabled, .Icon = data.Icon}))
                            G->AidToToggle = aid;
                    }
                }
                Hax::Gui::EndHorizontal();
                Hax::Gui::EndVertical();
            }
            EndPanel();
        }
        Hax::Gui::Dummy({0.f, 0.f});
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();
    }
}
