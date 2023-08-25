#include "setup/SignalHandlers.hpp"
#include "utils/nullptr.hpp"
#include "server/IRCServer.hpp"
#include "utils/Logger.hpp"
#include <csignal>
#include <cstdlib>
#include <stdexcept>
#include <iostream>

static IRCServer *g_ctx;

void graceful_signal_handler(int signal) {
	(void) signal;
	LOG("Shutting server down gracefully");
	g_ctx->stop();
}

void registerSignals(IRCServer *ctx) {
	g_ctx = ctx;
	if (std::signal(SIGABRT, graceful_signal_handler) == SIG_ERR)
		throw std::runtime_error("Could not register SIGABRT handler");
	if (std::signal(SIGTERM, graceful_signal_handler) == SIG_ERR)
		throw std::runtime_error("Could not register SIGTERM handler");
	if (std::signal(SIGHUP, graceful_signal_handler) == SIG_ERR)
		throw std::runtime_error("Could not register SIGPIPE handler");
	if (std::signal(SIGINT, graceful_signal_handler) == SIG_ERR)
		throw std::runtime_error("Could not register SIGPIPE handler");
}

void unregisterSignals() {
	g_ctx = nilptr;
	std::signal(SIGABRT, SIG_DFL);
	std::signal(SIGTERM, SIG_DFL);
	std::signal(SIGHUP, SIG_DFL);
}
