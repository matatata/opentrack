#!/bin/bash

# We're being run
# Name from parent process
AppName=$(basename "$0");

if [ "$AppName" == "script" ]
then
    AppName="$(ps -p $PPID -co comm=)"
fi

usage() {
    notifiy "To install/uninstall Opentrack's Wine DLLs into/from a Wine installation (e.g. CrossOver.app, Wine*.app or Wrappers created by Sikarugir/Kegworks), drag & drop it onto this window or use the File->Open (Command-O) menu. You can then choose which action you'd like to perform."
}

alert() {
    echo "ALERT:$AppName|$@"
}

notifiy() {
    echo "DETAILS:SHOW"
    echo "$@"
}

confirm() {
    if [ "$AppName" = "app.sh" ]
    then
        printf '%s\n' "${@}"
    else
        title="Please confirm"
        prompt="$AppName will operate on the following Wine DLLs directories. You can deselect specific list items to exclude paths that may have been incorrectly identified."

        osascript <<OSA  2>&1
tell application "$AppName"
    activate
    set ASlist to the paragraphs of "$(printf '%s\n' "${@}")"
    set selected to choose from list ASlist multiple selections allowed true default items ASlist with prompt "$prompt" with title "$title"
    if selected is false then
        return "false"
    else
        repeat with aPath in selected
            log aPath
        end repeat
    end if
end tell
OSA
    fi
}


getCommand(){
#button returned of (display dialog "Select command" buttons {"install", "uninstall", "list"} default button 3 with title "What would you like to do?")
    osascript <<OSA  2>&1
tell application "$AppName"
    activate
    choose from list {"install", "uninstall", "verify"} multiple selections allowed false with prompt "What would you like to do?" with title "Select operation"
end tell
OSA
}



if [ $# -gt 0 ]
then
    echo "DETAILS:HIDE"
    IFS=$'\n' outputLines=( $(/usr/local/bin/otrwine find "$@") )

    if [ ${#outputLines[@]} -eq 0 ]
    then
        for i in "$@"; do
            notifiy "$i does not look like a Wine installation. Skipping"
        done
        exit -1
    fi


    command=$(getCommand)

    if [ "$command" == "false" ] ; then usage; exit -1 ; fi



    confirmed=$(confirm "${outputLines[@]}") || ( echo "apple-script failed" && exit -2 )

    if [[ "$confirmed" != "false" && -n "$confirmed" ]];
    then
        IFS=$'\n' read -rd '' -a linesArray <<< "$confirmed"

        if /usr/local/bin/otrwine "$command" "${linesArray[@]}"
        then
            alert "The operation was successful."
            #echo "The operation was successful."
        else
            echo "DETAILS:SHOW"
            alert "The operation was not successful. Please inspect the program's output above."
        fi
    else
        notifiy "Aborted. Try again."
    fi







    exit 0
else
    usage
fi
