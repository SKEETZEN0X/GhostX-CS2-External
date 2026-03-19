#define IMGUI_DEFINE_MATH_OPERATORS
#include "settings/functions.h"

#include <stdafx.hpp>
#include <imgui.h>
#include <imgui_internal.h>
#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <algorithm>

namespace
{
	std::filesystem::path get_config_directory( )
	{
		const char* user_profile = std::getenv( "USERPROFILE" );
		if ( !user_profile ) return {};
		std::filesystem::path dir = std::filesystem::path( user_profile ) / "Documents" / "GhostX";
		std::error_code ec{};
		std::filesystem::create_directories( dir, ec );
		return dir;
	}
	std::filesystem::path get_config_path( const std::string& name )
	{
		auto dir = get_config_directory( );
		if ( dir.empty( ) ) return {};
		std::string safe = name;
		for ( char& c : safe ) if ( c == '/' || c == '\\' || c == ':' || c == '*' || c == '?' || c == '"' || c == '<' || c == '>' || c == '|' ) c = '_';
		if ( safe.empty( ) ) safe = "config";
		return dir / ( safe + ".bin" );
	}
	std::vector< std::string > get_config_list( )
	{
		auto dir = get_config_directory( );
		std::vector< std::string > out;
		if ( dir.empty( ) ) return out;
		std::error_code ec;
		for ( const auto& e : std::filesystem::directory_iterator( dir, ec ) )
		{
			if ( !e.is_regular_file( ec ) ) continue;
			std::string p = e.path( ).filename( ).string( );
			if ( p.size( ) > 4 && p.compare( p.size( ) - 4, 4, ".bin" ) == 0 )
				out.push_back( p.substr( 0, p.size( ) - 4 ) );
		}
		std::sort( out.begin( ), out.end( ) );
		return out;
	}
	bool save_config( const std::string& name )
	{
		const auto path = get_config_path( name );
		if ( path.empty( ) ) return false;
		std::ofstream out( path, std::ios::binary | std::ios::trunc );
		if ( !out ) return false;
		out.write( reinterpret_cast< const char* >( &settings::g_combat ), sizeof( settings::g_combat ) );
		out.write( reinterpret_cast< const char* >( &settings::g_esp ), sizeof( settings::g_esp ) );
		out.write( reinterpret_cast< const char* >( &settings::g_misc ), sizeof( settings::g_misc ) );
		return static_cast< bool >( out );
	}
	bool load_config( const std::string& name )
	{
		const auto path = get_config_path( name );
		if ( path.empty( ) ) return false;
		std::ifstream in( path, std::ios::binary );
		if ( !in ) return false;
		in.read( reinterpret_cast< char* >( &settings::g_combat ), sizeof( settings::g_combat ) );
		in.read( reinterpret_cast< char* >( &settings::g_esp ), sizeof( settings::g_esp ) );
		in.read( reinterpret_cast< char* >( &settings::g_misc ), sizeof( settings::g_misc ) );
		return static_cast< bool >( in );
	}
	bool delete_config( const std::string& name )
	{
		const auto path = get_config_path( name );
		if ( path.empty( ) ) return false;
		std::error_code ec{};
		return std::filesystem::remove( path, ec );
	}
	void reset_config_to_default( )
	{
		settings::g_combat = settings::combat{};
		settings::g_esp    = settings::esp{};
		settings::g_misc   = settings::misc{};
	}

	// float[4] helpers for zdraw::rgba
	void rgba_to_f4( const zdraw::rgba& c, float* f )
	{
		f[0] = c.r / 255.0f; f[1] = c.g / 255.0f;
		f[2] = c.b / 255.0f; f[3] = c.a / 255.0f;
	}
	void f4_to_rgba( const float* f, zdraw::rgba& c )
	{
		c.r = static_cast< std::uint8_t >( std::clamp( f[0] * 255.0f, 0.0f, 255.0f ) );
		c.g = static_cast< std::uint8_t >( std::clamp( f[1] * 255.0f, 0.0f, 255.0f ) );
		c.b = static_cast< std::uint8_t >( std::clamp( f[2] * 255.0f, 0.0f, 255.0f ) );
		c.a = static_cast< std::uint8_t >( std::clamp( f[3] * 255.0f, 0.0f, 255.0f ) );
	}

