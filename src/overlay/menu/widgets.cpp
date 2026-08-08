#include <src/pch.h>

#include "widgets.h"

#include <src/cheat.h>

namespace Cheat
{
    namespace Theme
    {
        // R.E.P.O. extraction-console palette: soot-black metal, aged brass,
        // amber controls and toxic-green status lights.
        constexpr Hax::Gui::Color WindowBg = 0x090A07FF;
        constexpr Hax::Gui::Color SidePanelBg = 0x11120CFF;
        constexpr float WindowR = 4.f;
        constexpr float MainFontSize = 13.f;
        constexpr float DescFontSize = 12.f;
        constexpr float TitleFontSize = 12.f;
        constexpr Hax::Gui::LinearColor BtnBg = 0x17170FFF;
        constexpr Hax::Gui::Color BtnBgActive = 0xC77A0CFF;
        constexpr Hax::Gui::Color DescColor = 0x978D67FF;
        constexpr Hax::Gui::Color DisabledMaskCol = 0x090A07C8;
        constexpr Hax::Gui::Color SeparatorCol = 0x504629FF;
        constexpr Hax::Gui::Color MainCol = 0xD8D0ACFF;
        constexpr Hax::Gui::Color PopupBg = 0x12130DFF;
        constexpr Hax::Gui::Color FrameColor = 0x1B1C13FF;
        constexpr Hax::Gui::Color ActiveColor = 0xE89416FF;
        constexpr Hax::Gui::Color HeaderColor = 0xE8B93FFF;
        constexpr Hax::Gui::Color MetalBorder = 0x65542BFF;
        constexpr Hax::Gui::Color MetalHighlight = 0x9A7732FF;
        constexpr Hax::Gui::Color StatusGreen = 0x9ED52AFF;
        constexpr Hax::Gui::Color AlertRed = 0xE34B22FF;

        #define DESC_FONT G->NunitoSans_SemiBold
        #define MAIN_FONT G->NunitoSans_Bold
        #define HEADER_FONT G->NunitoSans_ExtraBold
        #define ICONS_FONT G->Icons_Solid

        constexpr Hax::Vector2 ToggleSize = {39.f, 20.f};
        constexpr float SettingBtnSize = 16.f;
        constexpr float CheckboxSize = 16.f;
        constexpr Hax::Vector2 WindowSize = {1320.f, 740.f};
    }

    void Label(Hax::Gui::FontHandle hFont, Hax::WStringView text, float fontH, Hax::Gui::Color col)
    {
        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(hFont, text, fontH);

        Hax::Rect bounds;
        bounds.Min = Hax::Gui::GetCursorPos();
        bounds.Max = bounds.Min + textSize;

        Hax::Gui::PlaceItem(textSize);
        if (Hax::Gui::IsItemVisible(bounds))
            Hax::Gui::DrawString(hFont, text, bounds.Min, fontH, {.Color = col});
    }

    void MainLabel(Hax::WStringView text)
    {
        Label(MAIN_FONT, text, Theme::MainFontSize * Hax::Gui::G.ScaleFactor, Theme::MainCol);
    }

    void MainLabel(Hax::WStringView text, const Hax::Gui::Color& col)
    {
        Label(MAIN_FONT, text, Theme::MainFontSize * Hax::Gui::G.ScaleFactor, col);
    }

    Hax::Vector2 CalcMainLabelSize(Hax::WStringView text)
    {
        return Hax::Gui::CalcTextSize(MAIN_FONT, text, Hax::Gui::Scale(Theme::MainFontSize));
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
        if (Hax::Gui::GetTime() - hoverStartedAt < 0.22)
            return;

        const float fontH = Hax::Gui::Scale(Theme::DescFontSize);
        const Hax::Vector2 padding = {10_px, 7_px};
        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(DESC_FONT, text, fontH);
        const Hax::Vector2 size = textSize + padding * 2.f;
        const Hax::Vector2 viewport = Hax::Gui::GetViewportSize();
        Hax::Vector2 pos = mouse + Hax::Vector2(14_px, 18_px);
        pos.X = Hax::Clamp(pos.X, 8_px, Hax::Max(8_px, viewport.X - size.X - 8_px));
        pos.Y = Hax::Clamp(pos.Y, 8_px, Hax::Max(8_px, viewport.Y - size.Y - 8_px));

        Hax::Gui::SwitchLayer(L"Popups");
        Hax::Gui::DrawRect(pos + Hax::Vector2(3_px, 3_px), pos + size + Hax::Vector2(3_px, 3_px),
            {.FillColor = 0x000000A8, .Rounding = 2_px});
        Hax::Gui::DrawRect(pos, pos + size,
            {.BorderColor = 0xA87A28FF, .BorderTh = 1_px, .FillColor = 0x11120DFA, .Rounding = 2_px});
        Hax::Gui::DrawRect(pos, {pos.X + 3_px, pos.Y + size.Y},
            {.FillColor = Theme::ActiveColor, .Rounding = Hax::Vector4(2_px, 0.f, 0.f, 2_px)});
        Hax::Gui::DrawString(DESC_FONT, text, pos + padding, fontH, {.Color = 0xD7CDA7FF});
        Hax::Gui::RestoreLayer();
    }

