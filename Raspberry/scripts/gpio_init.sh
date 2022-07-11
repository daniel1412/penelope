#! /bin/bash

# Initial bootup config (all off -> nothing can happend when raspi crashes)
# SolarCharger Control PIN
gpio -g mode 26 out
gpio -g write 26 0
# 12V DC/DC Converter PIN (relay1)
gpio -g mode 5 out
gpio -g write 5 1 # relay gnd is on!
# 24V DC/DC Converter PIN (relay2)
gpio -g mode 6 out
gpio -g write 6 1
# Heater Remote Switch PIN (relay3)
gpio -g mode 12 out
gpio -g write 12 1
# unassigned PIN (relay4)
gpio -g mode 13 out
gpio -g write 13 1
# unassigned PIN (relay5)
gpio -g mode 16 out
gpio -g write 16 1
# unassigned PIN (relay6)
gpio -g mode 19 out
gpio -g write 19 1
# unassigned PIN (relay7)
gpio -g mode 20 out
gpio -g write 20 1
# unassigned PIN (relay8)
gpio -g mode 21 out
gpio -g write 21 1

exit 0
