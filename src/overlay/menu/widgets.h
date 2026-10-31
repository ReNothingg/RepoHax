#pragma once

#include <third_party/haxsdk/gui/hax_gui.h>

namespace Cheat
{
    enum class ButtonStyle
    {
        Secondary,
        Primary,
        Danger,
        Ghost
    };

    struct ButtonParams
    {
        bool Enabled = true;
        float MinW = 0.f;
        ButtonStyle Style = ButtonStyle::Secondary;
    };

    struct ToggleExParams
    {
        bool Disabled;
        size_t SettingsId;
    };

    struct SelectableParams
    {
        bool AutoLayout;
        bool Disabled;
        float MinW;
        float R = 0.f;
    };

    struct AutoLayoutButtonParams
    {
        bool Disabled;
        Hax::Gui::TextureHandle Icon;
    };

    // Text
    void Label(Hax::Gui::FontHandle hFont, Hax::WStringView text, float fontH, Hax::Gui::Color col);
    void MainLabel(Hax::WStringView text);
    void MainLabel(Hax::WStringView text, const Hax::Gui::Color& col);
    void HintLabel(Hax::WStringView text);
    Hax::Vector2 CalcMainLabelSize(Hax::WStringView text);
    void LabelAlignedByH(Hax::Gui::FontHandle hFont, Hax::WStringView label, float fontH, const Hax::Gui::Color& col, float h);
    void MainLabelAlignedByH(Hax::WStringView label, float h);

    // Window, rail and navigation
    void BeginWindow();
    void EndWindow();
    bool BeginSidePanel();
    void EndSidePanel();
    bool SearchField(size_t id, wchar_t* buf, size_t bufSize, Hax::WStringView hint, float width);
    bool NavItem(size_t id, Hax::WStringView text, Hax::char16 icon16, bool active, bool chevron);
    bool NavSubItem(size_t id, Hax::WStringView text, bool active);
    bool SearchResultItem(size_t id, Hax::WStringView text, Hax::WStringView where);
    void Badge(Hax::WStringView text, Hax::Gui::Color fg);
    Hax::Vector2 CalcBadgeSize(Hax::WStringView text);
    void KeyCap(Hax::WStringView text);
    bool IconButton(size_t id, Hax::char16 icon16, Hax::WStringView tooltip = {});
    void StatusDot(Hax::Gui::Color col, float h);

    // Search: flashes the control whose label matches after jumping to it.
    void SetSearchHighlight(Hax::WStringView label);
    void TickSearchHighlight();

    // Cards
    void BeginPanel(size_t id);
    void EndPanel();
    void PanelHeader(Hax::WStringView text, Hax::WStringView desc = {});
    void HorizontalLine(float th, float w = 0.f);

    // Controls
    bool SettingsBtn(size_t id, bool disabled = false);
    bool Toggle(size_t id, bool& value, bool enabled = true);
    bool ToggleEx(size_t id, bool& value, Hax::WStringView text, Hax::WStringView desc = {}, ToggleExParams params = {});
    Hax::Vector2 CalcButtonSize(Hax::WStringView label);
    float CalcButtonHeight();
    bool Button(size_t id, Hax::WStringView label, Hax::WStringView desc = {}, const ButtonParams& params = {});
    void Image(Hax::Gui::TextureHandle tex, Hax::Vector2 customSize = {}, float rounding = 0.f);
    int SliderConvertInt(int min, int max, float m);
    float SliderConvertFloat(float min, float max, float m);
    void DrawSliderVisual(const Hax::Rect& bounds, float t, float activeProgress);
    void ValuePill(Hax::WStringView text);
    Hax::Vector2 CalcValuePillSize(Hax::WStringView text);
    void SliderHeader(Hax::WStringView text, Hax::WStringView valTxt);
    bool AutoLayoutButton(size_t id, Hax::WStringView text, const AutoLayoutButtonParams & = {});
    bool DropdownBtn(size_t id, Hax::WStringView preview, float w);
    void OpenPopup(size_t id, const Hax::Vector2& pos);
    void ClosePopup(size_t id);
    bool BeginDropList(size_t id, const Hax::Vector2& size);
    void EndDropList();
    bool BeginModal(size_t id);
    void EndModal();
    bool Selectable(size_t id, Hax::WStringView label, bool selected, SelectableParams params = {});
    float CalcWidgetEqWidth(size_t nWidgets);
    Hax::Vector2 CalcRepeatBtnSize(Hax::WStringView label);
    bool RepeatBtn(size_t id, Hax::WStringView label, bool enabled = true);
    bool IntInput(size_t id, int& value, int min, int max, float width, bool enabled = true);
    bool Hotkey(size_t id, int& key);
    bool HotkeyEx(size_t id, int& key, Hax::WStringView label, Hax::WStringView desc = {});
    bool Checkbox(size_t id, bool& val);
    bool CheckboxEx(size_t id, bool& val, Hax::WStringView text);

    template <typename T>
    bool Slider(size_t id, T* val, T min, T max, T(*func)(T min, T max, float mousePosNorm), float w)
    {
        T tmp{};
        if (!val) val = &tmp;
        else tmp = *val;

        const float r = 9_px;

        Hax::Rect bounds;
        bounds.Min = Hax::Gui::GetCursorPos();
        bounds.Max.X = bounds.Min.X + w;
        bounds.Max.Y = bounds.Min.Y + r * 2.f;

        Hax::Gui::PlaceItem(bounds.GetSize());
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;

        const float minMousePos = bounds.Min.X + r;
        const float maxMousePos = bounds.Max.X - r;

        Hax::Gui::Interact(id, bounds);

        if (Hax::Gui::IsItemHovered(id))
            Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);

        Hax::Gui::LinearAnim& state = Hax::Gui::GetState<Hax::Gui::LinearAnim>(id);
        const float deltaTime = Hax::Gui::GetDeltaTime();

        if (Hax::Gui::IsItemActive(id))
        {
            state.Elapse(deltaTime, 0.15f);
            const float mousePos = Hax::Gui::GetMousePos().X;
            const float mousePosNorm = (mousePos - minMousePos) / (maxMousePos - minMousePos);

            *val = func(min, max, mousePosNorm);
        }
        else
            state.Elapse(-deltaTime, 0.15f);

        const float t = (float)(*val - min) / (float)(max - min);
        DrawSliderVisual(bounds, t, state.Progress);

        return *val != tmp;
    }

    template <typename T>
    bool SliderEx(size_t id, Hax::WStringView text, Hax::WStringView valTxt, T* val, T min, T max, T(*func)(T min, T max, float mousePosNorm))
    {
        Hax::Gui::BeginVertical(8_px);
        SliderHeader(text, valTxt);
        bool changed = Slider(id, val, min, max, func, Hax::Gui::GetContentRegionAvail().X);
        Hax::Gui::EndVertical();
        return changed;
    }
}
