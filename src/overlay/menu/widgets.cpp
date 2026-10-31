#include <src/pch.h>

#include "widgets.h"

#include <src/cheat.h>

#include "theme.h"

#define DESC_FONT G->NunitoSans_SemiBold
#define MAIN_FONT G->NunitoSans_SemiBold
#define BOLD_FONT G->NunitoSans_Bold
#define HEADER_FONT G->NunitoSans_ExtraBold
#define ICONS_FONT G->Icons_Solid

namespace Cheat
{

    static Hax::Gui::Color Mix(Hax::Gui::Color a, Hax::Gui::Color b, float t)
    {
        return Hax::Lerp(Hax::Gui::LinearColor(a), Hax::Gui::LinearColor(b), t).ToColor();
    }

    static void AnimateHover(Hax::Gui::LinearAnim& anim, bool hovered, float speed = 0.1f)
    {
        const float dt = Hax::Gui::GetDeltaTime();
        anim.Elapse(hovered ? dt : -dt, speed);
    }

    //=========================================================================
    // Search highlight
    //=========================================================================

    static Hax::WStringView s_Highlight;
    static double s_HighlightStart = 0.0;
    static int s_ScrollDelayFrames = 0;
    static bool s_ScrollPending = false;

    void SetSearchHighlight(Hax::WStringView label)
    {
        s_Highlight = label;
        s_HighlightStart = Hax::Gui::GetTime();
        // Wait for the newly selected page to lay out before scrolling to it.
        s_ScrollDelayFrames = 2;
        s_ScrollPending = true;
    }

    void TickSearchHighlight()
    {
        if (s_ScrollDelayFrames > 0)
            --s_ScrollDelayFrames;
        else if (s_ScrollPending && Hax::Gui::GetTime() - s_HighlightStart > 1.0)
            s_ScrollPending = false;

        if (!s_Highlight.Empty() && Hax::Gui::GetTime() - s_HighlightStart > 3.0)
            s_Highlight = {};
    }

    static void DrawSearchHighlight(Hax::WStringView text, const Hax::Rect& bounds)
    {
        if (s_Highlight.Empty() || text.Size() != s_Highlight.Size() || !(text == s_Highlight))
            return;

        if (s_ScrollPending && s_ScrollDelayFrames == 0)
        {
            s_ScrollPending = false;
            Hax::Gui::ScrollYTo(bounds.Min.Y - 80_px);
            s_HighlightStart = Hax::Gui::GetTime();
        }

        const float t = (float)(Hax::Gui::GetTime() - s_HighlightStart);
        const float fade = Hax::Clamp(1.f - (t - 1.8f) / 1.f, 0.f, 1.f);
        const float pulse = 0.55f + 0.45f * (Hax::Sin(t * 7.f) * 0.5f + 0.5f);

        Hax::Gui::LinearColor ring(Theme::Accent);
        ring.A = fade * pulse;
        Hax::Gui::LinearColor fill(Theme::Accent);
        fill.A = fade * 0.12f;

        const Hax::Vector2 pad = {8_px, 5_px};
        Hax::Gui::DrawRect(bounds.Min - pad, bounds.Max + pad,
            {.BorderColor = ring.ToColor(), .BorderTh = 1.5_px, .FillColor = fill.ToColor(), .Rounding = 7_px});
    }

    //=========================================================================
    // Text
    //=========================================================================

    void Label(Hax::Gui::FontHandle hFont, Hax::WStringView text, float fontH, Hax::Gui::Color col)
    {
        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(hFont, text, fontH);

        Hax::Rect bounds;
        bounds.Min = Hax::Gui::GetCursorPos();
        bounds.Max = bounds.Min + textSize;

        Hax::Gui::PlaceItem(textSize);
        if (!Hax::Gui::IsItemVisible(bounds))
            return;

        DrawSearchHighlight(text, bounds);
        Hax::Gui::DrawString(hFont, text, bounds.Min, fontH, {.Color = col});
    }

    void MainLabel(Hax::WStringView text)
    {
        Label(MAIN_FONT, text, Hax::Gui::Scale(Theme::MainFontSize), Theme::Text);
    }

    void MainLabel(Hax::WStringView text, const Hax::Gui::Color& col)
    {
        Label(MAIN_FONT, text, Hax::Gui::Scale(Theme::MainFontSize), col);
    }

    Hax::Vector2 CalcMainLabelSize(Hax::WStringView text)
    {
        return Hax::Gui::CalcTextSize(MAIN_FONT, text, Hax::Gui::Scale(Theme::MainFontSize));
    }

    void HintLabel(Hax::WStringView text)
    {
        Label(DESC_FONT, text, Hax::Gui::Scale(11.5f), Theme::Faint);
    }

    static void TextTooltip(size_t id, Hax::WStringView text, const Hax::Rect& hoverBounds)
    {
        if (text.Empty())
            return;

        double& hoverStartedAt = Hax::Gui::GetState<double>(id);
        const Hax::Vector2 mouse = Hax::Gui::GetMousePos();
        const bool hovered = hoverBounds.Contains(mouse) && Hax::Gui::GetContainerBounds().Contains(mouse);
        if (!hovered)
        {
            hoverStartedAt = 0.0;
            return;
        }

        if (hoverStartedAt == 0.0)
            hoverStartedAt = Hax::Gui::GetTime();
        if (Hax::Gui::GetTime() - hoverStartedAt < 0.25)
            return;

        const float fontH = Hax::Gui::Scale(12.f);
        const Hax::Vector2 padding = {11_px, 8_px};
        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(DESC_FONT, text, fontH);
        const Hax::Vector2 size = textSize + padding * 2.f;
        const Hax::Vector2 viewport = Hax::Gui::GetViewportSize();
        Hax::Vector2 pos = mouse + Hax::Vector2(14_px, 18_px);
        pos.X = Hax::Clamp(pos.X, 8_px, Hax::Max(8_px, viewport.X - size.X - 8_px));
        pos.Y = Hax::Clamp(pos.Y, 8_px, Hax::Max(8_px, viewport.Y - size.Y - 8_px));

        Hax::Gui::SwitchLayer(L"Popups");
        Hax::Gui::DrawRect(pos + Hax::Vector2(0.f, 4_px), pos + size + Hax::Vector2(0.f, 4_px),
            {.FillColor = 0x00000080, .Rounding = 8_px});
        Hax::Gui::DrawRect(pos, pos + size,
            {.BorderColor = Theme::Line2, .BorderTh = 1_px, .FillColor = Theme::Surface2, .Rounding = 8_px});
        Hax::Gui::DrawString(DESC_FONT, text, pos + padding, fontH, {.Color = Theme::Text});
        Hax::Gui::RestoreLayer();
    }

    void LabelAlignedByH(Hax::Gui::FontHandle hFont, Hax::WStringView label, float fontH, const Hax::Gui::Color& col, float h)
    {
        const Hax::Gui::FontMetrics metrics = Hax::Gui::GetFontMetrics(hFont, fontH);

        Hax::Gui::BeginVertical();
        {
            Hax::Gui::Space((h - metrics.LineHeight) / 2.f);
            Label(hFont, label, fontH, col);
        }
        Hax::Gui::EndVertical();
    }

    void MainLabelAlignedByH(Hax::WStringView label, float h)
    {
        LabelAlignedByH(MAIN_FONT, label, Hax::Gui::Scale(Theme::MainFontSize), Theme::Text, h);
    }

    //=========================================================================
    // Window and side panel
    //=========================================================================

