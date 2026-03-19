#include "../settings/functions.h"

bool c_widgets::render_main_keybind_stoke() {

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    std::string get_id = "Main keybind Stroke";
    const ImGuiID id = window->GetID(get_id.data());

    const ImVec2 pos = window->DC.CursorPos;

    const ImVec2 feature_size = ImVec2(gui->text_size(var->font.medium11, "Features").x / 2.f, gui->text_size(var->font.medium11, "Features").y / 2.f);
    const ImVec2 key_size = ImVec2(gui->text_size(var->font.medium11, "Key").x / 2.f, gui->text_size(var->font.medium11, "Key").y / 2.f);
    const ImVec2 mode_size = ImVec2(gui->text_size(var->font.medium11, "Mode").x / 2.f, gui->text_size(var->font.medium11, "Mode").y / 2.f);
    const ImVec2 show_size = ImVec2(gui->text_size(var->font.medium11, "Show").x / 2.f, gui->text_size(var->font.medium11, "Show").y / 2.f);
    const ImVec2 value_size = ImVec2(gui->text_size(var->font.medium11, "Value").x / 2.f, gui->text_size(var->font.medium11, "Value").y / 2.f);

    ImDrawList* drawlist = GetWindowDrawList();

    const float max_test_size = gui->text_size(var->font.medium11, elements->keybinds_list.max_label.data()).x;

    const ImVec2 size = ImVec2(SCALE(300.f) + max_test_size, SCALE(20.f));

    const ImRect total_bb(pos, pos + size);

    const ImRect zone1_bb = ImRect(total_bb.Min + ImVec2(SCALE(10.f), 0), total_bb.Min + ImVec2(SCALE(20.f) + max_test_size, size.y));
    const ImRect zone2_bb = ImRect(zone1_bb.GetTR(), zone1_bb.GetTR() + ImVec2(SCALE(70.f), size.y));
    const ImRect zone3_bb = ImRect(zone2_bb.GetTR(), zone2_bb.GetTR() + ImVec2(SCALE(70.f), size.y));
    const ImRect zone4_bb = ImRect(zone3_bb.GetTR(), zone3_bb.GetTR() + ImVec2(SCALE(70.f), size.y));
    const ImRect zone5_bb = ImRect(zone4_bb.GetTR(), zone4_bb.GetTR() + ImVec2(SCALE(70.f), size.y));

    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(zone2_bb, id, &hovered, &held, 0);

    draw->text_clipped(drawlist, var->font.medium11, zone1_bb.GetCenter() - feature_size, zone1_bb.GetCenter() + feature_size, draw->get_clr(clr->text.active), "Features");
    draw->text_clipped(drawlist, var->font.medium11, zone2_bb.GetCenter() - key_size, zone2_bb.GetCenter() + key_size, draw->get_clr(clr->text.active), "Key");
    draw->text_clipped(drawlist, var->font.medium11, zone3_bb.GetCenter() - mode_size, zone3_bb.GetCenter() + mode_size, draw->get_clr(clr->text.active), "Mode");
    draw->text_clipped(drawlist, var->font.medium11, zone4_bb.GetCenter() - show_size, zone4_bb.GetCenter() + show_size, draw->get_clr(clr->text.active), "Show");
    draw->text_clipped(drawlist, var->font.medium11, zone5_bb.GetCenter() - value_size, zone5_bb.GetCenter() + value_size, draw->get_clr(clr->text.active), "Value");

    return true;

}

bool c_widgets::keybind_stroke(keybind_data_t* current_keybind, int number) {

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    std::string get_id = std::string(current_keybind->label.data()) + std::to_string(number).c_str();
    const ImGuiID id = window->GetID(get_id.data());

    const ImVec2 pos = window->DC.CursorPos;

    const ImVec2 feature_size = ImVec2(gui->text_size(var->font.medium11, current_keybind->label.data()).x / 2.f, gui->text_size(var->font.medium11, current_keybind->label.data()).y / 2.f);
    const ImVec2 key_size = ImVec2(gui->text_size(var->font.medium11, key_names[current_keybind->key].data()).x / 2.f, gui->text_size(var->font.medium11, key_names[current_keybind->key].data()).y / 2.f);
    const ImVec2 mode_size = ImVec2(gui->text_size(var->font.medium11, mode_names[current_keybind->mode].data()).x / 2.f, gui->text_size(var->font.medium11, mode_names[current_keybind->mode].data()).y / 2.f);
    const ImVec2 show_size = ImVec2(gui->text_size(var->font.medium11, show_names[current_keybind->show].data()).x / 2.f, gui->text_size(var->font.medium11, show_names[current_keybind->show].data()).y / 2.f);
    const ImVec2 value_size = ImVec2(gui->text_size(var->font.medium11, current_keybind->value.data()).x / 2.f, gui->text_size(var->font.medium11, current_keybind->value.data()).y / 2.f);

    ImDrawList* drawlist = GetWindowDrawList();

    const float max_test_size = gui->text_size(var->font.medium11, elements->keybinds_list.max_label.data()).x;

    const ImVec2 size = ImVec2(SCALE(300.f) + max_test_size, SCALE(20.f));

    const ImRect total_bb(pos, pos + size);

    const ImRect zone1_bb = ImRect(total_bb.Min + ImVec2(SCALE(10.f), 0), total_bb.Min + ImVec2(SCALE(20.f) + max_test_size, size.y));
    const ImRect zone2_bb = ImRect(zone1_bb.GetTR(), zone1_bb.GetTR() + ImVec2(SCALE(70.f), size.y));
    const ImRect zone3_bb = ImRect(zone2_bb.GetTR(), zone2_bb.GetTR() + ImVec2(SCALE(70.f), size.y));
    const ImRect zone4_bb = ImRect(zone3_bb.GetTR(), zone3_bb.GetTR() + ImVec2(SCALE(70.f), size.y));
    const ImRect zone5_bb = ImRect(zone4_bb.GetTR(), zone4_bb.GetTR() + ImVec2(SCALE(70.f), size.y));
    
    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(zone2_bb, id, &hovered, &held, 0);

    draw->text_clipped(drawlist, var->font.medium11, zone1_bb.GetCenter() - feature_size, zone1_bb.GetCenter() + feature_size, draw->get_clr(clr->text.inactive), current_keybind->label.data());
    draw->text_clipped(drawlist, var->font.medium11, zone2_bb.GetCenter() - key_size, zone2_bb.GetCenter() + key_size, draw->get_clr(clr->text.inactive), key_names[current_keybind->key].data());
    draw->text_clipped(drawlist, var->font.medium11, zone3_bb.GetCenter() - mode_size, zone3_bb.GetCenter() + mode_size, draw->get_clr(clr->text.inactive), mode_names[current_keybind->mode].data());
    draw->text_clipped(drawlist, var->font.medium11, zone4_bb.GetCenter() - show_size, zone4_bb.GetCenter() + show_size, draw->get_clr(clr->text.inactive), show_names[current_keybind->show].data());
    draw->text_clipped(drawlist, var->font.medium11, zone5_bb.GetCenter() - value_size, zone5_bb.GetCenter() + value_size, draw->get_clr(clr->text.inactive), current_keybind->value.data());

    return true;
}