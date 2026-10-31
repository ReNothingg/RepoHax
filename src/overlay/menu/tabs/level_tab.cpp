#include <src/pch.h>

#include <src/cheat.h>

#include "../localization.h"
#include "../widgets.h"
#include "../theme.h"
#include "tabs.h"

#define LINE_ID (HAX_LINE * 345)

namespace Cheat
{
    static Hax::WStringView LastActionText()
    {
        Hax::WStringView action = Hax::WStringView(G->SessionLastAction);
        return action == Hax::WStringView(L"Idle") ? G->Loc[LocKey_Idle] : action;
    }

    static void DrawStatusTiles()
    {
        RunManager manager = RunManager::instance();
        System::String levelName = (manager && manager.levelCurrent()) ? manager.levelCurrent().NarrativeName() : null;
        const int completed = manager ? manager.levelsCompleted() : 0;

        LevelGenerator gen = LevelGenerator::Instance();
        const int spawned = gen ? gen.ModulesSpawned() : 0;
        const int amount = gen ? gen.ModuleAmount() : 0;

        int enemies = 0;
        if (EnemyDirector dir = EnemyDirector::instance(); dir && dir.enemiesSpawned() != null)
            enemies = dir.enemiesSpawned().Count();

        int valuables = 0;
        if (ValuableDirector dir = ValuableDirector::instance(); dir && dir.valuableList() != null)
            valuables = dir.valuableList().Count();

        int points = 0;
        if (RoundDirector dir = RoundDirector::instance(); dir && dir.extractionPointList() != null)
            points = dir.extractionPointList().Count();

        Hax::char16 done[64]{}, modules[32]{}, enemyCount[16]{}, loot[32]{};
        swprintf_s(done, G->Loc[LocKey_LevelsDoneFmt].Data(), completed);
        swprintf_s(modules, L"%d / %d", spawned, amount);
        swprintf_s(enemyCount, L"%d", enemies);
        swprintf_s(loot, L"%d · %d", valuables, points);

        BeginColumns(Hax::Hash("LevelTiles"), 4);
        StatTile(G->Loc[LocKey_LevelName], levelName != null ? levelName.ToHaxView() : Hax::WStringView(L"—"), done);
        NextColumn();
        StatTile(G->Loc[LocKey_Modules], modules, {}, amount > 0 ? (float)spawned / (float)amount : 0.f);
        NextColumn();
        StatTile(G->Loc[LocKey_ActiveEnemies], enemyCount, G->Loc[LocKey_OnMapHint]);
        NextColumn();
        StatTile(G->Loc[LocKey_ValuablesPointsTile], loot, G->Loc[LocKey_PointsOpenHint]);
        EndColumns();
    }

    static void DrawProgressCard()
    {
        RunManager manager = RunManager::instance();
        const bool host = G->IsInGame && !G->IsClient;

        BeginPanel(LINE_ID);
        PanelHeader(G->Loc[LocKey_PROGRESS], {}, {.Icon = Icon::Bars});
        {
            const int completed = manager ? manager.levelsCompleted() : 0;
            const int delta = StepperRow(LINE_ID, G->Loc[LocKey_LevelsCompletedShort2], {}, completed, manager && host, true);
            if (delta != 0 && manager)
                manager.levelsCompleted() = Hax::Clamp(completed + delta, 0, 100);
            HorizontalLine(1_px);

            if (ActionRow(LINE_ID, G->Loc[LocKey_NextLevel], {}, G->Loc[LocKey_GoShort], {.Enabled = host, .HostOnly = true}))
                G->ForceNextLevel = true;
            HorizontalLine(1_px);

            StatsManager stats = StatsManager::instance();
            System::String saveName = stats ? stats.saveFileCurrent() : null;
            if (ActionRow(LINE_ID, G->Loc[LocKey_SaveFile], saveName != null ? saveName.ToHaxView() : Hax::WStringView(L"—"),
                G->Loc[LocKey_SaveShort], {.Enabled = G->IsInGame && stats}))
                G->SaveWorldNow = true;
        }
        EndPanel();
    }

