#include "server/IRCServer.hpp"
#include "server/CodeDefines.hpp"
#include "utils/Logger.hpp"
#include <ctime>
#include <iostream>

// TODO check that bot name is not used another time


// TODO help as command
// TODO shit jokes
// TODO higher lower game

static std::string buildBotMsg(const std::string &botNick, const std::string &message) {
	return ":" + botNick + "!" + botNick + "@localhost PRIVMSG #" + botNick + " :" + message + "\r\n";
}

bool IRCServer::botResponse(IRCClient *client, const IRCCommand &cmd) {
	const std::string &command = cmd.mCommand.mName;
	std::string message;

	if (command == "TIMEANDDAY") {
		std::time_t t = std::time(nullptr);
		std::tm *tm = std::localtime(&t);
		int hour = tm->tm_hour;
		int min = tm->tm_min;
		int day = tm->tm_mday;
		int month = tm->tm_mon + 1;
		int year = tm->tm_year + 1900;
		message = "It's " + std::to_string(hour) + ":" + std::to_string(min) + " on " + std::to_string(day) + "." + std::to_string(month) + "." + std::to_string(year) + ".";
	}
	else if (command == "HELLO") {
		message = "Howdy partner 🤠";
	}
	else
		return true;
	message = buildBotMsg(mBotNick, message);
	client->mResponseBuffer = message;
	//client->sendErrorMessage("BOTRESPONSE", RPL_INFO, message);
	(void) client;
	return true;
}
// :test!test@localhost PRIVMSG #test :hello
// :<sender_nick> PRIVMSG <channel_name> :<message_content>
