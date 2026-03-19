#include <Windows.h>
#include <iostream>
#include "overlay/Overlay/overlay.hpp"
#include "source/storage/Globals.hpp"

int main() {
    SetConsoleTitleA("Standalone Menu");

    // Initialize default keybinds just in case GUI relies on it
    storage::init_menu_keybind();

    // Start rendering the menu. This will block and run the message loop.
    overlay::render();

    return 0;
}
