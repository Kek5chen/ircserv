#!/bin/bash

if [ $# -lt 3 ]; then
    echo "Usage: $0 <ip> <port> <num> [pass]"
    exit 1
fi

IP="$1"
PORT="$2"
NUM="$3"
PASS="$4"
CHANNELS=("channel1" "channel2" "channel3")
MESSAGES=("I am a bot" "Hello" "Hi" "How are you?" "I am fine" "Bye" "Goodbye" "Goodnight" "Good morning" "Good afternoon" "Good evening")

declare -a SOCKETS=()

register_with_irc() {
    local fd=$1
    local user="user$2"

    if [ ! -z "$PASS" ]; then
        echo "PASS $PASS" >&${fd}
    fi

    echo "NICK $user" >&${fd}
    echo "USER $user 8 * :$user" >&${fd}
}

send_random_message() {
    local fd=$1
    local channel=$2
    local msg=${MESSAGES[$RANDOM % ${#MESSAGES[@]}]}
    echo "PRIVMSG #$channel :$msg" >&${fd}
}

for i in $(seq 1 $NUM); do
    exec {fd}>/dev/tcp/$IP/$PORT
    SOCKETS+=($fd)
    register_with_irc $fd $i
done

sleep 5

for fd in "${SOCKETS[@]}"; do
    channel=${CHANNELS[$RANDOM % ${#CHANNELS[@]}]}
    echo "JOIN $channel" >&${fd}
    echo "PART $channel" >&${fd}
done

channel=${CHANNELS[$RANDOM % ${#CHANNELS[@]}]}
for fd in "${SOCKETS[@]}"; do
    echo "JOIN $channel" >&${fd}
done

sleep 10

for fd in "${SOCKETS[@]}"; do
    for j in $(seq 1 5); do
        send_random_message $fd $channel
    done
done

sleep 5

for fd in "${SOCKETS[@]}"; do
    echo "QUIT :Bye" >&${fd}
done

