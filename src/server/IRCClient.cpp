#include <csignal>
#include <stdexcept>
#include <sys/socket.h>
#include <iostream>
#include "server/IRCClient.hpp"

IRCClient::IRCClient(int socket_id) : m_is_open(false), m_pfd(), m_is_registered(false),
	m_nickname(), m_username(), m_supplied_password() {
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
	this->flush_response();
	int changed = ::poll(&m_pfd, 1, 0);
	if (changed == -1)
		throw std::runtime_error("An error occurred while trying to handle a client");
	if (!changed)
		return 0;
	return m_pfd.revents;
}

bool IRCClient::has_access(const std::string &pass) {
	return pass.empty() || pass == m_supplied_password;
}

void IRCClient::send_response(const std::string &str) {
	m_response_buffer += str;
	m_response_buffer += '\n';
}

bool IRCClient::flush_response() {
	if (m_response_buffer.empty())
		return true;
	std::cout << "[OUT] " << m_response_buffer << std::endl;
	int result = send(m_socket_fd, m_response_buffer.data(), m_response_buffer.size(), 0);
	if (result == -1)
		throw std::runtime_error("Error when sending response");
	m_response_buffer.clear();
	return (size_t)result == m_response_buffer.size();
}
