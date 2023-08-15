#include "server/IRCServer.hpp"

void IRCServer::handlePASS(IRCClient *client, const IRCCommand &cmd) {
	client->mSuppliedPassword = cmd.mParams[0];
	if (!client->hasAccess(mPassword))
		IRCServer::getResponseBase().setCommand(464)
				.setEnd("Incorrect Password")
				.sendTo(client);
}