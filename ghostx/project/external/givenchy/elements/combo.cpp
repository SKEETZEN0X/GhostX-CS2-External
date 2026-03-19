#include "../settings/functions.h"

struct anim_state {
	bool open_description = false;
	ImVec2 description_size = ImVec2(0.f, 0.f);
	float arrow_rad = 90.f;
	ImVec4 arrow_color = clr->text.inactive;

	float arrow_rad2 = 90.f;
	ImVec4 arrow_color2 = clr->text.inactive;

	ImVec2 vars_begin_pos, vars_begin_size = ImVec2(0.f, 0.f);
	bool vars_is_active, vars_open_begin = false;
	float vars_begin_anim = 0.f;

	ImVec2 begin_pos;
	bool open_begin = false;
	float begin_anim = 0.f;

	int current_point = 0;
	bool open_kh1_begin, open_kh2_begin = false;
	float kh1_anim, kh2_anim = 0.f;

	int bind_value = 0;
	std::vector<bool> multi_bind_value = { };
	bool reset_keybinds = false;
	int key, mode = 0;
};

std::vector<ImVec2> c_widgets::render_combo(const ImVec2 pos, std::string_view label, std::string_view value, std::string_view description, bool keybind, bool is_keybind_window) {

	ImGuiContext& g = *GImGui;
	const ImGuiID id = GetCurrentWindow()->GetID(label.data());
	anim_state* state = gui->anim_container(&state, id);
	const ImVec2 mouse_pos = GetMousePos();

	const ImVec2 value_size = gui->text_size(var->font.medium9, value.data());
	const ImVec2 label_size = gui->text_size(var->font.medium10, label.data());
	const ImVec2 keybind_size = gui->text_size(var->font.icon10, var->icons.gear.data());
	ImVec2 padding = SCALE(is_keybind_window ? ImVec2(0.f, 0.f) : elements->combo.padding);
	ImVec2 inner_padding = SCALE(elements->combo.padding);
	std::string wrap_description = gui->wrap_text_to_width(var->font.medium10, description.data(), SCALE(elements->child.size.x) - padding.x * 2);
	const ImVec2 open_description_size = gui->text_size(var->font.medium10, wrap_description.data());
	bool is_desc = open_description_size.x > 0.1f;
	ImVec2 description_size = is_desc ? (state->open_description ? open_description_size : label_size) : ImVec2(0.f, 0.f);
	bool is_large_desc = open_description_size.y > label_size.y;
	gui->easing(state->description_size.y, description_size.y, var->anim.speed, dynamic_easing);
	const ImVec2 arrow_size = gui->text_size(var->font.icon6, var->icons.arrow.data());

	ImDrawList* drawlist = GetWindowDrawList();

	const ImVec2 size = ImVec2(SCALE(is_keybind_window ? elements->kh_window.size.x : elements->child.size.x), padding.y * (is_keybind_window ? 1 : 2) + label_size.y + SCALE((elements->combo.spacing * 2) + elements->combo.button_size.y) + (is_desc ? SCALE(elements->combo.spacing) + state->description_size.y : 0));
	const ImRect total_bb(pos, pos + size);
	const ImRect label_bb(total_bb.Min + padding, total_bb.Min + padding + label_size);
	const ImRect desc_bb(ImVec2(label_bb.Min.x, label_bb.Max.y + SCALE(elements->combo.spacing)), ImVec2(label_bb.Min.x + SCALE(elements->child.size.x) - padding.x * 2, label_bb.Max.y + SCALE(elements->combo.spacing) + state->description_size.y));
	const ImVec2 button_size = SCALE(elements->child.size.x, elements->combo.button_size.y) - ImVec2(padding.x, 0);
	const ImRect button_bb(ImVec2(label_bb.Min.x, total_bb.Max.y - padding.y - button_size.y), ImVec2(total_bb.Max.x - padding.x, total_bb.Max.y - padding.y));
	const ImRect value_bb(ImVec2(button_bb.Min.x + inner_padding.x, button_bb.GetCenter().y - value_size.y / 2), ImVec2(button_bb.Max.x - inner_padding.x - arrow_size.x, button_bb.GetCenter().y + value_size.y / 2));
	const ImRect arrow_bb(ImVec2(button_bb.Max.x - inner_padding.x - arrow_size.x, button_bb.GetCenter().y - arrow_size.y / 2), ImVec2(button_bb.Max.x - inner_padding.x, button_bb.GetCenter().y + arrow_size.y / 2));
	const ImRect arrow_bb2(ImVec2(label_bb.Min.x + size.x - padding.x * 2 - arrow_size.x, desc_bb.Min.y + SCALE(3.f)), ImVec2(label_bb.Min.x + size.x - padding.x * 2, desc_bb.Min.y + arrow_size.y + SCALE(3.f)));
	const ImRect keybind_bb = ImRect(ImVec2(label_bb.Max.x + padding.x, label_bb.GetCenter().y - keybind_size.y / 2), ImVec2(label_bb.Max.x + padding.x + keybind_size.x, label_bb.GetCenter().y + keybind_size.y / 2 + SCALE(1.f)));

	gui->easing(state->arrow_rad, state->open_description ? 270.f : 90.f, var->anim.speed, dynamic_easing);
	gui->easing(state->arrow_rad2, state->vars_open_begin ? 270.f : 90.f, var->anim.speed, dynamic_easing);
	gui->easing(state->arrow_color, state->open_description ? clr->text.active.Value : clr->text.inactive.Value, var->anim.speed, dynamic_easing);
	gui->easing(state->arrow_color2, state->vars_open_begin ? clr->text.active.Value : clr->text.inactive.Value, var->anim.speed, dynamic_easing);
	draw->rect_filled(drawlist, button_bb.Min, button_bb.Max, draw->get_clr(clr->main.dark_4), SCALE(elements->rounding), ImDrawFlags_RoundCornersAll);
	draw->rect(drawlist, button_bb.Min, button_bb.Max, draw->get_clr(clr->main.border), SCALE(elements->rounding), ImDrawFlags_RoundCornersAll);

	draw->text_clipped(drawlist, var->font.medium10, label_bb.Min, label_bb.Max, draw->get_clr(clr->text.inactive), label.data());

	if ( widgets->center_combo )
	{
		const ImVec2 icon_sz = gui->text_size( var->font.icon9, var->icons.crosshair.data() );
		const float gap = SCALE( 6.f );
		const float total_w = icon_sz.x + gap + value_size.x;
		const float start_x = button_bb.Min.x + ( button_size.x - total_w ) * 0.5f;
		const ImRect icon_rc(
			ImVec2( start_x, button_bb.GetCenter().y - icon_sz.y * 0.5f ),
			ImVec2( start_x + icon_sz.x, button_bb.GetCenter().y + icon_sz.y * 0.5f ) );
		draw->text_clipped( drawlist, var->font.icon9, icon_rc.Min, icon_rc.Max, draw->get_clr( clr->accent.main ), var->icons.crosshair.data() );
		const ImRect val_rc(
			ImVec2( start_x + icon_sz.x + gap, button_bb.GetCenter().y - value_size.y * 0.5f ),
			ImVec2( start_x + total_w, button_bb.GetCenter().y + value_size.y * 0.5f ) );
		draw->text_clipped( drawlist, var->font.medium9, val_rc.Min, val_rc.Max, draw->get_clr( clr->text.active ), value.data() );
	}
	else
	{
		draw->text_clipped(drawlist, var->font.medium9, value_bb.Min, value_bb.Max, draw->get_clr(clr->text.active), value.data());
	}
	if (keybind)
		draw->text_clipped(drawlist, var->font.icon10, keybind_bb.Min, keybind_bb.Max, draw->get_clr(clr->text.inactive), var->icons.gear.data());
	draw->rotate_start(drawlist);
	draw->text_clipped(drawlist, var->font.icon6, arrow_bb.Min, arrow_bb.Max, draw->get_clr(state->arrow_color2), var->icons.arrow.data());
	draw->rotate_end(drawlist, state->arrow_rad2, arrow_bb.GetCenter());

	if (is_desc) {
		if (is_large_desc) {
			draw->rotate_start(drawlist);
			draw->text_clipped(drawlist, var->font.icon6, arrow_bb2.Min, arrow_bb2.Max, draw->get_clr(state->arrow_color), var->icons.arrow.data());
			draw->rotate_end(drawlist, state->arrow_rad, arrow_bb2.GetCenter());
		}
		draw->text_clipped(drawlist, var->font.medium10, desc_bb.Min, desc_bb.Max, draw->get_clr(clr->text.inactive), wrap_description.data());
		if (desc_bb.Contains(mouse_pos) && IsMouseClicked(0) && is_large_desc && IsWindowHovered())
			state->open_description = !state->open_description;
	}

	if (!is_keybind_window)
		draw->line(drawlist, ImVec2(total_bb.Min.x + padding.x, total_bb.Max.y), ImVec2(total_bb.Max.x - padding.x, total_bb.Max.y), draw->get_clr(clr->main.border));

	return { size, button_bb.Min, button_bb.Max, keybind_bb.Min, keybind_bb.Max };
}

