#pragma once

void graceful_signal_handler(int signal);
void register_signals(class IRCServer* ctx);
void unregister_signals();