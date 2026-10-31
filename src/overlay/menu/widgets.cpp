#include <src/pch.h>

#include "widgets.h"

#include <src/cheat.h>

#include "localization.h"
#include "theme.h"

#include <initializer_list>

#define DESC_FONT G->NunitoSans_SemiBold
#define MAIN_FONT G->NunitoSans_SemiBold
#define BOLD_FONT G->NunitoSans_Bold
#define HEADER_FONT G->NunitoSans_ExtraBold

namespace Cheat
{
    static Hax::Gui::Color Mix(Hax::Gui::Color a, Hax::Gui::Color b, float t)
    {
        return Hax::Lerp(Hax::Gui::LinearColor(a), Hax::Gui::LinearColor(b), t).ToColor();
    }

    static Hax::Gui::Color WithAlpha(Hax::Gui::Color c, float a)
    {
        Hax::Gui::LinearColor l(c);
        l.A = a;
        return l.ToColor();
    }

    static void AnimateHover(Hax::Gui::LinearAnim& anim, bool hovered, float speed = 0.1f)
    {
        const float dt = Hax::Gui::GetDeltaTime();
        anim.Elapse(hovered ? dt : -dt, speed);
    }

    static float LineH(Hax::Gui::FontHandle font, float size)
    {
        return Hax::Gui::GetFontLineHeight(font, size);
    }

    static bool IsHostDesc(Hax::WStringView desc)
    {
        return !desc.Empty() && (desc == G->Loc[LocKey_HostOnly] || desc == G->Loc[LocKey_AvailableIfHost]);
    }

    // Draws `text`, cutting it with an ellipsis when it is wider than `maxW`.
    // Returns true when the text had to be cut.
    static bool DrawTextFit(Hax::Gui::FontHandle font, Hax::WStringView text, Hax::Vector2 pos, float fontH, Hax::Gui::Color col, float maxW)
    {
        if (Hax::Gui::CalcTextSize(font, text, fontH).X <= maxW)
        {
            Hax::Gui::DrawString(font, text, pos, fontH, {.Color = col});
            return false;
        }

        wchar_t buf[256];
        size_t n = Hax::Min(text.Size(), (size_t)250);
        while (n > 0)
        {
            wmemcpy(buf, text.Data(), n);
            buf[n] = L'…';
            buf[n + 1] = L'\0';
            if (Hax::Gui::CalcTextSize(font, Hax::WStringView(buf, n + 1), fontH).X <= maxW)
                break;
            --n;
        }
        if (n > 0)
            Hax::Gui::DrawString(font, Hax::WStringView(buf, n + 1), pos, fontH, {.Color = col});
        return true;
    }

    //=========================================================================
    // Vector icons (the icon font only ships the navigation glyphs)
    //=========================================================================

    struct IconPen
    {
        Hax::Vector2 Origin;
        float S;
        float Th;
        Hax::Gui::Color Col;

        Hax::Vector2 P(float x, float y) const { return {Origin.X + x * S, Origin.Y + y * S}; }

        void Seg(float x1, float y1, float x2, float y2) const
        {
            Hax::Gui::DrawLine(P(x1, y1), P(x2, y2), {.FillColor = Col, .Th = Th});
            Hax::Gui::DrawCircle(P(x1, y1), Th * 0.5f, {.FillColor = Col});
            Hax::Gui::DrawCircle(P(x2, y2), Th * 0.5f, {.FillColor = Col});
        }

        void Poly(std::initializer_list<float> pts, bool closed = false) const
        {
            const float* p = pts.begin();
            const size_t n = pts.size() / 2;
            for (size_t i = 0; i + 1 < n; ++i)
                Seg(p[i * 2], p[i * 2 + 1], p[i * 2 + 2], p[i * 2 + 3]);
            if (closed && n > 2)
                Seg(p[(n - 1) * 2], p[(n - 1) * 2 + 1], p[0], p[1]);
        }

        void Ring(float x, float y, float r) const
        {
            Hax::Gui::DrawCircle(P(x, y), r * S, {.BorderColor = Col, .BorderTh = Th, .FillColor = Hax::Gui::Color::Transparent});
        }

        void Dot(float x, float y, float r) const
        {
            Hax::Gui::DrawCircle(P(x, y), r * S, {.FillColor = Col});
        }

        // Approximated arc, angles in degrees, 0 = right, clockwise (screen space).
        void Arc(float cx, float cy, float r, float from, float to) const
        {
            constexpr int steps = 14;
            float prevX = cx + r * Hax::Cos(from * Hax::kPi / 180.f);
            float prevY = cy + r * Hax::Sin(from * Hax::kPi / 180.f);
            for (int i = 1; i <= steps; ++i)
            {
                const float a = (from + (to - from) * (float)i / steps) * Hax::kPi / 180.f;
                const float x = cx + r * Hax::Cos(a);
                const float y = cy + r * Hax::Sin(a);
                Seg(prevX, prevY, x, y);
                prevX = x;
                prevY = y;
            }
        }
    };

