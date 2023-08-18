#pragma once

#include <string>

struct InitData {
	InitData(int argc, const char **argv);

	unsigned short getPort() const;
	const std::string &getPassword() const;
	const std::string &getIp() const;

	bool isValid() const;
	const std::string &getError() const;

private:
	unsigned short mPort;
	std::string mPassword;
	std::string mIp;
	bool mValid;
	std::string mError;
};