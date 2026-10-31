#include <src/pch.h>

#include "menu.h"

#include <src/cheat.h>
#include <src/resource.h>

#include "localization.h"
#include "search_index.h"
#include "theme.h"
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
        {DrawStatsTab, LocKey_Stats, LocKey_StatsPageDesc, L''},
        {DrawEntitiesTab, LocKey_Entities, LocKey_EntitiesPageDesc, L''},
        {DrawVisionTab, LocKey_Vision, LocKey_VisionPageDesc, L''},
        {DrawValuablesTab, LocKey_Valuables, LocKey_EconomyPageDesc, L''},
        {DrawItemsTab, LocKey_Items, LocKey_ItemsPageDesc, L''},
        {DrawLevelTab, LocKey_Level, LocKey_LevelPageDesc, L''},
        {DrawCosmeticTab, LocKey_Cosmetic, LocKey_CosmeticPageDesc, L''},
        {DrawMiscTab, LocKey_Misc, LocKey_TeleportPageDesc, L''},
        {DrawSettingsTab, LocKey_Settings, LocKey_SettingsPageDesc, L''},
        {DrawGodToolsTab, LocKey_GodTools, LocKey_EditorPageDesc, L''}
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
        {LocKey_PlayerCategory, L'', {0, 2, 6}, 3},
        {LocKey_SessionCategory,L'', {5, 1, 0}, 2},
        {LocKey_EconomyCategory,L'', {3, 0, 0}, 1},
        {LocKey_ToolsCategory,  L'', {4, 7, 9}, 3},
        {LocKey_Settings,       L'', {8, 0, 0}, 1}
    };

    static size_t g_ActiveGroup = 0;
    static size_t g_ActiveTab = 0;
    static wchar_t g_SearchBuf[64]{};

    static Hax::Gui::ScrollStyle PageScrollStyle()
    {
        return
        {
            .TrackWidth = 8_px,
            .ThumbPadding = 2_px,
            .TrackCol = 0x0,
            .ThumbCol = Theme::Surface3,
            .ThumbHovCol = 0x3A414AFF,
            .ThumbActiveCol = Theme::Accent
        };
    }

    void ToggleMenuVisibility()
    {
        ::PostMessageW(G->GameWndHandle, MenuToggleMessage(), 0, 0);
    }

    void PrepareMenu()
    {
        Hax::Gui::CreateLayer(L"Background", -1);
        Hax::Gui::CreateLayer(L"Popups", 1);

        Hax::Handle hCheat = (Hax::Handle)G->Handle;

        // Images
        {
            G->Logo                                          = Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG1, L"PNG"));
            G->AppIcon                                       = Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG16, L"PNG"));
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

    //=========================================================================
    // Helpers
    //=========================================================================

    static size_t GroupOfTab(size_t tab)
    {
        for (size_t g = 0; g < _countof(g_TabGroups); ++g)
            for (size_t i = 0; i < g_TabGroups[g].Count; ++i)
                if (g_TabGroups[g].Tabs[i] == tab)
                    return g;
        return 0;
    }

    static void Navigate(size_t tab)
    {
        g_ActiveTab = tab;
        g_ActiveGroup = GroupOfTab(tab);
    }

    static Hax::WStringView LastActionText()
    {
        Hax::WStringView action = Hax::WStringView(G->SessionLastAction);
        return action == Hax::WStringView(L"Idle") ? G->Loc[LocKey_Idle] : action;
    }

    // Case-insensitive substring match; `needle` must already be lower case.
    static bool ContainsNoCase(Hax::WStringView hay, const wchar_t* needle)
    {
        wchar_t buf[192];
        const size_t n = Hax::Min(hay.Size(), _countof(buf) - 1);
        wmemcpy(buf, hay.Data(), n);
        buf[n] = L'\0';
        ::CharLowerBuffW(buf, (DWORD)n);
        return wcsstr(buf, needle) != nullptr;
    }

    // Panel titles are stored in capitals; show them in sentence case in results.
    static void SentenceCase(Hax::WStringView text, wchar_t* out, size_t outSize)
    {
        const size_t n = Hax::Min(text.Size(), outSize - 1);
        wmemcpy(out, text.Data(), n);
        out[n] = L'\0';
        if (n > 1)
            ::CharLowerBuffW(out + 1, (DWORD)(n - 1));
    }

    //=========================================================================
    // Side panel
    //=========================================================================

    static void DrawNavigation()
    {
        for (size_t i = 0; i < _countof(g_TabGroups); ++i)
        {
            const TabGroup& group = g_TabGroups[i];
            const bool active = g_ActiveGroup == i;
            const bool hasChildren = group.Count > 1;

            if (NavItem(Hax::Hash(L"NavGroup") + i, G->Loc[group.Loc], group.Icon, active, hasChildren && !active) && !active)
            {
                g_ActiveGroup = i;
                g_ActiveTab = group.Tabs[0];
            }

            if (active && hasChildren)
            {
                Hax::Gui::Space(2_px);
                for (size_t t = 0; t < group.Count; ++t)
                {
                    const size_t tabIndex = group.Tabs[t];
                    if (NavSubItem(Hax::Hash(L"NavSub") + tabIndex, G->Loc[g_Tabs[tabIndex].Loc], g_ActiveTab == tabIndex))
                        g_ActiveTab = tabIndex;
                }
                Hax::Gui::Space(6_px);
            }
        }
    }

    static void DrawSearchResults(const wchar_t* query, float maxHeight)
    {
        struct Result
        {
            Hax::WStringView Label;   // original text, used to highlight the control
            size_t Tab;
            bool IsPage;
        };

        constexpr size_t kMaxResults = 16;
        Result results[kMaxResults];
        size_t count = 0;

        const float rowH = 42_px + 2_px;
        const size_t fit = (size_t)Hax::Max(1.f, maxHeight / rowH);
        const size_t limit = Hax::Min(fit, kMaxResults);

        for (size_t t = 0; t < _countof(g_Tabs) && count < limit; ++t)
            if (ContainsNoCase(G->Loc[g_Tabs[t].Loc], query))
                results[count++] = {G->Loc[g_Tabs[t].Loc], t, true};

        for (const SearchEntry& entry : g_SearchIndex)
        {
            if (count >= limit)
                break;

            const Hax::WStringView label = G->Loc[entry.Key];
            if (!ContainsNoCase(label, query))
                continue;

            bool duplicate = false;
            for (size_t i = 0; i < count && !duplicate; ++i)
                duplicate = results[i].Tab == entry.Tab && results[i].Label == label;
            if (!duplicate)
                results[count++] = {label, entry.Tab, false};
        }

        if (count == 0)
        {
            Hax::Gui::Space(6_px);
            Hax::Gui::BeginHorizontal();
            Hax::Gui::Space(10_px);
            HintLabel(G->Loc[LocKey_NothingFound]);
            Hax::Gui::EndHorizontal();
            return;
        }

        Hax::Gui::BeginVertical(2_px);
        for (size_t i = 0; i < count; ++i)
        {
            const Result& r = results[i];
            const size_t group = GroupOfTab(r.Tab);

            wchar_t title[96];
            SentenceCase(r.Label, title, _countof(title));

            wchar_t where[96];
            if (r.IsPage)
                swprintf_s(where, _countof(where), L"%.*ls", (int)G->Loc[g_TabGroups[group].Loc].Size(), G->Loc[g_TabGroups[group].Loc].Data());
            else
                swprintf_s(where, _countof(where), L"%.*ls · %.*ls",
                    (int)G->Loc[g_TabGroups[group].Loc].Size(), G->Loc[g_TabGroups[group].Loc].Data(),
                    (int)G->Loc[g_Tabs[r.Tab].Loc].Size(), G->Loc[g_Tabs[r.Tab].Loc].Data());

            if (SearchResultItem(Hax::Hash(L"SearchResult") + i, title, where))
            {
                Navigate(r.Tab);
                if (!r.IsPage)
                    SetSearchHighlight(r.Label);
                g_SearchBuf[0] = L'\0';
            }
        }
        Hax::Gui::EndVertical();
    }

    static void DrawSessionCard(float width)
    {
        const float cardH = 96_px;
        const Hax::Rect card = Hax::Rect::FromPosSize(Hax::Gui::GetCursorPos(), {width, cardH});
        Hax::Gui::PlaceItem(card.GetSize());
        if (!Hax::Gui::IsItemVisible(card))
            return;

        Hax::Gui::DrawRect(card.Min, card.Max,
            {.BorderColor = Theme::Line, .BorderTh = 1_px, .FillColor = Theme::Surface1, .Rounding = 10_px});

        const float pad = 12_px;
        const float titleH = Hax::Gui::Scale(12.5f);
        const float rowH = Hax::Gui::Scale(12.f);

        // Status line with the authority badge.
        {
            const float lineY = card.Min.Y + pad + 10_px;
            Hax::Gui::DrawCircle({card.Min.X + pad + 3.5_px, lineY}, 3.5_px, {.FillColor = G->IsInGame ? Theme::Ok : Theme::Faint});

            const Hax::WStringView status = G->IsInGame ? G->Loc[LocKey_InGame] : G->Loc[LocKey_MainMenuStatus];
            const Hax::Vector2 statusSize = Hax::Gui::CalcTextSize(G->NunitoSans_Bold, status, titleH);
            Hax::Gui::DrawString(G->NunitoSans_Bold, status, {card.Min.X + pad + 14_px, lineY - statusSize.Y / 2.f}, titleH, {.Color = Theme::Text});

            const Hax::WStringView badge = G->IsClient ? G->Loc[LocKey_ClientBadge] : G->Loc[LocKey_HostBadge];
            const Hax::Gui::Color badgeCol = G->IsClient ? Theme::Accent : Theme::Host;
            const float badgeFont = Hax::Gui::Scale(10.f);
            const Hax::Vector2 badgeText = Hax::Gui::CalcTextSize(G->NunitoSans_ExtraBold, badge, badgeFont);
            const Hax::Vector2 badgeSize = {badgeText.X + 16_px, 20_px};
            const Hax::Vector2 badgeMin = {card.Max.X - pad - badgeSize.X, lineY - badgeSize.Y / 2.f};
            Hax::Gui::LinearColor badgeBg(badgeCol);
            badgeBg.A = 0.13f;
            Hax::Gui::DrawRect(badgeMin, badgeMin + badgeSize, {.FillColor = badgeBg.ToColor(), .Rounding = badgeSize.Y / 2.f});
            Hax::Gui::DrawString(G->NunitoSans_ExtraBold, badge, badgeMin + (badgeSize - badgeText) / 2.f, badgeFont, {.Color = badgeCol});
        }

        // Level and players.
        Hax::WStringView levelName = L"—";
        Hax::char16 players[24] = L"—";
        if (G->IsInGame)
        {
            RunManager manager = RunManager::instance();
            System::String name = (manager && manager.levelCurrent()) ? manager.levelCurrent().NarrativeName() : null;
            if (name != null)
                levelName = name.ToHaxView();

            GameDirector director = GameDirector::instance();
            if (director && director.PlayerList() != null)
                swprintf_s(players, _countof(players), L"%d", director.PlayerList().Count());
        }

        auto row = [&](float y, Hax::WStringView key, Hax::WStringView value)
        {
            const Hax::Vector2 keySize = Hax::Gui::CalcTextSize(G->NunitoSans_SemiBold, key, rowH);
            Hax::Gui::DrawString(G->NunitoSans_SemiBold, key, {card.Min.X + pad, y}, rowH, {.Color = Theme::Faint});

            const float valueMinX = card.Min.X + pad + keySize.X + 10_px;
            const Hax::Vector2 valueSize = Hax::Gui::CalcTextSize(G->NunitoSans_SemiBold, value, rowH);
            const float valueX = Hax::Max(valueMinX, card.Max.X - pad - valueSize.X);
            Hax::Gui::PushClipRect({{valueMinX, y - 2_px}, {card.Max.X - pad, y + valueSize.Y + 2_px}});
            Hax::Gui::DrawString(G->NunitoSans_SemiBold, value, {valueX, y}, rowH, {.Color = Theme::Text});
            Hax::Gui::PopClipRect();
        };

        row(card.Min.Y + 46_px, G->Loc[LocKey_LevelName], levelName);
        row(card.Min.Y + 70_px, G->Loc[LocKey_PlayersLabel], players);
    }

    static void DrawSidePanel()
    {
        if (!BeginSidePanel())
            return;

        const float railW = Hax::Gui::GetContentRegionAvail().X;
        const float pad = 12_px;

        Hax::Gui::Space(18_px);
        Hax::Gui::BeginHorizontal();
        Hax::Gui::Space(pad);
        Hax::Gui::BeginContainer(0, {.W = railW - pad * 2.f, .H = Hax::Gui::GetContentRegionAvail().Y - 14_px});
        Hax::Gui::BeginVertical();
        {
            const float innerW = Hax::Gui::GetContentRegionAvail().X;

            // Program icon + feature search.
            Hax::Gui::BeginHorizontal(10_px);
            {
                Image(G->AppIcon, {34_px, 34_px}, 17_px);
                SearchField(Hax::Hash(L"FeatureSearch"), g_SearchBuf, _countof(g_SearchBuf), G->Loc[LocKey_SearchHint],
                    Hax::Gui::GetContentRegionAvail().X);
            }
            Hax::Gui::EndHorizontal();

            Hax::Gui::Space(16_px);

            const float buttonH = CalcButtonHeight();
            const float bottomH = 96_px + 10_px + buttonH;

            wchar_t query[_countof(g_SearchBuf)];
            const wchar_t* start = g_SearchBuf;
            while (*start == L' ')
                ++start;
            wcscpy_s(query, start);
            size_t len = wcslen(query);
            while (len > 0 && query[len - 1] == L' ')
                query[--len] = L'\0';
            ::CharLowerBuffW(query, (DWORD)len);

            if (len == 0)
                DrawNavigation();
            else
                DrawSearchResults(query, Hax::Gui::GetContentRegionAvail().Y - bottomH - 12_px);

            Hax::Gui::Space(Hax::Max(0.f, Hax::Gui::GetContentRegionAvail().Y - bottomH));
            DrawSessionCard(innerW);
            Hax::Gui::Space(10_px);
            if (Button(Hax::Hash(L"ResetRiskyRail"), G->Loc[LocKey_ResetRiskyShort], G->Loc[LocKey_ResetRiskyTogglesDesc],
                {.MinW = innerW, .Style = ButtonStyle::Danger}))
                G->ResetSessionSafety = true;
        }
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();
        Hax::Gui::EndHorizontal();

        EndSidePanel();
    }

    //=========================================================================
    // Page header and footer
    //=========================================================================

    static void VCentered(float rowH, float itemH)
    {
        Hax::Gui::Space(Hax::Max(0.f, (rowH - itemH) / 2.f));
    }

    static void DrawPageHeader()
    {
        const float headerH = 64_px;
        Hax::Gui::BeginContainer(Hax::Hash("PageHeader"), {.H = headerH});
        Hax::Gui::BeginHorizontal();
        {
            const Tab& tab = g_Tabs[g_ActiveTab];
            const TabGroup& group = g_TabGroups[g_ActiveGroup];

            const float crumbH = Hax::Gui::Scale(11.5f);
            const float titleH = Hax::Gui::Scale(18.f);
            const float crumbLine = Hax::Gui::GetFontLineHeight(G->NunitoSans_SemiBold, crumbH);
            const float titleLine = Hax::Gui::GetFontLineHeight(G->NunitoSans_ExtraBold, titleH);

            Hax::Gui::Space(24_px);
            Hax::Gui::BeginVertical();
            {
                VCentered(headerH, crumbLine + 2_px + titleLine);
                Label(G->NunitoSans_SemiBold, G->Loc[group.Loc], crumbH, Theme::Faint);
                Hax::Gui::Space(2_px);
                Label(G->NunitoSans_ExtraBold, G->Loc[tab.Loc], titleH, Theme::Text);
            }
            Hax::Gui::EndVertical();

            Hax::Gui::Space(16_px);
            Hax::Gui::BeginVertical();
            {
                const float descH = Hax::Gui::Scale(11.5f);
                VCentered(headerH, crumbLine + 2_px + titleLine);
                Hax::Gui::Space(crumbLine + 2_px + (titleLine - Hax::Gui::GetFontLineHeight(G->NunitoSans_SemiBold, descH)) / 2.f);
                Label(G->NunitoSans_SemiBold, G->Loc[tab.Desc], descH, Theme::Faint);
            }
            Hax::Gui::EndVertical();

            // Right cluster: hide hint, menu key and close button.
            const Hax::WStringView hideText = G->Loc[LocKey_HideMenu];
            const Hax::WStringView keyName = G->VkOpenClose ? Hax::Gui::GetKeyName((Hax::uint8)G->VkOpenClose) : Hax::WStringView(L"—");
            const float hintH = Hax::Gui::Scale(11.5f);
            const Hax::Vector2 hintSize = Hax::Gui::CalcTextSize(G->NunitoSans_SemiBold, hideText, hintH);
            const float keyW = Hax::Max(22_px, Hax::Gui::CalcTextSize(G->NunitoSans_Bold, keyName, Hax::Gui::Scale(11.f)).X + 12_px);
            const float clusterW = hintSize.X + 8_px + keyW + 14_px + 32_px + 18_px;
            Hax::Gui::Space(Hax::Max(0.f, Hax::Gui::GetContentRegionAvail().X - clusterW));

            Hax::Gui::BeginVertical();
            VCentered(headerH, hintSize.Y);
            Label(G->NunitoSans_SemiBold, hideText, hintH, Theme::Faint);
            Hax::Gui::EndVertical();
            Hax::Gui::Space(8_px);
            Hax::Gui::BeginVertical();
            VCentered(headerH, 22_px);
            KeyCap(keyName);
            Hax::Gui::EndVertical();
            Hax::Gui::Space(14_px);
            Hax::Gui::BeginVertical();
            VCentered(headerH, 32_px);
            if (IconButton(Hax::Hash(L"CloseMenu"), L'', G->Loc[LocKey_CloseMenu]))
                ToggleMenuVisibility();
            Hax::Gui::EndVertical();
        }
        Hax::Gui::EndHorizontal();
        Hax::Gui::EndContainer();
        HorizontalLine(1_px);
    }

    static void DrawFooter(float footerH)
    {
        HorizontalLine(1_px);
        Hax::Gui::BeginContainer(Hax::Hash("PageFooter"), {.H = footerH - 1_px});
        Hax::Gui::BeginHorizontal();
        {
            const float textH = Hax::Gui::Scale(11.5f);
            const float lineH = Hax::Gui::GetFontLineHeight(G->NunitoSans_SemiBold, textH);
            const float h = footerH - 1_px;

            Hax::Gui::Space(24_px);
            StatusDot(G->IsInGame ? Theme::Ok : Theme::Faint, h);
            Hax::Gui::Space(8_px);
            Hax::Gui::BeginVertical();
            VCentered(h, lineH);
            Label(G->NunitoSans_SemiBold, LastActionText(), textH, Theme::Muted);
            Hax::Gui::EndVertical();

            const Hax::WStringView saved = G->Loc[LocKey_SettingsAutosave];
            const float savedW = Hax::Gui::CalcTextSize(G->NunitoSans_SemiBold, saved, textH).X;
            Hax::Gui::Space(Hax::Max(16_px, Hax::Gui::GetContentRegionAvail().X - savedW - 20_px));
            Hax::Gui::BeginVertical();
            VCentered(h, lineH);
            Label(G->NunitoSans_SemiBold, saved, textH, Theme::Faint);
            Hax::Gui::EndVertical();
        }
        Hax::Gui::EndHorizontal();
        Hax::Gui::EndContainer();
    }

    //=========================================================================
    // Menu
    //=========================================================================

    void RenderMenu()
    {
        TickSearchHighlight();

        BeginWindow();
        {
            DrawSidePanel();

            Hax::Gui::BeginVertical();
            {
                DrawPageHeader();

                const float footerH = 34_px;

                // The whole active page owns scrolling. Tab columns use auto-height so both
                // sides move together and only one scrollbar is shown.
                static size_t s_LastRenderedTab = (size_t)-1;
                const bool resetScroll = s_LastRenderedTab != g_ActiveTab;
                s_LastRenderedTab = g_ActiveTab;
                Hax::Gui::BeginContainer(Hax::Hash("Main area"),
                    {.H = Hax::Max(1.f, Hax::Gui::GetContentRegionAvail().Y - footerH), .Clip = true, .ScrollY = true, .Style = PageScrollStyle()});
                {
                    if (resetScroll)
                        Hax::Gui::ScrollYTo(0.f);
                    Hax::Gui::BeginHorizontal();
                    g_Tabs[g_ActiveTab].DrawFunc();
                    Hax::Gui::EndHorizontal();
                }
                Hax::Gui::EndContainer();

                DrawFooter(footerH);
            }
            Hax::Gui::EndVertical();
        }
        EndWindow();
    }
}
