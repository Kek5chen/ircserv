#include "server/IRCServer.hpp"
#include "utils/Logger.hpp"
#include <iostream>

bool IRCServer::handleCAP(IRCClient *client, const IRCCommand &cmd) {
	(void) client;
	(void) cmd;
	LOG(YELLOW("[WARN] Ignore Capability Negotiation"));
	return true;
}