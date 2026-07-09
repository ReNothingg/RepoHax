#include <src/pch.h>

#include <src/cheat.h>

#include "../localization.h"
#include "../widgets.h"

#define LINE_ID (HAX_LINE * 345)

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

    static Hax::WStringView LastActionText()
    {
        Hax::WStringView action = Hax::WStringView(G->SessionLastAction);
        return action == Hax::WStringView(L"Idle") ? G->Loc[LocKey_Idle] : action;
    }

    void DrawLevelTab()
    {
        const Hax::Vector2 mainAreaSize = Hax::Gui::GetContentRegionAvail();
        const float spacing = 20_px;
        const Hax::Vector2 columnSize = {(mainAreaSize.X - 3 * spacing) / 2, mainAreaSize.Y};

        // Column 1
        Hax::Gui::Space(spacing);
        Hax::Gui::BeginContainer(Hax::Hash("LevelColumnLeft"), {.W = columnSize.X, .H = columnSize.Y, .Clip = true, .ScrollY = true, .ScrollVisible = true, .Style = ColumnScrollStyle()});
        Hax::Gui::BeginVertical(spacing);
        Hax::Gui::Dummy({0.f, 0.f});
        {
            BeginPanel(LINE_ID);
            PanelHeader(G->Loc[LocKey_WorldSessionStatus]);
            {
                RunManager manager = RunManager::instance();
                System::String levelName = (manager && manager.levelCurrent()) ? manager.levelCurrent().NarrativeName() : null;
                StatusLine(G->Loc[LocKey_LevelName], levelName != null ? levelName.ToHaxView() : Hax::WStringView(L"-"));
                StatusLine(G->Loc[LocKey_InGame], G->IsInGame ? G->Loc[LocKey_Yes] : G->Loc[LocKey_No], G->IsInGame ? 0x7EE787FF : 0xFF8A8AFF);
                StatusLine(G->Loc[LocKey_Authority], G->IsClient ? G->Loc[LocKey_Client] : G->Loc[LocKey_HostSingleplayer], G->IsClient ? 0xFFD36EFF : 0x7EE787FF);

                Hax::char16 buf[64]{};
                swprintf_s(buf, _countof(buf), L"%d", manager ? manager.levelsCompleted() : 0);
                StatusLine(G->Loc[LocKey_LevelsCompletedShort], buf);

                LevelGenerator gen = LevelGenerator::Instance();
                swprintf_s(buf, _countof(buf), L"%d / %d", gen ? gen.ModulesSpawned() : 0, gen ? gen.ModuleAmount() : 0);
                StatusLine(G->Loc[LocKey_Modules], buf);

                int enemies = 0;
                if (EnemyDirector dir = EnemyDirector::instance(); dir && dir.enemiesSpawned() != null)
                    enemies = dir.enemiesSpawned().Count();
                swprintf_s(buf, _countof(buf), L"%d", enemies);
                StatusLine(G->Loc[LocKey_ActiveEnemies], buf);

                int valuables = 0;
                if (ValuableDirector dir = ValuableDirector::instance(); dir && dir.valuableList() != null)
                    valuables = dir.valuableList().Count();
                swprintf_s(buf, _countof(buf), L"%d", valuables);
                StatusLine(G->Loc[LocKey_ValuablesCount], buf);

                int points = 0;
                if (RoundDirector dir = RoundDirector::instance(); dir && dir.extractionPointList() != null)
                    points = dir.extractionPointList().Count();
                swprintf_s(buf, _countof(buf), L"%d", points);
                StatusLine(G->Loc[LocKey_ExtractionPointsCount], buf);

                StatsManager stats = StatsManager::instance();
                System::String saveName = stats ? stats.saveFileCurrent() : null;
                StatusLine(G->Loc[LocKey_SaveFile], saveName != null ? saveName.ToHaxView() : Hax::WStringView(L"-"));
                StatusLine(G->Loc[LocKey_LastAction], LastActionText(), 0xC2C8D4FF);
            }
            EndPanel();

            BeginPanel(LINE_ID);
            PanelHeader(G->Loc[LocKey_WorldSessionTools], G->Loc[LocKey_WorldSessionToolsDesc]);
            {
                const float gap = 5_px;
                const float w = Hax::Gui::GetContentRegionAvail().X;
                const float halfW = (w - gap) / 2.f;

                Hax::Gui::BeginHorizontal(gap);
                {
                    if (Button(LINE_ID, G->Loc[LocKey_SaveNow], {}, {.Enabled = G->IsInGame && StatsManager::instance(), .MinW = halfW}))
                        G->SaveWorldNow = true;

                    if (Button(LINE_ID, G->Loc[LocKey_ReloadLevel], G->Loc[LocKey_HostOnly], {.Enabled = G->IsInGame && !G->IsClient && RunManager::instance(), .MinW = halfW}))
                        G->ReloadCurrentLevel = true;
                }
                Hax::Gui::EndHorizontal();

                if (Button(LINE_ID, G->Loc[LocKey_UnlockExtractionPoints], G->Loc[LocKey_HostOnly], {.Enabled = G->IsInGame && !G->IsClient && RoundDirector::instance(), .MinW = w}))
                    G->UnlockExtractionPoints = true;

                HorizontalLine(1_px);

                ToggleEx(LINE_ID, G->PreserveSaveOnDeath, G->Loc[LocKey_PreserveSaveOnDeath], G->Loc[LocKey_PreserveSaveOnDeathDesc]);
                HorizontalLine(1_px);
                ToggleEx(LINE_ID, G->SessionSafetyEnabled, G->Loc[LocKey_SessionSafety], G->Loc[LocKey_SessionSafetyDesc]);
            }
            EndPanel();

            BeginPanel(LINE_ID);
            PanelHeader(G->Loc[LocKey_GENERAL]);
            {
                {
                    RunManager manager = RunManager::instance();
                    int levelsCompl = manager ? manager.levelsCompleted() : 0;

                    Hax::char16 buf[16] = {};
                    int nChars = swprintf_s(buf, _countof(buf), L"%d", levelsCompl);

                    if (nChars > 0)
                    {
                        Hax::WStringView valTxt = { buf, (size_t)nChars };
                        Hax::Vector2 valTxtSize = CalcMainLabelSize(valTxt);

                        float spacing = 2_px;
                        Hax::Vector2 btnSize = CalcRepeatBtnSize(L"+");

                        Hax::Gui::BeginHorizontal(spacing);
                        {
                            bool disabled = !manager || G->IsClient || !G->IsInGame;

                            MainLabelAlignedByH(G->Loc[LocKey_LevelsCompleted], btnSize.Y);
                            Hax::Gui::Space(Hax::Gui::GetContentRegionAvail().X - valTxtSize.X - spacing * 2.f - btnSize.X * 2.f - 8_px);
                            MainLabelAlignedByH(buf, btnSize.Y);
                            Hax::Gui::Space(8_px);
                            if (RepeatBtn(LINE_ID, L"-", !disabled && levelsCompl > 0)) manager.levelsCompleted()--;
                            if (RepeatBtn(LINE_ID, L"+", !disabled && levelsCompl < 100)) manager.levelsCompleted()++;
                        }
                        Hax::Gui::EndHorizontal();
                    }
                }

                HorizontalLine(1_px);

                bool disabled = G->IsClient || !G->IsInGame;
                if (Button(LINE_ID, G->Loc[LocKey_NextLevel], {}, {.Enabled = !disabled, .MinW = Hax::Gui::GetContentRegionAvail().X}))
                    G->ForceNextLevel = true;
            }
            EndPanel();
        }
        Hax::Gui::Dummy({0.f, 0.f});
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();

        // Column 2
        Hax::Gui::Space(spacing);
        Hax::Gui::BeginContainer(Hax::Hash("LevelColumnRight"), {.W = columnSize.X, .H = columnSize.Y, .Clip = true, .ScrollY = true, .ScrollVisible = true, .Style = ColumnScrollStyle()});
        Hax::Gui::BeginVertical(spacing);
        Hax::Gui::Dummy({0.f, 0.f});
        {
            BeginPanel(LINE_ID);
            PanelHeader(G->Loc[LocKey_MAPFILTER], G->Loc[LocKey_AvailableIfHost]);
            {
                Hax::Gui::BeginVertical(5_px);
                {
                    Hax::Gui::BeginHorizontal(5_px);
                    {
                        bool lastMap = G->TotalBans + 1 == G->LevelBans.Size();
                        for (size_t i = 0; i < G->LevelBans.Size(); ++i)
                        {
                            LevelBan& ban = G->LevelBans[i];
                            bool disabled = G->IsClient || lastMap && ban.Allowed;
                            if (Selectable(LINE_ID + i * 10000, ban.Name, !G->IsClient && !ban.Allowed, { .AutoLayout = true, .Disabled = disabled, .R = 5_px }))
                            {
                                ban.Allowed = !ban.Allowed;
                                G->TotalBans += ban.Allowed ? -1 : 1;
                            }
                        }
                    }
                    Hax::Gui::EndHorizontal();
                }
                Hax::Gui::EndVertical();
            }
            EndPanel();
        }
        Hax::Gui::Dummy({0.f, 0.f});
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();
    }
}
