if((LINUX OR APPLE) AND (EXISTS "${SDK_WINE_PATH}" AND IS_DIRECTORY "${SDK_WINE_PATH}"))


    if(APPLE)
        set(CMAKE_OSX_ARCHITECTURES "x86_64")
    endif()

    # function(mk_wine_unix_lib name sources includedir cflags lflags suffix)
    function(mk_wine_unix_lib)
        cmake_parse_arguments(arg "" "NAME;SUFFIX;OUTPUT" "SOURCES;INCLUDEDIR;CFLAGS;LFLAGS" ${ARGN})

        set(targetname "${arg_NAME}${arg_SUFFIX}")

        add_library("${targetname}" SHARED
            "${arg_SOURCES}"
        )


        set_target_properties("${targetname}" PROPERTIES
            LIBRARY_OUTPUT_NAME "${arg_NAME}${arg_SUFFIX}"
            LIBRARY_OUTPUT_DIRECTORY ${arg_OUTPUT}
            PREFIX "" SUFFIX ".so")

        target_compile_options("${targetname}" PRIVATE -DOTR_REQUIRED -fvisibility=hidden ${arg_CFLAGS})
        target_compile_definitions("${targetname}" PRIVATE WINE_UNIX_LIB)
        target_include_directories("${targetname}" PRIVATE
            "${arg_INCLUDEDIR}"
            "${SDK_WINE_PATH}/include"
        )

        if(OTR_WINE_BRIDGE_DEMO_PERIOD)
            target_compile_definitions("${targetname}" PRIVATE OTR_WINE_BRIDGE_DEMO_PERIOD)
        endif()

        # if(APPLE)
        #     set_target_properties("${targetname}" PROPERTIES INSTALL_RPATH "${opentrack-runtime-opentrackclient-lib-rpath};@loader_path")
        # else()
        #     set_target_properties("${targetname}" PROPERTIES INSTALL_RPATH "${opentrack-runtime-opentrackclient-lib-rpath};$ORIGIN")
        # endif()
        # target_link_libraries("${targetname}" PRIVATE "libopentrackclient${suffix}")

        target_link_options("${targetname}" PRIVATE ${arg_LFLAGS})

        # WTF just link it statically:
        set_target_properties("${targetname}" PROPERTIES INSTALL_RPATH "")
        add_dependencies("${targetname}" "libopentrackclient_static${arg_SUFFIX}")
        target_link_libraries("${targetname}" PRIVATE "libopentrackclient_static${arg_SUFFIX}")
        if(APPLE)
            target_link_options("${targetname}" PRIVATE -framework CoreFoundation) #OTR_Priv_NotifyUser
        endif()

        if(APPLE)
            # on APPLE there's no 32bit we just copy the .so to 64.so - symlinking is a hasse in cmake
            set(libcopy64 "$<TARGET_FILE_DIR:${targetname}>/${arg_NAME}64.so")
            add_custom_target("${arg_NAME}_cp_so_to_64so" ALL
                COMMAND cp "$<TARGET_FILE:${targetname}>" "${libcopy64}"
                DEPENDS ${arg_NAME}
            )
            set_property( TARGET "${arg_NAME}_cp_so_to_64so" APPEND PROPERTY ADDITIONAL_CLEAN_FILES
                "${libcopy64}"
            )
            install(FILES
                "$<TARGET_FILE:${targetname}>"
                "${libcopy64}"
                DESTINATION "${opentrack-install-wine-unixlib}/${arg_OUTPUT}"
            )
        else()
            install(TARGETS
                "${targetname}"
                DESTINATION "${opentrack-install-wine-unixlib}/${arg_OUTPUT}"
            )
        endif()


    endfunction()

    if(APPLE)
        # freetrack and np client are 64 bit only on macOS
        mk_wine_unix_lib(NAME freetrackclient SOURCES "freetrackclient_wine_unixlib.c" INCLUDEDIR "${CMAKE_SOURCE_DIR}/freetrackclient" OUTPUT "x86_64-unix")
        mk_wine_unix_lib(NAME npclient SOURCES "npclient_wine_unixlib.c" INCLUDEDIR "${CMAKE_SOURCE_DIR}/freetrackclient" OUTPUT "x86_64-unix")
    else()
        # For Linux we build 32 and 64 bit libs:
        # 64 bit freetrackclient
        mk_wine_unix_lib(NAME freetrackclient SOURCES "freetrackclient_wine_unixlib.c" INCLUDEDIR "${CMAKE_SOURCE_DIR}/freetrackclient" CFLAGS "-m32" LFLAGS "-m32" OUTPUT "i386-unix")
        mk_wine_unix_lib(NAME freetrackclient SOURCES "freetrackclient_wine_unixlib.c" INCLUDEDIR "${CMAKE_SOURCE_DIR}/freetrackclient" CFLAGS "-m64" LFLAGS "-m64" SUFFIX "64" OUTPUT "x86_64-unix")
        # 32 bit npclient
        mk_wine_unix_lib(NAME npclient SOURCES "npclient_wine_unixlib.c" INCLUDEDIR "${CMAKE_SOURCE_DIR}/freetrackclient" CFLAGS "-m32" LFLAGS "-m32" OUTPUT "i386-unix")
        mk_wine_unix_lib(NAME npclient SOURCES "npclient_wine_unixlib.c" INCLUDEDIR "${CMAKE_SOURCE_DIR}/freetrackclient" CFLAGS "-m64" LFLAGS "-m64" SUFFIX "64" OUTPUT "x86_64-unix")
    endif()



endif()






