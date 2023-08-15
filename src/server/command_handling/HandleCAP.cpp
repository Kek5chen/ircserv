#include "server/IRCServer.hpp"
#include <iostream>

void IRCServer::handleCAP(IRCClient *client, const IRCCommand &cmd) {
	(void) client;
	(void) cmd;
	std::cout << "[INFO] Ignore Capability Negotiation" << std::endl;
}