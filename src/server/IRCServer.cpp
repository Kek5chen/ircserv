#include <sys/socket.h>
#include <stdexcept>
#include <csignal>
#include <netinet/in.h>
#include <climits>
#include <cerrno>
#include <iostream>
#include <algorithm>
#include <sys/poll.h>
#include "server/IRCServer.hpp"
#include "server/IRCClient.hpp"

IRCServer::IRCServer(unsigned short port, const std::string& password)
	: m_port(port), m_password(password), m_socket_fd(0), m_is_bound(false),
	  m_is_listening(false), m_should_stop(false) {
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

void IRCServer::handle(IRCClient* client) {
	static char preBuf[MSG_BUFFER_SIZE];
	std::string buf;
	int received;
	short revents = client->poll();
	if (!revents)
		return;

	do {
		received = recv(client->get_socket_fd(), preBuf, MSG_BUFFER_SIZE, 0);
		if (received == -1)
			throw std::runtime_error("An error occurred while trying to receive the sockets message.");
		buf += preBuf;
	} while (received == MSG_BUFFER_SIZE);
	std::cout << "..." << buf << std::endl;
}

void IRCServer::poll_clients() {
	for (size_t i = 0; i < m_clients.size(); i++) {
		this->handle(m_clients[i]);
	}
}
