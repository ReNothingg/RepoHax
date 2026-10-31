#include <src/pch.h>

#include <src/cheat.h>

#include "menu/menu.h"
#include "menu/theme.h"
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
            auto isGameplayHotkey = [](int vk)
            {
                return G->VkToggleFlight == vk || G->VkToggleEyeLasers == vk ||
                    G->VkTeleportPlayerToCamera == vk || G->VkSpawnItem == vk ||
                    G->VkDeleteObject == vk || G->VkLockGodTarget == vk || G->VkReviveSelf == vk ||
                    G->VkDroneLaunch == vk || G->VkDroneDetonate == vk || G->VkDroneFpv == vk ||
                    G->VkOrbitalStrike == vk || G->VkBlackHole == vk;
            };

            if (!isGameplayHotkey('W') && GizmoKeyJustPressed('W')) G->GodGizmoModeCurrent = GodGizmoMode::Move;
            if (!isGameplayHotkey('E') && GizmoKeyJustPressed('E')) G->GodGizmoModeCurrent = GodGizmoMode::Rotate;
            if (!isGameplayHotkey('R') && GizmoKeyJustPressed('R')) G->GodGizmoModeCurrent = GodGizmoMode::Scale;

            const Hax::Vector2 boxMin = Hax::Gui::GetViewportCenter() + Hax::Vector2(-155_px, -Hax::Gui::GetViewportSize().Y / 2.f + 18_px);
            const Hax::Vector2 boxMax = boxMin + Hax::Vector2(310_px, 34_px);
            Hax::Gui::DrawRect(boxMin, boxMax, {.BorderColor = Theme::Line2, .BorderTh = 1_px, .FillColor = 0x101215E6, .Rounding = 10_px});
            const wchar_t* mode = G->GodGizmoModeCurrent == GodGizmoMode::Move ? L"MOVE [W]" :
                (G->GodGizmoModeCurrent == GodGizmoMode::Rotate ? L"ROTATE [E]" : L"SCALE [R]");
            Hax::Gui::DrawString(G->NunitoSans_Bold, mode, boxMin + Hax::Vector2(14_px, 9_px), 13_px, {.Color = Theme::Accent});
            Hax::Gui::DrawString(G->NunitoSans_SemiBold, L"LMB drag · Shift precision · Esc menu",
                boxMin + Hax::Vector2(105_px, 10_px), 11_px, {.Color = Theme::Muted});
        }
    }

    static bool IsKeyJustDown(size_t id, Hax::uint8 vk)
    {
        return (G->KeyListenerId == 0 || G->KeyListenerId == id) && !IsTextInputActive() && Hax::Gui::IsKeyJustDown(vk);
    }

    static void DrawHudText(Hax::Gui::FontHandle font, Hax::WStringView text, Hax::Vector2 pos, float size, Hax::Gui::Color color)
    {
        // A dark copy underneath keeps the text readable on any background.
        Hax::Gui::DrawString(font, text, pos + Hax::Vector2(1_px, 1_px), size, {.Color = 0x000000C0});
        Hax::Gui::DrawString(font, text, pos, size, {.Color = color});
    }

    static void DrawHudTextCentered(Hax::Gui::FontHandle font, Hax::WStringView text, Hax::Vector2 center, float size, Hax::Gui::Color color)
    {
        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(font, text, size);
        DrawHudText(font, text, center - textSize / 2.f, size, color);
    }

    // Drone FPV feed: scanlines, static that grows with distance from the pilot,
    // telemetry, and a "signal lost" screen when the drone goes up in smoke.
    static void RenderDroneFpvHud(uint64_t now)
    {
        if (!G->FpvActive || G->MenuVisible)
            return;

        const Hax::Vector2 size = Hax::Gui::GetViewportSize();
        static uint32_t s_Rng = 0x1234567u;
        auto random = [&](uint32_t range)
        {
            s_Rng ^= s_Rng << 13;
            s_Rng ^= s_Rng >> 17;
            s_Rng ^= s_Rng << 5;
            return range ? s_Rng % range : 0u;
        };

        const bool lost = G->FpvSignalLost;
        const float noise = lost ? 1.f : Hax::Clamp(1.f - G->FpvSignal, 0.f, 1.f);

        if (lost)
            Hax::Gui::DrawRect({}, size, {.FillColor = 0x1A1A1AE0});

        for (float y = 0.f; y < size.Y; y += 4_px)
            Hax::Gui::DrawRect({0.f, y}, {size.X, y + 1_px}, {.FillColor = 0x00000038});

        const int specks = (int)(80.f + noise * (lost ? 2400.f : 900.f));
        for (int i = 0; i < specks; ++i)
        {
            const float x = (float)random((uint32_t)size.X);
            const float y = (float)random((uint32_t)size.Y);
            const float w = (float)(1 + random(lost ? 6u : 3u)) * 1_px;
            const uint32_t shade = 0x60 + random(0x9F);
            const uint32_t alpha = 0x30 + (uint32_t)(noise * 0x90);
            Hax::Gui::DrawRect({x, y}, {x + w, y + 1_px * (float)(1 + random(2))},
                {.FillColor = (shade << 24) | (shade << 16) | (shade << 8) | alpha});
        }

        const Hax::Vector2 center = Hax::Gui::GetViewportCenter();
        if (lost)
        {
            if ((now / 400) % 2 == 0)
                DrawHudTextCentered(G->NunitoSans_ExtraBold, G->Loc[LocKey_FpvSignalLost], center, 42_px, 0xFF4D55FF);
            return;
        }

        const Hax::Gui::Color hud = 0xE8F4E8E6;
        const Hax::Gui::Color warn = 0xFF4D55FF;

        // Corner brackets
        const float margin = 34_px;
        const float arm = 46_px;
        const Hax::Vector2 corners[4] = {{margin, margin}, {size.X - margin, margin}, {margin, size.Y - margin}, {size.X - margin, size.Y - margin}};
        for (int i = 0; i < 4; ++i)
        {
            const float sx = (i % 2 == 0) ? 1.f : -1.f;
            const float sy = (i < 2) ? 1.f : -1.f;
            Hax::Gui::DrawLine(corners[i], corners[i] + Hax::Vector2(arm * sx, 0.f), {.FillColor = hud, .Th = 2_px});
            Hax::Gui::DrawLine(corners[i], corners[i] + Hax::Vector2(0.f, arm * sy), {.FillColor = hud, .Th = 2_px});
        }

        // Crosshair
        const float gap = 6_px;
        const float len = 16_px;
        const Hax::Gui::Color crossCol = G->FpvArming ? warn : hud;
        Hax::Gui::DrawLine(center + Hax::Vector2(-len - gap, 0.f), center + Hax::Vector2(-gap, 0.f), {.FillColor = crossCol, .Th = 2_px});
        Hax::Gui::DrawLine(center + Hax::Vector2(gap, 0.f), center + Hax::Vector2(len + gap, 0.f), {.FillColor = crossCol, .Th = 2_px});
        Hax::Gui::DrawLine(center + Hax::Vector2(0.f, -len - gap), center + Hax::Vector2(0.f, -gap), {.FillColor = crossCol, .Th = 2_px});
        Hax::Gui::DrawLine(center + Hax::Vector2(0.f, gap), center + Hax::Vector2(0.f, len + gap), {.FillColor = crossCol, .Th = 2_px});

        // REC + drone count
        if ((now / 500) % 2 == 0)
            Hax::Gui::DrawCircle({margin + 18_px, margin + 26_px}, 6_px, {.FillColor = warn});
        Hax::char16 line[96]{};
        swprintf_s(line, L"REC  FPV  %d", G->DronesInFlight);
        DrawHudText(G->NunitoSans_Bold, line, {margin + 32_px, margin + 16_px}, 18_px, hud);

        // Signal bars, top right
        const int bars = (int)std::ceil(Hax::Clamp(G->FpvSignal, 0.f, 1.f) * 5.f);
        for (int i = 0; i < 5; ++i)
        {
            const float h = (float)(i + 1) * 5_px;
            const Hax::Vector2 base = {size.X - margin - 80_px + (float)i * 11_px, margin + 40_px};
            Hax::Gui::DrawRect({base.X, base.Y - h}, {base.X + 7_px, base.Y},
                {.FillColor = i < bars ? (bars <= 1 ? warn : hud) : 0xFFFFFF30});
        }

        // Telemetry, bottom left
        const int seconds = (int)G->FpvFlightTime;
        swprintf_s(line, L"SPD %3.0f m/s", G->FpvSpeed);
        DrawHudText(G->NunitoSans_Bold, line, {margin + 10_px, size.Y - margin - 92_px}, 17_px, hud);
        swprintf_s(line, L"DST %3.0f m", G->FpvDistance);
        DrawHudText(G->NunitoSans_Bold, line, {margin + 10_px, size.Y - margin - 70_px}, 17_px, hud);
        swprintf_s(line, L"RNG %3.0f m", G->FpvRange);
        DrawHudText(G->NunitoSans_Bold, line, {margin + 10_px, size.Y - margin - 48_px}, 17_px, G->FpvSignal < 0.3f ? warn : hud);
        swprintf_s(line, L"T+ %02d:%02d", seconds / 60, seconds % 60);
        DrawHudText(G->NunitoSans_Bold, line, {margin + 10_px, size.Y - margin - 26_px}, 17_px, hud);

        if (G->FpvArming && (now / 150) % 2 == 0)
            DrawHudTextCentered(G->NunitoSans_ExtraBold, L"IMPACT", center + Hax::Vector2(0.f, 60_px), 34_px, warn);

        if (G->FpvPiloting)
            DrawHudTextCentered(G->NunitoSans_SemiBold, G->Loc[LocKey_FpvThrottleHint],
                {center.X, size.Y - margin - 20_px}, 15_px, 0xE8F4E8B0);
    }

    static void RenderChaosBanner(uint64_t now)
    {
        if (now >= G->ChaosBannerUntil || G->ChaosBanner[0] == 0)
            return;
        const Hax::WStringView text = G->ChaosBanner;
        const float fontSize = 26_px;
        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(G->NunitoSans_ExtraBold, text, fontSize);
        const Hax::Vector2 center = {Hax::Gui::GetViewportCenter().X, 90_px};
        const Hax::Vector2 pad = {22_px, 12_px};
        Hax::Gui::DrawRect(center - textSize / 2.f - pad, center + textSize / 2.f + pad,
            {.BorderColor = Theme::Accent, .BorderTh = 2_px, .FillColor = 0x101215E6, .Rounding = 12_px});
        DrawHudTextCentered(G->NunitoSans_ExtraBold, text, center, fontSize, Theme::Accent);
    }

    void RenderOverlay()
    {
        // The menu key stays live while typing in search so the menu can always be closed.
        if ((G->KeyListenerId == 0 || G->KeyListenerId == HAX_LINE) && Hax::Gui::IsKeyJustDown((Hax::uint8)G->VkOpenClose))
            ToggleMenuVisibility();

        constexpr size_t reviveSelfHotkeyId = Hax::Hash(L"ReviveSelfHotkeyAction");
        if (G->VkReviveSelf != 0 && G->ReviveSelfSessionReady && !G->MenuVisible &&
            ::GetForegroundWindow() == G->GameWndHandle &&
            IsKeyJustDown(reviveSelfHotkeyId, (Hax::uint8)G->VkReviveSelf))
        {
            G->ReviveSelfRequested = true;
        }

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

        constexpr size_t droneLaunchHotkeyId = Hax::Hash(L"DroneLaunchHotkeyAction");
        if (G->VkDroneLaunch != 0 &&
            G->IsInGame &&
            !G->IsClient &&
            !G->MenuVisible &&
            IsKeyJustDown(droneLaunchHotkeyId, (Hax::uint8)G->VkDroneLaunch))
        {
            G->DroneLaunchRequested = true;
        }

        const bool funHotkeysLive = G->IsInGame && !G->IsClient && !G->MenuVisible;

        constexpr size_t droneFpvHotkeyId = Hax::Hash(L"DroneFpvHotkeyAction");
        if (G->VkDroneFpv != 0 && funHotkeysLive && IsKeyJustDown(droneFpvHotkeyId, (Hax::uint8)G->VkDroneFpv))
            G->DroneFpvEnabled = !G->DroneFpvEnabled;

        constexpr size_t orbitalHotkeyId = Hax::Hash(L"OrbitalHotkeyAction");
        if (G->VkOrbitalStrike != 0 && funHotkeysLive && IsKeyJustDown(orbitalHotkeyId, (Hax::uint8)G->VkOrbitalStrike))
            G->OrbitalRequested = true;

        constexpr size_t blackHoleHotkeyId = Hax::Hash(L"BlackHoleHotkeyAction");
        if (G->VkBlackHole != 0 && funHotkeysLive && IsKeyJustDown(blackHoleHotkeyId, (Hax::uint8)G->VkBlackHole))
            G->BlackHoleRequested = true;

        constexpr size_t droneDetonateHotkeyId = Hax::Hash(L"DroneDetonateHotkeyAction");
        if (G->VkDroneDetonate != 0 &&
            G->IsInGame &&
            G->DronesInFlight > 0 &&
            !G->MenuVisible &&
            IsKeyJustDown(droneDetonateHotkeyId, (Hax::uint8)G->VkDroneDetonate))
        {
            G->DroneDetonateRequested = true;
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
        const uint64_t hudNow = ::GetTickCount64();
        RenderDroneFpvHud(hudNow);
        RenderChaosBanner(hudNow);
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
