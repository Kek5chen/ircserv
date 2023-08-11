#pragma once

#include <string>

struct InitData {
	InitData(int argc, const char **argv);

	unsigned short getPort() const;
	const std::string &getPassword() const;

	bool isValid() const;
	const std::string &getError() const;

private:
	unsigned short mPort;
	std::string mPassword;
	bool mValid;
	std::string mError;
};