cmake_minimum_required(VERSION 3.10)

set(SDK_WINE_PATH "" CACHE BOOL "Where did you clone the wine repo?")




if((LINUX OR APPLE) AND (EXISTS "${SDK_WINE_PATH}" AND IS_DIRECTORY "${SDK_WINE_PATH}"))

    function(mk_lib name cflags lflags suffix)
        add_library(${name} SHARED
            wine/npclient_wine_unixlib.c
        )

        target_compile_options(${name} PUBLIC -fvisibility=hidden ${cflags})
        target_link_options(${name} PUBLIC ${lflags})
        target_link_libraries(${name} PUBLIC "opentrackclient_static${suffix}")
        target_compile_definitions(${name} PUBLIC WINE_UNIX_LIB)
        target_include_directories(${name} PRIVATE
            "${SDK_WINE_PATH}/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../freetrackclient"
        )
        set_target_properties(${name} PROPERTIES
            LIBRARY_OUTPUT_NAME "npclient${suffix}"
            PREFIX "" SUFFIX ".so")
    endfunction()

    if(APPLE)
        mk_lib(npclient "" "" "")
    else()
        mk_lib(npclient "-m32" "-m32" "")
        mk_lib(npclient64 "-m64" "-m64" "64")
    endif()

    if(APPLE)
        # on APPLE there's no 32bit so just copy the .so to 64.so
        add_custom_target(np_cp_so_to_64so ALL
            COMMAND cp
            "${CMAKE_CURRENT_BINARY_DIR}/npclient.so"
            "${CMAKE_CURRENT_BINARY_DIR}/npclient64.so"
            DEPENDS freetrackclient
        )
        set_property( TARGET np_cp_so_to_64so APPEND PROPERTY ADDITIONAL_CLEAN_FILES
            "${CMAKE_CURRENT_BINARY_DIR}/npclient64.so"
        )
    endif()

     # Compile the wine builtin DLLS:
    function(mk_dll mingw_target suffix)
        set(dllname "npclient${suffix}.dll")
        add_custom_command(
            OUTPUT ${dllname}
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/npclient.c"

            COMMAND winegcc -b ${mingw_target} -DNP_WINE_UNIX_LIB
                    -I"${CMAKE_CURRENT_SOURCE_DIR}"
                    -I"${CMAKE_CURRENT_SOURCE_DIR}/wine"
                    -I"${CMAKE_SOURCE_DIR}/freetrackclient"
                    -I"${SDK_WINE_PATH}/include"
                    -Wall -Wextra -Wpedantic -Os -fomit-frame-pointer -mtune=generic -ffunction-sections -fdata-sections
                    -Wl,--kill-at,--nxcompat,--dynamicbase,--as-needed,--gc-sections
                    -Wl,--no-insert-timestamp
                    -fno-math-errno -fno-trapping-math -fmerge-all-constants
                    -fno-stack-protector
                    -fvisibility=hidden
                    -Wl,--wine-builtin
                    -shared
                    "${CMAKE_CURRENT_SOURCE_DIR}/npclient.c"
                    -o ${dllname}
            COMMAND
                "${mingw_target}-strip" --strip-all --remove-section=.eh_frame ${dllname}
        )
    endfunction()

    mk_dll(x86_64-w64-mingw32 "64")
    mk_dll(i686-w64-mingw32 "")

    add_custom_target(wine_npclient.dll DEPENDS npclient64.dll npclient.dll)

    add_dependencies(npclient wine_npclient.dll)

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/npclient.so"
        "${CMAKE_CURRENT_BINARY_DIR}/npclient64.so"
        DESTINATION otrwinebridge/wine/x86_64-unix
    )

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/npclient64.dll"
        DESTINATION otrwinebridge/wine/x86_64-windows
    )

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/npclient.dll"
        DESTINATION otrwinebridge/wine/i386-windows
    )

    function(mk_exec mingw_target suffix)
        add_custom_command(
            OUTPUT test${suffix}.exe
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/wine/test.c" npclient${suffix}.dll

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

    add_custom_target(npclient.wine_test DEPENDS test.exe test64.exe)
    add_dependencies(npclient.wine_test wine_npclient.dll)


    add_dependencies(npclient wine_npclient.dll npclient.wine_test)

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
