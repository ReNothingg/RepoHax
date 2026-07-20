#include <src/pch.h>

#include <src/cheat.h>

#include "../localization.h"
#include "../menu.h"
#include "../widgets.h"

#define LINE_ID (HAX_LINE * 917)

namespace Cheat
{
    static void StatusLine(Hax::WStringView name, Hax::WStringView value, Hax::Gui::Color valueColor = 0xD8DCE5FF)
    {
        Hax::Gui::BeginHorizontal();
        MainLabel(name, 0x8D96A8FF);
        const Hax::Vector2 valueSize = CalcMainLabelSize(value);
        Hax::Gui::Space(Hax::Max(0.f, Hax::Gui::GetContentRegionAvail().X - valueSize.X));
        MainLabel(value, valueColor);
        Hax::Gui::EndHorizontal();
    }

    static void TwoButtons(Hax::WStringView left, Hax::WStringView right, bool enabled,
        GodObjectCommand leftAction, GodObjectCommand rightAction)
    {
        const float gap = 5_px;
        const float width = (Hax::Gui::GetContentRegionAvail().X - gap) / 2.f;
        Hax::Gui::BeginHorizontal(gap);
        if (Button(LINE_ID, left, {}, {.Enabled = enabled, .MinW = width}))
            G->GodObjectAction = leftAction;
        if (Button(LINE_ID, right, {}, {.Enabled = enabled, .MinW = width}))
            G->GodObjectAction = rightAction;
        Hax::Gui::EndHorizontal();
    }