bool c_widgets::combo_button(std::string_view label, bool selected, bool is_keybind_window) {

	struct anim_state2 {
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

	anim_state2* state = gui->anim_container(&state, id);

	state->selected = selected;

	const ImVec2 pos = window->DC.CursorPos;
	const ImVec2 label_size = gui->text_size(var->font.medium9, label.data());
	const ImVec2 check_size = gui->text_size(var->font.icon9, var->icons.check.data());
	const ImVec2 padding = SCALE(elements->combo_button.padding); 
	const ImVec2 size = ImVec2(SCALE(is_keybind_window ? elements->kh_window.size.x : elements->child.size.x - padding.x * 2), SCALE(elements->combo_button.size.y));

	ImDrawList* drawlist = GetWindowDrawList();

	const ImRect bb(pos, pos + size);

	ImRect label_pos;
	if ( widgets->center_combo )
	{
		const ImVec2 icon_sz = gui->text_size( var->font.icon9, var->icons.crosshair.data() );
		const float gap = SCALE( 6.f );
		const float total_text_w = icon_sz.x + gap + label_size.x;
		const float start_x = bb.Min.x + ( size.x - total_text_w ) * 0.5f;
		const ImRect icon_rect(
			ImVec2( start_x, bb.GetCenter().y - icon_sz.y * 0.5f ),
			ImVec2( start_x + icon_sz.x, bb.GetCenter().y + icon_sz.y * 0.5f ) );
		draw->text_clipped( drawlist, var->font.icon9, icon_rect.Min, icon_rect.Max, draw->get_clr( clr->accent.main ), var->icons.crosshair.data() );
		label_pos = ImRect(
			ImVec2( start_x + icon_sz.x + gap, bb.GetCenter().y - label_size.y * 0.5f ),
			ImVec2( start_x + total_text_w, bb.GetCenter().y + label_size.y * 0.5f ) );
	}
	else
	{
		label_pos = ImRect( bb.Min + ImVec2( padding.x, size.y / 2 - label_size.y / 2 ), bb.Min + ImVec2( padding.x + label_size.x, size.y / 2 + label_size.y / 2 ) );
	}
	const ImRect check_pos(bb.Max - ImVec2(padding.x + check_size.x - SCALE(1.f), size.y / 2 + check_size.y / 2), bb.Max - ImVec2(padding.x - SCALE(1.f), size.y / 2 - check_size.y / 2));

	ItemSize(bb, style.FramePadding.y);

	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held);

	if (!ItemAdd(bb, id)) {
		state->background = state->selected ? clr->main.dark_4 : clr->main.dark_3_255;
		state->text_color = state->selected ? clr->text.active : clr->text.inactive;
		state->icon_color = state->selected ? clr->accent.main.Value : clr->accent.invisble.Value;
		return false;
	}
	else {
		gui->easing(state->background, state->selected ? clr->main.dark_4.Value : hovered ? clr->main.dark_5.Value : clr->main.dark_3_255.Value, var->anim.speed, dynamic_easing);
		gui->easing(state->text_color, state->selected ? clr->text.active.Value : hovered ? clr->text.hover.Value : clr->text.inactive.Value, var->anim.speed, dynamic_easing);
		gui->easing(state->icon_color, state->selected ? clr->accent.main.Value : clr->accent.invisble.Value, var->anim.speed, dynamic_easing);
	}

	float rounding = gui->is_last_item(1, pos) ? elements->rounding : gui->is_first_item(1, pos) ? elements->rounding : 0.f;
	ImDrawFlags flags = gui->is_last_item(1, pos) ? ImDrawFlags_RoundCornersBottom : gui->is_first_item(1, pos) ? ImDrawFlags_RoundCornersTop : 0;
	draw->rect_filled(drawlist, bb.Min, bb.Max, draw->get_clr(state->background), SCALE(rounding), flags);
	draw->text_clipped(drawlist, var->font.medium9, label_pos.Min, label_pos.Max, draw->get_clr(state->text_color), label.data());
	draw->text_clipped(drawlist, var->font.icon9, check_pos.Min, check_pos.Max, draw->get_clr(state->icon_color), var->icons.check.data());

	return pressed;
}

