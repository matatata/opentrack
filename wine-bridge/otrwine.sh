#!/bin/bash

verb=""
appname=$(basename "$0")
cleanmode=false
dryrun=false
print0=false
interactive=false
ask_confirmation=false

interactive_cmds=( install uninstall detect verify list )

if [ -n "$INTERACTIVE_CMDS" ]
then
    interactive_cmds=( $INTERACTIVE_CMDS )
fi

# https://stackoverflow.com/a/246128
# also works when scitp is symlinked
get_script_dir()
{
    local SOURCE_PATH="${BASH_SOURCE[0]}"
    local SYMLINK_DIR
    local SCRIPT_DIR
    # Resolve symlinks recursively
    while [ -L "$SOURCE_PATH" ]; do
        # Get symlink directory
        SYMLINK_DIR="$( cd -P "$( dirname "$SOURCE_PATH" )" >/dev/null 2>&1 && pwd )"
        # Resolve symlink target (relative or absolute)
        SOURCE_PATH="$(readlink "$SOURCE_PATH")"
        # Check if candidate path is relative or absolute
        if [[ $SOURCE_PATH != /* ]]; then
            # Candidate path is relative, resolve to full path
            SOURCE_PATH=$SYMLINK_DIR/$SOURCE_PATH
        fi
    done
    # Get final script directory path from fully resolved source path
    SCRIPT_DIR="$(cd -P "$( dirname "$SOURCE_PATH" )" >/dev/null 2>&1 && pwd)"
    echo "$SCRIPT_DIR"
}

SCRIPT_DIR=$(get_script_dir)
OTR_WINEDLLPATH="${OTR_WINEDLLPATH:-"$SCRIPT_DIR/../lib/wine"}"
PICKLE_CMD="${SCRIPT_DIR}/pickle.app/Contents/MacOS/pickle"

echo_stderr() {
    >&2 echo "$@"
}


check_dir() {
    if [ ! -d "$1" ] ; then
        echo_stderr "Not a directory or does not exist: $1"
        return 1
    fi
    return 0
}

print_usage() {
cat << EOT
Usage: $appname (-h | --help | help)
       $appname [-i] (install | uninstall | list | detect | verify ) [-0dpac | -p <pattern>] [<dst>...]
EOT
}

print_help() {
    print_usage

    cat << EOT
This script adds symbolic links from $OTR_WINEDLLPATH to the corresponding paths at a destination Wine installation.
When your Wine installation is updated you probably need to re-install it again.

It is reversible and is harmless - I took great care to make it as safe as possible, but if you use it then YOU DO IT AT YOUR OWN RISK.
Note that this tool never removes real files, only symbolic links with specific names.

macOS only: If you omit the dst argument(s) you'll be asked to pick a location, which might be necessery with sandboxed apps like CrossOver.app.

<verb> is any of the folllowing:

-h                                  Show usage

help, --help                        Show this long help

install [-0dpa]     [<dst>...]      Install wine-bridge into the Wine installation which is expected to be located at or somewhere below the given
                                    location(s). You might need to prepend 'sudo': sudo $appname install <destination(s)>

uninstall [-0dpac]  [<dst>...]      Uninstall wine-bridge into the Wine installation which is expected to be located at or somewhere below the given path.
                                    This reverts what 'install' did - meaning it only looks where install did look. See option -c for another option.
                                    You might need to prepend 'sudo': sudo $appname install <destination(s)>

list [-0]           [<dst>...]      Show any occurences of files that belong to wine-bridge for the given path.
                                    This is helpful for troubleshooting and to find out where those files exacty are.

detect [-0p]        [<dst>...]      Finds and lists wine dlls directory candidates at the given location(s). These are the directies the install
                                    and uninstall verbs would operated on.

verify [-0p]        [<dst>...]      Verify if links are present as expected.
Options:
-i                                  The run runs interactively asking you for input at the gui. You may even omit the verb.

-a                                  Make the verbs install and uninstall to ask for confirmation (implies -i)

-d                                  With this option install and uninstall do not actually perform modifications.

-p  <pattern>                       By default $appname looks for paths matching ${patterns[@]}, but you can define one custom pattern for special needs
                                    (including wildcards). If for instance your Wine installation structure is unexpected and x86-64-unix is located at

                                        /Applications/FooBar.app/Contents/strange/pathXYZ/to/x86_64-unix e.g.

                                    then you could dry-run (-d) and refine the following command until you're happy. When using * put the whole pattern
                                    in singkle or double quotes.

                                        $appname install -d -p "*/strange/path*/to" /Applications/FooBar.app

-c                                  To really remove every occurence of wine-bridge symbolic links regardless of where exactly below the given <destination(s)>
                                    they are located use the -c option. Use this to really clean-up if you think you've made a mess perhaps using the verb install
                                    in conjunction with bad -p option.

-0                                  Paths will be printed with Null-Termination, which is useful to pipe the output into xargs -0 for further processing.

Note:
It is not really important what exact location you choose, because the script will search for specific subpaths such as "lib/wine/x86_64-unix"
so it's enough that you specify or pick a folder that is "close" enough. That also means that you should not specify very unspecific paths such as /Applications (macOS)
/usr/ or /opt/local - it will take the script a long time to terminate. For macports /opt/local/lib/wine would be the right choice
as of October 2025.

If there's no output then nothing happened or there was nothing to show you. Check the destination. Also your Wine installation may have an unexpected directory
structure and I just did not recognize it. Let me know what you've got.

IMPORTANT -$appname's action verbs only output something if it found a suitable Wine installations and actually did something or
has something to show/tell you. So no output usually means the destination path is not a Wine installation or in case of the "list" verb there are no wine-bridge
files found.

Generally a GOOD output for activating a macOS app-bundle should usually list 8 symbolic links. In Linux things may differ depending on the architectures you installed.

Anyway it'll resemble this:

1 [...]/wine/i386-windows/freetrackclient.dll -> $OTR_WINEDLLPATH/lib/wine/i386-windows/freetrackclient.dll
2 [...]/wine/i386-unix/freetrackclient.so -> $OTR_WINEDLLPATH/lib/wine/x86_64-unix/freetrackclient.so
3 [...]/wine/x86_64-windows/freetrackclient64.dll -> $OTR_WINEDLLPATH/lib/wine/x86_64-windows/freetrackclient64.dll
4 [...]/wine/x86_64-unix/freetrackclient64.so -> $OTR_WINEDLLPATH/lib/wine/x86_64-unix/freetrackclient64.so
.
.
.
8
.
.
.

Note that i386-unix may not be present in Linux depending on whether i386 wine is installed while on macOS the non 64-bit *.so files are installed into x86_64-unix as well.
On Linux files are layed out differently and you might get redundat matches. For example when trying to install /usr/lib in debian I got redundant matches in
/usr/lib/x86_64-linux-gnu/wine/ and /usr/lib/i386-linux-gnu/wine/ which is fine.

In any case you don't want to blindly alter every directory containing folder like x86_64-unix. In CrossOver you don't want to alter lib64/apple_gptk lib/dxmt/x86_64-unix e.g.
If you did by accident nothing bad should happen, but you can alway revert things by using the "uninstall" verb with the same options you used when mistakenly
altering wrong paths.

Examples:

(1) Install Opentrack's Wine DLLs into app bundles like CrossOver.app or homebrew's "Wine Stable.app" or "Wine Develop.app"

    First perform a dry-run and tell the script to prompt us to pick a app-bundle via UI (-a):

    $appname install -a -d

    Navigate to the Applications-folder "/Applications" and pick the app bundle. Verify the output and do it again
    this time without the -d option to actually perform the actions.

    $appname install -a

    You may ask "why not pass the location as an argument?" - Yes you can do that, but because of macOS security protection mechanisms
    commercial apps like CrossOver usually are a sandboxed and a script cannot alter the app without explicit user consent via a GUI-interaction.
    When picking a destination path via GUI the protection for the selected path should be more relaxed (keyword "accessingSecurityScopedResource"
    in macOS's URL-API). Luckily that seems to work with scripts like $appname as well. If it doesn't Apple has fooled me or changed something,
    so please let me know.

(2) Steam's Proton on Linux
    TODO

(3) Install Opentrack's Wine DLLs of any other Wine installation not covered here

    First find out the destination path <path-to-wine-installation>. For macOS macports' wine /opt/local/lib/wine is the right choice.
    Look inside it and search for folders wine/x86_64-windows wine/x86_64-unix and maybe correspondent folders for i386.

    Experiment with:

        $appname detect <path-to-wine-installation>

    and/or dry-run (-d)

        $appname install -d <path-to-wine-installation>

    until you get reasonable output (see explanation above).
    It you don't get reasonable output try custom patterns via the -p option. Start with -p "*" and then refine it until the number of matches do not exceed 8
    and you only touch the "right" directories. You'll have to get an intuition for it. The pattern "*/wine" should really do it in most cases.

    If unsure install the destinations individually (if present):

        $appname install <prefix>/wine/x86_64-unix
        $appname install <prefix>/wine/x86_64-windows
        $appname install <prefix>/wine/i368-unix
        $appname install <prefix>/wine/i368-windows

    If you get permission errors you might want to prepend "sudo " to the above verbs once you're confident you found the right options.
EOT
}


pick_destination() {
    echo "omitting/destination/only/supported/in/macOS"
    return 1
}

ask_command() {
    echo "not implemented"
    return 1
}

confirm() {
    return 1
}

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # linux
    patterns=("lib/wine" "lib/*-linux-gnu/wine")
elif [[ "$OSTYPE" == "darwin"* ]]; then
    # Mac OSX

    # In macOS scripts we always assume this being at /
    patterns=("lib/wine")



    map_verb_text() {
        case "$1" in
            install) echo "Install OTR Wine DLLs" ;;
            uninstall) echo "Uninstall OTR Wine DLLs" ;;
            verify) echo "Verify OTR Wine DLLs" ;;
            detect) echo "Detect Wine DLL directories" ;;
            list) echo "Find OTR Wine DLLs" ;;
            *) echo "?" ;;
        esac
    }

    map_text_verb() {
        case "$1" in
            Install*) echo "install" ;;
            Uninstall*) echo "uninstall" ;;
            Verify*) echo "verify" ;;
            Detect*) echo "detect" ;;
            Find*) echo "list" ;;
            *) echo_stderr "unknown verb text $1"
                echo "?"
            ;;
        esac
    }

    mk_osa_list() {
        echo -n "{"
        while [ $# -gt 0 ]
        do
            local text=$(map_verb_text $1)
            echo -n "\"${text:unknown}\""
            shift 1
            if [ $# -gt 0 ] ; then echo -n ", "; fi
        done
        echo -n "}"
    }

    ask_command() {
        local title="Select operation"
        local prompt="What would you like to do?"
        local cmd=$(osascript <<OSA 2>&1
    activate
    choose from list $(mk_osa_list "$@") multiple selections allowed false with prompt "$prompt" with title "$title"
OSA
        )  || ( echo_stderr "Error in apple-script" && echo "" && return 1 )

        if [ "$cmd" == "false" ]
        then
            echo ""
        else
            echo $(map_text_verb "$cmd")
            return 0
        fi
    }


    pick_destination() {
        if [ ! -x "$PICKLE_CMD" ]
        then
            echo_stderr "$PICKLE_CMD not found or not executable. You must specify destination paths and the command line"
            exit 1
        fi
        prompt=$(map_verb_text "$verb")
        path=$("$PICKLE_CMD" -t -m "Select a Wine installation e.g. CrossOver.app, Wine*.app or similar" -p "$prompt" -df) || return 1
        echo "$path"
        return 0
    }

    confirm() {
        local prompt="Confirm & Proceed"
        local message="The program has identified the following Wine DLLs directories. You can deselect specific list items to exclude paths, if you have reasons to believe they have been incorrectly identified."
        "$PICKLE_CMD" -p "$prompt" -m "$message" "${@}"
    }

else
    # Unknown.
    echo_stderr "Unsupported os $OSTYPE"
    exit 1
fi

if [ "$1" == "-i" ]
then
    interactive=true
    shift 1
fi

verb="$1"
shift 1

if ( $interactive && [ -z "$verb" ] ) || [ "$verb" == "ask" ]
then
    verb=$(ask_command "${interactive_cmds[@]}" || exit 1)
    [ -z "$verb" ] && echo_stderr "Cancelled" &&  exit 1
fi


while getopts "cda0pa:" arg; do
  case "$arg" in
    a)
      ask_confirmation=true
      interactive=true
      ;;
    d)
      dryrun=true
      ;;
    p)
      patterns=("$OPTARG")
      ;;
    c)
      cleanmode=true
      ;;
    0)
      print0=true
      ;;
    *)
      print_usage
      exit 1
    ;;
  esac
