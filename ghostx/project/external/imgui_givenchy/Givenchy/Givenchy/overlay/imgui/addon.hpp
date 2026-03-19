// render.hpp
#pragma once
#include "imgui.h"
#include <vector>
#define IMADD_ANIMATIONS_SPEED 0.05f // Second

namespace render {
    bool tab(const char* label, int* v, int current_id, const ImVec2& size_arg);
    bool check(const char* label, bool* v);

    void child(const char* label, const ImVec2& size_arg, bool hasOtherChild);

    void child2(const char* label, const ImVec2& size_arg, bool hasOtherChild);

}
