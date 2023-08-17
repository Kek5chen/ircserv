#pragma once

#include <string>
#include <vector>
#include "IRCClient.hpp"
#include "IRCCommand.hpp"
#include "IRCCommandEmitter.hpp"

class IRCServer;

class IRCChannel : public IRCCommandEmitter, public IIRCServerOwned {
public:
	explicit IRCChannel(IRCServer *owningServer, std::string name, IRCClient *creator);

	bool join(IRCClient *client, const std::string &password = "");
	bool part(IRCClient *client);
	bool partAll();
	bool kick(IRCClient *sender, IRCClient *client, const std::string &reason);

	void send(const IRCCommand &message);
	void send(IRCClient *sender, const IRCCommand &message);

	bool hasJoined(const IRCClient *client) const;
	bool isOperator(const IRCClient *client) const;
	const std::vector<const IRCClient *> &getClients() const;

	const std::string &getName() const;
	size_t getMemberCount() const;

	IRCClient *getClient(const std::string &userName);

	void	setInviteOnly(bool inviteOnly);
	void	setTopicRestriction(bool topicRestricted);
	void	setPassword(const std::string &password);
	void	setUserLimit(int userLimit);
	void	addOperator(const std::string &nickname);
	void	removeOperator(const std::string &nickname);
	void	printChannelMode();

	bool	checkPermission(IRCClient *client, const std::string &password);

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
