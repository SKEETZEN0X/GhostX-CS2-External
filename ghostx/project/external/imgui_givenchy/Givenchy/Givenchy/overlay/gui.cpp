#include <stack>
#include <unordered_set>
#include <filesystem> 
#include <vector>
#include <string>
#include <algorithm>
#include "imgui/imgui.h"
#include "../source/storage/Globals.hpp"
#include "gui/settings/functions.h"
#include "../source/configs/configs.hpp"
#include "xorstr/xorstr.hpp"
#include "Overlay/gui2.h"
#include "Overlay/overlay.hpp"
#include <d3d11.h>


void c_gui::render()
{
    gui->begin(var->window.window_name, nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus);
    {
        gui->set_style();
        gui->draw_decoration();
        gui->setup_animations();

        const ImVec2 pos = GetWindowPos();
        const ImVec2 size = GetWindowSize();

        gui->begin_content("title_bar", SCALE(elements->titlebar.size));
        {
            gui->begin_content("name", SCALE(elements->titlebar.name_size));
            {

            }
            gui->end_content();

            gui->sameline();

            static float tabs_width = 0.f;

            gui->set_pos(size.x - tabs_width, pos_x);
            gui->begin_content("tabs", SCALE(tabs_width, elements->titlebar.tabs_size.y));
            {
                std::vector<std::string_view> tab_names = { "Combat", "Visuals", "Exploits", "Misc" };
                std::vector<std::vector<std::string_view>> subtab_names = {
                    { "Aimbot", "Silent"},
                    { "Visuals" },
                    { "Exploits", "Movement", "Client Sided" },
                    { "Settings" }
                };
                std::vector<std::vector<int>> subtabs = {
                    {0, 1},
                    {3},
                    {6, 10, 11},
                    {7},
                };

                for (int i = 0; i < tab_names.size(); i++) {
                    widgets->tab_button(tab_names[i].data(), subtabs[i], subtab_names[i]);
                    gui->sameline();
                }

                tabs_width = GetCurrentWindow()->ContentSize.x;
            }
            gui->end_content();
        }
        gui->end_content();

        gui->begin_content("tab_info", SCALE(elements->tab_info.size));
        {
            widgets->tab_info();
        }
        gui->end_content();

        gui->push_var(ImGuiStyleVar_Alpha, var->anim.features_alpha);

        gui->set_pos(gui->get_pos().y + SCALE(1.f), pos_y);
        gui->begin_content("features", SCALE(elements->features.size), SCALE(15, 15), SCALE(15, 15), ImGuiWindowFlags_AlwaysVerticalScrollbar);
        {
            widgets->setup_child_anim();

            if (var->tab.current == 0) {  
                gui->begin_group();
                {
                    if (widgets->child("Aimbot")) {
                        widgets->checkbox("Enable Aimbot", &storage::aimbot);
                        std::vector<std::string> aimbot_types = { "Mouse Aim", "Camera" };
                        widgets->combo("Aimbot Type", &storage::aimbot_type, aimbot_types);
                        widgets->checkbox("Sticky Aim", &storage::sticky);
                        widgets->checkbox("Draw FOV", &storage::draw_fov);
                        HotkeyButton1(storage::aimbotkeybind.key, ChangeKey1, keystatus1);
                        if (ImGui::BeginPopupContextItem("aimbot_keybind_context_1")) {
                            if (ImGui::Selectable("Hold", storage::aimbotkeybind.type == keybind::HOLD))
                                storage::aimbotkeybind.type = keybind::HOLD;
                            if (ImGui::Selectable("Toggle", storage::aimbotkeybind.type == keybind::TOGGLE))
                                storage::aimbotkeybind.type = keybind::TOGGLE;
                            if (ImGui::Selectable("Always", storage::aimbotkeybind.type == keybind::ALWAYS))
                                storage::aimbotkeybind.type = keybind::ALWAYS;
                            ImGui::EndPopup();
                        }
                        
                        if (storage::aimbot_checks.size() < 4) {
                            storage::aimbot_checks.resize(4, false);
                        }
                        std::vector<std::string> check_options = { "Team Check", "Wall Check", "FOV Check", "KO Check" };
                        widgets->multicombo("Aimbot Checks", &storage::aimbot_checks, check_options);

                        widgets->end_child();
                    }
                }
                gui->end_group();

                gui->sameline();

                gui->begin_group();
                {
                    if (widgets->child("Aimbot Settings")) {
                        std::vector<std::string> hitboxes = { 
                            "Head", "HumanoidRootPart", "UpperTorso", "LowerTorso", 
                            "Left Hand", "Right Hand", "Left Upper Arm", "Right Upper Arm", 
                            "Left Upper Leg", "Right Upper Leg", "Left Foot", "Right Foot" 
                        };
                        widgets->combo("Target Hitbox", &storage::aimpart, hitboxes);
                        widgets->slider_float("FOV Size", &storage::fov, 1.0f, 1000.0f);
                        
                        if (storage::aimbot_type == 0) {
                            widgets->checkbox("Mouse Smooth", &globals::smoothing);
                            if (globals::smoothing) {
                                widgets->slider_float("Mouse Smoothing X", &globals::smoothingx, 1.0f, 100.0f);
                                widgets->slider_float("Mouse Smoothing Y", &globals::smoothingy, 1.0f, 100.0f);
                                widgets->slider_float("Mouse Sensitivity", &globals::aimbotSensitivity, 0.1f, 10.f);
                            }
                            widgets->checkbox("Mouse Prediction", &storage::camera_prediction);
                            if (storage::camera_prediction) {
                                widgets->slider_float("Mouse Prediction X", &storage::camera_prediction_x, 1.0f, 20.0f);
                                widgets->slider_float("Mouse Prediction Y", &storage::camera_prediction_y, 1.0f, 20.0f);
                            }
                        }
                        if (storage::aimbot_type == 1) {
                            widgets->checkbox("Camera Smoothings", &globals::smoothenable);
                            if (globals::smoothenable) {
                                widgets->slider_float("Camera Smoothing X", &globals::smoothcameraX, 1.0f, 100.0f);
                                widgets->slider_float("Camera Smoothing Y", &globals::smoothcameraY, 1.0f, 100.0f);
                            }
                            widgets->checkbox("Camera Prediction", &storage::camera_prediction);
                            if (storage::camera_prediction) {
                                widgets->slider_float("Camera Prediction X", &storage::camera_prediction_x, 1.0f, 20.0f);
                                widgets->slider_float("Camera Prediction Y", &storage::camera_prediction_y, 1.0f, 20.0f);
                            }
                        }
                        widgets->checkbox("Enable Shake", &globals::shake);
                        if (globals::shake) {
                            widgets->slider_float("Shake X Axis", &globals::shakeX, -5.f, 5.f);
                            widgets->slider_float("Shake Y Axis", &globals::shakeY, -5.f, 5.f);
                        }
                        
                        
                        widgets->end_child();
                    }
                }
                gui->end_group();
            }

            else if (var->tab.current == 1) {
                gui->begin_group();
                {
                    if (widgets->child("Silent")) {
                        widgets->checkbox("Enable Silent Aim", &storage::free_aim);
                        HotkeyButton2(storage::silentaimkeybind.key, ChangeKey2, keystatus2);
                        if (ImGui::BeginPopupContextItem("silentaim_keybind_context_1")) {
                            if (ImGui::Selectable("Hold", storage::silentaimkeybind.type == keybind::HOLD))
                                storage::silentaimkeybind.type = keybind::HOLD;
                            if (ImGui::Selectable("Toggle", storage::silentaimkeybind.type == keybind::TOGGLE))
                                storage::silentaimkeybind.type = keybind::TOGGLE;
                            if (ImGui::Selectable("Always", storage::silentaimkeybind.type == keybind::ALWAYS))
                                storage::silentaimkeybind.type = keybind::ALWAYS;
                            ImGui::EndPopup();
                        }

                        widgets->end_child();
                    }
                }
                gui->end_group();

                gui->sameline();

                gui->begin_group();
                {
                    if (widgets->child("Settings")) {
                        widgets->checkbox("Use FOV", &storage::free_aim_is_in_fov);
                        widgets->checkbox("Draw FOV", &storage::free_aim_Draw_fov);
                        widgets->slider_float("FOV Size", &storage::free_aim_fov, 1.0f, 1000.0f);

                        widgets->end_child();
                    }
                }
                gui->end_group();
            }

            else if (var->tab.current == 3) {

                gui->begin_group();
                {
                    if (widgets->child("Visuals")) {
                        static std::vector<std::string> player_filter_options = { "Everyone Except Local", "Local Player", "Team Check" };
                        widgets->combo("Player Filter", &storage::visuals::player_filter_mode, player_filter_options);
                        if (storage::visuals::player_filter_mode == 0) {
                            storage::visuals::teamcheck = false;
                            storage::visuals::allow_local_player = false;
                        } else if (storage::visuals::player_filter_mode == 1) {
                            storage::visuals::teamcheck = false;
                            storage::visuals::allow_local_player = true;
                        } else if (storage::visuals::player_filter_mode == 2) {
                            storage::visuals::teamcheck = true;
                            storage::visuals::allow_local_player = false;
                        }
                        
                        widgets->checkbox_colorpicker("Box", &storage::visuals::box, storage::box_color);
                        widgets->checkbox_colorpicker("Filled Box", &storage::fill_box, storage::box_color1);
                        widgets->checkbox("Gradient", &storage::fill_box_gradient);
                        widgets->checkbox_colorpicker("Display Username", &storage::name_esp, storage::color_5);
                        widgets->checkbox_colorpicker("Distance", &storage::visuals::distance, storage::distance_color);
                        widgets->checkbox_colorpicker("Tool", &storage::tool_esp, storage::tool_esp_color);
                        widgets->checkbox_colorpicker("Weapon Icon", &storage::weapon_icon_esp, storage::weapon_icon_color);
                        widgets->checkbox_colorpicker("Health Bar", &storage::visuals::health_bar, globals::HealtBarColor);
                        if (storage::visuals::health_bar) {
                            widgets->checkbox("Health Text", &storage::health_bar_text);
                        }
                        widgets->checkbox_colorpicker("Tracers", &storage::tracers, storage::tracers_color);
                        widgets->checkbox_colorpicker("Chams", &storage::visuals::chams, storage::chams_color, true);
                        widgets->checkbox_colorpicker("Skeleton", &storage::visuals::skeleton, storage::visuals::skeleton_color);
                        widgets->checkbox_colorpicker("Sonar", &globals::sonar_enabled, globals::sonar_color);
                        widgets->checkbox_colorpicker("Crosshair", &storage::visuals::crosshair_enabled, storage::visuals::crosshair_color);
                        widgets->checkbox_colorpicker("Fog", &storage::visuals::fog, storage::visuals::fog_color);
                        
                        widgets->end_child();
                    }
                }
                gui->end_group();

                gui->sameline();

                gui->begin_group();
                {
                    if (widgets->child("Visuals Settings")) {
                        if (storage::visuals::box) {
                            widgets->slider_float("Box Thickness", &storage::Boxthickness, 1.0f, 10.0f);
                            static std::vector<std::string> box_types = { "Box", "Corner Box" };
                            widgets->combo("Box Style", &storage::box_type, box_types);
                          
                        }
                        if (storage::tracers) {
                            static std::vector<std::string> tracer_types = { "Bottom", "Top", "Normal", "Spiderweb" };
                            widgets->combo("Tracer Type", &storage::tracer_type, tracer_types);
                        }
                        if (storage::visuals::tracers) {
                            static std::vector<std::string> snapline_modes = { "Crosshair", "Bottom", "Top" };
                            widgets->combo("SnapLine Mode", &storage::mode, snapline_modes);
                        }
                        if (storage::visuals::skeleton) {
                            widgets->slider_float("Skeleton Thickness", &storage::visuals::skeleton_thickness, 0.5f, 5.0f);
                        }
                        if (storage::fill_box_gradient) {
                            widgets->color_button("Gradient Color 1", storage::fill_box_gradient_color1);
                            widgets->color_button("Gradient Color 2", storage::fill_box_gradient_color2);
                        }
                        if (globals::sonar_enabled) {
                            widgets->slider_float("Sonar Range", &globals::sonar_range, 10.0f, 200.0f);
                            widgets->slider_float("Sonar Thickness", &globals::sonar_thickness, 0.1f, 5.0f);
                            widgets->checkbox("Show Distance", &globals::sonar_show_distance);
                            widgets->color_button("Sonar Dot Color", globals::sonar_dot_color);
                        }
                        
                        if (storage::visuals::crosshair_enabled) {
                            widgets->slider_float("Size", &storage::visuals::crosshair_size, 1.0f, 50.0f, "");
                            widgets->slider_float("Gap", &storage::visuals::crosshair_gap, 0.0f, 30.0f, "");
                            widgets->slider_float("Thickness", &storage::visuals::crosshair_thickness, 0.5f, 5.0f, "");
                            widgets->slider_float("Speed", &storage::visuals::crosshair_baseSpeed, 10.0f, 500.0f, "");
                            widgets->checkbox("Tween", &storage::visuals::crosshair_gapTween);
                            if (storage::visuals::crosshair_gapTween) {
                                widgets->slider_float("Gap Speed", &storage::visuals::crosshair_gapSpeed, 0.1f, 5.0f, "");
                            }
                            static std::vector<std::string> crosshair_styles = { "Static", "Pulse" };
                            widgets->combo("Style", &storage::visuals::crosshair_styleIdx, crosshair_styles);
                        }
                        
                        if (storage::visuals::fog) {
                            widgets->slider_float("Fog Start", &storage::visuals::fog_start, 0.0f, 1000.0f);
                            widgets->slider_float("Fog End", &storage::visuals::fog_end, storage::visuals::fog_start + 1.0f, 2000.0f);
                        }
                        
                        
                        widgets->end_child();
                    }
                }
                gui->end_group();

            }

            if (var->tab.current == 3) {
				// ESP preview removed to keep standalone menu without cheat dependencies
            }


            else if (var->tab.current == 4) {

                gui->begin_group();
                {
                    if (widgets->child("Player list")) {
                        widget_player_list();
                        widgets->end_child();
                    }
                }
                gui->end_group();

                gui->sameline();

                gui->begin_group();
                {
                    if (widgets->child("Player Option"))
                    {
                        widget_player_actions();
                        widgets->end_child();
                    }
                }
                gui->end_group();
            }


            else if (var->tab.current == 6) {  

                gui->begin_group();
                {
                    if (widgets->child("Exploits"))
                    {
                        widgets->checkbox("Bypass Jump Cooldown", &storage::nojumpcooldown);
                        widgets->checkbox("Hitbox Expander", &storage::hitbox_expander::enabled);
                        widgets->checkbox("Custom Models", &storage::custom_models_enabled);
                        widgets->checkbox("Spinbot", &storage::spinbot_enabled);

                        widgets->end_child();
                    }
                }
                gui->end_group();

                gui->sameline();

                gui->begin_group();
                {
                    if (widgets->child("Exploits Settings"))
                    {
                        if (storage::hitbox_expander::enabled) {
                            widgets->slider_float("Size X", &storage::hitbox_expander::size_x, 0.1f, 30.0f);
                            widgets->slider_float("Size Y", &storage::hitbox_expander::size_y, 0.1f, 30.0f);
                            widgets->slider_float("Size Z", &storage::hitbox_expander::size_z, 0.1f, 30.0f);
                            widgets->checkbox("Custom Models", &storage::hitbox_expander::custom_models);
                            widgets->checkbox("CanCollide", &storage::hitbox_expander::cancollide);
                            widgets->checkbox_colorpicker("Visualize Hitbox", &storage::hitbox_expander::visualize, storage::hitbox_expander::visualize_color, true);
                        }


                        widgets->end_child();
                    }
                }
                gui->end_group();
            }

            else if (var->tab.current == 10) {
                gui->begin_group();
                {
                    if (widgets->child("Movement"))
                    {
                        widgets->checkbox("Walkspeed", &storage::walkspeed);
                        widgets->checkbox("Jump Power", &storage::followsomene);
                        widgets->checkbox("Noclip", &storage::noclip);

                        widgets->end_child();
                    }
                }
                gui->end_group();

                gui->sameline();

                gui->begin_group();
                {
                    if (widgets->child("Movement Settings"))
                    {
                        if (storage::walkspeed) {
                            static std::vector<std::string> speedtype_options = { "WalkSpeed" };
                            widgets->combo("Speed Type", &storage::speedtype, speedtype_options);
                            widgets->slider_float("Speed Hack", &storage::walkspeed_speed, 1.0f, 1000.0f);
                            HotkeyButtonSpeed(storage::speedkeybind.key, ChangeKeySpeed, keystatusSpeed);
                            
                            if (ImGui::BeginPopupContextItem("speed_keybind_context_1")) {
                                if (ImGui::Selectable("Toggle", storage::walkspeed_mode == 0))
                                    storage::walkspeed_mode = 0;
                                if (ImGui::Selectable("Always", storage::walkspeed_mode == 1))
                                    storage::walkspeed_mode = 1;
                                if (ImGui::Selectable("Hold", storage::walkspeed_mode == 2))
                                    storage::walkspeed_mode = 2;
                                ImGui::EndPopup();
                            }
                        }

                        if (storage::followsomene) {
                            widgets->slider_float("Power", &storage::JumpPower, 1.0f, 1000.0f);
                            HotkeyButtonJump(storage::jumppowerkeybind.key, ChangeKeyJump, keystatusJump);
                            
                            if (ImGui::BeginPopupContextItem("jump_keybind_context_1")) {
                                if (ImGui::Selectable("Toggle", storage::jumppower_mode == 0))
                                    storage::jumppower_mode = 0;
                                if (ImGui::Selectable("Always", storage::jumppower_mode == 1))
                                    storage::jumppower_mode = 1;
                                if (ImGui::Selectable("Hold", storage::jumppower_mode == 2))
                                    storage::jumppower_mode = 2;
                                ImGui::EndPopup();
                            }
                        }

                        if (storage::noclip) {
                            HotkeyButtonNoclip(storage::noclipkeybind.key, ChangeKeyNoclip, keystatusNoclip);
                            
                            if (ImGui::BeginPopupContextItem("noclip_keybind_context_1")) {
                                if (ImGui::Selectable("Toggle", storage::noclip_mode == 0))
                                    storage::noclip_mode = 0;
                                if (ImGui::Selectable("Always", storage::noclip_mode == 1))
                                    storage::noclip_mode = 1;
                                if (ImGui::Selectable("Hold", storage::noclip_mode == 2))
                                    storage::noclip_mode = 2;
                                ImGui::EndPopup();
                            }
                        }

                        widgets->end_child();
                    }
                }
                gui->end_group();
            }


            else if (var->tab.current == 11) {
                gui->begin_group();
                {
                    if (widgets->child("Client Sided"))
                    {
                        widgets->checkbox("Headless", &storage::headless);
                        widgets->checkbox("Korblox", &storage::korblox);
                        widgets->checkbox("Emote Changer", &storage::emote_changer);
                        widgets->checkbox("Animation Changer", &storage::animation_changer);
                        
                        if (storage::gameid == 1008451066) {
                            widgets->checkbox("Skin Changer [DH]", &storage::skin_changer_enabled);
                        }

                        widgets->end_child();
                    }
                }
                gui->end_group();

                gui->sameline();

                gui->begin_group();
                {
                    if (widgets->child("Client Sided Settings"))
                    {
                        if (storage::emote_changer) {
                            static std::vector<std::string> emote_types = { 
                                "cha cha", 
                                "team usa breaking emote", 
                                "d4vd - backflip", 
                                "/e hidden", 
                                "backshots", 
                                "receive backshots", 
                                "gangnam style", 
                                "California Girl Dance", 
                                "No animation", 
                                "Huda's Signature Move Dance" 
                            };
                            widgets->combo("Emote Type", &storage::emote_id, emote_types);
                        }

                        if (storage::animation_changer) {
                            static std::vector<std::string> idle_animations = {
                                "Default Idle 1", "Default Idle 2", "Zombie Idle 1", "Zombie Idle 2", 
                                "Ninja Idle 1", "Ninja Idle 2", "Robot Idle 1", "Robot Idle 2",
                                "Levitation Idle 1", "Levitation Idle 2", "Stylish Idle 1", "Stylish Idle 2",
                                "Cartoony Idle 1", "Cartoony Idle 2", "Super Hero Idle 1", "Super Hero Idle 2",
                                "Elder Idle 1", "Elder Idle 2", "Toy Idle 1", "Toy Idle 2", "Old School Idle 1", "Old School Idle 2"
                            };
                            widgets->combo("Idle Animation", &storage::idle_animation, idle_animations);
                            
                            static std::vector<std::string> run_animations = {
                                "Default Run", "Zombie Run", "Ninja Run", "Robot Run", 
                                "Levitation Run", "Stylish Run", "Cartoony Run", "Super Hero Run",
                                "Elder Run", "Toy Run", "Old School Run"
                            };
                            widgets->combo("Run Animation", &storage::run_animation, run_animations);
                            
                            static std::vector<std::string> walk_animations = {
                                "Default Walk", "Zombie Walk", "Ninja Walk", "Robot Walk", 
                                "Levitation Walk", "Stylish Walk", "Cartoony Walk", "Super Hero Walk",
                                "Elder Walk", "Toy Walk", "Old School Walk"
                            };
                            widgets->combo("Walk Animation", &storage::walk_animation, walk_animations);
                            
                            static std::vector<std::string> jump_animations = {
                                "Default Jump", "Zombie Jump", "Ninja Jump", "Robot Jump", 
                                "Levitation Jump", "Stylish Jump", "Cartoony Jump", "Super Hero Jump",
                                "Elder Jump", "Toy Jump", "Old School Jump"
                            };
                            widgets->combo("Jump Animation", &storage::jump_animation, jump_animations);
                            
                            static std::vector<std::string> fall_animations = {
                                "Default Fall", "Zombie Fall", "Ninja Fall", "Robot Fall", 
                                "Levitation Fall", "Stylish Fall", "Cartoony Fall", "Super Hero Fall",
                                "Elder Fall", "Toy Fall", "Old School Fall"
                            };
                            widgets->combo("Fall Animation", &storage::fall_animation, fall_animations);
                        }

                        if (storage::gameid == 1008451066) {
                            if (storage::skin_changer_enabled) {
                                static std::vector<std::string> skin_types = {
                                    "Galaxy", "Inferno", "Matrix", "RedDeath", "GoldGlory"
                                };
                                widgets->combo("Double-Barrel SG", &storage::skin_changer_doublebarrel, skin_types);
                                widgets->combo("Revolver", &storage::skin_changer_revolver, skin_types);
                                widgets->combo("TacticalShotgun", &storage::skin_changer_tacticalshotgun, skin_types);
                                widgets->combo("AK47", &storage::skin_changer_ak47, skin_types);
                                widgets->combo("SMG", &storage::skin_changer_smg, skin_types);
                                widgets->combo("LMG", &storage::skin_changer_lmg, skin_types);
                            }
                        }

                        widgets->end_child();
                    }
                }
                gui->end_group();
            }

            else if (var->tab.current == 7) {

                gui->begin_group();
                {
                    if (widgets->child("Settings"))
                    {
                        widgets->checkbox("Vsync", &storage::vsync);
                        widgets->checkbox("Streamproof", &storage::streamproof);
                        widgets->checkbox("DEX Explorer", &storage::dex_explorer_enabled);
                        widgets->checkbox("Watermark", &storage::watermark_enabled);
                        widgets->keybind_button("Menu Keybind", &storage::menukeybind.key);
                        
                        std::vector<std::string> performance_options = { "High", "Potato" };
                        widgets->combo("Performance", &globals::performance_mode, performance_options);
                        
                        widgets->end_child();
                    }
                }
                gui->end_group();


                gui->sameline();

                gui->begin_group();
                {
                    if (widgets->child("Configs"))
                    {
                        static int selectedConfigIndex = 0;
                        static std::vector<std::string> configsList;
                        static bool configNameEdited = false;

                        if (configsList.empty())
                            LoadConfigFiles(configsList);

                        if (!configsList.empty())
                        {
                            widgets->combo("Select Config", &selectedConfigIndex, configsList);

                            if (!configNameEdited)
                            {
                                strncpy(configname, configsList[selectedConfigIndex].c_str(), IM_ARRAYSIZE(configname));
                                configname[IM_ARRAYSIZE(configname) - 1] = '\0';
                            }
                        }

                        if (widgets->input_text("Config Name", configname, IM_ARRAYSIZE(configname)))
                            configNameEdited = true;

                        if (widgets->button("Load"))
                        {
                            Engine::configs::load(configname);
                            configNameEdited = false;

                            auto it = std::find(configsList.begin(), configsList.end(), std::string(configname));
                            selectedConfigIndex = (it != configsList.end())
                                ? static_cast<int>(std::distance(configsList.begin(), it))
                                : -1;
                        }
                        if (widgets->button("Save"))
                        {
                            Engine::configs::save(configname);
                            LoadConfigFiles(configsList);
                            configNameEdited = false;

                            auto it = std::find(configsList.begin(), configsList.end(), std::string(configname));
                            selectedConfigIndex = (it != configsList.end())
                                ? static_cast<int>(std::distance(configsList.begin(), it))
                                : -1;
                        }
                        if (widgets->button("Refresh"))
                        {
                            LoadConfigFiles(configsList);
                            configNameEdited = false;

                            selectedConfigIndex = !configsList.empty() ? 0 : -1;
                            if (selectedConfigIndex >= 0)
                            {
                                strncpy(configname, configsList[selectedConfigIndex].c_str(), IM_ARRAYSIZE(configname));
                                configname[IM_ARRAYSIZE(configname) - 1] = '\0';
                            }
                        }

                        widgets->end_child();
                    }
                }
                gui->end_group();
            }

                }
                gui->end_content();

            gui->pop_var();

            notify->setup_notify();
        }
        gui->end();
    }