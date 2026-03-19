#pragma once

#include "../../overlay/ckeybind/keybind.hpp"
#include <unordered_map>
#include <vector>
#include <mutex>

namespace globals
{
    inline float r3dboxcolor[3] = { 1.0f, 1.0f, 1.0f };
    inline float box_filled_color[3] = { 1.0f, 1.0f, 1.0f };
    inline bool phantomforceteamcheck = false;
    inline bool r3dbox = false;
    inline bool g_showLobbyWarning = false;
    inline bool frontlineteamcheck = true;
    inline bool badbuisnessteamcheck = true;
    inline bool radar_enabled = false;
    inline float radar_pos_x = 20.0f;
    inline float radar_pos_y = 60.0f;
    inline bool radar_show_distance = false;
    inline float radar_size = 180.0f;
    inline ImVec2 radar_position = ImVec2(20, 60);
    inline bool sonar_enabled = false;
    inline float sonar_range = 50.0f;
    inline float sonar_thickness = 0.3f;
    inline float sonar_color[4] = { 1.0f, 1.0f, 1.0f, 0.8f };
    inline float sonar_dot_color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
    inline bool sonar_show_distance = false;
    inline bool sonar_dot_color_dummy = true;
    inline float smoothcameraX = 5.f;
    inline float smoothcameraY = 5.f;
    inline float smoothingx = 5;
    inline bool smoothing = false;
    inline float smoothingy = 5;
    inline bool smoothenable = false;
    inline int TypeTextName = 1;
    inline int TypeTextDistance = 0;
    inline int TypeTextTool = 0;
    inline int TypeTextRigType = 0;
    inline int TypeHealthBar = 2;
 
    inline std::vector<std::string> ESPFieldOrder = {"tool", "distance", "name"};
    inline float HealtBarColor[3] = { 0.0f, 1.0f, 0.0f };
    inline float snipline[3] = { 1.0f, 1.0f, 1.0f };

    inline std::unordered_map<std::string, int> playerStatuses;
    inline bool skipFriendlyESP = true;
    inline std::vector<std::string> whitelist;
    inline std::vector<std::string> blacklist;
    inline bool IsPlayerFriendly(const std::string& playerName) {
        auto it = playerStatuses.find(playerName);
        return (it != playerStatuses.end() && it->second == 1);
    }
    inline int GetPlayerStatus(const std::string& playerName) {
        auto it = playerStatuses.find(playerName);
        return (it != playerStatuses.end()) ? it->second : 0;
    }
    inline void SetPlayerStatus(const std::string& playerName, int status) {
        playerStatuses[playerName] = status;
    }

    inline float shakeX = 3.0f;  
    inline float shakeY = 3.0f;  

    inline bool beizer = false;
    inline bool blatant = false;
    inline bool healtbartext = false;
    inline bool flickbot = false;
    inline bool shake = false;
    inline float aimbotSensitivity = 1.0f; 

    inline bool headESP = false;
    inline int performance_mode = 0;

    inline bool teamcheck = false;
    inline bool fov_check = false;
    inline bool ko_check = false;

    namespace rage {
        inline bool exploits = false;
        inline bool spreadremove = false;
        inline float spreadamount = 0.0f;
    }

}

struct storage {
    static inline int speedtype = 0;
    static inline float thickness = 2.0f;
    static inline float Boxthickness = 1.0f;
    static inline bool streamble = false;
    static inline bool streamproof = false;
    static inline bool vsync = true;
    static inline bool playerlist = false;
    static inline std::uint64_t mouse_service{};
    static inline int_fast64_t gameid = 0;
    static inline int_fast64_t placeid = 0;

    static inline int box_type = 0;
    static inline int mode = 0;
    static inline float glow_size = 30.0f;
    
