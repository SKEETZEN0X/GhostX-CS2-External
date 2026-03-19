#include "../settings/functions.h"

bool c_widgets::setup_child_anim() {

	bool start_anim = GetScrollMaxY() > 0;
	gui->easing(elements->child.size.x, start_anim ? elements->child.with_scrollbar_x : elements->child.without_scrollbar_x, var->anim.speed, dynamic_easing);

	return start_anim;
}

bool c_widgets::child(std::string_view label) {

	struct anim_state {
		float arrow_rad = 90.f;
		bool open_child = true;
		float get_size_y, size_y, child_anim = 0.f;
	};

	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	const ImGuiID id = window->GetID(label.data());
	anim_state* state = gui->anim_container(&state, id);
	ImDrawList* drawlist = GetWindowDrawList();

	const ImVec2 pos = window->DC.CursorPos;
	const ImVec2 label_size = gui->text_size(var->font.medium11, label.data());
	const ImVec2 arrow_size = gui->text_size(var->font.icon6, var->icons.arrow.data());
	ImRect label_bb(pos, pos + label_size);
	ImRect arrow_bb(ImVec2(pos.x + SCALE(elements->child.size.x) - arrow_size.x, label_bb.GetCenter().y - arrow_size.y / 2), ImVec2(pos.x + SCALE(elements->child.size.x), label_bb.GetCenter().y + arrow_size.y / 2));

	draw->text_clipped(drawlist, var->font.medium11, label_bb.Min, label_bb.Max, draw->get_clr(clr->text.inactive), label.data());
	
	gui->easing(state->arrow_rad, state->open_child ? 90.f : 270.f, var->anim.speed, dynamic_easing);

	draw->rotate_start(drawlist);
	draw->text_clipped(drawlist, var->font.icon6, arrow_bb.Min, arrow_bb.Max, draw->get_clr(clr->text.inactive), var->icons.arrow.data());
	draw->rotate_end(drawlist, state->arrow_rad, arrow_bb.GetCenter());

	gui->easing(state->size_y, state->open_child ? state->get_size_y : 0.f, var->anim.speed, dynamic_easing);
	gui->easing(state->child_anim, state->open_child ? 1.f : 0.f, var->anim.speed, dynamic_easing);

	gui->push_var(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	gui->push_var(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

	if (widgets->invisible_button(label, ImVec2(SCALE(elements->child.size.x), label_size.y + SCALE(elements->child.invisible_button_y)))) {
		state->open_child = !state->open_child;
	}

	if (state->child_anim > 0.01f) {

		gui->push_var(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		gui->push_var(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		gui->push_var(ImGuiStyleVar_Alpha, var->anim.features_alpha < 0.99f ? var->anim.features_alpha : state->child_anim);

		gui->set_pos(gui->get_pos().y - 5 + 5 * state->child_anim, pos_y);
		bool child = gui->begin_def_child(label, ImVec2(SCALE(elements->child.size.x), state->size_y), 0, elements->child.window_flags);

		const ImVec2 c_pos = GetWindowPos();
		const ImVec2 c_size = GetWindowSize();

		state->get_size_y = GetCurrentWindow()->ContentSize.y;

		if (state->child_anim > 0.01f) {
			draw->rect_filled(drawlist, c_pos, c_pos + c_size, draw->get_clr(clr->main.dark_3_255), SCALE(elements->rounding), ImDrawFlags_RoundCornersAll);
			draw->rect(drawlist, c_pos, c_pos + c_size, draw->get_clr(clr->main.border), SCALE(elements->rounding), ImDrawFlags_RoundCornersAll);
			const float bar_w = SCALE(2.5f);
			const float bar_pad = SCALE(6.f);
			ImU32 accent = draw->get_clr(clr->accent.main);
			drawlist->AddRectFilled(
				ImVec2(c_pos.x, c_pos.y + bar_pad),
				ImVec2(c_pos.x + bar_w, c_pos.y + c_size.y - bar_pad),
				accent, SCALE(1.f));
		}

		gui->pop_var(2);
		return true;
	}
	else {
		std::string child_label2 = std::string(label.data()) + "child_inv2";
		widgets->invisible_button(child_label2.data(), ImVec2(SCALE(elements->child.size.x), SCALE(15.f)));
		gui->pop_var(2);
	}

	return false;

}

bool c_widgets::end_child() {

	gui->pop_var(3);
	gui->end_def_child();

	return true;
}
