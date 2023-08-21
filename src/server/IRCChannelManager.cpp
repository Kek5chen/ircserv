#include "server/IRCChannelManager.hpp"
#include "server/IRCServer.hpp"
#include "utils/FuckCast.hpp"

IRCChannelManager::IRCChannelManager(IRCServer *owningServer) : IIRCServerOwned(owningServer) {}

IRCChannel *IRCChannelManager::get(const std::string &channelName) {
	std::string rawName = channelName;
	if (rawName[0] == '#')
		rawName = rawName.substr(1);
	if (mChannels.find(rawName) == mChannels.end())
		return 0;
	return mChannels[rawName];
}

IRCChannelManager::~IRCChannelManager() {
	for (std::map<std::string, IRCChannel *>::iterator it = mChannels.begin(); it != mChannels.end(); it++) {
		it->second->partAll("Shutting down");
	}
}

IRCChannel *IRCChannelManager::getOrCreate(const std::string &channelName, IRCClient *requester) {
	if (mChannels.find(channelName) == mChannels.end()) {
		IRCChannel *channel = new IRCChannel(getServer(), channelName, requester);
		mChannels[channelName] = channel;
	}
	return mChannels[channelName];
}

bool IRCChannelManager::remove(IRCChannel *channel) {
	if (!channel->getMemberCount())
		for (size_t i = 0; i < channel->getMemberCount(); i++)
			channel->partAll("Channel Empty");
	if (!mChannels.erase(channel->getName()))
		return false;
	delete channel;
	return true;
}

bool IRCChannelManager::join(const std::string &channelName, IRCClient *client, std::string password) {
	IRCChannel *channel = this->getOrCreate(channelName, client);
	if (channel->hasJoined(client))
		return false;
	return channel->join(client, password);
}

bool IRCChannelManager::part(IRCClient *client, const std::string &channelName, const std::string &reason) {
	IRCChannel *channel = this->get(channelName);
	if (!channel)
		return false;
	if (!channel->hasJoined(client))
		return false;
	bool result = channel->part(client, reason);
	if (!channel->getMemberCount())
		this->remove(channel);
	return result;
}

bool IRCChannelManager::kick(
	IRCClient *sender, const std::string &channelName, const std::string &userName, const std::string &reason) {
	IRCChannel *channel = this->get(channelName);
	if (!channel)
		return false; // TODO: return error code ERR_NOSUCHCHANNEL
	if (!channel->isOperator(sender))
		return false; // TODO: return error code ERR_CHANOPRIVSNEEDED
	if (!channel->hasJoined(sender))
		return false; // TODO: return error code ERR_NOTONCHANNEL
	IRCClient *client = channel->getClient(userName);
	if (!client) {
		return false; // TODO: return error code ERR_NOSUCHNICK
	}
	if (!channel->hasJoined(client)) {
		return false; // TODO: return error code ERR_USERNOTINCHANNEL
	}
	bool status = channel->kick(sender, client, reason);
	if (!channel->getMemberCount())
		this->remove(channel);
	return status;
}

void IRCChannelManager::partFromAll(IRCClient *client, const std::string &reason) {
	for (std::map<std::string, IRCChannel *>::iterator it = mChannels.begin(); it != mChannels.end(); it++)
		it->second->part(client, reason);
}

void IRCChannelManager::send(const std::string &channelName, const IRCCommand &message) {
	IRCChannel *channel = this->get(channelName);
	if (!channel)
		return;
	message.sendTo(channel);
}

void IRCChannelManager::send(IRCClient *sender, const std::string &channelName, const IRCCommand &message) {
	IRCChannel *channel = this->get(channelName);
	if (!channel)
		return;
	if (!channel->hasJoined(sender))
		return;
	channel->send(sender, message);
}

void IRCChannelManager::setInviteOnly(const std::string &channelName, std::string flag) {
	IRCChannel *channel = this->get(channelName);
	if (!channel)
		return;
	channel->setInviteOnly(flag == "+i");
}

void IRCChannelManager::setTopicRestriction(const std::string &channelName, std::string flag) {
	IRCChannel *channel = this->get(channelName);
	if (!channel)
		return;
	channel->setTopicRestriction(flag == "+t");;
}

void IRCChannelManager::setPassword(const std::string &channelName, const std::string &password) {
	IRCChannel *channel = this->get(channelName);
	if (!channel)
		return;
	channel->setPassword(password);
}

void IRCChannelManager::setUserLimit(const std::string &channelName, int userLimit) {
	IRCChannel *channel = this->get(channelName);
	if (!channel)
		return;
	channel->setUserLimit(userLimit);
}

void IRCChannelManager::addOperator(const std::string &channelName, const std::string &nickname) {
	IRCChannel *channel = this->get(channelName);
	if (!channel)
		return;
	channel->addOperator(nickname);
}

void IRCChannelManager::removeOperator(const std::string &channelName, const std::string &nickname) {
	IRCChannel *channel = this->get(channelName);
	if (!channel)
		return;
	channel->removeOperator(nickname);
}

// TODO: remove this
bool IRCChannelManager::printChannelMode(IRCClient *client, const std::string &channelName) {
	IRCChannel *channel = this->get(channelName);
	if (!channel)
		return false;
	channel->printChannelMode(client);
    return true;
}

bool IRCChannelManager::isOperator(const std::string &channelName, IRCClient *client) {
	IRCChannel *channel = this->get(channelName);
	if (!channel)
		return false;
	return channel->isOperator(client);
}

const std::map<std::string, const IRCChannel *> &IRCChannelManager::getChannels() {
	return std::fuck_cast<const std::map<std::string, const IRCChannel *> >(mChannels);
}

bool IRCChannelManager::printChannelTopic(const std::string &channelName) {
	IRCChannel *channel = this->get(channelName);
	if (!channel)
		return false;
	return channel->printChannelTopic();
}

bool IRCChannelManager::setChannelTopic(const std::string &channelName, IRCClient *client, const std::string &topic) {
	IRCChannel *channel = this->get(channelName);
	if (!channel)
		return false;
	return channel->setChannelTopic(client, topic);
}
