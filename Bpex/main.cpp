#include "Driver/mainClass.h"

int main()
{
	if (Driver::InstallDriver()) {
		std::cout << "驱动安装成功！" << std::endl;
	} else {
		std::cerr << "驱动安装失败！" << std::endl;
	}

	system("pause");
	return 0;
}