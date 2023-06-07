#include <csignal>
#include <pthread.h>
#include "server/IRCClient.hpp"

IRCClient::IRCClient(int socket_id) : m_is_open(false) {
	m_socket_fd = socket_id;
	m_is_open = m_socket_fd >= 0;
}

bool IRCClient::is_valid() const {
	return m_is_open;
}

IRCClient::~IRCClient() {
	close(m_socket_fd);
}

int IRCClient::get_socket_fd() {
	return m_socket_fd;
}
