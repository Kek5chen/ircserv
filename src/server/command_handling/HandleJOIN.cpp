#include "server/IRCServer.hpp"

bool IRCServer::handleJOIN(IRCClient *client, const IRCCommand &cmd) {
	std::string channels = cmd.mParams[0];
	if (channels.empty()) {
		return true;
	}

	size_t pos = 0;
	while (pos < channels.size()) {
		size_t end = channels.find(',', pos);
		if (end == std::string::npos)
			end = channels.size();
		std::string channel = channels.substr(pos, end - pos);

		channel = channel.substr(channel[0] == '#');
		if (channel.empty())
			return true;
		if (cmd.mParams.size() == 1)
			mChannelManager.join(channel, client, "");
		else if (cmd.mParams.size() >= 2)
			mChannelManager.join(channel, client, cmd.mParams[1]);

		pos = end + 1;
	}
	return true;
}
