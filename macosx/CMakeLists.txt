if(APPLE)
    otr_escape_string(srcdir "${CMAKE_SOURCE_DIR}")
    otr_escape_string(instdir "${CMAKE_INSTALL_PREFIX}")
    otr_escape_string(commit "${OPENTRACK_COMMIT}")

    configure_file(Info.in.plist Info.plist @ONLY)
    set_target_properties(opentrack-executable PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_BINARY_DIR}/Info.plist")

    install(CODE "
       execute_process(COMMAND /bin/sh \"${srcdir}/macosx/make-app-bundle.sh\"
                                        \"${srcdir}/macosx\"
                                        \"${instdir}\"
                                       \"${commit}\"
                                       \"${CMAKE_OSX_ARCHITECTURES}\"
                                       \"${MACDEPLOYQT_EXECUTABLE}\")
    ")

endif()
