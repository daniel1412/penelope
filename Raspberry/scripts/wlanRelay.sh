#!/bin/bash
# Switch Relays 1 and 2 on to activate CP210 WLAN and Netgear Router

waitSeconds=120
NOW=$(date)
LOGFILE="/home/pi/LOG/wlanRelay.txt"
echo "Run wlanToggle argument = $1 at: $NOW" >> $LOGFILE
echo "Run at: $NOW"
if [ $1 -eq '1' ]; then
	echo "Switch WLAN ON"
	echo '1-1' |sudo tee /sys/bus/usb/drivers/usb/bind     # USB controller + LAN ON
	gpio -g write 6 0
	while [ $waitSeconds -gt 0 ]; do
		sleep 1s
		#clear
		echo "$waitSeconds"
		waitSeconds=$(($waitSeconds-1))
	done
	echo "Switch network-switch on"
	gpio -g write 5 0
else
	echo "Switch WLAN OFF"
	echo '1-1' |sudo tee /sys/bus/usb/drivers/usb/unbind	# USB controller + LAN OFF
	gpio -g write 5 1
	gpio -g write 6 1
fi
echo "---------------------------------------------------------------"
