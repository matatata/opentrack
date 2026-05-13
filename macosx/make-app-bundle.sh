#!/bin/sh

# exit when any command fails
set -e

# keep track of the last executed command
trap 'last_command=$current_command; current_command=$BASH_COMMAND' DEBUG
# echo an error message before exiting
trap 'echo "--\n--\n--\n--\n\"${last_command}\" command exited with exit code $?."' EXIT

APPNAME=opentrack

# Alternative we could look at https://github.com/arl/macdeployqtfix ??

#macosx directory
dir="$1"
test -n "$dir"
# install directory
install="$2"
test -n "$install"
version="$3"
test -n "$version"
bindir="${4}"
osx_arch="${5:-'unknownarch'}"
macdeployqt_executable="${6:-macdeployqt}"

if [ -n "$CODESIGN" ]
then
 CODESIGN_IDENTITY=${CODESIGN_IDENTITY:-Developer ID Application}
 echo "Code-Sign-Identity is $CODESIGN_IDENTITY"
fi


tmp="$(mktemp -d "/tmp/$APPNAME-tmp.XXXXXXX")"
test $? -eq 0

APPROOT="$install/$APPNAME.app"
XPLANE_PLUGIN="$install/X-Plane-Plugin"
SDK="$install/sdk"

# TODO this is actually good
# Add rpath for application so it can find the libraries
#install_name_tool -add_rpath @executable_path/../Frameworks "$APPROOT/Contents/MacOS/$APPNAME"


# Create an 512 resolution size for the icon (for retina displays mostly)
#gm convert -size 512x512 "$dir/../gui/images/opentrack.png" "$tmp/opentrack.png"
convert "$dir/../gui/images/opentrack.png" -filter triangle -resize 512x512 "$tmp/opentrack.png"

mkdir -p "$APPROOT/Contents/Resources/"

# Build iconset
mkdir "$tmp/$APPNAME.iconset"
sips -z 16 16     "$tmp/opentrack.png" --out "$tmp/$APPNAME.iconset/icon_16x16.png"
sips -z 32 32     "$tmp/opentrack.png" --out "$tmp/$APPNAME.iconset/icon_16x16@2x.png"
sips -z 32 32     "$tmp/opentrack.png" --out "$tmp/$APPNAME.iconset/icon_32x32.png"
sips -z 64 64     "$tmp/opentrack.png" --out "$tmp/$APPNAME.iconset/icon_32x32@2x.png"
sips -z 128 128   "$tmp/opentrack.png" --out "$tmp/$APPNAME.iconset/icon_128x128.png"
sips -z 256 256   "$tmp/opentrack.png" --out "$tmp/$APPNAME.iconset/icon_128x128@2x.png"
sips -z 512 512   "$tmp/opentrack.png" --out "$tmp/$APPNAME.iconset/icon_256x256@2x.png"
sips -z 512 512   "$tmp/opentrack.png" --out "$tmp/$APPNAME.iconset/icon_512x512.png"
iconutil -c icns -o "$APPROOT/Contents/Resources/$APPNAME.icns" "$tmp/$APPNAME.iconset"
rm -rf "$tmp"

if [ -n "$DEPLOY" ]
then
    echo "Will use macdeployqt to make the app standalone. This will take some time."
    "${macdeployqt_executable}" "$APPROOT" # -libpath="$APPROOT/Contents/Frameworks" #-libpath="$install/Library"
else
    echo "Not using macdeployqt to make the app standalone. Enable with env variable DEPLOY=1"
fi

function do_codesign() {
    codesign -vv --force --timestamp --options runtime --sign "$CODESIGN_IDENTITY" "$@"
}

if [ -n "$CODESIGN_IDENTITY" ]
then
    find "$APPROOT" -type f \( \
        \( -path "*/Contents/Frameworks/*" -name "*.dylib" \) -o \
        \( -path "*/Contents/PlugIns/*" \( -name "*.dylib" -o -name "*.onnx" \) \) -o \
        \( -path "*/Contents/Resources/*" \( -name "*.dll" -o -name "*.exe" \) \) \
    \) -print0 | while IFS= read -r -d '' file; do
        do_codesign "$file"
    done

    if [ -d "$APPROOT/Contents/Frameworks" ]; then
        find "$APPROOT/Contents/Frameworks" -type d -name "*.framework" -print0 | while IFS= read -r -d '' fw; do
            do_codesign "$fw"
        done
    fi

    do_codesign --entitlements "$dir/entitlements.plist" "$APPROOT"
    codesign -vvv --deep --strict "$APPROOT"


    if [ -d "$XPLANE_PLUGIN" ]
    then
        # Sign X-Plane-Plugin
        do_codesign  "$XPLANE_PLUGIN/opentrack/mac_x64/opentrack.xpl"
        codesign -vvv --deep --strict "$XPLANE_PLUGIN/opentrack/mac_x64/opentrack.xpl"
    fi

    # Sign the opentrackclient framework
    do_codesign "$SDK/opentrackclient.framework/Versions/A/opentrackclient"
    do_codesign "$SDK//opentrackclient.framework"
    codesign -vvv --deep --strict "$SDK//opentrackclient.framework"

    do_codesign "$SDK/otrclient-tester"
    codesign -vvv --deep --strict "$SDK/otrclient-tester"

fi

# wine-bridge has it's own script)


if [ -z "$PACKAGE" ]
then
    echo "done. Not packaging. To do so set environment variable PACKAGE=1"
    exit 0
fi




################ distribution layout:
# Opentrack PKG:
#  - /Applications/Opentrack.app
#  - /Library/Application Support/opentrack
#  - /Library/Application Support/opentrack-wine-bridge
#  - X-Plane-Plugin

####################################

tmpdir=$(mktemp -d)


ditto "$install/" "$tmpdir/"

#ditto "$pkgroot/@opentrack-install-winebridge@/README".*  "$tmpdir/"

rm -f "$bindir"/*.dmg
DMGFILE="$bindir/MacOpentrack_${version}.dmg"

hdiutil create   -srcfolder "$tmpdir" -volname "MacOpentrack" -ov -format UDZO "$DMGFILE" #-size 250m
codesign  -vv --force --sign "$CODESIGN_IDENTITY" "$DMGFILE"

rm -rf "$tmpdir"


if [ -n "$NOTARIZE" ]
then
    xcrun notarytool submit "$DMGFILE" \
                   --keychain-profile "notarytool-password" \
                   --verbose \
                   --wait \

    xcrun stapler staple -v "$DMGFILE"

fi



