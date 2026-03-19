#pragma once
#include <string>
#include <imgui.h>

class c_elements
{
public:

	float rounding = 5.f;

	struct
	{
		ImVec2 size { 720.f, 50.f };
		ImVec2 name_size = { 258.f, 50.f };
		ImVec2 tabs_size = { 462.f, 50.f };

	} titlebar;

	struct
	{
		ImVec2 size { 720.f, 50.f };
		float circle_size = 2.f;

	} tab_info;

	struct
	{
		ImVec2 size { 470.f, 50.f };

	} tab_bar;

	struct
	{
		ImVec2 size { 55.f, 50.f };
		ImVec2 padding{ 15.f, 15.f };

	} tab_button;

	struct
	{
		ImVec2 size{ 194.f, 30.f };
		ImVec2 padding{ 10.f, 0.f };

	} subtab_button;

	struct
	{
		ImVec2 size{ 720.f, 344.f };

	} features;

	struct
	{
		ImVec2 size{ 337.5f, 0.f };
		float invisible_button_y = 15.f;

		float with_scrollbar_x = 327.5f;
		float without_scrollbar_x = 337.5f;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

	} child;

	struct
	{
		ImVec2 size { 327.5f, 30.f };
		ImVec2 button_size { 14.f, 14.f };
		ImVec2 padding{ 10.f, 9.5f };
		ImVec2 window_padding{ 8.f, 8.f };
		float spacing = 5.f;
		float circle_size = 4.f;
		float colorpicker_circle_size = 5.f;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize;

	} checkbox;

	struct {

		ImVec2 button_size { 327.5f, 25.f };
		ImVec2 padding{ 10.f, 6.5f };

	} button;

	struct {

		ImVec2 button_size{ 159.0f, 25.f };
		ImVec2 padding{ 10.f, 6.5f };

	} k_button;

	struct {

		ImVec2 size{ 159.0f, 0.f };

	} kh_window;

	struct
	{
		ImVec2 size{ 327.5f, 45.f };
		ImVec2 button_size{ 307.5f, 5.f };
		ImVec2 padding{ 10.f, 9.5f };
		float spacing = 5.f;
		float circle_size = 4.f;
		float circle_active_size = 6.f;

	} slider;

	struct
	{
		ImVec2 size{ 327.5f, 65.f };
		ImVec2 button_size{ 327.5f, 25.f };
		ImVec2 padding{ 10.f, 9.5f };
		float spacing = 5.f;


	} combo;

	struct
	{
		ImVec2 size{ 307.5f, 25.f };
		ImVec2 padding{ 10.f, 6.5f };


	} combo_button;

	struct
	{
		float circle_size_active = 6.f;
		float circle_size = 4.f;
		float input_width_1 = 159.f;
		float input_width_2 = 145.f;
	} color_picker;

	struct
	{
		ImVec2 padding { 10.f, 7.f };
	} input_text;

	struct
	{
		std::string_view max_label = "Features";
	} keybinds_list;


};

inline std::unique_ptr<c_elements> elements = std::make_unique<c_elements>();
