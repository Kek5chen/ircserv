#include "server/IRCServer.hpp"

void IRCServer::handleJOIN(IRCClient *client, const IRCCommand &cmd) {
	std::string channel = cmd.mParams[0];
	if (channel.empty()) {
		return;
	}

	channel = channel.substr(channel[0] == '#');
	if (channel.empty())
		return;
	mChannelManager.join(channel, client);
}
