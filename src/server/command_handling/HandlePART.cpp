#include "server/IRCServer.hpp"

bool IRCServer::handlePART(IRCClient *client, const IRCCommand &cmd) {
	const std::string &channel = cmd.mParams[0];
	const std::string &reason = cmd.mEnd;
	mChannelManager.part(client, channel, reason);
	return true;
}