    void DrawTeamControlPanel()
    {
        const bool host = G->IsInGame && !G->IsClient;
        BeginPanel(LINE_ID);
        PanelHeader(G->Loc[LocKey_TEAM_CONTROL], G->Loc[LocKey_HostOnly]);
        {
            ToggleEx(LINE_ID, G->GodAllPlayers, G->Loc[LocKey_GodmodeAll], G->Loc[LocKey_HostOnly], {.Disabled = !host});
            HorizontalLine(1_px);
            ToggleEx(LINE_ID, G->GodAllPlayersNoTumble, G->Loc[LocKey_AntiTumbleAll], G->Loc[LocKey_HostOnly], {.Disabled = !host});

            const float gap = 5_px;
            const float half = (Hax::Gui::GetContentRegionAvail().X - gap) / 2.f;
            Hax::Gui::BeginHorizontal(gap);
            if (Button(LINE_ID, G->Loc[LocKey_HealAllPlayers], {}, {.Enabled = host, .MinW = half}))
                G->PlayerGodAction = PlayerGodCommand::HealAll;
            if (Button(LINE_ID, G->Loc[LocKey_ReviveAllPlayers], {}, {.Enabled = host, .MinW = half}))
                G->PlayerGodAction = PlayerGodCommand::ReviveAll;
            Hax::Gui::EndHorizontal();

            if (Button(LINE_ID, G->Loc[LocKey_GatherAllPlayers], {}, {.Enabled = host, .MinW = Hax::Gui::GetContentRegionAvail().X}))
                G->PlayerGodAction = PlayerGodCommand::GatherAll;

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
        PanelHeader(G->Loc[LocKey_WORLD_CONTROL]);
        {
            Hax::char16 value[32]{};
            swprintf_s(value, L"%d%%", G->WorldTimeScalePercent);
            SliderEx(LINE_ID, G->Loc[LocKey_TimeScale], value, &G->WorldTimeScalePercent, 0, 300, SliderConvertInt);
            HorizontalLine(1_px);
            ToggleEx(LINE_ID, G->WorldGravityOverride, G->Loc[LocKey_GravityOverride]);
            swprintf_s(value, L"%d", G->WorldGravityY);
            SliderEx(LINE_ID, G->Loc[LocKey_GravityY], value, &G->WorldGravityY, -30, 30, SliderConvertInt);
            HorizontalLine(1_px);
            if (ToggleEx(LINE_ID, G->WorldFreezePhysics, G->Loc[LocKey_FreezePhysics]))
                G->WorldFreezePhysicsChanged = true;

            const float gap = 5_px;
            const float third = (Hax::Gui::GetContentRegionAvail().X - gap * 2.f) / 3.f;
            Hax::Gui::BeginHorizontal(gap);
            if (Button(LINE_ID, G->Loc[LocKey_Blackout], {}, {.Enabled = inGame, .MinW = third}))
                G->WorldLightAction = WorldLightCommand::Blackout;
            if (Button(LINE_ID, G->Loc[LocKey_FullBrightWorld], {}, {.Enabled = inGame, .MinW = third}))
                G->WorldLightAction = WorldLightCommand::FullBright;
            if (Button(LINE_ID, G->Loc[LocKey_RestoreLighting], {}, {.Enabled = inGame, .MinW = third}))
                G->WorldLightAction = WorldLightCommand::Restore;
            Hax::Gui::EndHorizontal();
        }
        EndPanel();
    }

    void DrawEnemyDirectorPanel()
    {
        const bool host = G->IsInGame && !G->IsClient;
        BeginPanel(LINE_ID);
        PanelHeader(G->Loc[LocKey_ENEMY_DIRECTOR], G->Loc[LocKey_HostOnly]);
        {
            if (ToggleEx(LINE_ID, G->GodEnemiesPacified, G->Loc[LocKey_PacifyEnemies], G->Loc[LocKey_HostOnly], {.Disabled = !host}))
                G->GodEnemiesPacifiedChanged = true;
            HorizontalLine(1_px);
            const bool frozenBefore = G->GodEnemiesFrozen;
            if (ToggleEx(LINE_ID, G->GodEnemiesFrozen, G->Loc[LocKey_FreezeEnemies], G->Loc[LocKey_HostOnly], {.Disabled = !host}) && frozenBefore != G->GodEnemiesFrozen)
                G->GodEnemiesFreezeChanged = true;

            if (Button(LINE_ID, G->Loc[LocKey_GatherEnemies], {}, {.Enabled = host, .MinW = Hax::Gui::GetContentRegionAvail().X}))
                G->EnemyGodAction = EnemyGodCommand::Gather;

            const float gap = 5_px;
            const float half = (Hax::Gui::GetContentRegionAvail().X - gap) / 2.f;
            Hax::Gui::BeginHorizontal(gap);
            if (Button(LINE_ID, G->Loc[LocKey_KillAllEnemies], {}, {.Enabled = host, .MinW = half}))
                G->EnemyGodAction = EnemyGodCommand::KillAll;
            if (Button(LINE_ID, G->Loc[LocKey_DeleteAllEnemies], {}, {.Enabled = host, .MinW = half}))
                G->EnemyGodAction = EnemyGodCommand::DeleteAll;
            Hax::Gui::EndHorizontal();
        }
        EndPanel();
    }

    void DrawLootControlPanel()
    {
        const bool host = G->IsInGame && !G->IsClient;
        BeginPanel(LINE_ID);
        PanelHeader(G->Loc[LocKey_LOOT_CONTROL], G->Loc[LocKey_HostOnly]);
        {
            const bool frozenBefore = G->GodLootFrozen;
            if (ToggleEx(LINE_ID, G->GodLootFrozen, G->Loc[LocKey_FreezeLoot], G->Loc[LocKey_HostOnly], {.Disabled = !host}) && frozenBefore != G->GodLootFrozen)
                G->GodLootFreezeChanged = true;

            const float gap = 5_px;
            const float half = (Hax::Gui::GetContentRegionAvail().X - gap) / 2.f;
            Hax::Gui::BeginHorizontal(gap);
            if (Button(LINE_ID, G->Loc[LocKey_LootToPlayer], {}, {.Enabled = host, .MinW = half}))
                G->LootGodAction = LootGodCommand::BringToPlayer;
            if (Button(LINE_ID, G->Loc[LocKey_LootToTruck], {}, {.Enabled = host, .MinW = half}))
                G->LootGodAction = LootGodCommand::BringToTruck;
            Hax::Gui::EndHorizontal();
            Hax::Gui::BeginHorizontal(gap);
            if (Button(LINE_ID, G->Loc[LocKey_LootToExtraction], {}, {.Enabled = host, .MinW = half}))
                G->LootGodAction = LootGodCommand::BringToExtraction;
            if (Button(LINE_ID, G->Loc[LocKey_DiscoverLoot], {}, {.Enabled = host, .MinW = half}))
                G->LootGodAction = LootGodCommand::DiscoverAll;
            Hax::Gui::EndHorizontal();

            Hax::char16 value[32]{};
            swprintf_s(value, L"%d%%", G->GodLootValuePercent);
            SliderEx(LINE_ID, G->Loc[LocKey_ValueMultiplier], value, &G->GodLootValuePercent, 0, 1000, SliderConvertInt);
            if (Button(LINE_ID, G->Loc[LocKey_ApplyValueMultiplier], {}, {.Enabled = host, .MinW = Hax::Gui::GetContentRegionAvail().X}))
                G->LootGodAction = LootGodCommand::ApplyValueMultiplier;
        }
        EndPanel();
    }

    static void DrawObjectManipulationPanel(bool inGame)
    {
        BeginPanel(LINE_ID);
        PanelHeader(G->Loc[LocKey_TELEKINESIS]);
        {
            const bool hasTarget = inGame && G->GodTargetObjectValid;
            if (Button(LINE_ID, G->Loc[LocKey_HoldRelease], G->GodTelekinesisActive ? G->Loc[LocKey_Yes] : G->Loc[LocKey_No],
                {.Enabled = hasTarget, .MinW = Hax::Gui::GetContentRegionAvail().X}))
                G->GodObjectAction = GodObjectCommand::ToggleTelekinesis;

            Hax::char16 value[32]{};
            swprintf_s(value, G->Loc[LocKey_Meters].Data(), G->GodHoldDistance);
            SliderEx(LINE_ID, G->Loc[LocKey_HoldDistance], value, &G->GodHoldDistance, 1, 25, SliderConvertInt);
            swprintf_s(value, L"%d", G->GodThrowForce);
            SliderEx(LINE_ID, G->Loc[LocKey_ThrowForce], value, &G->GodThrowForce, 5, 150, SliderConvertInt);

            TwoButtons(G->Loc[LocKey_PullObject], G->Loc[LocKey_PushObject], hasTarget,
                GodObjectCommand::Pull, GodObjectCommand::Push);
            TwoButtons(G->Loc[LocKey_FreezeObject], G->Loc[LocKey_DuplicateObject], hasTarget,
                GodObjectCommand::FreezeToggle, GodObjectCommand::Duplicate);

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
            swprintf_s(value, L"%d°", G->GodRotationX);
            SliderEx(LINE_ID, G->Loc[LocKey_RotationX], value, &G->GodRotationX, -180, 180, SliderConvertInt);
            swprintf_s(value, L"%d°", G->GodRotationY);
            SliderEx(LINE_ID, G->Loc[LocKey_RotationY], value, &G->GodRotationY, -180, 180, SliderConvertInt);
            swprintf_s(value, L"%d°", G->GodRotationZ);
            SliderEx(LINE_ID, G->Loc[LocKey_RotationZ], value, &G->GodRotationZ, -180, 180, SliderConvertInt);

            if (Button(LINE_ID, G->Loc[LocKey_ApplyTransform], {}, {.Enabled = hasTarget, .MinW = Hax::Gui::GetContentRegionAvail().X}))
                G->GodObjectAction = GodObjectCommand::ApplyTransform;
            TwoButtons(G->Loc[LocKey_DeleteTarget], G->Loc[LocKey_ClearTarget], hasTarget && !G->IsClient,
                GodObjectCommand::DeleteTarget, GodObjectCommand::ClearTarget);

            Hax::char16 undo[64]{};
            swprintf_s(undo, G->Loc[LocKey_UndoActionFmt].Data(), G->GodUndoCount);
            if (Button(LINE_ID, undo, {}, {.Enabled = G->GodUndoCount > 0, .MinW = Hax::Gui::GetContentRegionAvail().X}))
                G->GodObjectAction = GodObjectCommand::Undo;
        }
        EndPanel();
    }

    void DrawGodToolsTab()
    {
        const Hax::Vector2 mainAreaSize = Hax::Gui::GetContentRegionAvail();
        const float spacing = 20_px;
        const Hax::Vector2 columnSize = {(mainAreaSize.X - spacing * 3.f) / 2.f, mainAreaSize.Y};
        const bool inGame = G->IsInGame;

        Hax::Gui::Space(spacing);
        Hax::Gui::BeginContainer(Hax::Hash("GodToolsColumnLeft"), {.W = columnSize.X, .FitY = true});
        Hax::Gui::BeginVertical(spacing);
        Hax::Gui::Dummy({});
        {
            BeginPanel(LINE_ID);
            PanelHeader(G->Loc[LocKey_TARGET_INSPECTOR]);
            {
                ToggleEx(LINE_ID, G->GodTargetingEnabled, G->Loc[LocKey_TargetingEnabled]);
                HorizontalLine(1_px);

                StatusLine(G->Loc[LocKey_AimedObject], G->GodAimedObjectName,
                    G->GodAimedObjectValid ? 0xFFD36EFF : 0x8D96A8FF);
                StatusLine(G->Loc[LocKey_LockedObject], G->GodTargetName,
                    G->GodTargetObjectValid ? 0x7EE787FF : 0x8D96A8FF);
                Hax::WStringView authority = !G->GodTargetObjectValid ? Hax::WStringView(L"-") :
                    (G->GodTargetNetworked ? G->Loc[LocKey_NetworkObject] : G->Loc[LocKey_LocalObject]);
                StatusLine(G->Loc[LocKey_ObjectAuthority], authority,
                    G->GodTargetNetworked ? 0x65B8FFFF : 0xC2C8D4FF);

                if (G->GodTargetObjectValid)
                {
                    Hax::char16 position[96]{};
                    swprintf_s(position, L"%.1f, %.1f, %.1f", G->GodTargetPosition.X,
                        G->GodTargetPosition.Y, G->GodTargetPosition.Z);
                    StatusLine(G->Loc[LocKey_ObjectPosition], position);

                    Hax::char16 layer[24]{};
                    swprintf_s(layer, L"%d", G->GodTargetLayer);
                    StatusLine(G->Loc[LocKey_ObjectLayer], layer);
                }

                HorizontalLine(1_px);
                TwoButtons(G->Loc[LocKey_LockTarget], G->Loc[LocKey_ClearTarget],
                    inGame, GodObjectCommand::LockAimed, GodObjectCommand::ClearTarget);
                HotkeyEx(Hax::Hash(L"GodTargetHotkeyEditor"), G->VkLockGodTarget, G->Loc[LocKey_Hotkey], G->Loc[LocKey_LockTarget]);
            }
            EndPanel();

            BeginPanel(LINE_ID);
            PanelHeader(G->Loc[LocKey_TRANSFORM_GIZMO]);
            {
                const bool hasTarget = inGame && G->GodTargetObjectValid;
                ToggleEx(LINE_ID, G->GodGizmoVisible, G->Loc[LocKey_GizmoVisible]);
                if (!hasTarget)
                    G->GodGizmoEditMode = false;
                if (ToggleEx(LINE_ID, G->GodGizmoEditMode, G->Loc[LocKey_GizmoEditMode],
                    G->Loc[LocKey_GizmoEditModeDesc], {.Disabled = !hasTarget}) && G->GodGizmoEditMode)
                {
                    // Enter the scene editor immediately instead of leaving the menu on
                    // top of the handles and relying on a second hotkey press.
                    ToggleMenuVisibility();
                }

                const float gap = 5_px;
                const float third = (Hax::Gui::GetContentRegionAvail().X - gap * 2.f) / 3.f;
                Hax::Gui::BeginHorizontal(gap);
                if (Button(LINE_ID, G->Loc[LocKey_GizmoMove],
                    G->GodGizmoModeCurrent == GodGizmoMode::Move ? L"●" : L"", {.Enabled = hasTarget, .MinW = third}))
                    G->GodGizmoModeCurrent = GodGizmoMode::Move;
                if (Button(LINE_ID, G->Loc[LocKey_GizmoRotate],
                    G->GodGizmoModeCurrent == GodGizmoMode::Rotate ? L"●" : L"", {.Enabled = hasTarget, .MinW = third}))
                    G->GodGizmoModeCurrent = GodGizmoMode::Rotate;
                if (Button(LINE_ID, G->Loc[LocKey_GizmoScale],
                    G->GodGizmoModeCurrent == GodGizmoMode::Scale ? L"●" : L"", {.Enabled = hasTarget, .MinW = third}))
                    G->GodGizmoModeCurrent = GodGizmoMode::Scale;
                Hax::Gui::EndHorizontal();

                const float half = (Hax::Gui::GetContentRegionAvail().X - gap) / 2.f;
                Hax::Gui::BeginHorizontal(gap);
                if (Button(LINE_ID, G->Loc[LocKey_GizmoWorld],
                    G->GodGizmoSpaceCurrent == GodGizmoSpace::World ? L"●" : L"", {.Enabled = hasTarget, .MinW = half}))
                    G->GodGizmoSpaceCurrent = GodGizmoSpace::World;
                if (Button(LINE_ID, G->Loc[LocKey_GizmoLocal],
                    G->GodGizmoSpaceCurrent == GodGizmoSpace::Local ? L"●" : L"", {.Enabled = hasTarget, .MinW = half}))
                    G->GodGizmoSpaceCurrent = GodGizmoSpace::Local;
                Hax::Gui::EndHorizontal();
                DescLabel(G->Loc[LocKey_GizmoControls]);
            }
            EndPanel();

        }
        Hax::Gui::Dummy({});
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();

        Hax::Gui::Space(spacing);
        Hax::Gui::BeginContainer(Hax::Hash("GodToolsColumnRight"), {.W = columnSize.X, .FitY = true});
        Hax::Gui::BeginVertical(spacing);
        Hax::Gui::Dummy({});
        {
            DrawObjectManipulationPanel(inGame);
        }
        Hax::Gui::Dummy({});
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();
    }
}