bool c_widgets::combo(std::string_view label, int* value, std::vector<std::string> variebles, std::string_view description, bool keybind, bool is_keybind_window) {
	
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label.data());

	anim_state* state = gui->anim_container(&state, id);

	const ImVec2 pos = window->DC.CursorPos;
	const ImVec2 mouse_pos = GetMousePos();

	std::vector<ImVec2> begin_combo = render_combo(pos, label, variebles[*value], description, keybind, is_keybind_window);

	const ImRect total_bb(pos, pos + begin_combo[0]);
	const ImRect button_bb(begin_combo[1], begin_combo[2]);
	const ImRect keybind_bb(begin_combo[3], begin_combo[4]);
	const ImVec2 padding = SCALE(elements->combo.padding);

	ItemSize(total_bb, style.FramePadding.y);
	if (!ItemAdd(total_bb, id))
		return false;

	bool hovered, held;
	bool pressed = ButtonBehavior(button_bb, id, &hovered, &held);

	gui->easing(state->vars_begin_anim, state->vars_open_begin ? 1.f : 0.f, var->anim.speed, dynamic_easing);
	gui->easing(state->begin_anim, state->open_begin ? 1.f : 0.f, var->anim.speed, dynamic_easing);

	if (pressed && !state->vars_open_begin)
		state->vars_open_begin = true;

	if (state->vars_begin_anim > 0.01f) {

		state->vars_begin_pos = pos + ImVec2(is_keybind_window ? 0.f : padding.x, is_keybind_window ? (total_bb.Max.y - pos.y) + SCALE(7.f) : (total_bb.Max.y - pos.y) - SCALE(2.f));
		state->vars_begin_size = ImVec2(SCALE(is_keybind_window ? elements->kh_window.size.x : elements->child.size.x - padding.x * 2), (variebles.size() * SCALE(elements->combo_button.size.y)) * state->vars_begin_anim);

		gui->set_next_window_pos(state->vars_begin_pos);
		gui->set_next_window_size(state->vars_begin_size);
		gui->push_var(ImGuiStyleVar_Alpha, state->vars_begin_anim);
		gui->push_var(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		gui->push_var(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		std::string main_begin_name = std::string(label.data()) + "keybind_window";
		std::string keybinds_begin_name = std::string(label.data()) + "keybinds_window";
		std::string hotkeys_begin_name = std::string(label.data()) + "hotkeys_window";
		std::string dropdown_begin_name = std::string(label.data()) + "dropdown_window";

		gui->push_color(ImGuiCol_WindowBg, draw->get_clr(clr->main.dark_3_255));
		if (gui->begin(dropdown_begin_name, nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {

			for (int i = 0; i < variebles.size(); i++) {
				const bool selected = (*value == i);
				if (widgets->combo_button(variebles[i], selected, is_keybind_window)) {
					*value = i;
					state->vars_open_begin = false;
				}
			}

			gui->pop_var(2);

			draw->rect(GetWindowDrawList(), GetWindowPos(), GetWindowPos() + GetWindowSize(), draw->get_clr(clr->main.border), SCALE(elements->rounding), ImDrawFlags_RoundCornersAll);

			bool dropdown_hovered = g.HoveredWindow && strstr(g.HoveredWindow->Name, dropdown_begin_name.data());
			if (!dropdown_hovered && IsMouseClicked(0)) {
				if (!hovered) {
					state->vars_open_begin = false;
				}
			}

			gui->end();
		}

		gui->pop_color();
		gui->pop_var();
	}

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
		std::string dropdown_begin_name = std::string(label.data()) + "dropdown_window";

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
						if (key_names[state->key] == "None") {
							notify->add_notify("Keybind is not found", success);
						}
						else {
							keybinds.push_back({ label, state->key, state->mode, true, std::to_string(state->bind_value) });
							state->bind_value = 0;
							state->key = 0;
							state->mode = 0;
							notify->add_notify("The keybind has been added", success);
						}
					}
					widgets->mode_button("Mode", &state->mode);
					widgets->combo("Value", &state->bind_value, variebles, "", false, true);

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
			bool dropdown_hovered = g.HoveredWindow && strstr(g.HoveredWindow->Name, "Valuedropdown_window");

			if (!keybind_bb.Contains(mouse_pos) && !main_hovered && !keybinds_hovered && !hotkeys_hovered && !dropdown_hovered && IsMouseClicked(0)) {
				state->open_kh1_begin = false;
				state->open_kh2_begin = false;
				state->open_begin = false;
			}

			gui->end();
		}

		gui->pop_var(4);
	}

	return pressed;
}

