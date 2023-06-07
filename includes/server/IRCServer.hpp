#pragma once

#include <string>
#include <vector>
#include "IRCClient.hpp"

class IRCServer {
public:
	explicit IRCServer(unsigned short port, const std::string& password = "");
	~IRCServer();

	void bind();
	void listen();
	void loop();

private:
	void accept_new_clients();
	const unsigned short m_port;
	const std::string& m_password;
	int m_socket_fd;
	bool m_is_bound;
	bool m_is_listening;
	bool m_should_stop;

	std::vector<IRCClient*> m_clients;

	void poll_clients();
};