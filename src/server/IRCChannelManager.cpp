#include "server/IRCChannelManager.hpp"
#include "server/IRCServer.hpp"

IRCChannel *IRCChannelManager::get(const std::string &channelName) {
	std::string rawName = channelName;
	if (rawName[0] == '#')
		rawName = rawName.substr(1);
	if (mChannels.find(rawName) == mChannels.end())
		return 0;
	return mChannels[rawName];
}

IRCChannel *IRCChannelManager::getOrCreate(const std::string &channelName, IRCClient *requester) {
	if (mChannels.find(channelName) == mChannels.end()) {
		IRCChannel *channel = new IRCChannel(channelName, requester);
		mChannels[channelName] = channel;
	}
	return mChannels[channelName];
}

bool IRCChannelManager::remove(IRCChannel *channel) {
	if (!channel->getMemberCount())
		for (size_t i = 0; i < channel->getMemberCount(); i++)
			channel->partAll();
	if (!mChannels.erase(channel->getName()))
		return false;
	delete channel;
	return true;
}

bool IRCChannelManager::join(const std::string &channelName, IRCClient *client) {
	IRCChannel *channel = this->getOrCreate(channelName, client);
	if (channel->hasJoined(client))
		return false;
	return channel->join(client);
}

bool IRCChannelManager::part(const std::string &channelName, IRCClient *client) {
	IRCChannel *channel = this->get(channelName);
	if (!channel)
		return false;
	if (!channel->hasJoined(client))
		return false;
	bool result = channel->part(client);
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

void IRCChannelManager::partFromAll(IRCClient *client) {
	for (std::map<std::string, IRCChannel *>::iterator it = mChannels.begin(); it != mChannels.end(); it++)
		it->second->part(client);
}

void IRCChannelManager::send(const std::string &channelName, const IRCCommand &message) {
	IRCChannel *channel = this->get(channelName);
	if (!channel)
		return;
	channel->send(message);
}

void IRCChannelManager::send(IRCClient *sender, const std::string &channelName, const IRCCommand &message) {
	IRCChannel *channel = this->get(channelName);
	if (!channel)
		return;
	if (!channel->hasJoined(sender))
		return;
	channel->send(sender, message);
}
