#include <src/pch.h>

#include <src/cheat.h>

#include "../localization.h"
#include "../menu.h"
#include "../widgets.h"
#include "../theme.h"

#define LINE_ID (HAX_LINE * 917)

namespace Cheat
{
    //=========================================================================
    // Shared panels (shown on Session, Entities and Economy pages)
    //=========================================================================

    void DrawTeamControlPanel()
    {
        const bool host = G->IsInGame && !G->IsClient;
        BeginPanel(LINE_ID);
        PanelHeader(G->Loc[LocKey_TEAM_CONTROL], {}, {.Icon = Icon::Shield, .HostOnly = true});
        {
            ToggleEx(LINE_ID, G->GodAllPlayers, G->Loc[LocKey_GodmodeAll], {}, {.Disabled = !host});
            HorizontalLine(1_px);
            ToggleEx(LINE_ID, G->GodAllPlayersNoTumble, G->Loc[LocKey_AntiTumbleAll], {}, {.Disabled = !host});
            HorizontalLine(1_px);

            bool heal = false, revive = false, gather = false;
            const GridButton buttons[] =
            {
                {.Label = G->Loc[LocKey_HealAllPlayers], .Clicked = &heal, .Enabled = host},
                {.Label = G->Loc[LocKey_ReviveAllPlayers], .Clicked = &revive, .Enabled = host},
                {.Label = G->Loc[LocKey_GatherAllPlayers], .Clicked = &gather, .Enabled = host},
            };
            ButtonGrid(LINE_ID, buttons, _countof(buttons), 3);
            if (heal) G->PlayerGodAction = PlayerGodCommand::HealAll;
            if (revive) G->PlayerGodAction = PlayerGodCommand::ReviveAll;
            if (gather) G->PlayerGodAction = PlayerGodCommand::GatherAll;
            HorizontalLine(1_px);

            Hax::char16 scale[32]{};
            swprintf_s(scale, L"%d%%", G->GodAllPlayerScalePercent);
            SliderEx(LINE_ID, G->Loc[LocKey_PlayerScaleAll], scale, &G->GodAllPlayerScalePercent, 25, 300, SliderConvertInt);
            if (Button(LINE_ID, G->Loc[LocKey_ApplyPlayerScaleAll], {}, {.Enabled = host, .MinW = Hax::Gui::GetContentRegionAvail().X}))
                G->PlayerGodAction = PlayerGodCommand::ApplyScaleAll;
        }
        EndPanel();
    }

    void DrawWorldControlPanel()
    {
        const bool inGame = G->IsInGame;
        BeginPanel(LINE_ID);
        PanelHeader(G->Loc[LocKey_WORLD_CONTROL], {}, {.Icon = Icon::Globe, .HostOnly = true});
        {
            Hax::char16 value[32]{};
            swprintf_s(value, L"%d%%", G->WorldTimeScalePercent);
            SliderEx(LINE_ID, G->Loc[LocKey_TimeScale], value, &G->WorldTimeScalePercent, 0, 300, SliderConvertInt);
            HorizontalLine(1_px);
            ToggleEx(LINE_ID, G->WorldGravityOverride, G->Loc[LocKey_GravityOverride]);
            if (G->WorldGravityOverride)
            {
                swprintf_s(value, L"%d", G->WorldGravityY);
                SliderEx(LINE_ID, G->Loc[LocKey_GravityY], value, &G->WorldGravityY, -30, 30, SliderConvertInt);
            }
            HorizontalLine(1_px);
            if (ToggleEx(LINE_ID, G->WorldFreezePhysics, G->Loc[LocKey_FreezePhysics]))
                G->WorldFreezePhysicsChanged = true;
            HorizontalLine(1_px);

            // Lighting commands are one-shot; remember the last choice for the selection.
            static int s_Lighting = 0;
            const Hax::WStringView items[] = {G->Loc[LocKey_LightNormal], G->Loc[LocKey_LightDark], G->Loc[LocKey_LightBright]};
            const int picked = SegmentedRow(LINE_ID, G->Loc[LocKey_LIGHTING], items, 3, s_Lighting, inGame, true);
            if (picked >= 0)
            {
                s_Lighting = picked;
                G->WorldLightAction = picked == 0 ? WorldLightCommand::Restore
                    : picked == 1 ? WorldLightCommand::Blackout : WorldLightCommand::FullBright;
            }
        }
        EndPanel();
    }

