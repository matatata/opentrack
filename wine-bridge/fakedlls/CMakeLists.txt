if((LINUX OR APPLE) AND MINGW_CXX_COMPILER AND MINGW_CXX_COMPILER64)

        add_custom_command(
            OUTPUT freetrackclient.dll
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/freetrackclient_main.c" "${CMAKE_CURRENT_SOURCE_DIR}/common_dllmain.c"
            COMMAND
                ${MINGW_GCC_COMPILER64}   -shared "${CMAKE_CURRENT_SOURCE_DIR}/freetrackclient_main.c" "${CMAKE_CURRENT_SOURCE_DIR}/common_dllmain.c" -o freetrackclient64.dll
            COMMAND
                ${MINGW_GCC_COMPILER}  -shared "${CMAKE_CURRENT_SOURCE_DIR}/freetrackclient_main.c" "${CMAKE_CURRENT_SOURCE_DIR}/common_dllmain.c" -o freetrackclient.dll
        )

        add_custom_command(
            OUTPUT npclient.dll
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/npclient_main.c" "${CMAKE_CURRENT_SOURCE_DIR}/common_dllmain.c"
            COMMAND
                ${MINGW_GCC_COMPILER64} -shared "${CMAKE_CURRENT_SOURCE_DIR}/npclient_main.c"  "${CMAKE_CURRENT_SOURCE_DIR}/common_dllmain.c" -o npclient64.dll
            COMMAND
                ${MINGW_GCC_COMPILER}  -shared "${CMAKE_CURRENT_SOURCE_DIR}/npclient_main.c" "${CMAKE_CURRENT_SOURCE_DIR}/common_dllmain.c" -o npclient.dll
        )

        add_custom_command(
            OUTPUT otrclient.dll
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/otrclient_main.c" "${CMAKE_CURRENT_SOURCE_DIR}/common_dllmain.c"
            COMMAND
                ${MINGW_GCC_COMPILER64} -shared "${CMAKE_CURRENT_SOURCE_DIR}/otrclient_main.c"  "${CMAKE_CURRENT_SOURCE_DIR}/common_dllmain.c" -o otrclient64.dll
            COMMAND
                ${MINGW_GCC_COMPILER}  -shared "${CMAKE_CURRENT_SOURCE_DIR}/otrclient_main.c" "${CMAKE_CURRENT_SOURCE_DIR}/common_dllmain.c" -o otrclient.dll
        )

        add_custom_target(fakedlls ALL DEPENDS freetrackclient.dll npclient.dll otrclient.dll)

        foreach(f   otrclient.dll otrclient64.dll
                    #npclient.dll npclient64.dll freetrackclient.dll freetrackclient64.dll
                )
            install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${f}"
                DESTINATION "${opentrack-install-winebridge-bin}"
            )
        endforeach(f)



endif()
