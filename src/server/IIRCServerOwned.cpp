#include "server/IIRCServerOwned.hpp"
#include "server/IRCServer.hpp"

IIRCServerOwned::IIRCServerOwned(IRCServer *owningServer) {
	mServer = owningServer;
}

IRCServer *IIRCServerOwned::getServer() const {
	return mServer;
}

IIRCServerOwned::~IIRCServerOwned() {

}
