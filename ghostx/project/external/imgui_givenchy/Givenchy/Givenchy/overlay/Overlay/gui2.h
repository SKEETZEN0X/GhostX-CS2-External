#pragma once
#include <stack>
#include <unordered_set>
#include <filesystem> 
#include <vector>
#include <string>
#include "../xorstr/xorstr.hpp"
#include <unordered_map>

#include "../../source/configs/configs.hpp"
#include "../imgui/imgui.h"
#include "../../source/storage/Globals.hpp"
#include "../gui/settings/functions.h"

inline std::string GetConfigFolderPath()
{
    std::string configFolderPath = Engine::appdata_path() + "/Givenchy/configs";

    if (!std::filesystem::exists(configFolderPath))
    {
        std::filesystem::create_directories(configFolderPath);
    }

    return configFolderPath;
}
inline std::vector<std::string> configFiles;
inline std::string configFolderPath = GetConfigFolderPath();
inline void RenderExplorer() {
    ImGui::Text("Explorer removed in standalone menu.");
}

inline void LoadConfigFiles(std::vector<std::string>& configsList)
{
    configsList.clear();

    std::string configDir = Engine::appdata_path() + "/Givenchy/configs";

    for (const auto& entry : std::filesystem::directory_iterator(configDir))
    {
        if (!entry.is_regular_file())
            continue;

        if (entry.path().extension() == ".cfg")
        {
            configsList.push_back(entry.path().stem().string());
        }
    }
}
inline char configname[100];
inline void ShowConfigWindow()
{
    if (ImGui::BeginChild("Configs", ImVec2(300, 200), true))
    {
        static int selectedConfigIndex = -1;
        static std::vector<std::string> configsList;

        if (configsList.empty())
            LoadConfigFiles(configsList);

        if (!configsList.empty())
        {
            ImGui::SetNextItemWidth(200.0f);

            if (ImGui::Combo("Select Config", &selectedConfigIndex,
                [](void* data, int idx, const char** out_text) {
                    auto& items = *static_cast<std::vector<std::string>*>(data);
                    if (idx < 0 || idx >= (int)items.size())
                        return false;
                    *out_text = items[idx].c_str();
                    return true;
                }, &configsList, (int)configsList.size()))
            {
                strncpy(configname, configsList[selectedConfigIndex].c_str(), IM_ARRAYSIZE(configname));
                configname[IM_ARRAYSIZE(configname) - 1] = '\0';
            }
        }

        ImGui::InputText("Config Name", configname, IM_ARRAYSIZE(configname));

        ImGui::Text("Current config name: %s", configname);

        if (ImGui::Button("Load"))
        {
            if (configname[0] != '\0')
            {
                Engine::configs::load(configname);
                ImGui::Text("Loaded config: %s", configname);
            }
            else
            {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Please select or enter a config name.");
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Save"))
        {
            if (configname[0] != '\0')
            {
                Engine::configs::save(configname);
                LoadConfigFiles(configsList);
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Refresh"))
        {
            LoadConfigFiles(configsList);
        }

        ImGui::EndChild();
    }
}
inline void widget_player_list() { ImGui::Text("Player list disabled in standalone."); }
inline void widget_player_actions() { ImGui::Text("Player actions disabled in standalone."); }
inline static int keystatus1 = 0;
inline static bool Items_ArrayGetter(void* data, int idx, const char** out_text)
{
    const char* const* items = (const char* const*)data;
    if (out_text)
        *out_text = items[idx];
    return true;
}
inline void ChangeKey1(void*) {
    keystatus1 = 1;

    while (true) {
        for (int i = 1; i < 256; ++i) {
            if (GetAsyncKeyState(i) & 0x8000) {
                storage::aimbotkeybind.key = i;
                keystatus1 = 0;
                return;
            }
        }
        Sleep(1);
    }
}
static const char* keyNames1[] =
{

    "Keybind",
    "Left Mouse",
    "Right Mouse",
    "Cancel",
    "Middle Mouse",
    "Mouse 5",
    "Mouse 4",
    "",
    "Backspace",
    "Tab",
    "",
    "",
    "Clear",
    "Enter",
    "",
    "",
    "Shift",
    "Control",
    "Alt",
    "Pause",
    "Caps",
    "",
    "",
    "",
    "",
    "",
    "",
    "Escape",
    "",
    "",
    "",
    "",
    "Space",
    "Page Up",
    "Page Down",
    "End",
    "Home",
    "Left",
    "Up",
    "Right",
    "Down",
    "",
    "",
    "",
    "Print",
    "Insert",
    "Delete",
    "",
    "0",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "A",
    "B",
    "C",
    "D",
    "E",
    "F",
    "G",
    "H",
    "I",
    "J",
    "K",
    "L",
    "M",
    "N",
    "O",
    "P",
    "Q",
    "R",
    "S",
    "T",
    "U",
    "V",
    "W",
    "X",
    "Y",
    "Z",
    "",
    "",
    "",
    "",
    "",
    "Numpad 0",
    "Numpad 1",
    "Numpad 2",
    "Numpad 3",
    "Numpad 4",
    "Numpad 5",
    "Numpad 6",
    "Numpad 7",
    "Numpad 8",
    "Numpad 9",
    "Multiply",
    "Add",
    "",
    "Subtract",
    "Decimal",
    "Divide",
    "F1",
    "F2",
    "F3",
    "F4",
    "F5",
    "F6",
    "F7",
    "F8",
    "F9",
    "F10",
    "F11",
    "F12",
};
inline void HotkeyButton1(int aimkey1, void* changekey1, int status)
{
    const char* preview_value = NULL;
    if (aimkey1 >= 0 && aimkey1 < IM_ARRAYSIZE(keyNames1))
        Items_ArrayGetter(keyNames1, aimkey1, &preview_value);

    std::string aimkeys;
    if (preview_value == NULL)
        aimkeys = ("Aimbot key (press)");
    else
        aimkeys = preview_value;

    if (status == 1)
    {
        aimkeys = ("Press the Key");
    }
    if (status == 1) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
    }
    else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    }

    if (widgets->button(aimkeys.c_str()))
    {
        if (status == 0)
        {
            CreateThread(0, 0, (LPTHREAD_START_ROUTINE)changekey1, nullptr, 0, nullptr);
        }
    }

    ImGui::PopStyleColor(2);
}











inline static int keystatus2 = 0;
inline static bool Items_ArrayGetter2(void* data, int idx, const char** out_text)
{
    const char* const* items = (const char* const*)data;
    if (out_text)
        *out_text = items[idx];
    return true;
}
inline void ChangeKey2(void*) {
    keystatus2 = 1;
    while (true) {
        for (int i = 1; i < 256; ++i) {
            if (GetAsyncKeyState(i) & 0x8000) {
                storage::silentaimkeybind.key = i;
                keystatus2 = 0;
                return;
            }
        }
        Sleep(1);
    }
}
static const char* keyNames2[] =
{
    "Keybind",
    "Left Mouse",
    "Right Mouse",
    "Cancel",
    "Middle Mouse",
    "Mouse 5",
    "Mouse 4",
    "",
    "Backspace",
    "Tab",
    "",
    "",
    "Clear",
    "Enter",
    "",
    "",
    "Shift",
    "Control",
    "Alt",
    "Pause",
    "Caps",
    "",
    "",
    "",
    "",
    "",
    "",
    "Escape",
    "",
    "",
    "",
    "",
    "Space",
    "Page Up",
    "Page Down",
    "End",
    "Home",
    "Left",
    "Up",
    "Right",
    "Down",
    "",
    "",
    "",
    "Print",
    "Insert",
    "Delete",
    "",
    "0",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "A",
    "B",
    "C",
    "D",
    "E",
    "F",
    "G",
    "H",
    "I",
    "J",
    "K",
    "L",
    "M",
    "N",
    "O",
    "P",
    "Q",
    "R",
    "S",
    "T",
    "U",
    "V",
    "W",
    "X",
    "Y",
    "Z",
    "",
    "",
    "",
    "",
    "",
    "Numpad 0",
    "Numpad 1",
    "Numpad 2",
    "Numpad 3",
    "Numpad 4",
    "Numpad 5",
    "Numpad 6",
    "Numpad 7",
    "Numpad 8",
    "Numpad 9",
    "Multiply",
    "Add",
    "",
    "Subtract",
    "Decimal",
    "Divide",
    "F1",
    "F2",
    "F3",
    "F4",
    "F5",
    "F6",
    "F7",
    "F8",
    "F9",
    "F10",
    "F11",
    "F12",
};
inline void HotkeyButton2(int aimkey2, void* changekey2, int status)
{
    const char* preview_value = NULL;
    if (aimkey2 >= 0 && aimkey2 < IM_ARRAYSIZE(keyNames2))
        Items_ArrayGetter2(keyNames2, aimkey2, &preview_value);

    std::string aimkeys;
    if (preview_value == NULL)
        aimkeys = ("Silent Aim Key (press)");
    else
        aimkeys = preview_value;

    if (status == 1)
    {
        aimkeys = ("Press the Key");
    }

    if (status == 1) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
    }
    else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    }

    if (widgets->button(aimkeys.c_str()))
    {
        if (status == 0)
        {
            CreateThread(0, 0, (LPTHREAD_START_ROUTINE)changekey2, nullptr, 0, nullptr);
        }
    }

    ImGui::PopStyleColor(2);
}































