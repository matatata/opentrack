if(NOT WIN32)
    set(SDK_WINE "" CACHE BOOL "Build for Wine")
    if(SDK_WINE)
        otr_module(proto-wine)
        target_link_libraries(opentrack-proto-wine opentrack-csv)
        set(my-rt -lrt)
        if(APPLE)
            set(my-rt)
            set(my-opts "-DUSE_WS_PREFIX")
        endif()
        file(GLOB wine-deps "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx")
        #install(FILES ${wine-deps} DESTINATION "${opentrack-src}/proto-wine")
        set(winegxx-multilib "-m32")
        if (NOT OPENTRACK_WINE_ARCH STREQUAL "")
            set(winegxx-multilib "${OPENTRACK_WINE_ARCH}")
        endif()
        if(PREBUILT_WINE_WRAPPER_LOCATION)
            install(FILES "${PREBUILT_WINE_WRAPPER_LOCATION}/opentrack-wrapper-wine.exe.so" DESTINATION ${opentrack-libexec})
        else()
            add_custom_command(
            OUTPUT opentrack-wrapper-wine.exe.so
            DEPENDS ${wine-deps}
            COMMAND wineg++ -mconsole -g -DNOMINMAX -O2 ${winegxx-multilib} -std=c++17 -fPIC -o
                    opentrack-wrapper-wine.exe -I "${CMAKE_SOURCE_DIR}" -I "${CMAKE_BINARY_DIR}"
                    ${wine-deps} -Wall -Wextra -Wpedantic
                    ${my-rt} ${my-opts})
            if (APPLE)
                file(GLOB bridge-deps "${CMAKE_CURRENT_SOURCE_DIR}/bridge/*")
                add_custom_command(
                OUTPUT winelib/opentrack-wrapper-wine.dll.so
                DEPENDS ${wine-deps} ${bridge-deps}
                COMMAND mkdir -p winelib
                COMMAND wineg++ -DDLL=1 -shared -mconsole -g -DNOMINMAX -O2 ${winegxx-multilib} -std=c++17 -fPIC -o
                        winelib/opentrack-wrapper-wine.dll -I "${CMAKE_SOURCE_DIR}" -I "${CMAKE_BINARY_DIR}"
                        "${CMAKE_CURRENT_SOURCE_DIR}/bridge/opentrack-wrapper-wine.spec" ${wine-deps} -Wall -Wextra -Wpedantic
                        ${my-rt} ${my-opts}
                COMMAND cp winelib/opentrack-wrapper-wine.dll.so winelib/opentrack-wrapper-wine.dll
                )
                add_custom_command(
                OUTPUT fakedll/opentrack-wrapper-wine.dll opentrack-wrapper-wine.def
                DEPENDS ${bridge-deps}
                COMMAND mkdir -p fakedll
                COMMAND winebuild --def -E "${CMAKE_CURRENT_SOURCE_DIR}/bridge/opentrack-wrapper-wine.spec" > opentrack-wrapper-wine.def
                COMMAND x86_64-w64-mingw32-gcc -shared -o fakedll/opentrack-wrapper-wine.dll -I "${CMAKE_CURRENT_SOURCE_DIR}/bridge/" "${CMAKE_CURRENT_SOURCE_DIR}/bridge/opentrack-wrapper-wine-dummy-dll.c" opentrack-wrapper-wine.def
                )
                add_custom_command(
                DEPENDS ${bridge-deps}
                OUTPUT otrwinebridge.exe otrwinebridge.rc.o
                COMMAND x86_64-w64-mingw32-windres "${CMAKE_CURRENT_SOURCE_DIR}/bridge/otrwinebridge.rc" -o otrwinebridge.rc.o
                COMMAND x86_64-w64-mingw32-gcc -fno-permissive -mwindows  -Wl,-Bdynamic -L./fakedll/ -lopentrack-wrapper-wine "${CMAKE_CURRENT_SOURCE_DIR}/bridge/otrwinebridge.c" otrwinebridge.rc.o -I "${CMAKE_CURRENT_SOURCE_DIR}/bridge/"  -o otrwinebridge.exe
                )
                add_custom_target(wine-wrapper DEPENDS opentrack-wrapper-wine.exe.so winelib/opentrack-wrapper-wine.dll.so fakedll/opentrack-wrapper-wine.dll otrwinebridge.exe)
                install(FILES "${CMAKE_CURRENT_BINARY_DIR}/winelib/opentrack-wrapper-wine.dll" "${CMAKE_CURRENT_SOURCE_DIR}/bridge/README.txt" "${CMAKE_CURRENT_BINARY_DIR}/otrwinebridge.exe" DESTINATION ${CMAKE_INSTALL_PREFIX}/OtrWineBridge/)
            else()
                add_custom_target(wine-wrapper DEPENDS opentrack-wrapper-wine.exe.so)
            endif()
            add_dependencies(opentrack-proto-wine wine-wrapper)
            add_dependencies(wine-wrapper opentrack-compat)
            install(FILES "${CMAKE_CURRENT_BINARY_DIR}/opentrack-wrapper-wine.exe.so" DESTINATION ${opentrack-libexec})
        endif()
    endif()
endif()
