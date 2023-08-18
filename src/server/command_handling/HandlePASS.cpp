#include "server/IRCServer.hpp"
#include "server/CodeDefines.hpp"

bool IRCServer::handlePASS(IRCClient *client, const IRCCommand &cmd) {
	client->mSuppliedPassword = cmd.mParams[0];
	bool hasAccess = client->hasAccess();
	if (!hasAccess)
		IRCServer::getResponseBase().setCommand(ERR_PASSWDMISMATCH)
				.setEnd("Incorrect Password")
				.sendTo(client);
	return hasAccess;
}