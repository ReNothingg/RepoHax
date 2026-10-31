#include <src/pch.h>

#include <src/cheat.h>

#include "../localization.h"
#include "../widgets.h"
#include "../theme.h"
#include "tabs.h"

#define LINE_ID (HAX_LINE * 123)

namespace Cheat
{
    static PlayerAvatar s_SelectedPlayer;

    static Hax::WStringView LocalizedEnemyName(Hax::WStringView name)
    {
        if (G->Language != Lang_Ru)
            return name;

        struct EnemyName { Hax::WStringView English; Hax::WStringView Russian; };
        static constexpr EnemyName names[] =
        {
            {L"Animal", L"Зверь"}, {L"Beamer", L"Лучевик"}, {L"Birthday Boy", L"Именинник"},
            {L"Bomb Thrower", L"Подрывник"}, {L"Bowtie", L"Бабочка"}, {L"Ceiling Eye", L"Потолочный глаз"},
            {L"Duck", L"Утка"}, {L"Elsa", L"Эльза"}, {L"Floater", L"Парящий"},
            {L"Head Grabber", L"Головохват"}, {L"Heart Hugger", L"Сердцеед"}, {L"Hidden", L"Невидимка"},
            {L"Hunter", L"Охотник"}, {L"Oogly", L"Глазастик"}, {L"Robe", L"Мантия"},
            {L"Runner", L"Бегун"}, {L"Shadow", L"Тень"}, {L"Slow Mouth", L"Пасть"},
            {L"Slow Walker", L"Тихоход"}, {L"Spinny", L"Крутилка"}, {L"Thin Man", L"Тонкий человек"},
            {L"Tick", L"Клещ"}, {L"Tricycle", L"Трёхколёсник"}, {L"Tumbler", L"Кувыркун"},
            {L"Upscream", L"Крикун"}, {L"Valuable Thrower", L"Метатель ценностей"}
        };

        for (const EnemyName& entry : names)
            if (name == entry.English)
                return entry.Russian;
        return name;
    }

    static bool PlayerInSlots(PlayerAvatar player, PlayerAvatar (&slots)[16])
    {
        if (!player)
            return false;

        for (PlayerAvatar entry : slots)
            if (entry == player)
                return true;
        return false;
    }

    static void SetPlayerInSlots(PlayerAvatar player, PlayerAvatar (&slots)[16], bool enabled)
    {
        for (PlayerAvatar& entry : slots)
        {
            if (entry != player)
                continue;
            if (!enabled)
                entry = null;
            return;
        }

        if (enabled)
        {
            for (PlayerAvatar& entry : slots)
            {
                if (!entry)
                {
                    entry = player;
                    return;
                }
            }
        }
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

        player = nullptr;
    }

    static constexpr Hax::Gui::Color kAvatarColors[] = {0xF5A524FF, 0x86B9FFFF, 0xC9A7F5FF, 0x6FD08CFF, 0xF08FB0FF, 0x7FD6D0FF};

    struct PlayerView
    {
        wchar_t Initial[2];
        Hax::Gui::Color Color;
        int Hp;
        int MaxHp;
        bool Dead;
    };

    static PlayerView ViewOf(PlayerAvatar player, size_t index)
    {
        PlayerView v{};
        Hax::WStringView name = SemiFunc::PlayerGetName(player).ToHaxView();
        v.Initial[0] = name.Empty() ? L'?' : name.Data()[0];
        v.Initial[1] = L'\0';
        ::CharUpperBuffW(v.Initial, 1);
        v.Color = kAvatarColors[index % _countof(kAvatarColors)];
        v.Dead = player.deadSet();
        if (PlayerHealth health = player.playerHealth())
        {
            v.Hp = v.Dead ? 0 : health.health();
            v.MaxHp = health.maxHealth();
        }
        return v;
    }

    static void DrawPlayersCard(System::List<PlayerAvatar> players)
    {
        const size_t count = (players != null) ? players.Count() : 0;
        Hax::char16 note[16]{};
        swprintf_s(note, L"%zu", count);

        BeginPanel(LINE_ID);
        PanelHeader(G->Loc[LocKey_PLAYERS], {}, {.Icon = Icon::Users, .Note = note});
        {
            ToggleEx(LINE_ID, G->PlayersEsp, G->Loc[LocKey_DisplayThroughWalls]);
            HorizontalLine(1_px);
            ToggleEx(LINE_ID, G->PlayersChams, G->Loc[LocKey_XrayHighlight]);

            if (count == 0)
            {
                HorizontalLine(1_px);
                HintLabel(G->Loc[LocKey_NoPlayers]);
            }
            else
            {
                PlayerAvatar local = PlayerAvatar::instance();
                Hax::Gui::BeginVertical();
                for (size_t i = 0; i < count; ++i)
                {
                    PlayerAvatar player = players[i];
                    if (!player)
                        continue;

                    const PlayerView v = ViewOf(player, i);
                    PlayerRowData row{};
                    row.Initial = v.Initial;
                    row.AvatarColor = v.Color;
                    row.Name = SemiFunc::PlayerGetName(player).ToHaxView();
                    row.Tag = player == local ? G->Loc[LocKey_YouTag] : Hax::WStringView();
                    row.Status = v.Dead ? G->Loc[LocKey_DeadStatus] : G->Loc[LocKey_AliveStatus];
                    row.StatusColor = v.Dead ? Theme::Danger : Theme::Ok;
                    row.Hp = v.Hp;
                    row.MaxHp = v.MaxHp;
                    row.Selected = player == s_SelectedPlayer;
                    if (PlayerRow(LINE_ID + i * 97, row))
                        s_SelectedPlayer = player;
                }
                Hax::Gui::EndVertical();
            }
        }
        EndPanel();
    }

