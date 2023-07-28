#include "server/IRCCommand.hpp"
#include <sstream>
#include <string>
#include <iterator>

static int stol(const std::string& str) {
    std::istringstream iss(str);
    int result;
    iss >> result;
    return result;
}

IRCCommand::IRCCommand(const std::string& cmd) : m_has_prefix() {
    size_t cmdEnd = cmd.find("\r\n");
    std::istringstream iss(cmdEnd == std::string::npos ? cmd : cmd.substr(0, cmdEnd));
    std::string str;

    // Prefix
    std::getline(iss, str, ' ');
    if (str[0] == ':') {
        str = str.substr(1);
        std::string username_host = str.substr(str.find('!') + 1);
        m_prefix.m_hostname = str.substr(0, str.find('!'));
        m_prefix.m_username = username_host.substr(0, username_host.find('@'));
        m_prefix.m_host = username_host.substr(username_host.find('@') + 1);
        std::getline(iss, str, ' ');
        m_has_prefix = true;
    }

    // Command
    m_command.m_name = str;
    m_command.m_code = stol(m_command.m_name);

    // Parameters
    std::getline(iss, str, ':');
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, ' '))
    {
        m_params.push_back(token);
    }

    std::getline(iss, m_end);
    if (!m_end.empty() && m_end[0] == ':') {
        m_end = m_end.substr(1);
    }
}

IRCCommand::~IRCCommand() {
    // Destructor
}

bool IRCCommand::is_valid() const{
    return !m_command.m_name.empty() && !m_params.empty();
}

void IRCCommand::send_to(IRCClient &client) const {
    client.send_response(*this);
}

std::string IRCCommand::make_prefix() const {
    std::string prefix;
    if (m_has_prefix) {
        prefix += ":" + m_prefix.m_hostname;
        if (!m_prefix.m_username.empty())
            prefix += "!" + m_prefix.m_username;
        if (!m_prefix.m_host.empty())
            prefix += "@" + m_prefix.m_host;
    }
    return prefix;
}

IRCCommand::operator std::string() const {
    std::string result;
    result += make_prefix();
    result += m_command.m_name;
    std::stringstream ss;
    std::copy(m_params.begin(), m_params.end(), std::ostream_iterator<std::string>(ss, " "));
    std::string params = ss.str();
    result += params;
    if (!result.empty())
        result.erase(result.end() - 1);
    if (!m_end.empty()) {
        result += " :" + m_end;
    }
    return result;
}

