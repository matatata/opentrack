if((LINUX OR APPLE) AND MINGW_GCC_COMPILER64 AND MINGW_GCC_COMPILER)
    set(toolname "wine-bridge")

    function(mk_exec sources suffix)
        set(fname "${toolname}${suffix}.exe")
        add_custom_command(
            OUTPUT "${fname}"
            DEPENDS ${sources}
            COMMAND
                ${MINGW_GCC_COMPILER${suffix}} -O -s
                    -I"${CMAKE_SOURCE_DIR}"
                    -Wl,--enable-stdcall-fixup
                    ${sources} -o "${fname}"
        )

        # install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${fname}"
        #     DESTINATION "${opentrack-install-winebridge-bin}")

    endfunction()

    mk_exec("${CMAKE_CURRENT_SOURCE_DIR}/bridge.c;${CMAKE_SOURCE_DIR}/compat/shm.c" "")
    mk_exec("${CMAKE_CURRENT_SOURCE_DIR}/bridge.c;${CMAKE_SOURCE_DIR}/compat/shm.c" "64")

    add_custom_target(wine-bridge-execs ALL  DEPENDS "${toolname}.exe" "${toolname}64.exe")
    add_dependencies(wine-bridge-execs wine_otrclient.dlls)
endif()
