#pragma once

#include <string>
#include <map>
#include "IRCChannel.hpp"

class IRCChannelManager {
public:
	IRCChannel* get(const std::string& channelName);
	IRCChannel* get_or_create(const std::string& channelName, IRCClient* requester);
	bool join(const std::string& channelName, IRCClient* client);
	bool part(const std::string& channelName, IRCClient* client);
	void part_from_all(IRCClient* client);
	void send(const std::string& channelName, const std::string& message);
	void send(IRCClient* sender, const std::string& channelName, const std::string& message);
private:
	std::map<std::string, IRCChannel*> m_channels;
};
