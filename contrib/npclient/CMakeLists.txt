if((LINUX OR APPLE) AND MINGW_GCC_COMPILER64 AND MINGW_GCC_COMPILER)
    # I adapted what's described in COMPILE.TXT
    set(shared_dll_cflags
        -Wall -Wextra -Wpedantic -Os -fomit-frame-pointer -mtune=generic -ffunction-sections -fdata-sections
        -Wl,--kill-at,--nxcompat,--dynamicbase,--as-needed,--gc-sections
        -Wl,--no-insert-timestamp
        -fno-math-errno -fno-trapping-math -fmerge-all-constants
        -fno-stack-protector
        -fvisibility=hidden
    )

    set(common_dll_cflags
        -march=pentium4 ${shared_dll_cflags}
    )
    set(common_dll_cflags64
        -Wl,--high-entropy-va ${shared_dll_cflags}
    )
    # Compile the native windows DLLS:
    function(mk_dll suffix)
        set(dllname "npclient${suffix}.dll")
        add_custom_command(
            OUTPUT ${dllname}
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/npclient.c"

            COMMAND ${MINGW_GCC_COMPILER${suffix}}
                    -I"${CMAKE_CURRENT_SOURCE_DIR}"
                    ${common_dll_cflags${suffix}}
                    -mdll
                    "${CMAKE_CURRENT_SOURCE_DIR}/npclient.c"
                    -o ${dllname}
            COMMAND
                ${MINGW_STRIP${suffix}} --strip-all --remove-section=.eh_frame ${dllname}
        )

        install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/${dllname}"
            DESTINATION "${CMAKE_SOURCE_DIR}/bin/"
        )

    endfunction()

    mk_dll("64")
    mk_dll("")

    add_custom_target(npclient.dlls ALL DEPENDS npclient64.dll npclient.dll)

endif()
