#include "server/IRCCommand.hpp"
#include <sstream>
#include <string>
#include <iterator>

static int ft_stol(const std::string& str) {
    std::istringstream iss(str);
    int result;
    iss >> result;
    return result;
}

IRCCommand::IRCCommand() {

}

IRCCommand::IRCCommand(const std::string& cmd) : mHasPrefix() {
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
        mHasPrefix = true;
    }

    // Command
    mCommand.mName = str;
    mCommand.mCode = ft_stol(mCommand.mName);

    // Parameters
    std::getline(iss, str, ':');
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, ' '))
    {
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

void IRCCommand::sendTo(IRCClient &client) const {
    client.sendResponse(*this);
}

std::string IRCCommand::makePrefix() const {
    std::string prefix;
    if (mHasPrefix) {
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
    if (mCommand.mCode)
        result += mCommand.mCode;
    else
        result += mCommand.mName;
    std::stringstream ss;
    std::copy(mParams.begin(), mParams.end(), std::ostream_iterator<std::string>(ss, " "));
    std::string params = ss.str();
    result += params;
    if (!result.empty())
        result.erase(result.end() - 1);
    if (!mEnd.empty()) {
        result += " :" + mEnd;
    }
    return result;
}

