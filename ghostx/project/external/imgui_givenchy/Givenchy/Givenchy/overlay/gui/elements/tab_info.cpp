#include "../settings/functions.h"

bool c_widgets::tab_info() {

    struct anim_state {
        bool begin_open = false;
        float begin_alpha = 0.f;
    };

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const std::string_view label = "tab_info";
    const ImGuiID id = window->GetID(label.data());

    anim_state* state = gui->anim_container(&state, id);

    const ImVec2 pos = window->DC.CursorPos;
    const ImVec2 crosshair_size = gui->text_size(var->font.icon15, var->icons.crosshair.data());
    const ImVec2 keybinds_size = gui->text_size(var->font.icon12, var->icons.gear.data());
    const ImVec2 tab_size = gui->text_size(var->font.medium12, var->tab.tab_name.data());
    const ImVec2 subtab_size = gui->text_size(var->font.medium12, var->tab.subtab_name.data());
    const ImVec2 size = SCALE(elements->tab_info.size);
    const float circle_size = SCALE(elements->tab_info.circle_size);

    ImDrawList* drawlist = GetWindowDrawList();

    ImVec2 padding = SCALE(15.f, 18.f);
    const ImRect total_bb(pos, pos + size);
    const ImRect crosshair_bb(total_bb.Min + ImVec2(padding.x, size.y / 2 - crosshair_size.y / 2), total_bb.Min + ImVec2(padding.x + crosshair_size.x, size.y / 2 + crosshair_size.y / 2));
    const ImRect keybinds_bb(total_bb.Max - ImVec2(padding.x + keybinds_size.x, size.y / 2 + keybinds_size.y / 2), total_bb.Max - ImVec2(padding.x, size.y / 2 - keybinds_size.y / 2 - SCALE(2.f)));
    const ImRect tab_bb(ImVec2(crosshair_bb.Max.x + padding.x, crosshair_bb.GetCenter().y - tab_size.y / 2), ImVec2(crosshair_bb.Max.x + padding.x + tab_size.x, crosshair_bb.GetCenter().y + tab_size.y / 2));
    const ImRect subtab_bb(ImVec2(tab_bb.Max.x + padding.x * 2, crosshair_bb.GetCenter().y - subtab_size.y / 2), ImVec2(tab_bb.Max.x + padding.x * 2 + subtab_size.x, crosshair_bb.GetCenter().y + subtab_size.y / 2));
    const ImVec2 circle_pos(ImVec2(tab_bb.Max.x + padding.x, tab_bb.GetCenter().y));

    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(keybinds_bb, id, &hovered, &held);

    if (pressed)
        state->begin_open = true;

    gui->easing(state->begin_alpha, state->begin_open ? 1.f : 0.f, var->anim.speed, dynamic_easing);

    draw->text_clipped(drawlist, var->font.icon15, crosshair_bb.Min, crosshair_bb.Max, draw->get_clr(clr->accent.main), var->icons.crosshair.data());
    draw->text_clipped(drawlist, var->font.icon12, keybinds_bb.Min, keybinds_bb.Max, draw->get_clr(clr->text.inactive), var->icons.gear.data());
    draw->text_clipped(drawlist, var->font.medium12, tab_bb.Min, tab_bb.Max, draw->get_clr(clr->text.active), var->tab.tab_name.data());
    draw->text_clipped(drawlist, var->font.medium12, subtab_bb.Min, subtab_bb.Max, draw->get_clr(clr->accent.main), var->tab.subtab_name.data());
    draw->circle_filled(drawlist, circle_pos, circle_size, draw->get_clr(clr->accent.main), 32);

    if (state->begin_alpha > 0.01f) {

        ImGuiWindow* main_window = FindWindowByName(var->window.window_name.data());

        gui->set_next_window_pos(main_window->Pos + ImVec2(main_window->Size.x + SCALE(20.f), 0.f));
        gui->push_var(ImGuiStyleVar_Alpha, state->begin_alpha);
        gui->push_var(ImGuiStyleVar_WindowPadding, ImVec2(0, 8));
        if (gui->begin("Keybinds list", nullptr, elements->checkbox.window_flags)) {

            draw->rect_filled(GetWindowDrawList(), GetWindowPos(), GetWindowPos() + GetWindowSize(), draw->get_clr(clr->main.dark_2), SCALE(elements->rounding), ImDrawFlags_RoundCornersAll);
            draw->rect(GetWindowDrawList(), GetWindowPos(), GetWindowPos() + GetWindowSize(), draw->get_clr(clr->main.border), SCALE(elements->rounding), ImDrawFlags_RoundCornersAll);

            for (int i = 0; i < widgets->keybinds.size(); i++) {
                float get_text_size = gui->text_size(var->font.medium11, widgets->keybinds[i].label.data()).x;
                float max_text_size = gui->text_size(var->font.medium11, elements->keybinds_list.max_label.data()).x;
                if (get_text_size > max_text_size) {
                    elements->keybinds_list.max_label = widgets->keybinds[i].label;
                }
            }

            gui->begin_content("render main list", ImVec2(SCALE(300.f) + gui->text_size(var->font.medium11, elements->keybinds_list.max_label.data()).x, SCALE(20.f)), SCALE(0, 0), SCALE(0, 0));
            {
                widgets->render_main_keybind_stoke();
            }
            gui->end_content();

            gui->begin_content("render list", ImVec2(SCALE(300.f) + gui->text_size(var->font.medium11, elements->keybinds_list.max_label.data()).x, SCALE(110.f)), SCALE(0, 5), SCALE(0, 8));
            {
                for (int i = 0; i < widgets->keybinds.size(); i++) {
                    widgets->keybind_stroke(&widgets->keybinds[i], i);
                }
            }
            gui->end_content();

            gui->end();

            bool var_hovered = g.HoveredWindow && strstr(g.HoveredWindow->Name, "Keybinds list");

            if (!var_hovered && !hovered && IsMouseClicked(0))
                state->begin_open = false;
        }
        gui->pop_var(2);
    }

    return pressed;
}