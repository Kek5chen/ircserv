#include "server/IRCCommand.hpp"
#include <sstream>
#include <string>
#include <iterator>
#include <algorithm>
#include <iomanip>

static int ft_stol(const std::string &str) {
	std::istringstream iss(str);
	int result;
	iss >> result;
	return result;
}

IRCCommand::IRCCommand() {

}

IRCCommand::IRCCommand(const std::string &cmd) {
	size_t cmdEnd = cmd.find("\r\n");
	std::istringstream iss(cmdEnd == std::string::npos ? cmd : cmd.substr(0, cmdEnd));
	std::string str;

	// Prefix
	std::getline(iss, str, ' ');
	if (str[0] == ':') {
		str = str.substr(1);
		std::string username_host = str.substr(str.find('!') + 1);
		mPrefix.mHostname = str.substr(0, str.find('!'));
		mPrefix.mUsername = username_host.substr(0, username_host.find('@'));
		mPrefix.mHost = username_host.substr(username_host.find('@') + 1);
		std::getline(iss, str, ' ');
	}

	// Command
	mCommand.mName.resize(str.size());
	std::transform(str.begin(), str.end(), mCommand.mName.begin(), ::toupper);
	mCommand.mCode = ft_stol(mCommand.mName);

	// Parameters
	std::getline(iss, str, ':');
	std::string token;
	std::istringstream tokenStream(str);
	while (std::getline(tokenStream, token, ' ')) {
		mParams.push_back(token);
	}

	std::getline(iss, mEnd);
	if (!mEnd.empty() && mEnd[0] == ':') {
		mEnd = mEnd.substr(1);
	}
}

IRCCommand::~IRCCommand() {
	// Destructor
}

bool IRCCommand::isValid() const {
	return !mCommand.mName.empty() && !mParams.empty();
}

bool IRCCommand::hasPrefix() const {
	return !mPrefix.mHostname.empty() || !mPrefix.mUsername.empty() || !mPrefix.mHost.empty();
}

void IRCCommand::sendTo(IRCCommandEmitter *emitter) const {
	emitter->send(*this);
}

std::string IRCCommand::makePrefix() const {
	std::string prefix;
	if (hasPrefix()) {
		prefix += ":" + mPrefix.mHostname;
		if (!mPrefix.mUsername.empty())
			prefix += "!" + mPrefix.mUsername;
		if (!mPrefix.mHost.empty())
			prefix += "@" + mPrefix.mHost;
	}
	return prefix;
}

IRCCommand::operator std::string() const {
	std::string result;
	result += makePrefix();
	if (result.length() > 0)
		result += " ";

	if (mCommand.mCode) {
		std::stringstream code;
		code << std::setw(3) << std::setfill('0') << mCommand.mCode;
		result += code.str();
	} else
		result += mCommand.mName;

	std::stringstream ss;
	std::copy(mParams.begin(), mParams.end(), std::ostream_iterator<std::string>(ss, " "));
	std::string params = ss.str();
	if (!params.empty()) {
		params.erase(params.end() - 1);
		result += " " + params;
	}

	if (!mEnd.empty()) {
		result += " :" + mEnd;
	}
	return result;
}

IRCCommand &IRCCommand::setHostname(const std::string &hostname) {
	mPrefix.mHostname = hostname;
	return *this;
}

IRCCommand &IRCCommand::setUsername(const std::string &username) {
	mPrefix.mUsername = username;
	return *this;
}

IRCCommand &IRCCommand::setHost(const std::string &host) {
	mPrefix.mHost = host;
	return *this;
}

IRCCommand &IRCCommand::setCommand(const std::string &command) {
	mCommand.mName = command;
	mCommand.mCode = 0;
	return *this;
}

IRCCommand &IRCCommand::setCommand(int command) {
	mCommand.mCode = command;
	mCommand.mName = "";
	return *this;
}

IRCCommand &IRCCommand::addParam(const std::string &param) {
	mParams.push_back(param);
	return *this;
}

IRCCommand &IRCCommand::setEnd(const std::string &end) {
	mEnd = end;
	return *this;
}

std::string IRCCommand::getHostname() const {
	return mPrefix.mHostname;
}

std::string IRCCommand::getUsername() const {
	return mPrefix.mUsername;
}

std::string IRCCommand::getHost() const {
	return mPrefix.mHost;
}

std::string IRCCommand::getCommand() const {
	return mCommand.mName;
}

std::vector<std::string> IRCCommand::getParams() const {
	return mParams;
}

std::string IRCCommand::getEnd() const {
	return mEnd;
}