    void DrawIcon(Icon icon, const Hax::Rect& bounds, Hax::Gui::Color col)
    {
        const float size = Hax::Min(bounds.GetSize().X, bounds.GetSize().Y);
        IconPen pen;
        pen.S = size / 24.f;
        pen.Th = Hax::Max(1.25_px, 1.8f * pen.S);
        pen.Col = col;
        pen.Origin = bounds.GetCenter() - Hax::Vector2(size, size) / 2.f;

        switch (icon)
        {
        case Icon::Search:       pen.Ring(11, 11, 6.5f); pen.Seg(16, 16, 20, 20); break;
        case Icon::Close:        pen.Seg(6, 6, 18, 18); pen.Seg(18, 6, 6, 18); break;
        case Icon::ChevronRight: pen.Poly({9, 6, 15, 12, 9, 18}); break;
        case Icon::ChevronDown:  pen.Poly({6, 9, 12, 15, 18, 9}); break;
        case Icon::Check:        pen.Poly({5, 12.5f, 10, 17, 19, 7}); break;
        case Icon::Plus:         pen.Seg(12, 5, 12, 19); pen.Seg(5, 12, 19, 12); break;
        case Icon::Minus:        pen.Seg(5, 12, 19, 12); break;
        case Icon::Heart:
            pen.Poly({12, 20, 5.5f, 14, 4, 10, 4.6f, 7, 7, 5, 10, 5.4f, 12, 7.6f, 14, 5.4f, 17, 5, 19.4f, 7, 20, 10, 18.5f, 14}, true);
            break;
        case Icon::Move:         pen.Seg(4, 12, 19, 12); pen.Poly({13, 6, 19, 12, 13, 18}); break;
        case Icon::ArrowUp:      pen.Seg(12, 20, 12, 5); pen.Poly({6, 11, 12, 5, 18, 11}); break;
        case Icon::Hand:
            pen.Poly({8, 12, 8, 6.5f}); pen.Poly({11, 11, 11, 4.5f}); pen.Poly({14, 11, 14, 5.5f}); pen.Poly({17, 12, 17, 8.5f});
            pen.Poly({8, 12, 6, 13.5f, 6.5f, 16, 9.5f, 20, 15, 20, 17, 17.5f, 17, 12});
            break;
        case Icon::Zap:          pen.Poly({13, 2.5f, 5, 13.5f, 11.5f, 13.5f, 10.5f, 21.5f, 19, 10.5f, 12.5f, 10.5f}, true); break;
        case Icon::Sparkle:      pen.Poly({12, 3, 14, 10, 21, 12, 14, 14, 12, 21, 10, 14, 3, 12, 10, 10}, true); break;
        case Icon::ChevronsUp:   pen.Poly({7, 11, 12, 6, 17, 11}); pen.Poly({7, 18, 12, 13, 17, 18}); break;
        case Icon::Eye:
            pen.Poly({2.5f, 12, 6, 7.5f, 12, 5.5f, 18, 7.5f, 21.5f, 12, 18, 16.5f, 12, 18.5f, 6, 16.5f}, true);
            pen.Ring(12, 12, 3);
            break;
        case Icon::Shield:       pen.Poly({12, 3, 4.5f, 6, 4.5f, 12, 6, 16, 12, 21, 18, 16, 19.5f, 12, 19.5f, 6}, true); break;
        case Icon::Globe:
            pen.Ring(12, 12, 9); pen.Seg(3, 12, 21, 12);
            Hax::Gui::DrawEllipse(pen.P(12, 12), Hax::Vector2(3.8f * pen.S, 9 * pen.S), {.BorderColor = col, .BorderTh = pen.Th, .FillColor = Hax::Gui::Color::Transparent});
            break;
        case Icon::Map:          pen.Poly({3, 6, 9, 3, 15, 6, 21, 3, 21, 18, 15, 21, 9, 18, 3, 21}, true); pen.Seg(9, 3, 9, 18); pen.Seg(15, 6, 15, 21); break;
        case Icon::User:         pen.Ring(12, 8, 4); pen.Arc(12, 21, 8, 180, 360); break;
        case Icon::Users:        pen.Ring(9, 8, 3.5f); pen.Arc(9, 20.5f, 6.5f, 180, 360); pen.Arc(16, 8, 3.5f, -90, 90); pen.Poly({18.5f, 14.5f, 21, 17, 21.5f, 20.5f}); break;
        case Icon::Coin:
            pen.Ring(12, 12, 9); pen.Seg(12, 6.5f, 12, 17.5f);
            pen.Poly({14.5f, 9.5f, 12, 8.3f, 9.6f, 9.4f, 10, 11.3f, 14, 12.7f, 14.4f, 14.6f, 12, 15.7f, 9.5f, 14.5f});
            break;
        case Icon::Gem:          pen.Poly({6, 4, 18, 4, 21, 9, 12, 20.5f, 3, 9}, true); pen.Seg(3, 9, 21, 9); pen.Poly({8.5f, 9, 12, 20.5f, 15.5f, 9}); break;
        case Icon::Box:          pen.Poly({12, 3, 20, 7.5f, 20, 16.5f, 12, 21, 4, 16.5f, 4, 7.5f}, true); pen.Poly({4, 7.5f, 12, 12, 20, 7.5f}); pen.Seg(12, 12, 12, 21); break;
        case Icon::Target:       pen.Ring(12, 12, 9); pen.Ring(12, 12, 5); pen.Dot(12, 12, 1.4f); break;
        case Icon::Extract:      pen.Seg(12, 3, 12, 15); pen.Poly({7, 10, 12, 15, 17, 10}); pen.Seg(4, 20.5f, 20, 20.5f); break;
        case Icon::Wallet:       pen.Poly({3, 6, 21, 6, 21, 19, 3, 19}, true); pen.Seg(3, 10, 21, 10); pen.Seg(16, 15, 18, 15); break;
        case Icon::Play:         pen.Poly({6, 4, 19, 12, 6, 20}, true); break;
        case Icon::Bars:         pen.Seg(5, 20, 5, 11); pen.Seg(11, 20, 11, 5); pen.Seg(17, 20, 17, 13); pen.Seg(2.5f, 20.5f, 21.5f, 20.5f); break;
        case Icon::Reset:        pen.Arc(12, 12, 8.5f, 200, 520); pen.Poly({4, 4, 4.3f, 9.3f, 9.5f, 9}); break;
        case Icon::Ghost:
            pen.Poly({4.5f, 20, 4.5f, 11, 6.5f, 6, 12, 3.5f, 17.5f, 6, 19.5f, 11, 19.5f, 20, 16.5f, 18, 14, 20, 12, 18, 10, 20, 7.5f, 18}, true);
            pen.Dot(9.3f, 10.5f, 1.4f); pen.Dot(14.7f, 10.5f, 1.4f);
            break;
        case Icon::Sliders:
            pen.Seg(4, 6, 12, 6); pen.Seg(18, 6, 20, 6); pen.Seg(4, 12, 6, 12); pen.Seg(12, 12, 20, 12); pen.Seg(4, 18, 14, 18); pen.Seg(20, 18, 20, 18);
            pen.Ring(15, 6, 2.2f); pen.Ring(9, 12, 2.2f); pen.Ring(17, 18, 2.2f);
            break;
        case Icon::Sun:
            pen.Ring(12, 12, 4);
            pen.Seg(12, 2.5f, 12, 4.5f); pen.Seg(12, 19.5f, 12, 21.5f); pen.Seg(2.5f, 12, 4.5f, 12); pen.Seg(19.5f, 12, 21.5f, 12);
            pen.Seg(5.3f, 5.3f, 6.7f, 6.7f); pen.Seg(17.3f, 17.3f, 18.7f, 18.7f); pen.Seg(5.3f, 18.7f, 6.7f, 17.3f); pen.Seg(17.3f, 6.7f, 18.7f, 5.3f);
            break;
        case Icon::Save:         pen.Poly({5, 3.5f, 16, 3.5f, 19.5f, 7, 19.5f, 20.5f, 5, 20.5f}, true); pen.Poly({8, 3.5f, 8, 8, 15, 8, 15, 3.5f}); pen.Poly({8, 20.5f, 8, 14, 16, 14, 16, 20.5f}); break;
        case Icon::Trash:        pen.Seg(4, 6.5f, 20, 6.5f); pen.Poly({9, 6.5f, 9.5f, 3.5f, 14.5f, 3.5f, 15, 6.5f}); pen.Poly({6, 6.5f, 7, 20.5f, 17, 20.5f, 18, 6.5f}); pen.Seg(10, 10.5f, 10, 16.5f); pen.Seg(14, 10.5f, 14, 16.5f); break;
        case Icon::Battery:      pen.Poly({3, 7.5f, 18, 7.5f, 18, 16.5f, 3, 16.5f}, true); pen.Seg(20.5f, 10.5f, 20.5f, 13.5f); pen.Seg(6.5f, 10.5f, 6.5f, 13.5f); pen.Seg(10, 10.5f, 10, 13.5f); break;
        case Icon::Keyboard:
            pen.Poly({2.5f, 6, 21.5f, 6, 21.5f, 18, 2.5f, 18}, true);
            pen.Dot(6.5f, 10, 1); pen.Dot(10.5f, 10, 1); pen.Dot(14.5f, 10, 1); pen.Dot(18, 10, 1); pen.Seg(7.5f, 14, 16.5f, 14);
            break;
        case Icon::Clock:        pen.Ring(12, 12, 9); pen.Poly({12, 7, 12, 12, 15.5f, 14}); break;
        case Icon::Crosshair:    pen.Ring(12, 12, 7.5f); pen.Seg(12, 2, 12, 7); pen.Seg(12, 17, 12, 22); pen.Seg(2, 12, 7, 12); pen.Seg(17, 12, 22, 12); break;
        case Icon::Cube:         pen.Poly({12, 3, 20, 7.5f, 20, 16.5f, 12, 21, 4, 16.5f, 4, 7.5f}, true); pen.Poly({4, 7.5f, 12, 12, 20, 7.5f}); pen.Seg(12, 12, 12, 21); break;
        case Icon::Shirt:        pen.Poly({8.5f, 3.5f, 3, 7, 5, 11, 7, 10, 7, 20.5f, 17, 20.5f, 17, 10, 19, 11, 21, 7, 15.5f, 3.5f, 14, 5.5f, 10, 5.5f}, true); break;
        case Icon::Truck:        pen.Poly({2.5f, 6, 14, 6, 14, 16, 2.5f, 16}, true); pen.Poly({14, 9.5f, 18, 9.5f, 21.5f, 13, 21.5f, 16, 14, 16}); pen.Ring(7, 18, 2); pen.Ring(17.5f, 18, 2); break;
        case Icon::Gun:          pen.Poly({3, 8, 20, 8, 20, 12, 11, 12, 10, 18, 6, 18, 7, 12, 3, 12}, true); pen.Seg(20, 10, 22, 10); break;
        case Icon::Dot:          pen.Dot(12, 12, 4); break;
        case Icon::None:
        default: break;
        }
    }