done
shift $((OPTIND-1))


if ! check_dir "$OTR_WINEDLLPATH"
then
    exit 1
fi

paths=( "${@}" )

case "$verb" in

    install | uninstall | list | detect | verify )
        if [ "${#paths[@]}" -eq 0 ]
        then
            d="$(pick_destination)" || exit 1
            paths+=( "$d" )
        fi
    ;;
    help | --help)
        print_help
        exit 0
    ;;
    -h)
        print_usage
        exit 0
    ;;
esac


# first argument is the function to call for matching target dirs x86_64-unix, x86_64-windows ...
# the rest of the args is the target diretories to search in
find_targets_print0(){
    local d=$1

    for s in "${patterns[@]}"
    do
        find "$d" -type d  \(   -path "*/$s/x86_64-unix" -or \
                                -path "*/$s/x86_64-windows" -or \
                                -path "*/$s/i386-unix" -or \
                                -path "*/$s/i386-windows" \
                        \) -print0
    done

}

print_libfile(){
    $print0 && printf '%s\0' "$1" || echo "$1"
}

# first argument is the function to call for matching target dirs x86_64-unix, x86_64-windows ...
# the rest of the args is the target diretories to search in
find_targets_and_do(){

    find_targets_print0 "$2"  | while IFS= read -r -d '' dir #"${@:2}"
            do
                $1 "$dir" || exit 1
            done || exit 1
}

