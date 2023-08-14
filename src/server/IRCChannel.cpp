#include <algorithm>
#include <map>
#include "server/IRCChannel.hpp"
#include "server/IRCCommand.hpp"
#include "server/IRCServer.hpp"

IRCChannel::IRCChannel(std::string name, IRCClient *creator) : mName(name), mCreator(creator), mInviteOnly(false), mTopicRestricted(false),
																	mPassword(""), mUserLimit(-1){}

bool IRCChannel::join(IRCClient *client) {
	if (std::find(mMembers.begin(), mMembers.end(), client) != mMembers.end())
		return false; // TODO: return error code
	mMembers.push_back(client);
	client->getResponseBase().setCommand("JOIN")
		.addParam("#" + mName)
		.sendTo(this);
	std::string userList;
	for (size_t i = 0; i < mMembers.size(); i++) {
		if (this->isOperator(mMembers[i]))
			userList += '@';
		userList += mMembers[i]->getNickname();
		if (i != mMembers.size() - 1)
			userList += ' ';
	}
	IRCServer::getResponseBase().setCommand(353)
		.addParam("#" + mName)
		.setEnd(userList)
		.sendTo(client);
	IRCServer::getResponseBase().setCommand(366)
		.addParam("#" + mName)
		.setEnd("End of /NAMES list")
		.sendTo(client);
	return true;
}

bool IRCChannel::part(IRCClient *client) {
	std::vector<IRCClient *>::iterator it = std::find(mMembers.begin(), mMembers.end(), client);
	if (it == mMembers.end())
		return false; // TODO: return error code ERR_NEEDMOREPARAMS

	client->getResponseBase().setCommand("PART")
		.addParam("#" + mName)
		.sendTo(this); // TODO: Add reason as end
	mMembers.erase(it);
	return true;
}

bool IRCChannel::kick(IRCClient *sender, IRCClient *client, const std::string &reason) {
	std::vector<IRCClient *>::iterator it = std::find(mMembers.begin(), mMembers.end(), client);
	if (it == mMembers.end())
		return false; // TODO: return error code ERR_NOTONCHANNEL
	sender->getResponseBase().setCommand("KICK")
		.addParam("#" + mName)
		.addParam(client->getNickname())
		.setEnd(reason)
		.sendTo(this);
	mMembers.erase(it);
	return true;
}

bool IRCChannel::partAll() {
	bool result = true;
	for (std::vector<IRCClient *>::iterator it = mMembers.begin(); it < mMembers.end(); it++)
		this->part(*it);
	return result;
}

void IRCChannel::send(const IRCCommand &message) {
	for (size_t i = 0; i < mMembers.size(); i++)
		message.sendTo(mMembers[i]);
}

void IRCChannel::send(IRCClient *sender, const IRCCommand &message) {
	for (size_t i = 0; i < mMembers.size(); i++)
		if (mMembers[i] != sender)
			message.sendTo(mMembers[i]);
}

bool IRCChannel::hasJoined(IRCClient *client) {
	for (size_t i = 0; i < mMembers.size(); i++)
		if (mMembers[i] == client)
			return true;
	return false;
}

bool IRCChannel::isOperator(IRCClient *client) {
	return mCreator == client;
}

size_t IRCChannel::getMemberCount() {
	return mMembers.size();
}

const std::string &IRCChannel::getName() {
	return mName;
}

IRCClient *IRCChannel::getClient(const std::string &userName) {
	for (size_t i = 0; i < mMembers.size(); i++)
		if (mMembers[i]->getUsername() == userName)
			return mMembers[i];
	return 0;
}

void IRCChannel::setInviteOnly(bool inviteOnly) {
	mInviteOnly = inviteOnly;
}

void IRCChannel::setTopicRestriction(bool topicRestricted) {
	mTopicRestricted = topicRestricted;
}

void IRCChannel::setPassword(const std::string &password) {
	mPassword = password;
}

void IRCChannel::setUserLimit(int userLimit) {
	mUserLimit = userLimit;
}

void IRCChannel::addOperator(const std::string &nickname) {
	mOperators.push_back(nickname);
}

void IRCChannel::removeOperator(const std::string &nickname) {
	std::vector<std::string>::iterator it = std::find(mOperators.begin(), mOperators.end(), nickname);

	if (it != mOperators.end())
		mOperators.erase(it);
}




