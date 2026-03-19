#include "addon.hpp"
#include <string>

#include "imgui.h"
#include "imgui_internal.h"

#include <map>
#include <string>
#include <iostream>
using namespace ImGui;
using std::string;
bool render::tab(const char* label, int* v, int current_id, const ImVec2& size_arg)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = ImGui::CalcTextSize(label, nullptr, true);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + 20.0f, label_size.y + 8.0f);
    const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));

    ImGui::ItemSize(size, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool active = (*v == current_id);
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);
    if (pressed) *v = current_id;

    // Colors
    ImU32 bgColor; 
    ImU32 borderColor = IM_COL32(80, 80, 80, 255); 
    ImU32 textColor = IM_COL32(220, 220, 220, 255); 


    ImVec4 childBgColor = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
   
    window->DrawList->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), ImGui::ColorConvertFloat4ToU32(childBgColor));
    float thickness = 1.0f;
    if (active)
    {
        bgColor = GetColorU32(childBgColor);
        textColor = IM_COL32(100, 255, 100, 255);

        // Draw top, left, right (no bottom — looks "connected" to child)
        window->DrawList->AddLine(ImVec2(pos.x, pos.y), ImVec2(pos.x + size.x, pos.y), borderColor, thickness); // top
        window->DrawList->AddLine(ImVec2(pos.x, pos.y), ImVec2(pos.x, pos.y + size.y), borderColor, thickness); // left
        window->DrawList->AddLine(ImVec2(pos.x + size.x, pos.y), ImVec2(pos.x + size.x, pos.y + size.y), borderColor, thickness); // right
    }
    else
    {
        // Inactive tab = full rectangle border
        window->DrawList->AddRect(bb.Min, bb.Max, borderColor, 0.0f, 0, thickness);
        bgColor = IM_COL32(80, 80, 80, 255);
    }


    ImVec2 text_pos = ImVec2(
        pos.x + (size.x - label_size.x) * 0.5f,
        pos.y + (size.y - label_size.y) * 0.5f
    );
    window->DrawList->AddText(text_pos, textColor, label);

    return pressed;
}

bool render::check(const char* label, bool* v)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    const float square_sz = GetFrameHeight();
    const ImVec2 pos = window->DC.CursorPos;
    ImVec2 bb_max = ImVec2(
        pos.x + square_sz + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f),
        pos.y + label_size.y + style.FramePadding.y * 2.0f
    );
    const ImRect bb(pos, bb_max);

    ItemSize(bb, style.FramePadding.y);
    if (!ItemAdd(bb, id))
        return false;

    float height = g.FontSize * 1.0f;
    ImVec2 size = ImVec2(height, height);

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);
    if (pressed) *v = !*v;

    ImVec4 target = *v ? ImGui::ColorConvertU32ToFloat4(GetColorU32(ImGuiCol_SliderGrab))
        : ImGui::ColorConvertU32ToFloat4(GetColorU32(ImGuiCol_WindowBg));

    static std::map<ImGuiID, ImVec4> anim;
    if (anim.find(id) == anim.end()) anim[id] = target;
    anim[id] = ImLerp(anim[id], target, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);

    // Shadow
    ImVec2 shadow_offset = ImVec2(1.0f, 1.0f);
    window->DrawList->AddRectFilled(
        ImVec2(pos.x + shadow_offset.x, pos.y + shadow_offset.y),
        ImVec2(pos.x + size.x + shadow_offset.x, pos.y + size.y + shadow_offset.y),
        IM_COL32(0, 0, 0, 80),
        style.FrameRounding
    );

    ImU32 blackColor = GetColorU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f));

    // Fill
    RenderNavHighlight(bb, id);
    window->DrawList->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), GetColorU32(anim[id]), style.FrameRounding);

    // White outline
    window->DrawList->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), blackColor, style.FrameRounding, 0, 1.0f);

    // Label text
    float original_scale = g.Font->Scale;
    g.Font->Scale *= 1.0f;
    ImGui::PushFont(g.Font);
    ImVec2 text_pos = ImVec2(pos.x + height + style.ItemInnerSpacing.x + 3, pos.y);
    window->DrawList->AddText(text_pos, GetColorU32(ImGuiCol_Text), label);
    ImGui::PopFont();
    g.Font->Scale = original_scale;

    return pressed;
}

void render::child(const char* label, const ImVec2& size_arg, bool hasOtherChild)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiStyle& style = ImGui::GetStyle();
    ImDrawList* draw = ImGui::GetWindowDrawList();

    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 size = ImGui::CalcItemSize(size_arg, 200.f, 200.f);
    ImVec2 end = ImVec2(pos.x + size.x, pos.y + size.y);

    float border_thickness = 1.0f;
    float top_thickness = 1.5f;
    ImU32 bg_color = IM_COL32(30, 30, 30, 255);                    // match tab active bg
    ImU32 border_color = IM_COL32(70, 255, 70, 255);               // bright green top line
    ImU32 outline_color = IM_COL32(80, 80, 80, 255);               // soft gray border

    // Full background
    draw->AddRectFilled(pos, end, bg_color);

    // Top green line (active tab connection style)
    draw->AddLine(pos, ImVec2(pos.x + size.x, pos.y), border_color, top_thickness);

    // Optional outer border (if not connected to another child)
    if (!hasOtherChild)
        draw->AddRect(pos, end, outline_color, 0.0f, 0, border_thickness);

    // Actual child region inside
    ImGui::SetCursorScreenPos(pos); // reset cursor
    ImGui::BeginChild(
        std::string(label).append("#Main").c_str(),
        size,
        true, // border
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground
    );
}



void render::child2(const char* label, const ImVec2& size_arg, bool hasOtherChild)
{
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;

        ImGui::BeginChild(label, size_arg, ImGuiChildFlags_None, ImGuiWindowFlags_NoDecoration | ImGuiChildFlags_Border | ImGuiWindowFlags_NoBackground);
        {
            ImVec2 pos = ImGui::GetWindowPos();
            ImVec2 size = ImGui::GetWindowSize();
            ImDrawList* pDrawList = ImGui::GetWindowDrawList();

            float HeaderHeight = ImGui::GetFontSize() + style.WindowPadding.y * 2 + style.ChildBorderSize * 2;

            // Full child background


            pDrawList->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), ImGui::GetColorU32(ImGuiCol_ChildBg));


            if (style.ChildBorderSize > 0)
            {
                float topBorderThickness = 8;
                if (hasOtherChild)
                {
                    // Draw only top thick border line
                    pDrawList->AddLine(
                        pos,
                        ImVec2(pos.x + size.x, pos.y),
                        GetColorU32(ImGuiCol_Border),
                        topBorderThickness
                    );
                }
                else
                {
                    // Draw top thick line + full rect border
                    pDrawList->AddLine(
                        pos,
                        ImVec2(pos.x + size.x, pos.y),
                        GetColorU32(ImGuiCol_Border),
                        topBorderThickness
                    );
                    pDrawList->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), ImGui::GetColorU32(ImGuiCol_Border));

                }
            }

            // Begin internal child
            ImGui::BeginChild(std::string(label).append("#Main").c_str(), ImVec2(), ImGuiChildFlags_Border, ImGuiWindowFlags_NoBackground);
        }
    }
