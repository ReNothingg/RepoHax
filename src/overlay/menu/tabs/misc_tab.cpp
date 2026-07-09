#include <src/pch.h>

#include <src/cheat.h>

#include "../localization.h"
#include "../widgets.h"

#define LINE_ID (HAX_LINE * 456)

namespace Cheat
{
    static Hax::Gui::ScrollStyle ColumnScrollStyle()
    {
        return
        {
            .TrackWidth = 6_px,
            .ThumbPadding = 0.f,
            .TrackCol = 0x00000000,
            .ThumbCol = 0x4F5B70FF,
            .ThumbHovCol = 0x6C7A94FF,
            .ThumbActiveCol = 0x7F8FADFF
        };
    }

    static void UpToDatePlayer(PlayerAvatar& player)
    {
        if (GameDirector dir = GameDirector::instance())
        {
            System::List<PlayerAvatar> players = dir.PlayerList();
            if (players != null)
            {
                for (PlayerAvatar avatar : players)
                {
                    if (avatar == player)
                        return;
                }
            }
        }

        player = null;
    }

    static void StatusLine(Hax::WStringView name, Hax::WStringView value, Hax::Gui::Color valueColor = 0xD8DCE5FF)
    {
        Hax::Gui::BeginHorizontal();
        {
            MainLabel(name, 0x8D96A8FF);
            const Hax::Vector2 valueSize = CalcMainLabelSize(value);
            Hax::Gui::Space(Hax::Max(0.f, Hax::Gui::GetContentRegionAvail().X - valueSize.X));
            MainLabel(value, valueColor);
        }
        Hax::Gui::EndHorizontal();
    }

    static void DrawPlayerTeleportSelector()
    {
        GameDirector dir = GameDirector::instance();
        System::List<PlayerAvatar> players{}; if (dir) players = dir.PlayerList();

        if (G->SelectedTeleportPlayer)
            UpToDatePlayer(G->SelectedTeleportPlayer);

        const size_t nPlayers = (players != null) ? players.Count() : 0;
        const float w = Hax::Gui::GetContentRegionAvail().X;
        constexpr size_t dropListId = Hax::Hash(L"TeleportPlayerSelect");

        Hax::Gui::BeginVertical(3_px);
        {
            Hax::WStringView preview = G->SelectedTeleportPlayer ? SemiFunc::PlayerGetName(G->SelectedTeleportPlayer).ToHaxView() : L"-";
            if (DropdownBtn(LINE_ID, preview, w))
                OpenPopup(dropListId, Hax::Gui::GetCursorPos());
        }
        Hax::Gui::EndVertical();

        const float selectableH = CalcButtonHeight();
        const float dropListH = selectableH * Hax::Max(1ULL, Hax::Min(nPlayers, 6ULL)) + 5_px * 2.f + 1.f;
        if (BeginDropList(dropListId, { w, dropListH }))
        {
            for (size_t j = 0; j < nPlayers; ++j)
            {
                PlayerAvatar player = players[j];
                if (!player)
                    continue;

                if (Selectable(LINE_ID + j * 10000, SemiFunc::PlayerGetName(player).ToHaxView(), player == G->SelectedTeleportPlayer, { .MinW = w }))
                {
                    G->SelectedTeleportPlayer = player;
                    ClosePopup(dropListId);
                }
            }
            EndDropList();
        }
    }

    static void DrawSavedPositionRow(size_t id, int slot)
    {
        SavedPositionSlot& data = G->SavedPositions[slot];

        Hax::char16 label[96]{};
        if (data.Active)
        {
            swprintf_s(label, _countof(label), L"Slot %d  [L%d]  %.1f, %.1f, %.1f",
                slot + 1,
                data.LevelsCompleted,
                data.Position.X,
                data.Position.Y,
                data.Position.Z);
        }
        else
            swprintf_s(label, _countof(label), L"Slot %d  empty", slot + 1);

        Hax::Gui::BeginVertical(4_px);
        {
            MainLabel(label, data.Active ? 0xD8DCE5FF : 0x8D96A8FF);

            const float spacing = 5_px;
            const float w = Hax::Gui::GetContentRegionAvail().X;
            const float btnW = (w - spacing * 2.f) / 3.f;
            Hax::Gui::BeginHorizontal(spacing);
            {
                if (Button(id + 1, L"Save", {}, { .Enabled = G->IsInGame, .MinW = btnW }))
                    G->SavePositionSlot = slot;

                if (Button(id + 2, L"Go", {}, { .Enabled = G->IsInGame && data.Active, .MinW = btnW }))
                    G->TeleportSavedPositionSlot = slot;

                if (Button(id + 3, L"Clear", {}, { .Enabled = data.Active, .MinW = btnW }))
                    G->ClearSavedPositionSlot = slot;
            }
            Hax::Gui::EndHorizontal();
        }
        Hax::Gui::EndVertical();
    }

