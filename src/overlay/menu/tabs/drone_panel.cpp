#include <src/pch.h>

#include <src/cheat.h>

#include "../localization.h"
#include "../theme.h"
#include "../widgets.h"
#include "tabs.h"

#define LINE_ID (HAX_LINE * 1931)

namespace Cheat
{
    static bool DronePanelEnemyAlive(EnemyParent parent)
    {
        if (!parent || !parent.Spawned())
            return false;
        Enemy enemy = parent.enemy();
        if (!enemy)
            return false;
        EnemyHealth health = enemy.Health();
        return !health || !health.dead();
    }

    static bool DronePanelPlayerListed(System::List<PlayerAvatar> players, PlayerAvatar player)
    {
        if (!player || players == null)
            return false;
        for (PlayerAvatar entry : players)
            if (entry == player)
                return true;
        return false;
    }

    static void FormatDroneTarget(Hax::char16* buf, size_t size, Hax::WStringView tag, Hax::WStringView name)
    {
        swprintf_s(buf, size, L"%.*ls · %.*ls", (int)tag.Size(), tag.Data(), (int)name.Size(), name.Data());
    }

    void DrawDroneStrikePanel()
    {
        const bool host = G->IsInGame && !G->IsClient;

        GameDirector director = GameDirector::instance();
        System::List<PlayerAvatar> players{};
        if (director)
            players = director.PlayerList();
        System::List<EnemyParent> enemies{};
        if (EnemyDirector enemyDirector = EnemyDirector::instance())
            enemies = enemyDirector.enemiesSpawned();

        // Forget targets that left the game or died.
        if (G->DroneTargetPlayer && !DronePanelPlayerListed(players, G->DroneTargetPlayer))
            G->DroneTargetPlayer = null;
        if (G->DroneTargetEnemy && !DronePanelEnemyAlive(G->DroneTargetEnemy))
            G->DroneTargetEnemy = null;

        Hax::char16 note[32]{};
        if (G->DronesInFlight > 0)
            swprintf_s(note, L"%ls: %d", G->Loc[LocKey_DronesInFlight].Data(), G->DronesInFlight);

        BeginPanel(LINE_ID);
        PanelHeader(G->Loc[LocKey_DRONESTRIKE], G->Loc[LocKey_DroneStrikeDesc],
            {.Icon = Icon::Crosshair, .HostOnly = true, .Note = note});
        {
            const Hax::WStringView modes[] = {G->Loc[LocKey_DroneModeAuto], G->Loc[LocKey_DroneModeManual]};
            const int picked = SegmentedRow(LINE_ID, G->Loc[LocKey_DroneGuidance], modes, 2, G->DroneManualMode ? 1 : 0, true);
            if (picked >= 0)
                G->DroneManualMode = picked == 1;

            if (G->DroneManualMode)
            {
                HintLabel(G->Loc[LocKey_DroneManualHint]);
            }
            else
            {
                // Target picker: players first, then live enemies.
                Hax::char16 preview[128] = L"—";
                if (G->DroneTargetPlayer)
                    FormatDroneTarget(preview, _countof(preview), G->Loc[LocKey_DronePlayerTag],
                        SemiFunc::PlayerGetName(G->DroneTargetPlayer).ToHaxView());
                else if (G->DroneTargetEnemy)
                    FormatDroneTarget(preview, _countof(preview), G->Loc[LocKey_DroneEnemyTag],
                        G->DroneTargetEnemy.enemyName().ToHaxView());

                constexpr size_t dropListId = Hax::Hash(L"DroneTargetSelect");
                MainLabel(G->Loc[LocKey_DroneTarget]);
                const float w = Hax::Gui::GetContentRegionAvail().X;
                const Hax::Vector2 dropPos = Hax::Gui::GetCursorPos();
                if (DropdownBtn(LINE_ID, preview, w))
                    OpenPopup(dropListId, {dropPos.X, dropPos.Y + CalcButtonHeight()});

                if (BeginDropList(dropListId, {w, 240_px}))
                {
                    const float itemW = Hax::Gui::GetContentRegionAvail().X;
                    Hax::char16 label[128]{};
                    size_t index = 0;
                    if (players != null)
                    {
                        for (PlayerAvatar player : players)
                        {
                            ++index;
                            if (!player)
                                continue;
                            FormatDroneTarget(label, _countof(label), G->Loc[LocKey_DronePlayerTag],
                                SemiFunc::PlayerGetName(player).ToHaxView());
                            if (Selectable(LINE_ID + index * 10000, label, G->DroneTargetPlayer == player,
                                {.Disabled = player.deadSet(), .MinW = itemW}))
                            {
                                G->DroneTargetPlayer = player;
                                G->DroneTargetEnemy = null;
                                ClosePopup(dropListId);
                            }
                        }
                    }
                    if (enemies != null)
                    {
                        for (EnemyParent parent : enemies)
                        {
                            ++index;
                            if (!DronePanelEnemyAlive(parent))
                                continue;
                            FormatDroneTarget(label, _countof(label), G->Loc[LocKey_DroneEnemyTag], parent.enemyName().ToHaxView());
                            if (Selectable(LINE_ID + index * 10000, label, G->DroneTargetEnemy == parent, {.MinW = itemW}))
                            {
                                G->DroneTargetEnemy = parent;
                                G->DroneTargetPlayer = null;
                                ClosePopup(dropListId);
                            }
                        }
                    }
                    EndDropList();
                }
            }

            HorizontalLine(1_px);
            Hax::char16 buf[32]{};
            swprintf_s(buf, G->Loc[LocKey_DroneSpeedFmt].Data(), G->DroneSpeed);
            SliderEx(LINE_ID, G->Loc[LocKey_DroneSpeed], buf, &G->DroneSpeed, 4, 40, SliderConvertInt);
            HorizontalLine(1_px);
            swprintf_s(buf, L"%d", G->DroneSwarmSize);
            SliderEx(LINE_ID, G->Loc[LocKey_DroneSwarm], buf, &G->DroneSwarmSize, 1, 5, SliderConvertInt);
            HorizontalLine(1_px);
            swprintf_s(buf, L"%d", G->DroneWarheads);
            SliderEx(LINE_ID, G->Loc[LocKey_DroneWarheads], buf, &G->DroneWarheads, 1, 3, SliderConvertInt);
            HorizontalLine(1_px);
            ToggleEx(LINE_ID, G->DroneThroughWalls, G->Loc[LocKey_DroneThroughWalls], G->Loc[LocKey_DroneThroughWallsDesc]);
            HorizontalLine(1_px);
            ToggleEx(LINE_ID, G->DroneLethal, G->Loc[LocKey_DroneLethal], G->Loc[LocKey_DroneLethalDesc]);
            HorizontalLine(1_px);

            const bool hasTarget = G->DroneManualMode || G->DroneTargetPlayer || G->DroneTargetEnemy;
            const float gap = 8_px;
            const float btnW = (Hax::Gui::GetContentRegionAvail().X - gap) / 2.f;
            Hax::Gui::BeginHorizontal(gap);
            {
                if (Button(LINE_ID, G->Loc[LocKey_DroneLaunch], {},
                    {.Enabled = host && hasTarget, .MinW = btnW, .Style = ButtonStyle::Danger, .Icon = Icon::Zap}))
                    G->DroneLaunchRequested = true;
                if (Button(LINE_ID, G->Loc[LocKey_DroneDetonate], {},
                    {.Enabled = host && G->DronesInFlight > 0, .MinW = btnW}))
                    G->DroneDetonateRequested = true;
            }
            Hax::Gui::EndHorizontal();

            HorizontalLine(1_px);
            HotkeyEx(Hax::Hash(L"DroneLaunchHotkeyEditor"), G->VkDroneLaunch, G->Loc[LocKey_DroneLaunchHotkey]);
            HorizontalLine(1_px);
            HotkeyEx(Hax::Hash(L"DroneDetonateHotkeyEditor"), G->VkDroneDetonate, G->Loc[LocKey_DroneDetonateHotkey]);
        }
        EndPanel();
    }
}
