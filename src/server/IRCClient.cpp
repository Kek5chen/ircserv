#include <unistd.h>
#include <stdexcept>
#include <sys/socket.h>
#include <iostream>
#include "server/IRCClient.hpp"

IRCClient::IRCClient(int socket_id) : mIsOpen(false), mPfd(), mIsRegistered(false),
									  mNickname(), mUsername(), mSuppliedPassword() {
	mSocketFd = socket_id;
	mIsOpen = mSocketFd >= 0;
	mPfd.events = POLLIN | POLLOUT;
	mPfd.fd = mSocketFd;
}

bool IRCClient::isValid() const {
	return mIsOpen;
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
	if (changed == -1)
		throw std::runtime_error("An error occurred while trying to handle a client");
	if (!changed)
		return 0;
	return mPfd.revents;
}

bool IRCClient::hasAccess(const std::string &pass) {
	return pass.empty() || pass == mSuppliedPassword;
}

void IRCClient::sendResponse(const std::string &str) {
	mResponseBuffer += str;
	mResponseBuffer += '\n';
}

bool IRCClient::flushResponse() {
	if (mResponseBuffer.empty())
		return true;
	std::cout << "[OUT] " << mResponseBuffer << std::endl;
	int result = send(mSocketFd, mResponseBuffer.data(), mResponseBuffer.size(), 0);
	mResponseBuffer.clear();
	return (size_t) result == mResponseBuffer.size();
}

const std::string &IRCClient::getNickname() {
	return mNickname;
}

const std::string &IRCClient::getUsername() {
	return mUsername;
}
