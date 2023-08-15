#include "server/IRCServer.hpp"

void IRCServer::handlePRIVMSG(IRCClient *client, const IRCCommand &cmd) {
	const std::string &channel = cmd.mParams[0];
	const std::string &message = cmd.mEnd;
	mChannelManager.send(client, channel.substr(1),
						 client->getResponseBase().setCommand("PRIVMSG")
								 .addParam(channel)
								 .setEnd(message));
}
