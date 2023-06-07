#include "setup/SignalHandlers.hpp"
#include "server/IRCServer.hpp"
#include <csignal>
#include <cstdlib>

static IRCServer* g_ctx;

void graceful_signal_handler(int signal) {
	(void) signal;
	g_ctx->stop();
	exit(0);
}

void register_signals(IRCServer* ctx) {
	g_ctx = ctx;
	std::signal(SIGABRT, graceful_signal_handler);
	std::signal(SIGTERM, graceful_signal_handler);
	std::signal(SIGHUP, graceful_signal_handler);
}

void unregister_signals() {
	g_ctx = 0;
	std::signal(SIGABRT, SIG_DFL);
	std::signal(SIGTERM, SIG_DFL);
}