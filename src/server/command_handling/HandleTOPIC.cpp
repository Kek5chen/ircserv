#include "server/IRCServer.hpp"
#include "server/CodeDefines.hpp"

// TODO implement: Parameters: /topic <channel> [<topic>]
//		when JOINING a channel, TOPIC is send before NAMES list
//		when channel has +t flag only channel operator can change topic
// 		when channel has not t flag anyone can change topic
//		when there is only one parameter (channel) TOPIC of this channel is send to client
//		ERR_NEEDMOREPARAMS              ERR_NOTONCHANNEL
//      RPL_NOTOPIC                     RPL_TOPIC
//      ERR_CHANOPRIVSNEEDED

void IRCServer::handleTOPIC(IRCClient *client, const IRCCommand &cmd) {
	const std::string &channel = cmd.mParams[0];
	if (!mChannelManager.get(channel)) {
		sendErrorMessage(client, cmd, ERR_NOSUCHCHANNEL, channel + " :No such channel");
		return;
	}
	if (!mChannelManager.get(channel)->hasJoined(client)) {
		sendErrorMessage(client, cmd, ERR_NOTONCHANNEL, channel + " :You're not on that channel");
		return;
	}
	if (cmd.mParams.size() == 1) {
		if (!mChannelManager.printChannelTopic(channel))
			sendErrorMessage(client, cmd, RPL_NOTOPIC, channel + " :No topic is set");
	}
	else if (cmd.mParams.size() == 2) {
		if (!mChannelManager.setChannelTopic(channel, client, cmd.mParams[1]))
			sendErrorMessage(client, cmd, ERR_CHANOPRIVSNEEDED, channel + " :You're not channel operator");
	}
	else
		sendErrorMessage(client, cmd, ERR_NEEDMOREPARAMS, "Not enough parameters");
}