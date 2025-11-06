#!/bin/bash


alert() {
    echo "ALERT:$AppName|$@"
}

notify() {
    echo "DETAILS:SHOW"
    echo "$@"
}


# We're being run
# Name from parent process
# AppName="OpentrackWineBridge2.app"
# AppName=$(osascript -e "path to frontmost application as text") #"de.matatata.OpentrackWineBridge2"

AppName=$(basename "$0");
if [[ "$AppName" == "script" ]]
then
    AppName="$(ps -p $PPID -co comm=)"
fi

# activate the app, otherwise we're trying to control some other frontmost app
osascript   -e "try" \
            -e "tell application \"$AppName\"" \
            -e "activate" \
            -e "end tell" \
            -e "on error errMsg" \
            -e "end try" || exit -1


if [ $# -gt 0 ] && ! OSASCRIPT_REVEICERAPP="$AppName" OTR_WINEDLLPATH="/Library/Application Support/opentrack-wine-bridge/lib/wine" /bin/bash otrwine.sh detect "$@" 1>/dev/null
then
    # echo "That really does not look like a Wine installation"
    # echo "DETAILS:SHOW"
    exit 1
fi

output="$(OSASCRIPT_REVEICERAPP="$AppName" INTERACTIVE_CMDS="install uninstall verify" OTR_WINEDLLPATH="/Library/Application Support/opentrack-wine-bridge/lib/wine" /bin/bash otrwine.sh -i ask  -a "$@" 2>&1)"
ok=$?
[ -n "$output" ] && echo "$output" && echo "DETAILS:SHOW"
if [ $ok -eq 0 ]
then
    notify "OK"
    # echo "QUITAPP"
else
    # echo "DETAILS:SHOW"
    notify "NOK"
fi

