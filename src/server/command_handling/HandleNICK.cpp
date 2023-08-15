#include "server/IRCServer.hpp"

void IRCServer::handleNICK(IRCClient *client, const IRCCommand &cmd) {
	client->setNickname(cmd.mParams[0]);
	if (client->mIsRegistered)
		return;
	sendMotd(client);
	client->mIsRegistered = true;
}