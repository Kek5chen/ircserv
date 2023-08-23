#pragma once

#include <string>
#include <vector>
#include <map>
#include "IRCClient.hpp"
#include "IRCChannelManager.hpp"
#include "IRCCommand.hpp"

#define MSG_BUFFER_SIZE 512

#define IRC_VERSION "0.5"

class IRCServer;

typedef std::map<std::string, bool (IRCServer::*)(IRCClient *, const IRCCommand &)> handler_map_type;

class IRCServer {
public:
	explicit IRCServer(unsigned short port, const std::string &password = "", const std::string &ip = "0.0.0.0");
	~IRCServer();

	void bind();
	void listen();
	int loop();
	void stop();

	static IRCCommand getResponseBase();
	const std::string &getHostname();
	const std::vector<const IRCClient *> &getClients() const;
	const std::string &getPassword();
private:
	void acceptNewClients();
	void pollClients();
	bool receiveData(IRCClient *client, std::string *buffer);
	bool handle(IRCClient *client);

	void sendMotd(IRCClient *client);

	bool handleQUIT(IRCClient *client, const IRCCommand &cmd);
	bool handlePASS(IRCClient *client, const IRCCommand &cmd);
	bool handleNICK(IRCClient *client, const IRCCommand &cmd);
	bool handleUSER(IRCClient *client, const IRCCommand &cmd);
	bool handlePING(IRCClient *client, const IRCCommand &cmd);
	bool handleJOIN(IRCClient *client, const IRCCommand &cmd);
	bool handlePART(IRCClient *client, const IRCCommand &cmd);
	bool handlePRIVMSG(IRCClient *client, const IRCCommand &cmd);
	bool handleCAP(IRCClient *client, const IRCCommand &cmd);
	bool handleKICK(IRCClient *client, const IRCCommand &cmd);
	bool handleMODE(IRCClient *client, const IRCCommand &cmd);
	bool handleINVITE(IRCClient *client, const IRCCommand &cmd);
	bool handleWHO(IRCClient *client, const IRCCommand &cmd);
	bool handleTOPIC(IRCClient *client, const IRCCommand &cmd);

	bool botResponse(IRCClient *client, const IRCCommand &cmd);

	static void initCmdHandlers();

	const unsigned short mPort;
	const std::string &mPassword;
	int mSocketFd;
	bool mIsBound;
	bool mIsListening;
	bool mShouldStop;
	std::string mHost;
	IRCCommand mCmdBase;

	std::vector<IRCClient *> mClients;
	IRCChannelManager mChannelManager;

	static handler_map_type mCmdHandlers;
	static bool mCmdHandlersInit;
	static IRCServer *lastInstance;

	const std::string mBotNick;
};