#pragma once

#include <string>
#include <vector>
#include "IRCClient.hpp"

class IRCChannel {
public:
	explicit IRCChannel(std::string name);

	bool join(IRCClient* client);
	bool part(IRCClient* client);

	void send(const std::string &message);
	void send(IRCClient* sender, const std::string &message);
private:
	const std::string m_name;
	std::vector<IRCClient*> m_members;
};
