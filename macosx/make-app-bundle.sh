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
code_sign_identity="${4:-"-"}"
echo "Code-Sign-Identity is $code_sign_identity"
osx_arch="${5:-'unknownarch'}"
macdeployqt_executable="${6:-macdeployqt}"

tmp="$(mktemp -d "/tmp/$APPNAME-tmp.XXXXXXX")"
test $? -eq 0

APPROOT="$install/$APPNAME.app"


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


# Copy our own plist and set correct version

cp "$dir/Info.plist" "$APPROOT/Contents/"
sed -i '' -e "s#@OPENTRACK-VERSION@#$version#g" "$APPROOT/Contents/Info.plist"

# Copy PkgInfo
cp "$dir/PkgInfo" "$APPROOT/Contents/"

# Copy plugins
#mkdir -p "$APPROOT/Contents/PlugIns"
#mkdir -p "$APPROOT/Contents/MacOS/"
# put the modules in the bundle
#cp -v -r "$install/PlugIns" "$APPROOT/Contents/"
#cp -v -r "$install/Library/" "$APPROOT/Contents/Frameworks/"

if [[ -z "$SKIP_QT_MACDEPLOY" ]]
then
    echo "Will use macdeployqt to make the app standalone. This will take some time skip with env variable SKIP_QT_MACDEPLOY=1"
    "${macdeployqt_executable}" "$APPROOT" # -libpath="$APPROOT/Contents/Frameworks" #-libpath="$install/Library"
fi

if [ ! -d "$install/xplane" ]
then
 mkdir -p "$install/xplane"
fi

if [ "$code_sign_identity" = "-" ]
then
  echo "sign to run locally"
  # Sign it to run it locally otherwise you'll get errors. Also I've noticed that
  # this might make the binaries also usable for other users, but macOS will complain very much and it might not even work. I gave up and bought an Apple Developer Membership
  codesign --force --deep -vv --sign - "$APPROOT"
else
  # You have a proper developer certificate:
  echo "sign as $code_sign_identity"
  codesign --force --deep --verify -vv --options runtime --timestamp --entitlements $dir/entitlements.plist --sign "$code_sign_identity" "$APPROOT"
  find "$install/usr/" "$install/xplane/" "$install/doc/" "$install/Wine/" "$install/SDK/" -type f -exec codesign --force --verify -vv --options runtime --sign "$code_sign_identity" {} \;
fi



# create pkg
pkgroot="$install/../pkgroot"
rm -rf "$pkgroot" &> /dev/null

ditto "$APPROOT" "$pkgroot/Applications/$APPNAME.app"
ditto "$install/usr" "$pkgroot/usr"

appsupportroot="$pkgroot/Library/Application Support/$APPNAME"
mkdir -p "$appsupportroot"
for d in xplane doc Wine SDK
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
    --identifier com.github.matatata.opentrack   \
    "TEMP.pkg"

productsign --sign "Developer ID Installer" \
    "TEMP.pkg" "$PGKFILE"

pkgutil --check-signature  "$PGKFILE"



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


