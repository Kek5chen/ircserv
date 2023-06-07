#include <stdexcept>
#include <sstream>
#include <climits>
#include "setup/InitData.hpp"

static bool is_port_valid(const char* port) {
	if (!port)
		return false;
	while (*port) {
		if (!std::isdigit(*port))
			return false;
		port++;
	}
	return true;
}

InitData::InitData(int argc, const char **argv) : m_valid(true) {
	if (argc < 3) {
		m_error = "The program can not be started with less than 2 arguments.";
		m_valid = false;
		return;
	}
	if (!is_port_valid(argv[1])) {
		m_error = "The supplied port was not a valid number.";
		m_valid = false;
		return;
	}
	unsigned int port;
	std::istringstream ss(argv[1]);
	ss >> port;
	if (port > SHRT_MAX) {
		m_error = "The supplied port was out of range.";
		m_valid = false;
		return;
	}

	m_port = static_cast<unsigned short>(port);
	m_password = argv[2];
}

unsigned short InitData::get_port() const {
	return m_port;
}

const std::string &InitData::get_password() const {
	return m_password;
}

bool InitData::is_valid() const {
	return m_valid;
}

const std::string& InitData::get_error() const {
	return m_error;
}
