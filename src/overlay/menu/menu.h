#pragma once

namespace Cheat
{
    void PrepareMenu();
    void RenderMenu();
    void ToggleMenuVisibility();

    // True while the user is typing into the menu's search field, so letter
    // hotkeys (noclip, lasers...) don't fire as they type.
    bool IsTextInputActive();
}