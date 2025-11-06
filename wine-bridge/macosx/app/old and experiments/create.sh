
AppName="OpentrackWineBridge"
AppIdentifier="de.matatata.$AppName"
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


# --droppable \
#--file-prompt \
/usr/local/bin/platypus \
    --app-icon "$icns"  \
    --name "$AppName" \
    --interface-type 'Progress Bar' \
    --interpreter '/bin/bash' \
    --droppable \
    --uniform-type-identifiers 'public.folder|com.apple.bundle' \
    --bundle-identifier "$AppIdentifier" --app-version "$AppVersion" \
    --overwrite \
    --author "matatata" $(test -n "$DEVELOP" && echo "--symlink" ) \
    --bundled-file "Credits.rtf" \
    --text-font "Monaco 11" \
    app.sh
    # --file-prompt \
    # --admin-privileges \
    #    --quit-after-execution \




codesign --sign "Developer ID Application" "$AppName.app/Contents/Resources/script"
codesign --deep --sign "Developer ID Application" "$AppName.app"










/usr/local/bin/platypus --background \
 --app-icon "$icns" \
 --name 'MenuThing' \
  --interface-type 'Status Menu'  \
  --interpreter '/bin/bash' \
   --status-item-kind 'Icon' --status-item-icon  "$icns" \
   --status-item-sysfont \
   --status-item-template-icon \
   --overwrite \
   --author "matatata" $(test -n "$DEVELOP" && echo "--symlink" ) \
    'MenuThing.bash'

rm -rf "$tmp"

















#codesign --verify --deep -vv  "$AppName.app"


# mkdir -p "$AppName.app"/Contents/{MacOS,Resources}

# cp app.sh "$AppName.app/Contents/MacOS/$AppName"
# chmod a+x "$AppName.app/Contents/MacOS/$AppName"


# cat << EOT > "$AppName.app/Contents/Info.plist"
# <?xml version="1.0" encoding="UTF-8" standalone="no"?><plist version="1.0">
#   <dict>
#     <key>CFBundleExecutable</key>
#     <string>$AppName</string>
#     <key>CFBundleIdentifier</key>
#     <string>$AppIdentifier</string>
#     <key>CFBundleIconFile</key>
#     <string>$AppName.icns</string>
#     <key>LSArchitecturePriority</key>
#     <array>
#         <string>arm64</string>
#     </array>
#     <key>LSRequiresNativeExecution</key>
#     <true/>
#     <key>CFBundleGetInfoString</key>
#     <string>$AppVersion</string>
#     <key>CFBundleVersion</key>
#     <string>1.0</string>
#     <key>CFBundleShortVersionString</key>
#     <string>1.0</string>
# </dict>
# </plist>
# EOT

# codesign --force --deep --sign "Developer ID Application" "$AppName.app"


# You should also try https://github.com/andrewpetrochenkov/mac-appify


