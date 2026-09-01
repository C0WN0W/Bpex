#include "renders.hpp"

void Draw::DrawStrokeText(ImVec2 TextPos, const char* Text, ImColor TextColor, ImColor OutlineColor)
{
    auto drawList = ImGui::GetBackgroundDrawList();
    drawList->AddText(ImVec2(TextPos.x - 1, TextPos.y), OutlineColor, Text);
    drawList->AddText(ImVec2(TextPos.x + 1, TextPos.y), OutlineColor, Text);
    drawList->AddText(ImVec2(TextPos.x, TextPos.y - 1), OutlineColor, Text);
    drawList->AddText(ImVec2(TextPos.x, TextPos.y + 1), OutlineColor, Text);

    drawList->AddText(TextPos, TextColor, Text);
}

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

void Draw::DrawHealth(ImVec2 Start, ImVec2 End, int currentHealth)
{
    float targetHealth = (float)currentHealth / (float)100 * 255.f;
    float col_r = 255.f - targetHealth;
    float col_g = targetHealth;
    float col_b = 0.f;

    ImVec2 textPos = ImVec2(End.x + 1, Start.y);

    auto drawList = ImGui::GetBackgroundDrawList();
	std::string healthText = "HP " + std::to_string(currentHealth);
	drawList->AddText(ImVec2(textPos.x, textPos.y), IM_COL32((int)col_r, (int)col_g, (int)col_b, 255), healthText.c_str());
}

void Draw::DrawArmor(ImVec2 Start, ImVec2 End, int currentArmor, int MaxArmor)
{
    if (MaxArmor <= 0) return;

    currentArmor = max(0, min(currentArmor, MaxArmor));

    ImVec2 textPos = ImVec2(End.x + 1, Start.y + 12);

    ImColor colorLightGray = ImColor(247, 247, 247, 255);
    ImColor colorSkyBlue = ImColor(39, 178, 255, 255);
    ImColor colorPurple = ImColor(206, 59, 255, 255);
    ImColor colorRed = ImColor(219, 2, 2, 255);

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

	std::string armorText = "SH " + std::to_string(currentArmor) + " / " + std::to_string(MaxArmor);
    auto drawList = ImGui::GetBackgroundDrawList();
    drawList->AddText(ImVec2(textPos.x, textPos.y), armorColor, armorText.c_str());
}

void Draw::DrawDistance(ImVec2 Start, ImVec2 End, float dist)
{
    float boxWidth = End.x - Start.x;
    float boxHeight = End.y - Start.y;

    float centerX = Start.x + (boxWidth / 2);

    float textY = End.y + 5;

    char distanceText[32];
    if (dist >= 1000) {
        sprintf_s(distanceText, "%.2fkm", dist / 1000.0f);
    }
    else {
        sprintf_s(distanceText, "%.0fm", dist);
    }

    ImVec2 textSize = ImGui::CalcTextSize(distanceText);
    ImVec2 textPos = ImVec2(centerX - (textSize.x / 2), textY);

    DrawStrokeText(textPos, distanceText);
}