inline static int keystatusSpeed = 0;
inline void ChangeKeySpeed(void* blank)
{
    keystatusSpeed = 1;

    while (true)
    {
        for (int i = 1; i < 256; ++i)
        {
            if (GetAsyncKeyState(i) & 0x8000)
            {
                storage::speedkeybind.key = i;
                keystatusSpeed = 0;
                return;
            }
        }
        Sleep(1);
    }
}
inline void HotkeyButtonSpeed(int key, void* changekeyfunc, int status)
{
    const char* preview_value = nullptr;
    if (key >= 0 && key < IM_ARRAYSIZE(keyNames1))
        preview_value = keyNames1[key];

    std::string button_text = preview_value ? preview_value : "TriggerBot key (press)";
    if (status == 1)
        button_text = "Press the Key";

    if (status == 1) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.2f, 0.2f, 0.2f, 1));
    }
    else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.5f, 0.5f, 0.5f, 1));
    }

    if (widgets->button(button_text.c_str()))
    {
        if (status == 0)
        {
            CreateThread(0, 0, (LPTHREAD_START_ROUTINE)changekeyfunc, nullptr, 0, nullptr);
        }
    }

    ImGui::PopStyleColor(2);
}











inline static int keystatusJump = 0;
inline void ChangeKeyJump(void* blank)
{
    keystatusJump = 1;

    while (true)
    {
        for (int i = 1; i < 256; ++i)
        {
            if (GetAsyncKeyState(i) & 0x8000)
            {
                storage::jumppowerkeybind.key = i;
                keystatusJump = 0;
                return;
            }
        }
        Sleep(1);
    }
}
inline void HotkeyButtonJump(int key, void* changekeyfunc, int status)
{
    const char* preview_value = nullptr;
    if (key >= 0 && key < IM_ARRAYSIZE(keyNames1))
        preview_value = keyNames1[key];

    std::string button_text = preview_value ? preview_value : "TriggerBot key (press)";
    if (status == 1)
        button_text = "Press the Key";

    if (status == 1) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.2f, 0.2f, 0.2f, 1));
    }
    else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.5f, 0.5f, 0.5f, 1));
    }

    if (widgets->button(button_text.c_str()))
    {
        if (status == 0)
        {
            CreateThread(0, 0, (LPTHREAD_START_ROUTINE)changekeyfunc, nullptr, 0, nullptr);
        }
    }

    ImGui::PopStyleColor(2);
}











