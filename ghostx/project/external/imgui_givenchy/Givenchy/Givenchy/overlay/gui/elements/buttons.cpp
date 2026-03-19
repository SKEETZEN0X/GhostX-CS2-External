#include "../settings/functions.h"

bool c_widgets::invisible_button(std::string_view label, ImVec2 size) {

	return InvisibleButton(label.data(), size, ImGuiButtonFlags_None);

}

bool c_widgets::button(std::string_view label) {

    struct anim_state {
        ImVec4 text_color = clr->text.inactive;
        ImVec4 circle_color = clr->main.border_0;
        float press = 0.f;
        bool anim = false;
        ImVec2 circle_pos = ImVec2(0.f, 0.f);
    };

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label.data());
    anim_state* state = gui->anim_container(&state, id);
    const ImVec2 label_size = gui->text_size(var->font.medium10, label.data());

    const ImVec2 pos = window->DC.CursorPos;
    ImDrawList* drawlist = GetWindowDrawList();
    ImVec2 padding = SCALE(elements->button.padding);

    const ImVec2 size = ImVec2(SCALE(elements->child.size.x), padding.y * 2 + SCALE(elements->button.button_size.y));
    const ImRect total_bb(pos, pos + size);
    const ImRect button_bb(total_bb.Min + padding, total_bb.Max - padding);
    const ImRect label_bb(button_bb.GetCenter() - ImVec2(label_size.x / 2, label_size.y / 2), button_bb.GetCenter() + ImVec2(label_size.x / 2, label_size.y / 2));

    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(button_bb, id, &hovered, &held);

    gui->easing(state->text_color, hovered ? clr->text.hover.Value : clr->text.inactive.Value, var->anim.speed, dynamic_easing);

    if (pressed) {
        state->anim = true;
        state->circle_pos = GetMousePos();
    }

    if (state->anim && (state->press > SCALE(150.f) - 10.f))
        state->anim = false;

    gui->easing(state->press, state->anim ? SCALE(150.f) : 0.f, var->anim.speed, dynamic_easing);
    gui->easing(state->circle_color, state->anim ? clr->main.border.Value : clr->main.border_0.Value, var->anim.speed, dynamic_easing);

    draw->rect_filled(drawlist, button_bb.Min, button_bb.Max, draw->get_clr(clr->main.dark_4), SCALE(elements->rounding), ImDrawFlags_RoundCornersAll);
    draw->push_clip_rect(drawlist, button_bb.Min, button_bb.Max, true);
    if (state->press > 0.1f) {
        draw->circle_filled(drawlist, state->circle_pos, state->press, draw->get_clr(state->circle_color), 32);
    }
    draw->pop_clip_rect(drawlist);
    draw->rect(drawlist, button_bb.Min, button_bb.Max, draw->get_clr(clr->main.border), SCALE(elements->rounding), ImDrawFlags_RoundCornersAll);

    draw->text_clipped(drawlist, var->font.medium10, label_bb.Min, label_bb.Max, draw->get_clr(state->text_color), label.data());

    draw->line(drawlist, ImVec2(total_bb.Min.x + padding.x, total_bb.Max.y), total_bb.Max - ImVec2(padding.x, 0), draw->get_clr(clr->main.border));

    return pressed;

}

