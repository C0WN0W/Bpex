#include "menu.hpp"

int main()
{
	try {
		Gui.Attach("x64", "", DrawMenu);
		// Gui.AttachByLyric("x64", "", DrawMenu);
	}
	catch (OSImGui::OSException& e)
	{
		std::cout << e.what() << std::endl;
	}

	system("pause");
	return 0;
}