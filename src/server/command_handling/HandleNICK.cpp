#include "server/IRCServer.hpp"
#include "server/CodeDefines.hpp"

bool IRCServer::handleNICK(IRCClient *client, const IRCCommand &cmd) {
	const std::string &newNickname = cmd.mParams[0];
	IRCCommand updateMsg = client->getResponseBase();
	bool wasRegistered = client->isRegistered();
	if (!client->isNicknameFree(newNickname) || newNickname == mBotNick) {
		IRCServer::getResponseBase().setCommand(ERR_NICKNAMEINUSE)
				.addParam(newNickname)
				.setEnd("Nickname is already in use")
				.sendTo(client);
		return true;
	}
	client->setNickname(cmd.mParams[0]);
	updateMsg.setCommand("NICK").setEnd(newNickname);
	client->send(updateMsg);
	if (!wasRegistered && client->isRegistered())
		sendMotd(client);
	else
		mChannelManager.sendToClientChannels(client, updateMsg.setCommand("NICK").setEnd(newNickname));
	return true;
}