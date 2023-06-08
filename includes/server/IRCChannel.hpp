#pragma once

#include <string>
#include <vector>
#include "IRCClient.hpp"

class IRCChannel {
public:
	explicit IRCChannel(std::string name, IRCClient* creator);

	bool join(IRCClient* client);
	bool part(IRCClient* client);
	bool part_all();

	void send(const std::string& message);
	void send(IRCClient* sender, const std::string& message);

	bool has_joined(IRCClient* client);
	bool is_operator(IRCClient* client);

	const std::string& get_name();
	size_t get_member_count();
private:
	const std::string m_name;
	std::vector<IRCClient*> m_members;
	IRCClient* m_creator;
};
