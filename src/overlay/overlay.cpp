#include <src/pch.h>

#include <src/cheat.h>

#include "menu/menu.h"
#include "esp/esp.h"

namespace Cheat
{
    static bool IsKeyJustDown(size_t id, Hax::uint8 vk)
    {
        return (G->KeyListenerId == 0 || G->KeyListenerId == id) && Hax::Gui::IsKeyJustDown(vk);
    }

    void RenderOverlay()
    {
        if (IsKeyJustDown(HAX_LINE, G->VkOpenClose))
            ToggleMenuVisibility();

        constexpr size_t toggleFlightHotkeyId = Hax::Hash(L"ToggleFlightHotkeyAction");
        if (!G->IsInGame)
        {
            G->FlightEnabled = false;
        }
        else if (G->VkToggleFlight != 0 &&
                 IsKeyJustDown(toggleFlightHotkeyId, (Hax::uint8)G->VkToggleFlight))
        {
            G->FlightEnabled = !G->FlightEnabled;
        }

        constexpr size_t toggleEyeLasersHotkeyId = Hax::Hash(L"ToggleEyeLasersHotkeyAction");
        if (!G->IsInGame)
        {
            G->EyeLasersEnabled = false;
        }
        else if (G->VkToggleEyeLasers != 0 &&
                 IsKeyJustDown(toggleEyeLasersHotkeyId, (Hax::uint8)G->VkToggleEyeLasers))
        {
            G->EyeLasersEnabled = !G->EyeLasersEnabled;
        }

        constexpr size_t teleportPlayerToCameraHotkeyId = Hax::Hash(L"TeleportPlayerToCameraHotkeyAction");
        if (G->VkTeleportPlayerToCamera != 0 &&
            G->IsInGame &&
            IsKeyJustDown(teleportPlayerToCameraHotkeyId, (Hax::uint8)G->VkTeleportPlayerToCamera))
            G->TeleportAction = TeleportQuickAction::PlayerToCamera;

        constexpr size_t spawnItemHotkeyId = Hax::Hash(L"SpawnItemHotkeyAction");
        if (G->VkSpawnItem != 0 &&
            IsKeyJustDown(spawnItemHotkeyId, (Hax::uint8)G->VkSpawnItem) &&
            G->IsInGame &&
            !G->IsClient &&
            G->SelectedItem)
        {
            G->ItemToSpawn = G->SelectedItem;
        }

        Hax::Gui::SwitchLayer(L"Background");
        RenderEsp();
        if (G->MenuVisible && G->DarkenBackground)
            Hax::Gui::DrawRect(Hax::Vector2(), Hax::Gui::GetViewportSize(), {.FillColor = 0x000000E6});
        Hax::Gui::RestoreLayer();

        if (G->MenuVisible)
        {
            Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Arrow);
            RenderMenu();
        }
    }
}
