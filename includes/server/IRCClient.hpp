#pragma once

#include <poll.h>
#include "IRCCommand.hpp"
#include "IRCCommandEmitter.hpp"
#include "IIRCServerOwned.hpp"

class IRCClient : public IRCCommandEmitter, public IIRCServerOwned {
	friend class IRCServer;

public:
	explicit IRCClient(IRCServer *owningServer, int socket_id);
	~IRCClient();

	bool isValid() const;
	bool hasAccess() const;
	int getSocketFd();
	short poll();
	void send(const IRCCommand &command);
	void sendResponse(const std::string &str);
	bool flushResponse();

	void setNickname(const std::string &nick);
	void setUsername(const std::string &username);

	const std::string &getNickname() const;
	const std::string &getUsername() const;
	const std::string &getHostname() const;
	const std::string &getRealName() const;
	IRCCommand getResponseBase() const;
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
	std::string mQuitReason;
};