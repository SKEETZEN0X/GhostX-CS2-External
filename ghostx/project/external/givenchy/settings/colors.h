#pragma once
#include <imgui.h>
#include <memory>
#include <vector>

class c_colors
{
public:

	struct
	{
		ImColor main{ 50, 160, 235, 255 };
		ImColor alpha{ 50, 160, 235, 38 };
		ImColor invisble{ 50, 160, 235, 0 };

	} accent;

	struct {

		ImColor light_2{ 255, 255, 255, 255 };
		ImColor light_1{ 44, 52, 70, 255 };

		ImColor border{ 32, 36, 46, 255 };
		ImColor border_0{ 32, 36, 46, 0 };

		ImColor dark_invisble{ 0, 0, 0, 0 };
		ImColor dark_1{ 15, 17, 23, 255 };
		ImColor dark_1_0{ 15, 17, 23, 0 };
		ImColor dark_2{ 18, 20, 28, 255 };
		ImColor dark_3_255{ 22, 25, 33, 255 };
		ImColor dark_3_0{ 22, 25, 33, 0 };
		ImColor dark_3_155{ 22, 25, 33, 155 };
		ImColor dark_4{ 28, 32, 42, 255 };
		ImColor dark_4_0{ 28, 32, 42, 0 };
		ImColor dark_5{ 25, 28, 36, 255 };
		ImColor dark_6{ 30, 34, 44, 255 };
		ImColor dark_7{ 36, 40, 50 };

	} main;

	struct
	{
		ImColor active { 230, 235, 245 };
		ImColor hover{ 230, 235, 245, 155 };
		ImColor inactive{ 100, 115, 140, 255 };

	} text;

};

inline std::unique_ptr<c_colors> clr = std::make_unique<c_colors>();
