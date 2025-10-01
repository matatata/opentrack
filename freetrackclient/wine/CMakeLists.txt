cmake_minimum_required(VERSION 3.10)
project(wine-freetrackclient DESCRIPTION "Produces the wine builtin unixlib freetrackclient.dll and its accompanying freetrackclient.so")

if(NOT DEFINED common_dll_cflags)
    message(FATAL_ERROR "missing the common_dll_cflags variable it's supposed to be defined in the parent")
endif()

if((LINUX OR APPLE) AND (EXISTS "${SDK_WINE_PATH}" AND IS_DIRECTORY "${SDK_WINE_PATH}"))

    if(APPLE)
        set(CMAKE_OSX_ARCHITECTURES "x86_64")
    endif()

    function(mk_ftlib name cflags lflags suffix)
        set(targetname "${name}${suffix}")

        add_library("${targetname}" SHARED
            freetrackclient_wine_unixlib.c
        )

        target_compile_options("${targetname}" PRIVATE -fvisibility=hidden ${cflags})
        target_link_options("${targetname}" PRIVATE ${lflags})

        target_link_libraries("${targetname}" PRIVATE "libfreetrackclient${arch}")
        if(APPLE)
            set_target_properties("${targetname}" PROPERTIES INSTALL_RPATH "@loader_path;/usr/local/lib")
        else()
            set_target_properties("${targetname}" PROPERTIES INSTALL_RPATH "$ORIGIN:/usr/local/lib")
        endif()

        target_compile_definitions("${targetname}" PRIVATE WINE_UNIX_LIB)
        target_include_directories("${targetname}" PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/.."
            "${SDK_WINE_PATH}/include"
        )

        set_target_properties("${targetname}" PROPERTIES
            LIBRARY_OUTPUT_NAME "freetrackclient${arch}"
            PREFIX "" SUFFIX ".so")
    endfunction()

    if(APPLE)
        mk_ftlib(freetrackclient "" "" "")
    else()
        mk_ftlib(freetrackclient "-m32" "-m32" "")
        mk_ftlib(freetrackclient "-m64" "-m64" "64")
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

    if(APPLE)
        install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/freetrackclient.so"
            "${CMAKE_CURRENT_BINARY_DIR}/freetrackclient64.so"
            "$<TARGET_FILE:libfreetrackclient>"
            DESTINATION "${opentrack-install-wine-unixlib}/x86_64-unix"
        )
    else()
        install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/freetrackclient64.so"
            "$<TARGET_FILE:libfreetrackclient64>"
            DESTINATION "${opentrack-install-wine-unixlib}/x86_64-unix"
        )
        install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/freetrackclient.so"
            "$<TARGET_FILE:libfreetrackclient>"
            DESTINATION "${opentrack-install-wine-unixlib}/i386-unix"
        )
    endif()


    if(MINGW_GCC_COMPILER64 AND MINGW_GCC_COMPILER)

        # Compile the wine builtin DLLS:
        function(mk_dll mingw_target arch)
            set(dllname "freetrackclient${arch}.dll")
            add_custom_command(
                OUTPUT ${dllname}
                DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/../freetrackclient.c"

                COMMAND winegcc -b ${mingw_target}
                        -I"${CMAKE_CURRENT_SOURCE_DIR}/.."
                        -I"${CMAKE_CURRENT_SOURCE_DIR}"
                        -I"${SDK_WINE_PATH}/include"
                        -DFT_WINE_UNIX_LIB
                        ${common_dll_cflags}
                        -Wl,--wine-builtin
                        -shared
                        "${CMAKE_CURRENT_SOURCE_DIR}/../freetrackclient.c"
                        -o ${dllname}
                COMMAND
                    "${mingw_target}-strip" --strip-all --remove-section=.eh_frame ${dllname}
            )
        endfunction()

        mk_dll(x86_64-w64-mingw32 "64")
        mk_dll(i686-w64-mingw32 "")

        add_custom_target(wine_freetrackclient.dlls ALL DEPENDS freetrackclient64.dll freetrackclient.dll)


        install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/freetrackclient64.dll"
            DESTINATION "${opentrack-install-wine-unixlib}/x86_64-windows"
        )

        install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/freetrackclient.dll"
            DESTINATION "${opentrack-install-wine-unixlib}/i386-windows"
        )
    endif()

endif()






