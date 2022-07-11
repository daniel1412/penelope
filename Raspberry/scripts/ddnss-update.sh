#!/bin/bash

#LOGFILE="/home/pi/LOG/logddnss.txt"
NOW=$(date)
echo $NOW
#echo $NOW >> $LOGFILE

IPDATEI="/tmp/ipold.txt"
UPDATE=false
#IPNOW=$(/usr/bin/curl https://klenzel.net/remoteip.php)
#IPNOW=$(/usr/bin/dig @resolver1.opendns.com ANY myip.opendns.com +short)
IPNOW=$(wget -qqO- 'https://duckduckgo.com/?q=what+is+my+ip' | grep -Pow 'Your IP address is \K[0-9.]+')

if [ ! -f $IPDATEI ] ; then
    UPDATE=true
    echo "ipold.txt Datei Existiert nicht"
else
    IPOLD=$(cat $IPDATEI)
    echo "Alte IP = " $IPOLD
    echo "Aktuelle IP = " $IPNOW
    if [ "$IPOLD" != "$IPNOW" ] ; then
        UPDATE=true;
        echo "UPDATE der IP wird durchgeführt"
    else
        echo "KEIN Update Erforderlich"
    fi
fi

if $UPDATE ; then
    echo $IPNOW > $IPDATEI
    echo "UPDATE"
   /usr/bin/curl -4 -s "https://ddnss.de/upd.php?key=42ea43b876b7e24df812059777177e43&host=penelope.ddnss.de" > /dev/null
    echo "UPDATE DONE"
fi
echo "---------------------------------------------------------------------------------------"
