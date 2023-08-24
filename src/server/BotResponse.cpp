#include "server/IRCServer.hpp"
#include <ctime>
#include <cstdlib>
#include <sstream>

static std::string buildBotMsg(const std::string &botNick, const std::string &message) {
	return ":" + botNick + "!" + botNick + "@localhost PRIVMSG #" + botNick + " :" + message + "\r\n";
}

static std::string timeAndDay() {
	std::time_t t = std::time(NULL);
	std::tm *tm = std::localtime(&t);
	std::string message;
	int hour = tm->tm_hour;
	int min = tm->tm_min;
	int day = tm->tm_mday;
	int month = tm->tm_mon + 1;
	int year = tm->tm_year + 1900;
	std::ostringstream oss;
	oss << hour << ":" << min << " on " << day << "." << month << "." << year << ".";
	message = oss.str();
	return message;
}

static std::string helpMessage(int index) {
	std::string message[] = {"--------------------------- HELP ---------------------------",
							 "/invite <nickname> <channel>",
						  "/join <channel>",
						  "/kick <channel> <nickname> [<message>]",
						  "/mode <channel> {[+|-]i|t|k|o|l} [<parameters (k|o|l)>]",
						  "/nick <nickname>",
						  "/part <channel>",
						  "/pass <password>",
						  "/ping",
						  "/privmsg | /msg <nickname|channel> <message>",
						  "/quit [<message>]",
						  "/topic <channel> [<topic>]",
						  "/who [<channel>]"};
	return message[index];
}

static std::string generateJoke() {
	std::string jokes[] = {
			"Why do programmers prefer using the dark mode? Because the light attracts bugs!",
			"Why do programmers always mix up Christmas and Halloween? Because Oct 31 == Dec 25!",
			"Why did the programmer go broke? Because he used up all his cache!",
			"Why do programmers hate nature? It has too many bugs!",
			"Why do programmers prefer to use the dark mode? Because light attracts too many bugs!",
			"Why did the computer keep freezing? Because it left its Windows open!",
			"Why do programmers prefer using the keyboard? Because the mouse has too many clicks!",
			"Why did the programmer go to therapy? Because he had too many issues!",
			"Why do programmers prefer gardening? Because they have a lot of plant bugs!",
			"Why was the JavaScript developer sad? Because he didn't 'null' his emotions!",
			"Why do programmers always mix up Christmas and Halloween? Because Oct 31 == Dec 25!",
			"Why don't programmers like nature? It has too many bugs!",
			"Why do programmers prefer to work in the dark? Because light attracts bugs!",
			"Why do Java developers wear glasses? Because they don't see sharp!",
			"Why don't programmers like going outside? The sunlight causes too many 'burn' errors!"
	};
	return jokes[std::rand() % 15];
}

bool IRCServer::botResponse(IRCClient *client, const IRCCommand &cmd) {
	const std::string &command = cmd.mCommand.mName;
	std::string message;

	if (command == "TIMEANDDAY") {
		message = timeAndDay();
	}
	else if (command == "HELLO") {
		message = "Howdy partner 🤠";
	}
	else if (command == "HELP") {
		for (int i = 0; i < 11; ++i) {
			message = buildBotMsg(mBotNick, helpMessage(i));
			client->mResponseBuffer = message;
			client->flushResponse();
		}
		message = "------------------------------------------------------------";
	}
	else if (command == "JOKE")
		message = generateJoke();
	else
		return true;
	message = buildBotMsg(mBotNick, message);
	client->mResponseBuffer = message;
	return true;
}
