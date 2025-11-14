if(NOT WIN32)
    set(SDK_WINE "" CACHE BOOL "Build for Wine")
    if(SDK_WINE AND WINECXX_COMPILER)
        otr_module(proto-wine)
        target_link_libraries(opentrack-proto-wine PRIVATE opentrack-csv)
        set(my-rt -lrt)
        if(APPLE)
            set(my-rt)
            set(my-opts "-DUSE_WS_PREFIX")
        endif()

        if(CMAKE_COMPILER_IS_CLANG)
            message(WARNING "Linkage of opentrack-wrapper-wine.exe.so may fail with Unix clang on x86-64. On macOS it is known to fail since Wine 10.x. See https://bugs.winehq.org/show_bug.cgi?id=58580")
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
            COMMAND ${WINECXX_COMPILER} -mconsole -g -DNOMINMAX -O2 ${winegxx-multilib} -std=c++17 -fPIC -o
                    opentrack-wrapper-wine.exe -I "${CMAKE_SOURCE_DIR}" -I "${CMAKE_BINARY_DIR}"
                    ${wine-deps} -Wall -Wextra -Wpedantic
                    ${my-rt} ${my-opts})
            add_custom_target(wine-wrapper DEPENDS opentrack-wrapper-wine.exe.so)
            add_dependencies(opentrack-proto-wine wine-wrapper)
            add_dependencies(wine-wrapper opentrack-compat)
            install(FILES "${CMAKE_CURRENT_BINARY_DIR}/opentrack-wrapper-wine.exe.so" DESTINATION ${opentrack-libexec})
        endif()
    endif()
endif()
