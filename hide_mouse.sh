#!/bin/bash
#Hide mouse after 5 seconds of inactivity (if not opened already)

if [ $(ps aux | grep 'unclutter' | grep -v '<defunct>' -c) -le 1 ]; then #if more than 3 ssh-agents are active
	unclutter -display :0.0 -idle 5
fi