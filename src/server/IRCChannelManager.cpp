#include "server/IRCChannelManager.hpp"

IRCChannel* IRCChannelManager::get(const std::string& channelName) {
	if (m_channels.find(channelName) == m_channels.end())
		return 0;
	return m_channels[channelName];
}

IRCChannel* IRCChannelManager::get_or_create(const std::string& channelName) {
	if (m_channels.find(channelName) == m_channels.end())
		m_channels[channelName] = new IRCChannel(channelName);
	return m_channels[channelName];
}

bool IRCChannelManager::join(const std::string& channelName, IRCClient* client) {
	IRCChannel* channel = this->get_or_create(channelName);
	if (channel->has_joined(client))
		return false;
	return channel->join(client);
}

bool IRCChannelManager::part(const std::string &channelName, IRCClient *client) {
	IRCChannel* channel = this->get(channelName);
	if (!channel)
		return false;
	if (!channel->has_joined(client))
		return false;
	return channel->part(client);
}

void IRCChannelManager::part_from_all(IRCClient* client) {
	for (std::map<std::string, IRCChannel*>::iterator it = m_channels.begin(); it != m_channels.end(); it++)
		it->second->part(client);
}

void IRCChannelManager::send(const std::string& channelName, const std::string& message) {
	IRCChannel* channel = this->get(channelName);
	if (!channel)
		return;
	channel->send(message);
}

void IRCChannelManager::send(IRCClient* sender, const std::string& channelName, const std::string& message) {
	IRCChannel* channel = this->get(channelName);
	if (!channel)
		return;
	if (!channel->has_joined(sender))
		return;
	channel->send(sender, message);
}
