#!/bin/bash

sleep 600
# switch LEDs off
sudo sh -c 'echo none > /sys/class/leds/led0/trigger'
sudo sh -c 'echo none > /sys/class/leds/led1/trigger'
sudo sh -c 'echo 0 > /sys/class/leds/led0/brightness'
sudo sh -c 'echo 0 > /sys/class/leds/led1/brightness'
# switch HDMI off
sudo /opt/vc/bin/tvservice -o		# -p -> on
# switch USB/LAN IC off
echo '1-1' |sudo tee /sys/bus/usb/drivers/usb/unbind	#OFF
#eco '1-1' |sudo tee /sys/bus/usb/drivers/usb/bind	#ON
