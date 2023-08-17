#pragma once

#include <string>
#include <map>
#include "IRCChannel.hpp"

class IRCChannelManager : IIRCServerOwned {
public:
	IRCChannelManager(IRCServer *owningServer);
	IRCChannel *get(const std::string &channelName);
	IRCChannel *getOrCreate(const std::string &channelName, IRCClient *requester);

	bool remove(IRCChannel *channel);
	bool join(const std::string &channelName, IRCClient *client, std::string password);
	bool part(const std::string &channelName, IRCClient *client);
	bool kick(IRCClient *sender, const std::string &channelName, const std::string &userName, const std::string &reason);
	void partFromAll(IRCClient *client);
	void send(const std::string &channelName, const IRCCommand &message);
	void send(IRCClient *sender, const std::string &channelName, const IRCCommand &message);

	void setInviteOnly(const std::string &channelName, std::string flag);
	void setTopicRestriction(const std::string &channelName, std::string flag);
	void setPassword(const std::string &channelName, const std::string &password);
	void setUserLimit(const std::string &channelName, int userLimit);
	void addOperator(const std::string &channelName, const std::string &nickname);
	void removeOperator(const std::string &channelName, const std::string &nickname);

	const std::map<std::string, const IRCChannel *> &getChannels();

	bool printChannelMode(const std::string &channelName);

	bool isOperator(const std::string &channelName, IRCClient *client);

private:
	std::map<std::string, IRCChannel *> mChannels;
};
