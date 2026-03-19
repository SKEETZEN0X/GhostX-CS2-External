#include "../settings/functions.h"

bool input_rgb_hex(const char* label, float col[4], bool alpha) {
    char buf[10];

    if (alpha)
        snprintf(buf, sizeof(buf), "#%02X%02X%02X%02X",
            (int)(col[0] * 255.0f),
            (int)(col[1] * 255.0f),
            (int)(col[2] * 255.0f),
            (int)(col[3] * 255.0f));
    else
        snprintf(buf, sizeof(buf), "#%02X%02X%02X",
            (int)(col[0] * 255.0f),
            (int)(col[1] * 255.0f),
            (int)(col[2] * 255.0f));

    bool value_changed = widgets->input_text_icon(label, var->icons.pencil, buf, sizeof(buf), ImGuiInputTextFlags_CharsUppercase);

    if (value_changed)
    {
        if (buf[0] != '#')
        {
            memmove(buf + 1, buf, sizeof(buf) - 2);
            buf[0] = '#';
        }

        for (int i = 1; i < sizeof(buf) - 1; ++i)
        {
            if (buf[i] == '\0' || !isxdigit(buf[i]))
            {
                buf[i] = 'F';
            }
        }

        if (alpha && strlen(buf) < 9)
        {
            for (int i = strlen(buf); i < 9; ++i)
                buf[i] = 'F';
            buf[9] = '\0';
        }
        else if (!alpha && strlen(buf) < 7)
        {
            for (int i = strlen(buf); i < 7; ++i)
                buf[i] = 'F';
            buf[7] = '\0';
        }

        int r, g, b, a = 255;
        if (alpha)
            sscanf(buf, "#%02X%02X%02X%02X", &r, &g, &b, &a);
        else
            sscanf(buf, "#%02X%02X%02X", &r, &g, &b);

        col[0] = r / 255.0f;
        col[1] = g / 255.0f;
        col[2] = b / 255.0f;
        col[3] = a / 255.0f;
    }

    return false;
}

static void color_edit_restore_h(const float* col, float* H) {
    ImGuiContext& g = *GImGui;
    IM_ASSERT(g.ColorEditCurrentID != 0);
    if (g.ColorEditSavedID != g.ColorEditCurrentID || g.ColorEditSavedColor != ImGui::ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0)))
        return;
    *H = g.ColorEditSavedHue;
}

static void color_edit_restore_hs(const float* col, float* H, float* S, float* V) {
    ImGuiContext& g = *GImGui;
    IM_ASSERT(g.ColorEditCurrentID != 0);
    if (g.ColorEditSavedID != g.ColorEditCurrentID || g.ColorEditSavedColor != ImGui::ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0)))
        return;
    if (*S == 0.0f || (*H == 0.0f && g.ColorEditSavedHue == 1))
        *H = g.ColorEditSavedHue;
    if (*V == 0.0f)
        *S = g.ColorEditSavedSat;
}

