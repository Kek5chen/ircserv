#pragma once

#include <poll.h>

class IRCClient {
	friend class IRCServer;
public:
	explicit IRCClient(int socket_id);
	~IRCClient();

	bool isValid() const;
	bool hasAccess(const std::string& pass);
	int getSocketFd();
	short poll();
	void sendResponse(const std::string& str);
	bool flushResponse();

	const std::string& getNickname();
	const std::string& getUsername();
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
};