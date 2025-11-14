if(APPLE)
    project(wine-bridge-mac VERSION 1.0.0)

    set(PICKLE_APP_PATH "" CACHE PATH "Where is the pickle.app? Get it at https://github.com/matatata/pickle")
    set(OTRWINEMGR_APP_PATH "" CACHE PATH "Where is the Opentrack DLL Manager.app?")

    # using ditto to preserce permissions and code signature
    if(PICKLE_APP_PATH)
        install(CODE "
            execute_process(COMMAND ditto -v \"${PICKLE_APP_PATH}\" \"${CMAKE_INSTALL_PREFIX}/${opentrack-install-winebridge}/pickle.app\")
        ")
    endif()
    if(OTRWINEMGR_APP_PATH)
        install(CODE "
            execute_process(COMMAND ditto -v \"${OTRWINEMGR_APP_PATH}\" \"${CMAKE_INSTALL_PREFIX}/Applications/Opentrack DLL Manager.app\")
        ")
    endif()


    configure_file(scripts/postinstall.in scripts/postinstall @ONLY)
    configure_file(scripts/preinstall.in scripts/preinstall @ONLY)

    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/make-pkg.sh.in" "${CMAKE_CURRENT_BINARY_DIR}/make-pkg.sh" @ONLY)

    otr_escape_string(srcdir "${CMAKE_CURRENT_SOURCE_DIR}")
    otr_escape_string(bindir "${CMAKE_CURRENT_BINARY_DIR}")
    otr_escape_string(scriptdir "${CMAKE_CURRENT_BINARY_DIR}/scripts")
    otr_escape_string(instdir "${CMAKE_INSTALL_PREFIX}")
    otr_escape_string(commit "${OPENTRACK_COMMIT}")

    install(CODE "
       execute_process(COMMAND /bin/sh \"${bindir}/make-pkg.sh\"
                                            \"${srcdir}\"
                                            \"${scriptdir}\"
                                            \"${instdir}\"
                                            \"${commit}\"
                                            \"${bindir}\"
     )")

endif()
