#include <algorithm>
#include "server/IRCChannel.hpp"

IRCChannel::IRCChannel(std::string name) : m_name(name) {}

bool IRCChannel::join(IRCClient *client) {
	if (std::find(m_members.begin(), m_members.end(), client) != m_members.end())
		return false;
	m_members.push_back(client);
	return true;
}

bool IRCChannel::part(IRCClient *client) {
	if (std::find(m_members.begin(), m_members.end(), client) == m_members.end())
		return false;
	m_members.erase(std::remove(m_members.begin(), m_members.end(), client), m_members.end());
	return true;
}

void IRCChannel::send(const std::string &message) {
	for (size_t i = 0; i < m_members.size(); i++)
		m_members[i]->send_response(message);
}

void IRCChannel::send(IRCClient* sender, const std::string &message) {
	for (size_t i = 0; i < m_members.size(); i++)
		if (m_members[i] != sender)
			m_members[i]->send_response(message);
}
