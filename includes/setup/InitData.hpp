#pragma once

#include <string>

struct InitData {
	InitData(int argc, const char** argv);

	unsigned short get_port() const;
	const std::string& get_password() const;

	bool is_valid() const;
	const std::string &get_error() const;

private:
	unsigned short m_port;
	std::string m_password;
	bool m_valid;
	std::string m_error;
};