    void LabelAlignedByH(Hax::Gui::FontHandle hFont, Hax::WStringView label, float fontH, const Hax::Gui::Color& col, float h)
    {
        const Hax::Gui::FontMetrics metrics = Hax::Gui::GetFontMetrics(hFont, fontH);

        Hax::Gui::BeginVertical();
        {
            //Hax::Gui::Space((h - metrics.CapHeight) / 2.f - (metrics.Ascent - metrics.CapHeight));
            Hax::Gui::Space((h - metrics.LineHeight) / 2.f);
            Label(hFont, label, fontH, col);
        }
        Hax::Gui::EndVertical();
    }

    void MainLabelAlignedByH(Hax::WStringView label, float h)
    {
        LabelAlignedByH(MAIN_FONT, label, Hax::Gui::Scale(Theme::MainFontSize), Theme::MainCol, h);
    }

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

        Hax::Gui::SetCursorPos(windowBounds.Min);
        Hax::Gui::BeginContainer(WINDOW_ID, {.W = windowSize.X, .H = windowSize.Y, .Clip = true});
        Hax::Gui::BeginHorizontal();

        const float r = Hax::Gui::Scale(Theme::WindowR);
        Hax::Gui::DrawRect(windowBounds.Min, windowBounds.Max,
            {.BorderColor = 0x8A6A2DFF, .BorderTh = 2_px, .FillColor = Theme::WindowBg, .Rounding = r});
        Hax::Gui::DrawRect(windowBounds.Min + Hax::Vector2(5_px, 5_px), windowBounds.Max - Hax::Vector2(5_px, 5_px),
            {.BorderColor = 0x332D1BFF, .BorderTh = 1_px, .FillColor = Hax::Gui::Color::Transparent, .Rounding = 2_px});
    }

    void EndWindow()
    {
        Hax::Gui::EndHorizontal();
        Hax::Gui::EndContainer();
    }

    bool BeginSidePanel()
    {
        const Hax::Vector2 size = {236_px, Hax::Gui::GetContentRegionAvail().Y};

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
        Hax::Gui::DrawRect(bounds.Min, bounds.Max,
            {.BorderColor = Theme::MetalBorder, .BorderTh = 1_px, .FillColor = Theme::SidePanelBg, .Rounding = Hax::Vector4(r, 0.f, 0.f, r)});
        Hax::Gui::DrawLine({bounds.Max.X - 4_px, bounds.Min.Y + 8_px}, {bounds.Max.X - 4_px, bounds.Max.Y - 8_px},
            {.FillColor = 0x2F2A19FF, .Th = 2_px});

        Hax::Gui::BeginVertical();
        return true;
    }

    void EndSidePanel()
    {
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();
    }

    void PanelHeader(Hax::WStringView text, Hax::WStringView desc)
    {
        const float fontH = Hax::Gui::Scale(Theme::TitleFontSize);
        const Hax::Vector2 pos = Hax::Gui::GetCursorPos();
        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(HEADER_FONT, text, fontH);
        Label(HEADER_FONT, text, fontH, Theme::HeaderColor);
        TextTooltip(Hax::Hash(text) ^ 0x50414E454C484452ull, desc, Hax::Rect::FromPosSize(pos, textSize));
    }

    bool SettingsBtn(size_t id, bool disabled)
    {
        const float h = Theme::SettingBtnSize * Hax::Gui::G.ScaleFactor;
        const Hax::Vector2 size = Hax::Gui::CalcTextSize(ICONS_FONT, L"\uF013", h);
        const Hax::Rect bounds = Hax::Rect::FromPosSize(Hax::Gui::GetCursorPos(), size);

        Hax::Gui::PlaceItem(size);
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;

        if (!disabled)
            Hax::Gui::Interact(id, bounds);
        Hax::Gui::LinearAnim& state = Hax::Gui::GetState<Hax::Gui::LinearAnim>(id);

        const float deltaTime = Hax::Gui::GetDeltaTime();
        if (Hax::Gui::IsItemHovered(id))
        {
            Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);
            state.Elapse(deltaTime, 0.1f);
        }
        else
            state.Elapse(-deltaTime, 0.1f);

        const Hax::Gui::Color color = Hax::Gui::IsItemActive(id) ? Theme::ActiveColor
            : Hax::Lerp(Hax::Gui::LinearColor(0x8E835FFF), Hax::Gui::LinearColor(0xF0C14AFF), state.Progress).ToColor();
        Hax::Gui::DrawString(ICONS_FONT, L"\uF013", bounds.Min, h, {.Color = color});

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
        state.ToggleAnim.Elapse(value ? deltaTime : -deltaTime, 0.1f);

        if (enabled)
            Hax::Gui::Interact(id, bounds);

        if (Hax::Gui::IsItemHovered(id))
        {
            state.HoverAnim.Elapse(deltaTime, 0.1f);
            Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);
        }
        else
            state.HoverAnim.Elapse(-deltaTime, 0.1f);

        if (Hax::Gui::IsItemClicked(id))
            value = !value;

        const Hax::Gui::Color border = Hax::Lerp(Hax::Gui::LinearColor(0x55492AFF), Hax::Gui::LinearColor(0xB98A32FF), state.HoverAnim.Progress).ToColor();
        Hax::Gui::DrawRect(bounds.Min, bounds.Max,
            {.BorderColor = border, .BorderTh = 1_px, .FillColor = 0x0D0E09FF, .Rounding = 2_px});

        const float lampW = 15_px;
        const float lampX = Hax::Lerp(bounds.Min.X + 3_px, bounds.Max.X - lampW - 3_px, state.ToggleAnim.Progress);
        const Hax::Rect lamp = Hax::Rect::FromPosSize({lampX, bounds.Min.Y + 3_px}, {lampW, bounds.GetHeight() - 6_px});
        const Hax::Gui::Color lampColor = Hax::Lerp(Hax::Gui::LinearColor(0x49301FFF), Hax::Gui::LinearColor(Theme::StatusGreen), state.ToggleAnim.Progress).ToColor();
        Hax::Gui::DrawRect(lamp.Min, lamp.Max,
            {.BorderColor = state.ToggleAnim.Progress > 0.5f ? 0xC5E55AFF : 0x68412AFF, .BorderTh = 1_px, .FillColor = lampColor, .Rounding = 2_px});

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
                LabelAlignedByH(MAIN_FONT, text, Hax::Gui::Scale(Theme::MainFontSize), Theme::MainCol, toggleSize.Y);
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
                    Hax::Gui::DrawRect(bounds.Min, bounds.Max, { .FillColor = Theme::DisabledMaskCol });
                }
            }
            Hax::Gui::EndHorizontal();

        }
        Hax::Gui::EndVertical();

        return changed;
    }

    Hax::Vector2 CalcButtonSize(Hax::WStringView label)
    {
        const float fontH = Theme::DescFontSize * Hax::Gui::G.ScaleFactor;
        const Hax::Vector2 padding = {12_px, 5_px};

        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(MAIN_FONT, label, fontH);
        return textSize + padding * 2.f;
    }

    float CalcButtonHeight()
    {
        const float fontH = Theme::DescFontSize * Hax::Gui::G.ScaleFactor;
        return Hax::Gui::GetFontLineHeight(MAIN_FONT, fontH) + 5_px * 2.f;
    }

    bool Button(size_t id, Hax::WStringView label, Hax::WStringView desc, const ButtonParams& params)
    {
        const float fontH = Theme::DescFontSize * Hax::Gui::G.ScaleFactor;
        const Hax::Vector2 padding = {12_px, 5_px};

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

        Hax::Gui::LinearAnim& state = Hax::Gui::GetState<Hax::Gui::LinearAnim>(id);

        Hax::Gui::Color bg = 0x6B470FFF;
        Hax::Gui::Color fg = 0x0B0C08FF;
        if (!Hax::Gui::IsItemActive(id))
        {
            constexpr Hax::Gui::LinearColor fromBg = 0x1A1A11FF;
            constexpr Hax::Gui::LinearColor toBg = 0x7B5312FF;
            bg = Hax::Lerp(fromBg, toBg, state.Progress).ToColor();

            constexpr Hax::Gui::LinearColor fromFg = 0xD0C59AFF;
            constexpr Hax::Gui::LinearColor toFg = 0xFFE17AFF;
            fg = Hax::Lerp(fromFg, toFg, state.Progress).ToColor();
        }

        const float deltaTime = Hax::Gui::GetDeltaTime();
        if (Hax::Gui::IsItemHovered(id))
        {
            state.Elapse(deltaTime, 0.1f);
            Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);
        }
        else
            state.Elapse(-deltaTime, 0.1f);

        Hax::Gui::DrawRect(bounds.Min, bounds.Max,
            {.BorderColor = 0x8A6A2DFF, .BorderTh = 1_px, .FillColor = bg, .Rounding = 2_px});
        const Hax::Vector2 labelPos = bounds.Min + (btnSize - textSize) / 2.f;
        Hax::Gui::DrawString(MAIN_FONT, label, labelPos, fontH, {.Color = fg});
        TextTooltip(id ^ 0x425554544F4E4849ull, desc, Hax::Rect::FromPosSize(labelPos, textSize));

        if (!params.Enabled)
            Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = Theme::DisabledMaskCol});

        return Hax::Gui::IsItemClicked(id);
    }

    void Image(Hax::Gui::TextureHandle tex, Hax::Vector2 customSize)
    {
        const Hax::Vector2 size = (bool)customSize ? customSize : Hax::Gui::GetImageSize(tex);

        Hax::Rect bounds;
        bounds.Min = Hax::Gui::GetCursorPos();
        bounds.Max = bounds.Min + size;

        Hax::Gui::PlaceItem(size);
        if (!Hax::Gui::IsItemVisible(bounds))
            return;

        Hax::Gui::DrawImage(tex, bounds.Min, bounds.Max);
    }

    bool TabButton(size_t id, Hax::WStringView text, Hax::char16 icon16, bool active)
    {
        const float fontSize = Hax::Gui::Scale(Theme::MainFontSize);
        const float iconHeight = Hax::Gui::GetFontLineHeight(MAIN_FONT, fontSize);
        const Hax::Vector2 padding = {12_px, 7_px};

        Hax::Vector2 btnSize;
        btnSize.X = Hax::Gui::GetContentRegionAvail().X;
        btnSize.Y = Hax::Gui::GetFontLineHeight(MAIN_FONT, fontSize) + padding.Y * 2.f;

        Hax::Rect btnBounds;
        btnBounds.Min = Hax::Gui::GetCursorPos();
        btnBounds.Max = btnBounds.Min + btnSize;

        Hax::Gui::PlaceItem(btnSize);
        if (!Hax::Gui::IsItemVisible(btnBounds))
            return false;

        Hax::Gui::LinearAnim& state = Hax::Gui::GetState<Hax::Gui::LinearAnim>(id);

        // Interaction
        {
            auto res = Hax::Gui::Interact(id, btnBounds);
            const float deltaTime = Hax::Gui::GetDeltaTime();
            state.Elapse(res.Hovered ? deltaTime : -deltaTime, 0.1f);
            if (res.Hovered)
                Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);
        }

        // Background
        {
            const Hax::Gui::Color btnColor = active ? Theme::BtnBgActive
                : Hax::Lerp(Hax::Gui::LinearColor(0x12130DFF), Hax::Gui::LinearColor(0x2B2818FF), state.Progress).ToColor();
            Hax::Gui::DrawRect(btnBounds.Min, btnBounds.Max,
                {.BorderColor = active ? 0xE6A525FF : 0x51472BFF, .BorderTh = 1_px, .FillColor = btnColor, .Rounding = 2_px});
            if (active)
                Hax::Gui::DrawRect(btnBounds.Min, {btnBounds.Min.X + 4_px, btnBounds.Max.Y}, {.FillColor = 0xF0B52DFF, .Rounding = 1_px});
        }

        const Hax::WStringView icon = {&icon16, 1};
        const Hax::Gui::Color labelColor = active ? 0x17130AFF
            : Hax::Lerp(Hax::Gui::LinearColor(0x8F8769FF), Hax::Gui::LinearColor(0xE4D6A4FF), state.Progress).ToColor();

        Hax::Vector2 posCursor = btnBounds.Min;
        posCursor.Y += (btnSize.Y - iconHeight) / 2.f;
        posCursor.X += padding.X;
        Hax::Gui::DrawString(ICONS_FONT, icon, posCursor, iconHeight, {.Color = labelColor});

        const float capHeight = Hax::Gui::GetFontCapHeight(MAIN_FONT, fontSize);
        const Hax::Vector2 labelSize = Hax::Gui::CalcTextSize(MAIN_FONT, text, fontSize);
        posCursor.X += 18_px + padding.X;
        posCursor.Y += (iconHeight - capHeight) / 2.f - (Hax::Gui::GetFontAscent(MAIN_FONT, fontSize) - capHeight);
        Hax::Gui::DrawString(MAIN_FONT, text, posCursor, fontSize, {.Color = active ? 0x17130AFF : 0xD9D0ACFF});

        return Hax::Gui::IsItemClicked(id);
    }

    bool SubTabButton(size_t id, Hax::WStringView text, bool active, float width)
    {
        const float fontSize = Hax::Gui::Scale(Theme::MainFontSize);
        const Hax::Vector2 padding = {14_px, 8_px};
        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(MAIN_FONT, text, fontSize);
        const Hax::Vector2 size = {width, textSize.Y + padding.Y * 2.f};
        const Hax::Rect bounds = Hax::Rect::FromPosSize(Hax::Gui::GetCursorPos(), size);

        Hax::Gui::PlaceItem(size);
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;

        const auto interaction = Hax::Gui::Interact(id, bounds);
        Hax::Gui::LinearAnim& state = Hax::Gui::GetState<Hax::Gui::LinearAnim>(id);
        state.Elapse(interaction.Hovered ? Hax::Gui::GetDeltaTime() : -Hax::Gui::GetDeltaTime(), 0.12f);
        if (interaction.Hovered)
            Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);

        const Hax::Gui::Color bg = active
            ? 0x6A4A12FF
            : Hax::Lerp(Hax::Gui::LinearColor(0x15160FFF), Hax::Gui::LinearColor(0x292719FF), state.Progress).ToColor();
        Hax::Gui::DrawRect(bounds.Min, bounds.Max,
            {.BorderColor = active ? 0xD49B27FF : 0x4B4228FF, .BorderTh = 1_px, .FillColor = bg, .Rounding = 2_px});

        if (active)
        {
            Hax::Vector2 accentMax = bounds.Max;
            accentMax.Y = bounds.Min.Y + 3_px;
            Hax::Gui::DrawRect(bounds.Min, accentMax, {.FillColor = Theme::ActiveColor, .Rounding = 1_px});
        }

        Hax::Gui::DrawString(MAIN_FONT, text, bounds.Min + (size - textSize) / 2.f, fontSize,
            {.Color = active ? 0xFFE17AFF : 0xAFA684FF});

        return Hax::Gui::IsItemClicked(id);
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
                {.BorderColor = Theme::MetalBorder, .BorderTh = 1_px, .FillColor = 0x12130DFF, .Rounding = 3_px});
            Hax::Gui::DrawLine({a.X + 7_px, a.Y + 4_px}, {b.X - 7_px, a.Y + 4_px}, {.FillColor = 0x2E2919FF, .Th = 1_px});
            constexpr float boltR = 1.5f;
            Hax::Gui::DrawCircle(a + Hax::Vector2(6_px, 6_px), Hax::Gui::Scale(boltR), {.FillColor = 0x8B7342FF});
            Hax::Gui::DrawCircle({b.X - 6_px, a.Y + 6_px}, Hax::Gui::Scale(boltR), {.FillColor = 0x8B7342FF});
        }
        else
            Hax::Gui::PushSkipDrawing();

        Hax::Gui::BeginHorizontal();
        const float padding = 12_px;
        Hax::Gui::Space(padding);
        Hax::Gui::BeginContainer(id + 1, {.W = containerSize.X - padding * 2.f, .H = 15.f, .FitY = true});
        Hax::Gui::BeginVertical(padding);
        Hax::Gui::Dummy({0.f, 0.f});
    }

    void EndPanel()
    {
        Hax::Gui::Dummy({0.f, 0.f});
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();
        Hax::Gui::EndHorizontal();

        if (Hax::Gui::IsDrawingSkipped())
            Hax::Gui::PopSkipDrawing();

        Hax::Gui::EndContainer();
    }

    int SliderConvertInt(int min, int max, float m)
    {
        return Hax::Clamp(min + (int)Hax::Round((float)(max - min) * m), min, max);
    }

    float SliderConvertFloat(float min, float max, float m)
    {
        return Hax::Lerp(min, max, Hax::Clamp(m, 0.f, 1.f));
    }

    //template <float Step>
    //float SliderConvertFloatStep(float min, float max, float m)
    //{
    //    float rawVal = min + m * (max - min);
    //
    //    float stepsCount = Round((rawVal - min) / Step);
    //
    //    float finalVal = min + (stepsCount * Step);
    //    return Clamp(finalVal, min, max);
    //}

    void ButtonBeh(size_t id, Hax::Gui::Color& bg, Hax::Gui::Color& fg)
    {
        Hax::Gui::LinearAnim& state = Hax::Gui::GetState<Hax::Gui::LinearAnim>(id);

        bg = 0x6B470FFF;
        fg = 0x1A1408FF;
        if (!Hax::Gui::IsItemActive(id))
        {
            constexpr Hax::Gui::LinearColor fromBg = 0x17180FFF;
            constexpr Hax::Gui::LinearColor toBg = 0x3B341CFF;
            bg = Hax::Lerp(fromBg, toBg, state.Progress).ToColor();

            constexpr Hax::Gui::LinearColor fromFg = 0xC8BE94FF;
            constexpr Hax::Gui::LinearColor toFg = 0xF2C64FFF;
            fg = Hax::Lerp(fromFg, toFg, state.Progress).ToColor();
        }

        const float deltaTime = Hax::Gui::GetDeltaTime();
        if (Hax::Gui::IsItemHovered(id))
        {
            state.Elapse(deltaTime, 0.1f);
            Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);
        }
        else
            state.Elapse(-deltaTime, 0.1f);
    }

    bool AutoLayoutButton(size_t id, Hax::WStringView text, const AutoLayoutButtonParams& params)
    {
        const float fontH = Theme::DescFontSize * Hax::Gui::G.ScaleFactor;
        const Hax::Vector2 padding = {12_px, 5_px};

        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(MAIN_FONT, text, fontH);
        Hax::Vector2 btnSize = textSize + padding * 2.f;

        Hax::Rect bounds; 
        bounds.Min = Hax::Gui::GetCursorPos();
        bounds.Max = bounds.Min + btnSize;

        if (params.Icon != 0)
        {
            Hax::Vector2 iconSize = Hax::Gui::GetImageSize(params.Icon);
            bounds.Max.X += padding.X + (textSize.Y * (iconSize.X / iconSize.Y));
            btnSize = bounds.GetSize();
        }

        if (btnSize.X > Hax::Gui::GetContentRegionAvail().X)
        {
            Hax::Gui::EndHorizontal();
            Hax::Gui::BeginHorizontal(5_px);

            bounds.Min = Hax::Gui::GetCursorPos();
            bounds.Max = bounds.Min + btnSize;
        }

        Hax::Gui::PlaceItem(btnSize);
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;

        if (!params.Disabled)
            Hax::Gui::Interact(id, bounds);

        Hax::Gui::Color bg, fg;
        ButtonBeh(id, bg, fg);

        Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.BorderColor = 0x62532CFF, .BorderTh = 1_px, .FillColor = bg, .Rounding = 2_px});

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
        Hax::Gui::DrawString(MAIN_FONT, text, drawPos, fontH, {.Color = fg});

        if (params.Disabled)
            Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = Theme::DisabledMaskCol});

        return Hax::Gui::IsItemClicked(id);
    }

    bool DropdownBtn(size_t id, Hax::WStringView preview, float w)
    {
        const float fontH = Theme::DescFontSize * Hax::Gui::G.ScaleFactor;
        const Hax::Vector2 padding = {12_px, 5_px};//!
        const float textH = Hax::Gui::GetFontLineHeight(MAIN_FONT, fontH);

        const Hax::Vector2 size = {w, textH + padding.Y * 2.f};
        Hax::Rect bounds; bounds.Min = Hax::Gui::GetCursorPos(); bounds.Max = bounds.Min + size;

        Hax::Gui::PlaceItem(size);
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;

        Hax::Gui::Interact(id, bounds);

        Hax::Gui::Color bg, fg;
        ButtonBeh(id, bg, fg);

        Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.BorderColor = 0x62532CFF, .BorderTh = 1_px, .FillColor = bg, .Rounding = 2_px});
        Hax::Gui::DrawString(MAIN_FONT, preview, bounds.Min + padding, fontH, {.Color = fg});

        const Hax::Vector2 arrowSize = padding;
        Hax::Vector2 arrowPos;
        arrowPos.X = bounds.Max.X - arrowSize.X - padding.X;
        arrowPos.Y = bounds.Min.Y + (bounds.GetHeight() - arrowSize.Y) / 2.f;

        Hax::Gui::DrawTriangle(arrowPos, arrowPos + Hax::Vector2(arrowSize.X, 0.f), arrowPos + Hax::Vector2(arrowSize.X / 2.f, arrowSize.Y), {.FillColor = Theme::ActiveColor});

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

    bool BeginDropList(size_t id, const Hax::Vector2& size)
    {
        PopupState& state = Hax::Gui::GetState<PopupState>(id);

        if (!state.Opened)
            return false;

        Hax::Gui::SwitchLayer(L"Popups");
        Hax::Gui::SetCursorPos(state.Pos);

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

        const float r = 2_px;
        Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.BorderColor = Theme::MetalHighlight, .BorderTh = 1_px, .FillColor = Theme::FrameColor, .Rounding = r});

        bounds.Min.Y += r;
        bounds.Max.Y -= r;

        Hax::Gui::SetCursorPos(bounds.Min);
        Hax::Gui::ScrollStyle ScrollSt = 
        {
            .TrackWidth = 6_px,
            .ThumbPadding = 0.f,
            .TrackCol = 0x0,
            .ThumbCol = 0x8F6D29FF,
            .ThumbHovCol = 0xC58B23FF,
            .ThumbActiveCol = 0xE5A32BFF
        };
        Hax::Gui::BeginContainer(id, {.W = size.X, .H = size.Y - r * 2.f, .Clip = true, .ScrollY = true, .Style = ScrollSt});
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
            const float r = 2_px;
            Hax::Gui::DrawRect(bounds.Min, bounds.Max,
                {.BorderColor = Theme::MetalHighlight, .BorderTh = 1_px, .FillColor = Theme::PopupBg, .Rounding = r});
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
        const float fontH = Theme::DescFontSize * Hax::Gui::G.ScaleFactor;
        const float markH = fontH * 1.2f;
        const Hax::Vector2 padding = {12_px, 5_px};

        Hax::Vector2 markSize{};
        float markSpacing = 0.f;
        if (selected)
        {
            markSize = Hax::Gui::CalcTextSize(ICONS_FONT, L"\uF00C", markH);
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
            Hax::Gui::BeginHorizontal(5_px);

            bounds.Min = Hax::Gui::GetCursorPos();
            bounds.Max = bounds.Min + btnSize;
        }

        Hax::Gui::PlaceItem(bounds.GetSize());
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;

        if (!params.Disabled)
            Hax::Gui::Interact(id, bounds);

        bool active = Hax::Gui::IsItemActive(id);
        bool hovered = Hax::Gui::IsItemHovered(id);
        Hax::Gui::Color bg = selected ? 0x765015FF : (hovered ? 0x2B291AFF : 0x15160FFF);
        Hax::Gui::Color fg = selected ? 0xFFE17AFF : 0xC8BE94FF;

        if (hovered)
            Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);

        Hax::Gui::DrawRect(bounds.Min, bounds.Max,
            {.BorderColor = selected ? 0xD49323FF : 0x443B23FF, .BorderTh = 1_px, .FillColor = bg, .Rounding = Hax::Min(params.R, 2_px)});
        Hax::Gui::DrawString(MAIN_FONT, label, bounds.Min + padding, fontH, {.Color = fg});
        if (selected)
        {
            Hax::Vector2 pos = {bounds.Max.X - padding.X - markSize.X, bounds.Min.Y + (btnSize.Y - markSize.Y) / 2.f};
            Hax::Gui::DrawString(ICONS_FONT, L"\uF00C", pos, markH, {.Color = Theme::StatusGreen});
        }

        if (params.Disabled)
            Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = Theme::DisabledMaskCol});

        return Hax::Gui::IsItemClicked(id);
    }

    float CalcWidgetEqWidth(size_t nWidgets)
    {
        const float spacing = Hax::Gui::GetLayoutSpacing();
        return (Hax::Gui::GetContentRegionAvail().X - spacing * float(nWidgets - 1)) / (float)nWidgets;
    }

    Hax::Vector2 CalcRepeatBtnSize(Hax::WStringView label)
    {
        const float fontH = Theme::DescFontSize * Hax::Gui::G.ScaleFactor;
        const Hax::Vector2 padding = {8_px, 3_px};

        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(MAIN_FONT, label, fontH);
        const Hax::Vector2 btnSize = textSize + padding * 2.f;
        return btnSize;
    }

    bool RepeatBtn(size_t id, Hax::WStringView label, bool enabled)
    {
        const float fontH = Theme::DescFontSize * Hax::Gui::G.ScaleFactor;
        const Hax::Vector2 padding = {8_px, 3_px};

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

        Hax::Gui::LinearAnim& state = Hax::Gui::GetState<Hax::Gui::LinearAnim>(id);

        Hax::Gui::Color bg = 0x6B470FFF;
        Hax::Gui::Color fg = 0x1A1408FF;
        if (!Hax::Gui::IsItemActive(id))
        {
            constexpr Hax::Gui::LinearColor fromBg = 0x17180FFF;
            constexpr Hax::Gui::LinearColor toBg = 0x3B341CFF;
            bg = Hax::Lerp(fromBg, toBg, state.Progress).ToColor();

            constexpr Hax::Gui::LinearColor fromFg = 0xC8BE94FF;
            constexpr Hax::Gui::LinearColor toFg = 0xF2C64FFF;
            fg = Hax::Lerp(fromFg, toFg, state.Progress).ToColor();
        }

        const float deltaTime = Hax::Gui::GetDeltaTime();
        if (Hax::Gui::IsItemHovered(id))
        {
            state.Elapse(deltaTime, 0.1f);
            Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);
        }
        else
            state.Elapse(-deltaTime, 0.1f);

        Hax::Gui::DrawRect(bounds.Min, bounds.Max,
            {.BorderColor = 0x66552CFF, .BorderTh = 1_px, .FillColor = bg, .Rounding = 2_px});
        Hax::Gui::DrawString(MAIN_FONT, label, bounds.Min + (btnSize - textSize) / 2.f, fontH, {.Color = fg});

        if (!enabled)
            Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = Theme::DisabledMaskCol});

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

        const float fontH = Theme::DescFontSize * Hax::Gui::G.ScaleFactor;
        const Hax::Vector2 size = {width, CalcButtonHeight()};
        const Hax::Rect bounds = Hax::Rect::FromPosSize(Hax::Gui::GetCursorPos(), size);
        Hax::Gui::PlaceItem(size);
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;

        Hax::Gui::DrawRect(bounds.Min, bounds.Max,
            {.BorderColor = wasFocused ? Theme::ActiveColor : 0x66552CFF, .BorderTh = wasFocused ? 2_px : 1_px,
             .FillColor = 0x0D0E09FF, .Rounding = 2_px});

        bool changed = false;
        const Hax::Vector2 padding = {8_px, 3_px};
        const Hax::Rect editBounds = {bounds.Min + padding, bounds.Max - padding};
        if (enabled)
        {
            changed = Hax::Gui::StringEdit(id, MAIN_FONT, state.Buffer, _countof(state.Buffer), fontH, editBounds,
                {.CaretColor = Theme::ActiveColor, .Hint = L"0", .HintColor = 0x635D48FF, .TextColor = 0xF0C34AFF});

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
            const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(MAIN_FONT, state.Buffer, fontH);
            Hax::Gui::DrawString(MAIN_FONT, state.Buffer, bounds.Min + (size - textSize) / 2.f, fontH, {.Color = 0x756E55FF});
            Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = Theme::DisabledMaskCol, .Rounding = 2_px});
        }

        if (Hax::Gui::IsItemHovered(id))
            Hax::Gui::SetMouseIcon(enabled ? Hax::Gui::MouseIcon_TextInput : Hax::Gui::MouseIcon_NotAllowed);
        return changed;
    }

    bool Hotkey(size_t id, int& key)
    {
        const float fontH = Theme::DescFontSize * Hax::Gui::G.ScaleFactor;
        const Hax::Vector2 padding = {8_px, 4_px};

        Hax::WStringView keyName = Hax::Gui::GetKeyName(key);
        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(MAIN_FONT, keyName, fontH);
        const Hax::Vector2 btnSize = textSize + padding * 2.f;

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
        if (Hax::Gui::IsItemHovered(id))
            Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);

        bool changed = false;

        if (G->KeyListenerId == id)
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

        Hax::Gui::LinearColor color = 0xE49B1800;
        color.A = (Hax::Sin(Hax::kPi * (float)(Hax::Gui::GetTime() - state) / 0.5f) + 1.f) * 0.5f;

        float px3 = 3_px; float px1 = 1_px;
        if (G->KeyListenerId == id)
        {
            Hax::Gui::DrawRect(bounds.Min - Hax::Vector2(px3, px3), bounds.Max + Hax::Vector2(px3, px3), { .FillColor = color.ToColor(), .Rounding = 2_px });
            Hax::Gui::DrawRect(bounds.Min - Hax::Vector2(px1, px1), bounds.Max + Hax::Vector2(px1, px1), { .FillColor = 0x18180FFF, .Rounding = 2_px });
        }
        Hax::Gui::DrawRect(bounds.Min, bounds.Max,
            {.BorderColor = 0x66552CFF, .BorderTh = 1_px, .FillColor = 0x15160FFF, .Rounding = 2_px});
        Hax::Gui::DrawString(MAIN_FONT, keyName, bounds.Min + padding, fontH, {.Color = Theme::MainCol});

        Hax::Gui::DrawString(ICONS_FONT, L"\uf11c", bounds.Max - Hax::Vector2(8_px, 8_px), Theme::DescFontSize * Hax::Gui::G.ScaleFactor, {.Color = Theme::DescColor});

        return changed;
    }

    bool HotkeyEx(size_t id, int& key, Hax::WStringView label, Hax::WStringView desc)
    {
        const float fontH = Theme::DescFontSize * Hax::Gui::G.ScaleFactor;
        const Hax::Vector2 padding = {8_px, 4_px};
        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(MAIN_FONT, Hax::Gui::GetKeyName(key), fontH);
        Hax::Vector2 btnSize = textSize + padding * 2.f;

        Hax::Gui::BeginVertical();
        Hax::Gui::BeginHorizontal();
        const Hax::Vector2 labelPos = Hax::Gui::GetCursorPos();
        const Hax::Vector2 labelSize = Hax::Gui::CalcTextSize(MAIN_FONT, label, Hax::Gui::Scale(Theme::MainFontSize));
        LabelAlignedByH(MAIN_FONT, label, Hax::Gui::Scale(Theme::MainFontSize), Theme::MainCol, btnSize.Y);
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
        float size = Theme::CheckboxSize * Hax::Gui::G.ScaleFactor;

        Hax::Rect bounds;
        bounds.Min = Hax::Gui::GetCursorPos();
        bounds.Max = bounds.Min + Hax::Vector2(size, size);

        Hax::Gui::PlaceItem(bounds.GetSize());
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;

        Hax::Gui::Interact(id, bounds);

        if (Hax::Gui::IsItemClicked(id))
            val = !val;

        if (Hax::Gui::IsItemHovered(id))
            Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);

        Hax::Gui::DrawRect(bounds.Min, bounds.Max,
            {.BorderColor = val ? 0xD9B33DFF : 0x5B4D2BFF, .BorderTh = 1_px,
             .FillColor = val ? Theme::StatusGreen : Theme::FrameColor, .Rounding = 1_px});
        return Hax::Gui::IsItemClicked(id);
    }

    bool CheckboxEx(size_t id, bool& val, Hax::WStringView text)
    {
        bool changed = false;
        Hax::Gui::BeginHorizontal(5_px);
        {
            changed = Checkbox(id, val);
            LabelAlignedByH(MAIN_FONT, text, Theme::DescFontSize * Hax::Gui::G.ScaleFactor, Hax::Gui::Color(val ? 0xD4C99FFF : 0x847C5FFF), Hax::Gui::Scale(Theme::CheckboxSize));
            Hax::Gui::Interact(id, Hax::Gui::GetLayoutBounds());
        }
        Hax::Gui::EndHorizontal();

        return changed;
    }

    void HorizontalLine(float th, float w)
    {
        Hax::Gui::HorizontalLine(th, Theme::SeparatorCol, w);
    }
}
