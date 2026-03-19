#include "../settings/functions.h"

void c_gui::push_color(ImGuiCol idx, ImU32 col)
{
    ImGuiContext& g = *GImGui;
    ImGuiColorMod backup;
    backup.Col = idx;
    backup.BackupValue = g.Style.Colors[idx];
    g.ColorStack.push_back(backup);
    if (g.DebugFlashStyleColorIdx != idx)
        g.Style.Colors[idx] = ColorConvertU32ToFloat4(col);
}

void c_gui::pop_color(int count)
{
    ImGuiContext& g = *GImGui;
    if (g.ColorStack.Size < count)
    {
        IM_ASSERT_USER_ERROR(g.ColorStack.Size > count, "Calling PopStyleColor() too many times!");
        count = g.ColorStack.Size;
    }
    while (count > 0)
    {
        ImGuiColorMod& backup = g.ColorStack.back();
        g.Style.Colors[backup.Col] = backup.BackupValue;
        g.ColorStack.pop_back();
        count--;
    }
}

void c_gui::push_var(ImGuiStyleVar idx, float val)
{
    ImGuiContext& g = *GImGui;
    const ImGuiDataVarInfo* var_info = GetStyleVarInfo(idx);
    if (var_info->Type != ImGuiDataType_Float || var_info->Count != 1)
    {
        IM_ASSERT_USER_ERROR(0, "Calling PushStyleVar() variant with wrong type!");
        return;
    }
    float* pvar = (float*)var_info->GetVarPtr(&g.Style);
    g.StyleVarStack.push_back(ImGuiStyleMod(idx, *pvar));
    *pvar = val;
}

void c_gui::push_var(ImGuiStyleVar idx, const ImVec2& val)
{
    ImGuiContext& g = *GImGui;
    const ImGuiDataVarInfo* var_info = GetStyleVarInfo(idx);
    if (var_info->Type != ImGuiDataType_Float || var_info->Count != 2)
    {
        IM_ASSERT_USER_ERROR(0, "Calling PushStyleVar() variant with wrong type!");
        return;
    }
    ImVec2* pvar = (ImVec2*)var_info->GetVarPtr(&g.Style);
    g.StyleVarStack.push_back(ImGuiStyleMod(idx, *pvar));
    *pvar = val;
}

void c_gui::pop_var(int count)
{
    ImGuiContext& g = *GImGui;
    if (g.StyleVarStack.Size < count)
    {
        IM_ASSERT_USER_ERROR(g.StyleVarStack.Size > count, "Calling PopStyleVar() too many times!");
        count = g.StyleVarStack.Size;
    }
    while (count > 0)
    {
        ImGuiStyleMod& backup = g.StyleVarStack.back();
        const ImGuiDataVarInfo* info = GetStyleVarInfo(backup.VarIdx);
        void* data = info->GetVarPtr(&g.Style);
        if (info->Type == ImGuiDataType_Float && info->Count == 1) { ((float*)data)[0] = backup.BackupFloat[0]; }
        else if (info->Type == ImGuiDataType_Float && info->Count == 2) { ((float*)data)[0] = backup.BackupFloat[0]; ((float*)data)[1] = backup.BackupFloat[1]; }
        g.StyleVarStack.pop_back();
        count--;
    }
}

void c_gui::push_font(ImFont* font)
{
    ImGuiContext& g = *GImGui;
    if (font == NULL)
        font = GetDefaultFont();
    g.FontStack.push_back(font);
    SetCurrentFont(font);
    g.CurrentWindow->DrawList->_SetTextureID(font->ContainerAtlas->TexID);
}

void c_gui::pop_font()
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(g.FontStack.Size > 0);
    g.FontStack.pop_back();
    ImFont* font = g.FontStack.Size == 0 ? GetDefaultFont() : g.FontStack.back();
    SetCurrentFont(font);
    g.CurrentWindow->DrawList->_SetTextureID(font->ContainerAtlas->TexID);
}

