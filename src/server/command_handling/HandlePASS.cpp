#include "server/IRCServer.hpp"
#include "server/CodeDefines.hpp"

bool IRCServer::handlePASS(IRCClient *client, const IRCCommand &cmd) {
	if (client->hasAccess() && !client->mSuppliedPassword.empty()) {
		IRCServer::getResponseBase().setCommand(ERR_ALREADYREGISTRED)
			.addParam(client->getNickname())
			.setEnd("You may not reregister")
			.sendTo(client);
		return true;
	}
	client->mSuppliedPassword = cmd.mParams[0];
	bool hasAccess = client->hasAccess();
	if (!hasAccess)
		IRCServer::getResponseBase().setCommand(ERR_PASSWDMISMATCH)
			.setEnd("Password incorrect")
			.sendTo(client);
	return hasAccess;
}