bool c_widgets::k_button(std::string_view label, int point, int* current_point) {

    struct anim_state {
        ImVec4 background_color = clr->main.dark_4_0;
        ImVec4 text_color = clr->text.inactive;
        ImVec4 border_color = clr->main.border_0;

        float arrow_rad = 90.f;
        ImVec4 arrow_color = clr->text.inactive;
    };

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label.data());
    anim_state* state = gui->anim_container(&state, id);
    const ImVec2 label_size = gui->text_size(var->font.medium10, label.data());
    const ImVec2 arrow_size = gui->text_size(var->font.icon6, var->icons.arrow.data());

    const ImVec2 pos = window->DC.CursorPos;
    ImDrawList* drawlist = GetWindowDrawList();
    ImVec2 padding = SCALE(elements->k_button.padding);

    const ImVec2 size = SCALE(elements->k_button.button_size);
    const ImRect total_bb(pos, pos + size);
    const ImRect label_bb(total_bb.Min + ImVec2(padding.x, size.y / 2 - label_size.y / 2), total_bb.Min + ImVec2(padding.x + label_size.x, size.y / 2 + label_size.y / 2));
    const ImRect arrow_bb(total_bb.Max - ImVec2(padding.x + arrow_size.x, size.y / 2 + arrow_size.y / 2), total_bb.Max - ImVec2(padding.x, size.y / 2 - arrow_size.y / 2));

    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);

    if (pressed) {
        if (*current_point == point)
            *current_point = 0;
        else if (*current_point != point)
            *current_point = point;
    }

    gui->easing(state->text_color, *current_point == point ? clr->text.hover.Value : clr->text.inactive.Value, var->anim.speed, dynamic_easing);
    gui->easing(state->background_color, *current_point == point ? clr->main.dark_4.Value : clr->main.dark_4_0.Value, var->anim.speed, dynamic_easing);
    gui->easing(state->border_color, *current_point == point ? clr->main.border.Value : clr->main.border_0.Value, var->anim.speed, dynamic_easing);
    gui->easing(state->arrow_color, *current_point == point ? clr->text.active.Value : clr->text.inactive.Value, var->anim.speed, dynamic_easing);
    gui->easing(state->arrow_rad, *current_point == point ? 180.f : 90.f, var->anim.speed, dynamic_easing);

    draw->rect_filled(drawlist, total_bb.Min, total_bb.Max, draw->get_clr(state->background_color), SCALE(elements->rounding), ImDrawFlags_RoundCornersAll);

    draw->rect(drawlist, total_bb.Min, total_bb.Max, draw->get_clr(state->border_color), SCALE(elements->rounding), ImDrawFlags_RoundCornersAll);

    draw->text_clipped(drawlist, var->font.medium10, label_bb.Min, label_bb.Max, draw->get_clr(state->text_color), label.data());
    
    draw->rotate_start(drawlist);
    draw->text_clipped(drawlist, var->font.icon6, arrow_bb.Min, arrow_bb.Max, draw->get_clr(state->arrow_color), var->icons.arrow.data());
    draw->rotate_end(drawlist, state->arrow_rad, arrow_bb.GetCenter());
    return pressed;
}

bool c_widgets::k_accent_button(std::string_view label) {

    struct anim_state {
        ImVec4 text_color = clr->text.inactive;
    };

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label.data());
    anim_state* state = gui->anim_container(&state, id);
    const ImVec2 label_size = gui->text_size(var->font.medium10, label.data());

    const ImVec2 pos = window->DC.CursorPos;
    ImDrawList* drawlist = GetWindowDrawList();
    ImVec2 padding = SCALE(elements->k_button.padding);

    const ImVec2 size = SCALE(elements->k_button.button_size);
    const ImRect total_bb(pos, pos + size);
    const ImRect label_bb(total_bb.Min + ImVec2(padding.x, size.y / 2 - label_size.y / 2), total_bb.Min + ImVec2(padding.x + label_size.x, size.y / 2 + label_size.y / 2));

    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);

    gui->easing(state->text_color, hovered ? clr->accent.main.Value : clr->text.inactive.Value, var->anim.speed, dynamic_easing);

    draw->text_clipped(drawlist, var->font.medium10, label_bb.Min, label_bb.Max, draw->get_clr(state->text_color), label.data());

    return pressed;
}

