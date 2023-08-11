#pragma once

#include <string>
#include <vector>

class IRCClient;

class IRCCommand {
public:
	explicit IRCCommand(const std::string &commandStr);
	IRCCommand();
	~IRCCommand();

	std::string makePrefix() const;
	operator std::string() const;

	IRCCommand &setHostname(const std::string &hostname);
	IRCCommand &setUsername(const std::string &username);
	IRCCommand &setHost(const std::string &host);
	IRCCommand &setCommand(const std::string &command);
	IRCCommand &setCommand(int command);
	IRCCommand &addParam(const std::string &param);
	IRCCommand &setEnd(const std::string &end);

	std::string getHostname() const;
	std::string getUsername() const;
	std::string getHost() const;
	std::string getCommand() const;
	std::vector<std::string> getParams() const;
	std::string getEnd() const;

	bool hasPrefix() const;

	struct {
		std::string mHostname;
		std::string mUsername;
		std::string mHost;
	} mPrefix;
	struct {
		std::string mName;
		int mCode;
	} mCommand;
	std::vector<std::string> mParams;
	std::string mEnd;

	bool isValid() const;
	void sendTo(IRCClient *client) const;
};
