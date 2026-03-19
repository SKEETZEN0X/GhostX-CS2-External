#include <stdafx.hpp>
#include <thread>

int main() {
  {
    if (!g::console.initialize("ghostx")) {
      return 1;
    }

    g::console.success("Starting GhostX...");

    if (const auto hwnd = ::GetConsoleWindow()) {
      ::ShowWindow(hwnd, SW_HIDE);
    }

    std::string init_error{};

    if (!g::input.initialize()) {
      init_error = "failed to initialize input";
    }

    if (init_error.empty() && !g::memory.initialize(L"cs2.exe")) {
      init_error = "cs2.exe not found — launch the game first";
    }

    if (init_error.empty() && !g::modules.initialize()) {
      init_error = "failed to resolve game modules";
    }

    if (init_error.empty() && !g::offsets.initialize()) {
      init_error = "failed to resolve offsets — update may be needed";
    }

    if (init_error.empty()) {
      std::thread(threads::game).detach();
      std::thread(threads::combat).detach();
    }

    if ( !init_error.empty( ) )
      g::console.print( "{}", init_error );

    if (!g::render.initialize()) {
      return 1;
    }
  }

  return 0;
}