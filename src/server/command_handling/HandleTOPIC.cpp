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


// TODO
//		no quotes: mParams[0] = channel name
//		mEnd = topic
//		no additional parameters: mParams.size() == 1 && mEnd.empty() -> mParams[0] = channel name
// 		topic with quotes: mParams[0] = channel name, mEnd = topic with some funky symbols

// TODO topic with only one word i considered an error -> should not be like this :(
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
	else if (cmd.mParams.size() == 1 && !cmd.mEnd.empty()) {
		if (!mChannelManager.setChannelTopic(channel, client, cmd.mEnd)) {
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