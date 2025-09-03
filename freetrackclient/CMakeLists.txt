cmake_minimum_required(VERSION 3.10)

set(SDK_WINE_PATH "" CACHE PATH "Where did you clone the wine repo?")

if((LINUX OR APPLE) AND (EXISTS "${SDK_WINE_PATH}" AND IS_DIRECTORY "${SDK_WINE_PATH}"))

    if(APPLE)
        set(CMAKE_OSX_ARCHITECTURES "x86_64")
    endif()

    function(mk_lib name cflags lflags suffix)
        add_library(${name} SHARED
            wine/freetrackclient_wine_unixlib.c
        )

        target_compile_options(${name} PUBLIC -fvisibility=hidden ${cflags})
        target_link_options(${name} PUBLIC ${lflags})
        target_link_libraries(${name} PUBLIC "opentrackclient_static${suffix}")
        target_compile_definitions(${name} PUBLIC WINE_UNIX_LIB)
        target_include_directories(${name} PRIVATE
            "${SDK_WINE_PATH}/include"
        )
        set_target_properties(${name} PROPERTIES
            LIBRARY_OUTPUT_NAME "freetrackclient${suffix}"
            PREFIX "" SUFFIX ".so")
    endfunction()

    if(APPLE)
        mk_lib(freetrackclient "" "" "")
    else()
        mk_lib(freetrackclient "-m32" "-m32" "")
        mk_lib(freetrackclient64 "-m64" "-m64" "64")
    endif()

    if(APPLE)
        # on APPLE there's no 32bit so just copy the .so to 64.so
        add_custom_target(ft_cp_so_to_64so ALL
            COMMAND cp
            "${CMAKE_CURRENT_BINARY_DIR}/freetrackclient.so"
            "${CMAKE_CURRENT_BINARY_DIR}/freetrackclient64.so"
            DEPENDS freetrackclient
        )
        set_property( TARGET ft_cp_so_to_64so APPEND PROPERTY ADDITIONAL_CLEAN_FILES
            "${CMAKE_CURRENT_BINARY_DIR}/freetrackclient64.so"
        )
    endif()

    # Compile the wine builtin DLLS:
    function(mk_dll mingw_target suffix)
        set(dllname "freetrackclient${suffix}.dll")
        add_custom_command(
            OUTPUT ${dllname}
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/freetrackclient.c"

            COMMAND winegcc -b ${mingw_target} -DFT_WINE_UNIX_LIB
                    -I"${CMAKE_CURRENT_SOURCE_DIR}"
                    -I"${CMAKE_CURRENT_SOURCE_DIR}/wine"
                    -I"${SDK_WINE_PATH}/include"
                    -Wall -Wextra -Wpedantic -Os -fomit-frame-pointer -mtune=generic -ffunction-sections -fdata-sections
                    -Wl,--kill-at,--nxcompat,--dynamicbase,--as-needed,--gc-sections
                    -Wl,--no-insert-timestamp
                    -fno-math-errno -fno-trapping-math -fmerge-all-constants
                    -fno-stack-protector
                    -fvisibility=hidden
                    -Wl,--wine-builtin
                    -shared
                    "${CMAKE_CURRENT_SOURCE_DIR}/freetrackclient.c"
                    -o ${dllname}
            COMMAND
                "${mingw_target}-strip" --strip-all --remove-section=.eh_frame ${dllname}
        )
    endfunction()

    mk_dll(x86_64-w64-mingw32 "64")
    mk_dll(i686-w64-mingw32 "")

    add_custom_target(wine_freetrackclient.dll DEPENDS freetrackclient64.dll freetrackclient.dll)

    if(APPLE)
        install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/freetrackclient.so"
            "${CMAKE_CURRENT_BINARY_DIR}/freetrackclient64.so"
            DESTINATION otrwinebridge/wine/x86_64-unix
        )
    else()
        install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/freetrackclient64.so"
            DESTINATION otrwinebridge/wine/x86_64-unix
        )
        install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/freetrackclient.so"
            DESTINATION otrwinebridge/wine/i386-unix
        )
    endif()

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/freetrackclient64.dll"
        DESTINATION otrwinebridge/wine/x86_64-windows
    )

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/freetrackclient.dll"
        DESTINATION otrwinebridge/wine/i386-windows
    )

    function(mk_exec mingw_target suffix)
        add_custom_command(
            OUTPUT test${suffix}.exe
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/wine/test.c" freetrackclient${suffix}.dll

            COMMAND
                i686-w64-mingw32-gcc
                    -I"${CMAKE_CURRENT_SOURCE_DIR}"
                    -I"${CMAKE_CURRENT_SOURCE_DIR}/wine"
                    -Wl,--enable-stdcall-fixup
                    "${CMAKE_CURRENT_SOURCE_DIR}/wine/test.c"
                    -o test${suffix}.exe
        )
    endfunction()

    mk_exec(i686-w64-mingw32 "")
    mk_exec(x86_64-w64-mingw32 "64")

    add_custom_target(freetrackclient.wine_test DEPENDS test.exe test64.exe)
    add_dependencies(freetrackclient.wine_test wine_freetrackclient.dll)

    add_dependencies(freetrackclient wine_freetrackclient.dll freetrackclient.wine_test)
else()
    # WINDOWS
    # If you want to recompile the pure windows dll on windows add it here perhaps like so:
    # add_library(freetrackclient SHARED
    #    freetrackclient.c
    # )
    # target_compile_options(freetrackclient
    #             -Wall -Wextra -Wpedantic -Os -fomit-frame-pointer -mtune=generic -ffunction-sections -fdata-sections
    #             -Wl,--kill-at,--nxcompat,--dynamicbase,--as-needed,--gc-sections
    #             -Wl,--no-insert-timestamp
    #             -fno-math-errno -fno-trapping-math -fmerge-all-constants
    #             -fno-stack-protector
    #             -fvisibility=hidden)
endif()






