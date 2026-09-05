#include "menu.hpp"
#include "configs.hpp"

void DrawMenu()
{
	ImGui::Begin("www.92ghai.com", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	// BPEX酷狗音乐单透 by CowNow
	{
		ImGui::BeginTabBar("TabBar", ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_NoTooltip);
		// 绘制部分
		// 方框 射线 倒地判断 可视判断
		// 血量 护甲 队伍标签
		// 透视范围调整
		if (ImGui::BeginTabItem("ESP"))
		{
			ImGui::Columns(2);
			ImGui::ColorEdit3("##BoxCol", reinterpret_cast<float*>(&cfg::BoxColor), 32);
			ImGui::SameLine();
			ImGui::Checkbox("Box", &cfg::BoxESP);
			ImGui::ColorEdit3("##LineCol", reinterpret_cast<float*>(&cfg::LineColor), 32);
			ImGui::SameLine();
			ImGui::Checkbox("Line", &cfg::LineESP);
			ImGui::ColorEdit3("##KnockCol", reinterpret_cast<float*>(&cfg::KnockColor), 32);
			ImGui::SameLine();
			ImGui::Checkbox("Knocked", &cfg::KnockESP);
			ImGui::NextColumn();
			ImGui::Checkbox("HP", &cfg::HealthESP);
			ImGui::Checkbox("SH", &cfg::ArmorESP);
			ImGui::Checkbox("Team", &cfg::TeamESP);
			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::SliderInt("Render Distance", &cfg::RenderDistance, 10, 500);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Radar"))
		{
			ImGui::Checkbox("Enable", &cfg::radarEnabled);
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
		ImGui::NewLine();
		ImGui::Text("QQ Group: 428714797");
	}
	ImGui::End();
}