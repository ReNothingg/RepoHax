#include <src/pch.h>

#include <src/cheat.h>

#include "menu/menu.h"
#include "esp/esp.h"

namespace Cheat
{
    struct GizmoPointerState
    {
        Hax::Vector2 Position{};
        Hax::Vector2 Delta{};
        bool Down;
        bool JustPressed;
        bool JustReleased;
    };

    static GizmoPointerState ReadGizmoPointer()
    {
        static Hax::Vector2 s_PreviousPosition{};
        static bool s_PreviousDown;
        static bool s_Initialized;

        POINT point{};
        if (!G->GetCursorPosHook.unsafe_stdcall<BOOL, LPPOINT>(&point))
            return {};
        ::ScreenToClient(G->GameWndHandle, &point);

        GizmoPointerState state{};
        state.Position = Hax::Vector2((float)point.x, (float)point.y);
        state.Down = (G->GetAsyncKeyStateHook.unsafe_stdcall<SHORT, int>(VK_LBUTTON) & 0x8000) != 0;
        if (s_Initialized)
        {
            state.Delta = state.Position - s_PreviousPosition;
            state.JustPressed = state.Down && !s_PreviousDown;
            state.JustReleased = !state.Down && s_PreviousDown;
        }

        s_PreviousPosition = state.Position;
        s_PreviousDown = state.Down;
        s_Initialized = true;
        return state;
    }

    static bool GizmoKeyJustPressed(int vk)
    {
        static bool s_Previous[256]{};
        const bool down = (G->GetAsyncKeyStateHook.unsafe_stdcall<SHORT, int>(vk) & 0x8000) != 0;
        const bool pressed = down && !s_Previous[vk & 0xFF];
        s_Previous[vk & 0xFF] = down;
        return pressed;
    }

    static float DistanceToSegment(const Hax::Vector2& point, const Hax::Vector2& a,
        const Hax::Vector2& b, Hax::Vector2* tangent = nullptr)
    {
        const Hax::Vector2 ab = b - a;
        const float lengthSq = ab.X * ab.X + ab.Y * ab.Y;
        if (lengthSq <= 0.001f)
            return (point - a).Length();
        const float t = Hax::Clamp(((point.X - a.X) * ab.X + (point.Y - a.Y) * ab.Y) / lengthSq, 0.f, 1.f);
        const Hax::Vector2 closest = a + ab * t;
        if (tangent)
            *tangent = ab / Hax::Sqrt(lengthSq);
        return (point - closest).Length();
    }

    static void RenderGodGizmo()
    {
        const bool interactive = G->GodGizmoEditMode && !G->MenuVisible;
        const GizmoPointerState pointer = ReadGizmoPointer();

        // Esc must remain available even when the selected object briefly leaves the
        // camera frustum; otherwise the hidden-menu edit session can trap the cursor.
        if (interactive && GizmoKeyJustPressed(VK_ESCAPE))
        {
            G->GodGizmoEditMode = false;
            G->GodGizmoDragEndRequested = true;
            ToggleMenuVisibility();
            return;
        }

        if (!G->GodGizmoVisible || !G->GodTargetObjectValid || !G->IsInGame)
        {
            G->GodGizmoEditMode = false;
            G->GodGizmoHoveredAxis = -1;
            if (G->GodGizmoActiveAxis >= 0)
            {
                G->GodGizmoDragEndRequested = true;
                G->GodGizmoActiveAxis = -1;
            }
            return;
        }

        if (!G->GodGizmoProjectionValid)
        {
            G->GodGizmoHoveredAxis = -1;
            if (G->GodGizmoActiveAxis >= 0)
            {
                G->GodGizmoDragEndRequested = true;
                G->GodGizmoActiveAxis = -1;
            }
            return;
        }

        constexpr Hax::Gui::Color axisColors[3] = {0xFF5D62FF, 0x62D982FF, 0x5A8DFFFF};
        constexpr Hax::Gui::Color hoverColor = 0xFFD866FF;
        const Hax::Vector2 mouse = interactive ? pointer.Position : Hax::Gui::GetMousePos();
        float bestDistance = 12_px;
        int bestAxis = -1;
        Hax::Vector2 bestDirection{};

        if (G->GodGizmoModeCurrent == GodGizmoMode::Rotate)
        {
            for (int axis = 0; axis < 3; ++axis)
            {
                for (int point = 0; point < GodGizmoRingSegments; ++point)
                {
                    if (!G->GodGizmoRingPointValid[axis][point] || !G->GodGizmoRingPointValid[axis][point + 1])
                        continue;
                    Hax::Vector2 tangent{};
                    const float distance = DistanceToSegment(mouse, G->GodGizmoRingPoints[axis][point],
                        G->GodGizmoRingPoints[axis][point + 1], &tangent);
                    if (interactive && distance < bestDistance)
                    {
                        bestDistance = distance;
                        bestAxis = axis;
                        bestDirection = tangent;
                    }
                }
            }

            for (int axis = 0; axis < 3; ++axis)
            {
                const bool highlighted = axis == bestAxis || axis == G->GodGizmoActiveAxis;
                const Hax::Gui::Color color = highlighted ? hoverColor : axisColors[axis];
                for (int point = 0; point < GodGizmoRingSegments; ++point)
                    if (G->GodGizmoRingPointValid[axis][point] && G->GodGizmoRingPointValid[axis][point + 1])
                        Hax::Gui::DrawLine(G->GodGizmoRingPoints[axis][point], G->GodGizmoRingPoints[axis][point + 1],
                            {.FillColor = color, .Th = highlighted ? 4_px : 2_px});
            }
        }
        else
        {
            for (int axis = 0; axis < 3; ++axis)
            {
                if (!G->GodGizmoScreenAxisValid[axis])
                    continue;
                Hax::Vector2 tangent{};
                const float distance = DistanceToSegment(mouse, G->GodGizmoScreenOrigin,
                    G->GodGizmoScreenAxes[axis], &tangent);
                if (interactive && distance < bestDistance)
                {
                    bestDistance = distance;
                    bestAxis = axis;
                    bestDirection = tangent;
                }
            }

            Hax::Gui::DrawCircle(G->GodGizmoScreenOrigin, 5_px, {.FillColor = 0xE6E9EFFF});
            for (int axis = 0; axis < 3; ++axis)
            {
                if (!G->GodGizmoScreenAxisValid[axis])
                    continue;
                const bool highlighted = axis == bestAxis || axis == G->GodGizmoActiveAxis;
                const Hax::Gui::Color color = highlighted ? hoverColor : axisColors[axis];
                const Hax::Vector2 end = G->GodGizmoScreenAxes[axis];
                Hax::Gui::DrawLine(G->GodGizmoScreenOrigin, end,
                    {.FillColor = color, .Th = highlighted ? 5_px : 3_px});
                if (G->GodGizmoModeCurrent == GodGizmoMode::Scale)
                    Hax::Gui::DrawRect(end - Hax::Vector2(5_px, 5_px), end + Hax::Vector2(5_px, 5_px),
                        {.FillColor = color, .Rounding = 2_px});
                else
                    Hax::Gui::DrawCircle(end, highlighted ? 7_px : 5_px, {.FillColor = color});
                const wchar_t* label = axis == 0 ? L"X" : (axis == 1 ? L"Y" : L"Z");
                Hax::Gui::DrawString(G->NunitoSans_Bold, label, end + Hax::Vector2(7_px, -10_px), 13_px,
                    {.Color = color});
            }
        }

        G->GodGizmoHoveredAxis = bestAxis;
        if (interactive && bestAxis >= 0)
            Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);

