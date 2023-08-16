#include <cstdlib>
#include "server/IRCServer.hpp"
#include "server/ServerCodeDefines.hpp"


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

static void sendErrorMessage(IRCClient *client, const IRCCommand &cmd, int code, std::string msg) {
	IRCServer::getResponseBase().setCommand(code)
			.addParam(client->getNickname())
			.addParam(cmd.mCommand.mName)
			.setEnd(msg)
			.sendTo(client);
}

void IRCServer::handleMODE(IRCClient *client, const IRCCommand &cmd) {
	// TODO: [IN] MODE #test
	//       [OUT] :server 461 kx MODE :Not enough parameters

	// TODO: [IN] MODE #hi +v kx
	//       [OUT] :127.0.0.1 501 kx MODE :Unknown mode flag
	if (cmd.mParams.size() < 2) {
		sendErrorMessage(client, cmd, ERR_NEEDMOREPARAMS, "Not enough parameters");
		return;
	}
	if (!mChannelManager.isOperator(cmd.mParams[0], client)) {
		sendErrorMessage(client, cmd, ERR_CHANOPRIVSNEEDED, "You're not channel operator");
		return;
	}
	const std::string &channel = cmd.mParams[0];
	const std::string &flag = cmd.mParams[1];
	const int &param_count = cmd.mParams.size();

	if (flag == "+i" || flag == "-i")
		mChannelManager.setInviteOnly(channel, flag);
	else if (flag == "+t" || flag == "-t")
		mChannelManager.setTopicRestriction(channel, flag);
	else if (flag == "+k") {
		if (param_count < 3) {
			sendErrorMessage(client, cmd, ERR_NEEDMOREPARAMS, "Not enough parameters");
			return;
		}
		mChannelManager.setPassword(channel, cmd.mParams[2]);
	}
	else if (flag == "-k")
		mChannelManager.setPassword(channel, "");
	else if (flag == "+o" || flag == "-o") {
		if (param_count < 3) {
			sendErrorMessage(client, cmd, ERR_NEEDMOREPARAMS, "Not enough parameters");
			return;
		}
		if (flag == "+o")
			mChannelManager.addOperator(channel, cmd.mParams[2]);
		else
			mChannelManager.removeOperator(channel, cmd.mParams[2]);
	}
	else if (flag == "+l") {
		char* endptr;
		long value = std::strtol(cmd.mParams[2].c_str(), &endptr, 10);
		if (param_count < 3 || *endptr != '\0') {
			sendErrorMessage(client, cmd, ERR_NEEDMOREPARAMS, "Not enough parameters");
			return;
		}
		mChannelManager.setUserLimit(channel, value);
	}
	else if (flag == "-l")
		mChannelManager.setUserLimit(channel, -1);
	else {
		sendErrorMessage(client, cmd, ERR_UMODEUNKNOWNFLAG, "Unknown mode flag");
	}
	mChannelManager.printChannelMode(channel);
}