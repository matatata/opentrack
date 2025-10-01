cmake_minimum_required(VERSION 3.10)

project(freetrackclient DESCRIPTION "Produces to flavours of freetrackclient.dll: native windows and builtin wine-unixlib (see subdir ./wine)")

set(common_dll_cflags
    -Wall -Wextra -Wpedantic -Os -fomit-frame-pointer -mtune=generic -ffunction-sections -fdata-sections
    -Wl,--kill-at,--nxcompat,--dynamicbase,--as-needed,--gc-sections
    -Wl,--no-insert-timestamp
    -fno-math-errno -fno-trapping-math -fmerge-all-constants
    -fno-stack-protector
    -fvisibility=hidden
)

if(LINUX OR APPLE)

    add_subdirectory(wine)

    if(MINGW_GCC_COMPILER64 AND MINGW_GCC_COMPILER)
        # Compile the native pure windows DLLS:
        function(mk_dll mingw_target suffix)
            set(dllname "freetrackclient${suffix}.dll")
            add_custom_command(
                OUTPUT ${dllname}
                DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/freetrackclient.c"

                COMMAND ${mingw_target}-gcc
                        -I"${CMAKE_CURRENT_SOURCE_DIR}"
                        ${common_dll_cflags}
                        -shared
                        "${CMAKE_CURRENT_SOURCE_DIR}/freetrackclient.c"
                        -o ${dllname}
                COMMAND
                    "${mingw_target}-strip" --strip-all --remove-section=.eh_frame ${dllname}
            )

            install(FILES
                "${CMAKE_CURRENT_BINARY_DIR}/${dllname}"
                DESTINATION "${CMAKE_SOURCE_DIR}/bin/"
            )
        endfunction()

        mk_dll(x86_64-w64-mingw32 "64")
        mk_dll(i686-w64-mingw32 "")

        add_custom_target(freetrackclient.dlls ALL DEPENDS freetrackclient64.dll freetrackclient.dll)

        function(mk_exec mingw_target suffix)
            add_custom_command(
                OUTPUT test${suffix}.exe
                DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/test.c" freetrackclient${suffix}.dll

                COMMAND
                    ${mingw_target}-gcc
                        -I"${CMAKE_CURRENT_SOURCE_DIR}"
                        -Wl,--enable-stdcall-fixup
                        "${CMAKE_CURRENT_SOURCE_DIR}/test.c"
                        -o test${suffix}.exe
            )
        endfunction()

        mk_exec(i686-w64-mingw32 "")
        mk_exec(x86_64-w64-mingw32 "64")

        add_custom_target(freetrackclient.test ALL DEPENDS test.exe test64.exe)
    endif()




elseif(WIN32)
   # we're not building it on windows
endif()






