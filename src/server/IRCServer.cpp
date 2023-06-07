#include <sys/socket.h>
#include <stdexcept>
#include <csignal>
#include <netinet/in.h>
#include <climits>
#include <cerrno>
#include <iostream>
#include <algorithm>
#include <map>
#include "server/IRCServer.hpp"
#include "server/IRCClient.hpp"

bool IRCServer::m_cmd_handlers_init = false;
std::map<std::string, void(IRCServer::*)(IRCClient*, const std::string&)> IRCServer::m_cmd_handlers;

void IRCServer::init_cmd_handlers() {
	m_cmd_handlers["NICK"] = &IRCServer::handle_NICK;
	m_cmd_handlers["PASS"] = &IRCServer::handle_PASS;
	m_cmd_handlers["USER"] = &IRCServer::handle_USER;
	m_cmd_handlers_init = true;
}

IRCServer::IRCServer(unsigned short port, const std::string& password)
	: m_port(port), m_password(password), m_socket_fd(0), m_is_bound(false),
	  m_is_listening(false), m_should_stop(false) {
	if (!m_cmd_handlers_init)
		init_cmd_handlers();
}

IRCServer::~IRCServer() {
	for (size_t i = 0; i < m_clients.size(); i++)
		delete m_clients[i];
	m_clients.clear();
	if (m_socket_fd > 0)
		close(m_socket_fd);
}

void IRCServer::bind() {
	m_socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
	if (m_socket_fd < 0)
		throw std::runtime_error("Socket creation failed");

	sockaddr_in socketAddr = {};
	socketAddr.sin_family = AF_INET;
	socketAddr.sin_port = htons(m_port);
	socketAddr.sin_addr.s_addr = INADDR_ANY;
	int bindResult = ::bind(m_socket_fd, reinterpret_cast<struct sockaddr*>(&socketAddr), sizeof(socketAddr));
	if (bindResult < 0)
		throw std::runtime_error("Socket binding failed");
	m_is_bound = true;
}

void IRCServer::listen() {
	if (!m_is_bound)
		this->bind();
	if (::listen(m_socket_fd, INT_MAX) < 0)
		throw std::runtime_error("Could not make socket listen");
	m_is_listening = true;
}

void IRCServer::loop() {
	if (!m_is_listening)
		this->listen();
	while (!m_should_stop) {
		this->accept_new_clients();
		this->poll_clients();
	}
}

void IRCServer::accept_new_clients() {
	sockaddr_in clientAddr;
	int clientSocket;
	socklen_t clientAddrLen;

	while (true) {
		clientAddrLen = sizeof(clientAddr);
		clientSocket = accept(m_socket_fd, reinterpret_cast<struct sockaddr*>(&clientAddr), &clientAddrLen);
		if (clientSocket == -1 && errno == EWOULDBLOCK)
			return;
		std::cout << "Received client connection" << std::endl;
		IRCClient* client = new IRCClient(clientSocket);
		if (!client->is_valid()) {
			delete client;
			throw std::runtime_error(
					std::string("Unable to accept connection."));
		}
		m_clients.push_back(client);
	}
}

bool IRCServer::receive_data(IRCClient* client, std::string* buffer) {
	static char preBuf[MSG_BUFFER_SIZE];
	int received;

	do {
		received = recv(client->get_socket_fd(), preBuf, MSG_BUFFER_SIZE, 0);
		if (received == -1)
			throw std::runtime_error("An error occurred while trying to receive the sockets message.");
		if (!received) {
			std::cout << "Client disconnected" << std::endl;
			return false;
		}
		buffer->append(preBuf);
	} while (received == MSG_BUFFER_SIZE);
	return true;
}

bool IRCServer::handle(IRCClient* client) {
	std::string buf;
	short revents = client->poll();
	if (!revents)
		return false;
	if (!receive_data(client, &buf))
		return false;
	while (!buf.empty()) {
		size_t end = buf.find("\r\n");
		if (end == std::string::npos)
			break;
		std::string cmd = buf.substr(0, end);
		std::string keyword = buf.substr(0, cmd.find(' '));
		buf = buf.substr(end + 2);

		handler_map_type::iterator cmdIt = m_cmd_handlers.find(keyword);
		if (cmdIt == m_cmd_handlers.end()) {
			std::cout << keyword << " not implemented" << std::endl;
			continue;
		}
		if (keyword != "PASS" && !client->has_access(m_password))
			return false;
		(this->*(cmdIt->second))(client, cmd);
	}
	return true;
}

void IRCServer::poll_clients() {
	for (size_t i = 0; i < m_clients.size(); i++) {
		if (this->handle(m_clients[i]))
			continue;
		delete m_clients[i];
		m_clients.erase(std::remove(m_clients.begin(), m_clients.end(), m_clients[i]), m_clients.end());
		i--;
	}
}

void IRCServer::handle_PASS(IRCClient* client, const std::string& cmd) {
	size_t pos = cmd.find(' ');
	if (pos == std::string::npos)
		return;
	std::string pass = cmd.substr(pos + 1);
	if (pass.empty())
		return;
	client->m_supplied_password = pass;
	std::cout << "Password set to " << pass << std::endl;
}

void IRCServer::handle_NICK(IRCClient* client, const std::string& cmd) {
	size_t pos = cmd.find(' ');
	if (pos == std::string::npos)
		return;
	std::string nickname = cmd.substr(pos + 1);
	if (nickname.empty())
		return;
	client->m_nickname = nickname;
	std::cout << "Nickname set to " << nickname << std::endl;
	const std::string response = ":127.0.0.1 001 " + client->m_nickname +  " :Welcome to the ImKX IRC Server\n:server.example.com 376 \" + client->m_nickname + \" :End of MOTD";
	std::cout << response.data() << std::endl;
	int sent = send(client->get_socket_fd(), response.data(), response.size(), 0);
	if (sent == -1)
		throw std::runtime_error("Error when sending response");
}

void IRCServer::handle_USER(IRCClient *client, const std::string &cmd) {
	(void) cmd;
	(void) client;
}
