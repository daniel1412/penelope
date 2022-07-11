#!/bin/bash
#tmux new \; \
tmux new-session -s dev -n "TEST" -d

tmux split-window -h -p 50
tmux select-pane -t 0
tmux split-window -v -p 50
#tmux select-pane -t 0
#tmux send-keys 'htop' C-m
#tmux select-pane -t 1
#tmux send-keys 'bmon' C-m
tmux select-pane -t 2
tmux send-keys 'python3 serialVictron2.py' C-m
tmux select-pane -t 1
