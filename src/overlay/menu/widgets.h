#pragma once

#include <third_party/haxsdk/gui/hax_gui.h>

namespace Cheat
{
    // Vector icons drawn with lines and circles. The bundled icon font only has
    // the navigation glyphs, so everything else is drawn here.
    enum class Icon : Hax::uint8
    {
        None, Search, Close, ChevronRight, ChevronDown, Check, Plus, Minus,
        Heart, Move, ArrowUp, Hand, Zap, Sparkle, ChevronsUp, Eye, Shield, Globe,
        Map, User, Users, Coin, Gem, Box, Target, Extract, Wallet, Play, Bars,
        Reset, Ghost, Sliders, Sun, Save, Trash, Battery, Keyboard, Clock,
        Crosshair, Cube, Shirt, Truck, Gun, Dot
    };

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
        Cheat::Icon Icon = Cheat::Icon::None;
    };

    struct ToggleExParams
    {
        bool Disabled;
        size_t SettingsId;
        bool HostOnly;
        int* Hotkey;
        size_t HotkeyId;
    };

    struct CardHeaderParams
    {
        Cheat::Icon Icon = Cheat::Icon::None;
        bool HostOnly = false;
        bool* Toggle = nullptr;          // master switch on the right
        bool ToggleDisabled = false;
        int* Hotkey = nullptr;           // editable key cap next to the switch
        size_t HotkeyId = 0;
        Hax::WStringView Action = {};    // small button on the right
        bool ActionEnabled = true;
        ButtonStyle ActionStyle = ButtonStyle::Secondary;
        size_t SettingsId = 0;           // settings popup button
        Hax::WStringView Note = {};      // faint text on the right
        bool NoDivider = false;
        size_t Id = 0;
    };

    struct ActionRowParams
    {
        bool Enabled = true;
        ButtonStyle Style = ButtonStyle::Secondary;
        bool HostOnly = false;
        int* Hotkey = nullptr;
        size_t HotkeyId = 0;
    };

    struct GridButton
    {
        Hax::WStringView Label;
        bool* Clicked = nullptr;
        bool Enabled = true;
        ButtonStyle Style = ButtonStyle::Secondary;
        Cheat::Icon Icon = Cheat::Icon::None;
        Hax::WStringView Tooltip = {};
    };

    struct PlayerRowData
    {
        Hax::WStringView Initial;
        Hax::Gui::Color AvatarColor;
        Hax::WStringView Name;
        Hax::WStringView Tag;
        Hax::WStringView Status;
        Hax::Gui::Color StatusColor;
        int Hp;
        int MaxHp;
        bool Selected;
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

    // Icons
    void DrawIcon(Icon icon, const Hax::Rect& bounds, Hax::Gui::Color col);
    void IconItem(Icon icon, float size, Hax::Gui::Color col, float rowH = 0.f);

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
    bool IconButton(size_t id, Icon icon, Hax::WStringView tooltip = {});
    void StatusDot(Hax::Gui::Color col, float h);

    // Search: flashes the control whose label matches after jumping to it.
    void SetSearchHighlight(Hax::WStringView label);
    void TickSearchHighlight();

    // Page layout: equal (or weighted) columns, each a vertical stack of cards.
    void BeginColumns(size_t id, int count, const float* weights = nullptr);
    void NextColumn();
    void EndColumns();
    void StatTile(Hax::WStringView label, Hax::WStringView value, Hax::WStringView sub = {}, float progress = -1.f, Hax::Gui::Color valueCol = 0xECEAE4FF);

    // Cards
    void BeginPanel(size_t id);
    void EndPanel();
    // Card title row. Returns true when its switch changed or its action button was clicked.
    bool PanelHeader(Hax::WStringView text, Hax::WStringView desc = {}, const CardHeaderParams& params = {});
    void HorizontalLine(float th, float w = 0.f);

    // Rows
    bool ToggleEx(size_t id, bool& value, Hax::WStringView text, Hax::WStringView desc = {}, ToggleExParams params = {});
    bool ActionRow(size_t id, Hax::WStringView text, Hax::WStringView hint, Hax::WStringView btnLabel, const ActionRowParams& params = {});
    void InfoRow(Hax::WStringView label, Hax::WStringView value, Hax::Gui::Color valueCol = 0xECEAE4FF);
    int SegmentedRow(size_t id, Hax::WStringView text, const Hax::WStringView* items, int count, int selected, bool enabled = true, bool fullWidth = false);
    int StepperRow(size_t id, Hax::WStringView text, Hax::WStringView hint, int value, bool enabled, bool hostOnly = false);
    int Stepper(size_t id, int value, bool enabled);
    int UpgradeCell(size_t id, Hax::WStringView label, int value, bool enabled, float width);
    bool PlayerRow(size_t id, const PlayerRowData& data);
    bool HotkeyEx(size_t id, int& key, Hax::WStringView label, Hax::WStringView desc = {});

    // Controls
    bool SettingsBtn(size_t id, bool disabled = false);
    bool Toggle(size_t id, bool& value, bool enabled = true);
    Hax::Vector2 CalcButtonSize(Hax::WStringView label);
    float CalcButtonHeight();
    bool Button(size_t id, Hax::WStringView label, Hax::WStringView desc = {}, const ButtonParams& params = {});
    bool ButtonGrid(size_t id, const GridButton* buttons, int count, int perRow);
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
