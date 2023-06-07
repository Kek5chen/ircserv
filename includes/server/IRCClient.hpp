#pragma once

#include <poll.h>

class IRCClient {
	friend class IRCServer;
public:
	explicit IRCClient(int socket_id);
	~IRCClient();

	bool is_valid() const;
	bool has_access(const std::string& pass);
	int get_socket_fd();
	short poll();
	void send_response(const std::string& str);
	bool flush_response();
private:
	int m_socket_fd;
	bool m_is_open;
	struct pollfd m_pfd;
	std::string m_response_buffer;

	std::string m_nickname;
	std::string m_supplied_password;
};