#include "menu.hpp"
#include "configs.hpp"
#include "aimbot.hpp"

void DrawMenu()
{
	ImGui::Begin("Bpex", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	{
		ImGui::BeginTabBar("TabBar", ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_NoTooltip);
		if (ImGui::BeginTabItem("ESP"))
		{
			ImGui::Columns(2);
			ImGui::ColorEdit3("##BoxCol", reinterpret_cast<float*>(&cfg::BoxColor), 32);
			ImGui::SameLine();
			ImGui::Checkbox("Box", &cfg::BoxESP);
			ImGui::ColorEdit3("##LineCol", reinterpret_cast<float*>(&cfg::LineColor), 32);
			ImGui::SameLine();
			ImGui::Checkbox("Line", &cfg::LineESP);
			ImGui::ColorEdit3("##BoneCol", reinterpret_cast<float*>(&cfg::BoneColor), 32);
			ImGui::SameLine();
			ImGui::Checkbox("Skeleton", &cfg::BoneESP);
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
			// ImGui::Checkbox("Visible Check", &cfg::VisCheck);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Chams"))
		{
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("AimAssist"))
		{
			if (ImGui::Button("Connect to KmboxNet"))
			{
				Aimbot::KmNetInit(Global::Km_IP, Global::Km_port, Global::Km_mac);
			}
			ImGui::Checkbox("Enable AimAssist", &cfg::aimbotEnabled);
			ImGui::Checkbox("RCS", &cfg::rcs);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Radar"))
		{
			ImGui::Checkbox("Enable", &cfg::radarEnabled);
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
	ImGui::End();
}