    void DrawMiscTab()
    {
        const Hax::Vector2 mainAreaSize = Hax::Gui::GetContentRegionAvail();
        const float spacing = 20_px;
        const Hax::Vector2 columnSize = {(mainAreaSize.X - 3 * spacing) / 2, mainAreaSize.Y};

        PlayerAvatar avatar = PlayerAvatar::instance();
        bool playerReady = G->IsInGame && avatar && !avatar.deadSet() && !avatar.isDisabled();

        // Column 1
        Hax::Gui::Space(spacing);
        Hax::Gui::BeginContainer(Hax::Hash("MiscColumnLeft"), {.W = columnSize.X, .H = columnSize.Y, .Clip = true, .ScrollY = true, .ScrollVisible = true, .Style = ColumnScrollStyle()});
        Hax::Gui::BeginVertical(spacing);
        Hax::Gui::Dummy({0.f, 0.f});
        {
            BeginPanel(LINE_ID);
            PanelHeader(L"TELEPORT / POSITION", L"Quick movement tools and saved slots");
            {
                const float w = Hax::Gui::GetContentRegionAvail().X;
                const float gap = 5_px;
                const float halfW = (w - gap) / 2.f;

                Hax::Gui::BeginHorizontal(gap);
                {
                    if (Button(LINE_ID, L"Truck", {}, {.Enabled = playerReady, .MinW = halfW}))
                        G->TeleportAction = TeleportQuickAction::ToTruck;

                    if (Button(LINE_ID, L"Extraction", {}, {.Enabled = playerReady, .MinW = halfW}))
                        G->TeleportAction = TeleportQuickAction::ToExtraction;
                }
                Hax::Gui::EndHorizontal();

                Hax::Gui::BeginHorizontal(gap);
                {
                    if (Button(LINE_ID, L"Nearest valuable", {}, {.Enabled = playerReady, .MinW = halfW}))
                        G->TeleportAction = TeleportQuickAction::ToNearestValuable;

                    if (Button(LINE_ID, L"Panic safe", {}, {.Enabled = playerReady, .MinW = halfW}))
                        G->TeleportAction = TeleportQuickAction::PanicSafe;
                }
                Hax::Gui::EndHorizontal();

                HorizontalLine(1_px);

                ToggleEx(LINE_ID, G->TruckEsp, G->Loc[LocKey_DisplayThroughWalls]);

                HorizontalLine(1_px);

                MainLabel(L"Player target");
                DrawPlayerTeleportSelector();

                Hax::Gui::BeginHorizontal(gap);
                {
                    bool targetReady = playerReady && G->SelectedTeleportPlayer && G->SelectedTeleportPlayer != avatar;
                    if (Button(LINE_ID, L"Go to player", {}, {.Enabled = targetReady, .MinW = halfW}))
                        G->TeleportAction = TeleportQuickAction::ToSelectedPlayer;

                    if (Button(LINE_ID, L"Bring to me", G->Loc[LocKey_HostOnly], {.Enabled = targetReady && !G->IsClient, .MinW = halfW}))
                        G->TeleportAction = TeleportQuickAction::SelectedPlayerToMe;
                }
                Hax::Gui::EndHorizontal();

                HorizontalLine(1_px);

                if (Button(LINE_ID, L"Player to camera", {}, {.Enabled = playerReady, .MinW = Hax::Gui::GetContentRegionAvail().X}))
                    G->TeleportAction = TeleportQuickAction::PlayerToCamera;
            }
            EndPanel();

            BeginPanel(LINE_ID);
            PanelHeader(L"SAVED POSITIONS", L"Runtime slots; cleared after game restart");
            {
                for (int i = 0; i < (int)_countof(G->SavedPositions); ++i)
                {
                    if (i > 0)
                        HorizontalLine(1_px);

                    DrawSavedPositionRow(LINE_ID + i * 100, i);
                }
            }
            EndPanel();
        }
        Hax::Gui::Dummy({0.f, 0.f});
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();

        // Column 2
        Hax::Gui::Space(spacing);
        Hax::Gui::BeginContainer(Hax::Hash("MiscColumnRight"), {.W = columnSize.X, .H = columnSize.Y, .Clip = true, .ScrollY = true, .ScrollVisible = true, .Style = ColumnScrollStyle()});
        Hax::Gui::BeginVertical(spacing);
        Hax::Gui::Dummy({0.f, 0.f});
        {
            BeginPanel(LINE_ID);
            PanelHeader(L"SESSION STATUS");
            {
                StatusLine(L"In game", G->IsInGame ? L"yes" : L"no", G->IsInGame ? 0x7EE787FF : 0xFF8A8AFF);
                StatusLine(L"Authority", G->IsClient ? L"client" : L"host / singleplayer", G->IsClient ? 0xFFD36EFF : 0x7EE787FF);

                RunManager manager = RunManager::instance();
                System::String levelName = (manager && manager.levelCurrent()) ? manager.levelCurrent().NarrativeName() : null;
                StatusLine(L"Level", levelName != null ? levelName.ToHaxView() : Hax::WStringView(L"-"));

                StatusLine(L"Last action", Hax::WStringView(G->SessionLastAction), 0xC2C8D4FF);
            }
            EndPanel();

            BeginPanel(LINE_ID);
            PanelHeader(L"SAFETY GUARDS", L"Guards that prevent stuck sessions and unsafe client-side calls");
            {
                ToggleEx(LINE_ID, G->SessionSafetyEnabled, L"Session safety", L"Auto-disables movement tools when player/game is not ready");
                HorizontalLine(1_px);
                ToggleEx(LINE_ID, G->AutoCancelClientUnsafe, L"Cancel host-only actions on client", L"Prevents queued host-only actions from firing without authority");
                HorizontalLine(1_px);
                ToggleEx(LINE_ID, G->PreserveSaveOnDeath, G->Loc[LocKey_PreserveSaveOnDeath], G->Loc[LocKey_PreserveSaveOnDeathDesc]);
                HorizontalLine(1_px);
                ToggleEx(LINE_ID, G->DisableAllPlayersDeadCheck, L"Block all-dead gameover", L"Host/singleplayer safety switch; reset it after testing");
                HorizontalLine(1_px);

                if (Button(LINE_ID, L"Reset risky toggles", L"Turns off noclip/freecam/visual overrides and clears queued commands", {.MinW = Hax::Gui::GetContentRegionAvail().X}))
                    G->ResetSessionSafety = true;
            }
            EndPanel();

            BeginPanel(LINE_ID);
            PanelHeader(L"QUICK SESSION ACTIONS");
            {
                const float gap = 5_px;
                const float w = Hax::Gui::GetContentRegionAvail().X;
                const float halfW = (w - gap) / 2.f;

                Hax::Gui::BeginHorizontal(gap);
                {
                    if (Button(LINE_ID, L"Save now", {}, {.Enabled = G->IsInGame && StatsManager::instance(), .MinW = halfW}))
                        G->SaveWorldNow = true;

                    if (Button(LINE_ID, L"Reload level", G->Loc[LocKey_HostOnly], {.Enabled = G->IsInGame && !G->IsClient && RunManager::instance(), .MinW = halfW}))
                        G->ReloadCurrentLevel = true;
                }
                Hax::Gui::EndHorizontal();

                if (Button(LINE_ID, L"Unlock extraction points", G->Loc[LocKey_HostOnly], {.Enabled = G->IsInGame && !G->IsClient && RoundDirector::instance(), .MinW = w}))
                    G->UnlockExtractionPoints = true;
            }
            EndPanel();
        }
        Hax::Gui::Dummy({0.f, 0.f});
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();
    }
}