    void IconItem(Icon icon, float size, Hax::Gui::Color col, float rowH)
    {
        const float h = Hax::Max(size, rowH);
        const Hax::Rect bounds = Hax::Rect::FromPosSize(Hax::Gui::GetCursorPos(), {size, h});
        Hax::Gui::PlaceItem(bounds.GetSize());
        if (Hax::Gui::IsItemVisible(bounds))
            DrawIcon(icon, Hax::Rect::FromPosSize({bounds.Min.X, bounds.Min.Y + (h - size) / 2.f}, {size, size}), col);
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

        const Hax::Vector2 pad = {8_px, 5_px};
        Hax::Gui::DrawRect(bounds.Min - pad, bounds.Max + pad,
            {.BorderColor = WithAlpha(Theme::Accent, fade * pulse), .BorderTh = 1.5_px, .FillColor = WithAlpha(Theme::Accent, fade * 0.12f), .Rounding = 7_px});
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

        DrawIcon(Icon::Search, Hax::Rect::FromPosSize({bounds.Min.X + 10_px, bounds.Min.Y + (size.Y - 15_px) / 2.f}, {15_px, 15_px}),
            focused ? Theme::Muted : Theme::Faint);

        const float fontH = Hax::Gui::Scale(12.5f);
        bool changed = false;
        const bool hasText = buf[0] != L'\0';
        const float clearW = hasText ? 22_px : 0.f;
        const Hax::Rect editBounds = {{bounds.Min.X + 32_px, bounds.Min.Y + 8_px}, {bounds.Max.X - 8_px - clearW, bounds.Max.Y - 8_px}};

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
            DrawIcon(Icon::Close, Hax::Rect::FromPosSize(clearBounds.GetCenter() - Hax::Vector2(6_px, 6_px), {12_px, 12_px}),
                hovered ? Theme::Text : Theme::Faint);
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
            Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = WithAlpha(Theme::Surface1, hover.Progress), .Rounding = 8_px});

        const float iconH = 14_px;
        const Hax::WStringView icon = {&icon16, 1};
        const Hax::Vector2 iconSize = Hax::Gui::CalcTextSize(G->Icons_Solid, icon, iconH);
        Hax::Gui::DrawString(G->Icons_Solid, icon, {bounds.Min.X + 10_px + (18_px - iconSize.X) / 2.f, bounds.Min.Y + (size.Y - iconSize.Y) / 2.f}, iconH,
            {.Color = active ? Theme::Accent : Mix(Theme::Faint, Theme::Muted, hover.Progress)});

        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(BOLD_FONT, text, fontH);
        Hax::Gui::DrawString(BOLD_FONT, text, {bounds.Min.X + 38_px, bounds.Min.Y + (size.Y - textSize.Y) / 2.f}, fontH,
            {.Color = active ? Theme::Text : Mix(Theme::Muted, Theme::Text, hover.Progress)});

        if (chevron)
            DrawIcon(Icon::ChevronRight, Hax::Rect::FromPosSize({bounds.Max.X - 24_px, bounds.Min.Y + (size.Y - 13_px) / 2.f}, {13_px, 13_px}), Theme::Faint);

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
            Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = WithAlpha(Theme::Surface1, hover.Progress), .Rounding = 8_px});

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

        const float maxW = size.X - 20_px;
        DrawTextFit(MAIN_FONT, text, {bounds.Min.X + 10_px, bounds.Min.Y + 6_px}, fontH, Theme::Text, maxW);
        DrawTextFit(DESC_FONT, where, {bounds.Min.X + 10_px, bounds.Min.Y + 23_px}, subH, Theme::Faint, maxW);

        return Hax::Gui::IsItemClicked(id);
    }

    //=========================================================================
    // Small pieces
    //=========================================================================

    static float DrawBadgeAt(Hax::WStringView text, Hax::Gui::Color fg, Hax::Vector2 min)
    {
        const float fontH = Hax::Gui::Scale(10.f);
        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(HEADER_FONT, text, fontH);
        const Hax::Vector2 size = {textSize.X + 16_px, 20_px};
        Hax::Gui::DrawRect(min, min + size, {.FillColor = WithAlpha(fg, 0.13f), .Rounding = size.Y / 2.f});
        Hax::Gui::DrawString(HEADER_FONT, text, min + (size - textSize) / 2.f, fontH, {.Color = fg});
        return size.X;
    }

    Hax::Vector2 CalcBadgeSize(Hax::WStringView text)
    {
        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(HEADER_FONT, text, Hax::Gui::Scale(10.f));
        return {textSize.X + 16_px, 20_px};
    }

    void Badge(Hax::WStringView text, Hax::Gui::Color fg)
    {
        const Hax::Vector2 size = CalcBadgeSize(text);
        const Hax::Rect bounds = Hax::Rect::FromPosSize(Hax::Gui::GetCursorPos(), size);
        Hax::Gui::PlaceItem(size);
        if (Hax::Gui::IsItemVisible(bounds))
            DrawBadgeAt(text, fg, bounds.Min);
    }

    static Hax::WStringView HostBadgeText()
    {
        return G->Loc[LocKey_HostBadge];
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

    bool IconButton(size_t id, Icon icon, Hax::WStringView tooltip)
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
        DrawIcon(icon, Hax::Rect::FromPosSize(bounds.GetCenter() - Hax::Vector2(7_px, 7_px), {14_px, 14_px}),
            Mix(Theme::Muted, Theme::Text, hover.Progress));
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
    // Page layout
    //=========================================================================

    struct ColumnsState
    {
        size_t Id;
        float W[6];
        int Count;
        int Index;
    };

    static ColumnsState s_ColumnStack[4];
    static int s_ColumnDepth = 0;

    void BeginColumns(size_t id, int count, const float* weights)
    {
        HAX_ASSERT(s_ColumnDepth < (int)_countof(s_ColumnStack));
        count = Hax::Clamp(count, 1, 6);
        ColumnsState& cols = s_ColumnStack[s_ColumnDepth++];
        cols.Id = id;
        cols.Count = count;
        cols.Index = 0;

        const float gap = 16_px;
        const float total = Hax::Max(1.f, Hax::Gui::GetContentRegionAvail().X - gap * (float)(count - 1));
        float sum = 0.f;
        for (int i = 0; i < count; ++i)
            sum += weights ? weights[i] : 1.f;
        for (int i = 0; i < count; ++i)
            cols.W[i] = Hax::Floor(total * (weights ? weights[i] : 1.f) / sum);

        Hax::Gui::BeginHorizontal(gap);
        Hax::Gui::BeginContainer(id, {.W = cols.W[0], .FitY = true});
        Hax::Gui::BeginVertical(16_px);
    }

    void NextColumn()
    {
        ColumnsState& cols = s_ColumnStack[s_ColumnDepth - 1];
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();
        cols.Index = Hax::Min(cols.Index + 1, cols.Count - 1);
        Hax::Gui::BeginContainer(cols.Id + (size_t)cols.Index * 7919, {.W = cols.W[cols.Index], .FitY = true});
        Hax::Gui::BeginVertical(16_px);
    }

    void EndColumns()
    {
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();
        Hax::Gui::EndHorizontal();
        --s_ColumnDepth;
    }

    void StatTile(Hax::WStringView label, Hax::WStringView value, Hax::WStringView sub, float progress, Hax::Gui::Color valueCol)
    {
        const Hax::Vector2 size = {Hax::Gui::GetContentRegionAvail().X, 86_px};
        const Hax::Rect bounds = Hax::Rect::FromPosSize(Hax::Gui::GetCursorPos(), size);
        Hax::Gui::PlaceItem(size);
        if (!Hax::Gui::IsItemVisible(bounds))
            return;

        Hax::Gui::DrawRect(bounds.Min, bounds.Max,
            {.BorderColor = Theme::Line, .BorderTh = 1_px, .FillColor = Theme::Surface1, .Rounding = Hax::Gui::Scale(Theme::CardR)});

        const float padX = 16_px;
        const float maxW = size.X - padX * 2.f;
        const float labelH = Hax::Gui::Scale(11.5f);
        const float valueH = Hax::Gui::Scale(20.f);

        float y = bounds.Min.Y + 14_px;
        DrawSearchHighlight(label, Hax::Rect::FromPosSize({bounds.Min.X + padX, y}, Hax::Gui::CalcTextSize(DESC_FONT, label, labelH)));
        DrawTextFit(DESC_FONT, label, {bounds.Min.X + padX, y}, labelH, Theme::Faint, maxW);
        y += LineH(DESC_FONT, labelH) + 4_px;
        DrawTextFit(HEADER_FONT, value, {bounds.Min.X + padX, y}, valueH, valueCol, maxW);
        y += LineH(HEADER_FONT, valueH) + 6_px;

        if (progress >= 0.f)
        {
            const float barY = bounds.Max.Y - 18_px;
            Hax::Gui::DrawRect({bounds.Min.X + padX, barY}, {bounds.Max.X - padX, barY + 4_px}, {.FillColor = Theme::Surface3, .Rounding = 2_px});
            const float fillX = Hax::Lerp(bounds.Min.X + padX, bounds.Max.X - padX, Hax::Clamp(progress, 0.f, 1.f));
            if (fillX > bounds.Min.X + padX + 1.f)
                Hax::Gui::DrawRect({bounds.Min.X + padX, barY}, {fillX, barY + 4_px}, {.FillColor = Theme::Accent, .Rounding = 2_px});
        }
        else if (!sub.Empty())
            DrawTextFit(DESC_FONT, sub, {bounds.Min.X + padX, bounds.Max.Y - 14_px - LineH(DESC_FONT, labelH)}, labelH, Theme::Faint, maxW);
    }

    //=========================================================================
    // Cards
    //=========================================================================

    static int s_CardDepth = 0;

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

        ++s_CardDepth;
        Hax::Gui::BeginHorizontal();
        const float padding = 16_px;
        Hax::Gui::Space(padding);
        Hax::Gui::BeginContainer(id + 1, {.W = containerSize.X - padding * 2.f, .H = 15.f, .FitY = true});
        Hax::Gui::BeginVertical(11_px);
        Hax::Gui::Dummy({0.f, 3_px});
    }

    void EndPanel()
    {
        Hax::Gui::Dummy({0.f, 3_px});
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();
        Hax::Gui::EndHorizontal();
        --s_CardDepth;

        if (Hax::Gui::IsDrawingSkipped())
            Hax::Gui::PopSkipDrawing();

        Hax::Gui::EndContainer();
    }

    void HorizontalLine(float th, float w)
    {
        // Inside a card a divider runs edge to edge, through the card padding.
        if (s_CardDepth > 0 && w == 0.f)
        {
            const Hax::Vector2 pos = Hax::Gui::GetCursorPos();
            const float avail = Hax::Gui::GetContentRegionAvail().X;
            Hax::Gui::PlaceItem({avail, th});
            const float y = pos.Y + th / 2.f;
            Hax::Gui::DrawLine({pos.X - 16_px + 1_px, y}, {pos.X + avail + 16_px - 1_px, y}, {.FillColor = Theme::Line, .Th = th});
            return;
        }
        Hax::Gui::HorizontalLine(th, Theme::Line, w);
    }

    //=========================================================================
    // Controls
    //=========================================================================

    struct ToggleState
    {
        Hax::Gui::LinearAnim HoverAnim;
        Hax::Gui::LinearAnim ToggleAnim;
    };

    static bool ToggleAt(size_t id, bool& value, bool enabled, const Hax::Rect& bounds)
    {
        ToggleState& state = Hax::Gui::GetState<ToggleState>(id);
        const float deltaTime = Hax::Gui::GetDeltaTime();
        state.ToggleAnim.Elapse(value ? deltaTime : -deltaTime, 0.12f);

        if (enabled)
            Hax::Gui::Interact(id, bounds);

        AnimateHover(state.HoverAnim, Hax::Gui::IsItemHovered(id));
        if (Hax::Gui::IsItemHovered(id))
            Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);

        const bool clicked = Hax::Gui::IsItemClicked(id);
        if (clicked)
            value = !value;

        const float t = state.ToggleAnim.Progress;
        const Hax::Gui::Color offTrack = Mix(Theme::Surface3, 0x343A42FF, state.HoverAnim.Progress);
        const float h = bounds.GetSize().Y;
        Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = Mix(offTrack, Theme::Accent, t), .Rounding = h / 2.f});

        const float knobX = Hax::Lerp(bounds.Min.X + h / 2.f, bounds.Max.X - h / 2.f, t);
        Hax::Gui::DrawCircle({knobX, bounds.GetCenter().Y}, h / 2.f - 3_px, {.FillColor = Mix(Theme::Faint, Theme::Ink, t)});

        return clicked;
    }

    bool Toggle(size_t id, bool& value, bool enabled)
    {
        const Hax::Vector2 toggleSize = Hax::Gui::Scale(Theme::ToggleSize);
        const Hax::Rect bounds = Hax::Rect::FromPosSize(Hax::Gui::GetCursorPos(), toggleSize);
        Hax::Gui::PlaceItem(toggleSize);
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;
        return ToggleAt(id, value, enabled, bounds);
    }

    static bool SettingsBtnAt(size_t id, bool disabled, const Hax::Rect& bounds)
    {
        if (!disabled)
            Hax::Gui::Interact(id, bounds);
        Hax::Gui::LinearAnim& state = Hax::Gui::GetState<Hax::Gui::LinearAnim>(id);
        AnimateHover(state, Hax::Gui::IsItemHovered(id));
        if (Hax::Gui::IsItemHovered(id))
            Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);

        const Hax::Gui::Color color = Hax::Gui::IsItemActive(id) ? Theme::Accent : Mix(Theme::Faint, Theme::Text, state.Progress);
        DrawIcon(Icon::Sliders, bounds, color);
        return Hax::Gui::IsItemClicked(id);
    }

    bool SettingsBtn(size_t id, bool disabled)
    {
        const Hax::Vector2 size = {16_px, 16_px};
        const Hax::Rect bounds = Hax::Rect::FromPosSize(Hax::Gui::GetCursorPos(), size);
        Hax::Gui::PlaceItem(size);
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;
        return SettingsBtnAt(id, disabled, bounds);
    }

    static Hax::Vector2 CalcHotkeySize(int key)
    {
        const float fontH = Hax::Gui::Scale(11.5f);
        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(BOLD_FONT, key ? Hax::Gui::GetKeyName((Hax::uint8)key) : Hax::WStringView(L"—"), fontH);
        return {Hax::Max(28_px, textSize.X + 16_px), 24_px};
    }

    static bool HotkeyAt(size_t id, int& key, const Hax::Rect& bounds)
    {
        const float fontH = Hax::Gui::Scale(11.5f);
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
                if (pressedKey == 0x1B) // VK_ESCAPE clears the binding
                    pressedKey = 0;

                changed = key != pressedKey;
                key = pressedKey;
                G->KeyListenerId = 0;
            }
        }

        if (listening)
        {
            const float a = 0.25f + 0.35f * (Hax::Sin(Hax::kPi * (float)(Hax::Gui::GetTime() - state) / 0.5f) + 1.f) * 0.5f;
            Hax::Gui::DrawRect(bounds.Min - Hax::Vector2(3_px, 3_px), bounds.Max + Hax::Vector2(3_px, 3_px),
                {.FillColor = WithAlpha(Theme::Accent, a), .Rounding = 7_px});
        }
        DrawKeyCap(bounds, listening ? Theme::Accent : (hovered ? 0x46505BFF : Theme::Line2));

        const Hax::WStringView keyName = key ? Hax::Gui::GetKeyName((Hax::uint8)key) : Hax::WStringView(L"—");
        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(BOLD_FONT, keyName, fontH);
        Hax::Gui::DrawString(BOLD_FONT, keyName, {bounds.Min.X + (bounds.GetSize().X - textSize.X) / 2.f, bounds.Min.Y + (bounds.GetSize().Y - 1_px - textSize.Y) / 2.f}, fontH,
            {.Color = listening ? Theme::Accent : Theme::Muted});

        return changed;
    }

    bool Hotkey(size_t id, int& key)
    {
        const Hax::Vector2 size = CalcHotkeySize(key);
        const Hax::Rect bounds = Hax::Rect::FromPosSize(Hax::Gui::GetCursorPos(), size);
        Hax::Gui::PlaceItem(size);
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;
        return HotkeyAt(id, key, bounds);
    }

    void OpenPopup(size_t id, const Hax::Vector2& pos);

    // Shared layout for a settings row: label (+ hint) on the left, controls on the right.
    struct RowLayout
    {
        Hax::Rect Bounds;
        float RightX;     // controls are placed from right to left starting here
        float RowH;
        bool Visible;
    };

    static RowLayout BeginRow(Hax::WStringView hint, float controlH)
    {
        const float labelLine = LineH(MAIN_FONT, Hax::Gui::Scale(Theme::MainFontSize));
        const float hintLine = LineH(DESC_FONT, Hax::Gui::Scale(11.5f));
        const float blockH = labelLine + (hint.Empty() ? 0.f : 1_px + hintLine);

        RowLayout row;
        row.RowH = Hax::Max(controlH, blockH);
        const Hax::Vector2 size = {Hax::Gui::GetContentRegionAvail().X, row.RowH};
        row.Bounds = Hax::Rect::FromPosSize(Hax::Gui::GetCursorPos(), size);
        row.RightX = row.Bounds.Max.X;
        Hax::Gui::PlaceItem(size);
        row.Visible = Hax::Gui::IsItemVisible(row.Bounds);
        return row;
    }

    static Hax::Rect TakeRight(RowLayout& row, Hax::Vector2 size, float gap = 8_px)
    {
        const Hax::Rect r = Hax::Rect::FromPosSize({row.RightX - size.X, row.Bounds.Min.Y + (row.RowH - size.Y) / 2.f}, size);
        row.RightX = r.Min.X - gap;
        return r;
    }

    static void DrawRowText(size_t id, const RowLayout& row, Hax::WStringView text, Hax::WStringView hint, Hax::Gui::Color textCol = Theme::Text)
    {
        const float labelH = Hax::Gui::Scale(Theme::MainFontSize);
        const float hintH = Hax::Gui::Scale(11.5f);
        const float labelLine = LineH(MAIN_FONT, labelH);
        const float hintLine = LineH(DESC_FONT, hintH);
        const float blockH = labelLine + (hint.Empty() ? 0.f : 1_px + hintLine);
        const float maxW = Hax::Max(10.f, row.RightX - row.Bounds.Min.X - 4_px);

        const Hax::Vector2 labelPos = {row.Bounds.Min.X, row.Bounds.Min.Y + (row.RowH - blockH) / 2.f};
        const Hax::Vector2 labelSize = Hax::Gui::CalcTextSize(MAIN_FONT, text, labelH);
        DrawSearchHighlight(text, Hax::Rect::FromPosSize(labelPos, {Hax::Min(labelSize.X, maxW), labelSize.Y}));
        bool cut = DrawTextFit(MAIN_FONT, text, labelPos, labelH, textCol, maxW);
        if (!hint.Empty())
            cut |= DrawTextFit(DESC_FONT, hint, {labelPos.X, labelPos.Y + labelLine + 1_px}, hintH, Theme::Faint, maxW);

        // Full text in a tooltip when something did not fit.
        if (cut)
        {
            wchar_t full[320];
            if (hint.Empty())
                swprintf_s(full, L"%.*ls", (int)text.Size(), text.Data());
            else
                swprintf_s(full, L"%.*ls — %.*ls", (int)text.Size(), text.Data(), (int)hint.Size(), hint.Data());
            TextTooltip(id ^ 0x524F57544950ull, full, Hax::Rect::FromPosSize(labelPos, {maxW, blockH}));
        }
    }

    bool ToggleEx(size_t id, bool& value, Hax::WStringView text, Hax::WStringView desc, ToggleExParams params)
    {
        const bool host = params.HostOnly || IsHostDesc(desc);
        const Hax::WStringView hint = IsHostDesc(desc) ? Hax::WStringView() : desc;
        const Hax::Vector2 toggleSize = Hax::Gui::Scale(Theme::ToggleSize);

        RowLayout row = BeginRow(hint, Hax::Max(toggleSize.Y, params.Hotkey ? 24_px : 0.f));
        if (!row.Visible)
            return false;

        const bool changed = ToggleAt(id, value, !params.Disabled, TakeRight(row, toggleSize, 10_px));

        if (params.SettingsId != 0)
        {
            const Hax::Rect gear = TakeRight(row, {16_px, 16_px}, 10_px);
            if (SettingsBtnAt(id + 1, params.Disabled, gear))
                OpenPopup(params.SettingsId, {gear.Min.X, gear.Max.Y + 6_px});
        }

        if (params.Hotkey)
            HotkeyAt(params.HotkeyId ? params.HotkeyId : id + 2, *params.Hotkey, TakeRight(row, CalcHotkeySize(*params.Hotkey)));

        if (host)
        {
            const Hax::Rect badge = TakeRight(row, CalcBadgeSize(HostBadgeText()));
            DrawBadgeAt(HostBadgeText(), Theme::Host, badge.Min);
        }

        DrawRowText(id, row, text, hint);

        if (params.Disabled)
            Hax::Gui::DrawRect(row.Bounds.Min - Hax::Vector2(0.f, 2_px), row.Bounds.Max + Hax::Vector2(0.f, 2_px), {.FillColor = Theme::DisabledMask});

        return changed;
    }

    //=========================================================================
    // Buttons
    //=========================================================================

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
        return LineH(MAIN_FONT, fontH) + Hax::Gui::Scale(kButtonPadding.Y) * 2.f;
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

    static bool ButtonAt(size_t id, Hax::WStringView label, const Hax::Rect& bounds, bool enabled, ButtonStyle style, float fontH, Hax::WStringView tooltip = {}, Icon icon = Icon::None)
    {
        if (enabled)
            Hax::Gui::Interact(id, bounds);

        const ButtonColors c = ButtonBeh(id, style);
        const float r = Hax::Min(Hax::Gui::Scale(Theme::ControlR), bounds.GetSize().Y / 2.f);
        Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.BorderColor = c.Border, .BorderTh = 1_px, .FillColor = c.Bg, .Rounding = r});

        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(MAIN_FONT, label, fontH);
        const float iconSize = icon != Icon::None ? 14_px : 0.f;
        const float iconGap = icon != Icon::None ? 7_px : 0.f;
        const float contentW = Hax::Min(textSize.X, bounds.GetSize().X - 16_px - iconSize - iconGap) + iconSize + iconGap;
        float x = bounds.Min.X + (bounds.GetSize().X - contentW) / 2.f;
        if (icon != Icon::None)
        {
            DrawIcon(icon, Hax::Rect::FromPosSize({x, bounds.GetCenter().Y - iconSize / 2.f}, {iconSize, iconSize}), c.Fg);
            x += iconSize + iconGap;
        }
        const Hax::Vector2 labelPos = {x, bounds.Min.Y + (bounds.GetSize().Y - textSize.Y) / 2.f};
        DrawSearchHighlight(label, bounds);
        DrawTextFit(MAIN_FONT, label, labelPos, fontH, c.Fg, bounds.Max.X - 8_px - labelPos.X);
        TextTooltip(id ^ 0x425554544F4E4849ull, tooltip, bounds);

        if (!enabled)
            Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = Theme::DisabledMask, .Rounding = r});

        return Hax::Gui::IsItemClicked(id);
    }

    bool Button(size_t id, Hax::WStringView label, Hax::WStringView desc, const ButtonParams& params)
    {
        const float fontH = Hax::Gui::Scale(Theme::DescFontSize);
        const Hax::Vector2 padding = Hax::Gui::Scale(kButtonPadding);
        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(MAIN_FONT, label, fontH);
        const float iconExtra = params.Icon != Icon::None ? 21_px : 0.f;
        const Hax::Vector2 btnSize = {Hax::Max(textSize.X + padding.X * 2.f + iconExtra, params.MinW), textSize.Y + padding.Y * 2.f};

        const Hax::Rect bounds = Hax::Rect::FromPosSize(Hax::Gui::GetCursorPos(), btnSize);
        Hax::Gui::PlaceItem(btnSize);
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;

        return ButtonAt(id, label, bounds, params.Enabled, params.Style, fontH, IsHostDesc(desc) ? Hax::WStringView() : desc, params.Icon);
    }

    bool ButtonGrid(size_t id, const GridButton* buttons, int count, int perRow)
    {
        bool any = false;
        const float gap = 8_px;
        perRow = Hax::Max(1, perRow);
        const float w = Hax::Floor((Hax::Gui::GetContentRegionAvail().X - gap * (float)(perRow - 1)) / (float)perRow);

        Hax::Gui::BeginVertical(gap);
        for (int i = 0; i < count; i += perRow)
        {
            Hax::Gui::BeginHorizontal(gap);
            for (int j = i; j < Hax::Min(count, i + perRow); ++j)
            {
                const GridButton& b = buttons[j];
                if (Button(id + (size_t)j * 31, b.Label, b.Tooltip, {.Enabled = b.Enabled, .MinW = w, .Style = b.Style, .Icon = b.Icon}) && b.Clicked)
                {
                    *b.Clicked = true;
                    any = true;
                }
            }
            Hax::Gui::EndHorizontal();
        }
        Hax::Gui::EndVertical();
        return any;
    }

    static Hax::Vector2 CalcSmallButtonSize(Hax::WStringView label)
    {
        const float fontH = Hax::Gui::Scale(12.f);
        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(MAIN_FONT, label, fontH);
        return {textSize.X + 20_px, 28_px};
    }

    bool ActionRow(size_t id, Hax::WStringView text, Hax::WStringView hint, Hax::WStringView btnLabel, const ActionRowParams& params)
    {
        const bool host = params.HostOnly || IsHostDesc(hint);
        if (IsHostDesc(hint))
            hint = {};

        RowLayout row = BeginRow(hint, 28_px);
        if (!row.Visible)
            return false;

        const bool clicked = ButtonAt(id, btnLabel, TakeRight(row, CalcSmallButtonSize(btnLabel), 8_px), params.Enabled, params.Style, Hax::Gui::Scale(12.f));

        if (params.Hotkey)
            HotkeyAt(params.HotkeyId ? params.HotkeyId : id + 1, *params.Hotkey, TakeRight(row, CalcHotkeySize(*params.Hotkey)));

        if (host)
        {
            const Hax::Rect badge = TakeRight(row, CalcBadgeSize(HostBadgeText()));
            DrawBadgeAt(HostBadgeText(), Theme::Host, badge.Min);
        }

        DrawRowText(id, row, text, hint);
        return clicked;
    }

    void InfoRow(Hax::WStringView label, Hax::WStringView value, Hax::Gui::Color valueCol)
    {
        const float fontH = Hax::Gui::Scale(12.5f);
        const float h = LineH(MAIN_FONT, fontH);
        const Hax::Vector2 size = {Hax::Gui::GetContentRegionAvail().X, h};
        const Hax::Rect bounds = Hax::Rect::FromPosSize(Hax::Gui::GetCursorPos(), size);
        Hax::Gui::PlaceItem(size);
        if (!Hax::Gui::IsItemVisible(bounds))
            return;

        const Hax::Vector2 labelSize = Hax::Gui::CalcTextSize(MAIN_FONT, label, fontH);
        DrawSearchHighlight(label, Hax::Rect::FromPosSize(bounds.Min, labelSize));
        Hax::Gui::DrawString(MAIN_FONT, label, bounds.Min, fontH, {.Color = Theme::Faint});

        const float valueMinX = bounds.Min.X + labelSize.X + 12_px;
        const float maxW = bounds.Max.X - valueMinX;
        const float valueW = Hax::Min(Hax::Gui::CalcTextSize(MAIN_FONT, value, fontH).X, maxW);
        DrawTextFit(MAIN_FONT, value, {bounds.Max.X - valueW, bounds.Min.Y}, fontH, valueCol, maxW);
    }

    int SegmentedRow(size_t id, Hax::WStringView text, const Hax::WStringView* items, int count, int selected, bool enabled, bool fullWidth)
    {
        const float fontH = Hax::Gui::Scale(12.f);
        const float segH = 30_px;
        float itemsW = 0.f;
        float widths[8]{};
        count = Hax::Min(count, 8);
        for (int i = 0; i < count; ++i)
        {
            widths[i] = Hax::Gui::CalcTextSize(MAIN_FONT, items[i], fontH).X + 20_px;
            itemsW += widths[i];
        }
        itemsW += 4_px + 2_px * (float)(count - 1);

        Hax::Rect seg;
        bool visible = true;
        if (fullWidth)
        {
            if (!text.Empty())
                MainLabel(text);
            const Hax::Vector2 size = {Hax::Gui::GetContentRegionAvail().X, segH};
            seg = Hax::Rect::FromPosSize(Hax::Gui::GetCursorPos(), size);
            Hax::Gui::PlaceItem(size);
            visible = Hax::Gui::IsItemVisible(seg);
            const float extra = (size.X - itemsW) / (float)count;
            for (int i = 0; i < count; ++i)
                widths[i] += extra;
        }
        else
        {
            RowLayout row = BeginRow({}, segH);
            visible = row.Visible;
            if (visible)
            {
                seg = TakeRight(row, {itemsW, segH});
                DrawRowText(id, row, text, {});
            }
        }
        if (!visible)
            return -1;

        Hax::Gui::DrawRect(seg.Min, seg.Max, {.BorderColor = Theme::Line, .BorderTh = 1_px, .FillColor = Theme::Surface2, .Rounding = 8_px});

        int clicked = -1;
        float x = seg.Min.X + 2_px;
        for (int i = 0; i < count; ++i)
        {
            const Hax::Rect item = Hax::Rect::FromPosSize({x, seg.Min.Y + 2_px}, {widths[i], segH - 4_px});
            const size_t itemId = id + (size_t)i * 13 + 1;
            if (enabled)
                Hax::Gui::Interact(itemId, item);
            const bool hovered = Hax::Gui::IsItemHovered(itemId);
            if (hovered)
                Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);
            if (i == selected)
                Hax::Gui::DrawRect(item.Min, item.Max, {.FillColor = Theme::Surface3, .Rounding = 6_px});
            else if (hovered)
                Hax::Gui::DrawRect(item.Min, item.Max, {.FillColor = WithAlpha(Theme::Surface3, 0.5f), .Rounding = 6_px});

            const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(MAIN_FONT, items[i], fontH);
            Hax::Gui::DrawString(MAIN_FONT, items[i], item.GetCenter() - textSize / 2.f, fontH,
                {.Color = i == selected ? Theme::Text : Theme::Muted});
            if (Hax::Gui::IsItemClicked(itemId))
                clicked = i;
            x += widths[i] + 2_px;
        }

        if (!enabled)
            Hax::Gui::DrawRect(seg.Min, seg.Max, {.FillColor = Theme::DisabledMask, .Rounding = 8_px});
        return clicked;
    }

    static int StepperAt(size_t id, int value, bool enabled, const Hax::Rect& bounds)
    {
        Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.BorderColor = Theme::Line2, .BorderTh = 1_px, .FillColor = Theme::Surface1, .Rounding = 7_px});

        const float btnW = 26_px;
        int delta = 0;
        for (int i = 0; i < 2; ++i)
        {
            const size_t btnId = id + (size_t)i + 1;
            const Hax::Rect b = i == 0
                ? Hax::Rect{bounds.Min, {bounds.Min.X + btnW, bounds.Max.Y}}
                : Hax::Rect{{bounds.Max.X - btnW, bounds.Min.Y}, bounds.Max};
            const bool can = enabled && (i == 1 || value > 0);
            if (can)
                Hax::Gui::Interact(btnId, b);
            const bool hovered = Hax::Gui::IsItemHovered(btnId);
            if (hovered)
                Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);
            if (hovered)
                Hax::Gui::DrawRect(b.Min + Hax::Vector2(2_px, 2_px), b.Max - Hax::Vector2(2_px, 2_px), {.FillColor = Theme::Surface3, .Rounding = 5_px});
            DrawIcon(i == 0 ? Icon::Minus : Icon::Plus, Hax::Rect::FromPosSize(b.GetCenter() - Hax::Vector2(6_px, 6_px), {12_px, 12_px}),
                can ? (hovered ? Theme::Text : Theme::Muted) : WithAlpha(Theme::Faint, 0.4f));
            if (Hax::Gui::IsItemPressedRepeat(btnId))
                delta = i == 0 ? -1 : 1;
        }

        wchar_t buf[16];
        swprintf_s(buf, L"%d", value);
        const float fontH = Hax::Gui::Scale(12.5f);
        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(BOLD_FONT, buf, fontH);
        Hax::Gui::DrawString(BOLD_FONT, buf, bounds.GetCenter() - textSize / 2.f, fontH, {.Color = enabled ? Theme::Text : Theme::Faint});
        return delta;
    }

    int Stepper(size_t id, int value, bool enabled)
    {
        const Hax::Vector2 size = {84_px, 28_px};
        const Hax::Rect bounds = Hax::Rect::FromPosSize(Hax::Gui::GetCursorPos(), size);
        Hax::Gui::PlaceItem(size);
        if (!Hax::Gui::IsItemVisible(bounds))
            return 0;
        return StepperAt(id, value, enabled, bounds);
    }

    int StepperRow(size_t id, Hax::WStringView text, Hax::WStringView hint, int value, bool enabled, bool hostOnly)
    {
        RowLayout row = BeginRow(hint, 28_px);
        if (!row.Visible)
            return 0;
        const int delta = StepperAt(id, value, enabled, TakeRight(row, {84_px, 28_px}, 8_px));
        if (hostOnly)
        {
            const Hax::Rect badge = TakeRight(row, CalcBadgeSize(HostBadgeText()));
            DrawBadgeAt(HostBadgeText(), Theme::Host, badge.Min);
        }
        DrawRowText(id, row, text, hint);
        return delta;
    }

    int UpgradeCell(size_t id, Hax::WStringView label, int value, bool enabled, float width)
    {
        const Hax::Vector2 size = {width, 44_px};
        const Hax::Rect bounds = Hax::Rect::FromPosSize(Hax::Gui::GetCursorPos(), size);
        Hax::Gui::PlaceItem(size);
        if (!Hax::Gui::IsItemVisible(bounds))
            return 0;

        Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = Theme::Surface2, .Rounding = 8_px});
        const Hax::Rect stepper = Hax::Rect::FromPosSize({bounds.Max.X - 8_px - 84_px, bounds.Min.Y + 8_px}, {84_px, 28_px});
        const int delta = StepperAt(id, value, enabled, stepper);

        const float fontH = Hax::Gui::Scale(12.5f);
        const Hax::Vector2 pos = {bounds.Min.X + 12_px, bounds.GetCenter().Y - LineH(MAIN_FONT, fontH) / 2.f};
        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(MAIN_FONT, label, fontH);
        DrawSearchHighlight(label, Hax::Rect::FromPosSize(pos, textSize));
        DrawTextFit(MAIN_FONT, label, pos, fontH, value > 0 ? Theme::Text : Theme::Muted, stepper.Min.X - pos.X - 8_px);
        return delta;
    }

    bool PlayerRow(size_t id, const PlayerRowData& p)
    {
        const Hax::Vector2 size = {Hax::Gui::GetContentRegionAvail().X, 50_px};
        const Hax::Rect bounds = Hax::Rect::FromPosSize(Hax::Gui::GetCursorPos(), size);
        Hax::Gui::PlaceItem(size);
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;

        const auto res = Hax::Gui::Interact(id, bounds);
        Hax::Gui::LinearAnim& hover = Hax::Gui::GetState<Hax::Gui::LinearAnim>(id);
        AnimateHover(hover, res.Hovered);
        if (res.Hovered)
            Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);

        // Rows run edge to edge inside the card.
        const Hax::Rect full = {{bounds.Min.X - 16_px + 1_px, bounds.Min.Y}, {bounds.Max.X + 16_px - 1_px, bounds.Max.Y}};
        if (p.Selected)
        {
            Hax::Gui::DrawRect(full.Min, full.Max, {.FillColor = Theme::Surface2});
            Hax::Gui::DrawRect(full.Min, {full.Min.X + 2_px, full.Max.Y}, {.FillColor = Theme::Accent});
        }
        else if (hover.Progress > 0.f)
            Hax::Gui::DrawRect(full.Min, full.Max, {.FillColor = WithAlpha(Theme::Surface2, hover.Progress * 0.7f)});
        Hax::Gui::DrawLine({full.Min.X, bounds.Min.Y}, {full.Max.X, bounds.Min.Y}, {.FillColor = Theme::Line, .Th = 1_px});

        // Avatar
        const Hax::Vector2 avC = {bounds.Min.X + 14_px, bounds.GetCenter().Y};
        Hax::Gui::DrawCircle(avC, 14_px, {.FillColor = p.AvatarColor});
        const float avFont = Hax::Gui::Scale(12.f);
        const Hax::Vector2 initialSize = Hax::Gui::CalcTextSize(HEADER_FONT, p.Initial, avFont);
        Hax::Gui::DrawString(HEADER_FONT, p.Initial, avC - initialSize / 2.f, avFont, {.Color = Theme::Ink});

        // HP column on the right, then the status badge.
        const float fontH = Hax::Gui::Scale(12.5f);
        const float hpW = 120_px;
        const float hpX = bounds.Max.X - hpW;
        const float t = p.MaxHp > 0 ? Hax::Clamp((float)p.Hp / (float)p.MaxHp, 0.f, 1.f) : 0.f;
        const float barY = bounds.GetCenter().Y - 2_px;
        const float barEnd = bounds.Max.X - 34_px;
        Hax::Gui::DrawRect({hpX, barY}, {barEnd, barY + 4_px}, {.FillColor = Theme::Surface3, .Rounding = 2_px});
        if (t > 0.f)
            Hax::Gui::DrawRect({hpX, barY}, {Hax::Lerp(hpX, barEnd, t), barY + 4_px},
                {.FillColor = Hax::Lerp(Hax::Gui::LinearColor(Theme::Danger), Hax::Gui::LinearColor(Theme::Ok), t).ToColor(), .Rounding = 2_px});
        wchar_t hp[16];
        swprintf_s(hp, L"%d", p.Hp);
        const Hax::Vector2 hpSize = Hax::Gui::CalcTextSize(BOLD_FONT, hp, Hax::Gui::Scale(11.5f));
        Hax::Gui::DrawString(BOLD_FONT, hp, {bounds.Max.X - hpSize.X, bounds.GetCenter().Y - hpSize.Y / 2.f}, Hax::Gui::Scale(11.5f), {.Color = Theme::Faint});

        const Hax::Vector2 badgeSize = CalcBadgeSize(p.Status);
        const float badgeX = hpX - 16_px - badgeSize.X;
        DrawBadgeAt(p.Status, p.StatusColor, {badgeX, bounds.GetCenter().Y - badgeSize.Y / 2.f});

        // Name and tag
        const float nameX = bounds.Min.X + 40_px;
        const Hax::Vector2 nameSize = Hax::Gui::CalcTextSize(MAIN_FONT, p.Name, fontH);
        const float maxName = badgeX - nameX - 10_px;
        DrawTextFit(MAIN_FONT, p.Name, {nameX, bounds.GetCenter().Y - nameSize.Y / 2.f}, fontH, Theme::Text, maxName);
        if (!p.Tag.Empty() && nameSize.X + 60_px < maxName)
            Hax::Gui::DrawString(DESC_FONT, p.Tag, {nameX + nameSize.X + 6_px, bounds.GetCenter().Y - nameSize.Y / 2.f}, Hax::Gui::Scale(11.5f), {.Color = Theme::Faint});

        return Hax::Gui::IsItemClicked(id);
    }

    bool PanelHeader(Hax::WStringView text, Hax::WStringView desc, const CardHeaderParams& params)
    {
        const bool host = params.HostOnly || IsHostDesc(desc);
        const Hax::WStringView tip = IsHostDesc(desc) ? Hax::WStringView() : desc;
        const size_t id = params.Id ? params.Id : (Hax::Hash(text) ^ 0x434152444844ull);

        const float rowH = 26_px;
        const Hax::Vector2 size = {Hax::Gui::GetContentRegionAvail().X, rowH};
        const Hax::Rect bounds = Hax::Rect::FromPosSize(Hax::Gui::GetCursorPos(), size);
        Hax::Gui::PlaceItem(size);

        bool result = false;
        if (Hax::Gui::IsItemVisible(bounds))
        {
            RowLayout row{bounds, bounds.Max.X, rowH, true};

            if (params.Toggle)
                result |= ToggleAt(id + 1, *params.Toggle, !params.ToggleDisabled, TakeRight(row, Hax::Gui::Scale(Theme::ToggleSize), 10_px));
            if (!params.Action.Empty())
                result |= ButtonAt(id + 2, params.Action, TakeRight(row, CalcSmallButtonSize(params.Action), 8_px),
                    params.ActionEnabled, params.ActionStyle, Hax::Gui::Scale(12.f));
            if (params.SettingsId != 0)
            {
                const Hax::Rect gear = TakeRight(row, {16_px, 16_px}, 10_px);
                if (SettingsBtnAt(id + 3, false, gear))
                    OpenPopup(params.SettingsId, {gear.Min.X, gear.Max.Y + 6_px});
            }
            if (params.Hotkey)
                HotkeyAt(params.HotkeyId ? params.HotkeyId : id + 4, *params.Hotkey, TakeRight(row, CalcHotkeySize(*params.Hotkey)));
            if (host)
            {
                const Hax::Rect badge = TakeRight(row, CalcBadgeSize(HostBadgeText()));
                DrawBadgeAt(HostBadgeText(), Theme::Host, badge.Min);
            }
            if (!params.Note.Empty())
            {
                const float noteH = Hax::Gui::Scale(11.5f);
                const Hax::Vector2 noteSize = Hax::Gui::CalcTextSize(DESC_FONT, params.Note, noteH);
                const Hax::Rect note = TakeRight(row, noteSize);
                Hax::Gui::DrawString(DESC_FONT, params.Note, note.Min, noteH, {.Color = Theme::Faint});
            }

            float x = bounds.Min.X;
            if (params.Icon != Icon::None)
            {
                DrawIcon(params.Icon, Hax::Rect::FromPosSize({x, bounds.Min.Y + (rowH - 16_px) / 2.f}, {16_px, 16_px}), Theme::Accent);
                x += 16_px + 9_px;
            }

            const float fontH = Hax::Gui::Scale(Theme::TitleFontSize);
            const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(HEADER_FONT, text, fontH);
            const Hax::Vector2 textPos = {x, bounds.Min.Y + (rowH - textSize.Y) / 2.f};
            DrawSearchHighlight(text, Hax::Rect::FromPosSize(textPos, textSize));
            DrawTextFit(HEADER_FONT, text, textPos, fontH, Theme::Muted, row.RightX - x - 4_px);
            TextTooltip(id ^ 0x50414E454C484452ull, tip, Hax::Rect::FromPosSize(textPos, textSize));
        }

        if (!params.NoDivider)
            HorizontalLine(1_px);

        return result;
    }

    bool HotkeyEx(size_t id, int& key, Hax::WStringView label, Hax::WStringView desc)
    {
        RowLayout row = BeginRow(desc, 24_px);
        if (!row.Visible)
            return false;
        const bool changed = HotkeyAt(id, key, TakeRight(row, CalcHotkeySize(key), 8_px));
        DrawRowText(id, row, label, desc, Theme::Text);
        return changed;
    }

    //=========================================================================
    // Values
    //=========================================================================

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

    void Image(Hax::Gui::TextureHandle tex, Hax::Vector2 customSize, float rounding)
    {
        const Hax::Vector2 size = (bool)customSize ? customSize : Hax::Gui::GetImageSize(tex);
        const Hax::Rect bounds = Hax::Rect::FromPosSize(Hax::Gui::GetCursorPos(), size);
        Hax::Gui::PlaceItem(size);
        if (!Hax::Gui::IsItemVisible(bounds))
            return;
        Hax::Gui::DrawImage(tex, bounds.Min, bounds.Max, {.R = rounding});
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
        const Hax::Vector2 size = {w, LineH(MAIN_FONT, fontH) + padding.Y * 2.f};
        const Hax::Rect bounds = Hax::Rect::FromPosSize(Hax::Gui::GetCursorPos(), size);

        Hax::Gui::PlaceItem(size);
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;

        Hax::Gui::Interact(id, bounds);
        const ButtonColors c = ButtonBeh(id, ButtonStyle::Secondary);

        Hax::Gui::DrawRect(bounds.Min, bounds.Max,
            {.BorderColor = c.Border, .BorderTh = 1_px, .FillColor = c.Bg, .Rounding = Hax::Gui::Scale(Theme::ControlR)});
        DrawTextFit(MAIN_FONT, preview, bounds.Min + padding, fontH, Theme::Text, size.X - padding.X * 2.f - 18_px);
        DrawIcon(Icon::ChevronDown, Hax::Rect::FromPosSize({bounds.Max.X - padding.X - 12_px, bounds.GetCenter().Y - 6_px}, {12_px, 12_px}), Theme::Faint);

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
        const float markSize = 12_px;
        const Hax::Vector2 padding = {10_px, 7_px};
        const float markSpace = selected ? markSize + padding.X : 0.f;

        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(MAIN_FONT, label, fontH);
        const Hax::Vector2 btnSize = {Hax::Max(textSize.X + padding.X * 2.f + markSpace, params.MinW), textSize.Y + padding.Y * 2.f};

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

        const float r = params.AutoLayout ? 8_px : 6_px;
        const Hax::Gui::Color bg = selected ? Theme::AccentSoft : (hovered ? Theme::Surface3 : (params.AutoLayout ? Theme::Surface2 : 0x00000000));
        const Hax::Gui::Color border = params.AutoLayout ? (selected ? 0xF5A52466 : Theme::Line) : Hax::Gui::Color::Transparent;
        Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.BorderColor = border, .BorderTh = 1_px, .FillColor = bg, .Rounding = r});
        Hax::Gui::DrawString(MAIN_FONT, label, bounds.Min + padding, fontH, {.Color = selected ? Theme::Accent : Theme::Text});
        if (selected)
            DrawIcon(Icon::Check, Hax::Rect::FromPosSize({bounds.Max.X - padding.X - markSize, bounds.GetCenter().Y - markSize / 2.f}, {markSize, markSize}), Theme::Accent);

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
        return Hax::Gui::CalcTextSize(MAIN_FONT, label, fontH) + Hax::Vector2(9_px, 5_px) * 2.f;
    }

    bool RepeatBtn(size_t id, Hax::WStringView label, bool enabled)
    {
        const float fontH = Hax::Gui::Scale(Theme::DescFontSize);
        const Hax::Vector2 btnSize = CalcRepeatBtnSize(label);
        const Hax::Rect bounds = Hax::Rect::FromPosSize(Hax::Gui::GetCursorPos(), btnSize);

        Hax::Gui::PlaceItem(btnSize);
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;

        ButtonAt(id, label, bounds, enabled, ButtonStyle::Secondary, fontH);
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
            {.BorderColor = wasFocused ? Theme::Accent : Theme::Line2, .BorderTh = 1_px, .FillColor = Theme::Surface2, .Rounding = 7_px});

        bool changed = false;
        const Hax::Vector2 padding = {10_px, 5_px};
        const Hax::Rect editBounds = {bounds.Min + padding, bounds.Max - padding};
        if (enabled)
        {
            changed = Hax::Gui::StringEdit(id, BOLD_FONT, state.Buffer, _countof(state.Buffer), fontH, editBounds,
                {.CaretColor = Theme::Accent, .Hint = L"0", .HintColor = Theme::Faint, .TextColor = Theme::Text});

            if (Hax::Gui::IsItemFocused(id))
                s_TextInputTime = Hax::Gui::GetTime();

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

    bool Checkbox(size_t id, bool& val)
    {
        const float size = Hax::Gui::Scale(Theme::CheckboxSize);
        const Hax::Rect bounds = Hax::Rect::FromPosSize(Hax::Gui::GetCursorPos(), {size, size});

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
            DrawIcon(Icon::Check, Hax::Rect::FromPosSize(bounds.Min + Hax::Vector2(2_px, 2_px), {size - 4_px, size - 4_px}), Theme::Ink);
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
