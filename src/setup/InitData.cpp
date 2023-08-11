#include <stdexcept>
#include <sstream>
#include <climits>
#include "setup/InitData.hpp"

static bool isPortValid(const char *port) {
	if (!port)
		return false;
	while (*port) {
		if (!std::isdigit(*port))
			return false;
		port++;
	}
	return true;
}

InitData::InitData(int argc, const char **argv) : mValid(true) {
	if (argc < 3) {
		mError = "The program can not be started with less than 2 arguments.";
		mValid = false;
		return;
	}
	if (!isPortValid(argv[1])) {
		mError = "The supplied port was not a valid number.";
		mValid = false;
		return;
	}
	unsigned int port;
	std::istringstream ss(argv[1]);
	ss >> port;
	if (port > USHRT_MAX) {
		mError = "The supplied port was out of range.";
		mValid = false;
		return;
	}

	mPort = static_cast<unsigned short>(port);
	mPassword = argv[2];
}

unsigned short InitData::getPort() const {
	return mPort;
}

const std::string &InitData::getPassword() const {
	return mPassword;
}

bool InitData::isValid() const {
	return mValid;
}

const std::string &InitData::getError() const {
	return mError;
}
