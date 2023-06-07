#include <csignal>
#include <stdexcept>
#include "server/IRCClient.hpp"

IRCClient::IRCClient(int socket_id) : m_is_open(false) {
	m_socket_fd = socket_id;
	m_is_open = m_socket_fd >= 0;
	m_pfd.events = POLLIN | POLLOUT;
	m_pfd.fd = m_socket_fd;
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

short IRCClient::poll() {
	int changed = ::poll(&m_pfd, 1, 0);
	if (changed == -1)
		throw std::runtime_error("An error occurred while trying to handle a client");
	if (!changed)
		return 0;
	return m_pfd.revents;
}
