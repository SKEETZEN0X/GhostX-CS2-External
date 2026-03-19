#pragma once
#include "../../imgui/imgui.h"
#include <memory>
#include <vector>

class c_colors
{
public:

	struct
	{
		ImColor main{ 255, 215, 0, 255 }; // Or avec opacité maximale
		ImColor alpha{ 216, 192, 111, 38 };
		ImColor invisble{ 216, 192, 111, 0 };

	} accent;

	struct {

		ImColor light_2{ 255, 255, 255, 255 };
		ImColor light_1{ 52, 52, 74, 255 };

		ImColor border{ 30, 29, 37, 255 };
		ImColor border_0{ 30, 29, 37, 0 };

		ImColor dark_invisble{ 0, 0, 0, 0 };
		ImColor dark_1{ 20, 20, 24, 255 };
		ImColor dark_1_0{ 20, 20, 24, 0 };
		ImColor dark_2{ 21, 20, 24, 255 };
		ImColor dark_3_255{ 23, 22, 27, 255 };
		ImColor dark_3_0{ 23, 22, 27, 0 };
		ImColor dark_3_155{ 23, 22, 27, 155 };
		ImColor dark_4{ 27, 26, 33, 255 };
		ImColor dark_4_0{ 27, 26, 33, 0 };
		ImColor dark_5{ 25, 24, 29, 255 };
		ImColor dark_6{ 29, 29, 38, 255 };
		ImColor dark_7{ 35, 34, 42 };

	} main;

	struct
	{
		ImColor active { 255, 255, 255 };
		ImColor hover{ 255, 255, 255, 155 };
		ImColor inactive{ 114, 113, 138, 255 };

	} text;

};

inline std::unique_ptr<c_colors> clr = std::make_unique<c_colors>();
