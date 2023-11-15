#!/bin/bash

PLAY_TIME="1m 2s"
SAMPLE_SIZE=256

dd if=/dev/random of=buf count=1 bs=$SAMPLE_SIZE


tmux split-window -v "watch -d -n0.1 -c './pulp2json buf| jq -C'";
tmux split-window -h "watch -d -n0.1 -c 'hexdump buf'"

tmux new-window -P -d -n pseudo-fuzz "watch -d -n0.1 './pseudo_fuzz.pl'"
tmux new-window -P -d -n watchdog "sleep $PLAY_TIME; tmux kill-session"


