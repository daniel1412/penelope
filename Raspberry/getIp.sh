#!/bin/bash

IPDATEI="/tmp/ipold.txt"
UPDATE=false
#IPNOW=$(/usr/bin/curl https://klenzel.net/remoteip.php)
#IPNOW=$(/usr/bin/dig @resolver1.opendns.com ANY myip.opendns.com +short)
IPNOW=$(wget -qqO- 'https://duckduckgo.com/?q=what+is+my+ip' | grep -Pow 'Your IP address is \K[0-9.]+')
echo $IPNOW
