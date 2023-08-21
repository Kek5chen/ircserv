#include "server/IRCServer.hpp"
#include "server/CodeDefines.hpp"

bool IRCServer::handleTOPIC(IRCClient *client, const IRCCommand &cmd) {
	const std::string &channel = cmd.mParams[0];
	if (!mChannelManager.get(channel)) {
		client->sendErrorMessage(cmd, ERR_NOSUCHCHANNEL, channel + " :No such channel");
		return true;
	}
	if (!mChannelManager.get(channel)->hasJoined(client)) {
		client->sendErrorMessage(cmd, ERR_NOTONCHANNEL, channel + " :You're not on that channel");
		return true;
	}
	if (cmd.mParams.size() == 1 && cmd.mEnd.empty()) {
		if (!mChannelManager.printChannelTopic(client, channel)) {
			client->sendErrorMessage(cmd, RPL_NOTOPIC, channel + " :No topic is set");
			return true;
		}
	}
	else if ((cmd.mParams.size() == 1 && !cmd.mEnd.empty()) || (cmd.mParams.size() == 2 && cmd.mEnd.empty())) {
		std::string topic;
		if (cmd.mParams.size() == 2)
			topic = cmd.mParams[1];
		else
			topic = cmd.mEnd;
		if (!mChannelManager.setChannelTopic(channel, client, topic)) {
			client->sendErrorMessage(cmd, ERR_CHANOPRIVSNEEDED, channel + " :You're not channel operator");
			return true;
		}
	}
	else {
		client->sendErrorMessage(cmd, ERR_NEEDMOREPARAMS, "Not enough parameters");
		return true;
	}
	return true;
}