void c_gui::set_pos(const ImVec2& pos, int type)
{
    ImGuiWindow* window = GetCurrentWindow();

    if (type == pos_all)
        window->DC.CursorPos = window->Pos - window->Scroll + pos;
    else if (type == pos_x)
        window->DC.CursorPos.x = window->Pos.x - window->Scroll.x + pos.x;
    else if (type == pos_y)
        window->DC.CursorPos.y = window->Pos.y - window->Scroll.y + pos.y;

    window->DC.IsSetPos = true;
}

void c_gui::set_pos(float pos, int type)
{
    set_pos(ImVec2(pos, pos), type);
}

ImVec2 c_gui::get_pos()
{
    ImGuiWindow* window = GetCurrentWindowRead();
    return window->DC.CursorPos - window->Pos + window->Scroll;
}

void c_gui::set_screen_pos(const ImVec2& pos, int type)
{
    ImGuiWindow* window = GetCurrentWindow();

    if (type == pos_all)
        window->DC.CursorPos = pos;
    else if (type == pos_x)
        window->DC.CursorPos.x = pos.x;
    else if (type == pos_y)
        window->DC.CursorPos.y = pos.y;

    window->DC.IsSetPos = true;
}

void c_gui::set_screen_pos(float pos, int type)
{
    set_screen_pos(ImVec2(pos, pos), type);
}

ImVec2 c_gui::get_screen_pos()
{
    ImGuiWindow* window = GetCurrentWindowRead();
    return window->DC.CursorPos;
}

void c_gui::begin_group()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    g.GroupStack.resize(g.GroupStack.Size + 1);
    ImGuiGroupData& group_data = g.GroupStack.back();
    group_data.WindowID = window->ID;
    group_data.BackupCursorPos = window->DC.CursorPos;
    group_data.BackupCursorPosPrevLine = window->DC.CursorPosPrevLine;
    group_data.BackupCursorMaxPos = window->DC.CursorMaxPos;
    group_data.BackupIndent = window->DC.Indent;
    group_data.BackupGroupOffset = window->DC.GroupOffset;
    group_data.BackupCurrLineSize = window->DC.CurrLineSize;
    group_data.BackupCurrLineTextBaseOffset = window->DC.CurrLineTextBaseOffset;
    group_data.BackupActiveIdIsAlive = g.ActiveIdIsAlive;
    group_data.BackupHoveredIdIsAlive = g.HoveredId != 0;
    group_data.BackupIsSameLine = window->DC.IsSameLine;
    group_data.BackupActiveIdPreviousFrameIsAlive = g.ActiveIdPreviousFrameIsAlive;
    group_data.EmitItem = true;

    window->DC.GroupOffset.x = window->DC.CursorPos.x - window->Pos.x - window->DC.ColumnsOffset.x;
    window->DC.Indent = window->DC.GroupOffset;
    window->DC.CursorMaxPos = window->DC.CursorPos;
    window->DC.CurrLineSize = ImVec2(0.0f, 0.0f);
    if (g.LogEnabled)
        g.LogLinePosY = -FLT_MAX;
}

