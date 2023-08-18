#include "server/IRCServer.hpp"
#include "utils/Logger.hpp"
#include <iostream>

bool IRCServer::handleQUIT(IRCClient *client, const IRCCommand &cmd) {
	client->mQuitReason = cmd.mEnd;
	return false;
}