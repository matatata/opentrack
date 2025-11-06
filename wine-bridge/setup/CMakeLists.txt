if((LINUX OR APPLE) AND MINGW_CXX_COMPILER AND MINGW_CXX_COMPILER64)

    # install(FILES "${CMAKE_CURRENT_BINARY_DIR}/setup.bat"
    #               DESTINATION "${opentrack-install-winebridge}")

    file(GLOB src-files "*.c")

    # configure_file("install.in.bat" "install.bat" @ONLY)
    # configure_file("registry-tool.in.bat" "registry-tool.bat" @ONLY)
    # configure_file("uninstall.in.bat" "uninstall.bat" @ONLY)
    # configure_file("install.in.c" "install.c" @ONLY)
    # configure_file("uninstall.in.c" "uninstall.c" @ONLY)
    # configure_file("registry-tool.in.c" "registry-tool.c" @ONLY)

    function(make_starter targetname name src)
        set(output_file "${CMAKE_CURRENT_BINARY_DIR}/${name}.exe")
        set(output_file64 "${CMAKE_CURRENT_BINARY_DIR}/${name}64.exe")
        add_custom_command(
        OUTPUT ${output_file} ${output_file64}
        DEPENDS "${src}"
        COMMAND ${MINGW_CXX_COMPILER} "${src}" -o "${output_file}"
        COMMAND ${MINGW_CXX_COMPILER64} "${src}" -o "${output_file64}"
        )
        add_custom_target("${targetname}_target" ALL DEPENDS ${output_file} ${output_file64})
        install(FILES ${output_file} ${output_file64}
            DESTINATION "${opentrack-install-winebridge}"
    )

    endfunction()

    # make_starter("uninstall" Opentrack-Uninstall "${CMAKE_CURRENT_BINARY_DIR}/uninstall.c" )
    # make_starter("install" Opentrack-Install  "${CMAKE_CURRENT_BINARY_DIR}/install.c" )
    make_starter("otr-proto-config" "Opentrack Protocol Config" "${CMAKE_CURRENT_BINARY_DIR}/registry-tool.c" )

    file(GLOB bat-files "${CMAKE_CURRENT_BINARY_DIR}/*.bat")
    install(FILES
        ${bat-files}
        DESTINATION "${opentrack-install-winebridge-bin}"
    )

endif()






