#pragma once

#include <string>
#include <vector>
#include "IRCClient.hpp"

class IRCCommand {
public:
	explicit IRCCommand(const std::string &commandStr);
	IRCCommand();
	~IRCCommand();

	std::string makePrefix() const;
	operator std::string() const;

	struct {
		std::string mHostname;
		std::string mUsername;
		std::string mHost;
	} mPrefix;
	bool mHasPrefix;
	struct {
		std::string mName;
		int mCode;
	} mCommand;
	std::vector<std::string> mParams;
	std::string mEnd;

	bool isValid() const;
	void sendTo(IRCClient &client) const;
};
