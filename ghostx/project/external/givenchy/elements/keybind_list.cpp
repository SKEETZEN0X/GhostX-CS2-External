#include "../settings/functions.h"

bool c_widgets::render_main_keybind_stoke() {
    return true;
}

bool c_widgets::keybind_stroke(keybind_data_t* current_keybind, int number) {

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    std::string get_id = std::string(current_keybind->label.data()) + std::to_string(number);
    const ImGuiID id = window->GetID(get_id.data());

    const ImVec2 pos = window->DC.CursorPos;

    const float max_test_size = gui->text_size(var->font.medium11, elements->keybinds_list.max_label.data()).x;
    const ImVec2 size = ImVec2(SCALE(300.f) + max_test_size, SCALE(20.f));

    const ImRect total_bb(pos, pos + size);
    const ImRect zone1_bb = ImRect(total_bb.Min + ImVec2(SCALE(10.f), 0), total_bb.Min + ImVec2(SCALE(20.f) + max_test_size, size.y));
    const ImRect zone2_bb = ImRect(zone1_bb.GetTR(), zone1_bb.GetTR() + ImVec2(SCALE(70.f), size.y));

    const ImVec2 feature_size = ImVec2(gui->text_size(var->font.medium11, current_keybind->label.data()).x / 2.f, gui->text_size(var->font.medium11, current_keybind->label.data()).y / 2.f);
    const ImVec2 key_size = ImVec2(gui->text_size(var->font.medium11, key_names[current_keybind->key].data()).x / 2.f, gui->text_size(var->font.medium11, key_names[current_keybind->key].data()).y / 2.f);

    ImDrawList* drawlist = GetWindowDrawList();

    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id))
        return false;

    draw->text_clipped(drawlist, var->font.medium11, zone1_bb.GetCenter() - feature_size, zone1_bb.GetCenter() + feature_size, draw->get_clr(clr->text.inactive), current_keybind->label.data());
    draw->text_clipped(drawlist, var->font.medium11, zone2_bb.GetCenter() - key_size, zone2_bb.GetCenter() + key_size, draw->get_clr(clr->text.inactive), key_names[current_keybind->key].data());

    return true;
}