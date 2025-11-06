

AppName="OpentrackWineBridge3"
AppIdentifier="com.github.matatata.$AppName"
AppVersion=1.0

rm -rf "$AppName.app"



# icns="$AppName.app/Contents/Resources/$AppName.icns"

# Build iconset
tmp=$(mktemp -d)
icns="$tmp/$AppName.icns"
mkdir -p "$tmp/$AppName.iconset"
/usr/bin/sips -z 512 512 Icon.png --out "$tmp/Icon.png"
sips -z 16 16     "$tmp/Icon.png" --out "$tmp/$AppName.iconset/icon_16x16.png"
sips -z 32 32     "$tmp/Icon.png" --out "$tmp/$AppName.iconset/icon_16x16@2x.png"
sips -z 32 32     "$tmp/Icon.png" --out "$tmp/$AppName.iconset/icon_32x32.png"
sips -z 64 64     "$tmp/Icon.png" --out "$tmp/$AppName.iconset/icon_32x32@2x.png"
sips -z 128 128   "$tmp/Icon.png" --out "$tmp/$AppName.iconset/icon_128x128.png"
sips -z 256 256   "$tmp/Icon.png" --out "$tmp/$AppName.iconset/icon_128x128@2x.png"
sips -z 512 512   "$tmp/Icon.png" --out "$tmp/$AppName.iconset/icon_256x256@2x.png"
sips -z 512 512   "$tmp/Icon.png" --out "$tmp/$AppName.iconset/icon_512x512.png"
iconutil -c icns -o "$icns" "$tmp/$AppName.iconset"


mkdir -p "$AppName.app"/Contents/{MacOS,Resources}

cp app.sh "$AppName.app/Contents/MacOS/$AppName"
chmod a+x "$AppName.app/Contents/MacOS/$AppName"
cp "$icns" "$AppName.app/Contents/Resources/"
#ditto "~/Desktop/pickle/build/pickle.app" "$AppName.app/Contents/Resources/"

cat << EOT > "$AppName.app/Contents/Info.plist"
<?xml version="1.0" encoding="UTF-8" standalone="no"?><plist version="1.0">
  <dict>
    <key>CFBundleExecutable</key>
    <string>$AppName</string>
    <key>CFBundleIdentifier</key>
    <string>$AppIdentifier</string>
    <key>CFBundleIconFile</key>
    <string>$AppName.icns</string>
    <key>LSArchitecturePriority</key>
    <array>
        <string>arm64</string>
    </array>
    <key>LSRequiresNativeExecution</key>
    <true/>
    <key>CFBundleGetInfoString</key>
    <string>$AppVersion</string>
    <key>CFBundleVersion</key>
    <string>1.0</string>
    <key>CFBundleShortVersionString</key>
    <string>1.0</string>
</dict>
</plist>
EOT


