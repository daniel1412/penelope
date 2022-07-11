#!/bin/bash
# Run script for WLAN connection 
/home/pi/scripts/wlanRelay.sh 1

# Run sqlSync tool after XX Seconds
waitSeconds=20
while [ $waitSeconds -gt 0 ]; do
	sleep 1s
	echo "$waitSeconds"
	waitSeconds=$(($waitSeconds-1))
done

python3 /home/pi/SQLSync2.py

# Run script to disable WLAN connection 
/home/pi/scripts/wlanRelay.sh 0