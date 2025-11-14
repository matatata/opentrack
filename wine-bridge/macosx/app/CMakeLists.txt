if(APPLE)
    project(wine-bridge-mac VERSION 1.0.0)

    set(PICKLE_APP_PATH "" CACHE PATH "Where is the pickle.app? Get it at https://github.com/matatata/pickle")

    set(AppName "OpentrackWineUtil")
    set(AppVersion "${PROJECT_VERSION}")
    set(AppIdentifier "com.github.matatata.${AppName}")

    set(app_deps "${CMAKE_CURRENT_SOURCE_DIR}/app.sh")
    set(app_output "${CMAKE_CURRENT_BINARY_DIR}/${AppName}.app")
    add_custom_command(
        DEPENDS "${app_deps}"
        OUTPUT "${app_output}"
        COMMAND /usr/local/bin/platypus
                    --droppable --uniform-type-identifiers 'public.folder|com.apple.bundle'
                    --app-icon "${CMAKE_CURRENT_SOURCE_DIR}/Icon.icns"
                    --name "${AppName}"
                    --interface-type 'Progress Bar'
                    --interpreter '/bin/bash'
                    --bundle-identifier "${AppIdentifier}" --app-version "${AppVersion}"
                    --overwrite
                    --author "matatata"
                    --bundled-file "${CMAKE_CURRENT_SOURCE_DIR}/Credits.rtf"
                    --text-font "Monaco 11"
                    --bundled-file "${CMAKE_CURRENT_SOURCE_DIR}/../../otrwine.sh"
                    --bundled-file "${PICKLE_APP_PATH}"
                    "${app_deps}"
                    "${app_output}"
    )
    add_custom_target(util-app ALL
        DEPENDS "${app_output}"
    )

    set(app-install-dest "${CMAKE_INSTALL_PREFIX}/${opentrack-install-winebridge}")

    install(CODE "execute_process(COMMAND ditto -v \"${app_output}\" \"${app-install-dest}/${AppName}.app\" )")
    install(CODE "execute_process(COMMAND codesign -vv --force --sign \"Developer ID Application\" \"${app-install-dest}/${AppName}.app/Contents/Resources/script\" )")
    install(CODE "execute_process(COMMAND codesign -vv --force --sign  \"Developer ID Application\" \"${app-install-dest}/${AppName}.app/Contents/Resources/otrwine.sh\" )")
    install(CODE "execute_process(COMMAND codesign -vv --force --sign  \"Developer ID Application\" \"${app-install-dest}/${AppName}.app\" )")

endif()
