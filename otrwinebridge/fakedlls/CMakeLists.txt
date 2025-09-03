if(NOT WIN32)



        add_custom_target(
            fake_ft
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/freetrackclient_main.c"
            COMMAND
                x86_64-w64-mingw32-gcc   -shared "${CMAKE_CURRENT_SOURCE_DIR}/freetrackclient_main.c" -o freetrackclient64.dll
            COMMAND
                i686-w64-mingw32-gcc  -shared "${CMAKE_CURRENT_SOURCE_DIR}/freetrackclient_main.c" -o freetrackclient.dll
        )

        add_custom_target(
            fake_np
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/npclient_main.c"
            COMMAND
                x86_64-w64-mingw32-gcc -shared "${CMAKE_CURRENT_SOURCE_DIR}/npclient_main.c" -o NPClient64.dll
            COMMAND
                i686-w64-mingw32-gcc  -shared "${CMAKE_CURRENT_SOURCE_DIR}/npclient_main.c" -o NPClient.dll
        )

        install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/NPClient.dll"
            "${CMAKE_CURRENT_BINARY_DIR}/NPClient64.dll"
            "${CMAKE_CURRENT_BINARY_DIR}/freetrackclient.dll"
            "${CMAKE_CURRENT_BINARY_DIR}/freetrackclient64.dll"
            DESTINATION otrwinebridge/
        )

endif()