	// Wrapper: use settings int as float for Givenchy sliders
	float s_fov_f{}, s_smooth_f{}, s_delay_f{}, s_delay_tb_f{}, s_tsd_f{};

	// Config name popup state (file scope so accessible both inside and outside gui->end)
	bool g_show_cfg_popup = false;
	char g_cfg_name[128] = "";
} // namespace

void c_gui::render( )
{
	static bool once_loaded = true;
	static float intro_alpha = 0.f;
	static float intro_squeeze = 0.f;
	static float intro_scale_anim = 0.f;
	static bool intro_done = false;
	static int intro_phase = 0; // 0=fade-in, 1=hold, 2=squeeze-out
	static float phase_timer = 0.f;

	if ( once_loaded )
	{
		once_loaded = false;
		intro_phase = 0;
		phase_timer = 0.f;
		intro_alpha = 0.f;
		intro_squeeze = 0.f;
		intro_scale_anim = 0.8f;
	}

	if ( !intro_done )
	{
		const float dt = ImGui::GetIO( ).DeltaTime;
		phase_timer += dt;

		const float fade_in_dur  = 0.6f;
		const float hold_dur     = 0.7f;
		const float fade_out_dur = 0.8f;

		if ( intro_phase == 0 )
		{
			float p = std::clamp( phase_timer / fade_in_dur, 0.f, 1.f );
			float e = p * p * ( 3.f - 2.f * p ); // smoothstep
			intro_alpha = e;
			intro_scale_anim = 0.8f + 0.2f * e;
			if ( phase_timer >= fade_in_dur ) { intro_phase = 1; phase_timer = 0.f; }
		}
		else if ( intro_phase == 1 )
		{
			intro_alpha = 1.f;
			intro_scale_anim = 1.f;
			if ( phase_timer >= hold_dur ) { intro_phase = 2; phase_timer = 0.f; }
		}
		else
		{
			float p = std::clamp( phase_timer / fade_out_dur, 0.f, 1.f );
			float e = p * p * ( 3.f - 2.f * p );
			intro_alpha = 1.f - e;
			intro_squeeze = e;
			intro_scale_anim = 1.f - 0.15f * e;
			if ( phase_timer >= fade_out_dur ) intro_done = true;
		}

		ImDrawList* fg = ImGui::GetForegroundDrawList( );
		ImVec2 display = ImGui::GetIO( ).DisplaySize;

		ImU32 bg_col = IM_COL32( 12, 14, 20, static_cast< int >( intro_alpha * 230.f ) );
		fg->AddRectFilled( ImVec2( 0, 0 ), display, bg_col );

		const char* ghost_str = "Ghost";
		const char* x_str = "X";
		ImFont* font = var->font.medium15;
		if ( font )
		{
			float scale = 3.5f * intro_scale_anim;
			float orig_size = font->FontSize;

			ImVec2 ghost_sz = font->CalcTextSizeA( orig_size * scale, FLT_MAX, 0.f, ghost_str );
			ImVec2 x_sz     = font->CalcTextSizeA( orig_size * scale, FLT_MAX, 0.f, x_str );
			float total_w = ghost_sz.x + x_sz.x;
			float center_x = display.x * 0.5f;
			float center_y = display.y * 0.5f - ghost_sz.y * 0.5f;
			float base_x = center_x - total_w * 0.5f;

			float cursor_x = base_x;
			int a = static_cast< int >( intro_alpha * 255.f );
			ImU32 white_col  = IM_COL32( 230, 235, 245, a );
			ImU32 accent_col = IM_COL32( 50, 160, 235, a );

			for ( int i = 0; ghost_str[i]; ++i )
			{
				char ch[2] = { ghost_str[i], 0 };
				ImVec2 ch_sz = font->CalcTextSizeA( orig_size * scale, FLT_MAX, 0.f, ch );
				float char_center = cursor_x + ch_sz.x * 0.5f;
				float offset = ( center_x - char_center ) * intro_squeeze;
				fg->AddText( font, orig_size * scale, ImVec2( cursor_x + offset, center_y ), white_col, ch );
				cursor_x += ch_sz.x;
			}

			{
				float char_center = cursor_x + x_sz.x * 0.5f;
				float offset = ( center_x - char_center ) * intro_squeeze;
				fg->AddText( font, orig_size * scale, ImVec2( cursor_x + offset, center_y ), accent_col, x_str );
			}
		}

		if ( !intro_done )
		{
			gui->set_next_window_size( var->window.window_size );
			gui->begin( var->window.window_name, nullptr,
				ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration |
				ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus );
			gui->end( );
			return;
		}
	}

	gui->set_next_window_size( var->window.window_size );
	gui->begin( var->window.window_name, nullptr,
		ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus );
	{
		// Adapt element widths to actual window size
		const float win_w = var->window.window_size.x;
		elements->titlebar.size.x  = win_w;
		elements->tab_info.size.x  = win_w;
		elements->features.size.x  = win_w;
		elements->features.size.y  = var->window.window_size.y - elements->titlebar.size.y - elements->tab_info.size.y - 2.f;

		const float child_w = ( win_w - 30.f - 10.f - var->window.scrollbar_size - 4.f ) * 0.5f;
		elements->child.size.x              = child_w;
		elements->child.with_scrollbar_x    = child_w;
		elements->child.without_scrollbar_x = child_w;

		const float inner_w = child_w - 10.f;
		elements->checkbox.size.x        = inner_w;
		elements->button.button_size.x   = inner_w;
		elements->slider.size.x          = inner_w;
		elements->slider.button_size.x   = inner_w - 20.f;
		elements->combo.size.x           = inner_w;
		elements->combo.button_size.x    = inner_w;
		elements->combo_button.size.x    = inner_w - 20.f;
		elements->k_button.button_size.x = ( inner_w - 10.f ) * 0.5f;
		elements->kh_window.size.x       = ( inner_w - 10.f ) * 0.5f;

		gui->set_style( );
		gui->draw_decoration( );
		gui->setup_animations( );

		const ImVec2 pos = GetWindowPos( );
		const ImVec2 size = GetWindowSize( );
		var->window.window_size = size;

		// Title bar
		gui->begin_content( "title_bar", SCALE( elements->titlebar.size ) );
		{
			gui->begin_content( "name", SCALE( elements->titlebar.name_size ) );
			gui->end_content( );

			gui->sameline( );

			static float tabs_width = 0.f;
			gui->set_pos( size.x - tabs_width, pos_x );
			gui->begin_content( "tabs", SCALE( tabs_width, elements->titlebar.tabs_size.y ) );
			{
				widgets->direct_tab_button( "Aimbot", 0 );
				gui->sameline( );
				widgets->direct_tab_button( "Visuals", 1 );
				gui->sameline( );
				widgets->direct_tab_button( "Misc", 2 );
				gui->sameline( );
				widgets->direct_tab_button( "Config", 3 );
				tabs_width = GetCurrentWindow( )->ContentSize.x;
			}
			gui->end_content( );
		}
		gui->end_content( );

		// Tab info bar
		gui->begin_content( "tab_info", SCALE( elements->tab_info.size ) );
		{
			widgets->tab_info( );
		}
		gui->end_content( );

		gui->push_var( ImGuiStyleVar_Alpha, var->anim.features_alpha );
		gui->push_var( ImGuiStyleVar_ItemSpacing, ImVec2( SCALE( 8.f ), SCALE( 6.f ) ) );

		gui->set_pos( gui->get_pos( ).y + SCALE( 1.f ), pos_y );
		gui->begin_content( "features", SCALE( elements->features.size ), SCALE( 15, 15 ), SCALE( 15, 15 ), ImGuiWindowFlags_AlwaysVerticalScrollbar );
		{
			widgets->setup_child_anim( );

			static int weapon_group = 2; // default rifle

			// ────── TAB 0: AIMBOT ──────
			if ( var->tab.current == 0 )
			{
				ImGui::PushID( "aimbot_tab" );

				// Weapon type: full width (spans both columns)
				{
					const float full_w = gui->content_avail( ).x / var->gui.dpi;
					const float full_inner = full_w - 10.f;

					const float s_child     = elements->child.size.x;
					const float s_child_sb  = elements->child.with_scrollbar_x;
					const float s_child_nsb = elements->child.without_scrollbar_x;
					const float s_combo     = elements->combo.size.x;
					const float s_combo_btn = elements->combo.button_size.x;
					const float s_combo_b2  = elements->combo_button.size.x;
					const float s_slider    = elements->slider.size.x;
					const float s_slider_b  = elements->slider.button_size.x;
					const float s_checkbox  = elements->checkbox.size.x;
					const float s_button    = elements->button.button_size.x;

					elements->child.size.x              = full_w;
					elements->child.with_scrollbar_x    = full_w - 10.f;
					elements->child.without_scrollbar_x = full_w;
					elements->combo.size.x              = full_inner;
					elements->combo.button_size.x       = full_inner;
					elements->combo_button.size.x       = full_inner - 20.f;
					elements->slider.size.x             = full_inner;
					elements->slider.button_size.x      = full_inner - 20.f;
					elements->checkbox.size.x           = full_inner;
					elements->button.button_size.x      = full_inner;

					widgets->center_combo = true;
					gui->begin_group( );
					{
						if ( widgets->child( "Weapon type" ) )
						{
							std::vector< std::string > groups = { "Pistol", "SMG", "Rifle", "Shotgun", "Sniper", "LMG" };
							widgets->combo( "Group", &weapon_group, groups );
							widgets->end_child( );
						}
					}
					gui->end_group( );
					widgets->center_combo = false;

					elements->child.size.x              = s_child;
					elements->child.with_scrollbar_x    = s_child_sb;
					elements->child.without_scrollbar_x = s_child_nsb;
					elements->combo.size.x              = s_combo;
					elements->combo.button_size.x       = s_combo_btn;
					elements->combo_button.size.x       = s_combo_b2;
					elements->slider.size.x             = s_slider;
					elements->slider.button_size.x      = s_slider_b;
					elements->checkbox.size.x           = s_checkbox;
					elements->button.button_size.x      = s_button;
				}

				gui->begin_group( );
				{
					if ( widgets->child( "Aimbot" ) )
					{
						auto& cfg = settings::g_combat.groups[ weapon_group ];
						widgets->checkbox( "Enable Aimbot", &cfg.aimbot.enabled );
						widgets->keybind_button( "Aimbot Key", &cfg.aimbot.key );

						s_fov_f = static_cast< float >( cfg.aimbot.fov );
						widgets->slider_float( "FOV", &s_fov_f, 1.0f, 45.0f );
						cfg.aimbot.fov = static_cast< int >( s_fov_f );

						s_smooth_f = static_cast< float >( cfg.aimbot.smoothing );
						widgets->slider_float( "Smoothing", &s_smooth_f, 0.0f, 50.0f );
						cfg.aimbot.smoothing = static_cast< int >( s_smooth_f );

						widgets->checkbox( "Head", &cfg.aimbot.target_head );
						widgets->checkbox( "Chest", &cfg.aimbot.target_chest );
						widgets->checkbox( "Stomach", &cfg.aimbot.target_stomach );
						widgets->checkbox( "Legs", &cfg.aimbot.target_legs );
						widgets->checkbox( "Visible Only", &cfg.aimbot.visible_only );

						widgets->checkbox( "Predictive", &cfg.aimbot.predictive );

						s_tsd_f = static_cast< float >( cfg.aimbot.target_switch_delay_ms );
						widgets->slider_float( "Switch Delay (ms)", &s_tsd_f, 0.0f, 2000.0f );
						cfg.aimbot.target_switch_delay_ms = static_cast< int >( s_tsd_f );

						widgets->checkbox( "Draw FOV", &cfg.aimbot.draw_fov );
						if ( cfg.aimbot.draw_fov )
						{
							float fc[4]; rgba_to_f4( cfg.aimbot.fov_color, fc );
							widgets->color_button( "FOV Color", fc );
							f4_to_rgba( fc, cfg.aimbot.fov_color );
						}

						widgets->end_child( );
					}
				}
				gui->end_group( );

				gui->sameline( );

				gui->begin_group( );
				{
					if ( widgets->child( "Triggerbot" ) )
					{
						auto& cfg = settings::g_combat.groups[ weapon_group ];
						widgets->checkbox( "Enable Triggerbot", &cfg.triggerbot.enabled );
						widgets->keybind_button( "Triggerbot Key", &cfg.triggerbot.key );
						widgets->slider_float( "Hitchance", &cfg.triggerbot.hitchance, 0.0f, 100.0f );

						s_delay_tb_f = static_cast< float >( cfg.triggerbot.delay );
						widgets->slider_float( "Delay (ms)", &s_delay_tb_f, 0.0f, 500.0f );
						cfg.triggerbot.delay = static_cast< int >( s_delay_tb_f );

						widgets->checkbox( "Autostop", &cfg.triggerbot.autostop );
						if ( cfg.triggerbot.autostop )
							widgets->checkbox( "Early Autostop", &cfg.triggerbot.early_autostop );

						widgets->checkbox( "Predictive", &cfg.triggerbot.predictive );
						widgets->end_child( );
					}
				}
				gui->end_group( );
				ImGui::PopID( );
			}

			// ────── TAB 1: VISUALS (ESP) ──────
			else if ( var->tab.current == 1 )
			{
				auto& p = settings::g_esp.m_player;

				// Left column: Box, Health Bar, Snaplines
				gui->begin_group( );
				{
					ImGui::PushID( "box" );
					if ( widgets->child( "Box" ) )
					{
						widgets->checkbox( "Enabled", &p.m_box.enabled );
						int bs = ( p.m_box.style == settings::esp::player::box::style0::cornered ) ? 1 : 0;
						std::vector< std::string > styles = { "Full", "Cornered" };
						widgets->combo( "Box style", &bs, styles );
						p.m_box.style = ( bs == 0 ) ? settings::esp::player::box::style0::full : settings::esp::player::box::style0::cornered;
						widgets->checkbox( "Fill", &p.m_box.fill );
						widgets->checkbox( "Outline", &p.m_box.outline );
						if ( p.m_box.style == settings::esp::player::box::style0::cornered )
							widgets->slider_float( "Corner Len", &p.m_box.corner_length, 4.0f, 30.0f );
						float vc[4]; rgba_to_f4( p.m_box.visible_color, vc );
						widgets->color_button( "Visible", vc );
						f4_to_rgba( vc, p.m_box.visible_color );
						float oc[4]; rgba_to_f4( p.m_box.occluded_color, oc );
						widgets->color_button( "Occluded", oc );
						f4_to_rgba( oc, p.m_box.occluded_color );
						widgets->end_child( );
					}
					ImGui::PopID( );

					ImGui::PushID( "health_bar" );
					if ( widgets->child( "Health Bar" ) )
					{
						widgets->checkbox( "Enabled", &p.m_health_bar.enabled );
						widgets->checkbox( "Outline", &p.m_health_bar.outline );
						widgets->checkbox( "Gradient", &p.m_health_bar.gradient );
						widgets->checkbox( "Show Value", &p.m_health_bar.show_value );
						widgets->end_child( );
					}
					ImGui::PopID( );

					ImGui::PushID( "snaplines" );
					if ( widgets->child( "Snaplines" ) )
					{
						widgets->checkbox( "Enabled", &p.m_snaplines.enabled );
						widgets->slider_float( "Thickness", &p.m_snaplines.thickness, 0.5f, 4.0f );
						float sc[4]; rgba_to_f4( p.m_snaplines.color, sc );
						widgets->color_button( "Color", sc );
						f4_to_rgba( sc, p.m_snaplines.color );
						widgets->end_child( );
					}
					ImGui::PopID( );
				}
				gui->end_group( );

				gui->sameline( );

				// Right column: Skeleton, Name/Weapon, Items
				gui->begin_group( );
				{
					ImGui::PushID( "skeleton" );
					if ( widgets->child( "Skeleton" ) )
					{
						widgets->checkbox( "Enabled", &p.m_skeleton.enabled );
						widgets->slider_float( "Thickness", &p.m_skeleton.thickness, 0.5f, 4.0f );
						float vc[4]; rgba_to_f4( p.m_skeleton.visible_color, vc );
						widgets->color_button( "Visible", vc );
						f4_to_rgba( vc, p.m_skeleton.visible_color );
						widgets->end_child( );
					}
					ImGui::PopID( );

					ImGui::PushID( "name_weapon" );
					if ( widgets->child( "Name / Weapon" ) )
					{
						widgets->checkbox( "Name", &p.m_name.enabled );
						widgets->checkbox( "Weapon", &p.m_weapon.enabled );
						int dv = static_cast< int >( p.m_weapon.display );
						std::vector< std::string > disp = { "Text", "Icon", "Text + Icon" };
						widgets->combo( "Weapon display", &dv, disp );
						p.m_weapon.display = static_cast< settings::esp::player::weapon::display_type >( std::clamp( dv, 0, 2 ) );
						widgets->end_child( );
					}
					ImGui::PopID( );

					ImGui::PushID( "items" );
					if ( widgets->child( "Items" ) )
					{
						auto& it = settings::g_esp.m_item;
						widgets->checkbox( "Enabled", &it.enabled );
						widgets->slider_float( "Max Dist", &it.max_distance, 5.0f, 150.0f );
						widgets->checkbox( "Icon", &it.m_icon.enabled );
						widgets->checkbox( "Name", &it.m_name.enabled );
						widgets->checkbox( "Ammo", &it.m_ammo.enabled );
						widgets->end_child( );
					}
					ImGui::PopID( );
				}
				gui->end_group( );
			}

			// ────── TAB 2: MISC ──────
			else if ( var->tab.current == 2 )
			{
				auto& gr = settings::g_misc.m_grenades;

				gui->begin_group( );
				{
					ImGui::PushID( "grenade" );
					if ( widgets->child( "Grenade Prediction" ) )
					{
						widgets->checkbox( "Enabled", &gr.enabled );
						widgets->checkbox( "Local Only", &gr.local_only );
						widgets->slider_float( "Line Thickness", &gr.line_thickness, 0.5f, 5.0f );
						widgets->checkbox( "Gradient Line", &gr.line_gradient );
						widgets->checkbox( "Show Bounces", &gr.show_bounces );
						widgets->slider_float( "Bounce Size", &gr.bounce_size, 1.0f, 8.0f );
						widgets->slider_float( "Detonate Size", &gr.detonate_size, 1.0f, 10.0f );
						widgets->slider_float( "Fade Duration", &gr.fade_duration, 0.0f, 2.0f );
						widgets->end_child( );
					}
					ImGui::PopID( );
				}
				gui->end_group( );

				gui->sameline( );

				gui->begin_group( );
				{
					ImGui::PushID( "custom_colors" );
					if ( widgets->child( "Custom Colors" ) )
					{
						widgets->checkbox( "Per Type Colors", &gr.per_type_colors );
						widgets->end_child( );
					}
					ImGui::PopID( );
				}
				gui->end_group( );
			}

			// ────── TAB 3: CONFIG ──────
			else if ( var->tab.current == 3 )
			{
				ImGui::PushID( "config_tab" );
				static int selected_config = 0;

				std::vector< std::string > list = get_config_list( );
				bool has_configs = !list.empty( );
				if ( !has_configs )
					list.push_back( "(none)" );
				if ( selected_config >= static_cast< int >( list.size( ) ) )
					selected_config = static_cast< int >( list.size( ) ) - 1;
				if ( selected_config < 0 )
					selected_config = 0;

				gui->begin_group( );
				{
					if ( widgets->child( "Configs" ) )
					{
						widgets->combo( "Config", &selected_config, list );
						if ( widgets->button( "Load" ) && has_configs )
						{
							if ( load_config( list[selected_config] ) )
								notify->add_notify( "Config loaded", notify_type::success );
						}
						if ( widgets->button( "Save" ) && has_configs )
						{
							if ( save_config( list[selected_config] ) )
								notify->add_notify( "Config saved", notify_type::success );
						}
						if ( widgets->button( "Delete" ) && has_configs )
						{
							if ( delete_config( list[selected_config] ) )
							{
								notify->add_notify( "Config deleted", notify_type::success );
								if ( selected_config > 0 ) --selected_config;
							}
						}
						if ( widgets->button( "Reset" ) )
						{
							reset_config_to_default( );
							notify->add_notify( "Reset to default", notify_type::success );
						}
						widgets->end_child( );
					}
				}
				gui->end_group( );

				gui->sameline( );

				gui->begin_group( );
				{
					if ( widgets->child( "New Config" ) )
					{
						if ( widgets->button( "Create new..." ) )
							g_show_cfg_popup = true;
						widgets->end_child( );
					}
				}
				gui->end_group( );
				ImGui::PopID( );
			}
		}
		gui->end_content( );

		gui->pop_var( 2 );

		// Resize grip bottom-right (manual hit-test to avoid clipping)
		{
			static bool grip_dragging = false;
			static ImVec2 grip_start_mouse;
			static ImVec2 grip_start_size;
			const float grip_sz = SCALE( 22.f );
			ImVec2 win_pos = GetWindowPos( );
			ImVec2 win_sz  = GetWindowSize( );
			ImVec2 grip_min = ImVec2( win_pos.x + win_sz.x - grip_sz, win_pos.y + win_sz.y - grip_sz );
			ImVec2 grip_max = ImVec2( win_pos.x + win_sz.x, win_pos.y + win_sz.y );

			ImVec2 mouse = ImGui::GetIO( ).MousePos;
			bool grip_hovered = ( mouse.x >= grip_min.x && mouse.x <= grip_max.x && mouse.y >= grip_min.y && mouse.y <= grip_max.y );

			if ( grip_hovered )
				ImGui::SetMouseCursor( ImGuiMouseCursor_ResizeNWSE );

			if ( grip_hovered && ImGui::IsMouseClicked( ImGuiMouseButton_Left ) )
			{
				grip_dragging = true;
				grip_start_mouse = mouse;
				grip_start_size = var->window.window_size;
			}

			if ( !ImGui::IsMouseDown( ImGuiMouseButton_Left ) )
				grip_dragging = false;

			if ( grip_dragging )
			{
				ImGuiContext& g = *GImGui;
				if ( g.MovingWindow )
					g.MovingWindow = nullptr;

				ImGui::SetMouseCursor( ImGuiMouseCursor_ResizeNWSE );
				ImVec2& sz = var->window.window_size;
				sz.x = std::clamp( grip_start_size.x + ( mouse.x - grip_start_mouse.x ), 500.f, 1400.f );
				sz.y = std::clamp( grip_start_size.y + ( mouse.y - grip_start_mouse.y ), 400.f, 1000.f );
			}

			ImDrawList* dl = ImGui::GetForegroundDrawList( );
			const float pad = SCALE( 4.f );
			ImVec2 br = ImVec2( grip_max.x - pad, grip_max.y - pad );
			ImU32 col = grip_hovered || grip_dragging
				? draw->get_clr( clr->accent.main )
				: draw->get_clr( clr->text.inactive );
			dl->AddTriangleFilled( br, ImVec2( grip_min.x + pad, br.y ), ImVec2( br.x, grip_min.y + pad ), col );
		}

	}
	gui->end( );

	// Config name popup - native ImGui window, completely outside custom framework
	if ( g_show_cfg_popup )
	{
		const ImVec2 display = ImGui::GetIO( ).DisplaySize;
		const ImVec2 popup_sz( 320.f, 140.f );
		ImGui::SetNextWindowPos( ImVec2( display.x * 0.5f - popup_sz.x * 0.5f, display.y * 0.5f - popup_sz.y * 0.5f ), ImGuiCond_Appearing );
		ImGui::SetNextWindowSize( popup_sz );

		ImGui::PushStyleColor( ImGuiCol_WindowBg, IM_COL32( 22, 25, 33, 245 ) );
		ImGui::PushStyleColor( ImGuiCol_Border, IM_COL32( 50, 160, 235, 120 ) );
		ImGui::PushStyleColor( ImGuiCol_TitleBg, IM_COL32( 18, 20, 28, 255 ) );
		ImGui::PushStyleColor( ImGuiCol_TitleBgActive, IM_COL32( 28, 32, 42, 255 ) );
		ImGui::PushStyleColor( ImGuiCol_Text, IM_COL32( 230, 235, 245, 255 ) );
		ImGui::PushStyleColor( ImGuiCol_FrameBg, IM_COL32( 28, 32, 42, 255 ) );
		ImGui::PushStyleColor( ImGuiCol_FrameBgHovered, IM_COL32( 36, 40, 50, 255 ) );
		ImGui::PushStyleColor( ImGuiCol_FrameBgActive, IM_COL32( 36, 40, 50, 255 ) );
		ImGui::PushStyleColor( ImGuiCol_Button, IM_COL32( 50, 160, 235, 200 ) );
		ImGui::PushStyleColor( ImGuiCol_ButtonHovered, IM_COL32( 70, 180, 255, 220 ) );
		ImGui::PushStyleColor( ImGuiCol_ButtonActive, IM_COL32( 40, 140, 210, 255 ) );
		ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 8.f );
		ImGui::PushStyleVar( ImGuiStyleVar_FrameRounding, 5.f );
		ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 16.f, 12.f ) );
		ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 10.f, 8.f ) );
		ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 1.f );

		bool open = true;
		if ( ImGui::Begin( "Create Config", &open, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar ) )
		{
			ImGui::Text( "Config name:" );
			ImGui::SetNextItemWidth( -1 );

			bool enter_pressed = ImGui::InputText( "##cfg_popup_input", g_cfg_name, sizeof( g_cfg_name ), ImGuiInputTextFlags_EnterReturnsTrue );

			if ( ImGui::IsWindowAppearing( ) )
				ImGui::SetKeyboardFocusHere( -1 );

			ImGui::Spacing( );

			const float btn_w = ( ImGui::GetContentRegionAvail( ).x - 8.f ) * 0.5f;
			if ( ( ImGui::Button( "Create", ImVec2( btn_w, 0 ) ) || enter_pressed ) && g_cfg_name[0] != '\0' )
			{
				if ( save_config( g_cfg_name ) )
				{
					notify->add_notify( "Config created", notify_type::success );
					g_cfg_name[0] = '\0';
					g_show_cfg_popup = false;
				}
			}
			ImGui::SameLine( 0, 8.f );
			if ( ImGui::Button( "Cancel", ImVec2( btn_w, 0 ) ) )
			{
				g_cfg_name[0] = '\0';
				g_show_cfg_popup = false;
			}
		}
		ImGui::End( );
		ImGui::PopStyleVar( 5 );
		ImGui::PopStyleColor( 11 );

		if ( !open )
		{
			g_cfg_name[0] = '\0';
			g_show_cfg_popup = false;
		}
	}
}
