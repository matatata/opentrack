if(NOT WIN32)
    set(OTRWINEBRIDGE "" CACHE BOOL "Build otrwinebridge stuff")
    if(OTRWINEBRIDGE)
        project(otrwinebridge VERSION 1.0.0 DESCRIPTION "")

        add_custom_target(
            otrwinebridge ALL

            DEPENDS
                "${CMAKE_CURRENT_SOURCE_DIR}/otrwinebridge.c"
                "${CMAKE_CURRENT_SOURCE_DIR}/otrwinebridge.rc"
                "${CMAKE_CURRENT_SOURCE_DIR}/resource.h"
            COMMAND x86_64-w64-mingw32-windres "${CMAKE_CURRENT_SOURCE_DIR}/otrwinebridge.rc" -o otrwinebridge.rc.o
            #-Wl,-Bdynamic -L./fakedll/ -lopentrack-wrapper-wine
            COMMAND x86_64-w64-mingw32-gcc -fno-permissive -mwindows  "${CMAKE_CURRENT_SOURCE_DIR}/otrwinebridge.c" otrwinebridge.rc.o -I "${CMAKE_CURRENT_SOURCE_DIR}"  -o otrwinebridge64.exe

            COMMAND i686-w64-mingw32-windres "${CMAKE_CURRENT_SOURCE_DIR}/otrwinebridge.rc" -o otrwinebridge.rc.o
            #-Wl,-Bdynamic -L./fakedll/ -lopentrack-wrapper-wine
            COMMAND i686-w64-mingw32-gcc -fno-permissive -mwindows  "${CMAKE_CURRENT_SOURCE_DIR}/otrwinebridge.c" otrwinebridge.rc.o -I "${CMAKE_CURRENT_SOURCE_DIR}"  -o otrwinebridge.exe
        )


        add_subdirectory(fakedlls)

        add_dependencies(otrwinebridge fake_ft fake_np)

         install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/otrwinebridge.exe" "${CMAKE_CURRENT_BINARY_DIR}/otrwinebridge64.exe"
            DESTINATION otrwinebridge/
        )

    endif()
endif()
