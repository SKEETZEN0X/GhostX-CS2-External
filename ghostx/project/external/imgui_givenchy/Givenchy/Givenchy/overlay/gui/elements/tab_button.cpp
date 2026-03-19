#include "../settings/functions.h"

bool c_widgets::subtab_button(std::string_view label, int tab) {

    struct anim_state {
        ImVec4 background = clr->main.dark_3_255;
        ImVec4 text_color = clr->text.inactive;
        ImVec4 icon_color = clr->accent.invisble;
        bool selected = false;
    };

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label.data());
    anim_state* state = gui->anim_container(&state, id);

    if (var->tab.active == tab)
        state->selected = true;
    else
        state->selected = false;

    const ImVec2 pos = window->DC.CursorPos;
    const ImVec2 label_size = gui->text_size(var->font.medium9, label.data());
    const ImVec2 check_size = gui->text_size(var->font.icon9, var->icons.check.data());
    const ImVec2 size = SCALE(elements->subtab_button.size);
    const ImVec2 padding = SCALE(elements->subtab_button.padding);

    ImDrawList* drawlist = GetForegroundDrawList();

    const ImRect bb(pos, pos + size);
    const ImRect label_pos(bb.Min + ImVec2(padding.x, size.y / 2 - label_size.y / 2), bb.Min + ImVec2(padding.x + label_size.x, size.y / 2 + label_size.y / 2));
    const ImRect check_pos(bb.Max - ImVec2(padding.x + check_size.x - SCALE(1.f), size.y / 2 + check_size.y / 2), bb.Max - ImVec2(padding.x - SCALE(1.f), size.y / 2 - check_size.y / 2));

    ItemSize(bb, style.FramePadding.y);

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);
    if (pressed) {
        var->tab.active = tab;
        var->tab.subtab_name = label;
    }

    if (!ItemAdd(bb, id)) {
        state->background = state->selected ? clr->main.dark_4 : clr->main.dark_3_255;
        state->text_color = state->selected ? clr->text.active : clr->text.inactive;
        state->icon_color = state->selected ? clr->accent.main.Value : clr->accent.invisble.Value;
        return false;
    }
    else {
        gui->easing(state->background, state->selected ? clr->main.dark_4.Value : hovered ? clr->main.dark_5.Value : clr->main.dark_3_255.Value, var->anim.speed, dynamic_easing);
        gui->easing(state->text_color, state->selected ? clr->text.active.Value :hovered ? clr->text.hover.Value : clr->text.inactive.Value, var->anim.speed, dynamic_easing);
        gui->easing(state->icon_color, state->selected ? clr->accent.main.Value : clr->accent.invisble.Value, var->anim.speed, dynamic_easing);
    }

    float rounding = gui->is_last_item(1, pos) ? elements->rounding : 0.f;
    ImDrawFlags flags = gui->is_last_item(1, pos) ? ImDrawFlags_RoundCornersBottom : 0;
    draw->rect_filled(drawlist, bb.Min, bb.Max, draw->get_clr(state->background), SCALE(rounding), flags);
    draw->text_clipped(drawlist, var->font.medium9, label_pos.Min, label_pos.Max, draw->get_clr(state->text_color), label.data());
    draw->text_clipped(drawlist, var->font.icon9, check_pos.Min, check_pos.Max, draw->get_clr(state->icon_color), var->icons.check.data());

    return pressed;
}

