#include "server/IRCServer.hpp"

void IRCServer::handlePART(IRCClient *client, const IRCCommand &cmd) {
	const std::string &channel = cmd.mParams[0];
	mChannelManager.part(channel, client);
}