void c_gui::end_group()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    IM_ASSERT(g.GroupStack.Size > 0);

    ImGuiGroupData& group_data = g.GroupStack.back();
    IM_ASSERT(group_data.WindowID == window->ID);

    if (window->DC.IsSetPos)
        ErrorCheckUsingSetCursorPosToExtendParentBoundaries();

    ImRect group_bb(group_data.BackupCursorPos, ImMax(ImMax(window->DC.CursorMaxPos, g.LastItemData.Rect.Max), group_data.BackupCursorPos));
    window->DC.CursorPos = group_data.BackupCursorPos;
    window->DC.CursorPosPrevLine = group_data.BackupCursorPosPrevLine;
    window->DC.CursorMaxPos = ImMax(group_data.BackupCursorMaxPos, group_bb.Max);
    window->DC.Indent = group_data.BackupIndent;
    window->DC.GroupOffset = group_data.BackupGroupOffset;
    window->DC.CurrLineSize = group_data.BackupCurrLineSize;
    window->DC.CurrLineTextBaseOffset = group_data.BackupCurrLineTextBaseOffset;
    window->DC.IsSameLine = group_data.BackupIsSameLine;
    if (g.LogEnabled)
        g.LogLinePosY = -FLT_MAX;

    if (!group_data.EmitItem)
    {
        g.GroupStack.pop_back();
        return;
    }

    window->DC.CurrLineTextBaseOffset = ImMax(window->DC.PrevLineTextBaseOffset, group_data.BackupCurrLineTextBaseOffset);
    ItemSize(group_bb.GetSize());
    ItemAdd(group_bb, 0, NULL, ImGuiItemFlags_NoTabStop);

    const bool group_contains_curr_active_id = (group_data.BackupActiveIdIsAlive != g.ActiveId) && (g.ActiveIdIsAlive == g.ActiveId) && g.ActiveId;
    const bool group_contains_prev_active_id = (group_data.BackupActiveIdPreviousFrameIsAlive == false) && (g.ActiveIdPreviousFrameIsAlive == true);
    if (group_contains_curr_active_id)
        g.LastItemData.ID = g.ActiveId;
    else if (group_contains_prev_active_id)
        g.LastItemData.ID = g.ActiveIdPreviousFrame;
    g.LastItemData.Rect = group_bb;

    const bool group_contains_curr_hovered_id = (group_data.BackupHoveredIdIsAlive == false) && g.HoveredId != 0;
    if (group_contains_curr_hovered_id)
        g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HoveredWindow;

    if (group_contains_curr_active_id && g.ActiveIdHasBeenEditedThisFrame)
        g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_Edited;

    g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HasDeactivated;
    if (group_contains_prev_active_id && g.ActiveId != g.ActiveIdPreviousFrame)
        g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_Deactivated;

    g.GroupStack.pop_back();
    if (g.DebugShowGroupRects)
        window->DrawList->AddRect(group_bb.Min, group_bb.Max, IM_COL32(255, 0, 255, 255));
}

void c_gui::begin_content(std::string_view id, const ImVec2& size, const ImVec2& padding, const ImVec2& spacing, ImGuiWindowFlags flags)
{
    gui->push_var(ImGuiStyleVar_WindowPadding, padding);
    gui->begin_def_child(id, size, ImGuiChildFlags_None, flags | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing);
    gui->push_var(ImGuiStyleVar_ItemSpacing, spacing);
}

void c_gui::end_content()
{
    gui->pop_var();
    gui->end_def_child();
    gui->pop_var();
}

void c_gui::sameline(float offset_from_start_x, float spacing_w)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    if (window->SkipItems)
        return;

    if (offset_from_start_x != 0.0f)
    {
        if (spacing_w < 0.0f)
            spacing_w = 0.0f;
        window->DC.CursorPos.x = window->Pos.x - window->Scroll.x + offset_from_start_x + spacing_w + window->DC.GroupOffset.x + window->DC.ColumnsOffset.x;
        window->DC.CursorPos.y = window->DC.CursorPosPrevLine.y;
    }
    else
    {
        if (spacing_w < 0.0f)
            spacing_w = g.Style.ItemSpacing.x;
        window->DC.CursorPos.x = window->DC.CursorPosPrevLine.x + spacing_w;
        window->DC.CursorPos.y = window->DC.CursorPosPrevLine.y;
    }
    window->DC.CurrLineSize = window->DC.PrevLineSize;
    window->DC.CurrLineTextBaseOffset = window->DC.PrevLineTextBaseOffset;
    window->DC.IsSameLine = true;
}

void c_gui::dummy(const ImVec2& size)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
    ItemSize(size);
    ItemAdd(bb, 0);
}