    static void DrawSelectedPlayerCard(System::List<PlayerAvatar> players)
    {
        const bool host = G->IsInGame && !G->IsClient;
        PlayerAvatar local = PlayerAvatar::instance();
        const bool localReady = G->IsInGame && local && !local.deadSet() && !local.isDisabled();
        PlayerAvatar target = s_SelectedPlayer;

        Hax::WStringView title = target ? SemiFunc::PlayerGetName(target).ToHaxView() : G->Loc[LocKey_PlayerTarget];

        BeginPanel(LINE_ID);
        PanelHeader(title, {}, {.Icon = Icon::User, .HostOnly = true, .Note = target ? G->Loc[LocKey_SelectedTag] : Hax::WStringView()});
        {
            if (!target)
                HintLabel(G->Loc[LocKey_SelectPlayerHint]);

            PlayerHealth health = target ? target.playerHealth() : null;
            const bool dead = target && target.deadSet();
            const bool canHeal = host && target && health && !dead && health.health() < health.maxHealth();

            bool revive = false, heal = false, tumble = false, go = false, bring = false, kill = false;
            const GridButton buttons[] =
            {
                {.Label = G->Loc[LocKey_Revive], .Clicked = &revive, .Enabled = host && target && dead, .Style = ButtonStyle::Primary},
                {.Label = G->Loc[LocKey_HealPlayer], .Clicked = &heal, .Enabled = canHeal},
                {.Label = G->Loc[LocKey_Tumble], .Clicked = &tumble, .Enabled = host && target && !target.isDisabled()},
                {.Label = G->Loc[LocKey_TeleportToShort], .Clicked = &go, .Enabled = localReady && target && target != local},
                {.Label = G->Loc[LocKey_BringShort], .Clicked = &bring, .Enabled = localReady && host && target && target != local},
                {.Label = G->Loc[LocKey_Kill], .Clicked = &kill, .Enabled = host && target && !dead, .Style = ButtonStyle::Danger},
            };
            ButtonGrid(LINE_ID, buttons, _countof(buttons), 3);
            if (revive) G->PlayerToRevive = target;
            if (heal) G->PlayerToHeal = target;
            if (tumble) G->PlayerToTumble = target;
            if (kill) G->PlayerToKill = target;
            if (go || bring)
            {
                G->SelectedTeleportPlayer = target;
                G->TeleportAction = go ? TeleportQuickAction::ToSelectedPlayer : TeleportQuickAction::SelectedPlayerToMe;
            }
            HorizontalLine(1_px);

            const bool canManage = host && target;
            bool protectedPlayer = PlayerInSlots(target, G->ProtectedPlayers);
            if (ToggleEx(LINE_ID, protectedPlayer, G->Loc[LocKey_ProtectPlayer], G->Loc[LocKey_ProtectPlayerDesc], {.Disabled = !canManage}))
                SetPlayerInSlots(target, G->ProtectedPlayers, protectedPlayer);
            HorizontalLine(1_px);
            bool protectedTumble = PlayerInSlots(target, G->NoTumblePlayers);
            if (ToggleEx(LINE_ID, protectedTumble, G->Loc[LocKey_ProtectPlayerTumble], G->Loc[LocKey_ProtectPlayerTumbleDesc], {.Disabled = !canManage}))
                SetPlayerInSlots(target, G->NoTumblePlayers, protectedTumble);
        }
        EndPanel();
    }

