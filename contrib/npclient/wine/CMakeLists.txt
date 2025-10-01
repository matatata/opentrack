cmake_minimum_required(VERSION 3.10)

project(wine-npclient DESCRIPTION "Produces the wine builtin unixlib npclient.dll and its accompanying npclient.so")

if(NOT DEFINED common_dll_cflags)
    message(FATAL_ERROR "missing variables that should have been defined the parent")
endif()

if((LINUX OR APPLE) AND (EXISTS "${SDK_WINE_PATH}" AND IS_DIRECTORY "${SDK_WINE_PATH}"))

    function(mk_nplib name cflags lflags suffix)
        add_library(${name} SHARED
            npclient_wine_unixlib.c
        )

        target_compile_options(${name} PUBLIC -fvisibility=hidden ${cflags})
        target_link_options(${name} PUBLIC ${lflags})


        target_link_libraries(${name} PRIVATE "libfreetrackclient${sufffix}")
        if(APPLE)
            set_target_properties(${name} PROPERTIES INSTALL_RPATH "@loader_path;/usr/local/lib")
        else()
            set_target_properties(${name} PROPERTIES INSTALL_RPATH "$ORIGIN:/usr/local/lib")
        endif()

        target_compile_definitions(${name} PUBLIC WINE_UNIX_LIB)
        target_include_directories(${name} PRIVATE
            "${SDK_WINE_PATH}/include"
            "${CMAKE_SOURCE_DIR}/freetrackclient"
        )
        set_target_properties(${name} PROPERTIES
            LIBRARY_OUTPUT_NAME "npclient${suffix}"
            PREFIX "" SUFFIX ".so")
    endfunction()

    if(APPLE)
        mk_nplib(npclient "" "" "")
    else()
        mk_nplib(npclient "-m32" "-m32" "")
        mk_nplib(npclient64 "-m64" "-m64" "64")
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


    if(APPLE)
        install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/npclient.so"
            "${CMAKE_CURRENT_BINARY_DIR}/npclient64.so"
            "$<TARGET_FILE:libfreetrackclient>"
            DESTINATION "${opentrack-install-wine-unixlib}/x86_64-unix"
        )
    else()
        install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/npclient.so"
            "$<TARGET_FILE:libfreetrackclient>"
            DESTINATION "${opentrack-install-wine-unixlib}/i386-unix"
        )
        install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/npclient64.so"
            "$<TARGET_FILE:libfreetrackclient64>"
            DESTINATION "${opentrack-install-wine-unixlib}/x86_64-unix"
        )
    endif()


    if(MINGW_GCC_COMPILER64 AND MINGW_GCC_COMPILER)
        # Compile the wine builtin DLLS:
        function(mk_dll mingw_target suffix)
            set(dllname "npclient${suffix}.dll")
            add_custom_command(
                OUTPUT ${dllname}
                DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/../npclient.c"

                COMMAND winegcc -b ${mingw_target} -DNP_WINE_UNIX_LIB
                        -I"${CMAKE_CURRENT_SOURCE_DIR}"
                        -I"${CMAKE_CURRENT_SOURCE_DIR}/.."
                        -I"${CMAKE_SOURCE_DIR}/freetrackclient"
                        -I"${SDK_WINE_PATH}/include"
                        ${common_dll_cflags}
                        -Wl,--wine-builtin
                        -shared
                        "${CMAKE_CURRENT_SOURCE_DIR}/../npclient.c"
                        -o ${dllname}
                COMMAND
                    "${mingw_target}-strip" --strip-all --remove-section=.eh_frame ${dllname}
            )
        endfunction()

        mk_dll(x86_64-w64-mingw32 "64")
        mk_dll(i686-w64-mingw32 "")

        add_custom_target(wine_npclient.dlls ALL DEPENDS npclient64.dll npclient.dll)

        install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/npclient64.dll"
            DESTINATION "${opentrack-install-wine-unixlib}/x86_64-windows"
        )

        install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/npclient.dll"
            DESTINATION "${opentrack-install-wine-unixlib}/i386-windows"
        )

    endif()

endif()
