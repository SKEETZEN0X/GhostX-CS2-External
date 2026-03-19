#include "../settings/functions.h"

bool c_widgets::checkbox_ex(std::string_view label, std::string_view description, bool* value, bool is_keybind, bool is_color, float color[4], bool alpha, bool is_keybind_window) {
    
    struct anim_state {
        ImVec4 button_background = clr->main.dark_4;
        ImVec4 button_border = clr->main.border;

        ImVec4 text_color = clr->text.inactive;
        bool open_description = false;
        ImVec2 description_size = ImVec2(0.f, 0.f);
        float arrow_rad = 90.f;
        ImVec4 arrow_color = clr->text.inactive;

        ImVec4 circle_color = clr->main.light_1;
        float circle_pos = 0.f;

        ImVec4 settings_color = clr->text.inactive;

        ImVec2 begin_pos;
        bool open_begin, is_key_begin, is_col_begin = false;
        float begin_anim = 0.f;

        int current_point = 0;
        bool open_kh1_begin = false;
        bool open_kh2_begin = false;
        float kh1_anim = 0.f;
        float kh2_anim = 0.f;

        bool bind_value, reset_keybinds = false;
        int key, mode = 0;
    };

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label.data());

    anim_state* state = gui->anim_container(&state, id);

    const ImVec2 label_size = gui->text_size(var->font.medium10, label.data());
    const ImVec2 keybind_size = gui->text_size(var->font.icon10, var->icons.gear.data());
    const ImVec2 arrow_size = gui->text_size(var->font.icon6, var->icons.arrow.data());

    ImVec2 padding = SCALE(is_keybind_window ? ImVec2(0.f, elements->checkbox.padding.y) : elements->checkbox.padding);
    std::string wrap_description = gui->wrap_text_to_width(var->font.medium10, description.data(), SCALE(elements->child.size.x) - padding.x * 2);
    const ImVec2 open_description_size = gui->text_size(var->font.medium10, wrap_description.data());
    bool is_desc = open_description_size.x > 0.1f;
    ImVec2 description_size = is_desc ? (state->open_description ? open_description_size : label_size) : ImVec2(0.f, 0.f);
    bool is_large_desc = open_description_size.y > label_size.y;
    gui->easing(state->description_size.y, description_size.y, var->anim.speed, dynamic_easing);

    const ImVec2 pos = window->DC.CursorPos;
    const ImVec2 mouse_pos = GetMousePos();
    ImDrawList* drawlist = GetWindowDrawList();

    const ImVec2 size = ImVec2(SCALE(is_keybind_window ? elements->kh_window.size.x : elements->child.size.x), is_keybind_window ? label_size.y + padding.y: (is_desc ? SCALE(elements->checkbox.spacing) + state->description_size.y : 0) + padding.y * 2 + label_size.y);
    const ImRect total_bb(pos, pos + size);
    const ImRect label_bb(total_bb.Min + padding, total_bb.Min + padding + label_size);
    const ImRect button_bb(ImVec2(total_bb.Max.x - SCALE(elements->checkbox.button_size.x) - padding.x, label_bb.GetCenter().y - SCALE(elements->checkbox.button_size.y / 2)), ImVec2(total_bb.Max.x - padding.x, label_bb.GetCenter().y + SCALE(elements->checkbox.button_size.y / 2)));
    const ImRect desc_bb(ImVec2(label_bb.Min.x, label_bb.Max.y + SCALE(elements->combo.spacing)), ImVec2(label_bb.Min.x + SCALE(elements->child.size.x) - padding.x * 2, label_bb.Max.y + SCALE(elements->combo.spacing) + state->description_size.y));
    const ImRect arrow_bb(ImVec2(label_bb.Min.x + size.x - padding.x * 2 - arrow_size.x, desc_bb.Min.y + SCALE(3.f)), ImVec2(label_bb.Min.x + size.x - padding.x * 2, desc_bb.Min.y + arrow_size.y + SCALE(3.f)));

    ImRect keybind_bb;
    ImRect color_bb;
    if (is_keybind && !is_color) {
        keybind_bb = ImRect(ImVec2(button_bb.Min.x - padding.x - keybind_size.x, button_bb.Min.y), ImVec2(button_bb.Min.x - padding.x, button_bb.Max.y + SCALE(1.f)));
    }
    else if (is_keybind && is_color) {
        keybind_bb = ImRect(ImVec2(button_bb.Min.x - (padding.x * 3) - keybind_size.x, button_bb.Min.y), ImVec2(button_bb.Min.x - (padding.x * 3), button_bb.Max.y + SCALE(1.f)));
    }
    if (is_color) {
        color_bb = ImRect(ImVec2(button_bb.Min.x - padding.x - keybind_size.x, button_bb.GetCenter().y - keybind_size.y / 2), ImVec2(button_bb.Min.x - padding.x, button_bb.GetCenter().y + keybind_size.y / 2));
    }

    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id))
        return false;

    bool hovered = IsItemHovered();
    bool button_hovered, held;
    bool pressed = ButtonBehavior(button_bb, id, &button_hovered, &held);
    if (pressed)
        *value = !*value;

    gui->easing(state->button_background, *value ? clr->accent.alpha.Value : clr->main.dark_4.Value, var->anim.speed, dynamic_easing);
    gui->easing(state->button_border, *value ? clr->main.border_0.Value : clr->main.border.Value, var->anim.speed, dynamic_easing);
    gui->easing(state->circle_color, *value ? clr->accent.main.Value : clr->main.light_1.Value, var->anim.speed, dynamic_easing);
    gui->easing(state->text_color, *value ? clr->text.active.Value : hovered ? clr->text.hover.Value : clr->text.inactive.Value, var->anim.speed, dynamic_easing);
    gui->easing(state->circle_pos, *value ? SCALE(elements->checkbox.button_size.x / 2) : 0.f, var->anim.speed, dynamic_easing);
    gui->easing(state->settings_color, is_keybind && keybind_bb.Contains(mouse_pos) ? clr->text.hover.Value : clr->text.inactive.Value, var->anim.speed, dynamic_easing);
    gui->easing(state->begin_anim, state->open_begin ? 1.f : 0.f, var->anim.speed, dynamic_easing);
    gui->easing(state->arrow_rad, state->open_description ? 270.f : 90.f, var->anim.speed, dynamic_easing);
    gui->easing(state->arrow_color, state->open_description ? clr->text.active.Value : clr->text.inactive.Value, var->anim.speed, dynamic_easing);

    draw->rect_filled(drawlist, button_bb.Min, button_bb.Max, draw->get_clr(state->button_background), SCALE(elements->checkbox.button_size.y / 2), ImDrawFlags_RoundCornersAll);
    draw->rect(drawlist, button_bb.Min, button_bb.Max, draw->get_clr(state->button_border), SCALE(elements->checkbox.button_size.y / 2), ImDrawFlags_RoundCornersAll);

    draw->circle_filled(drawlist, button_bb.Min + ImVec2(SCALE(5.f) + state->circle_pos, SCALE(elements->checkbox.button_size.y / 2)), SCALE(elements->checkbox.circle_size), draw->get_clr(state->circle_color), 32);
    if (is_color)
        draw->circle_filled(drawlist, color_bb.GetCenter(), SCALE(elements->checkbox.colorpicker_circle_size), draw->get_clr(ImVec4(color[0], color[1], color[2], 1.f)), 32);

    draw->text_clipped(drawlist, var->font.medium10, label_bb.Min, label_bb.Max, draw->get_clr(state->text_color), label.data());

    if (is_desc) {
        if (is_large_desc) {
            draw->rotate_start(drawlist);
            draw->text_clipped(drawlist, var->font.icon6, arrow_bb.Min, arrow_bb.Max, draw->get_clr(state->arrow_color), var->icons.arrow.data());
            draw->rotate_end(drawlist, state->arrow_rad, arrow_bb.GetCenter());
        }
        draw->text_clipped(drawlist, var->font.medium10, desc_bb.Min, desc_bb.Max, draw->get_clr(clr->text.inactive), wrap_description.data());
        if (desc_bb.Contains(mouse_pos) && IsMouseClicked(0) && is_large_desc && IsWindowHovered())
            state->open_description = !state->open_description;
    }

    draw->text_clipped(drawlist, var->font.icon10, keybind_bb.Min, keybind_bb.Max, draw->get_clr(state->settings_color), var->icons.gear.data());

    if (!is_keybind_window)
        draw->line(drawlist, ImVec2(total_bb.Min.x + padding.x, total_bb.Max.y), total_bb.Max - ImVec2(padding.x, 0), draw->get_clr(clr->main.border));

    if (keybind_bb.Contains(mouse_pos) && IsMouseClicked(0) && !state->open_begin && is_keybind && IsWindowHovered()) {
        state->open_begin = true;
        state->is_key_begin = true;
    }

    if (color_bb.Contains(mouse_pos) && IsMouseClicked(0) && !state->open_begin && is_color && IsWindowHovered()) {
        state->open_begin = true;
        state->is_col_begin = true;
    }

    if (!state->open_begin && state->begin_anim < 0.01f) {
        state->is_col_begin = false;
        state->is_key_begin = false;
    }

    if (IsKeyDown(ImGuiKey_MouseWheelY)) {
        state->open_begin = false;
    }

    if (state->begin_anim > 0.01f) {

        state->begin_pos = state->is_key_begin ? keybind_bb.Min : color_bb.Min;

        gui->set_next_window_pos(state->begin_pos);
        gui->push_var(ImGuiStyleVar_Alpha, state->begin_anim);
        gui->push_var(ImGuiStyleVar_WindowPadding, SCALE(elements->checkbox.window_padding));
        gui->push_var(ImGuiStyleVar_ItemInnerSpacing, SCALE(elements->checkbox.window_padding));
        gui->push_var(ImGuiStyleVar_ItemSpacing, SCALE(elements->checkbox.window_padding));

        std::string main_begin_name = std::string(label.data()) + (state->is_col_begin ? "color_window" : "keybind_window");
        std::string keybinds_begin_name = std::string(label.data()) + "keybinds_window";
        std::string hotkeys_begin_name = std::string(label.data()) + "hotkeys_window";

        if (gui->begin(main_begin_name, nullptr, elements->checkbox.window_flags)) {

            draw->rect_filled(GetWindowDrawList(), GetWindowPos(), GetWindowPos() + GetWindowSize(), draw->get_clr(clr->main.dark_3_255), SCALE(elements->rounding), ImDrawFlags_RoundCornersAll);
            draw->rect(GetWindowDrawList(), GetWindowPos(), GetWindowPos() + GetWindowSize(), draw->get_clr(clr->main.border), SCALE(elements->rounding), ImDrawFlags_RoundCornersAll);
            
            if (state->is_col_begin) {
                widgets->color_picker(label.data(), color, alpha);

                bool color_hovered = g.HoveredWindow && strstr(g.HoveredWindow->Name, main_begin_name.data());
                if (!color_hovered && IsMouseClicked(0))
                    if (!hovered) {
                        state->open_begin = false;
                    }
            }
            else {

                widgets->k_button("Keybinds", 1, &state->current_point);
                widgets->k_button("Hotkeys", 2, &state->current_point);
                if (widgets->k_accent_button("Reset")) {
                    state->reset_keybinds = true;
                }

                if (state->current_point == 1)
                    state->open_kh1_begin = true;
                else
                    state->open_kh1_begin = false;
                
                if (state->current_point == 2)
                    state->open_kh2_begin = true;
                else
                    state->open_kh2_begin = false;

                gui->easing(state->kh1_anim, state->open_kh1_begin ? 1.f : 0.f, var->anim.speed, dynamic_easing);
                gui->easing(state->kh2_anim, state->open_kh2_begin ? 1.f : 0.f, var->anim.speed, dynamic_easing);

                if (state->reset_keybinds) {
                    for (int i = 0; i < widgets->keybinds.size(); i++) {                    
                        if (widgets->keybinds[i].label.data() == label.data()) {
                            widgets->keybinds.erase(std::next(widgets->keybinds.begin(), i));
                            i--;
                        }
                    }
                    state->reset_keybinds = false;
                    notify->add_notify("Keybinds has been reseted", success);
                }

                if (state->kh1_anim > 0.01f) {

                    gui->set_next_window_pos(state->begin_pos + ImVec2(GetWindowSize().x + SCALE(10.f), 0));
                    gui->push_var(ImGuiStyleVar_Alpha, state->begin_anim < 0.99f ? state->begin_anim : state->kh1_anim);
                    gui->push_var(ImGuiStyleVar_ItemSpacing, SCALE(8, 8));
                    gui->push_var(ImGuiStyleVar_WindowPadding, SCALE(8, 8));
                    gui->push_var(ImGuiStyleVar_FramePadding, SCALE(0, 0));
                    gui->begin(keybinds_begin_name, nullptr, elements->checkbox.window_flags);
                    {
                        draw->rect_filled(GetWindowDrawList(), GetWindowPos(), GetWindowPos() + GetWindowSize(), draw->get_clr(clr->main.dark_3_255), SCALE(elements->rounding), ImDrawFlags_RoundCornersAll);
                        draw->rect(GetWindowDrawList(), GetWindowPos(), GetWindowPos() + GetWindowSize(), draw->get_clr(clr->main.border), SCALE(elements->rounding), ImDrawFlags_RoundCornersAll);

                        if (widgets->keybind_button("Keybind", &state->key)) {
                            if (key_names[state->key] == "None") {
                                notify->add_notify("Keybind is not found", success);
                            }
                            else {
                                keybinds.push_back({ label, state->key, state->mode, true, state->bind_value == false ? "Off" : "On" });
                                state->bind_value = false;
                                state->key = 0;
                                state->mode = 0;
                                notify->add_notify("The keybind has been added", success);
                            }
                        }
                        widgets->mode_button("Mode", &state->mode);
                        gui->set_pos(gui->get_pos().y - SCALE(10.f), pos_y);
                        widgets->checkbox("Value", &state->bind_value, "", true);
                    }
                    gui->end();
                    gui->pop_var(4);
                }

                if (state->kh2_anim > 0.01f) {

                    gui->set_next_window_pos(state->begin_pos + ImVec2(GetWindowSize().x + SCALE(10.f), 0));
                    gui->push_var(ImGuiStyleVar_Alpha, state->begin_anim < 0.99f ? state->begin_anim : state->kh2_anim);
                    gui->push_var(ImGuiStyleVar_ItemSpacing, SCALE(8, 8));
                    gui->push_var(ImGuiStyleVar_WindowPadding, SCALE(8, 8));
                    gui->push_var(ImGuiStyleVar_FramePadding, SCALE(0, 0));
                    gui->begin(hotkeys_begin_name, nullptr, elements->checkbox.window_flags);
                    {
                        draw->rect_filled(GetWindowDrawList(), GetWindowPos(), GetWindowPos() + GetWindowSize(), draw->get_clr(clr->main.dark_3_255), SCALE(elements->rounding), ImDrawFlags_RoundCornersAll);
                        draw->rect(GetWindowDrawList(), GetWindowPos(), GetWindowPos() + GetWindowSize(), draw->get_clr(clr->main.border), SCALE(elements->rounding), ImDrawFlags_RoundCornersAll);

                        struct keybind_number {
                            std::string key_name;
                            int index;
                        };

                        std::vector<keybind_number> current_keybinds = {};
                        for (int i = 0; i < widgets->keybinds.size(); i++) {
                            if (widgets->keybinds[i].label.data() == label.data()) {
                                current_keybinds.push_back({ key_names[widgets->keybinds[i].key] , i });
                            }

                        }

                        for (int i = 0; i < current_keybinds.size(); i++) {
                            std::string button_random_id = std::string("delete##") + std::to_string(i);
                            if (widgets->delete_keybind_button(current_keybinds[i].key_name.data(), button_random_id)) {
                                widgets->keybinds.erase(std::next(widgets->keybinds.begin(), current_keybinds[i].index));
                                notify->add_notify("The keybind has been deleted", success);
                            }
                        }

                        if (current_keybinds.size() < 1) {
                            widgets->sized_text("No hotkeys yet", SCALE(150.f, 75.f));
                        }
                    }
                    gui->end();
                    gui->pop_var(4);
                }

                bool keybinds_hovered = g.HoveredWindow && strstr(g.HoveredWindow->Name, keybinds_begin_name.data());
                bool hotkeys_hovered = g.HoveredWindow && strstr(g.HoveredWindow->Name, hotkeys_begin_name.data());
                bool main_hovered = g.HoveredWindow && strstr(g.HoveredWindow->Name, main_begin_name.data());

                if (!keybind_bb.Contains(mouse_pos) && !main_hovered && !keybinds_hovered && !hotkeys_hovered && IsMouseClicked(0)) {
                    state->open_kh1_begin = false;
                    state->open_kh2_begin = false;
                    state->open_begin = false;
                }

            }

            gui->end();
        }

        gui->pop_var(4);
    }

    return pressed;
}

bool c_widgets::checkbox(std::string_view label, bool* value, std::string_view description, bool is_keybind) {
    bool checkbox = widgets->checkbox_ex(label, description, value, false, false, 0, false, is_keybind);
    return checkbox;
}

bool c_widgets::checkbox_keybind(std::string_view label, bool* value, std::string_view description) {
    bool checkbox = widgets->checkbox_ex(label, description, value, true);
    return checkbox;
}

bool c_widgets::checkbox_colorpicker(std::string_view label, bool* value, float color[4], bool alpha, std::string_view description) {
    bool checkbox = widgets->checkbox_ex(label, description, value, false, true, color, alpha);
    return checkbox;
}

bool c_widgets::checkbox_keybind_colorpicker(std::string_view label, bool* value, float color[4], bool alpha, std::string_view description) {
    bool checkbox = widgets->checkbox_ex(label, description, value, true, true, color, alpha);
    return checkbox;
}