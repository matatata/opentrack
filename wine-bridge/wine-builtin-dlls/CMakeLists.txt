# Compile the wine builtin DLLS:
if((LINUX OR APPLE) AND (EXISTS "${SDK_WINE_PATH}" AND IS_DIRECTORY "${SDK_WINE_PATH}")
    AND MINGW_GCC_COMPILER64 AND MINGW_GCC_COMPILER AND WINEGCC_COMPILER)

        # I adapted what's described in contrib/client/COMPILE.TXT
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
    #TODO parse
    # make  OUPUT_DIRECTPRY
    function(mk_dll name mingw_target basedir cfilename suffix)
        set(dllname "${name}${suffix}.dll")
        add_custom_command(
            OUTPUT ${dllname}
            DEPENDS "${basedir}/${cfilename}"

            COMMAND ${WINEGCC_COMPILER} -b ${mingw_target}
                    -I"${basedir}"
                    -I"${CMAKE_CURRENT_SOURCE_DIR}/../wine-unix-lib"
                    -I"${SDK_WINE_PATH}/include"
                    -DWINE_BUILTIN_DLL -Wl,--wine-builtin
                    ${common_dll_cflags${suffix}}
                    -shared
                    "${basedir}/${cfilename}"
                    -o ${dllname}
            COMMAND
                ${MINGW_STRIP${suffix}} --strip-all --remove-section=.eh_frame ${dllname}
        )

        install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/${name}64.dll"
            DESTINATION "${opentrack-install-wine-unixlib}/x86_64-windows"
        )

        install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/${name}.dll"
            DESTINATION "${opentrack-install-wine-unixlib}/i386-windows"
        )
    endfunction()

    mk_dll(freetrackclient x86_64-w64-mingw32 "${CMAKE_SOURCE_DIR}/freetrackclient" "freetrackclient.c" "64")
    mk_dll(freetrackclient i686-w64-mingw32 "${CMAKE_SOURCE_DIR}/freetrackclient/" "freetrackclient.c" "")
    add_custom_target("wine_freetrackclient.dlls" ALL DEPENDS freetrackclient64.dll freetrackclient.dll)

    mk_dll(npclient x86_64-w64-mingw32 "${CMAKE_SOURCE_DIR}/contrib/npclient" "npclient.c" "64")
    mk_dll(npclient i686-w64-mingw32 "${CMAKE_SOURCE_DIR}/contrib/npclient/" "npclient.c" "")
    add_custom_target("wine_npclient.dlls" ALL DEPENDS npclient64.dll npclient.dll)

    add_subdirectory(test)

endif()






