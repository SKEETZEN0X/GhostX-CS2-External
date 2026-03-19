#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS

#include <imgui.h>
#include <imgui_internal.h>
#include "colors.h"
#include "variables.h"
#include "elements.h"

#include "../data/fonts.h"

#include <vector>
#include <sstream>
#include <string>
#include <variant>

using namespace ImGui;

#define SCALE(...) scale_impl(__VA_ARGS__, var->gui.dpi)

inline ImVec2 scale_impl(const ImVec2& vec, float dpi) {
    return ImVec2(roundf(vec.x * dpi), roundf(vec.y * dpi));
}

inline ImVec2 scale_impl(float x, float y, float dpi) {
    return ImVec2(roundf(x * dpi), roundf(y * dpi));
}

inline float scale_impl(float var, float dpi) {
    return roundf(var * dpi);
}

enum positions
{
    pos_all,
    pos_x,
    pos_y
};

enum keybind_type
{
    is_checkbox,
    is_slider,
    is_combo,
    is_multicombo
};

enum easing_type
{
    static_easing,
    dynamic_easing
};

struct keybind_data_t {
    std::string_view label;
    int key;
    int mode;
    bool show ;
    std::string value;
};

class c_gui
{
public:
    template <typename T>
    T* anim_container(T** state_ptr, ImGuiID id)
    {
        T* state = static_cast<T*>(GetStateStorage()->GetVoidPtr(id));
        if (!state)
            GetStateStorage()->SetVoidPtr(id, state = new T());

        *state_ptr = state;
        return state;
    }

    float fixed_speed(float speed) { return speed * ImGui::GetIO().DeltaTime; }

    template<typename T>
    T easing(T& value, T val, float speed, int type)
    {
        if (type == static_easing)
        {
            if constexpr (std::is_same<T, ImVec4>::value)
            {
                return { 1.f, 1.f, 1.f, 1.f };
            }
            else
            {
                if (value < val)
                    value += fixed_speed(speed);
                if (value > val)
                    value -= fixed_speed(speed);

            }
        }
        else if (type == dynamic_easing)
        {
            value = ImLerp(value, val, fixed_speed(speed));
        }

        return value;
    }

    bool begin(std::string_view name = var->window.window_name, bool* p_open = nullptr, ImGuiWindowFlags flags = var->window.window_flags, float shadow_size = var->window.window_shadow_size, const ImU32 shadow_col = clr->main.dark_invisble);

    void end();

    void push_color(ImGuiCol idx, ImU32 col);

    void pop_color(int count = 1);

    void push_var(ImGuiStyleVar idx, float val);

    void push_var(ImGuiStyleVar idx, const ImVec2& val);

    void pop_var(int count = 1);

    void push_font(ImFont* font);

    void pop_font();

    void set_pos(const ImVec2& pos, int type);

    void set_pos(float pos, int type);

    ImVec2 get_pos();

    void set_screen_pos(const ImVec2& pos, int type);

    void set_screen_pos(float pos, int type);

    ImVec2 get_screen_pos();

    void begin_group();

    void end_group();

    void begin_content(std::string_view id, const ImVec2& size, const ImVec2& padding = ImVec2(0, 0), const ImVec2& spacing = ImVec2(0, 0), ImGuiWindowFlags flags = 0);

    void end_content();

    void sameline(float offset_from_start_x = 0.f, float spacing_w = -1.f);

    void dummy(const ImVec2& size);

    bool begin_def_child(std::string_view name, const ImVec2& size_arg = ImVec2(0, 0), ImGuiChildFlags child_flags = 0, ImGuiWindowFlags window_flags = 0);

    void end_def_child();

    void set_next_window_pos(const ImVec2& pos, ImGuiCond cond = 0, const ImVec2& pivot = ImVec2(0, 0));

    void set_next_window_size(const ImVec2& size, ImGuiCond cond = 0);

