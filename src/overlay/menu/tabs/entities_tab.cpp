#include <src/pch.h>

#include <src/cheat.h>

#include "../localization.h"
#include "../widgets.h"

#define LINE_ID (HAX_LINE * 123)

namespace Cheat
{
    static void UpToDatePlayer(PlayerAvatar& player)
    {
        if (GameDirector dir = GameDirector::instance())
        {
            for (PlayerAvatar avatar : dir.PlayerList())
            {
                if (avatar == player)
                    return;
            }
        }

        player = nullptr;
    }

    void DrawEntitiesTab()
    {
        const Hax::Vector2 mainAreaSize = Hax::Gui::GetContentRegionAvail();
        const float spacing = 20_px;
        const Hax::Vector2 columnSize = { (mainAreaSize.X - 3 * spacing) / 2, mainAreaSize.Y };

        // Column 1 
        Hax::Gui::Space(spacing);
        Hax::Gui::BeginContainer(0, { .W = columnSize.X, .H = columnSize.Y });
        Hax::Gui::BeginVertical(spacing);
        Hax::Gui::Dummy({ 0.f, 0.f });
        {
            BeginPanel(LINE_ID);
            PanelHeader(G->Loc[LocKey_ENEMIES]);
            {
                const float spacing2 = 5_px;
                Hax::Vector2 sz = CalcButtonSize(G->Loc[LocKey_Spawn]);
                Hax::Gui::BeginHorizontal(spacing2);
                {
                    constexpr size_t dropListId = Hax::Hash(L"EnemySelect");
                    const float dropListW = Hax::Gui::GetContentRegionAvail().X - spacing2 - sz.X;

                    Hax::Gui::BeginVertical(3_px);
                    {
                        Hax::WStringView view = L"-";
                        if (G->SelectedEnemySetup < G->EnemiesPool.Size())
                            view = G->EnemiesPool.begin()[G->SelectedEnemySetup].key;

                        if (DropdownBtn(LINE_ID, view, dropListW))
                            OpenPopup(dropListId, Hax::Gui::GetCursorPos());
                    }
                    Hax::Gui::EndVertical();

                    if (BeginDropList(dropListId, { dropListW, 150_px }))
                    {
                        float w = Hax::Gui::GetContentRegionAvail().X;
                        for (size_t i = 0; i < G->EnemiesPool.Size(); ++i)
                        {
                            if (Selectable(LINE_ID + i * 10000, G->EnemiesPool.begin()[i].key,
                                G->SelectedEnemySetup == i, { .MinW = w }))
                            {
                                G->SelectedEnemySetup = i;
                                ClosePopup(dropListId);
                            }
                        }

                        EndDropList();
                    }

                    bool enabled = !G->IsClient && G->SelectedEnemySetup < G->EnemiesPool.Size() && G->IsInGame;
                    if (Button(LINE_ID, G->Loc[LocKey_Spawn], {}, { .Enabled = enabled }))
                        G->EnemyToSpawn = (G->EnemiesPool.begin() + G->SelectedEnemySetup)->value;
                }
                Hax::Gui::EndHorizontal();

                HorizontalLine(1_px);
                ToggleEx(LINE_ID, G->NoGrabMaxTime, G->Loc[LocKey_NoGrabLimit], G->Loc[LocKey_AvailableIfHost], { .Disabled = G->IsClient });
                HorizontalLine(1_px);
                ToggleEx(LINE_ID, G->EnemiesEsp, G->Loc[LocKey_DisplayThroughWalls]);
            }
            EndPanel();
        }
        Hax::Gui::Dummy({ 0.f, 0.f });
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();

        // Column 2
        Hax::Gui::Space(spacing);
        Hax::Gui::BeginContainer(0, { .W = columnSize.X, .H = columnSize.Y });
        Hax::Gui::BeginVertical(spacing);
        Hax::Gui::Dummy({ 0.f, 0.f });
        {
            BeginPanel(LINE_ID);
            PanelHeader(G->Loc[LocKey_PLAYERS]);
            {
                ToggleEx(LINE_ID, G->PlayersEsp, G->Loc[LocKey_DisplayThroughWalls]);
                HorizontalLine(1_px);
                ToggleEx(LINE_ID, G->PlayersChams, G->Loc[LocKey_XrayHighlight]);
                HorizontalLine(1_px);

                {
                    static PlayerAvatar s_SelectedPlayer;

                    GameDirector dir = GameDirector::instance();
                    System::List<PlayerAvatar> players{}; if (dir) players = dir.PlayerList();

                    if (s_SelectedPlayer)
                        UpToDatePlayer(s_SelectedPlayer);

                    const float w = Hax::Gui::GetContentRegionAvail().X;
                    constexpr size_t dropListId = Hax::Hash(L"PlayerSelect");
                    Hax::Gui::BeginVertical(3_px);
                    {
                        Hax::WStringView preview = s_SelectedPlayer ? SemiFunc::PlayerGetName(s_SelectedPlayer).ToView() : L"-";
                        if (DropdownBtn(LINE_ID, preview, w))
                            OpenPopup(dropListId, Hax::Gui::GetCursorPos());
                    }
                    Hax::Gui::EndVertical();

                    Hax::Gui::BeginVertical(5_px);
                    {
                        if (Button(LINE_ID, G->Loc[LocKey_Tumble], G->Loc[LocKey_HostOnly], { .Enabled = G->IsInGame && !G->IsClient && s_SelectedPlayer && !s_SelectedPlayer.isDisabled(), .MinW = w }))
                            G->PlayerToTumble = s_SelectedPlayer;

                        Hax::Gui::BeginHorizontal(5_px);
                        {
                            const float btnW = (w - 5_px) / 2.f;

                            bool enabled = G->IsInGame && !G->IsClient && s_SelectedPlayer && !s_SelectedPlayer.deadSet();
                            if (Button(LINE_ID, G->Loc[LocKey_Kill], G->Loc[LocKey_HostOnly], { .Enabled = enabled, .MinW = btnW }))
                                G->PlayerToKill = s_SelectedPlayer;

                            enabled = G->IsInGame && !G->IsClient && s_SelectedPlayer && s_SelectedPlayer.deadSet();
                            if (Button(LINE_ID, G->Loc[LocKey_Revive], G->Loc[LocKey_HostOnly], { .Enabled = enabled, .MinW = btnW }))
                                G->PlayerToRevive = s_SelectedPlayer;
                        }
                        Hax::Gui::EndHorizontal();
                    }
                    Hax::Gui::EndVertical();

                    const size_t nPlayers = players ? players.GetCount() : 0;
                    const float selectableH = CalcButtonHeight();
                    const float dropListH = selectableH * Hax::Max(1ULL, Hax::Min(nPlayers, 5ULL)) + 5_px * 2.f + 1.f;
                    if (BeginDropList(dropListId, { w, dropListH }))
                    {
                        for (size_t j = 0; j < nPlayers; ++j)
                        {
                            PlayerAvatar player = players[j];
                            if (Selectable(LINE_ID + j * 10000, SemiFunc::PlayerGetName(player).ToView(), player == s_SelectedPlayer, { .MinW = w }))
                            {
                                s_SelectedPlayer = player;
                                ClosePopup(dropListId);
                            }
                        }
                        EndDropList();
                    }
                }

                HorizontalLine(1_px);

                {
                    const float w = Hax::Gui::GetContentRegionAvail().X;
                    Hax::Gui::BeginVertical(5_px);
                    {
                        MainLabel(G->Loc[LocKey_VoiceChat]);
                        Hax::Gui::BeginHorizontal(5_px);
                        {
                            PlayerAvatar avatar = PlayerAvatar::instance();
                            PlayerVoiceChat chat = avatar ? avatar.voiceChat() : nullptr;

                            bool disabled = !G->IsInGame || !avatar.voiceChatFetched() || !chat;

                            const float btnW = (w - 5_px) / 2.f;
                            if (Button(LINE_ID, G->Loc[LocKey_Alive], {}, { .Enabled = !disabled && chat.inLobbyMixer(), .MinW = btnW }))
                                G->ChatPrefered = ChatPref::Alive;

                            if (Button(LINE_ID, G->Loc[LocKey_Dead], {}, { .Enabled = !disabled && !chat.inLobbyMixer(), .MinW = btnW }))
                                G->ChatPrefered = ChatPref::Dead;
                        }
                        Hax::Gui::EndHorizontal();
                    }
                    Hax::Gui::EndVertical();
                }
            }
            EndPanel();
        }
        Hax::Gui::Dummy({ 0.f, 0.f });
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();
    }
}