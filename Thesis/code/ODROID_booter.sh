#!/bin/bash

while true; do #Continue indefinitely
	
	if [ $(ps aux | grep 'odroid_server' | grep -v '<defunct>' -c) -le 5 ]; then #if less than 4 odroid_servers are active (2 default, 2 if running)
		xterm -iconic -e su -c "xterm -iconic -hold /home/odroid/Documents/SUNRISE-Odroid/_odroid_server/bin/Debug/odroid_server" 
	fi
	
	sleep 60 #check every minute
	
done