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

        constexpr size_t deleteObjectHotkeyId = Hax::Hash(L"DeleteObjectHotkeyAction");
        if (G->VkDeleteObject != 0 &&
            G->ObjectRemoverEnabled &&
            G->IsInGame &&
            !G->IsClient &&
            !G->MenuVisible &&
            IsKeyJustDown(deleteObjectHotkeyId, (Hax::uint8)G->VkDeleteObject))
        {
            G->DeleteObjectRequested = true;
        }

        Hax::Gui::SwitchLayer(L"Background");
        RenderEsp();
        if (G->ObjectRemoverEnabled && G->IsInGame && !G->MenuVisible)
        {
            const Hax::Vector2 center = Hax::Gui::GetViewportCenter();
            const Hax::Gui::Color color = G->ObjectRemoverTargetValid ? 0xFF4D55FF : 0x7C8492FF;
            constexpr float gap = 4.f;
            constexpr float length = 8.f;
            Hax::Gui::DrawLine(center + Hax::Vector2(-length, 0.f), center + Hax::Vector2(-gap, 0.f), {.FillColor = color, .Th = 2_px});
            Hax::Gui::DrawLine(center + Hax::Vector2(gap, 0.f), center + Hax::Vector2(length, 0.f), {.FillColor = color, .Th = 2_px});
            Hax::Gui::DrawLine(center + Hax::Vector2(0.f, -length), center + Hax::Vector2(0.f, -gap), {.FillColor = color, .Th = 2_px});
            Hax::Gui::DrawLine(center + Hax::Vector2(0.f, gap), center + Hax::Vector2(0.f, length), {.FillColor = color, .Th = 2_px});
        }
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
