#include <algorithm>
#include <map>
#include "server/IRCChannel.hpp"
#include "server/IRCCommand.hpp"
#include "server/IRCServer.hpp"
#include "server/CodeDefines.hpp"
#include "utils/FuckCast.hpp"

IRCChannel::IRCChannel(IRCServer *owningServer, std::string name, IRCClient *creator)
	: IIRCServerOwned(owningServer), mName(name), mCreator(creator), mInviteOnly(false), mTopicRestricted(false),
	  mPassword(""), mUserLimit(-1), mTopic("") {
	mOperators.push_back(creator->getNickname());
}

bool IRCChannel::join(IRCClient *client, const std::string &password) {
	if(!checkPermission(client, password))
		return false;
	if (std::find(mMembers.begin(), mMembers.end(), client) != mMembers.end()) {
		IRCServer::getResponseBase().setCommand(ERR_USERONCHANNEL)
			.addParam(client->getNickname())
			.addParam("#" + mName)
			.setEnd("is already on channel")
			.sendTo(client);
	}
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
	IRCServer::getResponseBase().setCommand(RPL_NAMREPLY)
		.addParam(client->getNickname())
		.addParam("=")
		.addParam("#" + mName)
		.setEnd(userList)
		.sendTo(client);
	IRCServer::getResponseBase().setCommand(RPL_ENDOFNAMES)
		.addParam("#" + mName)
		.setEnd("End of NAMES list")
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

bool IRCChannel::hasJoined(const IRCClient *client) const {
	for (size_t i = 0; i < mMembers.size(); i++)
		if (mMembers[i] == client)
			return true;
	return false;
}

bool IRCChannel::isOperator(const IRCClient *client) const {
	if (mCreator == client || std::find(mOperators.begin(), mOperators.end(), client->getNickname()) != mOperators.end())
		return true;
	return false;
}

size_t IRCChannel::getMemberCount() const {
	return mMembers.size();
}

const std::string &IRCChannel::getName() const {
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
	std::vector<std::string>::iterator it = std::find(mOperators.begin(), mOperators.end(), nickname);

	if (it == mOperators.end())
		mOperators.push_back(nickname);
}

void IRCChannel::removeOperator(const std::string &nickname) {
	std::vector<std::string>::iterator it = std::find(mOperators.begin(), mOperators.end(), nickname);

	if (it != mOperators.end())
		mOperators.erase(it);
}

void IRCChannel::printChannelMode() {
	std::string mode;

	if (mInviteOnly)
		mode += 'i';
	if (mTopicRestricted)
		mode += 't';
	if (!mPassword.empty())
		mode += 'k';
	if (mUserLimit != -1)
		mode += 'l';
	if (!mOperators.empty())
		mode += 'o';
	IRCServer::getResponseBase().setCommand(RPL_CHANNELMODEIS)
		.addParam(mName)
		.setEnd(mode)
		.sendTo(this);
}

bool IRCChannel::checkPermission(IRCClient *client, const std::string &password) {
	if (this->isOperator(client)) {
		return true;
	}
	if (mUserLimit != -1 && mMembers.size() >= (size_t)mUserLimit) {
		IRCServer::getResponseBase().setCommand(ERR_CHANNELISFULL)
			.addParam("#" + mName)
			.setEnd("Cannot join channel (+l)")
			.sendTo(client);
		return false;
	}
	if (password != mPassword) {
		IRCServer::getResponseBase().setCommand(ERR_BADCHANNELKEY)
			.addParam("#" + mName)
			.setEnd("Cannot join channel (+k)")
			.sendTo(client);
		return false;
	}
	if (mInviteOnly) {
		std::vector<std::string>::iterator it = std::find(mInvitedUsers.begin(), mInvitedUsers.end(), client->getNickname());
		if (it == mInvitedUsers.end()) {
			IRCServer::getResponseBase().setCommand(ERR_INVITEONLYCHAN)
				.addParam("#" + mName)
				.setEnd("Cannot join channel (+i)")
				.sendTo(client);
			return false;
		}
	}
	return true;
}

const std::vector<const IRCClient *> &IRCChannel::getClients() const {
	return std::fuck_cast<const std::vector<const IRCClient *> >(mMembers);
}

void IRCChannel::addInvitedUser(const std::string &nickname) {
	std::vector<std::string>::iterator it = std::find(mInvitedUsers.begin(), mInvitedUsers.end(), nickname);

	if (it == mInvitedUsers.end())
		mInvitedUsers.push_back(nickname);

}

bool IRCChannel::printChannelTopic() {
	if (mTopic.empty())
		return false;
	IRCServer::getResponseBase().setCommand("TOPIC")
		.addParam(mName)
		.addParam(mTopic)
		.sendTo(this);
	return true;
}

bool IRCChannel::setChannelTopic(IRCClient *client, const std::string &topic) {
	if (!mTopicRestricted || isOperator(client))
		mTopic = topic;
	else if (mTopicRestricted)
		return false;
	return true;
}