do_link() {
    if $dryrun
    then
        print_libfile "$2"
    else
        ln -sf "$(realpath "$1")" "$2" && print_libfile "$2"
    fi
}

do_install() {
    dir="$1"
    libdir=$(basename "$dir")
    if [ -d "$OTR_WINEDLLPATH/$libdir" ]
    then
        for file in "$OTR_WINEDLLPATH/$libdir/"*;
        do
            fname=$(basename "$file")
            do_link "$file" "$dir/$fname" || exit 1
        done
    else
        echo_stderr "$OTR_WINEDLLPATH/$libdir does not exist"
    fi
}

do_verify() {
    dir="$1"
    libdir=$(basename "$dir")
    if [ -d "$OTR_WINEDLLPATH/$libdir" ]
    then
        for file in "$OTR_WINEDLLPATH/$libdir/"*;
        do
            file="$(realpath "$file")"
            fname=$(basename "$file")
            if [ -L "$dir/$fname" ]
            then
                real=$(readlink -f "$dir/$fname")
                if [ "$file" == "$real" ]
                then
                    print_libfile "$dir/$fname"
                else
                    echo_stderr "$dir/$fname links to unexpected $real, expecting $file"
                    return 1
                fi
            elif [ -f "$dir/$fname" ]
            then
                echo_stderr "$dir/$fname is a file, but should be a link"
                return 1
            else
                echo_stderr "$dir/$fname missing"
                return 1
            fi
        done
    fi
}

