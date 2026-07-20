#pragma once

namespace Cheat
{
    void DrawStatsTab();
    void DrawEntitiesTab();
    void DrawVisionTab();
    void DrawValuablesTab();
    void DrawItemsTab();
    void DrawLevelTab();
    void DrawMiscTab();
    void DrawSettingsTab();
    void DrawCosmeticTab();
    void DrawGodToolsTab();

    // Shared feature panels. They live on the page that owns their domain,
    // instead of being duplicated across unrelated tabs.
    void DrawBalancePanel();
    void DrawWorldControlPanel();
    void DrawEnemyDirectorPanel();
    void DrawLootControlPanel();
    void DrawTeamControlPanel();
}
