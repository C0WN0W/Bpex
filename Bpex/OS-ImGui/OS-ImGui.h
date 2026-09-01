#pragma once
#include "OS-ImGui_External.h"

namespace OSImGui
{
	class OSImGui : public OSImGui_External, public Singleton<OSImGui>
	{
		public:
	};
}

inline OSImGui::OSImGui& Gui = OSImGui::OSImGui::get();