bool c_widgets::color_picker(std::string_view label, float col[4], bool alpha)
{
    ImGuiColorEditFlags flags = 0;

    struct anim_state {
        float circle_pos_x = GetCurrentWindow()->Pos.x + SCALE(15.f);
        float circle_pos_y = GetCurrentWindow()->Pos.y + SCALE(15.f);
        float bar0_pos_y, bar1_pos_y, circle_size = 0.f;
    };

    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImDrawList* drawlist = GetWindowDrawList();
    ImGuiStyle& style = g.Style;
    ImGuiIO& io = g.IO;

    const ImGuiID id = window->GetID(label.data());
    anim_state* state = gui->anim_container(&state, id);

    const float width = CalcItemWidth();
    const bool is_readonly = ((g.NextItemData.ItemFlags | g.CurrentItemFlags) & ImGuiItemFlags_ReadOnly) != 0;
    g.NextItemData.ClearFlags();

    PushID(label.data());
    const bool set_current_color_edit_id = (g.ColorEditCurrentID == 0);
    if (set_current_color_edit_id)
        g.ColorEditCurrentID = window->IDStack.back();
    BeginGroup();

    if (!(flags & ImGuiColorEditFlags_PickerMask_))
        flags |= ((g.ColorEditOptions & ImGuiColorEditFlags_PickerMask_) ? g.ColorEditOptions : ImGuiColorEditFlags_DefaultOptions_) & ImGuiColorEditFlags_PickerMask_;
    if (!(flags & ImGuiColorEditFlags_InputMask_))
        flags |= ((g.ColorEditOptions & ImGuiColorEditFlags_InputMask_) ? g.ColorEditOptions : ImGuiColorEditFlags_DefaultOptions_) & ImGuiColorEditFlags_InputMask_;
    IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_PickerMask_)); // Check that only 1 is selected
    IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_InputMask_));  // Check that only 1 is selected
    if (!(flags & ImGuiColorEditFlags_NoOptions))
        flags |= (g.ColorEditOptions & ImGuiColorEditFlags_AlphaBar);

    int components = alpha ? 4 : 3;
    bool alpha_bar = alpha;
    ImVec2 picker_pos = window->DC.CursorPos;
    float bars_width = SCALE(6.f);
    float sv_picker_size = SCALE(130.f);
    float bar0_pos_x = picker_pos.x + sv_picker_size + style.ItemInnerSpacing.x;
    float bar1_pos_x = bar0_pos_x + bars_width + style.ItemInnerSpacing.x;
    float bars_triangles_half_sz = IM_TRUNC(bars_width * 0.20f);

    float backup_initial_col[4];
    memcpy(backup_initial_col, col, components * sizeof(float));

    float wheel_thickness = sv_picker_size * 0.08f;
    float wheel_r_outer = sv_picker_size * 0.50f;
    float wheel_r_inner = wheel_r_outer - wheel_thickness;
    ImVec2 wheel_center(picker_pos.x + (sv_picker_size + bars_width) * 0.5f, picker_pos.y + sv_picker_size * 0.5f);

    float triangle_r = wheel_r_inner - (int)(sv_picker_size * 0.027f);
    ImVec2 triangle_pa = ImVec2(triangle_r, 0.0f); // Hue point.
    ImVec2 triangle_pb = ImVec2(triangle_r * -0.5f, triangle_r * -0.866025f); // Black point.
    ImVec2 triangle_pc = ImVec2(triangle_r * -0.5f, triangle_r * +0.866025f); // White point.

    float H = col[0], S = col[1], V = col[2];
    float R = col[0], G = col[1], B = col[2];
    if (flags & ImGuiColorEditFlags_InputRGB)
    {
        // Hue is lost when converting from grayscale rgb (saturation=0). Restore it.
        ColorConvertRGBtoHSV(R, G, B, H, S, V);
        color_edit_restore_hs(col, &H, &S, &V);
    }
    else if (flags & ImGuiColorEditFlags_InputHSV)
    {
        ColorConvertHSVtoRGB(H, S, V, R, G, B);
    }

    bool value_changed = false, value_changed_h = false, value_changed_sv = false;

    PushItemFlag(ImGuiItemFlags_NoNav, true);
    if (flags & ImGuiColorEditFlags_PickerHueBar)
    {
        widgets->invisible_button("sv", ImVec2(sv_picker_size, sv_picker_size));
        gui->easing(state->circle_size, IsItemActive() ? elements->color_picker.circle_size_active : elements->color_picker.circle_size, var->anim.speed, dynamic_easing);
        if (IsItemActive() && !is_readonly)
        {
            S = ImSaturate((io.MousePos.x - picker_pos.x) / (sv_picker_size - 1));
            V = 1.0f - ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));
            color_edit_restore_h(col, &H); // Greatly reduces hue jitter and reset to 0 when hue == 255 and color is rapidly modified using SV square.
            value_changed = value_changed_sv = true;
        }

        SetCursorScreenPos(ImVec2(bar0_pos_x, picker_pos.y));
        widgets->invisible_button("hue", ImVec2(bars_width, sv_picker_size));
        if (IsItemActive() && !is_readonly)
        {
            H = ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));
            value_changed = value_changed_h = true;
        }
    }

    if (alpha_bar)
    {
        SetCursorScreenPos(ImVec2(bar1_pos_x, picker_pos.y));
        widgets->invisible_button("alpha", ImVec2(bars_width, sv_picker_size));
        if (IsItemActive())
        {
            col[3] = 1.0f - ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));
            value_changed = true;
        }
    }
    PopItemFlag(); 

    if ((flags & ImGuiColorEditFlags_NoInputs) == 0)
    {
        PushItemWidth(SCALE(alpha ? elements->color_picker.input_width_1 : elements->color_picker.input_width_2));
        value_changed |= input_rgb_hex(label.data(), col, alpha);
        PopItemWidth();
    }

    if (value_changed_h || value_changed_sv)
    {
        if (flags & ImGuiColorEditFlags_InputRGB)
        {
            ColorConvertHSVtoRGB(H, S, V, col[0], col[1], col[2]);
            g.ColorEditSavedHue = H;
            g.ColorEditSavedSat = S;
            g.ColorEditSavedID = g.ColorEditCurrentID;
            g.ColorEditSavedColor = ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0));
        }
        else if (flags & ImGuiColorEditFlags_InputHSV)
        {
            col[0] = H;
            col[1] = S;
            col[2] = V;
        }
    }

    const int style_alpha8 = IM_F32_TO_INT8_SAT(style.Alpha);
    const ImU32 col_black = IM_COL32(0, 0, 0, style_alpha8);
    const ImU32 col_white = IM_COL32(255, 255, 255, style_alpha8);
    const ImU32 col_midgrey = IM_COL32(128, 128, 128, style_alpha8);
    const ImU32 col_hues[6 + 1] = { IM_COL32(255,0,0,style_alpha8), IM_COL32(255,255,0,style_alpha8), IM_COL32(0,255,0,style_alpha8), IM_COL32(0,255,255,style_alpha8), IM_COL32(0,0,255,style_alpha8), IM_COL32(255,0,255,style_alpha8), IM_COL32(255,0,0,style_alpha8) };

    ImVec4 hue_color_f(1, 1, 1, style.Alpha); ColorConvertHSVtoRGB(H, 1, 1, hue_color_f.x, hue_color_f.y, hue_color_f.z);
    ImU32 hue_color32 = ColorConvertFloat4ToU32(hue_color_f);
    ImU32 user_col32_striped_of_alpha = ColorConvertFloat4ToU32(ImVec4(R, G, B, style.Alpha)); // Important: this is still including the main rendering/style alpha!!

    ImVec2 sv_cursor_pos;

    if (flags & ImGuiColorEditFlags_PickerHueBar)
    {
        draw->rect_filled_multi_color(drawlist, picker_pos, picker_pos + ImVec2(sv_picker_size, sv_picker_size), col_white, hue_color32, hue_color32, col_white, SCALE(elements->rounding + 1), ImDrawFlags_RoundCornersAll);
        draw->rect_filled_multi_color(drawlist, picker_pos, picker_pos + ImVec2(sv_picker_size, sv_picker_size), 0, 0, col_black, col_black, SCALE(elements->rounding), ImDrawFlags_RoundCornersAll);

        sv_cursor_pos.x = ImClamp(IM_ROUND(picker_pos.x + ImSaturate(S) * sv_picker_size), picker_pos.x + 2, picker_pos.x + sv_picker_size - 2); // Sneakily prevent the circle to stick out too much
        sv_cursor_pos.y = ImClamp(IM_ROUND(picker_pos.y + ImSaturate(1 - V) * sv_picker_size), picker_pos.y + 2, picker_pos.y + sv_picker_size - 2);

        for (int i = 0; i < 6; ++i) {
            if ( i == 0 )
                draw->rect_filled_multi_color(drawlist, ImVec2(bar0_pos_x, picker_pos.y + i * (sv_picker_size / 6)), ImVec2(bar0_pos_x + bars_width, picker_pos.y + (i + 1) * (sv_picker_size / 6) + SCALE(1.f)), col_hues[i], col_hues[i], col_hues[i + 1], col_hues[i + 1], SCALE(elements->rounding), ImDrawFlags_RoundCornersTop);
            else if ( i == 5) 
                draw->rect_filled_multi_color(drawlist, ImVec2(bar0_pos_x, picker_pos.y + i * (sv_picker_size / 6) - SCALE(1.f)), ImVec2(bar0_pos_x + bars_width, picker_pos.y + (i + 1) * (sv_picker_size / 6)), col_hues[i], col_hues[i], col_hues[i + 1], col_hues[i + 1], SCALE(elements->rounding), ImDrawFlags_RoundCornersBottom);
            else
                draw->rect_filled_multi_color(drawlist, ImVec2(bar0_pos_x, picker_pos.y + i * (sv_picker_size / 6)), ImVec2(bar0_pos_x + bars_width, picker_pos.y + (i + 1) * (sv_picker_size / 6)), col_hues[i], col_hues[i], col_hues[i + 1], col_hues[i + 1]);
        }

        float bar0_line_y = IM_ROUND(picker_pos.y + H * sv_picker_size);
        gui->easing(state->bar0_pos_y, bar0_line_y - picker_pos.y, var->anim.speed, dynamic_easing);
        draw->rect_filled(drawlist, ImVec2(bar0_pos_x - SCALE(1.f), picker_pos.y + state->bar0_pos_y - SCALE(3.f)), ImVec2(bar0_pos_x + SCALE(7.f), picker_pos.y + state->bar0_pos_y + SCALE(3.f)), draw->get_clr(clr->main.light_2), SCALE(elements->rounding));
    
    }

    if (alpha_bar)
    {
        float alpha = ImSaturate(col[3]);
        ImRect bar1_bb(bar1_pos_x, picker_pos.y, bar1_pos_x + bars_width, picker_pos.y + sv_picker_size);
        
        //RenderColorRectWithAlphaCheckerboard(drawlist, bar1_bb.Min, bar1_bb.Max, 0, bar1_bb.GetWidth() / 2.0f, ImVec2(0.0f, 0.0f), SCALE(elements->rounding), ImDrawFlags_RoundCornersAll);
        draw->rect_filled_multi_color(drawlist, bar1_bb.Min, bar1_bb.Max, user_col32_striped_of_alpha, user_col32_striped_of_alpha, user_col32_striped_of_alpha & ~IM_COL32_A_MASK, user_col32_striped_of_alpha & ~IM_COL32_A_MASK, SCALE(elements->rounding), ImDrawFlags_RoundCornersAll);
        
        float bar1_line_y = IM_ROUND(picker_pos.y + (1.0f - alpha) * sv_picker_size);
        gui->easing(state->bar1_pos_y, bar1_line_y - picker_pos.y, var->anim.speed, dynamic_easing);
        draw->rect_filled(drawlist, ImVec2(bar1_pos_x - SCALE(1.f), picker_pos.y + state->bar1_pos_y - SCALE(3.f)), ImVec2(bar1_pos_x + SCALE(7.f), picker_pos.y + state->bar1_pos_y + SCALE(3.f)), draw->get_clr(clr->main.light_2), SCALE(elements->rounding));
    }

    gui->easing(state->circle_pos_x, sv_cursor_pos.x, var->anim.speed, dynamic_easing);
    gui->easing(state->circle_pos_y, sv_cursor_pos.y, var->anim.speed, dynamic_easing);
    draw->circle(drawlist, ImVec2(state->circle_pos_x, state->circle_pos_y), SCALE(state->circle_size), draw->get_clr(clr->main.light_2), 32, 1.f);

    EndGroup();

    if (value_changed && memcmp(backup_initial_col, col, components * sizeof(float)) == 0)
        value_changed = false;
    if (value_changed && g.LastItemData.ID != 0) // In case of ID collision, the second EndGroup() won't catch g.ActiveId
        MarkItemEdited(g.LastItemData.ID);

    if (set_current_color_edit_id)
        g.ColorEditCurrentID = 0;
    PopID();

    return value_changed;
}

