#include <src/pch.h>

#include <src/cheat.h>

#include "../localization.h"
#include "../widgets.h"
#include "../theme.h"

#define LINE_ID (HAX_LINE * 456)

namespace Cheat
{
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

    static void DrawPlayerTeleportSelector()
    {
        GameDirector dir = GameDirector::instance();
        System::List<PlayerAvatar> players{}; if (dir) players = dir.PlayerList();

        if (G->SelectedTeleportPlayer)
            UpToDatePlayer(G->SelectedTeleportPlayer);

        const size_t nPlayers = (players != null) ? players.Count() : 0;
        const float w = Hax::Gui::GetContentRegionAvail().X;
        constexpr size_t dropListId = Hax::Hash(L"TeleportPlayerSelect");

        const Hax::Vector2 pos = Hax::Gui::GetCursorPos();
        Hax::WStringView preview = G->SelectedTeleportPlayer ? SemiFunc::PlayerGetName(G->SelectedTeleportPlayer).ToHaxView() : Hax::WStringView(L"—");
        if (DropdownBtn(LINE_ID, preview, w))
            OpenPopup(dropListId, {pos.X, pos.Y + CalcButtonHeight()});

        const float selectableH = CalcButtonHeight();
        const float dropListH = selectableH * Hax::Max(1ULL, Hax::Min(nPlayers, 6ULL)) + 8_px + 1.f;
        if (BeginDropList(dropListId, {w, dropListH}))
        {
            for (size_t j = 0; j < nPlayers; ++j)
            {
                PlayerAvatar player = players[j];
                if (!player)
                    continue;

                if (Selectable(LINE_ID + j * 10000, SemiFunc::PlayerGetName(player).ToHaxView(), player == G->SelectedTeleportPlayer, {.MinW = w}))
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
            swprintf_s(label, _countof(label), G->Loc[LocKey_PositionSlotFmt].Data(),
                slot + 1, data.LevelsCompleted, data.Position.X, data.Position.Y, data.Position.Z);
        else
            swprintf_s(label, _countof(label), G->Loc[LocKey_PositionSlotEmptyFmt].Data(), slot + 1);

        MainLabel(label, data.Active ? Theme::Text : Theme::Faint);

        bool save = false, go = false, clear = false;
        const GridButton buttons[] =
        {
            {.Label = G->Loc[LocKey_SaveButton], .Clicked = &save, .Enabled = G->IsInGame},
            {.Label = G->Loc[LocKey_GoButton], .Clicked = &go, .Enabled = G->IsInGame && data.Active, .Style = ButtonStyle::Primary},
            {.Label = G->Loc[LocKey_ClearButton], .Clicked = &clear, .Enabled = data.Active, .Style = ButtonStyle::Ghost},
        };
        ButtonGrid(id, buttons, _countof(buttons), 3);
        if (save) G->SavePositionSlot = slot;
        if (go) G->TeleportSavedPositionSlot = slot;
        if (clear) G->ClearSavedPositionSlot = slot;
    }

    void DrawBalancePanel()
    {
        BeginPanel(LINE_ID);
        PanelHeader(G->Loc[LocKey_BALANCE], G->Loc[LocKey_BalanceDesc], {.Icon = Icon::Wallet, .HostOnly = true});
        {
            Hax::char16 current[32]{}, amount[32]{}, subtractLabel[48]{}, addLabel[48]{};
            swprintf_s(current, _countof(current), L"$%dK", G->CurrentRunCurrency);
            swprintf_s(amount, _countof(amount), L"$%dK", G->CurrencyChangeAmount);
            swprintf_s(subtractLabel, _countof(subtractLabel), L"− $%dK", G->CurrencyChangeAmount);
            swprintf_s(addLabel, _countof(addLabel), L"+ $%dK", G->CurrencyChangeAmount);

            InfoRow(G->Loc[LocKey_CurrentBalance], G->IsInGame ? Hax::WStringView(current) : Hax::WStringView(L"—"), G->IsInGame ? Theme::Ok : Theme::Faint);
            HorizontalLine(1_px);
            SliderEx(LINE_ID, G->Loc[LocKey_CurrencyChangeAmount], amount, &G->CurrencyChangeAmount, 1, 100000, SliderConvertInt);

            const bool canChange = G->IsInGame && !G->IsClient && StatsManager::instance();
            bool sub = false, add = false;
            const GridButton buttons[] =
            {
                {.Label = subtractLabel, .Clicked = &sub, .Enabled = canChange, .Tooltip = G->Loc[LocKey_SubtractMoney]},
                {.Label = addLabel, .Clicked = &add, .Enabled = canChange, .Style = ButtonStyle::Primary, .Tooltip = G->Loc[LocKey_AddMoney]},
            };
            ButtonGrid(LINE_ID, buttons, _countof(buttons), 2);
            if (sub) G->CurrencyDeltaPending = -G->CurrencyChangeAmount;
            if (add) G->CurrencyDeltaPending = G->CurrencyChangeAmount;
            HorizontalLine(1_px);

            if (G->CurrentRunCurrency > MaxSafeRunCurrency)
            {
                if (ActionRow(LINE_ID, G->Loc[LocKey_RepairBalance], G->Loc[LocKey_RepairBalanceDesc], G->Loc[LocKey_FixShort], {.Enabled = canChange}))
                    G->CurrencyRepairOverflow = true;
                HorizontalLine(1_px);
            }
            if (ActionRow(LINE_ID, G->Loc[LocKey_SetBalanceZero], {}, G->Loc[LocKey_ToZero], {.Enabled = canChange, .Style = ButtonStyle::Danger}))
                G->CurrencySetZero = true;
        }
        EndPanel();
    }

    void DrawMiscTab()
    {
        PlayerAvatar avatar = PlayerAvatar::instance();
        const bool playerReady = G->IsInGame && avatar && !avatar.deadSet() && !avatar.isDisabled();

        BeginColumns(Hax::Hash("TeleportColumns"), 3);
        {
            BeginPanel(LINE_ID);
            PanelHeader(G->Loc[LocKey_QUICK_TELEPORT], G->Loc[LocKey_TeleportPositionDesc], {.Icon = Icon::Target});
            {
                bool truck = false, extraction = false, valuable = false, panic = false;
                const GridButton buttons[] =
                {
                    {.Label = G->Loc[LocKey_TruckShort], .Clicked = &truck, .Enabled = playerReady, .Icon = Icon::Truck},
                    {.Label = G->Loc[LocKey_ExtractionShort], .Clicked = &extraction, .Enabled = playerReady, .Icon = Icon::Extract},
                    {.Label = G->Loc[LocKey_NearestValuable], .Clicked = &valuable, .Enabled = playerReady, .Icon = Icon::Gem},
                    {.Label = G->Loc[LocKey_PanicSafe], .Clicked = &panic, .Enabled = playerReady, .Icon = Icon::Shield},
                };
                ButtonGrid(LINE_ID, buttons, _countof(buttons), 1);
                if (truck) G->TeleportAction = TeleportQuickAction::ToTruck;
                if (extraction) G->TeleportAction = TeleportQuickAction::ToExtraction;
                if (valuable) G->TeleportAction = TeleportQuickAction::ToNearestValuable;
                if (panic) G->TeleportAction = TeleportQuickAction::PanicSafe;
                HorizontalLine(1_px);

                ToggleEx(LINE_ID, G->TruckEsp, G->Loc[LocKey_DisplayThroughWalls]);
                HorizontalLine(1_px);
                if (ActionRow(LINE_ID, G->Loc[LocKey_PlayerToCamera], {}, G->Loc[LocKey_MoveShort],
                    {.Enabled = playerReady, .Hotkey = &G->VkTeleportPlayerToCamera, .HotkeyId = Hax::Hash(L"PlayerToCameraHotkeyEditorTp")}))
                    G->TeleportAction = TeleportQuickAction::PlayerToCamera;
            }
            EndPanel();
        }
        NextColumn();
        {
            BeginPanel(LINE_ID);
            PanelHeader(G->Loc[LocKey_PLAYER_TELEPORT], {}, {.Icon = Icon::User});
            {
                MainLabel(G->Loc[LocKey_PlayerTarget], Theme::Muted);
                DrawPlayerTeleportSelector();

                const bool targetReady = playerReady && G->SelectedTeleportPlayer && G->SelectedTeleportPlayer != avatar;
                bool go = false, bring = false;
                const GridButton buttons[] =
                {
                    {.Label = G->Loc[LocKey_GoToPlayer], .Clicked = &go, .Enabled = targetReady, .Style = ButtonStyle::Primary},
                    {.Label = G->Loc[LocKey_BringToMe], .Clicked = &bring, .Enabled = targetReady && !G->IsClient, .Tooltip = G->Loc[LocKey_HostOnly]},
                };
                ButtonGrid(LINE_ID, buttons, _countof(buttons), 2);
                if (go) G->TeleportAction = TeleportQuickAction::ToSelectedPlayer;
                if (bring) G->TeleportAction = TeleportQuickAction::SelectedPlayerToMe;
            }
            EndPanel();
        }
        NextColumn();
        {
            BeginPanel(LINE_ID);
            PanelHeader(G->Loc[LocKey_SavedPositions], G->Loc[LocKey_SavedPositionsDesc], {.Icon = Icon::Save});
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
        EndColumns();
    }
}
