#pragma once

#include <poll.h>
#include "IRCCommand.hpp"

class IRCClient {
	friend class IRCServer;

public:
	explicit IRCClient(int socket_id);
	~IRCClient();

	bool isValid() const;
	bool hasAccess(const std::string &pass);
	int getSocketFd();
	short poll();
	IRCCommand getResponseBase();
	void sendResponse(const std::string &str);
	bool flushResponse();

	void setNickname(const std::string &nick);
	void setUsername(const std::string &username);

	const std::string &getNickname();
	const std::string &getUsername();
private:
	int mSocketFd;
	bool mIsOpen;
	struct pollfd mPfd;
	std::string mResponseBuffer;

	bool mIsRegistered;
	std::string mNickname;
	std::string mUsername;
	std::string mSuppliedPassword;
	std::string mMode;
	std::string mRealName;
	std::string mHost;
	IRCCommand mBaseCommand;
};