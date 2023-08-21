#include "server/CodeDefines.hpp"
#include "server/IRCServer.hpp"
#include <algorithm>

bool IRCServer::handleINVITE(IRCClient *client, const IRCCommand &cmd) {
	if (cmd.mParams.size() < 2) {
		client->sendErrorMessage(cmd.mCommand.mName, ERR_NEEDMOREPARAMS, "Not enough parameters");
		return true;
	}
	const std::string &channel = cmd.mParams[1];
	const std::string &invitee = cmd.mParams[0];

	std::vector<IRCClient *>::iterator it;
	for (it = mClients.begin(); it != mClients.end(); ++it) {
		if ((*it)->getNickname() == invitee) {
			break;
		}
	}
	if (it == mClients.end()) {
		client->sendErrorMessage(cmd.mCommand.mName, ERR_NOSUCHNICK, invitee + " :No such nick");
		return true;
	}
	IRCChannel *chan = mChannelManager.get(channel);
	if (!chan) {
		client->sendErrorMessage(cmd.mCommand.mName, ERR_NOSUCHCHANNEL, channel + " :No such channel");
		return true;
	}
	if (chan->hasJoined(*it)) {
		client->sendErrorMessage(cmd.mCommand.mName, ERR_USERONCHANNEL, invitee + " #" + channel + " :is already on channel");
		return true;
	}
	chan->addInvitedUser(invitee);
	client->getResponseBase().setCommand("INVITE")
		.addParam(invitee)
		.addParam(channel)
		.sendTo(*it);
	client->getResponseBase().setCommand(RPL_INVITING)
		.addParam(client->getNickname())
		.addParam(invitee)
		.addParam(channel)
		.sendTo(client);
	return true;
}