#pragma once

#include <third_party/haxsdk/gui/hax_gui.h>

namespace Cheat::Theme
{
    // Graphite "field terminal" palette with one amber accent.
    constexpr Hax::Gui::Color Bg         = 0x0C0D10FF;
    constexpr Hax::Gui::Color Rail       = 0x101215FF;
    constexpr Hax::Gui::Color Surface1   = 0x15181CFF;
    constexpr Hax::Gui::Color Surface2   = 0x1B1F24FF;
    constexpr Hax::Gui::Color Surface3   = 0x272C33FF;
    constexpr Hax::Gui::Color Line       = 0x23272DFF;
    constexpr Hax::Gui::Color Line2      = 0x2F353DFF;
    constexpr Hax::Gui::Color Text       = 0xECEAE4FF;
    constexpr Hax::Gui::Color Muted      = 0xA6A39BFF;
    constexpr Hax::Gui::Color Faint      = 0x8D8A82FF;
    constexpr Hax::Gui::Color Accent     = 0xF5A524FF;
    constexpr Hax::Gui::Color AccentSoft = 0xF5A52424;
    constexpr Hax::Gui::Color Ok         = 0x6FD08CFF;
    constexpr Hax::Gui::Color Danger     = 0xFF7A6EFF;
    constexpr Hax::Gui::Color DangerSoft = 0xFF7A6E16;
    constexpr Hax::Gui::Color Host       = 0x86B9FFFF;
    constexpr Hax::Gui::Color Ink        = 0x17110AFF;
    constexpr Hax::Gui::Color Thumb      = 0xF4F1EAFF;
    constexpr Hax::Gui::Color DisabledMask = 0x15181CB4;

    constexpr float WindowR = 14.f;
    constexpr float CardR = 12.f;
    constexpr float ControlR = 8.f;
    constexpr float MainFontSize = 13.f;
    constexpr float DescFontSize = 12.5f;
    constexpr float TitleFontSize = 11.5f;

    constexpr Hax::Vector2 ToggleSize = {36.f, 20.f};
    constexpr float SettingBtnSize = 15.f;
    constexpr float CheckboxSize = 16.f;
    constexpr Hax::Vector2 WindowSize = {1280.f, 860.f};
    constexpr float SidePanelWidth = 232.f;
}
