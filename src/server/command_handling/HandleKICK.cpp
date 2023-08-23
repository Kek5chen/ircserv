#include "server/IRCServer.hpp"

bool IRCServer::handleKICK(IRCClient *client, const IRCCommand &cmd) {
	if (cmd.mParams.size() < 2)
		return true;
	const std::string &channel = cmd.mParams[0];
	const std::string &targetClientNick = cmd.mParams[1];
	std::string kickMessage;
	if (cmd.mParams.size() == 3)
		 kickMessage = cmd.mParams[2];
	mChannelManager.kick(client, channel, targetClientNick, kickMessage);
	std::string buf = client->mResponseBuffer;
	return true;
}