bool c_widgets::mode_button(std::string_view label, int* mode) {

    struct anim_state {
        float draw_pos = 0.f;
        ImVec4 toggle_color = clr->text.inactive;
        ImVec4 hold_color = clr->text.inactive;
    };

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label.data());
    anim_state* state = gui->anim_container(&state, id);

    const ImVec2 toggle_size = gui->text_size(var->font.medium10, "Toggle");
    const ImVec2 hold_size = gui->text_size(var->font.medium10, "Hold");
    const ImVec2 pos = window->DC.CursorPos;
    const ImVec2 size = SCALE(159.f, 25.f);
    const ImVec2 button_size = SCALE(74.5f, 25.f);
    const ImVec2 padding = SCALE(10.f, 0.f);
    const ImRect total_bb(pos, pos + size);
    const ImRect button1_bb(pos, pos + button_size);
    const ImRect button2_bb(ImVec2(pos.x + size.x - button_size.x, pos.y), ImVec2(pos.x + size.x, pos.y + button_size.y));
    const ImRect toggle_bb(button1_bb.GetCenter() - ImVec2(toggle_size.x / 2, toggle_size.y / 2), button1_bb.GetCenter() + ImVec2(toggle_size.x / 2, toggle_size.y / 2));
    const ImRect hold_bb(button2_bb.GetCenter() - ImVec2(hold_size.x / 2, hold_size.y / 2), button2_bb.GetCenter() + ImVec2(hold_size.x / 2, hold_size.y / 2));

    ImDrawList* drawlist = GetWindowDrawList();

    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id))
        return false;

    bool toggle_clicked = button1_bb.Contains(GetMousePos()) && IsMouseClicked(0);
    bool hold_clicked = button2_bb.Contains(GetMousePos()) && IsMouseClicked(0);

    if (toggle_clicked)
        *mode = 0;

    if (hold_clicked)
        *mode = 1;

    gui->easing(state->draw_pos, *mode == 0 ? 0.f : button_size.x + padding.x, var->anim.speed, dynamic_easing);
    gui->easing(state->toggle_color, *mode == 0 ? clr->text.active.Value : clr->text.inactive.Value, var->anim.speed, dynamic_easing);
    gui->easing(state->hold_color, *mode == 1 ? clr->text.active.Value : clr->text.inactive.Value, var->anim.speed, dynamic_easing);

    draw->rect_filled(drawlist, total_bb.Min, total_bb.Max, draw->get_clr(clr->main.dark_4), SCALE(elements->rounding), ImDrawFlags_RoundCornersAll);
    draw->rect_filled(drawlist, total_bb.Min + ImVec2(state->draw_pos, 0.f), total_bb.Min + ImVec2(state->draw_pos + button_size.x, button_size.y), draw->get_clr(clr->main.dark_7), SCALE(elements->rounding), ImDrawFlags_RoundCornersAll);
    draw->text_clipped(drawlist, var->font.medium10, toggle_bb.Min, toggle_bb.Max, draw->get_clr(state->toggle_color), "Toggle");
    draw->text_clipped(drawlist, var->font.medium10, hold_bb.Min, hold_bb.Max, draw->get_clr(state->hold_color), "Hold");

    return true;
}

bool c_widgets::delete_keybind_button(std::string_view label, std::string_view button_random_id) {

    struct anim_state {
        ImVec4 icon_color = clr->text.inactive;
        ImVec4 text_color = clr->text.inactive;
    };

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(button_random_id.data());
    anim_state* state = gui->anim_container(&state, id);

    const ImVec2 label_size = gui->text_size(var->font.medium10, label.data());
    const ImVec2 trash_size = gui->text_size(var->font.icon9, var->icons.trash.data());
    const ImVec2 pos = window->DC.CursorPos;
    const ImVec2 size = SCALE(159.f, 25.f);
    const ImVec2 padding = SCALE(10.f, 0.f);
    const ImRect total_bb(pos, pos + size);
    const ImRect label_bb(total_bb.Min + ImVec2(padding.x, size.y / 2 - label_size.y / 2), total_bb.Min + ImVec2(padding.x + label_size.x, size.y / 2 + label_size.y / 2));
    const ImRect trash_bb(ImVec2(total_bb.Max.x - padding.x - trash_size.x, total_bb.Min.y + size.y / 2 - trash_size.y / 2), ImVec2(total_bb.Max.x - padding.x, total_bb.Min.y + size.y / 2 + trash_size.y / 2));

    ImDrawList* drawlist = GetWindowDrawList();

    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);

    gui->easing(state->icon_color, hovered ? clr->accent.main.Value : clr->text.inactive.Value, var->anim.speed, dynamic_easing);
    gui->easing(state->text_color, hovered ? clr->text.active.Value : clr->text.inactive.Value, var->anim.speed, dynamic_easing);

    draw->rect_filled(drawlist, total_bb.Min, total_bb.Max, draw->get_clr(clr->main.dark_4), SCALE(elements->rounding), ImDrawFlags_RoundCornersAll);
    draw->text_clipped(drawlist, var->font.medium10, label_bb.Min, label_bb.Max, draw->get_clr(state->text_color), label.data());
    draw->text_clipped(drawlist, var->font.icon9, trash_bb.Min, trash_bb.Max, draw->get_clr(state->icon_color), var->icons.trash.data());

    return pressed;
}

bool c_widgets::sized_text(std::string_view label, ImVec2 size) {

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label.data());

    const ImVec2 label_size = gui->text_size(var->font.medium11, label.data());
    const ImVec2 pos = window->DC.CursorPos;
    const ImRect total_bb(pos, pos + size);
    const ImRect label_bb(total_bb.GetCenter() - ImVec2(label_size.x / 2, label_size.y / 2), total_bb.GetCenter() + ImVec2(label_size.x / 2, label_size.y / 2));

    ImDrawList* drawlist = GetWindowDrawList();

    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id))
        return false;

    draw->text_clipped(drawlist, var->font.medium11, label_bb.Min, label_bb.Max, draw->get_clr(clr->text.inactive), label.data());

    return true;
}

