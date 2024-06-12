#!/bin/bash

SCRIPT_PATH="./bin/updateDatabase.exe"

CRON_LINE="0 0 * * * $SCRIPT_PATH"

crontab -l > /tmp/crontab.tmp
echo "$CRON_LINE" >> /tmp/crontab.tmp
crontab /tmp/crontab.tmp
rm /tmp/crontab.tmp