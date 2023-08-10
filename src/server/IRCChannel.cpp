#include <algorithm>
#include <map>
#include "server/IRCChannel.hpp"

IRCChannel::IRCChannel(std::string name, IRCClient* creator) : mName(name), mCreator(creator) {}

bool IRCChannel::join(IRCClient* client) {
	if (std::find(mMembers.begin(), mMembers.end(), client) != mMembers.end())
		return false;
	mMembers.push_back(client);
	this->send(":" + client->getNickname() + "!" + client->getUsername() + "@127.0.0.1 JOIN :#" + mName); // TODO: Get Client Hostname
	std::string userList = ":127.0.0.1 353 " + client->getNickname() + " = #" + mName + " :";
	for (size_t i = 0; i < mMembers.size(); i++) {
		if (this->isOperator(mMembers[i]))
			userList += '@';
		userList += mMembers[i]->getNickname();
		if (i != mMembers.size() - 1)
			userList += ' ';
	}
	client->sendResponse(userList);
	const std::string userListEnd = ":127.0.0.1 366 " + client->getNickname() + " #" + mName + " :End of NAMES list";
	client->sendResponse(userListEnd);
	return true;
}

bool IRCChannel::part(IRCClient* client) {
	std::vector<IRCClient*>::iterator it = std::find(mMembers.begin(), mMembers.end(), client);
	if (it == mMembers.end())
		return false;
	this->send(":" + client->getNickname() + "!" + client->getUsername() + "@127.0.0.1 PART #" + mName); // TODO: Get Client Hostname
	mMembers.erase(it);
	return true;
}

bool IRCChannel::kick(IRCClient *client, const std::string &reason) {
	std::vector<IRCClient*>::iterator it = std::find(mMembers.begin(), mMembers.end(), client);
	if (it == mMembers.end())
		return false;
	this->send(":" + client->getNickname() + "!" + client->getUsername() + "@127.0.0.1 KICK #" + mName + " :" + reason); // TODO: Get Client Hostname
	mMembers.erase(it);
	return true;
}

bool IRCChannel::partAll() {
	bool result = true;
	for (std::vector<IRCClient*>::iterator it = mMembers.begin(); it < mMembers.end(); it++)
		this->part(*it);
	return result;
}

void IRCChannel::send(const std::string &message) {
	for (size_t i = 0; i < mMembers.size(); i++)
		mMembers[i]->sendResponse(message);
}

void IRCChannel::send(IRCClient* sender, const std::string &message) {
	for (size_t i = 0; i < mMembers.size(); i++)
		if (mMembers[i] != sender)
			mMembers[i]->sendResponse(message);
}

bool IRCChannel::hasJoined(IRCClient* client) {
	for (size_t i = 0; i < mMembers.size(); i++)
		if (mMembers[i] == client)
			return true;
	return false;
}

bool IRCChannel::isOperator(IRCClient* client) {
	return mCreator == client;
}

size_t IRCChannel::getMemberCount() {
	return mMembers.size();
}

const std::string &IRCChannel::getName() {
	return mName;
}

IRCClient* IRCChannel::getClient(const std::string &userName) {
    for (size_t i = 0; i < mMembers.size(); i++)
        if (mMembers[i]->getUsername() == userName)
            return mMembers[i];
    return 0;
}