bool c_widgets::tab_button(std::string_view label, std::vector<int> tabs, std::vector<std::string_view> subtabs)
{
    struct anim_state {
        ImVec4 background = clr->main.dark_3_0;
        ImVec4 text_color = clr->text.inactive;
        ImVec4 border_color = clr->main.border_0;

        float arrow_rad = 90.f;

        ImVec2 begin_pos, begin_size = ImVec2(0.f, 0.f);
        bool is_active, open_begin = false;
        float begin_anim = 0.f;
    };

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label.data());
    anim_state* state = gui->anim_container(&state, id);

    const ImVec2 pos = window->DC.CursorPos;
    const ImVec2 mouse_pos = GetMousePos();
    const ImVec2 label_size = gui->text_size(var->font.medium12, label.data());
    const ImVec2 arrow_size = gui->text_size(var->font.icon6, var->icons.arrow.data());
    const ImVec2 size = SCALE(elements->tab_button.size) + ImVec2(label_size.x, 0);
    const ImVec2 padding = SCALE(elements->tab_button.padding);

    ImDrawList* drawlist = GetForegroundDrawList();

    const ImRect bb(pos, pos + size);
    const ImRect label_bb(bb.Min + ImVec2(padding.x, size.y / 2 - label_size.y / 2), bb.Min + ImVec2(padding.x + label_size.x, size.y / 2 + label_size.y / 2));
    const ImRect arrow_bb(bb.Max - ImVec2(padding.x + arrow_size.x, size.y / 2 + arrow_size.y / 2), bb.Max - ImVec2(padding.x, size.y / 2 - arrow_size.y / 2));

    ItemSize(bb, style.FramePadding.y);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);

    state->is_active = false;

    for (int i = 0; i < 3; i++) {
        if (tabs[i] == var->tab.active) {
            state->is_active = true;
            break;
        }
    } 

    gui->easing(state->background, state->is_active ? clr->main.dark_3_255.Value : (hovered || g.LastActiveId == id) ? clr->main.dark_3_155.Value : clr->main.dark_3_0.Value, var->anim.speed, dynamic_easing);
    gui->easing(state->text_color, state->is_active ? clr->text.active.Value : (hovered || g.LastActiveId == id) ? clr->text.hover.Value : clr->text.inactive.Value, var->anim.speed, dynamic_easing);
    gui->easing(state->border_color, state->is_active ? clr->main.border.Value : clr->main.border_0.Value, var->anim.speed, dynamic_easing);
    gui->easing(state->begin_anim, state->open_begin ? 1.f : 0.f, var->anim.speed, dynamic_easing);
    gui->easing(state->arrow_rad, state->open_begin ? 270.f : 90.f, var->anim.speed, dynamic_easing);

    draw->rect_filled(drawlist, bb.Min, bb.Max, draw->get_clr(state->background), window->ContentSize.x == (pos.x - GetWindowPos().x + size.x) ? SCALE(elements->rounding * 3) : 0.f, window->ContentSize.x == (pos.x - GetWindowPos().x + size.x) ? ImDrawFlags_RoundCornersTopRight : 0);
    draw->line(drawlist, bb.Min, ImVec2(bb.Min.x, bb.Max.y), draw->get_clr(state->border_color));
    
    if (!gui->is_last_item(0, pos))
        draw->line(drawlist, ImVec2(bb.Max.x, bb.Min.y), bb.Max, draw->get_clr(state->border_color));
        
    draw->text_clipped(drawlist, var->font.medium12, label_bb.Min, label_bb.Max, draw->get_clr(state->text_color), label.data());
    draw->rotate_start(drawlist);
    draw->text_clipped(drawlist, var->font.icon6, arrow_bb.Min, arrow_bb.Max, draw->get_clr(state->text_color), var->icons.arrow.data());
    draw->rotate_end(drawlist, state->arrow_rad, arrow_bb.GetCenter());

    if (pressed && !state->open_begin)
        state->open_begin = true;

    if (state->begin_anim > 0.01f) {

        state->begin_pos = pos + ImVec2(0, size.y);
        state->begin_size = ImVec2(SCALE(elements->subtab_button.size.x), (tabs.size() * SCALE(elements->subtab_button.size.y)) * state->begin_anim);

        gui->set_next_window_pos(state->begin_pos);
        gui->set_next_window_size(state->begin_size);
        gui->push_var(ImGuiStyleVar_Alpha, state->begin_anim);
        gui->push_var(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        gui->push_var(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        ImRect window_rect(state->begin_pos, state->begin_pos + state->begin_size);

        if (gui->begin(label.data(), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground)) {

            draw->push_clip_rect(drawlist, GetWindowPos(), GetWindowPos() + GetWindowSize(), true);

            for (int i = 0; i < tabs.size(); i++) {
                if (widgets->subtab_button(subtabs[i], tabs[i])) {
                    var->tab.tab_name = label;
                }
            }

            draw->pop_clip_rect(drawlist);

            draw->rect(drawlist, GetWindowPos(), GetWindowPos() + GetWindowSize(), draw->get_clr(clr->main.border), SCALE(elements->rounding), ImDrawFlags_RoundCornersBottom);
            
            if (!window_rect.Contains(mouse_pos) && IsMouseClicked(0))
                if (!hovered)
                    state->open_begin = false;

            gui->end();
        }

        gui->pop_var(3);
    }

    return pressed;
}