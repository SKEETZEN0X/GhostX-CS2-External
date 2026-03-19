#include "settings/functions.h"

bool c_gui::init_static(ImGuiIO& io, ImFontConfig* cfg) {

	var->font.medium15 = io.Fonts->AddFontFromMemoryTTF(font_medium_binary, sizeof(font_medium_binary), SCALE(17.5f), cfg, io.Fonts->GetGlyphRangesCyrillic());
	var->font.medium12 = io.Fonts->AddFontFromMemoryTTF(font_medium_binary, sizeof(font_medium_binary), SCALE(14.5f), cfg, io.Fonts->GetGlyphRangesCyrillic());
	var->font.medium11 = io.Fonts->AddFontFromMemoryTTF(font_medium_binary, sizeof(font_medium_binary), SCALE(13.5f), cfg, io.Fonts->GetGlyphRangesCyrillic());
	var->font.medium10 = io.Fonts->AddFontFromMemoryTTF(font_medium_binary, sizeof(font_medium_binary), SCALE(12.5f), cfg, io.Fonts->GetGlyphRangesCyrillic());
	var->font.medium9 = io.Fonts->AddFontFromMemoryTTF(font_medium_binary, sizeof(font_medium_binary), SCALE(11.5f), cfg, io.Fonts->GetGlyphRangesCyrillic());

	var->font.icon15 = io.Fonts->AddFontFromMemoryTTF(icon_font_binary, sizeof(icon_font_binary), SCALE(15.0f), cfg, io.Fonts->GetGlyphRangesCyrillic());
	var->font.icon13 = io.Fonts->AddFontFromMemoryTTF(icon_font_binary, sizeof(icon_font_binary), SCALE(13.0f), cfg, io.Fonts->GetGlyphRangesCyrillic());
	var->font.icon12 = io.Fonts->AddFontFromMemoryTTF(icon_font_binary, sizeof(icon_font_binary), SCALE(12.0f), cfg, io.Fonts->GetGlyphRangesCyrillic());
	var->font.icon10 = io.Fonts->AddFontFromMemoryTTF(icon_font_binary, sizeof(icon_font_binary), SCALE(10.0f), cfg, io.Fonts->GetGlyphRangesCyrillic());
	var->font.icon9 = io.Fonts->AddFontFromMemoryTTF(icon_font_binary, sizeof(icon_font_binary), SCALE(9.0f), cfg, io.Fonts->GetGlyphRangesCyrillic());
	var->font.icon6 = io.Fonts->AddFontFromMemoryTTF(icon_font_binary, sizeof(icon_font_binary), SCALE(6.0f), cfg, io.Fonts->GetGlyphRangesCyrillic());

	return true;
}

bool c_gui::reinit_static(ImGuiIO& io, ImFontConfig* cfg) {

	if (var->gui.dpi_changed)
	{
		var->gui.dpi = var->gui.stored_dpi / 100.f;

		io.Fonts->Clear();
		{
			var->font.medium15 = io.Fonts->AddFontFromMemoryTTF(font_medium_binary, sizeof(font_medium_binary), SCALE(17.5f), cfg, io.Fonts->GetGlyphRangesCyrillic());
			var->font.medium12 = io.Fonts->AddFontFromMemoryTTF(font_medium_binary, sizeof(font_medium_binary), SCALE(14.5f), cfg, io.Fonts->GetGlyphRangesCyrillic());
			var->font.medium11 = io.Fonts->AddFontFromMemoryTTF(font_medium_binary, sizeof(font_medium_binary), SCALE(13.5f), cfg, io.Fonts->GetGlyphRangesCyrillic());
			var->font.medium10 = io.Fonts->AddFontFromMemoryTTF(font_medium_binary, sizeof(font_medium_binary), SCALE(12.5f), cfg, io.Fonts->GetGlyphRangesCyrillic());
			var->font.medium9 = io.Fonts->AddFontFromMemoryTTF(font_medium_binary, sizeof(font_medium_binary), SCALE(11.5f), cfg, io.Fonts->GetGlyphRangesCyrillic());

			var->font.icon15 = io.Fonts->AddFontFromMemoryTTF(icon_font_binary, sizeof(icon_font_binary), SCALE(15.0f), cfg, io.Fonts->GetGlyphRangesCyrillic());
			var->font.icon13 = io.Fonts->AddFontFromMemoryTTF(icon_font_binary, sizeof(icon_font_binary), SCALE(13.0f), cfg, io.Fonts->GetGlyphRangesCyrillic());
			var->font.icon12 = io.Fonts->AddFontFromMemoryTTF(icon_font_binary, sizeof(icon_font_binary), SCALE(12.0f), cfg, io.Fonts->GetGlyphRangesCyrillic());
			var->font.icon10 = io.Fonts->AddFontFromMemoryTTF(icon_font_binary, sizeof(icon_font_binary), SCALE(10.0f), cfg, io.Fonts->GetGlyphRangesCyrillic());
			var->font.icon9 = io.Fonts->AddFontFromMemoryTTF(icon_font_binary, sizeof(icon_font_binary), SCALE(9.0f), cfg, io.Fonts->GetGlyphRangesCyrillic());
			var->font.icon6 = io.Fonts->AddFontFromMemoryTTF(icon_font_binary, sizeof(icon_font_binary), SCALE(6.0f), cfg, io.Fonts->GetGlyphRangesCyrillic());

			var->gui.dpi_changed = false;
		}
		io.Fonts->Build();

		return true;
	}

	return false;
}