bool begin_def_child_ex(const char* name, ImGuiID id, const ImVec2& size_arg, ImGuiChildFlags child_flags, ImGuiWindowFlags window_flags)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* parent_window = g.CurrentWindow;
    IM_ASSERT(id != 0);

    const ImGuiChildFlags ImGuiChildFlags_SupportedMask_ = ImGuiChildFlags_Borders | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_ResizeX | ImGuiChildFlags_ResizeY | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_FrameStyle | ImGuiChildFlags_NavFlattened;
    IM_UNUSED(ImGuiChildFlags_SupportedMask_);
    IM_ASSERT((child_flags & ~ImGuiChildFlags_SupportedMask_) == 0 && "Illegal ImGuiChildFlags value. Did you pass ImGuiWindowFlags values instead of ImGuiChildFlags?");
    IM_ASSERT((window_flags & ImGuiWindowFlags_AlwaysAutoResize) == 0 && "Cannot specify ImGuiWindowFlags_AlwaysAutoResize for BeginChild(). Use ImGuiChildFlags_AlwaysAutoResize!");
    if (child_flags & ImGuiChildFlags_AlwaysAutoResize)
    {
        IM_ASSERT((child_flags & (ImGuiChildFlags_ResizeX | ImGuiChildFlags_ResizeY)) == 0 && "Cannot use ImGuiChildFlags_ResizeX or ImGuiChildFlags_ResizeY with ImGuiChildFlags_AlwaysAutoResize!");
        IM_ASSERT((child_flags & (ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY)) != 0 && "Must use ImGuiChildFlags_AutoResizeX or ImGuiChildFlags_AutoResizeY with ImGuiChildFlags_AlwaysAutoResize!");
    }
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    if (window_flags & ImGuiWindowFlags_AlwaysUseWindowPadding)
        child_flags |= ImGuiChildFlags_AlwaysUseWindowPadding;
    if (window_flags & ImGuiWindowFlags_NavFlattened)
        child_flags |= ImGuiChildFlags_NavFlattened;
#endif
    if (child_flags & ImGuiChildFlags_AutoResizeX)
        child_flags &= ~ImGuiChildFlags_ResizeX;
    if (child_flags & ImGuiChildFlags_AutoResizeY)
        child_flags &= ~ImGuiChildFlags_ResizeY;

    window_flags |= ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_NoTitleBar;
    window_flags |= (parent_window->Flags & ImGuiWindowFlags_NoMove);
    if (child_flags & (ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize))
        window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
    if ((child_flags & (ImGuiChildFlags_ResizeX | ImGuiChildFlags_ResizeY)) == 0)
        window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;

    if (child_flags & ImGuiChildFlags_FrameStyle)
    {
        PushStyleColor(ImGuiCol_ChildBg, g.Style.Colors[ImGuiCol_FrameBg]);
        PushStyleVar(ImGuiStyleVar_ChildRounding, g.Style.FrameRounding);
        PushStyleVar(ImGuiStyleVar_ChildBorderSize, g.Style.FrameBorderSize);
        PushStyleVar(ImGuiStyleVar_WindowPadding, g.Style.FramePadding);
        child_flags |= ImGuiChildFlags_Borders | ImGuiChildFlags_AlwaysUseWindowPadding;
        window_flags |= ImGuiWindowFlags_NoMove;
    }

    g.NextWindowData.Flags |= ImGuiNextWindowDataFlags_HasChildFlags;
    g.NextWindowData.ChildFlags = child_flags;

    const ImVec2 size_avail = GetContentRegionAvail();
    const ImVec2 size_default((child_flags & ImGuiChildFlags_AutoResizeX) ? 0.0f : size_avail.x, (child_flags & ImGuiChildFlags_AutoResizeY) ? 0.0f : size_avail.y);
    const ImVec2 size = CalcItemSize(size_arg, size_default.x, size_default.y);
    SetNextWindowSize(size);

    const char* temp_window_name;

    if (name)
        ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%s_%08X", parent_window->Name, name, id);
    else
        ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%08X", parent_window->Name, id);

    const float backup_border_size = g.Style.ChildBorderSize;
    if ((child_flags & ImGuiChildFlags_Borders) == 0)
        g.Style.ChildBorderSize = 0.0f;

    const bool ret = gui->begin(temp_window_name, NULL, window_flags);

    g.Style.ChildBorderSize = backup_border_size;
    if (child_flags & ImGuiChildFlags_FrameStyle)
    {
        PopStyleVar(3);
        PopStyleColor();
    }

    ImGuiWindow* child_window = g.CurrentWindow;
    child_window->ChildId = id;

    if (child_window->BeginCount == 1)
        parent_window->DC.CursorPos = child_window->Pos;

    const ImGuiID temp_id_for_activation = ImHashStr("##Child", 0, id);
    if (g.ActiveId == temp_id_for_activation)
        ClearActiveID();
    if (g.NavActivateId == id && !(child_flags & ImGuiChildFlags_NavFlattened) && (child_window->DC.NavLayersActiveMask != 0 || child_window->DC.NavWindowHasScrollY))
    {
        FocusWindow(child_window);
        NavInitWindow(child_window, false);
        SetActiveID(temp_id_for_activation, child_window);
        g.ActiveIdSource = g.NavInputSource;
    }
    return ret;
}