    void BeginWindow()
    {
        constexpr size_t WINDOW_ID = Hax::Hash("MainWindow");
        const Hax::Vector2 viewportSize = Hax::Gui::GetViewportSize();
        const float viewportMargin = Hax::Gui::Scale(12.f);
        Hax::Vector2 windowSize = Hax::Gui::Scale(Theme::WindowSize);
        windowSize.X = Hax::Min(windowSize.X, Hax::Max(1.f, viewportSize.X - viewportMargin * 2.f));
        windowSize.Y = Hax::Min(windowSize.Y, Hax::Max(1.f, viewportSize.Y - viewportMargin * 2.f));

        const Hax::Vector2 viewportCenter = Hax::Gui::GetViewportCenter();
        Hax::Vector2& posOffset = Hax::Gui::GetState<Hax::Vector2>(WINDOW_ID);

        const Hax::Vector2 centeredPos = viewportCenter - windowSize / 2.f;
        Hax::Rect dragBounds = Hax::Rect::FromPosSize(centeredPos + posOffset, windowSize);

        Hax::Gui::Interact(WINDOW_ID, dragBounds);
        if (Hax::Gui::IsItemActive(WINDOW_ID))
            posOffset += Hax::Gui::GetMouseDeltaPos();

        Hax::Vector2 windowPos = centeredPos + posOffset;
        windowPos.X = Hax::Clamp(windowPos.X, viewportMargin, viewportSize.X - windowSize.X - viewportMargin);
        windowPos.Y = Hax::Clamp(windowPos.Y, viewportMargin, viewportSize.Y - windowSize.Y - viewportMargin);
        posOffset = windowPos - centeredPos;

        const Hax::Rect windowBounds = Hax::Rect::FromPosSize(windowPos, windowSize);
        const float r = Hax::Gui::Scale(Theme::WindowR);

        // Soft drop shadow, then the window body.
        Hax::Gui::DrawRect(windowBounds.Min + Hax::Vector2(0.f, 10_px), windowBounds.Max + Hax::Vector2(0.f, 10_px),
            {.FillColor = 0x00000066, .Rounding = r});

        Hax::Gui::SetCursorPos(windowBounds.Min);
        Hax::Gui::BeginContainer(WINDOW_ID, {.W = windowSize.X, .H = windowSize.Y, .Clip = true});
        Hax::Gui::BeginHorizontal();

        Hax::Gui::DrawRect(windowBounds.Min, windowBounds.Max,
            {.BorderColor = Theme::Line2, .BorderTh = 1_px, .FillColor = Theme::Bg, .Rounding = r});
    }

    void EndWindow()
    {
        Hax::Gui::EndHorizontal();
        Hax::Gui::EndContainer();
    }

    bool BeginSidePanel()
    {
        const Hax::Vector2 size = {Hax::Gui::Scale(Theme::SidePanelWidth), Hax::Gui::GetContentRegionAvail().Y};

        constexpr size_t id = Hax::Hash("SidePanel");
        Hax::Gui::BeginContainer(id, {.W = size.X, .H = size.Y});

        Hax::Rect bounds;
        bounds.Min = Hax::Gui::GetCursorPos();
        bounds.Max = bounds.Min + size;

        if (!Hax::Gui::IsItemVisible(bounds))
        {
            Hax::Gui::EndContainer();
            return false;
        }

        const float r = Hax::Gui::Scale(Theme::WindowR);
        Hax::Gui::DrawRect(bounds.Min + Hax::Vector2(1_px, 1_px), {bounds.Max.X, bounds.Max.Y - 1_px},
            {.FillColor = Theme::Rail, .Rounding = Hax::Vector4(r, 0.f, 0.f, r)});
        Hax::Gui::DrawLine({bounds.Max.X - 0.5_px, bounds.Min.Y + 1_px}, {bounds.Max.X - 0.5_px, bounds.Max.Y - 1_px},
            {.FillColor = Theme::Line, .Th = 1_px});

        Hax::Gui::BeginVertical();
        return true;
    }

    void EndSidePanel()
    {
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();
    }

    //=========================================================================
    // Navigation
    //=========================================================================

    // Last time the search field had keyboard focus. Compared against the clock so a
    // hidden menu (which stops rendering) never leaves hotkeys blocked.
    static double s_TextInputTime = -1.0;

    bool IsTextInputActive()
    {
        return s_TextInputTime >= 0.0 && Hax::Gui::GetTime() - s_TextInputTime < 0.15;
    }

    bool SearchField(size_t id, wchar_t* buf, size_t bufSize, Hax::WStringView hint, float width)
    {
        const Hax::Vector2 size = {width, 34_px};
        const Hax::Rect bounds = Hax::Rect::FromPosSize(Hax::Gui::GetCursorPos(), size);
        Hax::Gui::PlaceItem(size);
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;

        const bool focused = Hax::Gui::IsItemFocused(id);
        Hax::Gui::DrawRect(bounds.Min, bounds.Max,
            {.BorderColor = focused ? Theme::Accent : Theme::Line, .BorderTh = 1_px, .FillColor = Theme::Surface1, .Rounding = 8_px});

        const float iconH = 12_px;
        const Hax::Vector2 iconSize = Hax::Gui::CalcTextSize(ICONS_FONT, L"", iconH);
        Hax::Gui::DrawString(ICONS_FONT, L"", {bounds.Min.X + 11_px, bounds.Min.Y + (size.Y - iconSize.Y) / 2.f}, iconH,
            {.Color = focused ? Theme::Muted : Theme::Faint});

        const float fontH = Hax::Gui::Scale(12.5f);
        bool changed = false;
        const bool hasText = buf[0] != L'\0';
        const float clearW = hasText ? 22_px : 0.f;
        const Hax::Rect editBounds = {{bounds.Min.X + 31_px, bounds.Min.Y + 8_px}, {bounds.Max.X - 8_px - clearW, bounds.Max.Y - 8_px}};

        Hax::Gui::PushClipRect(editBounds);
        changed = Hax::Gui::StringEdit(id, MAIN_FONT, buf, bufSize, fontH, editBounds,
            {.CaretColor = Theme::Accent, .Hint = hint, .HintColor = Theme::Faint, .TextColor = Theme::Text});
        Hax::Gui::PopClipRect();

        if (Hax::Gui::IsItemHovered(id))
            Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_TextInput);

        if (Hax::Gui::IsItemFocused(id))
            s_TextInputTime = Hax::Gui::GetTime();

        if (Hax::Gui::IsItemFocused(id) && Hax::Gui::IsKeyJustDown(VK_ESCAPE) && buf[0] != L'\0')
        {
            buf[0] = L'\0';
            changed = true;
        }

