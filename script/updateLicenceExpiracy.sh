#!/bin/bash

SCRIPT_DIR=$(dirname "$(readlink -f "$0")")
SCRIPT_PATH="$SCRIPT_DIR/../bin/updateDatabase.exe"
CRON_LINE="0 0 * * * $SCRIPT_PATH"

tempfile=$(mktemp)

crontab -l > "$tempfile"

if ! grep -Fxq "$CRON_LINE" "$tempfile"; then
    echo "$CRON_LINE" >> "$tempfile"
    crontab "$tempfile"
fi

rm "$tempfile"
