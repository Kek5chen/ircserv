#include <iostream>
#include "setup/InitData.hpp"

int main(int argc, const char** argv) {
	InitData initData(argc, argv);

	if (!initData.is_valid()) {
		std::cout << "Could not initialize client.\n" << initData.get_error() << std::endl;
		return 0;
	}
	std::cout << "Starting IRC Server..." << std::endl;
	std::cout << "- Port: " << initData.get_port() << std::endl;
	std::cout << "- Password: " << initData.get_password() << std::endl;

	return 0;
}
