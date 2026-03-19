#define IMGUI_DEFINE_MATH_OPERATORS

#include "overlay.hpp"
#include <dwmapi.h>

#include <filesystem>
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>
#include <cctype>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "image.h"
#include "seekinblood.h"
#include <stack>
#include <unordered_set>

#ifdef max
#undef max
#endif
#include "../ckeybind/keybind.hpp"

#ifdef min
#undef min
#endif

#include <algorithm>

#include "../../source/storage/Globals.hpp"
#include "../../source/storage/include.h"
#include "../xorstr/xorstr.hpp"
#include "../imgui/backends/imgui_impl_win32.h"
#include "../imgui/misc/freetype/imgui_freetype.h"
#include "../gui/settings/functions.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../Overlay/gui2.h"


bool Keybind(keybind* keybind, const ImVec2& size_arg = ImVec2(0, 0), bool clicked = false, ImGuiButtonFlags flags = 0)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(keybind->get_name().c_str());
	const ImVec2 label_size = ImGui::CalcTextSize(keybind->get_name().c_str(), NULL, true);

	ImVec2 pos = window->DC.CursorPos;
	if ((flags & ImGuiButtonFlags_AlignTextBaseLine) &&
		style.FramePadding.y <
		window->DC.CurrLineTextBaseOffset)

		pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
	ImVec2 size = ImGui::CalcItemSize(
		size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);
	ImGui::ItemSize(size, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id))
		return false;


	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, flags);

	bool value_changed = false;
	int key = keybind->key;

	auto io = ImGui::GetIO();

	std::string name = keybind->get_key_name();

	if (keybind->waiting_for_input)
		name = "[...]";

	if (ImGui::GetIO().MouseClicked[0] && hovered)
	{
		if (g.ActiveId == id)
		{
			keybind->waiting_for_input = true;
		}
	}
	else if (ImGui::GetIO().MouseClicked[1] && hovered)
	{
		ImGui::OpenPopup(keybind->get_name().c_str());
	}
	else if (ImGui::GetIO().MouseClicked[0] && !hovered)
	{
		if (g.ActiveId == id)
			ImGui::ClearActiveID();
	}

	if (keybind->waiting_for_input)
	{
		if (ImGui::GetIO().MouseClicked[0] && !hovered)
		{
			keybind->key = VK_LBUTTON;

			ImGui::ClearActiveID();
			keybind->waiting_for_input = false;
		}
		else
		{
			if (keybind->set_key())
			{
				ImGui::ClearActiveID();
				keybind->waiting_for_input = false;
			}
		}
	}

	ImVec4 textcolor = ImLerp(ImVec4(201 / 255.f, 204 / 255.f, 210 / 255.f, 1.f), ImVec4(1.0f, 1.0f, 1.0f, 1.f), 1.f);



	window->DrawList->AddText(
		bb.Min +
		ImVec2(
			size_arg.x / 2 - ImGui::CalcTextSize(name.c_str()).x / 2,
			size_arg.y / 2 - ImGui::CalcTextSize(name.c_str()).y / 2),
		ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_Text)),
		name.c_str());

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Popup |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar;
	ImGui::SetNextWindowPos(pos + ImVec2(0, size_arg.y - 1));
	ImGui::SetNextWindowSize(ImVec2(56, 67));


	if (ImGui::BeginPopup(keybind->get_name().c_str(), 0))
	{
		ImGui::BeginGroup();
		{
			if (ImGui::Selectable(XorStr("Hold"), keybind->type == keybind::HOLD))
				keybind->type = keybind::HOLD;
			if (ImGui::Selectable(XorStr("Toggle"), keybind->type == keybind::TOGGLE))
				keybind->type = keybind::TOGGLE;
			if (ImGui::Selectable(XorStr("Always"), keybind->type == keybind::ALWAYS))
				keybind->type = keybind::ALWAYS;
		}
		ImGui::EndGroup();

		ImGui::EndPopup();
	}
	return pressed;
}

