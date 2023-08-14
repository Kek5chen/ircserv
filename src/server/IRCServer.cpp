#include <stdexcept>
#include <unistd.h>
#include <sys/socket.h>
#include <climits>
#include <cerrno>
#include <iostream>
#include <algorithm>
#include <map>
#include <sstream>
#include <netinet/in.h>
#include <fcntl.h>
#include "server/IRCServer.hpp"
#include "server/IRCClient.hpp"

bool IRCServer::mCmdHandlersInit = false;
std::map<std::string, void (IRCServer::*)(IRCClient *, const IRCCommand &)> IRCServer::mCmdHandlers;
const IRCCommand IRCServer::mCmdBase = IRCCommand().setHostname("server");

void IRCServer::initCmdHandlers() {
	mCmdHandlers["NICK"] = &IRCServer::handleNICK;
	mCmdHandlers["PASS"] = &IRCServer::handlePASS;
	mCmdHandlers["USER"] = &IRCServer::handleUSER;
	mCmdHandlers["PING"] = &IRCServer::handlePING;
	mCmdHandlers["JOIN"] = &IRCServer::handleJOIN;
	mCmdHandlers["PART"] = &IRCServer::handlePART;
	mCmdHandlers["PRIVMSG"] = &IRCServer::handlePRIVMSG;
	mCmdHandlers["CAP"] = &IRCServer::handleCAP;
	mCmdHandlers["AP"] = &IRCServer::handleCAP;
	mCmdHandlers["KICK"] = &IRCServer::handleKICK;
	//mCmdHandlers["INVITE"] = &IRCServer::handle_INVITE;
	//mCmdHandlers["TOPIC"] = &IRCServer::handle_TOPIC;
	mCmdHandlers["MODE"] = &IRCServer::handleMODE;
	//mCmdHandlers["WHO"] = &IRCServer::handle_WHO;
	mCmdHandlersInit = true;
}

IRCServer::IRCServer(unsigned short port, const std::string &password)
	: mPort(port), mPassword(password), mSocketFd(0), mIsBound(false),
	  mIsListening(false), mShouldStop(false) {
	if (!mCmdHandlersInit)
		initCmdHandlers();

	char hostname[_POSIX_HOST_NAME_MAX];
	if (gethostname(hostname, _POSIX_HOST_NAME_MAX) < 0)
		throw std::runtime_error("Could not get hostname");
	mHost = hostname;
}

IRCServer::~IRCServer() {
	this->stop();
}

