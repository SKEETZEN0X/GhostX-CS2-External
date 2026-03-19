#include "../settings/functions.h"

void c_notify::add_notify(std::string_view text, notify_type type)
{
    notifications.push_back({ notify_count++, text, type });
}

void c_notify::setup_notify()
{
    int cur_notify_value = 0;
    float accumulated_height = 0.f;

    for (auto& notification : notifications)
    {
        cur_notify_value++;
        if (notification.active_notify)
            notification.notify_timer += gui->fixed_speed(4.f);

        if (notification.notify_timer >= notify_time)
            notification.active_notify = false;

        gui->easing(notification.notify_alpha, notification.active_notify ? 1.f : 0.f, 4.f, static_easing);

        if (notification.notify_alpha > 0.f)
        {
            float target_position = accumulated_height + notify_padding.y;
            gui->easing(notification.notify_pos, target_position, 8.f, dynamic_easing);

            ImVec2 window_size = render_notify(cur_notify_value, notification.notify_alpha, notification.notify_timer, notification.notify_pos, notification.text, notification.notify_type);

            accumulated_height += window_size.y + notify_spacing;
        }
    }
}

ImVec2 c_notify::render_notify(int cur_notify_value, float notify_alpha, float notify_percentage, float notify_pos, std::string_view text, notify_type type)
{
    const ImVec2 label_size = gui->text_size(var->font.medium11, text.data());
    const ImVec2 padding = SCALE(10.f, 10.f);
    const ImVec2 size = ImVec2(label_size.x + padding.x * 2, padding.y * 3 + SCALE(5.f) + label_size.y);

    ImVec2 window_size;

    gui->set_next_window_pos(ImVec2(notify_padding.x, notify_pos));
    gui->set_next_window_size(size);

    gui->push_var(ImGuiStyleVar_Alpha, notify_alpha);
    gui->begin(std::to_string(cur_notify_value), nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_Tooltip | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);
    {
        ImDrawList* drawlist = GetForegroundDrawList();
        const ImVec2 pos = GetWindowPos();

        const ImRect total_bb(pos, pos + size);
        const ImRect label_bb(total_bb.Min + padding, total_bb.Min + padding + label_size);
        const ImRect line_bb(ImVec2(label_bb.Min.x, label_bb.Max.y + padding.y), ImVec2(label_bb.Max.x, label_bb.Max.y + padding.y + SCALE(5.f)));

        draw->rect_filled(drawlist, total_bb.Min, total_bb.Max, draw->get_clr(clr->main.dark_1), SCALE(elements->rounding), ImDrawFlags_RoundCornersAll);
        draw->text_clipped(drawlist, var->font.medium11, label_bb.Min, label_bb.Max, draw->get_clr(clr->text.active), text.data());
        draw->rect_filled(drawlist, line_bb.Min, line_bb.Max, draw->get_clr(clr->main.dark_6), SCALE(elements->rounding), ImDrawFlags_RoundCornersAll);
        draw->rect_filled(drawlist, line_bb.Min, ImVec2(line_bb.Min.x + ((line_bb.Max.x - line_bb.Min.x) / 15.f) * notify_percentage, line_bb.Max.y), draw->get_clr(clr->accent.main), SCALE(elements->rounding), ImDrawFlags_RoundCornersAll);

        window_size = GetWindowSize();
    }
    gui->end();
    gui->pop_var();

    return window_size;
}