bool overlay::fullsc(HWND windowHandle)
{
	MONITORINFO monitorInfo = { sizeof(MONITORINFO) };
	if (GetMonitorInfo(MonitorFromWindow(windowHandle, MONITOR_DEFAULTTOPRIMARY), &monitorInfo))
	{
		RECT windowRect;
		if (GetWindowRect(windowHandle, &windowRect))
		{
			return windowRect.left == monitorInfo.rcMonitor.left
				&& windowRect.right == monitorInfo.rcMonitor.right
				&& windowRect.top == monitorInfo.rcMonitor.top
				&& windowRect.bottom == monitorInfo.rcMonitor.bottom;
		}
	}
}

bool DrawMultiCombo(const char* label, const std::vector<std::string>& items, std::vector<bool>& selections)
{
	ImGuiContext& g = *GImGui;

	std::string preview;
	for (size_t i = 0; i < items.size(); ++i) {
		if (selections[i]) {
			if (!preview.empty())
				preview += ", ";
			preview += items[i];
		}
	}
	if (preview.empty())
		preview = "Select items";

	if (!ImGui::BeginCombo(label, preview.c_str(), ImGuiComboFlags_None))
		return false;

	bool value_changed = false;

	for (size_t i = 0; i < items.size(); ++i) {
		ImGui::PushID(static_cast<int>(i));
		bool selected = selections[i];
		if (ImGui::Selectable(items[i].c_str(), selected)) {
			selections[i] = !selected;
			value_changed = true;
		}
		ImGui::PopID();
	}

	ImGui::EndCombo();

	if (value_changed)
		ImGui::MarkItemEdited(g.LastItemData.ID);

	return value_changed;
}

void render_fullscreen_background(HWND hwnd, float alpha, float blurAmount) {



	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, alpha));
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
	ImGui::Begin("background", nullptr,
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoSavedSettings);
	ImGui::End();
	ImGui::PopStyleColor();
}

