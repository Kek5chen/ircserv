#include "server/IRCServer.hpp"
#include "server/CodeDefines.hpp"
#include <sstream>

void IRCServer::sendMotd(IRCClient *client) {
	IRCServer::getResponseBase().setCommand(RPL_WELCOME)
		.addParam(client->getNickname())
		.setEnd("- Welcome to ft_irc! Your host is " + mHost + ", running version "
															   IRC_VERSION " built on " __DATE__ " at " __TIME__)
		.sendTo(client);
	IRCServer::getResponseBase().setCommand(RPL_MOTDSTART)
		.addParam(client->getNickname())
		.setEnd("- ft_irc (mdoll, kschmidt) Message of the day - ")
		.sendTo(client);

	IRCServer::getResponseBase().setCommand(RPL_MOTD)
		.addParam(client->getNickname())
		.setEnd("- \xF0\x9F\x92\x80")
		.sendTo(client);

	std::string userAmount = ((std::ostringstream &) (std::ostringstream() << mClients.size())).str();
	IRCServer::getResponseBase().setCommand(RPL_LUSERCLIENT)
		.addParam(client->getNickname())
		.setEnd("- There are " + userAmount + " user(s) online")
		.sendTo(client);

	IRCServer::getResponseBase().setCommand(RPL_ENDOFMOTD)
		.addParam(client->getNickname())
		.setEnd("End of MOTD")
		.sendTo(client);
}

bool IRCServer::handleUSER(IRCClient *client, const IRCCommand &cmd) {
	const std::string &username = cmd.mParams[0];
	bool wasRegistered = client->isRegistered();
	client->setUsername(username);
	if (cmd.mParams.size() > 1)
		client->mMode = cmd.mParams[1];
	client->mRealName = cmd.mEnd;
	if (!wasRegistered && client->isRegistered())
		sendMotd(client);
	return true;
}
