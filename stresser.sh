#!/bin/bash

IP="$1"
PORT="$2"
NUM="$3"
PASS="$4"
CHANNELS=("channel1" "channel2" "channel3")
MESSAGES=("Hello" "How are you?" "Testing..." "IRC fun!" "Final message")

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

