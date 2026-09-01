#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
#include "OS-ImGui/OS-ImGui.h"

namespace Draw
{
	void DrawStrokeText(ImVec2 TextPos, const char* Text, ImColor TextColor = ImColor(255, 255, 255, 255), ImColor OutlineColor = ImColor(0, 0, 0, 255));

	void DrawBox(float x1, float y1, float x2, float y2, ULONG color, float strokeWidth, bool outline = false);
	void DrawBox(ImVec2 Start, ImVec2 End, ImColor color, float thickness = 1.2f, float rounding = 0.f);
	void DrawLine(ImVec2 Start, ImVec2 End, ImColor color, float thickness = 1.0f, bool outline = false);

	void DrawHealth(ImVec2 Start, ImVec2 End, int currentHealth);
	void DrawArmor(ImVec2 Start, ImVec2 End, int currentArmor, int MaxArmor);

	void DrawDistance(ImVec2 Start, ImVec2 End, float dist);

	void SeerHealth(int bar_width, float x, float y, int shield, int max_shield, int armorType, int health);
}