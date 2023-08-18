#include "server/IRCServer.hpp"

bool IRCServer::handleNICK(IRCClient *client, const IRCCommand &cmd) {
	client->setNickname(cmd.mParams[0]);
	if (client->mIsRegistered)
		return true;
	sendMotd(client);
	client->mIsRegistered = true;
	return true;
}