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
osx_arch="${4:-'unknownarch'}"
macdeployqt_executable="${5:-macdeployqt}"

if [ -n "$CODESIGN" ]
then
 CODESIGN_IDENTITY=${CODESIGN_IDENTITY:-Developer ID Application}
 echo "Code-Sign-Identity is $CODESIGN_IDENTITY"
fi


tmp="$(mktemp -d "/tmp/$APPNAME-tmp.XXXXXXX")"
test $? -eq 0

APPROOT="$install/Applications/$APPNAME.app"

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

if [ -n "$CODESIGN_IDENTITY" ]
then
  # first nested libaries, frameworks and plugins
  find  "$APPROOT"  \(  \( -type f -name "*.dylib" -path "*/Contents/Frameworks/*" \) \
                    -o \( -type d -path "*/Contents/Frameworks/*.framework" \) \
                    -o \( -type f -name "*.dylib" -path "*/Contents/PlugIns/*" \) \
                    \)  -exec codesign  -vv --force --timestamp --sign "$CODESIGN_IDENTITY" "{}" \;
  # last the bundle itself
  codesign -vv --force --options runtime --timestamp --entitlements $dir/entitlements.plist --sign "$CODESIGN_IDENTITY" "$APPROOT"

  # Now everything else (except wine-bridgewhich  has it's own script)
  # X-Plane


  #find "$install/usr/" "$install/xplane/" "$install/doc/" "$install/wine-bridge/" "$install/SDK/" "$install/usr" -type f -exec codesign --force --verify -vv --options runtime --sign "$CODESIGN_IDENTITY" {} \;
fi


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
#  TODO

####################################


# create pkg
pkgroot="$install/../pkgroot"
rm -rf "$pkgroot" &> /dev/null

ditto "$APPROOT" "$pkgroot/Applications/$APPNAME.app"
ditto "$install/usr" "$pkgroot/usr"

appsupportroot="$pkgroot/Library/Application Support/$APPNAME"
mkdir -p "$appsupportroot"
for d in xplane doc wine-bridge SDK
do
    ditto "$install/$d" "$appsupportroot/$d"
done


#xattr -cr "$pkgroot"

PGKFILE="${version}_${osx_arch}.pkg"
rm -f *.pkg
pkgbuild --root "$pkgroot"  \
    --install-location / \
    --script $dir/scripts \
    --component-plist ../opentrack/macosx/MacOpentrack-pkg.plist \
    --version "${version}" \
    --identifier com.github.opentrack.pkg   \
    "${PGKFILE}_unsigned.pkg"

if productsign --sign "Developer ID Installer" "${PGKFILE}_unsigned.pkg" "$PGKFILE"
then
    pkgutil --check-signature  "$PGKFILE"
    rm "${PGKFILE}_unsigned.pkg"
else
    echo "could not sign ${PGKFILE}_unsigned.pkg"
    mv "${PGKFILE}_unsigned.pkg" "${PGKFILE}.pkg"
fi



# Check if we have a pkg otherwise fail

if [ -f "$PGKFILE" ]; then

  #To notarize do this:
  #xcrun notarytool submit "$DMGFILE" --apple-id appleid@example.com--team-id "TEAM_ID" --password "specific-app-password" --verbose --wait
  #
  #xcrun stapler staple -v "$DMGFILE"

  ls -ial "$PGKFILE"
else
   echo "Failed to create $PGKFILE"
   exit 2
fi


