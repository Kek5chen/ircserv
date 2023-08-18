#include "server/IRCServer.hpp"
#include "server/CodeDefines.hpp"

bool IRCServer::handlePASS(IRCClient *client, const IRCCommand &cmd) {
	if (client->hasAccess())
		IRCServer::getResponseBase().setCommand(ERR_ALREADYREGISTRED)
			.setEnd("You may not reregister")
			.sendTo(client);
	client->mSuppliedPassword = cmd.mParams[0];
	bool hasAccess = client->hasAccess();
	if (!hasAccess)
		IRCServer::getResponseBase().setCommand(ERR_PASSWDMISMATCH)
			.sendTo(client);
	return hasAccess;
}