    void DrawEnemyDirectorControls()
    {
        const bool host = G->IsInGame && !G->IsClient;
        if (ToggleEx(LINE_ID, G->GodEnemiesPacified, G->Loc[LocKey_PacifyEnemies], {}, {.Disabled = !host, .HostOnly = true}))
            G->GodEnemiesPacifiedChanged = true;
        HorizontalLine(1_px);
        const bool frozenBefore = G->GodEnemiesFrozen;
        if (ToggleEx(LINE_ID, G->GodEnemiesFrozen, G->Loc[LocKey_FreezeEnemies], {}, {.Disabled = !host, .HostOnly = true}) && frozenBefore != G->GodEnemiesFrozen)
            G->GodEnemiesFreezeChanged = true;
        HorizontalLine(1_px);

        bool gather = false, kill = false, remove = false;
        const GridButton buttons[] =
        {
            {.Label = G->Loc[LocKey_GatherEnemies], .Clicked = &gather, .Enabled = host},
            {.Label = G->Loc[LocKey_KillAllEnemies], .Clicked = &kill, .Enabled = host, .Style = ButtonStyle::Danger},
            {.Label = G->Loc[LocKey_DeleteAllEnemies], .Clicked = &remove, .Enabled = host, .Style = ButtonStyle::Danger},
        };
        ButtonGrid(LINE_ID, buttons, _countof(buttons), 3);
        if (gather) G->EnemyGodAction = EnemyGodCommand::Gather;
        if (kill) G->EnemyGodAction = EnemyGodCommand::KillAll;
        if (remove) G->EnemyGodAction = EnemyGodCommand::DeleteAll;
    }

    void DrawEnemyDirectorPanel()
    {
        BeginPanel(LINE_ID);
        PanelHeader(G->Loc[LocKey_ENEMY_DIRECTOR], {}, {.Icon = Icon::Ghost, .HostOnly = true});
        DrawEnemyDirectorControls();
        EndPanel();
    }

    void DrawLootControlPanel()
    {
        const bool host = G->IsInGame && !G->IsClient;
        BeginPanel(LINE_ID);
        PanelHeader(G->Loc[LocKey_LOOT_CONTROL], {}, {.Icon = Icon::Box, .HostOnly = true});
        {
            const bool frozenBefore = G->GodLootFrozen;
            if (ToggleEx(LINE_ID, G->GodLootFrozen, G->Loc[LocKey_FreezeLoot], {}, {.Disabled = !host}) && frozenBefore != G->GodLootFrozen)
                G->GodLootFreezeChanged = true;
            HorizontalLine(1_px);

            bool toExtraction = false, toPlayer = false, toTruck = false, discover = false;
            const GridButton buttons[] =
            {
                {.Label = G->Loc[LocKey_LootToExtraction], .Clicked = &toExtraction, .Enabled = host, .Style = ButtonStyle::Primary, .Icon = Icon::Extract},
                {.Label = G->Loc[LocKey_LootToPlayer], .Clicked = &toPlayer, .Enabled = host, .Icon = Icon::User},
                {.Label = G->Loc[LocKey_LootToTruck], .Clicked = &toTruck, .Enabled = host, .Icon = Icon::Truck},
                {.Label = G->Loc[LocKey_DiscoverLoot], .Clicked = &discover, .Enabled = host, .Icon = Icon::Eye},
            };
            ButtonGrid(LINE_ID, buttons, _countof(buttons), 1);
            if (toExtraction) G->LootGodAction = LootGodCommand::BringToExtraction;
            if (toPlayer) G->LootGodAction = LootGodCommand::BringToPlayer;
            if (toTruck) G->LootGodAction = LootGodCommand::BringToTruck;
            if (discover) G->LootGodAction = LootGodCommand::DiscoverAll;
            HorizontalLine(1_px);

            Hax::char16 value[32]{};
            swprintf_s(value, L"%d%%", G->GodLootValuePercent);
            SliderEx(LINE_ID, G->Loc[LocKey_ValueMultiplier], value, &G->GodLootValuePercent, 0, 1000, SliderConvertInt);
            if (Button(LINE_ID, G->Loc[LocKey_ApplyValueMultiplier], {}, {.Enabled = host, .MinW = Hax::Gui::GetContentRegionAvail().X}))
                G->LootGodAction = LootGodCommand::ApplyValueMultiplier;
        }
        EndPanel();
    }

    //=========================================================================
    // Object editor page
    //=========================================================================

