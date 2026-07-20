#include <src/pch.h>

#include "menu.h"

#include <src/cheat.h>
#include <src/resource.h>

#include "localization.h"
#include "widgets.h"
#include "tabs/tabs.h"

namespace Cheat
{
    struct Tab
    {
        void(*DrawFunc)(void);
        LocKey Loc;
        LocKey Desc;
        Hax::char16 Icon;
    };

    static const Tab g_Tabs[] = 
    {
        {DrawStatsTab, LocKey_Stats, LocKey_StatsPageDesc, L'\ue473'},
        {DrawEntitiesTab, LocKey_Entities, LocKey_EntitiesPageDesc, L'\uf8f5'},
        {DrawVisionTab, LocKey_Vision, LocKey_VisionPageDesc, L'\uf06e'},
        {DrawValuablesTab, LocKey_Valuables, LocKey_EconomyPageDesc, L'\uf81d'},
        {DrawItemsTab, LocKey_Items, LocKey_ItemsPageDesc, L'\uf1e2'},
        {DrawLevelTab, LocKey_Level, LocKey_LevelPageDesc, L'\uf5fd'},
        {DrawCosmeticTab, LocKey_Cosmetic, LocKey_CosmeticPageDesc, L'\ue136'},
        {DrawMiscTab, LocKey_Misc, LocKey_TeleportPageDesc, L'\uf86d'},
        {DrawSettingsTab, LocKey_Settings, LocKey_SettingsPageDesc, L'\uf013'},
        {DrawGodToolsTab, LocKey_GodTools, LocKey_EditorPageDesc, L'\uf521'}
    };

    struct TabGroup
    {
        LocKey Loc;
        Hax::char16 Icon;
        size_t Tabs[3];
        size_t Count;
    };

    static const TabGroup g_TabGroups[] =
    {
        {LocKey_PlayerCategory, L'\ue473', {0, 2, 6}, 3},
        {LocKey_SessionCategory,L'\uf5fd', {5, 1, 0}, 2},
        {LocKey_EconomyCategory,L'\uf81d', {3, 0, 0}, 1},
        {LocKey_ToolsCategory,  L'\uf468', {4, 7, 9}, 3},
        {LocKey_Settings,       L'\uf013', {8, 0, 0}, 1}
    };

    static size_t g_ActiveGroup = 0;
    static size_t g_ActiveTab = 0;

    static Hax::Gui::ScrollStyle PageScrollStyle()
    {
        return
        {
            .TrackWidth = 8_px,
            .ThumbPadding = 1_px,
            .TrackCol = 0x151820FF,
            .ThumbCol = 0x66738AFF,
            .ThumbHovCol = 0x8492ACFF,
            .ThumbActiveCol = 0xA1AEC4FF
        };
    }

    void ToggleMenuVisibility()
    {
        ::PostMessageW(G->GameWndHandle, WM_USER, 0, 0);
    }

