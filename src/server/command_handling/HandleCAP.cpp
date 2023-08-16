#include "server/IRCServer.hpp"
#include "utils/Logger.hpp"
#include <iostream>

void IRCServer::handleCAP(IRCClient *client, const IRCCommand &cmd) {
	(void) client;
	(void) cmd;
	LOG("[INFO] Ignore Capability Negotiation");
}