#!/bin/bash
SERVICE="app"
FILE="/deploy/bin/app"
FINGERPRINT="/root/watchers/$SERVICE.md5"

mkdir /root/watchers

fingerprint_expected=$(/usr/bin/md5sum "$FILE")

if [ ! -e "$FINGERPRINT" ]
then
  echo "$fingerprint_expected" > "$FINGERPRINT"
fi

fingerprint_content=$(/bin/cat "${FINGERPRINT}")

if [ "$fingerprint_expected" != "$fingerprint_content" ];
then
  # Service has new binary
  echo "$fingerprint_expected" > "$FINGERPRINT"
  echo "Service $SERVICE has changed, trying to restart"
  kill $(ps -A | grep -2 app | awk '{print $1}')
fi