    static inline bool glow_box = false;
    static inline float glow_box_intensity = 15.0f;
    static inline float glow_box_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    static inline float color_1[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    static inline float color_2[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    static inline float color_5[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    static inline float dot_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    static inline float box_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    static inline float box_color1[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    static inline float distance_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    static inline float tool_esp_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    static inline float glow_col_dist[3] = { 1.0f, 1.0f, 1.0f };
    static inline float health_bar_color[3] = { 1.0f, 1.0f, 1.0f };
    static inline float chams_color[4] = { 0.0f, 0.0f, 0.0f, 0.42f };

    static inline float silent_prediction_y = 1.0f;
    static inline float silent_prediction_x = 1.0f;
    static inline bool silent_prediction = false;
    static inline float prediction_x = 1.0f;
    static inline float prediction_y = 1.0f;
    static inline bool prediction = false;

    static inline bool aimbot = false;
    static inline bool aimbot_teamcheck = false;
    static inline bool aimbot_fov_check = true;
    static inline bool use_fov = true;
    static inline bool aimbot_knockcheck = false;
    
    static inline int aimpart = 0;
    static inline int aimbot_mode = 0;
    static inline int aimbot_type = 0;
    static inline int aimbot_easing_style = 0;
    static inline bool sticky = false;
    static inline bool disable_outside_fov = true;
    static inline float fov = 100.0f;
    static inline bool draw_fov = false;
    static inline float hex_fov_rotation = 0.0f;
    static inline float hex_fov_rotation_speed = 0.1f;
    static inline bool wall_check = false;
    static inline float sensitivity = 1.0f;
    static inline float smoothness_x = 1.0f;
    static inline float smoothness_y = 1.0f;
    static inline bool closest_part = false;
    static inline float max_aimbot_distance = 1000.0f;
    static inline bool max_dist = false;
    static inline bool dis_check = false;

    static inline bool free_aim = false;
    static inline bool free_aim_sticky = false;
    static inline bool silentaim_spoof = false;
    static inline bool silentaim_closestpart = false;
    static inline bool free_aim_is_in_fov = false;
    static inline int free_aim_part = 0;
    static inline float free_aim_fov = 200.0f;
    static inline float free_aim_fov_size;
    static inline bool free_aim_Draw_fov = false;
    static inline float free_aim_fov_rotation = 0.0f;
    static inline float free_aim_fov_rotation_speed = 0.1f;
    static inline bool silent_grabbedcheck = false;
    static inline bool silent_teamcheck = false;


    static inline bool healthcheck = false;
    static inline bool knockcheck = false;
    static inline bool knockhecksilent = false;
    static inline bool knifecheck = false;
    static inline bool localplayercheck = false;
    static inline bool HealthBasedColor = false;
    static inline bool health_bar_text = false;
    static inline float health_x = 29.874f;
    static inline float health_y = 0.0f;
    static inline bool enable_health_glow = false;

    static inline bool name_esp = false;
    static inline bool tool_esp = false;
    static inline bool weapon_icon_esp = false;
    static inline float weapon_icon_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    static inline bool offscreen_Check = true;
    static inline bool fill_box = false;
    static inline bool fill_box_gradient = false;
    static inline float fill_box_gradient_color1[4] = { 1.0f, 1.0f, 1.0f, 0.3f }; // White gradient start
    static inline float fill_box_gradient_color2[4] = { 0.2f, 0.2f, 0.2f, 0.3f }; // Light black gradient end
    static inline float alpha = 1.0f;

    static inline bool walkspeed = false;
    static inline float walkspeedvalue = 16.0f;
    static inline float walkspeed_speed = 16.0f;
    static inline int walkspeed_mode = 0; // 0 = toggle, 1 = always, 2 = hold
    static inline bool nojumpcooldown = false;
    static inline float jumppower = 50.0f;
    static inline float JumpPower = 50.0f;
    static inline int jumppower_mode = 0; // 0 = toggle, 1 = always, 2 = hold
    static inline bool cframe = false;

    static inline bool followsomene = false;
    
    static inline bool korblox = false;
    static inline bool skin_changer_enabled = false;
    static inline int skin_changer_doublebarrel = 0;
    static inline int skin_changer_revolver = 0;
    static inline int skin_changer_tacticalshotgun = 0;
    static inline int skin_changer_ak47 = 0;
    static inline int skin_changer_smg = 0;
    static inline int skin_changer_lmg = 0;
    
    static inline bool bladeball_autoparry = false;

    static inline bool camera_prediction = false;
    static inline bool resolver = false;
    static inline float camera_prediction_x = 1.0f;
    static inline float camera_prediction_y = 1.0f;
    static inline float mouse_sensitivity = 1.0f;
    static inline float mouse_smoothness = 1.0f;
    static inline float smoothness_camera = 100.0f;

    static inline bool shake = false;
    static inline float shake_x = 0.0f;
    static inline float shake_y = 0.0f;
    static inline bool chams_rainbow = false;
    static inline int chamstype = 0;



    static inline bool tracers = false;
    static inline int tracer_type = 0;
    static inline float tracers_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    static inline bool hsuis = false;
    static inline bool headless = false;


    static inline bool custom_models_enabled = false;

    struct hitbox_expander {
        static inline bool enabled = false;
        static inline bool custom_models = false;
        static inline bool cancollide = false;
        static inline float size_x = 2.2f;
        static inline float size_y = 2.2f;
        static inline float size_z = 1.2f;
        static inline bool visualize = false;
        static inline float visualize_color[4] = { 1.0f, 0.0f, 0.0f, 0.3f };
    };


    static inline bool spinbot_enabled = false;

    static inline bool noclip = false;
    static inline int noclip_mode = 0; // 0 = Toggle, 1 = Always, 2 = Hold

    static inline bool emote_changer = false;
    static inline int emote_id = 0;

    static inline bool animation_changer = false;
    static inline int animation_type = 0;
    
    static inline int idle_animation = 0;
    static inline int run_animation = 0;
    static inline int walk_animation = 0;
    static inline int jump_animation = 0;
    static inline int fall_animation = 0;

    static inline keybind speedkeybind{ "speedkeybind" };
    static inline keybind jumppowerkeybind{ "jumppowerkeybind" };
    static inline keybind flightkeybind{ "flightkeybind" };
    static inline keybind noclipkeybind{ "noclipkeybind" };
    
    static inline bool keybind_types_initialized = false;
    static inline keybind aimbotkeybind{ "aimbotkeybind" };
    static inline keybind silentaimkeybind{ "silentaimkeybind" };
    static inline keybind menukeybind{ "menukeybind" };
    static inline int menukeybind_default_key = VK_INSERT;
    
    static inline void init_menu_keybind() {
        if (menukeybind.key == 0) {
            menukeybind.key = menukeybind_default_key;
        }
    }

    struct visuals {
        static inline bool head_dot = false;
        static inline bool box = false;
        static inline int name_display_mode = 0;
        static inline bool tracers = false;
        static inline bool tracers_style = false;
        static inline bool name = false;
        static inline bool distance = false;
        static inline bool teamcheck = false;
        static inline bool chams = false;
        static inline bool health_bar = false;
        static inline bool sonar_waves = false;
        static inline bool sonar = false;
        static inline bool china_rotation = false;
        static inline bool china_rainbow = false;
        static inline bool fog = false;
        static inline float fog_start = 0.0f;
        static inline float fog_end = 1000.0f;
        static inline float fog_color[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
        
        static inline bool crosshair_enabled = false;
        static inline float crosshair_color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
        static inline float crosshair_size = 10.0f;
        static inline float crosshair_gap = 5.0f;
        static inline bool crosshair_gapTween = false;
        static inline float crosshair_gapSpeed = 1.0f;
        static inline float crosshair_thickness = 1.0f;
        static inline int crosshair_styleIdx = 0;
        static inline float crosshair_baseSpeed = 100.0f;
        static inline float crosshair_fadeDuration = 1.0f;
        static inline bool death_check = false;
        static inline bool allow_local_player = false;
        static inline int player_filter_mode = 0; // 0 = Everyone Except Local, 1 = Local Player, 2 = Team Check
        static inline bool rig_type = false;
        static inline bool skeleton = false;
        static inline float skeleton_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        static inline float skeleton_thickness = 0.8f;
        
    };

    static inline std::mutex custom_players_mutex;
    
    static inline std::vector<bool> aimbot_checks = { false, false, false }; // Team Check, Wall Check, FOV Check
    
    static inline bool dex_explorer_enabled = false;
    static inline bool watermark_enabled = false;
};