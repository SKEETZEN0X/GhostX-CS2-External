#include "../settings/functions.h"

bool c_widgets::slider_scalar(std::string_view label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags, std::string_view description, bool keybind, bool is_keybind_window)
{
    struct anim_state {
        ImVec2 grab_pos;

        bool open_description = false;
        ImVec2 description_size = ImVec2(0.f, 0.f);
        float arrow_rad = 90.f;
        ImVec4 arrow_color = clr->text.inactive;

        ImVec2 begin_pos;
        bool open_begin = false;
        float begin_anim = 0.f;

        int current_point = 0;
        bool open_kh1_begin, open_kh2_begin = false;
        float kh1_anim, kh2_anim = 0.f;

        float bind_value = 0.f;
        bool reset_keybinds = false;
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

    ImVec2 padding = SCALE(is_keybind_window ? ImVec2(0.f, 0.f) : elements->slider.padding);
    ImVec2 inner_padding = SCALE(elements->slider.padding);
    std::string wrap_description = gui->wrap_text_to_width(var->font.medium10, description.data(), SCALE(elements->child.size.x) - padding.x * 2);
    const ImVec2 open_description_size = gui->text_size(var->font.medium10, wrap_description.data());
    bool is_desc = open_description_size.x > 0.1f;
    ImVec2 description_size = is_desc ? (state->open_description ? open_description_size : label_size) : ImVec2(0.f, 0.f);
    bool is_large_desc = open_description_size.y > label_size.y;
    gui->easing(state->description_size.y, description_size.y, var->anim.speed, dynamic_easing);

    const ImVec2 pos = window->DC.CursorPos;
    const ImVec2 mouse_pos = GetMousePos();
    ImDrawList* drawlist = GetWindowDrawList();

    const ImVec2 size = ImVec2(SCALE(is_keybind_window ? elements->kh_window.size.x : elements->child.size.x), is_keybind_window ? padding.y + label_size.y + SCALE(10.f + elements->slider.button_size.y): padding.y * 2 + label_size.y + SCALE(10.f + elements->slider.button_size.y) + (is_desc ? SCALE(elements->slider.spacing) + state->description_size.y : 0));
    const ImRect total_bb(pos, pos + size);
    const ImRect label_bb(total_bb.Min + padding, total_bb.Min + padding + label_size);
    const ImRect desc_bb(ImVec2(label_bb.Min.x, label_bb.Max.y + SCALE(elements->combo.spacing)), ImVec2(label_bb.Min.x + SCALE(elements->child.size.x) - padding.x * 2, label_bb.Max.y + SCALE(elements->combo.spacing) + state->description_size.y));
    const ImVec2 button_size = SCALE(elements->slider.button_size);
    const ImRect button_bb(ImVec2(label_bb.Min.x, total_bb.Max.y - padding.y - button_size.y), ImVec2(total_bb.Max.x - padding.x, total_bb.Max.y - padding.y));
    const ImRect arrow_bb(ImVec2(label_bb.Min.x + size.x - padding.x * 2 - arrow_size.x, desc_bb.Min.y + SCALE(3.f)), ImVec2(label_bb.Min.x + size.x - padding.x * 2, desc_bb.Min.y + arrow_size.y + SCALE(3.f)));

    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id))
        return false;

    if (format == NULL)
        format = DataTypeGetInfo(data_type)->PrintFmt;

    //draw->rect(drawlist, total_bb.Min, total_bb.Max, ImColor(255, 0, 0), 0, 0, 3);

    const bool hovered = ItemHoverable(button_bb, id, g.LastItemData.InFlags);

    const bool clicked = hovered && IsMouseClicked(0, ImGuiInputFlags_None, id);
    const bool make_active = (clicked || g.NavActivateId == id);
    if (make_active && clicked)
        SetKeyOwner(ImGuiKey_MouseLeft, id);

    if (make_active)
    {
        SetActiveID(id, window);
        SetFocusID(id, window);
        FocusWindow(window);
        g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
    }

    ImRect grab_bb;
    const bool value_changed = SliderBehavior(button_bb, id, data_type, p_data, p_min, p_max, format, flags, &grab_bb);
    if (value_changed)
        MarkItemEdited(id);


    gui->easing(state->grab_pos.x, grab_bb.Max.x - pos.x, var->anim.speed, dynamic_easing);
    gui->easing(state->grab_pos.y, grab_bb.Max.y - pos.y, var->anim.speed, dynamic_easing);
    gui->easing(state->arrow_rad, state->open_description ? 270.f : 90.f, var->anim.speed, dynamic_easing);
    gui->easing(state->arrow_color, state->open_description ? clr->text.active.Value : clr->text.inactive.Value, var->anim.speed, dynamic_easing);
    gui->easing(state->begin_anim, state->open_begin ? 1.f : 0.f, var->anim.speed, dynamic_easing);

    char value_buf[64];
    const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);
    const ImVec2 value_buf_size = gui->text_size(var->font.medium10, value_buf);
    const ImRect buf_bb(ImVec2(total_bb.Max.x - padding.x - value_buf_size.x, total_bb.Min.y + padding.y), ImVec2(total_bb.Max.x - padding.x, total_bb.Min.y + padding.y + value_buf_size.y));
    const ImRect keybind_bb = ImRect(ImVec2(buf_bb.Min.x - padding.x - keybind_size.x, buf_bb.GetCenter().y - keybind_size.y / 2), ImVec2(buf_bb.Min.x - padding.x, buf_bb.GetCenter().y + keybind_size.y / 2 + SCALE(1.f)));

    draw->text_clipped(drawlist, var->font.medium10, buf_bb.Min, buf_bb.Max, draw->get_clr(clr->text.active), value_buf);
    if (keybind)
        draw->text_clipped(drawlist, var->font.icon10, keybind_bb.Min, keybind_bb.Max, draw->get_clr(clr->text.inactive), var->icons.gear.data());
    draw->text_clipped(drawlist, var->font.medium10, label_bb.Min, label_bb.Max, draw->get_clr(clr->text.inactive), label.data());
    
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

    draw->rect_filled(drawlist, button_bb.Min, button_bb.Max, draw->get_clr(clr->main.dark_6), SCALE(elements->slider.size.y) / 2, ImDrawFlags_RoundCornersAll);
    
    if (grab_bb.Max.x > grab_bb.Min.x)
        draw->rect_filled(drawlist, button_bb.Min, ImVec2(pos.x + state->grab_pos.x, button_bb.Max.y), draw->get_clr(clr->accent.main), SCALE(elements->slider.size.y) / 2, ImDrawFlags_RoundCornersAll);

    draw->circle_filled(drawlist, pos + ImVec2(state->grab_pos.x, button_bb.GetCenter().y - pos.y), SCALE(elements->slider.circle_size), draw->get_clr(clr->main.light_2), 32);
    
    if (!is_keybind_window)
        draw->line(drawlist, ImVec2(total_bb.Min.x + padding.x, total_bb.Max.y), ImVec2(total_bb.Max.x - padding.x, total_bb.Max.y), draw->get_clr(clr->main.border));

    if (keybind_bb.Contains(GetMousePos()) && IsMouseClicked(0) && keybind) {
        state->open_begin = true;
    }

    if (IsKeyDown(ImGuiKey_MouseWheelY)) {
        state->open_begin = false;
    }

    if (state->begin_anim > 0.01f) {

        state->begin_pos = keybind_bb.Min;

        gui->set_next_window_pos(state->begin_pos);
        gui->push_var(ImGuiStyleVar_Alpha, state->begin_anim);
        gui->push_var(ImGuiStyleVar_WindowPadding, SCALE(elements->checkbox.window_padding));
        gui->push_var(ImGuiStyleVar_ItemInnerSpacing, SCALE(elements->checkbox.window_padding));
        gui->push_var(ImGuiStyleVar_ItemSpacing, SCALE(elements->checkbox.window_padding));

        std::string main_begin_name = std::string(label.data()) + "keybind_window";
        std::string keybinds_begin_name = std::string(label.data()) + "keybinds_window";
        std::string hotkeys_begin_name = std::string(label.data()) + "hotkeys_window";

        if (gui->begin(main_begin_name, nullptr, elements->checkbox.window_flags)) {

            draw->rect_filled(GetWindowDrawList(), GetWindowPos(), GetWindowPos() + GetWindowSize(), draw->get_clr(clr->main.dark_3_255), SCALE(elements->rounding), ImDrawFlags_RoundCornersAll);
            draw->rect(GetWindowDrawList(), GetWindowPos(), GetWindowPos() + GetWindowSize(), draw->get_clr(clr->main.border), SCALE(elements->rounding), ImDrawFlags_RoundCornersAll);

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
                        char value_buf[64];
                        ImFormatString(value_buf, IM_ARRAYSIZE(value_buf), format, state->bind_value);
                        const char* value_buf_end = value_buf + strlen(value_buf);
                        if (key_names[state->key] == "None") {
                            notify->add_notify("Keybind is not found", success);
                        }
                        else {
                            keybinds.push_back({ label, state->key, state->mode, true, value_buf });
                            state->bind_value = 0.f;
                            state->key = 0;
                            state->mode = 0;
                            notify->add_notify("The keybind has been added", success);
                        }
                    }
                    widgets->mode_button("Mode", &state->mode);
                    widgets->slider_scalar(label.data(), ImGuiDataType_Float, &state->bind_value, p_min, p_max, format, 0, "", false, true);
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

            gui->end();
        }

        gui->pop_var(4);
    }

    return value_changed;
}

bool c_widgets::slider_float(std::string_view label, float* value, float value_min, float value_max, std::string_view description, bool keybind, bool is_keybind) {
    return widgets->slider_scalar(label, ImGuiDataType_Float, value, &value_min, &value_max, "%.1f", 0, description, keybind, is_keybind);
}

bool c_widgets::slider_int(std::string_view label, float* value, float value_min, float value_max, std::string_view description, bool keybind, bool is_keybind) {
    return widgets->slider_scalar(label, ImGuiDataType_Float, value, &value_min, &value_max, "%.0f", 0, description, keybind, is_keybind);
}