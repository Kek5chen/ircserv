#include "server/IRCServer.hpp"
#include "server/CodeDefines.hpp"

void IRCServer::handlePASS(IRCClient *client, const IRCCommand &cmd) {
	client->mSuppliedPassword = cmd.mParams[0];
	if (!client->hasAccess(mPassword))
		IRCServer::getResponseBase().setCommand(ERR_PASSWDMISMATCH)
				.setEnd("Incorrect Password")
				.sendTo(client);
}