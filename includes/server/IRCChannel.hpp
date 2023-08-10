#pragma once

#include <string>
#include <vector>
#include "IRCClient.hpp"

class IRCChannel {
public:
	explicit IRCChannel(std::string name, IRCClient* creator);

	bool join(IRCClient* client);
	bool part(IRCClient* client);
	bool partAll();
	bool kick(IRCClient *client, const std::string &reason);

	void send(const std::string& message);
	void send(IRCClient* sender, const std::string& message);

	bool hasJoined(IRCClient* client);
	bool isOperator(IRCClient* client);

	const std::string& getName();
	size_t getMemberCount();

    IRCClient* getClient(const std::string &basicString);

private:
	const std::string mName;
	std::vector<IRCClient*> mMembers;
	IRCClient* mCreator;
};