bool c_gui::begin_def_child(std::string_view name, const ImVec2& size_arg, ImGuiChildFlags child_flags, ImGuiWindowFlags window_flags)
{
    ImGuiID id = GetCurrentWindow()->GetID(name.data());
    return begin_def_child_ex(name.data(), id, size_arg, child_flags, window_flags);
}

void c_gui::end_def_child()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* child_window = g.CurrentWindow;

    IM_ASSERT(g.WithinEndChild == false);
    IM_ASSERT(child_window->Flags & ImGuiWindowFlags_ChildWindow);

    g.WithinEndChild = true;
    ImVec2 child_size = child_window->Size;
    gui->end();
    if (child_window->BeginCount == 1)
    {
        ImGuiWindow* parent_window = g.CurrentWindow;
        ImRect bb(parent_window->DC.CursorPos, parent_window->DC.CursorPos + child_size);
        ItemSize(child_size);
        const bool nav_flattened = (child_window->ChildFlags & ImGuiChildFlags_NavFlattened) != 0;
        if ((child_window->DC.NavLayersActiveMask != 0 || child_window->DC.NavWindowHasScrollY) && !nav_flattened)
        {
            ItemAdd(bb, child_window->ChildId);
            RenderNavHighlight(bb, child_window->ChildId);

            if (child_window->DC.NavLayersActiveMask == 0 && child_window == g.NavWindow)
                RenderNavHighlight(ImRect(bb.Min - ImVec2(2, 2), bb.Max + ImVec2(2, 2)), g.NavId, ImGuiNavHighlightFlags_Compact);
        }
        else
        {
            ItemAdd(bb, child_window->ChildId, NULL, ImGuiItemFlags_NoNav);

            if (nav_flattened)
                parent_window->DC.NavLayersActiveMaskNext |= child_window->DC.NavLayersActiveMaskNext;
        }
        if (g.HoveredWindow == child_window)
            g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HoveredWindow;
    }
    g.WithinEndChild = false;
    g.LogLinePosY = -FLT_MAX;
}

void c_gui::set_next_window_pos(const ImVec2& pos, ImGuiCond cond, const ImVec2& pivot)
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(cond == 0 || ImIsPowerOfTwo(cond));
    g.NextWindowData.Flags |= ImGuiNextWindowDataFlags_HasPos;
    g.NextWindowData.PosVal = pos;
    g.NextWindowData.PosPivotVal = pivot;
    g.NextWindowData.PosCond = cond ? cond : ImGuiCond_Always;
}

void c_gui::set_next_window_size(const ImVec2& size, ImGuiCond cond)
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(cond == 0 || ImIsPowerOfTwo(cond));
    g.NextWindowData.Flags |= ImGuiNextWindowDataFlags_HasSize;
    g.NextWindowData.SizeVal = size;
    g.NextWindowData.SizeCond = cond ? cond : ImGuiCond_Always;
}

