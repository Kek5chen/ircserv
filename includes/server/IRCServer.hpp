#pragma once

#include <string>
#include <vector>
#include <map>
#include "IRCClient.hpp"
#include "IRCChannelManager.hpp"

#define MSG_BUFFER_SIZE 128

class IRCServer;
typedef std::map<std::string, void(IRCServer::*)(IRCClient*, const std::string&)> handler_map_type;

class IRCServer {
public:
	explicit IRCServer(unsigned short port, const std::string& password = "");
	~IRCServer();

	void bind();
	void listen();
	void loop();
	void stop();
private:
	void accept_new_clients();
	void poll_clients();
	bool receive_data(IRCClient* client, std::string* buffer);
	bool handle(IRCClient* client);

	void handle_PASS(IRCClient* client, const std::string& cmd);
	void handle_NICK(IRCClient* client, const std::string& cmd);
	void handle_USER(IRCClient* client, const std::string& cmd);
	void handle_PING(IRCClient* client, const std::string& cmd);
	void handle_JOIN(IRCClient* client, const std::string& cmd);
	void handle_PRIVMSG(IRCClient* client, const std::string& cmd);

	static void init_cmd_handlers();

	const unsigned short m_port;
	const std::string& m_password;
	int m_socket_fd;
	bool m_is_bound;
	bool m_is_listening;
	bool m_should_stop;

	std::vector<IRCClient*> m_clients;
	IRCChannelManager m_channel_manager;

	static handler_map_type m_cmd_handlers;
	static bool m_cmd_handlers_init;
};