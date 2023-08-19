#include "server/IRCServer.hpp"
#include "server/CodeDefines.hpp"

bool IRCServer::handleNICK(IRCClient *client, const IRCCommand &cmd) {
	const std::string &username = cmd.mParams[0];
	if (!client->isNicknameFree(username)) {
		sendErrorMessage(client, cmd, ERR_NICKNAMEINUSE, username + " :Nickname is already in use");
		return true;
	}
	client->setNickname(cmd.mParams[0]);
	if (!client->mUsername.empty())
		client->mIsRegistered = true;
	return true;
}