        if (hasText)
        {
            const size_t clearId = id + 1;
            const Hax::Rect clearBounds = {{bounds.Max.X - 8_px - clearW, bounds.Min.Y + 6_px}, {bounds.Max.X - 6_px, bounds.Max.Y - 6_px}};
            Hax::Gui::Interact(clearId, clearBounds);
            const bool hovered = Hax::Gui::IsItemHovered(clearId);
            if (hovered)
                Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);
            const float xH = 11_px;
            const Hax::Vector2 xSize = Hax::Gui::CalcTextSize(ICONS_FONT, L"", xH);
            Hax::Gui::DrawString(ICONS_FONT, L"", clearBounds.GetCenter() - xSize / 2.f, xH,
                {.Color = hovered ? Theme::Text : Theme::Faint});
            if (Hax::Gui::IsItemClicked(clearId))
            {
                buf[0] = L'\0';
                changed = true;
            }
        }

        return changed;
    }

    bool NavItem(size_t id, Hax::WStringView text, Hax::char16 icon16, bool active, bool chevron)
    {
        const float fontH = Hax::Gui::Scale(Theme::MainFontSize);
        const Hax::Vector2 size = {Hax::Gui::GetContentRegionAvail().X, 36_px};
        const Hax::Rect bounds = Hax::Rect::FromPosSize(Hax::Gui::GetCursorPos(), size);

        Hax::Gui::PlaceItem(size);
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;

        const auto res = Hax::Gui::Interact(id, bounds);
        Hax::Gui::LinearAnim& hover = Hax::Gui::GetState<Hax::Gui::LinearAnim>(id);
        AnimateHover(hover, res.Hovered);
        if (res.Hovered)
            Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);

        if (active)
            Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = Theme::Surface2, .Rounding = 8_px});
        else if (hover.Progress > 0.f)
        {
            Hax::Gui::LinearColor bg = Theme::Surface1;
            bg.A = hover.Progress;
            Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = bg.ToColor(), .Rounding = 8_px});
        }

        const float iconH = 14_px;
        const Hax::WStringView icon = {&icon16, 1};
        const Hax::Vector2 iconSize = Hax::Gui::CalcTextSize(ICONS_FONT, icon, iconH);
        Hax::Gui::DrawString(ICONS_FONT, icon, {bounds.Min.X + 10_px + (18_px - iconSize.X) / 2.f, bounds.Min.Y + (size.Y - iconSize.Y) / 2.f}, iconH,
            {.Color = active ? Theme::Accent : Mix(Theme::Faint, Theme::Muted, hover.Progress)});

        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(BOLD_FONT, text, fontH);
        Hax::Gui::DrawString(BOLD_FONT, text, {bounds.Min.X + 38_px, bounds.Min.Y + (size.Y - textSize.Y) / 2.f}, fontH,
            {.Color = active ? Theme::Text : Mix(Theme::Muted, Theme::Text, hover.Progress)});

        if (chevron)
        {
            const float chevH = 9_px;
            const Hax::Vector2 chevSize = Hax::Gui::CalcTextSize(ICONS_FONT, L"", chevH);
            Hax::Gui::DrawString(ICONS_FONT, L"", {bounds.Max.X - 12_px - chevSize.X, bounds.Min.Y + (size.Y - chevSize.Y) / 2.f}, chevH,
                {.Color = Theme::Faint});
        }

        return Hax::Gui::IsItemClicked(id);
    }

    bool NavSubItem(size_t id, Hax::WStringView text, bool active)
    {
        const float fontH = Hax::Gui::Scale(12.5f);
        const Hax::Vector2 size = {Hax::Gui::GetContentRegionAvail().X, 30_px};
        const Hax::Rect bounds = Hax::Rect::FromPosSize(Hax::Gui::GetCursorPos(), size);

        Hax::Gui::PlaceItem(size);
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;

        const auto res = Hax::Gui::Interact(id, bounds);
        Hax::Gui::LinearAnim& hover = Hax::Gui::GetState<Hax::Gui::LinearAnim>(id);
        AnimateHover(hover, res.Hovered && !active);
        if (res.Hovered)
            Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);

        if (hover.Progress > 0.f)
        {
            Hax::Gui::LinearColor bg = Theme::Surface1;
            bg.A = hover.Progress;
            Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = bg.ToColor(), .Rounding = 8_px});
        }

        if (active)
            Hax::Gui::DrawCircle({bounds.Min.X + 23_px, bounds.GetCenter().Y}, 2.5_px, {.FillColor = Theme::Accent});

        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(MAIN_FONT, text, fontH);
        Hax::Gui::DrawString(MAIN_FONT, text, {bounds.Min.X + 36_px, bounds.Min.Y + (size.Y - textSize.Y) / 2.f}, fontH,
            {.Color = active ? Theme::Accent : Mix(Theme::Muted, Theme::Text, hover.Progress)});

        return Hax::Gui::IsItemClicked(id);
    }

    bool SearchResultItem(size_t id, Hax::WStringView text, Hax::WStringView where)
    {
        const float fontH = Hax::Gui::Scale(12.5f);
        const float subH = Hax::Gui::Scale(11.f);
        const Hax::Vector2 size = {Hax::Gui::GetContentRegionAvail().X, 42_px};
        const Hax::Rect bounds = Hax::Rect::FromPosSize(Hax::Gui::GetCursorPos(), size);

        Hax::Gui::PlaceItem(size);
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;

        const auto res = Hax::Gui::Interact(id, bounds);
        Hax::Gui::LinearAnim& hover = Hax::Gui::GetState<Hax::Gui::LinearAnim>(id);
        AnimateHover(hover, res.Hovered);
        if (res.Hovered)
            Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);

        Hax::Gui::DrawRect(bounds.Min, bounds.Max,
            {.FillColor = Mix(Theme::Rail, Theme::Surface2, hover.Progress), .Rounding = 8_px});

        Hax::Gui::PushClipRect(bounds);
        Hax::Gui::DrawString(MAIN_FONT, text, {bounds.Min.X + 10_px, bounds.Min.Y + 6_px}, fontH, {.Color = Theme::Text});
        Hax::Gui::DrawString(DESC_FONT, where, {bounds.Min.X + 10_px, bounds.Min.Y + 23_px}, subH, {.Color = Theme::Faint});
        Hax::Gui::PopClipRect();

        return Hax::Gui::IsItemClicked(id);
    }

    void Badge(Hax::WStringView text, Hax::Gui::Color fg)
    {
        const float fontH = Hax::Gui::Scale(10.f);
        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(HEADER_FONT, text, fontH);
        const Hax::Vector2 size = {textSize.X + 16_px, 20_px};
        const Hax::Rect bounds = Hax::Rect::FromPosSize(Hax::Gui::GetCursorPos(), size);
        Hax::Gui::PlaceItem(size);
        if (!Hax::Gui::IsItemVisible(bounds))
            return;

        Hax::Gui::LinearColor bg(fg);
        bg.A = 0.13f;
        Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = bg.ToColor(), .Rounding = size.Y / 2.f});
        Hax::Gui::DrawString(HEADER_FONT, text, bounds.Min + (size - textSize) / 2.f, fontH, {.Color = fg});
    }

    Hax::Vector2 CalcBadgeSize(Hax::WStringView text)
    {
        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(HEADER_FONT, text, Hax::Gui::Scale(10.f));
        return {textSize.X + 16_px, 20_px};
    }

    static void DrawKeyCap(const Hax::Rect& bounds, Hax::Gui::Color border)
    {
        Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = border, .Rounding = 5_px});
        Hax::Gui::DrawRect(bounds.Min + Hax::Vector2(1_px, 1_px), bounds.Max - Hax::Vector2(1_px, 2_px),
            {.FillColor = Theme::Surface2, .Rounding = 4_px});
    }

    void KeyCap(Hax::WStringView text)
    {
        const float fontH = Hax::Gui::Scale(11.f);
        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(BOLD_FONT, text, fontH);
        const Hax::Vector2 size = {Hax::Max(22_px, textSize.X + 12_px), 22_px};
        const Hax::Rect bounds = Hax::Rect::FromPosSize(Hax::Gui::GetCursorPos(), size);
        Hax::Gui::PlaceItem(size);
        if (!Hax::Gui::IsItemVisible(bounds))
            return;

        DrawKeyCap(bounds, Theme::Line2);
        Hax::Gui::DrawString(BOLD_FONT, text, {bounds.Min.X + (size.X - textSize.X) / 2.f, bounds.Min.Y + (size.Y - 1_px - textSize.Y) / 2.f}, fontH,
            {.Color = Theme::Muted});
    }

    bool IconButton(size_t id, Hax::char16 icon16, Hax::WStringView tooltip)
    {
        const Hax::Vector2 size = {32_px, 32_px};
        const Hax::Rect bounds = Hax::Rect::FromPosSize(Hax::Gui::GetCursorPos(), size);
        Hax::Gui::PlaceItem(size);
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;

        const auto res = Hax::Gui::Interact(id, bounds);
        Hax::Gui::LinearAnim& hover = Hax::Gui::GetState<Hax::Gui::LinearAnim>(id);
        AnimateHover(hover, res.Hovered);
        if (res.Hovered)
            Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);

        Hax::Gui::DrawRect(bounds.Min, bounds.Max,
            {.BorderColor = Theme::Line2, .BorderTh = 1_px, .FillColor = Mix(Theme::Surface1, Theme::Surface3, hover.Progress), .Rounding = 8_px});
        const float iconH = 13_px;
        const Hax::WStringView icon = {&icon16, 1};
        const Hax::Vector2 iconSize = Hax::Gui::CalcTextSize(ICONS_FONT, icon, iconH);
        Hax::Gui::DrawString(ICONS_FONT, icon, bounds.GetCenter() - iconSize / 2.f, iconH,
            {.Color = Mix(Theme::Muted, Theme::Text, hover.Progress)});
        TextTooltip(id ^ 0x49434F4E42544E00ull, tooltip, bounds);

        return Hax::Gui::IsItemClicked(id);
    }

    void StatusDot(Hax::Gui::Color col, float h)
    {
        const Hax::Vector2 size = {7_px, h};
        const Hax::Rect bounds = Hax::Rect::FromPosSize(Hax::Gui::GetCursorPos(), size);
        Hax::Gui::PlaceItem(size);
        if (Hax::Gui::IsItemVisible(bounds))
            Hax::Gui::DrawCircle(bounds.GetCenter(), 3.5_px, {.FillColor = col});
    }

    //=========================================================================
    // Cards
    //=========================================================================

    void PanelHeader(Hax::WStringView text, Hax::WStringView desc)
    {
        const float fontH = Hax::Gui::Scale(Theme::TitleFontSize);
        const Hax::Vector2 pos = Hax::Gui::GetCursorPos();
        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(HEADER_FONT, text, fontH);
        Label(HEADER_FONT, text, fontH, Theme::Muted);
        TextTooltip(Hax::Hash(text) ^ 0x50414E454C484452ull, desc, Hax::Rect::FromPosSize(pos, textSize));
    }

    void BeginPanel(size_t id)
    {
        Hax::Gui::BeginContainer(id, {.FitY = true});
        const Hax::Vector2 containerSize = Hax::Gui::GetContentRegionAvail();
        if (containerSize.Y > 0.f)
        {
            const Hax::Vector2 a = Hax::Gui::GetCursorPos();
            const Hax::Vector2 b = a + containerSize;
            Hax::Gui::DrawRect(a, b,
                {.BorderColor = Theme::Line, .BorderTh = 1_px, .FillColor = Theme::Surface1, .Rounding = Hax::Gui::Scale(Theme::CardR)});
        }
        else
            Hax::Gui::PushSkipDrawing();

        Hax::Gui::BeginHorizontal();
        const float padding = 16_px;
        Hax::Gui::Space(padding);
        Hax::Gui::BeginContainer(id + 1, {.W = containerSize.X - padding * 2.f, .H = 15.f, .FitY = true});
        Hax::Gui::BeginVertical(12_px);
        Hax::Gui::Dummy({0.f, 2_px});
    }

    void EndPanel()
    {
        Hax::Gui::Dummy({0.f, 2_px});
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();
        Hax::Gui::EndHorizontal();

        if (Hax::Gui::IsDrawingSkipped())
            Hax::Gui::PopSkipDrawing();

        Hax::Gui::EndContainer();
    }

    void HorizontalLine(float th, float w)
    {
        Hax::Gui::HorizontalLine(th, Theme::Line, w);
    }

    //=========================================================================
    // Controls
    //=========================================================================

    bool SettingsBtn(size_t id, bool disabled)
    {
        const float h = Hax::Gui::Scale(Theme::SettingBtnSize);
        const Hax::Vector2 size = Hax::Gui::CalcTextSize(ICONS_FONT, L"", h);
        const Hax::Rect bounds = Hax::Rect::FromPosSize(Hax::Gui::GetCursorPos(), size);

        Hax::Gui::PlaceItem(size);
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;

        if (!disabled)
            Hax::Gui::Interact(id, bounds);
        Hax::Gui::LinearAnim& state = Hax::Gui::GetState<Hax::Gui::LinearAnim>(id);
        AnimateHover(state, Hax::Gui::IsItemHovered(id));
        if (Hax::Gui::IsItemHovered(id))
            Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);

        const Hax::Gui::Color color = Hax::Gui::IsItemActive(id) ? Theme::Accent : Mix(Theme::Faint, Theme::Text, state.Progress);
        Hax::Gui::DrawString(ICONS_FONT, L"", bounds.Min, h, {.Color = color});

        return Hax::Gui::IsItemClicked(id);
    }

    struct ToggleState
    {
        Hax::Gui::LinearAnim HoverAnim;
        Hax::Gui::LinearAnim ToggleAnim;
    };

    bool Toggle(size_t id, bool& value, bool enabled)
    {
        const Hax::Vector2 toggleSize = Hax::Gui::Scale(Theme::ToggleSize);

        Hax::Rect bounds;
        bounds.Min = Hax::Gui::GetCursorPos();
        bounds.Max = bounds.Min + toggleSize;

        Hax::Gui::PlaceItem(toggleSize);
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;

        ToggleState& state = Hax::Gui::GetState<ToggleState>(id);
        const float deltaTime = Hax::Gui::GetDeltaTime();
        state.ToggleAnim.Elapse(value ? deltaTime : -deltaTime, 0.12f);

        if (enabled)
            Hax::Gui::Interact(id, bounds);

        AnimateHover(state.HoverAnim, Hax::Gui::IsItemHovered(id));
        if (Hax::Gui::IsItemHovered(id))
            Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);

        if (Hax::Gui::IsItemClicked(id))
            value = !value;

        const float t = state.ToggleAnim.Progress;
        const Hax::Gui::Color offTrack = Mix(Theme::Surface3, 0x343A42FF, state.HoverAnim.Progress);
        Hax::Gui::DrawRect(bounds.Min, bounds.Max,
            {.FillColor = Mix(offTrack, Theme::Accent, t), .Rounding = toggleSize.Y / 2.f});

        const float knobR = 7_px;
        const float knobX = Hax::Lerp(bounds.Min.X + toggleSize.Y / 2.f, bounds.Max.X - toggleSize.Y / 2.f, t);
        Hax::Gui::DrawCircle({knobX, bounds.GetCenter().Y}, knobR, {.FillColor = Mix(Theme::Faint, Theme::Ink, t)});

        return Hax::Gui::IsItemClicked(id);
    }

    void OpenPopup(size_t id, const Hax::Vector2& pos);

    bool ToggleEx(size_t id, bool& value, Hax::WStringView text, Hax::WStringView desc, ToggleExParams params)
    {
        bool changed = false;
        const float px8 = 8_px;
        const Hax::Vector2 toggleSize = Hax::Gui::Scale(Theme::ToggleSize);

        Hax::Gui::BeginVertical();
        {
            Hax::Gui::BeginHorizontal(px8);
            {
                const Hax::Vector2 labelPos = Hax::Gui::GetCursorPos();
                const Hax::Vector2 labelSize = Hax::Gui::CalcTextSize(MAIN_FONT, text, Hax::Gui::Scale(Theme::MainFontSize));
                LabelAlignedByH(MAIN_FONT, text, Hax::Gui::Scale(Theme::MainFontSize), Theme::Text, toggleSize.Y);
                TextTooltip(id ^ 0x544F47474C454849ull, desc,
                    Hax::Rect::FromPosSize(labelPos, {labelSize.X, toggleSize.Y}));
                float space = Hax::Gui::GetContentRegionAvail().X - toggleSize.X;

                if (params.SettingsId != 0)
                {
                    const float settingSize = Hax::Gui::Scale(Theme::SettingBtnSize);
                    Hax::Gui::Space(space - px8 - settingSize);
                    Hax::Gui::BeginVertical(3_px);
                    {
                        Hax::Gui::Space((toggleSize.Y - settingSize) / 2.f);
                        if (SettingsBtn(id + 1, params.Disabled))
                            OpenPopup(params.SettingsId, Hax::Gui::GetCursorPos());
                    }
                    Hax::Gui::EndVertical();
                    space = 0.f;
                }

                Hax::Gui::Space(space);
                changed = Toggle(id, value, !params.Disabled);

                if (params.Disabled)
                {
                    Hax::Rect bounds = Hax::Gui::GetLayoutBounds();
                    Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = Theme::DisabledMask});
                }
            }
            Hax::Gui::EndHorizontal();
        }
        Hax::Gui::EndVertical();

        return changed;
    }

    static constexpr Hax::Vector2 kButtonPadding = {12.f, 8.f};

    Hax::Vector2 CalcButtonSize(Hax::WStringView label)
    {
        const float fontH = Hax::Gui::Scale(Theme::DescFontSize);
        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(MAIN_FONT, label, fontH);
        return textSize + Hax::Gui::Scale(kButtonPadding) * 2.f;
    }

    float CalcButtonHeight()
    {
        const float fontH = Hax::Gui::Scale(Theme::DescFontSize);
        return Hax::Gui::GetFontLineHeight(MAIN_FONT, fontH) + Hax::Gui::Scale(kButtonPadding.Y) * 2.f;
    }

    struct ButtonColors
    {
        Hax::Gui::Color Bg, Fg, Border;
    };

    static ButtonColors ButtonBeh(size_t id, ButtonStyle style)
    {
        Hax::Gui::LinearAnim& state = Hax::Gui::GetState<Hax::Gui::LinearAnim>(id);
        const bool hovered = Hax::Gui::IsItemHovered(id);
        AnimateHover(state, hovered);
        if (hovered)
            Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);

        const bool pressed = Hax::Gui::IsItemActive(id);
        const float t = state.Progress;

        switch (style)
        {
        case ButtonStyle::Primary:
            return {pressed ? 0xD98F1CFF : Mix(Theme::Accent, 0xFFB940FF, t), Theme::Ink, Hax::Gui::Color::Transparent};
        case ButtonStyle::Danger:
            return {pressed ? 0xFF7A6E40 : Mix(Theme::DangerSoft, 0xFF7A6E2A, t), Theme::Danger, 0xFF7A6E59};
        case ButtonStyle::Ghost:
            return {pressed ? Theme::Surface3 : Mix(0x1B1F2400, Theme::Surface2, t), Mix(Theme::Muted, Theme::Text, t), Hax::Gui::Color::Transparent};
        default:
            return {pressed ? Theme::AccentSoft : Mix(Theme::Surface2, Theme::Surface3, t), pressed ? Theme::Accent : Theme::Text, Theme::Line2};
        }
    }

    bool Button(size_t id, Hax::WStringView label, Hax::WStringView desc, const ButtonParams& params)
    {
        const float fontH = Hax::Gui::Scale(Theme::DescFontSize);
        const Hax::Vector2 padding = Hax::Gui::Scale(kButtonPadding);

        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(MAIN_FONT, label, fontH);
        const Hax::Vector2 btnSize = {Hax::Max(textSize.X + padding.X * 2.f, params.MinW), textSize.Y + padding.Y * 2.f};

        Hax::Rect bounds;
        bounds.Min = Hax::Gui::GetCursorPos();
        bounds.Max = bounds.Min + btnSize;

        Hax::Gui::PlaceItem(bounds.GetSize());
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;

        if (params.Enabled)
            Hax::Gui::Interact(id, bounds);

        const ButtonColors c = ButtonBeh(id, params.Style);
        Hax::Gui::DrawRect(bounds.Min, bounds.Max,
            {.BorderColor = c.Border, .BorderTh = 1_px, .FillColor = c.Bg, .Rounding = Hax::Gui::Scale(Theme::ControlR)});
        const Hax::Vector2 labelPos = bounds.Min + (btnSize - textSize) / 2.f;
        DrawSearchHighlight(label, bounds);
        Hax::Gui::DrawString(MAIN_FONT, label, labelPos, fontH, {.Color = c.Fg});
        TextTooltip(id ^ 0x425554544F4E4849ull, desc, bounds);

        if (!params.Enabled)
            Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = Theme::DisabledMask, .Rounding = Hax::Gui::Scale(Theme::ControlR)});

        return Hax::Gui::IsItemClicked(id);
    }

    void Image(Hax::Gui::TextureHandle tex, Hax::Vector2 customSize, float rounding)
    {
        const Hax::Vector2 size = (bool)customSize ? customSize : Hax::Gui::GetImageSize(tex);

        Hax::Rect bounds;
        bounds.Min = Hax::Gui::GetCursorPos();
        bounds.Max = bounds.Min + size;

        Hax::Gui::PlaceItem(size);
        if (!Hax::Gui::IsItemVisible(bounds))
            return;

        Hax::Gui::DrawImage(tex, bounds.Min, bounds.Max, {.R = rounding});
    }

    int SliderConvertInt(int min, int max, float m)
    {
        return Hax::Clamp(min + (int)Hax::Round((float)(max - min) * m), min, max);
    }

    float SliderConvertFloat(float min, float max, float m)
    {
        return Hax::Lerp(min, max, Hax::Clamp(m, 0.f, 1.f));
    }

    void DrawSliderVisual(const Hax::Rect& bounds, float t, float activeProgress)
    {
        const float r = bounds.GetHeight() / 2.f;
        const float minX = bounds.Min.X + r;
        const float maxX = bounds.Max.X - r;
        const float thumbX = Hax::Lerp(minX, maxX, Hax::Clamp(t, 0.f, 1.f));
        const float cy = bounds.GetCenter().Y;
        const float trackH = 4_px;

        Hax::Gui::DrawRect({bounds.Min.X, cy - trackH / 2.f}, {bounds.Max.X, cy + trackH / 2.f},
            {.FillColor = Theme::Surface3, .Rounding = trackH / 2.f});
        Hax::Gui::DrawRect({bounds.Min.X, cy - trackH / 2.f}, {thumbX, cy + trackH / 2.f},
            {.FillColor = Theme::Accent, .Rounding = trackH / 2.f});

        const float outer = Hax::Lerp(7.5_px, 8.5_px, activeProgress);
        Hax::Gui::DrawCircle({thumbX, cy}, outer + 2.5_px, {.FillColor = Theme::Surface1});
        Hax::Gui::DrawCircle({thumbX, cy}, outer, {.FillColor = Theme::Accent});
        Hax::Gui::DrawCircle({thumbX, cy}, outer - 3_px, {.FillColor = Theme::Thumb});
    }

    void ValuePill(Hax::WStringView text)
    {
        const float fontH = Hax::Gui::Scale(11.5f);
        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(BOLD_FONT, text, fontH);
        const Hax::Vector2 size = textSize + Hax::Vector2(14_px, 4_px);
        const Hax::Rect bounds = Hax::Rect::FromPosSize(Hax::Gui::GetCursorPos(), size);
        Hax::Gui::PlaceItem(size);
        if (!Hax::Gui::IsItemVisible(bounds))
            return;

        Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = Theme::AccentSoft, .Rounding = 5_px});
        Hax::Gui::DrawString(BOLD_FONT, text, bounds.Min + (size - textSize) / 2.f, fontH, {.Color = Theme::Accent});
    }

    Hax::Vector2 CalcValuePillSize(Hax::WStringView text)
    {
        return Hax::Gui::CalcTextSize(BOLD_FONT, text, Hax::Gui::Scale(11.5f)) + Hax::Vector2(14_px, 4_px);
    }

    void SliderHeader(Hax::WStringView text, Hax::WStringView valTxt)
    {
        Hax::Gui::BeginHorizontal();
        {
            const Hax::Vector2 pillSize = CalcValuePillSize(valTxt);
            LabelAlignedByH(MAIN_FONT, text, Hax::Gui::Scale(Theme::MainFontSize), Theme::Text, pillSize.Y);
            Hax::Gui::Space(Hax::Max(0.f, Hax::Gui::GetContentRegionAvail().X - pillSize.X));
            ValuePill(valTxt);
        }
        Hax::Gui::EndHorizontal();
    }

    bool AutoLayoutButton(size_t id, Hax::WStringView text, const AutoLayoutButtonParams& params)
    {
        const float fontH = Hax::Gui::Scale(Theme::DescFontSize);
        const Hax::Vector2 padding = {12_px, 7_px};

        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(MAIN_FONT, text, fontH);
        Hax::Vector2 btnSize = textSize + padding * 2.f;

        Hax::Rect bounds;
        bounds.Min = Hax::Gui::GetCursorPos();
        bounds.Max = bounds.Min + btnSize;

        if (params.Icon != 0)
        {
            Hax::Vector2 iconSize = Hax::Gui::GetImageSize(params.Icon);
            bounds.Max.X += padding.X / 2.f + (textSize.Y * (iconSize.X / iconSize.Y));
            btnSize = bounds.GetSize();
        }

        if (btnSize.X > Hax::Gui::GetContentRegionAvail().X)
        {
            Hax::Gui::EndHorizontal();
            Hax::Gui::BeginHorizontal(6_px);

            bounds.Min = Hax::Gui::GetCursorPos();
            bounds.Max = bounds.Min + btnSize;
        }

        Hax::Gui::PlaceItem(btnSize);
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;

        if (!params.Disabled)
            Hax::Gui::Interact(id, bounds);

        const ButtonColors c = ButtonBeh(id, ButtonStyle::Secondary);
        Hax::Gui::DrawRect(bounds.Min, bounds.Max,
            {.BorderColor = c.Border, .BorderTh = 1_px, .FillColor = c.Bg, .Rounding = Hax::Gui::Scale(Theme::ControlR)});

        Hax::Vector2 drawPos = bounds.Min + padding;
        if (params.Icon != 0)
        {
            Hax::Vector2 iconSize = Hax::Gui::GetImageSize(params.Icon);

            Hax::Vector2 iconBR;
            iconBR.X = drawPos.X + textSize.Y * (iconSize.X / iconSize.Y);
            iconBR.Y = drawPos.Y + textSize.Y;
            Hax::Gui::DrawImage(params.Icon, drawPos, iconBR);

            drawPos.X = iconBR.X + padding.X / 2.f;
        }
        DrawSearchHighlight(text, bounds);
        Hax::Gui::DrawString(MAIN_FONT, text, drawPos, fontH, {.Color = c.Fg});

        if (params.Disabled)
            Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = Theme::DisabledMask, .Rounding = Hax::Gui::Scale(Theme::ControlR)});

        return Hax::Gui::IsItemClicked(id);
    }

    bool DropdownBtn(size_t id, Hax::WStringView preview, float w)
    {
        const float fontH = Hax::Gui::Scale(Theme::DescFontSize);
        const Hax::Vector2 padding = Hax::Gui::Scale(kButtonPadding);
        const float textH = Hax::Gui::GetFontLineHeight(MAIN_FONT, fontH);

        const Hax::Vector2 size = {w, textH + padding.Y * 2.f};
        Hax::Rect bounds; bounds.Min = Hax::Gui::GetCursorPos(); bounds.Max = bounds.Min + size;

        Hax::Gui::PlaceItem(size);
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;

        Hax::Gui::Interact(id, bounds);
        const ButtonColors c = ButtonBeh(id, ButtonStyle::Secondary);

        Hax::Gui::DrawRect(bounds.Min, bounds.Max,
            {.BorderColor = c.Border, .BorderTh = 1_px, .FillColor = c.Bg, .Rounding = Hax::Gui::Scale(Theme::ControlR)});
        Hax::Gui::DrawString(MAIN_FONT, preview, bounds.Min + padding, fontH, {.Color = Theme::Text});

        const float chevH = 10_px;
        const Hax::Vector2 chevSize = Hax::Gui::CalcTextSize(ICONS_FONT, L"", chevH);
        Hax::Gui::DrawString(ICONS_FONT, L"", {bounds.Max.X - padding.X - chevSize.X, bounds.Min.Y + (size.Y - chevSize.Y) / 2.f}, chevH,
            {.Color = Theme::Faint});

        return Hax::Gui::IsItemClicked(id);
    }

    struct PopupState { bool Opened, IsFirstFrame; Hax::Vector2 Pos; };

    void OpenPopup(size_t id, const Hax::Vector2& pos)
    {
        PopupState& state = Hax::Gui::GetState<PopupState>(id);
        state.Opened = state.IsFirstFrame = true;
        state.Pos = pos;
    }

    void ClosePopup(size_t id)
    {
        PopupState& state = Hax::Gui::GetState<PopupState>(id);
        state.Opened = false;
    }

    static Hax::Gui::ScrollStyle ListScrollStyle()
    {
        return
        {
            .TrackWidth = 6_px,
            .ThumbPadding = 1_px,
            .TrackCol = 0x0,
            .ThumbCol = Theme::Surface3,
            .ThumbHovCol = 0x3A414AFF,
            .ThumbActiveCol = Theme::Accent
        };
    }

    bool BeginDropList(size_t id, const Hax::Vector2& size)
    {
        PopupState& state = Hax::Gui::GetState<PopupState>(id);

        if (!state.Opened)
            return false;

        Hax::Gui::SwitchLayer(L"Popups");
        Hax::Gui::SetCursorPos(state.Pos + Hax::Vector2(0.f, 4_px));

        Hax::Rect bounds;
        bounds.Min = Hax::Gui::GetCursorPos();
        bounds.Max = bounds.Min + size;

        Hax::Gui::Interact(id, bounds);
        if (!state.IsFirstFrame && Hax::Gui::IsLmbJustPressed() && !bounds.Contains(Hax::Gui::GetMousePos()))
        {
            state.Opened = false;
            Hax::Gui::RestoreLayer();
            return false;
        }

        state.IsFirstFrame = false;

        const float r = 8_px;
        Hax::Gui::DrawRect(bounds.Min + Hax::Vector2(0.f, 6_px), bounds.Max + Hax::Vector2(0.f, 6_px), {.FillColor = 0x00000070, .Rounding = r});
        Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.BorderColor = Theme::Line2, .BorderTh = 1_px, .FillColor = Theme::Surface2, .Rounding = r});

        bounds.Min.Y += 4_px;
        bounds.Max.Y -= 4_px;

        Hax::Gui::SetCursorPos(bounds.Min);
        Hax::Gui::BeginContainer(id, {.W = size.X, .H = size.Y - 8_px, .Clip = true, .ScrollY = true, .Style = ListScrollStyle()});
        Hax::Gui::BeginVertical();

        return true;
    }

    void EndDropList()
    {
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();
        Hax::Gui::RestoreLayer();
    }

    bool BeginModal(size_t id)
    {
        PopupState& state = Hax::Gui::GetState<PopupState>(id);

        if (!state.Opened)
            return false;

        Hax::Gui::SwitchLayer(L"Popups");
        Hax::Gui::SetCursorPos(state.Pos);

        Hax::Gui::BeginContainer(id, {.FitX = true, .FitY = true});
        Hax::Rect bounds = Hax::Gui::GetContainerBounds();

        Hax::Gui::Interact(id + 1, Hax::Gui::GetViewportBounds());
        Hax::Gui::Interact(id, bounds);

        if (Hax::Gui::IsItemPressed(id + 1))
            state.Opened = false;

        if (Hax::Gui::IsItemActive(id))
            state.Pos += Hax::Gui::GetMouseDeltaPos();

        if (bounds.GetSize().X > 0)
        {
            const float r = 12_px;
            Hax::Gui::DrawRect(bounds.Min + Hax::Vector2(0.f, 8_px), bounds.Max + Hax::Vector2(0.f, 8_px), {.FillColor = 0x00000070, .Rounding = r});
            Hax::Gui::DrawRect(bounds.Min, bounds.Max,
                {.BorderColor = Theme::Line2, .BorderTh = 1_px, .FillColor = Theme::Surface1, .Rounding = r});
        }
        else
            Hax::Gui::PushSkipDrawing();

        return true;
    }

    void EndModal()
    {
        if (Hax::Gui::IsDrawingSkipped())
            Hax::Gui::PopSkipDrawing();

        Hax::Gui::EndContainer();
        Hax::Gui::RestoreLayer();
    }

    bool Selectable(size_t id, Hax::WStringView label, bool selected, SelectableParams params)
    {
        const float fontH = Hax::Gui::Scale(Theme::DescFontSize);
        const float markH = 10_px;
        const Hax::Vector2 padding = {10_px, 7_px};

        Hax::Vector2 markSize{};
        float markSpacing = 0.f;
        if (selected)
        {
            markSize = Hax::Gui::CalcTextSize(ICONS_FONT, L"", markH);
            markSpacing = padding.X;
        }

        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(MAIN_FONT, label, fontH);
        const Hax::Vector2 btnSize = {Hax::Max(textSize.X + padding.X * 2.f + markSpacing + markSize.X, params.MinW), textSize.Y + padding.Y * 2.f};

        Hax::Rect bounds;
        bounds.Min = Hax::Gui::GetCursorPos();
        bounds.Max = bounds.Min + btnSize;

        if (params.AutoLayout && btnSize.X > Hax::Gui::GetContentRegionAvail().X)
        {
            Hax::Gui::EndHorizontal();
            Hax::Gui::BeginHorizontal(6_px);

            bounds.Min = Hax::Gui::GetCursorPos();
            bounds.Max = bounds.Min + btnSize;
        }

        Hax::Gui::PlaceItem(bounds.GetSize());
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;

        if (!params.Disabled)
            Hax::Gui::Interact(id, bounds);

        const bool hovered = Hax::Gui::IsItemHovered(id);
        if (hovered)
            Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);

        // Lists inside drop-downs keep a flat look; free-standing chips get a frame.
        const float r = params.AutoLayout ? 8_px : 6_px;
        const Hax::Gui::Color bg = selected ? Theme::AccentSoft : (hovered ? Theme::Surface3 : (params.AutoLayout ? Theme::Surface2 : 0x00000000));
        const Hax::Gui::Color border = params.AutoLayout ? (selected ? 0xF5A52466 : Theme::Line) : Hax::Gui::Color::Transparent;
        Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.BorderColor = border, .BorderTh = 1_px, .FillColor = bg, .Rounding = r});
        Hax::Gui::DrawString(MAIN_FONT, label, bounds.Min + padding, fontH, {.Color = selected ? Theme::Accent : Theme::Text});
        if (selected)
        {
            Hax::Vector2 pos = {bounds.Max.X - padding.X - markSize.X, bounds.Min.Y + (btnSize.Y - markSize.Y) / 2.f};
            Hax::Gui::DrawString(ICONS_FONT, L"", pos, markH, {.Color = Theme::Accent});
        }

        if (params.Disabled)
            Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = Theme::DisabledMask, .Rounding = r});

        return Hax::Gui::IsItemClicked(id);
    }

    float CalcWidgetEqWidth(size_t nWidgets)
    {
        const float spacing = Hax::Gui::GetLayoutSpacing();
        return (Hax::Gui::GetContentRegionAvail().X - spacing * float(nWidgets - 1)) / (float)nWidgets;
    }

    Hax::Vector2 CalcRepeatBtnSize(Hax::WStringView label)
    {
        const float fontH = Hax::Gui::Scale(Theme::DescFontSize);
        const Hax::Vector2 padding = {9_px, 5_px};
        return Hax::Gui::CalcTextSize(MAIN_FONT, label, fontH) + padding * 2.f;
    }

    bool RepeatBtn(size_t id, Hax::WStringView label, bool enabled)
    {
        const float fontH = Hax::Gui::Scale(Theme::DescFontSize);
        const Hax::Vector2 padding = {9_px, 5_px};

        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(MAIN_FONT, label, fontH);
        const Hax::Vector2 btnSize = textSize + padding * 2.f;

        Hax::Rect bounds;
        bounds.Min = Hax::Gui::GetCursorPos();
        bounds.Max = bounds.Min + btnSize;

        Hax::Gui::PlaceItem(bounds.GetSize());
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;

        if (enabled)
            Hax::Gui::Interact(id, bounds);

        const ButtonColors c = ButtonBeh(id, ButtonStyle::Secondary);
        Hax::Gui::DrawRect(bounds.Min, bounds.Max,
            {.BorderColor = c.Border, .BorderTh = 1_px, .FillColor = c.Bg, .Rounding = 7_px});
        Hax::Gui::DrawString(MAIN_FONT, label, bounds.Min + (btnSize - textSize) / 2.f, fontH, {.Color = c.Fg});

        if (!enabled)
            Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = Theme::DisabledMask, .Rounding = 7_px});

        return Hax::Gui::IsItemPressedRepeat(id);
    }

    struct IntInputState
    {
        Hax::char16 Buffer[24]{};
        int SyncedValue;
        bool Initialized;
    };

    bool IntInput(size_t id, int& value, int min, int max, float width, bool enabled)
    {
        IntInputState& state = Hax::Gui::GetState<IntInputState>(id);
        const bool wasFocused = Hax::Gui::IsItemFocused(id);
        if (!state.Initialized || (!wasFocused && state.SyncedValue != value))
        {
            swprintf_s(state.Buffer, _countof(state.Buffer), L"%d", value);
            state.SyncedValue = value;
            state.Initialized = true;
        }

        const float fontH = Hax::Gui::Scale(Theme::DescFontSize);
        const Hax::Vector2 size = {width, CalcButtonHeight()};
        const Hax::Rect bounds = Hax::Rect::FromPosSize(Hax::Gui::GetCursorPos(), size);
        Hax::Gui::PlaceItem(size);
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;

        Hax::Gui::DrawRect(bounds.Min, bounds.Max,
            {.BorderColor = wasFocused ? Theme::Accent : Theme::Line2, .BorderTh = 1_px,
             .FillColor = Theme::Surface2, .Rounding = 7_px});

        bool changed = false;
        const Hax::Vector2 padding = {10_px, 5_px};
        const Hax::Rect editBounds = {bounds.Min + padding, bounds.Max - padding};
        if (enabled)
        {
            changed = Hax::Gui::StringEdit(id, BOLD_FONT, state.Buffer, _countof(state.Buffer), fontH, editBounds,
                {.CaretColor = Theme::Accent, .Hint = L"0", .HintColor = Theme::Faint, .TextColor = Theme::Text});

            const bool focused = Hax::Gui::IsItemFocused(id);
            if (!wasFocused && focused)
            {
                state.Buffer[0] = L'\0';
                changed = false;
            }

            if (changed)
            {
                size_t write = 0;
                for (size_t read = 0; state.Buffer[read] != L'\0'; ++read)
                {
                    const Hax::char16 c = state.Buffer[read];
                    if ((c >= L'0' && c <= L'9') || (c == L'-' && write == 0 && min < 0))
                        state.Buffer[write++] = c;
                }
                state.Buffer[write] = L'\0';

                Hax::char16* end = nullptr;
                const long long parsed = wcstoll(state.Buffer, &end, 10);
                if (end != state.Buffer)
                {
                    const int next = (int)Hax::Clamp<long long>(parsed, min, max);
                    changed = next != value;
                    value = next;
                    state.SyncedValue = value;
                }
                else
                    changed = false;
            }
        }
        else
        {
            const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(BOLD_FONT, state.Buffer, fontH);
            Hax::Gui::DrawString(BOLD_FONT, state.Buffer, bounds.Min + (size - textSize) / 2.f, fontH, {.Color = Theme::Faint});
            Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = Theme::DisabledMask, .Rounding = 7_px});
        }

        if (Hax::Gui::IsItemHovered(id))
            Hax::Gui::SetMouseIcon(enabled ? Hax::Gui::MouseIcon_TextInput : Hax::Gui::MouseIcon_NotAllowed);
        return changed;
    }

    bool Hotkey(size_t id, int& key)
    {
        const float fontH = Hax::Gui::Scale(11.5f);
        const Hax::Vector2 padding = {8_px, 4_px};

        Hax::WStringView keyName = Hax::Gui::GetKeyName((Hax::uint8)key);
        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(BOLD_FONT, keyName, fontH);
        const Hax::Vector2 btnSize = {Hax::Max(28_px, textSize.X + padding.X * 2.f), textSize.Y + padding.Y * 2.f + 1_px};

        Hax::Rect bounds;
        bounds.Min = Hax::Gui::GetCursorPos();
        bounds.Max = bounds.Min + btnSize;

        Hax::Gui::PlaceItem(btnSize);
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;

        double& state = Hax::Gui::GetState<double>(id);

        Hax::Gui::Interact(id, bounds);
        if (Hax::Gui::IsItemClicked(id))
        {
            G->KeyListenerId = (G->KeyListenerId == 0) ? id : 0;
            state = Hax::Gui::GetTime();
        }
        const bool hovered = Hax::Gui::IsItemHovered(id);
        if (hovered)
            Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);

        bool changed = false;
        const bool listening = G->KeyListenerId == id;

        if (listening)
        {
            auto& keys = Hax::Gui::GetJustPressedKeys();
            if (keys.Size() > 0)
            {
                Hax::uint8 pressedKey = keys.Last();
                if (pressedKey == 0x1B) // VK_ESCAPE
                    pressedKey = 0;

                changed = key != pressedKey;
                key = pressedKey;

                G->KeyListenerId = 0;
            }
        }

        if (listening)
        {
            Hax::Gui::LinearColor glow(Theme::Accent);
            glow.A = 0.25f + 0.35f * (Hax::Sin(Hax::kPi * (float)(Hax::Gui::GetTime() - state) / 0.5f) + 1.f) * 0.5f;
            Hax::Gui::DrawRect(bounds.Min - Hax::Vector2(3_px, 3_px), bounds.Max + Hax::Vector2(3_px, 3_px), {.FillColor = glow.ToColor(), .Rounding = 7_px});
        }
        DrawKeyCap(bounds, listening ? Theme::Accent : (hovered ? 0x46505BFF : Theme::Line2));
        Hax::Gui::DrawString(BOLD_FONT, keyName, {bounds.Min.X + (btnSize.X - textSize.X) / 2.f, bounds.Min.Y + padding.Y}, fontH,
            {.Color = listening ? Theme::Accent : Theme::Muted});

        return changed;
    }

    bool HotkeyEx(size_t id, int& key, Hax::WStringView label, Hax::WStringView desc)
    {
        const float fontH = Hax::Gui::Scale(11.5f);
        const Hax::Vector2 padding = {8_px, 4_px};
        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(BOLD_FONT, Hax::Gui::GetKeyName((Hax::uint8)key), fontH);
        const Hax::Vector2 btnSize = {Hax::Max(28_px, textSize.X + padding.X * 2.f), textSize.Y + padding.Y * 2.f + 1_px};

        Hax::Gui::BeginVertical();
        Hax::Gui::BeginHorizontal();
        const Hax::Vector2 labelPos = Hax::Gui::GetCursorPos();
        const Hax::Vector2 labelSize = Hax::Gui::CalcTextSize(MAIN_FONT, label, Hax::Gui::Scale(Theme::MainFontSize));
        LabelAlignedByH(MAIN_FONT, label, Hax::Gui::Scale(Theme::MainFontSize), Theme::Muted, btnSize.Y);
        TextTooltip(id ^ 0x484F544B45594849ull, desc,
            Hax::Rect::FromPosSize(labelPos, {labelSize.X, btnSize.Y}));
        Hax::Gui::Space(Hax::Gui::GetContentRegionAvail().X - btnSize.X);
        bool changed = Hotkey(id, key);
        Hax::Gui::EndHorizontal();
        Hax::Gui::EndVertical();
        return changed;
    }

    bool Checkbox(size_t id, bool& val)
    {
        const float size = Hax::Gui::Scale(Theme::CheckboxSize);

        Hax::Rect bounds;
        bounds.Min = Hax::Gui::GetCursorPos();
        bounds.Max = bounds.Min + Hax::Vector2(size, size);

        Hax::Gui::PlaceItem(bounds.GetSize());
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;

        Hax::Gui::Interact(id, bounds);

        if (Hax::Gui::IsItemClicked(id))
            val = !val;

        const bool hovered = Hax::Gui::IsItemHovered(id);
        if (hovered)
            Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);

        Hax::Gui::DrawRect(bounds.Min, bounds.Max,
            {.BorderColor = val ? Theme::Accent : (hovered ? 0x46505BFF : Theme::Line2), .BorderTh = 1.5_px,
             .FillColor = val ? Theme::Accent : Theme::Surface1, .Rounding = 4_px});
        if (val)
        {
            const float markH = 10_px;
            const Hax::Vector2 markSize = Hax::Gui::CalcTextSize(ICONS_FONT, L"", markH);
            Hax::Gui::DrawString(ICONS_FONT, L"", bounds.GetCenter() - markSize / 2.f, markH, {.Color = Theme::Ink});
        }
        return Hax::Gui::IsItemClicked(id);
    }

    bool CheckboxEx(size_t id, bool& val, Hax::WStringView text)
    {
        bool changed = false;
        Hax::Gui::BeginHorizontal(8_px);
        {
            changed = Checkbox(id, val);
            LabelAlignedByH(MAIN_FONT, text, Hax::Gui::Scale(Theme::DescFontSize), val ? Theme::Text : Theme::Muted, Hax::Gui::Scale(Theme::CheckboxSize));
            Hax::Gui::Interact(id, Hax::Gui::GetLayoutBounds());
        }
        Hax::Gui::EndHorizontal();

        return changed;
    }
}