bool c_widgets::color_button(std::string_view label, float col[4], bool alpha) {

    struct anim_state {
        ImVec2 begin_pos;
        bool open_begin = false;
        float begin_anim = 0.f;
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
    const ImVec2 mouse_pos = GetMousePos();
    ImDrawList* drawlist = GetWindowDrawList();
    ImVec2 padding = SCALE(elements->checkbox.padding);

    const ImVec2 size = ImVec2(GetCurrentWindow()->Size.x, padding.y * 2 + label_size.y);
    const ImRect total_bb(pos, pos + size);
    const ImRect label_bb(total_bb.Min + padding, total_bb.Min + padding + label_size);
    const float cirlce_size = elements->checkbox.colorpicker_circle_size;
    const ImRect button_bb(ImVec2(total_bb.Max.x - SCALE(cirlce_size * 2) - padding.x, label_bb.GetCenter().y - SCALE(cirlce_size)), ImVec2(total_bb.Max.x - padding.x, label_bb.GetCenter().y + SCALE(cirlce_size)));

    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(button_bb, id, &hovered, &held);

    gui->easing(state->begin_anim, state->open_begin ? 1.f : 0.f, var->anim.speed, dynamic_easing);

    draw->circle_filled(drawlist, button_bb.GetCenter(), SCALE(elements->checkbox.colorpicker_circle_size), draw->get_clr(ImVec4(col[0], col[1], col[2], 1.f)), 32);
    draw->text_clipped(drawlist, var->font.medium10, label_bb.Min, label_bb.Max, draw->get_clr(clr->text.inactive), label.data());
    
    draw->line(drawlist, ImVec2(total_bb.Min.x + padding.x, total_bb.Max.y), total_bb.Max - ImVec2(padding.x, 0), draw->get_clr(clr->main.border));

    if (pressed) {
        state->open_begin = true;
    }

    if (IsKeyDown(ImGuiKey_MouseWheelY)) {
        state->open_begin = false;
    }

    if (state->begin_anim > 0.01f) {

        state->begin_pos = button_bb.Min;

        gui->set_next_window_pos(state->begin_pos);
        gui->push_var(ImGuiStyleVar_Alpha, state->begin_anim);
        gui->push_var(ImGuiStyleVar_WindowPadding, SCALE(elements->checkbox.window_padding));
        gui->push_var(ImGuiStyleVar_ItemInnerSpacing, SCALE(elements->checkbox.window_padding));
        gui->push_var(ImGuiStyleVar_ItemSpacing, SCALE(elements->checkbox.window_padding));

        std::string main_begin_name = std::string(label.data()) + "color_window";

        if (gui->begin(main_begin_name, nullptr, elements->checkbox.window_flags)) {

            draw->rect_filled(GetWindowDrawList(), GetWindowPos(), GetWindowPos() + GetWindowSize(), draw->get_clr(clr->main.dark_3_255), SCALE(2.f), ImDrawFlags_RoundCornersAll);
            draw->rect(GetWindowDrawList(), GetWindowPos(), GetWindowPos() + GetWindowSize(), draw->get_clr(clr->main.border), SCALE(2.f), ImDrawFlags_RoundCornersAll);

            widgets->color_picker(label.data(), col, alpha);

            bool color_hovered = g.HoveredWindow && strstr(g.HoveredWindow->Name, main_begin_name.data());
            if (!color_hovered && IsMouseClicked(0))
                if (!hovered) {
                    state->open_begin = false;
                }

            gui->end();
        }

        gui->pop_var(4);
    }

    return pressed;
}