    ImVec2 text_size(ImFont* font, const char* text, const char* text_end = nullptr, bool hide_text_after_double_hash = false, float wrap_width = -1.f);

    std::string wrap_text_to_width(ImFont* font, const std::string& text, float max_width);

    ImVec2 window_size();

    float window_width();

    float window_height();

    ImVec2 content_avail();

    ImVec2 content_max();

    void set_style();

    void draw_decoration();

    void setup_animations();

    bool is_first_item(int x_or_y, ImVec2 pos);
    bool is_last_item(int x_or_y, ImVec2 pos);

    void render();
    void renderDahood();
    void renderRivals();
    void renderphantomforce();
    void renderbadbuisness();
    void renderFrontline();
    void renderhypershot();


    bool init_static(ImGuiIO& io, ImFontConfig* cfg);
    bool reinit_static(ImGuiIO& io, ImFontConfig* cfg);

};

inline std::unique_ptr<c_gui> gui = std::make_unique<c_gui>();

class c_widgets
{
public:

    bool tab_button(std::string_view label, std::vector<int> tabs, std::vector<std::string_view> subtabs);
    bool direct_tab_button(std::string_view label, int tab_index);
    bool subtab_button(std::string_view label, int tab);
    bool tab_info();

    bool setup_child_anim();
    bool child(std::string_view label);
    bool end_child();

    bool invisible_button(std::string_view label, ImVec2 size = ImVec2(0, 0));
    bool button(std::string_view label);
    bool k_button(std::string_view label, int point, int* current_point);
    bool k_accent_button(std::string_view label);
    bool keybind_button(std::string_view label, int* key);
    bool mode_button(std::string_view label, int* mode);
    bool delete_keybind_button(std::string_view label, std::string_view button_random_id);
    bool sized_text(std::string_view label, ImVec2 size);

    bool color_button(std::string_view label, float col[4], bool alpha = false);

    bool checkbox(std::string_view label, bool* value, std::string_view description = "", bool is_keybind = false);
    bool checkbox_keybind(std::string_view label, bool* value, std::string_view description = "");
    bool checkbox_colorpicker(std::string_view label, bool* value, float color[4], bool alpha = false, std::string_view description = "");
    bool checkbox_keybind_colorpicker(std::string_view label, bool* value, float color[4], bool alpha = false, std::string_view description = "");
    
    bool slider_float(std::string_view label, float* value, float value_min, float value_max, std::string_view description = "", bool keybind = false, bool is_keybind = false);
    bool slider_int(std::string_view label, float* value, float value_min, float value_max, std::string_view description = "", bool keybind = false, bool is_keybind = false);

    bool combo_button(std::string_view label, bool selected, bool is_keybind_window = false);
    bool combo(std::string_view label, int* value, std::vector<std::string> variebles, std::string_view description = "", bool keybind = false, bool is_keybind_window = false);
    bool multicombo(std::string_view label, std::vector<bool>* values, std::vector<std::string> variebles, std::string_view description = "", bool keybind = false, bool is_keybind_window = false);

    bool input_text(std::string_view label, char* buf, size_t buf_size, ImGuiInputTextFlags flags = 0);
    bool input_text_icon(std::string_view label, std::string_view icon, char* buf, size_t buf_size, ImGuiInputTextFlags flags = 0);

    bool render_main_keybind_stoke();
    bool keybind_stroke(keybind_data_t* current_keybind, int number);

    std::vector<keybind_data_t> keybinds = { };
    bool center_combo = false;

private:
    bool checkbox_ex(std::string_view label, std::string_view description, bool* value, bool is_keybind = false, bool is_color = false, float color[4] = 0, bool alpha = false, bool is_keybind_window = false);

    bool slider_scalar(std::string_view label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags, std::string_view description = "", bool keybind = false, bool is_keybind = false);

    bool color_picker(std::string_view label, float col[4], bool alpha = false);

