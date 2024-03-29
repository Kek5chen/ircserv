#include "server/IRCServer.hpp"

bool IRCServer::handleNOTICE(IRCClient *client, const IRCCommand &cmd) {
	const std::string &channel = cmd.mParams[0];
	const std::string &message = (cmd.mParams.size() > 1 ? cmd.mParams[1] : "") + cmd.mEnd;
	mChannelManager.send(client, channel.substr(1),
						 client->getResponseBase().setCommand("NOTICE")
								 .addParam(channel)
								 .setEnd(message));
	return true;
}
