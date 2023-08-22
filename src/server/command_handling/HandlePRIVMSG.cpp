#include "server/IRCServer.hpp"

bool IRCServer::handlePRIVMSG(IRCClient *client, const IRCCommand &cmd) {
	if (cmd.mParams[0] == "bottich") {
		if (cmd.mParams.size() == 1)
			return botResponse(client, IRCCommand(cmd.mEnd));
		else
			return botResponse(client, IRCCommand(cmd.mParams[1]));
	}
	const std::string &channel = cmd.mParams[0];
	const std::string &message = (cmd.mParams.size() > 1 ? cmd.mParams[1] : "") + cmd.mEnd;
	mChannelManager.send(client, channel.substr(1),
						 client->getResponseBase().setCommand("PRIVMSG")
								 .addParam(channel)
								 .setEnd(message));
	return true;
}
