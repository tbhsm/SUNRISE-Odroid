#!/bin/bash
# If there are more than 4 remote desktops open, eject the oldest

while true; do #Continue indefinitely
	
	if [ $(ps aux | grep 'ssh-agent' | grep -v '<defunct>' -c) -gt 4 ]; then #if more than 3 ssh-agents are active
		kill $(ps aux | grep 'ssh-agent' | grep -v '<defunct>' | sort -n -k 9 | awk '{print $2}' | head -1) #kill oldest ssh-agent process
	fi

	if [ $(ps aux | grep 'Xvnc' -c) -gt 4 ]; then #if more than 3 Xvnc processes are active
		kill $(ps aux | grep 'Xvnc' | sort -n -k 9 | awk '{print $2}' | head -1) #kill oldest Xvnc process
	fi
	
	sleep 60 #check every minute
	
done