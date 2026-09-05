#include "renders.hpp"

// 方框
void Draw::DrawBox(float x1, float y1, float x2, float y2, ULONG color, float Width, bool outline) {
    if (outline) {
        ImGui::GetBackgroundDrawList()->AddRect(
            ImVec2(x1+1, y1+1),             
            ImVec2(x2+1, y2+1),            
            IM_COL32(0, 0, 0, 255),       
            0.0f,                      
            0,                           
            Width                      
        );
    }
    ImGui::GetBackgroundDrawList()->AddRect(
        ImVec2(x1, y1),        // 左上角坐标
        ImVec2(x2, y2),        // 右下角坐标
        color,                 // 颜色
        0.0f,                  // 圆角半径（0 表示直角）
        0,                     // 角标记（0 表示全部角）
        Width                  // 线宽
    );
}

// 射线
void Draw::DrawLine(ImVec2 Start, ImVec2 End, ImColor color, float thickness, bool outline)
{
    if (outline) {
        ImGui::GetBackgroundDrawList()->AddLine(
            ImVec2(Start.x + 1, Start.y + 1),
            ImVec2(End.x + 1, End.y + 1),
            IM_COL32(0, 0, 0, 255),
            thickness
        );
    }
    ImGui::GetBackgroundDrawList()->AddLine(
        Start,          // 起点坐标
        End,            // 终点坐标
        color,          // 颜色
        thickness       // 线宽
    );
}

void Draw::DrawBox(ImVec2 Start, ImVec2 End, ImColor color, float thickness, float rounding)
{
    ImGui::GetBackgroundDrawList()->AddRect(Start, Start + End, color, rounding, 0, thickness);
}

// 绘制血量标签
void Draw::DrawHealth(ImVec2 Start, ImVec2 End, int currentHealth)
{
    // 动态颜色变化
    float targetHealth = (float)currentHealth / (float)100 * 255.f;
    float col_r = 255.f - targetHealth;
    float col_g = targetHealth;
    float col_b = 0.f;

    ImVec2 textPos = ImVec2(End.x + 1, Start.y);

    auto drawList = ImGui::GetBackgroundDrawList();
	std::string healthText = std::to_string(currentHealth);
    ImVec2 textSize = ImGui::CalcTextSize(healthText.c_str());
    ImVec2 textbgOffset = ImVec2(1, 1);
    drawList->AddRectFilled(textPos - textbgOffset, textPos + textSize + textbgOffset, IM_COL32(0, 0, 0, 255));
	drawList->AddText(ImVec2(textPos.x, textPos.y), IM_COL32((int)col_r, (int)col_g, (int)col_b, 255), healthText.c_str());
}

// 护甲标签
void Draw::DrawArmor(ImVec2 Start, ImVec2 End, int currentArmor, int MaxArmor)
{
    if (MaxArmor <= 0) return;

    currentArmor = max(0, min(currentArmor, MaxArmor));

    ImVec2 textPos = ImVec2(End.x + 1, Start.y + 12);

    ImColor colorLightGray = ImColor(247, 247, 247, 255);
    ImColor colorSkyBlue = ImColor(39, 178, 255, 255);
    ImColor colorPurple = ImColor(206, 59, 255, 255);
    ImColor colorRed = ImColor(219, 2, 2, 255);

    // 根据护甲值上限设置标签颜色
    // 如果不使用MaxArmor 可以选择读取ArmorType的值来判断黄金进化甲
    ImColor armorColor;
    if (MaxArmor >= 125) {
        armorColor = colorRed;
    }
    else if (MaxArmor >= 100) {
        armorColor = colorPurple;
    }
    else if (MaxArmor >= 75) {
        armorColor = colorSkyBlue;
    }
    else {
        armorColor = colorLightGray;
    }

	std::string armorText = std::to_string(currentArmor);
    ImVec2 textSize = ImGui::CalcTextSize(armorText.c_str());
	ImVec2 textbgOffset = ImVec2(1, 1);
    auto drawList = ImGui::GetBackgroundDrawList();
    drawList->AddRectFilled(textPos - textbgOffset, textPos + textSize + textbgOffset, IM_COL32(0, 0, 0, 255));
    drawList->AddText(ImVec2(textPos.x, textPos.y), armorColor, armorText.c_str());
}

// 队伍标签
void Draw::DrawTeam(ImVec2 Start, ImVec2 End, int id, ImColor color)
{
    ImVec2 textSize = ImGui::CalcTextSize(std::to_string(id).c_str());
    ImVec2 textPos = ImVec2(Start.x - textSize.x - 1, Start.y);
    ImVec2 textbgOffset = ImVec2(1, 1);
    auto drawList = ImGui::GetBackgroundDrawList();
    drawList->AddRectFilled(textPos - textbgOffset, textPos + textSize + textbgOffset, color);
    drawList->AddText(ImVec2(textPos.x, textPos.y), ImColor(255, 255, 255, 255), std::to_string(id).c_str());
}