    static void DrawMapFilterCard()
    {
        BeginPanel(LINE_ID);
        PanelHeader(G->Loc[LocKey_MAPFILTER], {}, {.Icon = Icon::Map, .HostOnly = true});
        {
            Hax::Gui::BeginVertical(6_px);
            Hax::Gui::BeginHorizontal(6_px);
            {
                const bool lastMap = G->TotalBans + 1 == G->LevelBans.Size();
                for (size_t i = 0; i < G->LevelBans.Size(); ++i)
                {
                    LevelBan& ban = G->LevelBans[i];
                    const bool disabled = G->IsClient || (lastMap && ban.Allowed);
                    if (Selectable(LINE_ID + i * 10000, ban.Name, !G->IsClient && !ban.Allowed, {.AutoLayout = true, .Disabled = disabled}))
                    {
                        ban.Allowed = !ban.Allowed;
                        G->TotalBans += ban.Allowed ? -1 : 1;
                    }
                }
            }
            Hax::Gui::EndHorizontal();
            Hax::Gui::EndVertical();
        }
        EndPanel();
    }

    static void DrawSafetyCard()
    {
        BeginPanel(LINE_ID);
        PanelHeader(G->Loc[LocKey_SAFETY], G->Loc[LocKey_WorldSessionToolsDesc], {.Icon = Icon::Shield});
        {
            ToggleEx(LINE_ID, G->SessionSafetyEnabled, G->Loc[LocKey_SessionSafety], G->Loc[LocKey_SessionSafetyDesc]);
            HorizontalLine(1_px);
            ToggleEx(LINE_ID, G->AutoCancelClientUnsafe, G->Loc[LocKey_CancelHostOnlyClient], G->Loc[LocKey_CancelHostOnlyClientDesc]);
            HorizontalLine(1_px);
            ToggleEx(LINE_ID, G->PreserveSaveOnDeath, G->Loc[LocKey_PreserveSaveOnDeath], G->Loc[LocKey_PreserveSaveOnDeathDesc]);
            HorizontalLine(1_px);
            ToggleEx(LINE_ID, G->DisableAllPlayersDeadCheck, G->Loc[LocKey_BlockAllDeadGameover], G->Loc[LocKey_BlockAllDeadGameoverDesc]);
        }
        EndPanel();
    }

    static void DrawLevelActionsCard()
    {
        const bool host = G->IsInGame && !G->IsClient;

        BeginPanel(LINE_ID);
        PanelHeader(G->Loc[LocKey_LEVEL], {}, {.Icon = Icon::Play, .HostOnly = true});
        {
            bool reload = false, unlock = false;
            const GridButton buttons[] =
            {
                {.Label = G->Loc[LocKey_ReloadLevel], .Clicked = &reload, .Enabled = host && RunManager::instance()},
                {.Label = G->Loc[LocKey_UnlockExtractionPoints], .Clicked = &unlock, .Enabled = host && RoundDirector::instance()},
            };
            ButtonGrid(LINE_ID, buttons, _countof(buttons), 1);
            if (reload)
                G->ReloadCurrentLevel = true;
            if (unlock)
                G->UnlockExtractionPoints = true;

            HorizontalLine(1_px);
            InfoRow(G->Loc[LocKey_LastAction], LastActionText(), Theme::Muted);
        }
        EndPanel();
    }

    void DrawLevelTab()
    {
        DrawStatusTiles();

        BeginColumns(Hax::Hash("LevelColumns"), 3);
        {
            DrawProgressCard();
            DrawMapFilterCard();
        }
        NextColumn();
        {
            DrawSafetyCard();
            DrawLevelActionsCard();
        }
        NextColumn();
        {
            DrawWorldControlPanel();
        }
        EndColumns();
    }
}
