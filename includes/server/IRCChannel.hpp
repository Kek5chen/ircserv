#pragma once

#include <string>
#include <vector>
#include "IRCClient.hpp"
#include "IRCCommand.hpp"
#include "IRCCommandEmitter.hpp"

class IRCChannel : public IRCCommandEmitter {
public:
	explicit IRCChannel(std::string name, IRCClient *creator);

	bool join(IRCClient *client);
	bool part(IRCClient *client);
	bool partAll();
	bool kick(IRCClient *sender, IRCClient *client, const std::string &reason);

	void send(const IRCCommand &message);
	void send(IRCClient *sender, const IRCCommand &message);

	bool hasJoined(IRCClient *client);
	bool isOperator(IRCClient *client);

	const std::string &getName();
	size_t getMemberCount();

	IRCClient *getClient(const std::string &basicString);

	void	setInviteOnly(bool inviteOnly);
	void	setTopicRestriction(bool topicRestricted);
	void	setPassword(const std::string &password);
	void	setUserLimit(int userLimit);
	void	addOperator(const std::string &nickname);
	void	removeOperator(const std::string &nickname);
	void	printChannelMode();

private:
	const std::string mName;
	std::vector<IRCClient *> mMembers;
	IRCClient *mCreator;
	bool	mInviteOnly;
	bool	mTopicRestricted;
	std::string	mPassword;
	std::vector<std::string>	mOperators;
	int	mUserLimit;
};