        if (interactive && pointer.JustPressed && bestAxis >= 0)
        {
            G->GodGizmoActiveAxis = bestAxis;
            G->GodGizmoDragScreenDirection = bestDirection;
            G->GodGizmoDragPixelsPending = 0.f;
            G->GodGizmoDragBeginRequested = true;
        }

        if (interactive && G->GodGizmoActiveAxis >= 0 && pointer.Down)
        {
            const Hax::Vector2 delta = pointer.Delta;
            float pixels = delta.X * G->GodGizmoDragScreenDirection.X + delta.Y * G->GodGizmoDragScreenDirection.Y;
            if ((G->GetAsyncKeyStateHook.unsafe_stdcall<SHORT, int>(VK_SHIFT) & 0x8000) != 0)
                pixels *= 0.2f;
            G->GodGizmoDragPixelsPending += pixels;
        }

        if (G->GodGizmoActiveAxis >= 0 && pointer.JustReleased)
        {
            G->GodGizmoDragEndRequested = true;
            G->GodGizmoActiveAxis = -1;
        }

        if (interactive)
        {
            if (GizmoKeyJustPressed('W')) G->GodGizmoModeCurrent = GodGizmoMode::Move;
            if (GizmoKeyJustPressed('E')) G->GodGizmoModeCurrent = GodGizmoMode::Rotate;
            if (GizmoKeyJustPressed('R')) G->GodGizmoModeCurrent = GodGizmoMode::Scale;

            const Hax::Vector2 boxMin = Hax::Gui::GetViewportCenter() + Hax::Vector2(-155_px, -Hax::Gui::GetViewportSize().Y / 2.f + 18_px);
            const Hax::Vector2 boxMax = boxMin + Hax::Vector2(310_px, 34_px);
            Hax::Gui::DrawRect(boxMin, boxMax, {.FillColor = 0x111722E8, .Rounding = 8_px});
            const wchar_t* mode = G->GodGizmoModeCurrent == GodGizmoMode::Move ? L"MOVE [W]" :
                (G->GodGizmoModeCurrent == GodGizmoMode::Rotate ? L"ROTATE [E]" : L"SCALE [R]");
            Hax::Gui::DrawString(G->NunitoSans_Bold, mode, boxMin + Hax::Vector2(14_px, 9_px), 13_px, {.Color = 0xFFFFFFFF});
            Hax::Gui::DrawString(G->NunitoSans_SemiBold, L"LMB drag · Shift precision · Esc menu",
                boxMin + Hax::Vector2(105_px, 10_px), 11_px, {.Color = 0x9DA8BAFF});
        }
    }

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

        constexpr size_t lockGodTargetHotkeyId = Hax::Hash(L"LockGodTargetHotkeyAction");
        if (G->VkLockGodTarget != 0 &&
            G->GodTargetingEnabled &&
            G->IsInGame &&
            !G->MenuVisible &&
            IsKeyJustDown(lockGodTargetHotkeyId, (Hax::uint8)G->VkLockGodTarget))
        {
            G->GodObjectAction = GodObjectCommand::LockAimed;
        }

        Hax::Gui::SwitchLayer(L"Background");
        RenderEsp();
        RenderGodGizmo();
        if ((G->ObjectRemoverEnabled || G->GodTargetingEnabled) && G->IsInGame && !G->MenuVisible)
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
