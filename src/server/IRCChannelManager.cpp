#include "server/IRCChannelManager.hpp"

IRCChannel* IRCChannelManager::get(const std::string& channelName) {
	if (m_channels.find(channelName) == m_channels.end())
		return 0;
	return m_channels[channelName];
}

IRCChannel* IRCChannelManager::get_or_create(const std::string& channelName, IRCClient* requester) {
	if (m_channels.find(channelName) == m_channels.end()) {
		IRCChannel* channel = new IRCChannel(channelName, requester);
		m_channels[channelName] = channel;
	}
	return m_channels[channelName];
}

bool IRCChannelManager::remove(IRCChannel* channel) {
	if (!channel->get_member_count())
		for (size_t i = 0; i < channel->get_member_count(); i++)
			channel->part_all();
	if (!m_channels.erase(channel->get_name()))
		return false;
	delete channel;
	return true;
}

bool IRCChannelManager::join(const std::string& channelName, IRCClient* client) {
	IRCChannel* channel = this->get_or_create(channelName, client);
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
	bool result = channel->part(client);
	if (!channel->get_member_count())
		this->remove(channel);
	return result;
}

bool IRCChannelManager::kick(
		IRCClient* sender, const std::string &channelName, const std::string &userName, const std::string &reason) {
    IRCChannel* channel = this->get(channelName);
    if (!channel)
        return false;
    if (!channel->is_operator(sender))
        return false;
    IRCClient* client = channel->get_client(userName);
    if (!client)
	{
		sender->send_response(":server 401 " + sender->get_nickname() + " " + userName + " :No such nick/channel");
		return false;
	}
	if (!channel->has_joined(client))
	{
		sender->send_response(":server 441 " + sender->get_nickname() + " " + userName + " " + channelName + " :They aren't in this channel");
		return false;
	}
	bool status = channel->kick(client, reason);
	if (!channel->get_member_count())
		this->remove(channel);
	return status;
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