ImVec2 c_gui::text_size(ImFont* fontm, const char* text, const char* text_end, bool hide_text_after_double_hash, float wrap_width)
{
    this->push_font(fontm);

    ImGuiContext& g = *GImGui;

    const char* text_display_end;
    if (hide_text_after_double_hash)
        text_display_end = FindRenderedTextEnd(text, text_end);
    else
        text_display_end = text_end;

    ImFont* font = g.Font;
    const float font_size = g.FontSize;
    if (text == text_display_end)
        return ImVec2(0.0f, font_size);
    ImVec2 text_size = font->CalcTextSizeA(font_size, FLT_MAX, wrap_width, text, text_display_end, NULL);
    text_size.x = IM_TRUNC(text_size.x + 0.99999f);

    this->pop_font();

    return text_size;
}

std::string c_gui::wrap_text_to_width(ImFont* font, const std::string& text, float max_width)
{
    std::vector<std::string> lines;
    std::string current_line;
    float current_line_width = 0.0f;

    const char* current_char = text.c_str();
    const std::string line_break_indicator = "..."; // �������� �� ����������
    const std::string period_str = " ";

    bool added_ellipsis = false; // ���� ��� ������������ ���������� ����������

    while (*current_char != '\0')
    {
        std::string current_char_str(1, *current_char);
        float char_width = gui->text_size(font, current_char_str.c_str()).x;

        bool is_space = (*current_char == ' ');
        bool is_period = (*current_char == '.');

        if (current_line_width + char_width <= max_width)
        {
            current_line += current_char_str;
            current_line_width += char_width;
        }
        else
        {
            if (!current_line.empty())
            {
                if (is_space || is_period)
                {
                    lines.push_back(current_line);
                    current_line.clear();
                    current_line_width = 0.0f;
                }
                else
                {
                    if (!current_line.empty() && current_line.back() != '.' && current_line.back() != ' ' && !added_ellipsis)
                    {
                        lines.push_back(current_line + line_break_indicator);
                        added_ellipsis = true; // ������������� ����, ��� ���������� ���������
                    }
                    else
                    {
                        lines.push_back(current_line);
                    }

                    current_line = current_char_str;
                    current_line_width = char_width;
                }
            }
            else
            {
                current_line = current_char_str;
                current_line_width = char_width;
            }
        }

        current_char++;
    }

    if (!current_line.empty())
    {
        if (current_line.back() != '.')
        {
            current_line += period_str;
        }
        lines.push_back(current_line);
    }

    std::string wrapped_text;
    for (const auto& line : lines)
    {
        wrapped_text += line + "\n";
    }

    return wrapped_text;
}


ImVec2 c_gui::window_size()
{
    ImGuiWindow* window = GImGui->CurrentWindow;
    return window->Size;
}

float c_gui::window_width()
{
    ImGuiWindow* window = GImGui->CurrentWindow;
    return window->Size.x;
}

float c_gui::window_height()
{
    ImGuiWindow* window = GImGui->CurrentWindow;
    return window->Size.y;
}

ImVec2 c_gui::content_avail()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImVec2 mx = (window->DC.CurrentColumns || g.CurrentTable) ? window->WorkRect.Max : window->ContentRegionRect.Max;
    return mx - window->DC.CursorPos;
}

ImVec2 c_gui::content_max()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImVec2 mx = (window->DC.CurrentColumns || g.CurrentTable) ? window->WorkRect.Max : window->ContentRegionRect.Max;
    return mx - window->Pos;
}

void c_gui::set_style()
{
    ImGuiStyle* style = &GetStyle();

    style->WindowBorderSize = SCALE(var->window.window_border_size);
    style->WindowRounding = SCALE(elements->rounding );
    style->ChildRounding = 0.f;
    style->WindowPadding = SCALE(var->window.window_padding);

    style->ScrollbarSize = SCALE(var->window.scrollbar_size);
    style->ItemSpacing = SCALE(var->window.item_spacing);

}

