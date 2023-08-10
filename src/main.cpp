#include <iostream>
#include "setup/InitData.hpp"
#include "server/IRCServer.hpp"
#include "setup/SignalHandlers.hpp"
#include "color/Color.hpp"

int main(int argc, const char** argv) {
	InitData initData(argc, argv);

	if (!initData.isValid()) {
		std::cout << RED("Could not initialize client.\n") << RED(initData.getError()) << std::endl;
		return 0;
	}
	std::cout << YELLOW("Starting IRC Server...") << std::endl;
	std::cout << "- Port: " << initData.getPort() << std::endl;
	std::cout << "- Password: " << initData.getPassword() << std::endl;

	IRCServer server(initData.getPort(), initData.getPassword());
    registerSignals(&server);
	server.bind();
	std::cout << GREEN("Server socket bound") << std::endl;
	server.listen();
	std::cout << "Server listening on port " << initData.getPort() << std::endl;
	std::cout << MAGENTA("--- Start of Server Logs ---") << std::endl;
    int serverExitStatus = server.loop();
    std::cout << "Server exited with status " << serverExitStatus << std::endl;
    std::cout << MAGENTA("--- End of Server Logs ---") << std::endl;
    unregisterSignals();
	return serverExitStatus;
}
