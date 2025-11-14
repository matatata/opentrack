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
    # function(mk_dll name mingw_target basedir cfilename suffix)
    function(mk_dll)
        cmake_parse_arguments(arg "NOINSTALL" "NAME;SUFFIX;INCLUDEDIR;MINGW_TARGET" "SOURCES;CFLAGS;LFLAGS" ${ARGN})
        set(dllname "${arg_NAME}${arg_SUFFIX}.dll")
        add_custom_command(
            OUTPUT ${dllname}
            DEPENDS ${arg_SOURCES}

            COMMAND ${WINEGCC_COMPILER} -b ${arg_MINGW_TARGET}
                    -I"${arg_INCLUDEDIR}"
                    -I"${CMAKE_CURRENT_SOURCE_DIR}/../wine-unix-lib"
                    -I"${SDK_WINE_PATH}/include"
                    ${arg_CFLAGS}
                    -DWINE_BUILTIN_DLL -Wl,--wine-builtin
                    ${common_dll_cflags${arg_SUFFIX}}
                    -shared
                    ${arg_SOURCES}
                    -o ${dllname}
            COMMAND
                ${MINGW_STRIP${arg_SUFFIX}} --strip-all --remove-section=.eh_frame ${dllname}
        )
        if(NOT arg_NOINSTALL)
            install(FILES
                "${CMAKE_CURRENT_BINARY_DIR}/${arg_NAME}64.dll"
                DESTINATION "${opentrack-install-wine-unixlib}/x86_64-windows"
            )

            install(FILES
                "${CMAKE_CURRENT_BINARY_DIR}/${arg_NAME}.dll"
                DESTINATION "${opentrack-install-wine-unixlib}/i386-windows"
            )
        endif()
    endfunction()

    mk_dll(NAME freetrackclient MINGW_TARGET x86_64-w64-mingw32 INCLUDEDIR "${CMAKE_SOURCE_DIR}/freetrackclient"
        SOURCES "${CMAKE_SOURCE_DIR}/freetrackclient/freetrackclient.c" SUFFIX "64"
        NOINSTALL
    )
    mk_dll(NAME freetrackclient MINGW_TARGET i686-w64-mingw32 INCLUDEDIR "${CMAKE_SOURCE_DIR}/freetrackclient/"
        SOURCES "${CMAKE_SOURCE_DIR}/freetrackclient/freetrackclient.c" SUFFIX ""
        NOINSTALL
    )
    add_custom_target("wine_freetrackclient.dlls" ALL DEPENDS freetrackclient64.dll freetrackclient.dll)

    mk_dll(NAME npclient MINGW_TARGET x86_64-w64-mingw32 INCLUDEDIR "${CMAKE_SOURCE_DIR}/contrib/npclient"
        SOURCES "${CMAKE_SOURCE_DIR}/contrib/npclient/npclient.c" SUFFIX "64"
        NOINSTALL
    )
    mk_dll(NAME npclient MINGW_TARGET i686-w64-mingw32 INCLUDEDIR "${CMAKE_SOURCE_DIR}/contrib/npclient/"
        SOURCES "${CMAKE_SOURCE_DIR}/contrib/npclient/npclient.c" SUFFIX ""
        NOINSTALL
    )
    add_custom_target("wine_npclient.dlls" ALL DEPENDS npclient64.dll npclient.dll)


    mk_dll(NAME otrclient MINGW_TARGET x86_64-w64-mingw32 INCLUDEDIR "${CMAKE_SOURCE_DIR}"
        SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/otrclient.c"
        CFLAGS -DOTR_REQUIRED SUFFIX "64"
    )
    mk_dll(NAME otrclient MINGW_TARGET i686-w64-mingw32 INCLUDEDIR "${CMAKE_SOURCE_DIR}"
        SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/otrclient.c"
        CFLAGS -DOTR_REQUIRED SUFFIX ""
    )
    add_custom_target("wine_otrclient.dlls" ALL DEPENDS otrclient64.dll otrclient.dll)

    add_subdirectory(test)

endif()