    std::vector<ImVec2> render_combo(const ImVec2 pos, std::string_view label, std::string_view value, std::string_view description = "", bool keybind = false, bool is_keybind_window = false);
};

inline std::unique_ptr<c_widgets> widgets = std::make_unique<c_widgets>();

enum fade_direction : int
{
    vertically,
    horizontally,
    diagonally,
    diagonally_reversed,
};

class c_draw
{
public:
    ImU32 get_clr(const ImVec4& col, float alpha = 1.f);

    void text(ImDrawList* draw_list, const ImFont* font, float font_size, const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end = NULL, float wrap_width = NULL, const ImVec4* cpu_fine_clip_rect = 0);

    void text_clipped(ImDrawList* draw_list, ImFont* font, const ImVec2& pos_min, const ImVec2& pos_max, ImU32 color, const char* text, const char* text_display_end = NULL, const ImVec2* text_size_if_known = NULL, const ImVec2& align = ImVec2(0.f, 0.f), const ImRect* clip_rect = NULL);

    void radial_gradient(ImDrawList* draw_list, const ImVec2& center, float radius, ImU32 col_in, ImU32 col_out);

    void set_linear_color_alpha(ImDrawList* draw_list, int vert_start_idx, int vert_end_idx, ImVec2 gradient_p0, ImVec2 gradient_p1, ImU32 col0, ImU32 col1);

    void line(ImDrawList* draw_list, const ImVec2& p1, const ImVec2& p2, ImU32 col, float thickness = 1.0f);

    void rect(ImDrawList* draw_list, const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding = 0.0f, ImDrawFlags flags = 0, float thickness = 1.0f);

    void rect_filled(ImDrawList* draw_list, const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding = 0.0f, ImDrawFlags flags = 0);

    void rect_filled_multi_color(ImDrawList* draw, const ImVec2& p_min, const ImVec2& p_max, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left, float rounding = 0.f, ImDrawFlags flags = 0);

    void fade_rect_filled(ImDrawList* draw, const ImVec2& pos_min, const ImVec2& pos_max, ImU32 col_one, ImU32 col_two, fade_direction direction, float rounding = 0.f, ImDrawFlags flags = 0);

    void shadow_rect(ImDrawList* draw_list, const ImVec2& obj_min, const ImVec2& obj_max, ImU32 shadow_col, float shadow_thickness, const ImVec2& shadow_offset, ImDrawFlags flags = 0, float obj_rounding = 0.0f);

    void circle(ImDrawList* draw_list, const ImVec2& center, float radius, ImU32 col, int num_segments = 0, float thickness = 1.0f);

    void circle_filled(ImDrawList* draw_list, const ImVec2& center, float radius, ImU32 col, int num_segments = 0);

    void shadow_circle(ImDrawList* draw_list, const ImVec2& obj_center, float obj_radius, ImU32 shadow_col, float shadow_thickness, const ImVec2& shadow_offset, ImDrawFlags flags = 0, int obj_num_segments = 12);

    void triangle(ImDrawList* draw_list, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col, float thickness = 1.f);

    void triangle_filled(ImDrawList* draw_list, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col);

    void image(ImDrawList* draw_list, ImTextureID user_texture_id, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& uv_min = ImVec2(0, 0), const ImVec2& uv_max = ImVec2(1, 1), ImU32 col = IM_COL32_WHITE);

    void image_rounded(ImDrawList* draw_list, ImTextureID user_texture_id, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& uv_min = ImVec2(0, 0), const ImVec2& uv_max = ImVec2(1, 1), ImU32 col = IM_COL32_WHITE, float rounding = 1.f, ImDrawFlags flags = 0);

    void shadow_convex_poly(ImDrawList* draw_list, const ImVec2* points, int points_count, ImU32 shadow_col, float shadow_thickness, const ImVec2& shadow_offset, ImDrawFlags flags = 0);

    void shadow_ngon(ImDrawList* draw_list, const ImVec2& obj_center, float obj_radius, ImU32 shadow_col, float shadow_thickness, const ImVec2& shadow_offset, ImDrawFlags flags, int obj_num_segments);

