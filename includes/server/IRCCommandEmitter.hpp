#pragma once

class IRCCommand;

class IRCCommandEmitter {
public:
	virtual ~IRCCommandEmitter() {};

	virtual void send(const IRCCommand &command) = 0;
};
