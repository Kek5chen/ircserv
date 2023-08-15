#include "server/IRCServer.hpp"
#include <iostream>


// TODO params: /mode <channel> <+/-> <flag> <<flag_param>>
// TODO	// i: Set/remove Invite-only channel 										(+i invite only / -i no invite needed)								TODO: also need to change JOIN
// TODO	// t: Set/remove the restrictions of the TOPIC command to channel operators (+t only ops change TOPIC / -t anyone can change TOPIC)
// TODO	// k: Set/remove the channel key (password) 								(+k <password> sets a password to chanel / -k no password) 			TODO: also need to change JOIN
// TODO	// o: Give/take channel operator privilege 									(+o <nickname> sets op / -o <nickname> removes op)
// TODO	// l: Set/remove the user limit to channel 									(+l <user_limit> maximum of user_limit users / -l removes limit) 	TODO: also need to change JOIN
// TODO	// change JOIN, INVITE, TOPIC

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
			mChannelManager.removeOperator(channel, cmd.mParams[2]); // TODO besse machen
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