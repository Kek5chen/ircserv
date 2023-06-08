#include <algorithm>
#include "server/IRCChannel.hpp"

IRCChannel::IRCChannel(std::string name) : m_name(name) {}

bool IRCChannel::join(IRCClient* client) {
	if (std::find(m_members.begin(), m_members.end(), client) != m_members.end())
		return false;
	m_members.push_back(client);
	this->send(":" + client->get_nickname() + "!" + client->get_username() + "@127.0.0.1 JOIN :#" + m_name); // TODO: Get Client Hostname
	std::string userList = ":127.0.0.1 353 " + client->get_nickname() + " = #" + m_name + " :";
	for (size_t i = 0; i < m_members.size(); i++) {
		userList += m_members[i]->get_nickname();
		if (i != m_members.size() - 1)
			userList += ' ';
	}
	client->send_response(userList);
	const std::string userListEnd = ":127.0.0.1 366 " + client->get_nickname() + " #" + m_name + " :End of NAMES list";
	client->send_response(userListEnd);
	return true;
}

bool IRCChannel::part(IRCClient* client) {
	if (std::find(m_members.begin(), m_members.end(), client) == m_members.end())
		return false;
	this->send(":" + client->get_nickname() + "!" + client->get_username() + "@127.0.0.1 PART #" + m_name); // TODO: Get Client Hostname
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

bool IRCChannel::has_joined(IRCClient* client) {
	for (size_t i = 0; i < m_members.size(); i++)
		if (m_members[i] == client)
			return true;
	return false;
}