inline void HotkeyButtonFly(int key, void* changekeyfunc, int status)
{
    const char* preview_value = nullptr;
    if (key >= 0 && key < IM_ARRAYSIZE(keyNames1))
        preview_value = keyNames1[key];

    std::string button_text = preview_value ? preview_value : "TriggerBot key (press)";
    if (status == 1)
        button_text = "Press the Key";

    if (status == 1) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.2f, 0.2f, 0.2f, 1));
    }
    else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.5f, 0.5f, 0.5f, 1));
    }

    if (widgets->button(button_text.c_str()))
    {
        if (status == 0)
        {
            CreateThread(0, 0, (LPTHREAD_START_ROUTINE)changekeyfunc, nullptr, 0, nullptr);
        }
    }

    ImGui::PopStyleColor(2);
}

inline static int keystatusNoclip = 0;
inline void ChangeKeyNoclip(void* blank)
{
    keystatusNoclip = 1;

    while (true)
    {
        for (int i = 1; i < 256; ++i)
        {
            if (GetAsyncKeyState(i) & 0x8000)
            {
                storage::noclipkeybind.key = i;
                keystatusNoclip = 0;
                return;
            }
        }
        Sleep(1);
    }
}
inline void HotkeyButtonNoclip(int key, void* changekeyfunc, int status)
{
    const char* preview_value = nullptr;
    if (key >= 0 && key < IM_ARRAYSIZE(keyNames1))
        preview_value = keyNames1[key];

    std::string button_text = preview_value ? preview_value : "Noclip key (press)";
    if (status == 1)
        button_text = "Press the Key";

    if (status == 1) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.2f, 0.2f, 0.2f, 1));
    }
    else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.5f, 0.5f, 0.5f, 1));
    }

    if (widgets->button(button_text.c_str()))
    {
        if (status == 0)
        {
            CreateThread(0, 0, (LPTHREAD_START_ROUTINE)changekeyfunc, nullptr, 0, nullptr);
        }
    }

    ImGui::PopStyleColor(2);
}