    void rotate_start(ImDrawList* draw_list);

    void rotate_end(ImDrawList* draw_list, float rad, ImVec2 center = ImVec2(0, 0));

    void push_clip_rect(ImDrawList* draw_list, const ImVec2& clip_rect_min, const ImVec2& clip_rect_max, bool intersect_with_current_clip_rect = false);

    void pop_clip_rect(ImDrawList* draw_list);
};

inline std::unique_ptr<c_draw> draw = std::make_unique<c_draw>();

enum notify_type
{
    success = 0,
    warning = 1,
    error = 2
};

struct notify_state
{
    int notify_id;
    std::string_view text;
    notify_type notify_type = success;

    ImVec2 window_size = { 0, 0 };
    float notify_alpha = 0.f;
    bool active_notify = true;
    float notify_timer = 0.f;
    float notify_pos = 0.f;
};

class c_notify
{
public:
    void setup_notify();

    void add_notify(std::string_view text, notify_type type);

private:
    ImVec2 render_notify(int cur_notify_value, float notify_alpha, float notify_percentage, float notify_pos, std::string_view text, notify_type type);

    float notify_time = 15.f;
    int notify_count = 0;

    float notify_spacing{ 20.f };
    ImVec2 notify_padding{ 20.f, 20.f };

    std::vector<notify_state> notifications;

};

inline std::unique_ptr<c_notify> notify = std::make_unique<c_notify>();

inline std::string show_names[] = {
    "Hide",
    "Show"
};

inline std::string mode_names[] = {
    "Toggle",
    "Hold"
};

inline std::string key_names[] =
{
    "None",
    "Mouse 1",
    "Mouse 2",
    "CN",
    "Mouse 3",
    "Mouse 4",
    "Mouse 5",
    "-",
    "Back",
    "Tab",
    "-",
    "-",
    "CLR",
    "Enter",
    "-",
    "-",
    "Shift",
    "CTL",
    "Menu",
    "Pause",
    "Caps Lock",
    "KAN",
    "-",
    "JUN",
    "FIN",
    "KAN",
    "-",
    "Escape",
    "CON",
    "NCO",
    "ACC",
    "MAD",
    "Space",
    "PGU",
    "PGD",
    "End",
    "Home",
    "Left",
    "Up",
    "Right",
    "Down",
    "SEL",
    "PRI",
    "EXE",
    "PRI",
    "INS",
    "Delete",
    "HEL",
    "0",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "A",
    "B",
    "C",
    "D",
    "E",
    "F",
    "G",
    "H",
    "I",
    "J",
    "K",
    "L",
    "M",
    "N",
    "O",
    "P",
    "Q",
    "R",
    "S",
    "T",
    "U",
    "V",
    "W",
    "X",
    "Y",
    "Z",
    "WIN",
    "WIN",
    "APP",
    "-",
    "SLE",
    "Numpad 0",
    "Numpad 1",
    "Numpad 2",
    "Numpad 3",
    "Numpad 4",
    "Numpad 5",
    "Numpad 6",
    "Numpad 7",
    "Numpad 8",
    "Numpad 9",
    "MUL",
    "ADD",
    "SEP",
    "MIN",
    "Delete",
    "DIV",
    "F1",
    "F2",
    "F3",
    "F4",
    "F5",
    "F6",
    "F7",
    "F8",
    "F9",
    "F10",
    "F11",
    "F12",
    "F13",
    "F14",
    "F15",
    "F16",
    "F17",
    "F18",
    "F19",
    "F20",
    "F21",
    "F22",
    "F23",
    "F24",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "NUM",
    "SCR",
    "EQU",
    "MAS",
    "TOY",
    "OYA",
    "OYA",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "Shift",
    "Shift",
    "Ctrl",
    "Ctrl",
    "Alt",
    "Alt"
};