void IRCServer::bind() {
	mSocketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mSocketFd < 0)
		throw std::runtime_error("Socket creation failed");

	if (fcntl(mSocketFd, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("fcntl set failed");

	static const int state = 1;
	if (setsockopt(mSocketFd, SOL_SOCKET, SO_REUSEADDR, &state, sizeof(state)) < 0)
		throw std::runtime_error("Could not set socket options");

	sockaddr_in socketAddr = {};
	socketAddr.sin_family = AF_INET;
	socketAddr.sin_port = htons(mPort);
	socketAddr.sin_addr.s_addr = INADDR_ANY;
	int bindResult = ::bind(mSocketFd, reinterpret_cast<struct sockaddr *>(&socketAddr), sizeof(socketAddr));
	if (bindResult < 0) {
		close(mSocketFd);
		throw std::runtime_error("Socket binding failed. Is there another instance of the server running?");
	}
	mIsBound = true;
}

void IRCServer::listen() {
	if (!mIsBound)
		this->bind();
	if (::listen(mSocketFd, INT_MAX) < 0)
		throw std::runtime_error("Could not make socket listen");
	mIsListening = true;
}

int IRCServer::loop() {
	if (!mIsListening)
		this->listen();
	while (!mShouldStop) {
		this->acceptNewClients();
		this->pollClients();
	}
	return 0;
}

void IRCServer::stop() {
	for (size_t i = 0; i < mClients.size(); i++)
		delete mClients[i];
	mClients.clear();
	if (!mShouldStop && (!mIsBound || !mIsListening))
		return;
	mShouldStop = true;
	close(mSocketFd);
	mIsListening = false;
	mIsBound = false;
	mSocketFd = 0;
}

void IRCServer::acceptNewClients() {
	sockaddr_in clientAddr;
	int clientSocket;
	socklen_t clientAddrLen;

	while (true) {
		clientAddrLen = sizeof(clientAddr);
		clientSocket = accept(mSocketFd, reinterpret_cast<struct sockaddr *>(&clientAddr), &clientAddrLen);
		if (clientSocket == -1 && errno == EWOULDBLOCK)
			return;
		std::cout << "Received client connection" << std::endl;
		IRCClient *client = new IRCClient(clientSocket);
		if (!client->isValid()) {
			delete client;
			throw std::runtime_error(
				std::string("Unable to accept connection."));
		}
		mClients.push_back(client);
	}
}

bool IRCServer::receiveData(IRCClient *client, std::string *buffer) {
	char preBuf[MSG_BUFFER_SIZE + 1] = { 0 };
	ssize_t received;

	received = recv(client->getSocketFd(), preBuf, MSG_BUFFER_SIZE + 1, 0);
	if (received == -1)
		throw std::runtime_error("An error occurred while trying to receive the sockets message.");
	if (!received)
		return false;
	buffer->append(preBuf);
	return true;
}

bool IRCServer::handle(IRCClient *client) {
	std::string buf;
	short revents = client->poll();
	if (revents & POLLHUP || revents & POLLERR || revents & POLLNVAL)
		return false;
	if (!(revents & POLLIN))
		return true;
	if (!receiveData(client, &buf))
		return false;

	while (!buf.empty()) {
		size_t end = buf.find("\r\n");
		if (end == std::string::npos)
			break;
		IRCCommand cmd(buf.substr(0, end + 2));
		buf = buf.substr(end + 2);
		if (!cmd.isValid())
			continue;

		if (cmd.mCommand.mName == "QUIT")
			return false;
		handler_map_type::iterator cmdIt = mCmdHandlers.find(cmd.mCommand.mName);
		if (cmdIt == mCmdHandlers.end()) {
			std::cout << "[IN] === NOT IMPLEMENTED ===" << std::endl;
			std::cout << "[IN] " << cmd.mCommand.mName << std::endl;
			std::cout << "[IN] ===      ====       ===" << std::endl;
			continue;
		}
		if (cmd.mCommand.mName != "PASS" && !client->hasAccess(mPassword))
			return false;
		(this->*(cmdIt->second))(client, cmd);
	}
	return true;
}

void IRCServer::pollClients() {
	for (size_t i = 0; i < mClients.size(); i++) {
		bool keepConnection = this->handle(mClients[i]);
		if (keepConnection)
			continue;
		std::cout << "[INFO] Client disconnected" << std::endl;
		mChannelManager.partFromAll(mClients[i]);
		mClients[i]->flushResponse();
		delete mClients[i];
		mClients.erase(std::remove(mClients.begin(), mClients.end(), mClients[i]), mClients.end());
		i--;
	}
}

void IRCServer::handlePASS(IRCClient *client, const IRCCommand &cmd) {
	client->mSuppliedPassword = cmd.mParams[0];
	if (!client->hasAccess(mPassword))
		IRCServer::getResponseBase().setCommand(464)
								 .setEnd("Incorrect Password")
								 .sendTo(client);
}

void IRCServer::sendMotd(IRCClient *client) {
	IRCServer::getResponseBase().setCommand(375)
		.setEnd("- ft_irc (mdoll, kschmidt) Message of the day - ")
		.sendTo(client);
	IRCServer::getResponseBase().setCommand(372)
		.setEnd("- Welcome to ft_irc! Your host is " + mHost + ", running version "
															   IRC_VERSION " built on " __DATE__ " at " __TIME__)
		.sendTo(client);

	std::string userAmount = ((std::ostringstream &) (std::ostringstream() << mClients.size())).str();
	IRCServer::getResponseBase().setCommand(251)
		.setEnd("- There are " + userAmount + " user(s) online")
		.sendTo(client);

	IRCServer::getResponseBase().setCommand(376)
		.setEnd("- End of /MOTD command")
		.sendTo(client);
}

void IRCServer::handleNICK(IRCClient *client, const IRCCommand &cmd) {
	client->setNickname(cmd.mParams[0]);
	if (client->mIsRegistered)
		return;
	sendMotd(client);
	client->mIsRegistered = true;
}

void IRCServer::handleUSER(IRCClient *client, const IRCCommand &cmd) {
	client->setUsername(cmd.mParams[0]);
	if (cmd.mParams.size() > 1)
		client->mMode = cmd.mParams[1];
	client->mRealName = cmd.mEnd;
}

void IRCServer::handlePING(IRCClient *client, const IRCCommand &cmd) {
	const std::string &add = cmd.mParams.empty() ? "" : cmd.mParams[0];
	IRCServer::getResponseBase().setCommand("PONG")
		.setEnd(add)
		.sendTo(client);
}

void IRCServer::handleJOIN(IRCClient *client, const IRCCommand &cmd) {
	std::string channel = cmd.mParams[0];
	if (channel.empty()) {
		return;
	}

	channel = channel.substr(channel[0] == '#');
	if (channel.empty())
		return;
	mChannelManager.join(channel, client);
}

void IRCServer::handlePRIVMSG(IRCClient *client, const IRCCommand &cmd) {
	const std::string &channel = cmd.mParams[0];
	const std::string &message = cmd.mEnd;
	mChannelManager.send(client, channel.substr(1),
						 client->getResponseBase().setCommand("PRIVMSG")
							 .addParam(channel)
							 .setEnd(message));
}

void IRCServer::handlePART(IRCClient *client, const IRCCommand &cmd) {
	const std::string &channel = cmd.mParams[0];
	mChannelManager.part(channel, client);
}

void IRCServer::handleCAP(IRCClient *client, const IRCCommand &cmd) {
	(void) client;
	(void) cmd;
	std::cout << "[INFO] Ignore Capability Negotiation" << std::endl;
}

void IRCServer::handleKICK(IRCClient *client, const IRCCommand &cmd) {
	if (cmd.mParams.size() < 2)
		return;
	const std::string &channel = cmd.mParams[0];
	const std::string &targetClientNick = cmd.mParams[1];
	const std::string &kickMessage = cmd.mEnd;
	mChannelManager.kick(client, channel, targetClientNick, kickMessage);
	// TODO: WeeChat outputs "sender has kicked" and that's it. needs to be checked.
}

IRCCommand IRCServer::getResponseBase() {
	return mCmdBase;
}

// TODO params: /mode <channel> <+/-> <flag> <<flag_param>>
// TODO	// i: Set/remove Invite-only channel 										(+i invite only / -i no invite needed)								TODO: also need to change JOIN
// TODO	// t: Set/remove the restrictions of the TOPIC command to channel operators (+t only ops change TOPIC / -t anyone can change TOPIC)
// TODO	// k: Set/remove the channel key (password) 								(+k <password> sets a password to chanel / -k no password) 			TODO: also need to change JOIN
// TODO	// o: Give/take channel operator privilege 									(+o <nickname> sets op / -o <nickname> removes op)
// TODO	// l: Set/remove the user limit to channel 									(+l <user_limit> maximum of user_limit users / -l removes limit) 	TODO: also need to change JOIN
// TODO	// there can be multiple flags in one command

// TODO	// possible errors:
// TODO	// invalid channel
// TODO	// missing +/-
// TODO	// invalid flag
// TODO	// missing flag params
// TODO	// no rights?
// TODO	// flag already set

void IRCServer::handleMODE(IRCClient *client, const IRCCommand &cmd) {
	if (cmd.mParams.size() < 2) {
		std::cout << "[INFO] MODE: missing params" << std::endl;
		return;
	}
	(void) client;
	const std::string &channel = cmd.mParams[0];
	const std::string &flag = cmd.mParams[1];
	const int &param_count = cmd.mParams.size();

	if (flag == "+i" || flag == "-i")
		mChannelManager.setInviteOnly(channel, flag);
	else if (flag == "+t" || flag == "-t")
		mChannelManager.setTopicRestriction(channel, flag);
	else if (flag == "+k") {
		if (param_count < 3) {
			std::cout << "[INFO] MODE: missing password for MODE " << flag << std::endl;
			return;
		}
		mChannelManager.setPassword(channel, cmd.mParams[2]);
	}
	else if (flag == "-k")
		mChannelManager.setPassword(channel, "");
	else if (flag == "+o" || flag == "-o") {
		if (param_count < 3) {
			std::cout << "[INFO] MODE: missing nickname for MODE " << flag << std::endl;
			return;
		}
		if (flag == "+o")
			mChannelManager.addOperator(channel, cmd.mParams[2]);
		else
			mChannelManager.removeOperator(channel, cmd.mParams[2]);
	}
	else if (flag == "+l") {
		if (param_count < 3) {
			std::cout << "[INFO] MODE: missing user limit for MODE " << flag << std::endl;
			return;
		}
		mChannelManager.setUserLimit(channel, std::stoi(cmd.mParams[2])); // TODO: cpp98 compatibility
	}
	else if (flag == "-l")
		mChannelManager.setUserLimit(channel, -1);
	else
		std::cout << "[INFO] MODE: invalid flag" << std::endl;
	mChannelManager.printChannelMode(channel);
}
