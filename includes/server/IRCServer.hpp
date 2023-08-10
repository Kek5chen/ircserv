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
typedef std::map<std::string, void(IRCServer::*)(IRCClient*, const IRCCommand&)> handler_map_type;

class IRCServer {
public:
	explicit IRCServer(unsigned short port, const std::string& password = "");
	~IRCServer();

	void bind();
	void listen();
	int loop();
	void stop();
private:
	void acceptNewClients();
	void pollClients();
	bool receiveData(IRCClient* client, std::string* buffer);
	bool handle(IRCClient* client);

	void sendMotd(IRCClient* client);

	void handlePASS(IRCClient* client, const IRCCommand& cmd);
	void handleNICK(IRCClient* client, const IRCCommand& cmd);
	void handleUSER(IRCClient* client, const IRCCommand& cmd);
	void handlePING(IRCClient* client, const IRCCommand& cmd);
	void handleJOIN(IRCClient* client, const IRCCommand& cmd);
	void handlePART(IRCClient* client, const IRCCommand& cmd);
	void handlePRIVMSG(IRCClient* client, const IRCCommand& cmd);
	void handleCAP(IRCClient* client, const IRCCommand& cmd);
	void handleKICK(IRCClient* client, const IRCCommand& cmd);

	static void initCmdHandlers();

	const unsigned short mPort;
	const std::string& mPassword;
	int mSocketFd;
	bool mIsBound;
	bool mIsListening;
	bool mShouldStop;

	std::vector<IRCClient*> mClients;
	IRCChannelManager mChannelManager;

	static handler_map_type mCmdHandlers;
	static bool mCmdHandlersInit;
};