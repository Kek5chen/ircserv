#include "server/IRCServer.hpp"
#include "server/CodeDefines.hpp"

bool isMatchRecursive(const std::string &s, const std::string &p, size_t i, size_t j) {
	// When the pattern is exhausted
	if (j == p.length()) return i == s.length();

	// Check for the current character match
	bool firstMatch = (i < s.length() && (s[i] == p[j] || p[j] == '.'));

	// Handling '*'
	if (j + 1 < p.length() && p[j + 1] == '*') {
		return (isMatchRecursive(s, p, i, j + 2) || // Zero occurrence of the preceding character
				(firstMatch && isMatchRecursive(s, p, i + 1, j))); // One or more occurrence of the preceding character
	} else {
		return firstMatch && isMatchRecursive(s, p, i + 1, j + 1);
	}
}

void IRCServer::handleWHO(IRCClient *client, const IRCCommand &cmd) {
	const std::string &name = cmd.mParams[0];
	const bool &o = cmd.mParams.size() > 1 && cmd.mParams[1] == "o";
	const std::map<std::string, const IRCChannel *> &channels = mChannelManager.getChannels();
	const IRCChannel *channel = mChannelManager.get(name);

	if (channel) {
		for (std::vector<const IRCClient *>::const_iterator client2 = channel->getClients().begin();
			 client2 != channel->getClients().end(); ++client2) {
			if (o && !channel->isOperator(*client2))
				continue;


			IRCServer::getResponseBase().setCommand(RPL_WHOREPLY)
				.addParam(client->getNickname())
				.addParam("#" + channel->getName())
				.addParam((*client2)->getUsername())
				.addParam((*client2)->getHostname())
				.addParam(IRCServer::getHostname())
				.addParam(client->getNickname())
				.addParam("H")
				.setEnd("0 " + (*client2)->getRealName())
				.sendTo(client);
		}
	} else {
		for (std::map<std::basic_string<char>, const IRCChannel *>::const_iterator it = channels.begin();
			 it != channels.end(); ++it) {
			const IRCChannel *channel = it->second;
			if (isMatchRecursive(channel->getName(), name, 0, 0)) {
				for (std::vector<const IRCClient *>::const_iterator client2 = channel->getClients().begin();
					 client2 != channel->getClients().end(); ++client2) {
					if (o && !channel->isOperator(*client2))
						continue;
					// TODO: if client is not invisible
					IRCServer::getResponseBase().setCommand(RPL_WHOREPLY)
						.addParam(client->getNickname())
						.addParam("#" + channel->getName())
						.addParam((*client2)->getUsername())
						.addParam((*client2)->getHostname())
						.addParam(IRCServer::getHostname())
						.addParam(client->getNickname())
						.addParam("H")
						.setEnd("0 " + (*client2)->getRealName())
						.sendTo(client);
				}
			}
		}
	}

	IRCServer::getResponseBase().setCommand(RPL_ENDOFWHO)
		.addParam(client->getNickname())
		.addParam(name)
		.setEnd("End of WHO list")
		.sendTo(client);

	// TODO: Idk if this works
}