bool overlay::render() {

	ImGui_ImplWin32_EnableDpiAwareness();

	WNDCLASSEX wc;
	wc.cbClsExtra = NULL;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbWndExtra = NULL;
	wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hInstance = GetModuleHandle(nullptr);
	wc.lpfnWndProc = window_proc;
	wc.lpszClassName = TEXT("feshgvgavshfagfvd");
	wc.lpszMenuName = nullptr;
	wc.style = CS_VREDRAW | CS_HREDRAW;

	RegisterClassEx(&wc);
	const HWND hw = CreateWindowEx(0, wc.lpszClassName, TEXT("Givenchy Menu"),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1000, 800, nullptr, nullptr, wc.hInstance, nullptr);

	if (!create_device_d3d(hw))
	{
		cleanup_device_d3d();
		UnregisterClass(wc.lpszClassName, wc.hInstance);
		return false;
	}

	SetWindowPos(hw, HWND_TOP, 0, 0, 1000, 800, SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW);
	UpdateWindow(hw);


	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO();
	auto config = ImFontConfig();
	config.FontDataOwnedByAtlas = false;
	config.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_ForceAutoHint;

	gui->init_static(io, &config);
	ImFontConfig FontConfig;
	FontConfig.FontDataOwnedByAtlas = false;

	const ImWchar* Ranges = io.Fonts->GetGlyphRangesDefault();

	ImFontConfig smallestPixelConfig;
	smallestPixelConfig.OversampleH = 8;
	smallestPixelConfig.OversampleV = 8;
	smallestPixelConfig.PixelSnapH = false;
	smallestPixelConfig.FontDataOwnedByAtlas = false;
	

	ImGui_ImplWin32_Init(hw);
	ImGui_ImplDX11_Init(d3d11_device, d3d11_device_context);

	const ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	static ImFont* pixelatedFont{ NULL };
	bool draw = true;
	bool streamable = false;
	bool player_list = false;
	static bool performanceMode = true;
	std::string selectedPlayer;
	bool keybind_list = false;
	bool followSelectedPlayer = false;
	bool isFollowThreadRunning = false;
	bool esp_preview = false;
	std::thread followThread;
	bool done = false;
	bool ex = false;
	int tab = 0;
	int esptype = 0;

	ImGui::StyleColorsDark(); 

	ImFontConfig font_cfg;
	font_cfg.FontDataOwnedByAtlas = false;

	int font = 0; 
	ImFont* selectedFont = nullptr;

	if (font == 0) {
		selectedFont = io.Fonts->AddFontDefault();
	}
	unsigned char* rgba_data = stbi_load_from_memory(image_data, sizeof(image_data), &image_width, &image_height, &channels, 4);

	ID3D11ShaderResourceView* mytexture = nullptr;

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = image_width;
	desc.Height = image_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA subResource = {};
	subResource.pSysMem = rgba_data;
	subResource.SysMemPitch = image_width * 4;

	ID3D11Texture2D* pTexture = nullptr;
	HRESULT hr = d3d11_device->CreateTexture2D(&desc, &subResource, &pTexture);
	if (SUCCEEDED(hr))
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srcDesc = {};
		srcDesc.Format = desc.Format;
		srcDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srcDesc.Texture2D.MostDetailedMip = 0;
		srcDesc.Texture2D.MipLevels = 1;

		hr = d3d11_device->CreateShaderResourceView(pTexture, &srcDesc, &mytexture);
		pTexture->Release();
	}

	stbi_image_free(rgba_data);


	while (!done)
	{


		MSG msg;
		while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				done = true;
		}
		if (done) break;



		if (gui->reinit_static(io, &config))
			ImGui_ImplDX11_CreateDeviceObjects();

		if (GetAsyncKeyState(storage::menukeybind.key) & 1)
			draw = !draw;

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGuiStyle& style = ImGui::GetStyle();
		ImGuiIO& io = ImGui::GetIO();

			if (selectedFont)
				ImGui::PushFont(selectedFont);

			if (keybind_list)
			{
			bool espEnabled =
				storage::visuals::box ||
				storage::visuals::tracers ||
				storage::tool_esp ||
				storage::weapon_icon_esp ||
				storage::name_esp ||
				storage::visuals::distance ||
				storage::visuals::health_bar ||
					storage::visuals::chams;
				ImGui::SetNextWindowPos(ImVec2(10, 50), ImGuiCond_Always);
				int visibleLines = 0;
				if (storage::aimbotkeybind.get_key_name() != "[-]");
				if (storage::silentaimkeybind.get_key_name() != "[-]") visibleLines++;
				if (storage::speedkeybind.get_key_name() != "[-]") visibleLines++;
				if (storage::jumppowerkeybind.get_key_name() != "[-]") visibleLines++;
				float baseHeight = 80.0f; 

				float extraHeight = visibleLines * 16.0f; 
				if (espEnabled)
					extraHeight += 17.0f;  
				float totalHeight = baseHeight + extraHeight;

				ImGui::SetNextWindowSize(ImVec2(180, totalHeight));
				ImGui::Begin("##Keybind", nullptr,
					ImGuiWindowFlags_NoTitleBar |
					ImGuiWindowFlags_NoResize |
					ImGuiWindowFlags_AlwaysAutoResize |
					ImGuiWindowFlags_NoMove |
					ImGuiWindowFlags_NoSavedSettings |
					ImGuiWindowFlags_NoInputs  );
				ImGui::Text("Keybinds");
				ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.12f, 0.12f, 0.12f, 1.00f));

				ImGui::BeginChild("##ESPPPPP", ImVec2(0, 0), ImGuiChildFlags_Border);
				ImGui::PopStyleColor(); 
				ImGui::BeginChild("##ESPPPPP2", ImVec2(0, 0), ImGuiChildFlags_Border);

			
				if (storage::aimbotkeybind.get_key_name() != "[-]") {
					const char* mode = (storage::aimbotkeybind.type == 1) ? "[Hold]" :
						(storage::aimbotkeybind.type == 0) ? "[Toggle]" :
						(storage::aimbotkeybind.type == 2) ? "[Always]" :
						"[Unknown]";
					ImGui::Text("Aimbot - %s %s", storage::aimbotkeybind.get_key_name().c_str(), mode);
				}

				if (storage::silentaimkeybind.get_key_name() != "[-]") {
					const char* mode = (storage::silentaimkeybind.type == 1) ? "[Hold]" :
						(storage::silentaimkeybind.type == 0) ? "[Toggle]" :
						(storage::silentaimkeybind.type == 2) ? "[Always]" :
						"[Unknown]";;
					ImGui::Text("Silent Aim - %s %s", storage::silentaimkeybind.get_key_name().c_str(), mode);
				}


				if (storage::speedkeybind.get_key_name() != "[-]") {
					const char* mode = (storage::speedkeybind.type == 1) ? "[Hold]" :
						(storage::speedkeybind.type == 0) ? "[Toggle]" :
						(storage::speedkeybind.type == 2) ? "[Always]" :
						"[Unknown]";
					ImGui::Text("Walkspeed - %s %s", storage::speedkeybind.get_key_name().c_str(), mode);
				}

				if (storage::jumppowerkeybind.get_key_name() != "[-]") {
					const char* mode = (storage::jumppowerkeybind.type == 1) ? "[Hold]" :
						(storage::jumppowerkeybind.type == 0) ? "[Toggle]" :
						(storage::jumppowerkeybind.type == 2) ? "[Always]" :
						"[Unknown]";
					ImGui::Text("JumpPower - %s %s", storage::jumppowerkeybind.get_key_name().c_str(), mode);
				}

				if (espEnabled) {
					ImGui::Text("ESP - [Always]");
				}
				ImGui::End();
		}


			ImGui::PopFont();
			
			// Always clear with a solid color so there is a background, since it's no longer transparent
			const float clear_color_with_alpha[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
			d3d11_device_context->ClearRenderTargetView(d3d11_render_target_view, clear_color_with_alpha);

			if (draw) {
				gui->render();
			}





		ImGui::Render();
		d3d11_device_context->OMSetRenderTargets(1, &d3d11_render_target_view, nullptr);
		// Note: We already cleared the render target above
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		dxgi_swap_chain->Present(1, 0); // Present with VSync
	}

	init = false;

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	cleanup_device_d3d();
	DestroyWindow(hw);
	UnregisterClass(wc.lpszClassName, wc.hInstance);
}

