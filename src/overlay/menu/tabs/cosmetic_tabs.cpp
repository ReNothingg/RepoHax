#include <src/pch.h>

#include <src/cheat.h>

#include "../localization.h"
#include "../widgets.h"
#include "../theme.h"

#define LINE_ID (HAX_LINE * 789)

namespace Cheat
{
    void DrawCosmeticTab()
    {
        BeginColumns(Hax::Hash("CosmeticColumns"), 3);
        {
            BeginPanel(LINE_ID);
            PanelHeader(G->Loc[LocKey_GENERAL], {}, {.Icon = Icon::Shirt});
            {
                ToggleEx(LINE_ID, G->CosmeticBoxesEsp, G->Loc[LocKey_DisplayBoxesThroughWalls]);
                HorizontalLine(1_px);

                bool unlock = false, reset = false;
                const GridButton buttons[] =
                {
                    {.Label = G->Loc[LocKey_UnlockAll], .Clicked = &unlock, .Style = ButtonStyle::Primary},
                    {.Label = G->Loc[LocKey_ResetAll], .Clicked = &reset, .Style = ButtonStyle::Danger},
                };
                ButtonGrid(LINE_ID, buttons, _countof(buttons), 2);
                if (unlock) G->UnlockAllCosmetic = true;
                if (reset) G->ResetAllCosmetic = true;
            }
            EndPanel();
        }
        NextColumn();
        {
            const bool enabled = G->IsInGame && !G->IsClient;

            BeginPanel(LINE_ID);
            PanelHeader(G->Loc[LocKey_SPAWN], {}, {.Icon = Icon::Sparkle, .HostOnly = true});
            {
                bool clicked[4]{};
                const GridButton buttons[] =
                {
                    {.Label = G->Loc[LocKey_Common], .Clicked = &clicked[0], .Enabled = enabled},
                    {.Label = G->Loc[LocKey_Uncommon], .Clicked = &clicked[1], .Enabled = enabled},
                    {.Label = G->Loc[LocKey_Rare], .Clicked = &clicked[2], .Enabled = enabled},
                    {.Label = G->Loc[LocKey_UltraRare], .Clicked = &clicked[3], .Enabled = enabled, .Style = ButtonStyle::Primary},
                };
                ButtonGrid(LINE_ID, buttons, _countof(buttons), 2);
                for (int i = 0; i < 4; ++i)
                    if (clicked[i])
                        G->RarityToSpawn = i + 1;
            }
            EndPanel();
        }
        NextColumn();
        EndColumns();
    }
}
