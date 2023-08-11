#pragma once

void graceful_signal_handler(int signal);
void registerSignals(class IRCServer *ctx);
void unregisterSignals();