bool c_widgets::multicombo(std::string_view label, std::vector<bool>* values, std::vector<std::string> variebles, std::string_view description, bool keybind, bool is_keybind_window) {

	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label.data());

	anim_state* state = gui->anim_container(&state, id);

	const ImVec2 pos = window->DC.CursorPos;
	const ImVec2 mouse_pos = GetMousePos();

	std::string value = "None";

	for (int i = 0; i < variebles.size(); i++) {
		if (values->at(i)) {
			if (value == "None") {
				value = variebles[i];
			}
			else {
				value += (", ") + variebles[i];
			}
		}
	}

	std::vector<ImVec2> begin_combo = render_combo(pos, label, value, description, keybind, is_keybind_window);

	const ImRect total_bb(pos, pos + begin_combo[0]);
	const ImRect button_bb(begin_combo[1], begin_combo[2]);
	const ImRect keybind_bb(begin_combo[3], begin_combo[4]);
	const ImVec2 padding = SCALE(elements->combo.padding);

	ItemSize(total_bb, style.FramePadding.y);
	if (!ItemAdd(total_bb, id))
		return false;

	bool hovered, held;
	bool pressed = ButtonBehavior(button_bb, id, &hovered, &held);

	gui->easing(state->vars_begin_anim, state->vars_open_begin ? 1.f : 0.f, var->anim.speed, dynamic_easing);
	gui->easing(state->begin_anim, state->open_begin ? 1.f : 0.f, var->anim.speed, dynamic_easing);

	if (pressed && !state->vars_open_begin)
		state->vars_open_begin = true;

	if (state->vars_begin_anim > 0.01f) {

		state->vars_begin_pos = pos + ImVec2(is_keybind_window ? 0.f : padding.x, is_keybind_window ? (total_bb.Max.y - pos.y) + SCALE(7.f) : (total_bb.Max.y - pos.y) - SCALE(2.f));
		state->vars_begin_size = ImVec2(SCALE(is_keybind_window ? elements->kh_window.size.x : elements->child.size.x - padding.x * 2), (variebles.size() * SCALE(elements->combo_button.size.y)) * state->vars_begin_anim);

		gui->set_next_window_pos(state->vars_begin_pos);
		gui->set_next_window_size(state->vars_begin_size);
		gui->push_var(ImGuiStyleVar_Alpha, state->vars_begin_anim);
		gui->push_var(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		gui->push_var(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		std::string dropdown_begin_name = std::string(label.data()) + "dropdown_window";

		gui->push_color(ImGuiCol_WindowBg, draw->get_clr(clr->main.dark_3_255));
		if (gui->begin(dropdown_begin_name, nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {

			for (int i = 0; i < variebles.size(); i++) {
				bool selected = values->at(i);
				if (widgets->combo_button(variebles.at(i), selected, is_keybind_window)) {
					values->at(i) = !selected;
				}
			}

			gui->pop_var(2);

			draw->rect(GetWindowDrawList(), GetWindowPos(), GetWindowPos() + GetWindowSize(), draw->get_clr(clr->main.border), SCALE(elements->rounding), ImDrawFlags_RoundCornersAll);

			bool dropdown_hovered = g.HoveredWindow && strstr(g.HoveredWindow->Name, dropdown_begin_name.data());
			if (!dropdown_hovered && IsMouseClicked(0)) {
				if (!hovered) {
					state->vars_open_begin = false;
				}
			}

			gui->end();
		}

		gui->pop_color();
		gui->pop_var();
	}

	if (keybind_bb.Contains(GetMousePos()) && IsMouseClicked(0) && keybind) {
		state->open_begin = true;
	}

	state->multi_bind_value.resize(variebles.size(), false);

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
		std::string dropdown_begin_name = std::string(label.data()) + "dropdown_window";

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
						std::string multi_bind_value;
						for (size_t i = 0; i < state->multi_bind_value.size(); ++i) {
							multi_bind_value += (state->multi_bind_value[i] ? "1" : "0"); 
							if (i < state->multi_bind_value.size() - 1) {
								multi_bind_value += ", ";
							}
						}

						if (key_names[state->key] == "None") {
							notify->add_notify("Keybind is not found", success);
						}
						else {
							keybinds.push_back({ label, state->key, state->mode, true, multi_bind_value });
							for (int i = 0; i < state->multi_bind_value.size(); i++)
								state->multi_bind_value[i] = false;
							state->key = 0;
							state->mode = 0;
							notify->add_notify("The keybind has been added", success);
						}
					}
					widgets->mode_button("Mode", &state->mode);
					widgets->multicombo("Value", &state->multi_bind_value, variebles, "", false, true);

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
			bool dropdown_hovered = g.HoveredWindow && strstr(g.HoveredWindow->Name, "Valuedropdown_window");

			if (!keybind_bb.Contains(mouse_pos) && !main_hovered && !keybinds_hovered && !hotkeys_hovered && !dropdown_hovered && IsMouseClicked(0)) {
				state->open_kh1_begin = false;
				state->open_kh2_begin = false;
				state->open_begin = false;			
			}

			gui->end();
		}

		gui->pop_var(4);
	}

	value = "None";

	return pressed;
}