find_existing_links_print0() {
    if ! check_dir "$1"; then return 1; fi
    find "$1"  \(  -name "freetrackclient.dll" -o -name "npclient.dll" \
                    -o -name "freetrackclient64.dll" -o -name "npclient64.dll" \
                    -o -name "freetrackclient.so" -o -name "npclient.so" \
                    -o -name "freetrackclient64.so" -o -name "npclient64.so" \
                    -o -name "otrclient.dll" -o -name "otrclient64.dll" \
                    -o -name "otrclient.so" -o -name "otrclient64.so" \
                \)  -type l -print0
}

do_remove() {
    if $dryrun ; then
        test -L "$1" && print_libfile "$1"
    else
        rm "$1" && print_libfile "$1"
    fi
}

do_uninstall() {
    dir="$1"
    find_existing_links_print0 "$dir" | while IFS= read -r -d '' f; do
                            do_remove "$f"
                        done
}

do_list() {
    dir="$1"
    if ! check_dir "$dir"; then return 1; fi

    find "$dir"  \( -name "freetrackclient.dll" -o -name "npclient.dll" \
                    -o -name "freetrackclient64.dll" -o -name "npclient64.dll" \
                    -o -name "freetrackclient.so" -o -name "npclient.so" \
                    -o -name "freetrackclient64.so" -o -name "npclient64.so" \
                    -o -name "otrclient.dll" -o -name "otrclient64.dll" \
                    -o -name "otrclient.so" -o -name "otrclient64.so" \
                 \) -print0  | while IFS= read -r -d '' f; do
                    test -L "$f" || echo_stderr "not a symbolik link: $f"
                    print_libfile "$f"
                done
}

