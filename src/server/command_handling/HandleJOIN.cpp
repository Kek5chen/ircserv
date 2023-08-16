#include "server/IRCServer.hpp"

void IRCServer::handleJOIN(IRCClient *client, const IRCCommand &cmd) {
	std::string channel = cmd.mParams[0];
	if (channel.empty()) {
		return;
	}
	// TODO: channel might be comma separated list of channels

	channel = channel.substr(channel[0] == '#');
	if (channel.empty())
		return;
	if (cmd.mParams.size() == 1)
		mChannelManager.join(channel, client, "");
	else if (cmd.mParams.size() >= 2)
		mChannelManager.join(channel, client, cmd.mParams[1]);
}
