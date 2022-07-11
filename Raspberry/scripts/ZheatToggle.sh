#!/bin/bash
# Switch Heating relay for 1 sec on -> toggles actual status -> Heater On - toggle to Off and vice versa

#LOGFILE="/home/pi/LOG/ZheatToggle.txt"
NOW=$(date)
#echo "Switch ZheatToggle startet at $NOW" >> $LOGFILE
echo "Switch ZheatToggle startet at $NOW"

echo "1s Impulse Heating"
gpio -g write 12 0
sleep 1s
gpio -g write 12 1
echo "DONE"
echo "-------------------------------------------------------------------------"