do_detect() {
    for d in "${@}"
    do
        if check_dir "$d" ; then find_targets_and_do "ls -d" "$d" || return 1
        fi
    done
}


detect_and_ask_for_confirmation(){
    IFS=$'\n' candidates=( $(unset IFS ; do_detect "${@}" ) )

    if [ "${#candidates[@]}" -eq 0 ]
    then
        echo_stderr "Nothing found that looks like a Wine installation"
        return 1
    fi

    confirmed=$(confirm "${candidates[@]}") # || return 1
    [ -z "$confirmed" ] && return 1
    echo "$confirmed"
    return 0
}

case "$verb" in

ask)
    ask_command "${interactive_cmds[@]}"
;;

install)
    if $interactive && $ask_confirmation
    then
       confirmed="$(detect_and_ask_for_confirmation "${paths[@]}")" ||  exit 1
       IFS=$'\n' read -rd '' -a paths <<< "$confirmed"
    fi
    for d in "${paths[@]}"; do
        if check_dir "$d" ; then find_targets_and_do do_install "$d" || exit 1 ; fi
    done
    ;;
uninstall)
    if $interactive && $ask_confirmation
    then
       confirmed="$(detect_and_ask_for_confirmation "${paths[@]}")" || exit 1
       IFS=$'\n' read -rd '' -a paths <<< "$confirmed"
    fi

    $cleanmode || for d in "${paths[@]}"; do
        if check_dir "$d" ; then find_targets_and_do do_uninstall "$d" || exit 1 ; fi
    done

    $cleanmode && for d in "${paths[@]}"; do
        if check_dir "$d" ; then do_uninstall "$d" || exit 1 ; fi
    done
    ;;
detect)
    result=$(do_detect "${paths[@]}")
    if [ -z "$result" ]
    then
        echo_stderr "Did not find anything that looks like a Wine installation."
        exit 1
    fi

    echo  "$result"
    ;;
list)
    for d in "${paths[@]}"; do
        if check_dir "$d" ; then do_list "$d" || exit 1 ; fi
    done
    ;;

verify)
    for d in "${paths[@]}"; do
         if check_dir "$d" ; then find_targets_and_do do_verify "$d" || exit 1 ; fi
    done
    ;;
*)
    print_usage
    exit 1
    ;;
esac

exit 0