bool c_widgets::keybind_button(std::string_view label, int* key) {

    struct anim_state {
        ImVec4 background_color = clr->main.dark_4;
        ImVec4 text_color = clr->text.active;
        ImVec4 border_color = clr->main.border;
        float draw_pos = 0.f;
    };

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label.data());
    anim_state* state = gui->anim_container(&state, id);

    static char buf_display[64] = "None";

    ImVec2 label_size = gui->text_size(var->font.medium10, buf_display);
    ImVec2 plus_size = gui->text_size(var->font.medium15, "+");

    const ImVec2 pos = window->DC.CursorPos;
    ImDrawList* drawlist = GetWindowDrawList();
    ImVec2 padding = SCALE(elements->k_button.padding);

    const ImVec2 size = SCALE(elements->k_button.button_size);
    const ImRect frame_bb(pos, pos + size);
    const ImRect total_bb(pos, pos + size - SCALE(30.f, 0));
    const ImRect label_bb(total_bb.Min + ImVec2(padding.x, size.y / 2 - label_size.y / 2), total_bb.Min + ImVec2(padding.x + label_size.x, size.y / 2 + label_size.y / 2));
    const ImRect plus_bb(ImVec2(frame_bb.Max.x - padding.x - plus_size.x, total_bb.Min.y + size.y / 2 - plus_size.y / 2), ImVec2(frame_bb.Max.x - padding.x, total_bb.Min.y + size.y / 2 + plus_size.y / 2));

    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id))
        return false;

    bool hovered = total_bb.Contains(GetMousePos());
    bool add_hovered = plus_bb.Contains(GetMousePos());

    bool value_changed = false;
    int k = *key;

    std::string active_key = "";
    active_key += key_names[*key];

    if (*key != 0 && g.ActiveId != id) {
        strcpy_s(buf_display, active_key.c_str());
    }
    else if (g.ActiveId == id) {
        strcpy_s(buf_display, "...");
    }

    if (hovered && g.IO.MouseClicked[0])
    {
        if (g.ActiveId != id) {
            memset(g.IO.MouseDown, 0, sizeof(g.IO.MouseDown));
            memset(g.IO.KeysDown, 0, sizeof(g.IO.KeysDown));
            *key = 0;
        }
        ImGui::SetActiveID(id, window);
        ImGui::FocusWindow(window);
    }
    else if (g.IO.MouseClicked[0]) {
        if (g.ActiveId == id)
            ImGui::ClearActiveID();
    }

    if (g.ActiveId == id) {
        for (auto i = 0; i < 5; i++) {
            if (g.IO.MouseDown[i]) {
                switch (i) {
                case 0:
                    k = 0x01;
                    break;
                case 1:
                    k = 0x02;
                    break;
                case 2:
                    k = 0x04;
                    break;
                case 3:
                    k = 0x05;
                    break;
                case 4:
                    k = 0x06;
                    break;
                }
                value_changed = true;
                ImGui::ClearActiveID();
            }
        }
        if (!value_changed) {
            for (auto i = 0x08; i <= 0xA5; i++) {
                if (g.IO.KeysDown[i]) {
                    k = i;
                    value_changed = true;
                    ImGui::ClearActiveID();
                }
            }
        }

        if (IsKeyPressed(ImGuiKey_Escape)) {
            *key = 0;
            ImGui::ClearActiveID();
        }
        else {
            *key = k;
        }
    }

    draw->rect_filled(drawlist, frame_bb.Min, frame_bb.Max, draw->get_clr(state->background_color), SCALE(elements->rounding), ImDrawFlags_RoundCornersAll);
    draw->rect(drawlist, frame_bb.Min, frame_bb.Max, draw->get_clr(state->border_color), SCALE(elements->rounding), ImDrawFlags_RoundCornersAll);
    draw->text_clipped(drawlist, var->font.medium10, label_bb.Min, label_bb.Max, draw->get_clr(state->text_color), buf_display);
    draw->text_clipped(drawlist, var->font.medium15, plus_bb.Min, plus_bb.Max, draw->get_clr(clr->text.inactive), "+");

    if (add_hovered && IsMouseClicked(0)) {
        strcpy_s(buf_display, "None");
    }

    return add_hovered && IsMouseClicked(0);
}