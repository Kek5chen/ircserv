#include <cstdlib>
#include "server/IRCServer.hpp"
#include "server/CodeDefines.hpp"

bool IRCServer::handleMODE(IRCClient *client, const IRCCommand &cmd) {
	// TODO: [IN] MODE #hi +v kx
	//       [OUT] :127.0.0.1 501 kx MODE :Unknown mode flag
	if (cmd.mParams.size() < 2) {
		if (cmd.mParams.size() == 1) {
			if (!mChannelManager.printChannelMode(client, cmd.mParams[0])) {
				client->sendErrorMessage(cmd.mCommand.mName, ERR_NOSUCHCHANNEL, cmd.mParams[0] + " :No such channel");
				return true;
			}
			else
				return true;
        }
		client->sendErrorMessage(cmd.mCommand.mName, ERR_NEEDMOREPARAMS, "Not enough parameters");
		return true;
	}
	if (!mChannelManager.isOperator(cmd.mParams[0], client)) {
		client->sendErrorMessage(cmd.mCommand.mName, ERR_CHANOPRIVSNEEDED, "You're not channel operator");
		return true;
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
			client->sendErrorMessage(cmd.mCommand.mName, ERR_NEEDMOREPARAMS, "Not enough parameters");
			return true;
		}
		mChannelManager.setPassword(channel, cmd.mParams[2]);
	}
	else if (flag == "-k")
		mChannelManager.setPassword(channel, "");
	else if (flag == "+o" || flag == "-o") {
		if (param_count < 3) {
			client->sendErrorMessage(cmd.mCommand.mName, ERR_NEEDMOREPARAMS, "Not enough parameters");
			return true;
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
			client->sendErrorMessage(cmd.mCommand.mName, ERR_NEEDMOREPARAMS, "Not enough parameters");
			return true;
		}
		mChannelManager.setUserLimit(channel, value);
	}
	else if (flag == "-l")
		mChannelManager.setUserLimit(channel, -1);
	else {
		client->sendErrorMessage(cmd.mCommand.mName, ERR_UMODEUNKNOWNFLAG, "Unknown mode flag");
	}
	return true;
}