void c_gui::draw_decoration()
{
    const ImVec2 pos = GetWindowPos();
    ImDrawList* drawlist = GetWindowDrawList();

    const ImVec2 window_size = GetWindowSize();
    const ImVec2 titlebar_size = SCALE(elements->titlebar.size);
    const ImVec2 titlebar_spacing = ImVec2(0.f, titlebar_size.y);
    const ImVec2 tab_info_size = SCALE(elements->tab_info.size);
    const ImVec2 tab_info_spacing = ImVec2(0.f, titlebar_size.y + tab_info_size.y);
    const float r = SCALE(elements->rounding * 2.5f);

    // Main background
    draw->rect_filled(drawlist, pos, pos + window_size, draw->get_clr(clr->main.dark_1), r);

    // Outer border with subtle accent
    draw->rect(drawlist, pos, pos + window_size, draw->get_clr(clr->main.border), r);

    // Accent top bar
    const float bar_h = SCALE(2.5f);
    ImU32 accent_l = IM_COL32(50, 160, 235, 0);
    ImU32 accent_r = IM_COL32(50, 160, 235, 200);
    ImU32 accent_m = IM_COL32(50, 160, 235, 255);
    drawlist->AddRectFilledMultiColor(
        pos, ImVec2(pos.x + window_size.x * 0.5f, pos.y + bar_h),
        accent_l, accent_m, accent_m, accent_l );
    drawlist->AddRectFilledMultiColor(
        ImVec2(pos.x + window_size.x * 0.5f, pos.y), ImVec2(pos.x + window_size.x, pos.y + bar_h),
        accent_m, accent_r, accent_r, accent_m );

    // Title bar
    draw->rect_filled(drawlist, pos + ImVec2(0, bar_h), pos + titlebar_size, draw->get_clr(clr->main.dark_2), r, ImDrawFlags_RoundCornersTop);

    // Tab info bar
    draw->rect_filled(drawlist, pos + titlebar_spacing, pos + titlebar_spacing + tab_info_size, draw->get_clr(clr->main.dark_3_255));

    // Separator lines
    draw->line(drawlist, pos + titlebar_spacing, pos + titlebar_size, draw->get_clr(clr->main.border));
    draw->line(drawlist, pos + tab_info_spacing, pos + titlebar_spacing + tab_info_size, draw->get_clr(clr->main.border));

    // Logo
    const char* label = "Ghost";
    const char* label_cc = "X";
    const ImVec2 label_size = gui->text_size(var->font.medium15, label);

    draw->text(drawlist, var->font.medium15, var->font.medium15->FontSize, pos + ImVec2(SCALE(18.f), titlebar_size.y / 2 - label_size.y / 2), draw->get_clr(clr->text.active), label);
    draw->text(drawlist, var->font.medium15, var->font.medium15->FontSize, pos + ImVec2(SCALE(18.f) + label_size.x, titlebar_size.y / 2 - label_size.y / 2), draw->get_clr(clr->accent.main), label_cc);
}

void c_gui::setup_animations()
{
    {
        bool start_anim = var->tab.active != var->tab.current;

        gui->easing(var->anim.features_alpha, start_anim ? 0.f : 1.f, var->anim.speed, dynamic_easing);

        if (start_anim && var->anim.features_alpha < 0.01f)
            var->tab.current = var->tab.active;
        
        // Force immediate tab switch if animation is disabled or too slow
        if (var->tab.active != var->tab.current && var->anim.features_alpha > 0.99f)
            var->tab.current = var->tab.active;
    }

}

bool c_gui::is_first_item(int x_or_y, ImVec2 pos) {

    ImGuiWindow* window = GetCurrentWindow();
    float window_pos = x_or_y == 0 ? window->Pos.x : window->Pos.y;
    float pos_x_y = x_or_y == 0 ? pos.x : pos.y;

    if (window_pos != pos_x_y)
        return false;
    else
        return true;

}

bool c_gui::is_last_item(int x_or_y, ImVec2 pos) {

    ImGuiWindow* window = GetCurrentWindow();
    float window_pos = x_or_y == 0 ? window->Pos.x : window->Pos.y;
    float content_size = x_or_y == 0 ? window->ContentSize.x : window->ContentSize.y;
    float item_size = x_or_y == 0 ? GetItemRectSize().x : GetItemRectSize().y;
    float pos_x_y = x_or_y == 0 ? pos.x : pos.y;

    if (content_size != (pos_x_y - window_pos + item_size))
        return false;
    else
        return true;

}
