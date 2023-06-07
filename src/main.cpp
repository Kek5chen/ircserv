#include <iostream>
#include "setup/InitData.hpp"
#include "server/IRCServer.hpp"
#include "setup/SignalHandlers.hpp"

int main(int argc, const char** argv) {
	InitData initData(argc, argv);

	if (!initData.is_valid()) {
		std::cout << "Could not initialize client.\n" << initData.get_error() << std::endl;
		return 0;
	}
	std::cout << "Starting IRC Server..." << std::endl;
	std::cout << "- Port: " << initData.get_port() << std::endl;
	std::cout << "- Password: " << initData.get_password() << std::endl;

	IRCServer server(initData.get_port(), initData.get_password());
	server.bind();
	std::cout << "Server socket bound" << std::endl;
	server.listen();
	std::cout << "Server listening on port " << initData.get_port() << std::endl;
	std::cout << "--- Start of Server Logs ---" << std::endl;
	server.loop();
	return 0;
}
