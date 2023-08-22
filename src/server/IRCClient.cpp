#include <unistd.h>
#include <stdexcept>
#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <netdb.h>
#include "server/IRCClient.hpp"
#include "utils/Logger.hpp"
#include "server/IRCServer.hpp"

IRCClient::IRCClient(IRCServer *owningServer, int socket_id) : IIRCServerOwned(owningServer), mIsOpen(false), mPfd(),
															   mNickname(), mUsername(), mSuppliedPassword(),
															   mQuitReason("Client disconnected") {
	mSocketFd = socket_id;
	mIsOpen = mSocketFd >= 0;
	mPfd.events = POLLIN | POLLOUT;
	mPfd.fd = mSocketFd;

	// ip from socket
	sockaddr_in addr = {};
	socklen_t addr_len = sizeof(addr);
	if (getpeername(mSocketFd, reinterpret_cast<sockaddr *>(&addr), &addr_len) == -1) {
		mHost = "unknown";
		return;
	}

	// ip to hostname
	char host[NI_MAXHOST];
	if (getnameinfo(reinterpret_cast<sockaddr *>(&addr), addr_len, host, sizeof(host), 0, 0, 0) == -1) {
		mHost = "unknown";
		return;
	}
	mHost = host;

	mBaseCommand.mPrefix.mHostname = mNickname;
	mBaseCommand.mPrefix.mUsername = mUsername;
	mBaseCommand.mPrefix.mHost = mHost;
}

bool IRCClient::isValid() const {
	return mIsOpen;
}

bool IRCClient::isRegistered() const {
	return hasAccess() && !mNickname.empty() && !mUsername.empty();
}

IRCClient::~IRCClient() {
	close(mSocketFd);
}

int IRCClient::getSocketFd() {
	return mSocketFd;
}

short IRCClient::poll() {
	this->flushResponse();
	int changed = ::poll(&mPfd, 1, 0);
	if (changed == -1) // Monitor this error. Might be better to just disconnect the client if it's too frequent.
		throw std::runtime_error("An error occurred while trying to handle a client");
	if (!changed)
		return 0;
	return mPfd.revents;
}

bool IRCClient::hasAccess() const {
	const std::string &password = getServer()->getPassword();
	return password.empty() || password == mSuppliedPassword;
}

void IRCClient::send(const IRCCommand &command) {
	this->sendResponse(command);
}

void IRCClient::sendResponse(const std::string &str) {
	mResponseBuffer += str;
	mResponseBuffer += '\n';
}

bool IRCClient::flushResponse() {
	if (mResponseBuffer.empty())
		return true;
	if (!mIsOpen) {
		return false;
	}

	ssize_t result = ::send(mSocketFd, mResponseBuffer.data(), mResponseBuffer.size(), 0);
	if (result == -1)
		return false;
	LOG(BLUE("[OUT] ") << BLUE(mResponseBuffer));
	mResponseBuffer.clear();
	return (size_t) result == mResponseBuffer.size();
}

void IRCClient::setNickname(const std::string &nick) {
	mNickname = nick;
	mBaseCommand.mPrefix.mHostname = mNickname;
}

void IRCClient::setUsername(const std::string &username) {
	mUsername = username;
	mBaseCommand.mPrefix.mUsername = mUsername;
}

const std::string &IRCClient::getNickname() const {
	return mNickname;
}

const std::string &IRCClient::getUsername() const {
	return mUsername;
}

IRCCommand IRCClient::getResponseBase() const {
	return mBaseCommand;
}

void IRCClient::sendErrorMessage(const std::string &command, int errorCode, const std::string &msg) {
	IRCServer::getResponseBase().setCommand(errorCode)
		.addParam(mNickname)
		.addParam(command)
		.setEnd(msg)
		.sendTo(this);
}

const std::string &IRCClient::getHostname() const {
	return mHost;
}

const std::string &IRCClient::getRealName() const {
	return mRealName;
}

bool IRCClient::isNicknameFree(const std::string &nickname) {
	if (nickname == mNickname)
		return true;
	const std::vector<const IRCClient *> &clients = getServer()->getClients();
	for (std::vector<const IRCClient *>::const_iterator it = clients.begin(); it != clients.end(); it++) {
		if ((*it)->getNickname() == nickname)
			return false;
	}
	return true;
}