    static void DrawInspectorCard(bool inGame)
    {
        BeginPanel(LINE_ID);
        PanelHeader(G->Loc[LocKey_TARGET_INSPECTOR], {}, {.Icon = Icon::Crosshair});
        {
            ToggleEx(LINE_ID, G->GodTargetingEnabled, G->Loc[LocKey_TargetingEnabled]);
            HorizontalLine(1_px);

            InfoRow(G->Loc[LocKey_AimedObject], G->GodAimedObjectName, G->GodAimedObjectValid ? Theme::Accent : Theme::Faint);
            InfoRow(G->Loc[LocKey_LockedObject], G->GodTargetName, G->GodTargetObjectValid ? Theme::Ok : Theme::Faint);
            Hax::WStringView authority = !G->GodTargetObjectValid ? Hax::WStringView(L"—") :
                (G->GodTargetNetworked ? G->Loc[LocKey_NetworkObject] : G->Loc[LocKey_LocalObject]);
            InfoRow(G->Loc[LocKey_ObjectAuthority], authority, G->GodTargetNetworked ? Theme::Accent : Theme::Muted);

            if (G->GodTargetObjectValid)
            {
                Hax::char16 position[96]{};
                swprintf_s(position, L"%.1f, %.1f, %.1f", G->GodTargetPosition.X, G->GodTargetPosition.Y, G->GodTargetPosition.Z);
                InfoRow(G->Loc[LocKey_ObjectPosition], position);

                Hax::char16 layer[24]{};
                swprintf_s(layer, L"%d", G->GodTargetLayer);
                InfoRow(G->Loc[LocKey_ObjectLayer], layer);
            }
            HorizontalLine(1_px);

            if (ActionRow(LINE_ID, G->Loc[LocKey_LockTarget], {}, G->Loc[LocKey_TargetLockShort],
                {.Enabled = inGame, .Hotkey = &G->VkLockGodTarget, .HotkeyId = Hax::Hash(L"GodTargetHotkeyEditor")}))
                G->GodObjectAction = GodObjectCommand::LockAimed;
            if (Button(LINE_ID, G->Loc[LocKey_ClearTarget], {}, {.Enabled = inGame, .MinW = Hax::Gui::GetContentRegionAvail().X, .Style = ButtonStyle::Ghost}))
                G->GodObjectAction = GodObjectCommand::ClearTarget;
        }
        EndPanel();
    }

    static void DrawGizmoCard(bool inGame)
    {
        const bool hasTarget = inGame && G->GodTargetObjectValid;

        BeginPanel(LINE_ID);
        PanelHeader(G->Loc[LocKey_TRANSFORM_GIZMO], G->Loc[LocKey_GizmoControls], {.Icon = Icon::Cube});
        {
            ToggleEx(LINE_ID, G->GodGizmoVisible, G->Loc[LocKey_GizmoVisible]);
            HorizontalLine(1_px);
            if (!hasTarget)
                G->GodGizmoEditMode = false;
            if (ToggleEx(LINE_ID, G->GodGizmoEditMode, G->Loc[LocKey_GizmoEditMode], G->Loc[LocKey_GizmoEditModeDesc], {.Disabled = !hasTarget}) && G->GodGizmoEditMode)
            {
                // Enter the scene editor immediately instead of leaving the menu on
                // top of the handles and relying on a second hotkey press.
                ToggleMenuVisibility();
            }
            HorizontalLine(1_px);

            const Hax::WStringView modes[] = {G->Loc[LocKey_GizmoMove], G->Loc[LocKey_GizmoRotate], G->Loc[LocKey_GizmoScale]};
            const int mode = SegmentedRow(LINE_ID, G->Loc[LocKey_GizmoModeLabel], modes, 3, (int)G->GodGizmoModeCurrent, hasTarget, true);
            if (mode >= 0)
                G->GodGizmoModeCurrent = (GodGizmoMode)mode;

            const Hax::WStringView spaces[] = {G->Loc[LocKey_GizmoWorld], G->Loc[LocKey_GizmoLocal]};
            const int space = SegmentedRow(LINE_ID, G->Loc[LocKey_GizmoSpaceLabel], spaces, 2,
                G->GodGizmoSpaceCurrent == GodGizmoSpace::World ? 0 : 1, hasTarget, true);
            if (space >= 0)
                G->GodGizmoSpaceCurrent = space == 0 ? GodGizmoSpace::World : GodGizmoSpace::Local;
        }
        EndPanel();
    }

