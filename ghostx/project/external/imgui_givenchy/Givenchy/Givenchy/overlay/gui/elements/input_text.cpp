#include "../settings/functions.h"

bool c_widgets::input_text(std::string_view label, char* buf, size_t buf_size, ImGuiInputTextFlags flags) {

	const ImVec2 pos = GetCurrentWindow()->DC.CursorPos;
	ImDrawList* drawlist = GetWindowDrawList();

	gui->push_color(ImGuiCol_FrameBg, draw->get_clr(clr->main.dark_4));
	gui->push_color(ImGuiCol_FrameBgActive, draw->get_clr(clr->main.dark_4));
	gui->push_color(ImGuiCol_FrameBgHovered, draw->get_clr(clr->main.dark_4));
	gui->push_color(ImGuiCol_Text, draw->get_clr(clr->text.active));
	gui->push_color(ImGuiCol_TextSelectedBg, draw->get_clr(clr->main.dark_7));
	gui->push_var(ImGuiStyleVar_FramePadding, SCALE(elements->input_text.padding));
	gui->push_var(ImGuiStyleVar_FrameRounding, SCALE(elements->rounding));
	gui->push_font(var->font.medium10);

	PushID(label.data());
	bool input = InputText("##inputtext", buf, buf_size, flags);
	draw->rect(drawlist, pos, pos + GetItemRectSize(), draw->get_clr(clr->main.border), SCALE(elements->rounding), ImDrawFlags_RoundCornersAll);
	PopID();

	gui->pop_color(5);
	gui->pop_var(2);
	gui->pop_font();

	return input;
}

bool c_widgets::input_text_icon(std::string_view label, std::string_view icon, char* buf, size_t buf_size, ImGuiInputTextFlags flags) {

	const ImVec2 pos = GetCurrentWindow()->DC.CursorPos;
	ImDrawList* drawlist = GetWindowDrawList();
	const ImVec2 icon_size = gui->text_size(var->font.icon9, icon.data());
	const ImVec2 padding = SCALE(elements->input_text.padding);

	bool input = widgets->input_text(label, buf, buf_size, flags);
	const ImVec2 size = GetItemRectSize();
	const ImRect total_bb(pos, pos + size);
	const ImRect icon_bb(ImVec2(total_bb.Max.x - padding.x - icon_size.x, total_bb.GetCenter().y - icon_size.y / 2), ImVec2(total_bb.Max.x - padding.x, total_bb.GetCenter().y + icon_size.y / 2));
	draw->text_clipped(drawlist, var->font.icon9, icon_bb.Min, icon_bb.Max, draw->get_clr(clr->text.inactive), icon.data());

	return input;
}