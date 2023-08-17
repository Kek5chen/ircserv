#pragma once

class IRCServer;

class IIRCServerOwned {
public:
	explicit IIRCServerOwned(IRCServer *owningServer);
	virtual ~IIRCServerOwned();

	IRCServer *getServer() const;
private:
	IRCServer *mServer;
};