    static void DrawObjectManipulationPanel(bool inGame)
    {
        const bool hasTarget = inGame && G->GodTargetObjectValid;

        BeginPanel(LINE_ID);
        PanelHeader(G->Loc[LocKey_TELEKINESIS], {}, {.Icon = Icon::Hand});
        {
            if (ActionRow(LINE_ID, G->Loc[LocKey_HoldRelease], G->GodTelekinesisActive ? G->Loc[LocKey_Yes] : G->Loc[LocKey_No],
                G->GodTelekinesisActive ? G->Loc[LocKey_No] : G->Loc[LocKey_Yes], {.Enabled = hasTarget}))
                G->GodObjectAction = GodObjectCommand::ToggleTelekinesis;
            HorizontalLine(1_px);

            Hax::char16 value[32]{};
            swprintf_s(value, G->Loc[LocKey_Meters].Data(), G->GodHoldDistance);
            SliderEx(LINE_ID, G->Loc[LocKey_HoldDistance], value, &G->GodHoldDistance, 1, 25, SliderConvertInt);
            swprintf_s(value, L"%d", G->GodThrowForce);
            SliderEx(LINE_ID, G->Loc[LocKey_ThrowForce], value, &G->GodThrowForce, 5, 150, SliderConvertInt);

            bool pull = false, push = false, freeze = false, duplicate = false;
            const GridButton actions[] =
            {
                {.Label = G->Loc[LocKey_PullObject], .Clicked = &pull, .Enabled = hasTarget},
                {.Label = G->Loc[LocKey_PushObject], .Clicked = &push, .Enabled = hasTarget},
                {.Label = G->Loc[LocKey_FreezeObject], .Clicked = &freeze, .Enabled = hasTarget},
                {.Label = G->Loc[LocKey_DuplicateObject], .Clicked = &duplicate, .Enabled = hasTarget},
            };
            ButtonGrid(LINE_ID, actions, _countof(actions), 2);
            if (pull) G->GodObjectAction = GodObjectCommand::Pull;
            if (push) G->GodObjectAction = GodObjectCommand::Push;
            if (freeze) G->GodObjectAction = GodObjectCommand::FreezeToggle;
            if (duplicate) G->GodObjectAction = GodObjectCommand::Duplicate;
            HorizontalLine(1_px);

            swprintf_s(value, L"%d%%", G->GodScalePercent);
            if (SliderEx(LINE_ID, G->Loc[LocKey_ObjectScale], value, &G->GodScalePercent, 1, 1000, SliderConvertInt))
                G->GodScaleXPercent = G->GodScaleYPercent = G->GodScaleZPercent = G->GodScalePercent;
            swprintf_s(value, L"%d%%", G->GodScaleXPercent);
            SliderEx(LINE_ID, G->Loc[LocKey_ScaleX], value, &G->GodScaleXPercent, 1, 1000, SliderConvertInt);
            swprintf_s(value, L"%d%%", G->GodScaleYPercent);
            SliderEx(LINE_ID, G->Loc[LocKey_ScaleY], value, &G->GodScaleYPercent, 1, 1000, SliderConvertInt);
            swprintf_s(value, L"%d%%", G->GodScaleZPercent);
            SliderEx(LINE_ID, G->Loc[LocKey_ScaleZ], value, &G->GodScaleZPercent, 1, 1000, SliderConvertInt);
            HorizontalLine(1_px);
            swprintf_s(value, L"%d°", G->GodRotationX);
            SliderEx(LINE_ID, G->Loc[LocKey_RotationX], value, &G->GodRotationX, -180, 180, SliderConvertInt);
            swprintf_s(value, L"%d°", G->GodRotationY);
            SliderEx(LINE_ID, G->Loc[LocKey_RotationY], value, &G->GodRotationY, -180, 180, SliderConvertInt);
            swprintf_s(value, L"%d°", G->GodRotationZ);
            SliderEx(LINE_ID, G->Loc[LocKey_RotationZ], value, &G->GodRotationZ, -180, 180, SliderConvertInt);
            HorizontalLine(1_px);

            if (Button(LINE_ID, G->Loc[LocKey_ApplyTransform], {}, {.Enabled = hasTarget, .MinW = Hax::Gui::GetContentRegionAvail().X, .Style = ButtonStyle::Primary}))
                G->GodObjectAction = GodObjectCommand::ApplyTransform;

            bool del = false, clear = false;
            const GridButton tail[] =
            {
                {.Label = G->Loc[LocKey_DeleteTarget], .Clicked = &del, .Enabled = hasTarget && !G->IsClient, .Style = ButtonStyle::Danger},
                {.Label = G->Loc[LocKey_ClearTarget], .Clicked = &clear, .Enabled = hasTarget},
            };
            ButtonGrid(LINE_ID, tail, _countof(tail), 2);
            if (del) G->GodObjectAction = GodObjectCommand::DeleteTarget;
            if (clear) G->GodObjectAction = GodObjectCommand::ClearTarget;

            Hax::char16 undo[64]{};
            swprintf_s(undo, G->Loc[LocKey_UndoActionFmt].Data(), G->GodUndoCount);
            if (Button(LINE_ID, undo, {}, {.Enabled = G->GodUndoCount > 0, .MinW = Hax::Gui::GetContentRegionAvail().X, .Style = ButtonStyle::Ghost, .Icon = Icon::Reset}))
                G->GodObjectAction = GodObjectCommand::Undo;
        }
        EndPanel();
    }

    void DrawGodToolsTab()
    {
        const bool inGame = G->IsInGame;

        BeginColumns(Hax::Hash("GodToolsColumns"), 3);
        DrawInspectorCard(inGame);
        NextColumn();
        DrawGizmoCard(inGame);
        NextColumn();
        DrawObjectManipulationPanel(inGame);
        EndColumns();
    }
}