    void PrepareMenu()
    {
        Hax::Gui::CreateLayer(L"Background", -1);
        Hax::Gui::CreateLayer(L"Popups", 1);

        Hax::Handle hCheat = (Hax::Handle)G->Handle;

        // Images
        {
            G->Logo                                          = Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG1, L"PNG"));
            G->UpgradesData[(int)UpgradeType::Rest].Icon     = Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG2, L"PNG"));
            G->UpgradesData[(int)UpgradeType::Jump].Icon     = Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG3, L"PNG"));
            G->UpgradesData[(int)UpgradeType::Range].Icon    = Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG8, L"PNG"));
            G->UpgradesData[(int)UpgradeType::Speed].Icon    = Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG9, L"PNG"));
            G->UpgradesData[(int)UpgradeType::Stamina].Icon  = Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG10, L"PNG"));
            G->UpgradesData[(int)UpgradeType::Strength].Icon = Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG11, L"PNG"));
            G->UpgradesData[(int)UpgradeType::Tumble].Icon   = Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG12, L"PNG"));
            G->UpgradesData[(int)UpgradeType::Wings].Icon    = Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG13, L"PNG"));
            G->UpgradesData[(int)UpgradeType::Health].Icon   = Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG14, L"PNG"));
            G->UpgradesData[(int)UpgradeType::Map].Icon      = Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG15, L"PNG"));
            Hax::Gui::TextureHandle unknownIcon              = Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG7, L"PNG"));
            G->UpgradesData[(int)UpgradeType::Climb].Icon    = unknownIcon;
            G->UpgradesData[(int)UpgradeType::Battery].Icon  = unknownIcon;
            G->UpgradesData[(int)UpgradeType::N].Icon        = unknownIcon;
            G->AidsData[(int)AidType::Small].Icon            = Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG6, L"PNG"));
            G->AidsData[(int)AidType::Medium].Icon           = Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG5, L"PNG"));
            G->AidsData[(int)AidType::Large].Icon            = Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG4, L"PNG"));
        }

        // Fonts
        {
            G->NunitoSans_SemiBold  = Hax::Gui::LoadFont(Hax::Gui::GetResourceData(hCheat, IDR_WOFF21, L"WOFF2"));
            G->NunitoSans_Bold      = Hax::Gui::LoadFont(Hax::Gui::GetResourceData(hCheat, IDR_WOFF22, L"WOFF2"));
            G->NunitoSans_ExtraBold = Hax::Gui::LoadFont(Hax::Gui::GetResourceData(hCheat, IDR_WOFF23, L"WOFF2"));
            G->Icons_Solid          = Hax::Gui::LoadFont(Hax::Gui::GetResourceData(hCheat, IDR_WOFF24, L"WOFF2"));
        }

        // Mouse textures
        {
            Hax::Gui::SetMouseTextureFromRes(Hax::Gui::MouseIcon_Arrow, hCheat, IDC_CURSOR7);
            Hax::Gui::SetMouseTextureFromRes(Hax::Gui::MouseIcon_TextInput, hCheat, IDC_CURSOR1);
            Hax::Gui::SetMouseTextureFromRes(Hax::Gui::MouseIcon_ResizeAll, hCheat, IDC_CURSOR6);
            Hax::Gui::SetMouseTextureFromRes(Hax::Gui::MouseIcon_ResizeNS, hCheat, IDC_CURSOR9);
            Hax::Gui::SetMouseTextureFromRes(Hax::Gui::MouseIcon_ResizeEW, hCheat, IDC_CURSOR4);
            Hax::Gui::SetMouseTextureFromRes(Hax::Gui::MouseIcon_ResizeNESW, hCheat, IDC_CURSOR3);
            Hax::Gui::SetMouseTextureFromRes(Hax::Gui::MouseIcon_ResizeNWSE, hCheat, IDC_CURSOR2);
            Hax::Gui::SetMouseTextureFromRes(Hax::Gui::MouseIcon_Hand, hCheat, IDC_CURSOR5);
            Hax::Gui::SetMouseTextureFromRes(Hax::Gui::MouseIcon_NotAllowed, hCheat, IDC_CURSOR8);
        }
    }

    static bool IsKeyJustDown(size_t id, Hax::uint8 vk)
    {
        return (G->KeyListenerId == 0 || G->KeyListenerId == id) && Hax::Gui::IsKeyJustDown(vk);
    }

    void RenderMenu()
    {
        BeginWindow();
        {
            if (BeginSidePanel())
            {
                const float sidePanelWidth = Hax::Gui::GetContentRegionAvail().X;

                Hax::Gui::Space(27_px);

                // Compact product identity leaves more room for navigation.
                {
                    Hax::Gui::BeginHorizontal();
                    {
                        Hax::Gui::Space(20_px);
                        Hax::Gui::BeginVertical(2_px);
                        Label(G->NunitoSans_ExtraBold, L"REPOHAX", 22_px, 0xF5F7FBFF);
                        Label(G->NunitoSans_SemiBold, G->Loc[LocKey_ControlCenter], 10_px, 0x5A8DFFFF);
                        Hax::Gui::EndVertical();
                    }
                    Hax::Gui::EndHorizontal();
                }

                Hax::Gui::Space(28_px);

                const float padding = 12_px;
                Hax::Gui::BeginHorizontal();
                {
                    Hax::Gui::Space(padding);
                    Hax::Gui::BeginContainer(0, { .W = sidePanelWidth - padding * 2.f });
                    {
                        Hax::Gui::BeginVertical(5_px);
                        {
                            DescLabel(G->Loc[LocKey_Navigation]);
                            Hax::Gui::Space(5_px);

                            for (size_t i = 0; i < _countof(g_TabGroups); ++i)
                            {
                                const TabGroup& group = g_TabGroups[i];
                                if (TabButton(HAX_LINE + i, G->Loc[group.Loc], group.Icon, g_ActiveGroup == i))
                                {
                                    if (g_ActiveGroup != i)
                                    {
                                        g_ActiveGroup = i;
                                        g_ActiveTab = group.Tabs[0];
                                    }
                                }
                            }

                            Hax::Gui::Space(18_px);
                            HorizontalLine(1_px);
                            Hax::Gui::Space(10_px);
                            DescLabel(G->Loc[LocKey_SessionStatus]);
                            Hax::Gui::Space(4_px);
                            MainLabel(G->IsInGame ? G->Loc[LocKey_InGame] : G->Loc[LocKey_MainMenuStatus],
                                G->IsInGame ? 0x7EE787FF : 0x8D96A8FF);
                            MainLabel(G->IsClient ? G->Loc[LocKey_Client] : G->Loc[LocKey_HostSingleplayer],
                                G->IsClient ? 0xFFD36EFF : 0x65B8FFFF);
                        }
                        Hax::Gui::EndVertical();
                    }
                    Hax::Gui::EndContainer();
                }
                Hax::Gui::EndHorizontal();

                EndSidePanel();
            }

            // Main area
            Hax::Gui::BeginVertical();
            {
                const TabGroup& activeGroup = g_TabGroups[g_ActiveGroup];

                // Every page has a stable title and purpose. The content beneath it
                // owns the only vertical scrollbar in the main area.
                Hax::Gui::BeginContainer(Hax::Hash("Page header"), {.H = 78_px});
                {
                    Hax::Gui::BeginVertical(5_px);
                    Hax::Gui::Space(17_px);
                    Hax::Gui::BeginHorizontal();
                    Hax::Gui::Space(20_px);
                    Hax::Gui::BeginVertical(3_px);
                    Label(G->NunitoSans_ExtraBold, G->Loc[g_Tabs[g_ActiveTab].Loc], 20_px, 0xF5F7FBFF);
                    DescLabel(G->Loc[g_Tabs[g_ActiveTab].Desc]);
                    Hax::Gui::EndVertical();
                    Hax::Gui::EndHorizontal();
                    Hax::Gui::EndVertical();
                }
                Hax::Gui::EndContainer();

                if (activeGroup.Count > 1)
                {
                    Hax::Gui::BeginContainer(Hax::Hash("Sub navigation"), {.H = 54_px});
                    {
                        Hax::Gui::BeginVertical();
                        Hax::Gui::Space(7_px);
                        Hax::Gui::BeginHorizontal(8_px);
                        Hax::Gui::Space(16_px);
                        {
                            const float available = Hax::Gui::GetContentRegionAvail().X - 16_px - 8_px * (float)(activeGroup.Count - 1);
                            const float buttonWidth = available / (float)activeGroup.Count;
                            for (size_t i = 0; i < activeGroup.Count; ++i)
                            {
                                const size_t tabIndex = activeGroup.Tabs[i];
                                if (SubTabButton(Hax::Hash(L"SubTab") + tabIndex, G->Loc[g_Tabs[tabIndex].Loc], g_ActiveTab == tabIndex, buttonWidth))
                                    g_ActiveTab = tabIndex;
                            }
                        }
                        Hax::Gui::EndHorizontal();
                        Hax::Gui::EndVertical();
                    }
                    Hax::Gui::EndContainer();
                    Hax::Gui::HorizontalLine(1_px, 0x252A35FF);
                }

                // The whole active page owns scrolling. Tab columns use auto-height so both
                // sides move together and only one scrollbar is shown.
                static size_t s_LastRenderedTab = (size_t)-1;
                const bool resetScroll = s_LastRenderedTab != g_ActiveTab;
                s_LastRenderedTab = g_ActiveTab;
                Hax::Gui::BeginContainer(Hax::Hash("Main area"), {.Clip = true, .ScrollY = true, .Style = PageScrollStyle()});
                {
                    if (resetScroll)
                        Hax::Gui::ScrollYTo(0.f);
                    Hax::Gui::BeginHorizontal();
                    g_Tabs[g_ActiveTab].DrawFunc();
                    Hax::Gui::EndHorizontal();
                }
                Hax::Gui::EndContainer();
            }
            Hax::Gui::EndVertical();
        }
        EndWindow();
    }
}
