#pragma once

class IRCClient {
public:
	explicit IRCClient(int socket_id);
	~IRCClient();

	bool is_valid() const;
	int get_socket_fd();
private:
	int m_socket_fd;
	bool m_is_open;
};