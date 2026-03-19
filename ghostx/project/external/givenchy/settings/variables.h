#pragma once
#include <string>
#include <vector>
#include <imgui.h>

class c_variables
{
public:
	struct
	{
		ImGuiWindowFlags window_flags			{ ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground };
		std::string window_name					{ "GhostX" };

		ImVec2 window_padding					{ 0, 0 };
		ImVec2 item_spacing						{ 0, 0 };
		ImVec2 window_size						{ 820, 560 };

		float window_border_size				= 0.f;
		float window_shadow_size				= 0.f;
		float scrollbar_size					= 10.f;

	} window;

	struct {

		float speed = 12.f;
		float features_alpha = 1.f;

	} anim;

	struct {

		int active = 0;
		int current = 0;
		std::string_view tab_name = "Aimbot";
		std::string_view subtab_name = "Aimbot";

	} tab;

	struct
	{
		float dpi								= 1.f;
		float stored_dpi						= 100.f;
		bool dpi_changed						= false;

	} gui;

	struct
	{
		ImFont* medium15 = nullptr;
		ImFont* medium12 = nullptr;
		ImFont* medium11 = nullptr;
		ImFont* medium10 = nullptr;
		ImFont* medium9 = nullptr;

		ImFont* icon15 = nullptr;
		ImFont* icon13 = nullptr;
		ImFont* icon12 = nullptr;
		ImFont* icon10 = nullptr;
		ImFont* icon9 = nullptr;
		ImFont* icon6 = nullptr;
	} font;

	struct {
		std::string_view crosshair = "A";
		std::string_view gear = "B";
		std::string_view arrow = "C";
		std::string_view check = "D";
		std::string_view pencil = "E";
		std::string_view trash = "F";
	} icons;
};

inline std::unique_ptr<c_variables> var = std::make_unique<c_variables>();
