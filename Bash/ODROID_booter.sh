#!/bin/bash

while true; do #Continue indefinitely
	
	if [ $(ps aux | grep '/odroid_detection' | grep -v '<defunct>' -c) -le 2 ]; then #if less than 3 odroid_servers are active (booter opens 3 processes)
		xterm -iconic -hold -e "cd ~/Documents/SUNRISE-Odroid/_odroid_detection/_odroid_detection/bin/Debug/ && su -c ./_odroid_detection"
	fi
	
	sleep 60 #check every minute
	
done