void overlay::move_window(HWND hw)
{
	HWND target = FindWindowA(0, XorStr("Roblox"));
	
	if (!target)
	{
		MoveWindow(hw, 0, 0, 0, 0, true);
		return;
	}

	RECT rect;
	GetWindowRect(target, &rect);

	int rsize_x = rect.right - rect.left;
	int rsize_y = rect.bottom - rect.top;

	if (fullsc(target))
	{
		rsize_x += 16;
	}
	else
	{
		rsize_y -= 39;
		rect.left += 4;
		rect.top += 31;
	}

	MoveWindow(hw, rect.left, rect.top, rsize_x, rsize_y, TRUE);
}

bool overlay::create_device_d3d(HWND hw)
{
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hw;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	const UINT create_device_flags = 0;
	D3D_FEATURE_LEVEL d3d_feature_level;
	const D3D_FEATURE_LEVEL feature_level_arr[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, create_device_flags, feature_level_arr, 2, D3D11_SDK_VERSION, &sd, &dxgi_swap_chain, &d3d11_device, &d3d_feature_level, &d3d11_device_context);
	if (res == DXGI_ERROR_UNSUPPORTED)
		res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, create_device_flags, feature_level_arr, 2, D3D11_SDK_VERSION, &sd, &dxgi_swap_chain, &d3d11_device, &d3d_feature_level, &d3d11_device_context);
	if (res != S_OK)
		return false;

	create_render_target();
	return true;
}

void overlay::cleanup_device_d3d()
{
	cleanup_render_target();

	if (dxgi_swap_chain)
	{
		dxgi_swap_chain->Release();
		dxgi_swap_chain = nullptr;
	}

	if (d3d11_device_context)
	{
		d3d11_device_context->Release();
		d3d11_device_context = nullptr;
	}

	if (d3d11_device)
	{
		d3d11_device->Release();
		d3d11_device = nullptr;
	}
}

void overlay::create_render_target()
{
	ID3D11Texture2D* d3d11_back_buffer;
	dxgi_swap_chain->GetBuffer(0, IID_PPV_ARGS(&d3d11_back_buffer));
	if (d3d11_back_buffer != nullptr)
	{
		d3d11_device->CreateRenderTargetView(d3d11_back_buffer, nullptr, &d3d11_render_target_view);
		d3d11_back_buffer->Release();
	}
}

void overlay::cleanup_render_target()
{
	if (d3d11_render_target_view)
	{
		d3d11_render_target_view->Release();
		d3d11_render_target_view = nullptr;
	}
}

LRESULT __stdcall overlay::window_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (d3d11_device != nullptr && wParam != SIZE_MINIMIZED)
		{
			cleanup_render_target();
			dxgi_swap_chain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			create_render_target();
		}
		return 0;

	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU)
			return 0;
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}