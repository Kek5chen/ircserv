#include "server/CodeDefines.hpp"
#include "server/IRCServer.hpp"
#include <algorithm>

// TODO invitor must be channel op if the channel is invite only
// TODO channel must not exist or be valid
// TODO person beeing invited recieve a message nothing more
// TODO have a list of invited people for each channel or have a list of channels for each client
// TODO check if invitee is already on channel and even on server

void IRCServer::handleINVITE(IRCClient *client, const IRCCommand &cmd) {
	if (cmd.mParams.size() < 2) {
		sendErrorMessage(client, cmd, ERR_NEEDMOREPARAMS, "Not enough parameters");
		return;
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
		sendErrorMessage(client, cmd, ERR_NOSUCHNICK, invitee + " :No such nick");
		return;
	}
	IRCChannel *chan = mChannelManager.get(channel);
	if (!chan) {
		sendErrorMessage(client, cmd, ERR_NOSUCHCHANNEL, channel + " :No such channel");
		return;
	}
	if (chan->hasJoined(*it)) {
		sendErrorMessage(client, cmd, ERR_USERONCHANNEL, invitee + " #" + channel + " :is already on channel");
		return;
	}
	chan->addInvitedUser(invitee);
	client->getResponseBase().setCommand("INVITE")
		.addParam(invitee)
		.addParam(channel)
		.sendTo(client);
}