    static void DrawYouCard()
    {
        PlayerAvatar localPlayer = PlayerAvatar::instance();
        const bool canReviveSelf = G->ReviveSelfSessionReady && !G->IsClient && localPlayer &&
            localPlayer.deadSet() && localPlayer.playerDeathHead();

        BeginPanel(LINE_ID);
        PanelHeader(G->Loc[LocKey_YOU], {}, {.Icon = Icon::User});
        {
            const int keyBefore = G->VkReviveSelf;
            if (ActionRow(LINE_ID, G->Loc[LocKey_ReviveSelf], G->Loc[LocKey_ReviveSelfDesc], G->Loc[LocKey_Revive],
                {.Enabled = canReviveSelf, .HostOnly = true, .Hotkey = &G->VkReviveSelf, .HotkeyId = Hax::Hash(L"ReviveSelfHotkeyEditor")}))
                G->ReviveSelfRequested = true;
            if (keyBefore != G->VkReviveSelf)
                Hax::IniSave(G->Config);
            HorizontalLine(1_px);

            PlayerVoiceChat chat = localPlayer ? localPlayer.voiceChat() : nullptr;
            const bool voiceReady = G->IsInGame && localPlayer && localPlayer.voiceChatFetched() && chat;
            const Hax::WStringView items[] = {G->Loc[LocKey_Alive], G->Loc[LocKey_Dead]};
            const int current = voiceReady && chat.inLobbyMixer() ? 1 : 0;
            const int picked = SegmentedRow(LINE_ID, G->Loc[LocKey_VoiceChat], items, 2, current, voiceReady);
            if (picked >= 0 && picked != current)
                G->ChatPrefered = picked == 0 ? ChatPref::Alive : ChatPref::Dead;
        }
        EndPanel();
    }

    static void DrawEnemiesCard()
    {
        const bool host = G->IsInGame && !G->IsClient;
        Hax::char16 note[24]{};
        int enemies = 0;
        if (EnemyDirector dir = EnemyDirector::instance(); dir && dir.enemiesSpawned() != null)
            enemies = dir.enemiesSpawned().Count();
        swprintf_s(note, G->Loc[LocKey_OnMapFmt].Data(), enemies);

        BeginPanel(LINE_ID);
        PanelHeader(G->Loc[LocKey_ENEMIES], {}, {.Icon = Icon::Ghost, .Note = note});
        {
            const float gap = 8_px;
            const Hax::Vector2 spawnSize = CalcButtonSize(G->Loc[LocKey_Spawn]);
            const Hax::Vector2 badge = CalcBadgeSize(G->Loc[LocKey_HostBadge]);
            constexpr size_t dropListId = Hax::Hash(L"EnemySelect");
            const float dropW = Hax::Gui::GetContentRegionAvail().X - spawnSize.X - badge.X - gap * 2.f;

            Hax::Gui::BeginHorizontal(gap);
            {
                Hax::WStringView view = L"—";
                if (G->SelectedEnemySetup < G->EnemiesPool.Size())
                    view = LocalizedEnemyName(G->EnemiesPool.begin()[G->SelectedEnemySetup].key);

                Hax::Vector2 dropPos = Hax::Gui::GetCursorPos();
                if (DropdownBtn(LINE_ID, view, dropW))
                    OpenPopup(dropListId, {dropPos.X, dropPos.Y + CalcButtonHeight()});

                const bool enabled = host && G->SelectedEnemySetup < G->EnemiesPool.Size();
                if (Button(LINE_ID, G->Loc[LocKey_Spawn], {}, {.Enabled = enabled, .Style = ButtonStyle::Primary}))
                    G->EnemyToSpawn = (G->EnemiesPool.begin() + G->SelectedEnemySetup)->value;

                Hax::Gui::BeginVertical();
                Hax::Gui::Space((CalcButtonHeight() - badge.Y) / 2.f);
                Badge(G->Loc[LocKey_HostBadge], Theme::Host);
                Hax::Gui::EndVertical();
            }
            Hax::Gui::EndHorizontal();

            if (BeginDropList(dropListId, {dropW, 220_px}))
            {
                const float w = Hax::Gui::GetContentRegionAvail().X;
                for (size_t i = 0; i < G->EnemiesPool.Size(); ++i)
                {
                    if (Selectable(LINE_ID + i * 10000, LocalizedEnemyName(G->EnemiesPool.begin()[i].key), G->SelectedEnemySetup == i, {.MinW = w}))
                    {
                        G->SelectedEnemySetup = i;
                        ClosePopup(dropListId);
                    }
                }
                EndDropList();
            }

            HorizontalLine(1_px);
            ToggleEx(LINE_ID, G->EnemiesEsp, G->Loc[LocKey_DisplayThroughWalls]);
            HorizontalLine(1_px);
            ToggleEx(LINE_ID, G->NoGrabMaxTime, G->Loc[LocKey_NoGrabLimit], {}, {.Disabled = G->IsClient, .HostOnly = true});
            HorizontalLine(1_px);
            DrawEnemyDirectorControls();
        }
        EndPanel();
    }

    void DrawEntitiesTab()
    {
        GameDirector dir = GameDirector::instance();
        System::List<PlayerAvatar> players{};
        if (dir)
            players = dir.PlayerList();
        if (s_SelectedPlayer)
            UpToDatePlayer(s_SelectedPlayer);

        static constexpr float weights[] = {1.35f, 1.f};
        BeginColumns(Hax::Hash("EntitiesColumns"), 2, weights);
        {
            DrawPlayersCard(players);
            DrawSelectedPlayerCard(players);
            DrawYouCard();
        }
        NextColumn();
        {
            DrawEnemiesCard();
            DrawTeamControlPanel();
        }
        EndColumns();
    }
}
