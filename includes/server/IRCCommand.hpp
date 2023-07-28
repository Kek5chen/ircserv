#include <string>
#include <vector>
#include "IRCClient.hpp"

class IRCCommand {
public:
    explicit IRCCommand(const std::string& commandStr);
    ~IRCCommand();

    std::string make_prefix() const;
    operator std::string() const;

    struct {
        std::string m_hostname;
        std::string m_username;
        std::string m_host;
    } m_prefix;
    bool m_has_prefix;
    struct {
        std::string m_name;
        int m_code;
    } m_command;
    std::vector<std::string> m_params;
    std::string m_end;

    bool is_valid() const;
    void send_to(IRCClient& client) const;
};
