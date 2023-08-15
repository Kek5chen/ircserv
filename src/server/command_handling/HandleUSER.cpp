#include "server/IRCServer.hpp"

void IRCServer::handleUSER(IRCClient *client, const IRCCommand &cmd) {
	client->setUsername(cmd.mParams[0]);
	if (cmd.mParams.size() > 1)
		client->mMode = cmd.mParams[1];
	client->mRealName = cmd.mEnd;
}
