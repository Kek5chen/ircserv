#include <iostream>
#include "setup/InitData.hpp"
#include "server/IRCServer.hpp"
#include "setup/SignalHandlers.hpp"

int main(int argc, const char** argv) {
	InitData initData(argc, argv);

	if (!initData.isValid()) {
		std::cout << "Could not initialize client.\n" << initData.getError() << std::endl;
		return 0;
	}
	std::cout << "Starting IRC Server..." << std::endl;
	std::cout << "- Port: " << initData.getPort() << std::endl;
	std::cout << "- Password: " << initData.getPassword() << std::endl;

	IRCServer server(initData.getPort(), initData.getPassword());
    registerSignals(&server);
	server.bind();
	std::cout << "Server socket bound" << std::endl;
	server.listen();
	std::cout << "Server listening on port " << initData.getPort() << std::endl;
	std::cout << "--- Start of Server Logs ---" << std::endl;
    int serverExitStatus = server.loop();
    std::cout << "Server exited with status " << serverExitStatus << std::endl;
    std::cout << "--- End of Server Logs ---" << std::endl;
    unregisterSignals();
	return serverExitStatus;
}
