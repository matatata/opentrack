if(APPLE)
    otr_escape_string(srcdir "${CMAKE_SOURCE_DIR}")
    otr_escape_string(instdir "${CMAKE_INSTALL_PREFIX}")
    otr_escape_string(commit "${OPENTRACK_COMMIT}")
    install(CODE "
       execute_process(COMMAND /bin/sh \"${srcdir}/macosx/make-app-bundle.sh\"
                                        \"${srcdir}/macosx\"
                                        \"${instdir}\"
                                       \"${commit}\"
                                       \"${CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY}\"
                                       \"${CMAKE_OSX_ARCHITECTURES}\"
                                       \"${MACDEPLOYQT_EXECUTABLE}\")
    ")
    # TODO create pkg of postinstall
    add_custom_command(
        OUTPUT "MacOpentrackPostinstall.pkg"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/scripts/postinstall"
        COMMAND pkgbuild --nopayload
            --script "${CMAKE_CURRENT_SOURCE_DIR}/scripts/"
            --identifier com.github.matatata.opentrack.postinstall   #--version 25.1.1.0 # todo use version like this not matatata
            "MacOpentrackPostinstall.pkg"
    )

    add_custom_target(macosx_pkg ALL DEPENDS MacOpentrackPostinstall.pkg)
endif()
