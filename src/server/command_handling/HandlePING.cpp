#include "server/IRCServer.hpp"

void IRCServer::handlePING(IRCClient *client, const IRCCommand &cmd) {
	const std::string &add = cmd.mParams.empty() ? "" : cmd.mParams[0];
	IRCServer::getResponseBase().setCommand("PONG")
			.setEnd(add)
			.sendTo(client);
}
