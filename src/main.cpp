#include <iostream>
#include "setup/InitData.hpp"
#include "server/IRCServer.hpp"
#include "setup/SignalHandlers.hpp"
#include "utils/Logger.hpp"

int main(int argc, const char **argv) {
	int serverExitStatus = 0;
	try {
		InitData initData(argc, argv);
		if (!initData.isValid()) {
			LOG(RED("Could not initialize client.\n") << RED(initData.getError()));
			return 0;
		}

		LOG(YELLOW("Starting IRC Server..."));
		LOG("- Port: " << initData.getPort());
		LOG("- Password: " << initData.getPassword());
		IRCServer server(initData.getPort(), initData.getPassword(), initData.getIp());
		registerSignals(&server);
		server.bind();
		LOG(GREEN("Server socket bound"));

		server.listen();
		LOG("Server listening on port " << initData.getPort());

		LOG(MAGENTA("--- Start of Server Logs ---"));

		serverExitStatus = server.loop();
	} catch (const std::exception &e) {
		LOG(RED("An error occurred: ") << RED(e.what()));
	}
	LOG("Server exited with status " << serverExitStatus);
	LOG(MAGENTA("--- End of Server Logs ---"));

	unregisterSignals();
	return serverExitStatus;
}
