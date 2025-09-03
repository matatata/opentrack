cmake_minimum_required(VERSION 3.10)

if(LINUX OR APPLE)
    set(OPENTRACKCLIENT_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/posix
    )
    # we're not a part of opentrack at runtime                                                                                                     so don't inherit its rpath settings:
    set(CMAKE_INSTALL_RPATH "")

    set(otr_clientlibs_install_dir "lib")

    function(add_lib name libtype cflags lflags suffix)
        add_library(${name} ${libtype}
            posix/opentrackclient_posix.c
        )

        set_target_properties(${name} PROPERTIES OUTPUT_NAME "opentrackclient${suffix}")
        set_target_properties(${name} PROPERTIES PUBLIC_HEADER posix/opentrackclient_public.h)
        set_target_properties(${name} PROPERTIES PRIVATE_HEADER posix/opentrackclient_private.h)
        target_compile_options(${name} PRIVATE ${cflags})
        target_link_options(${name} PRIVATE ${lflags})

        target_include_directories(${name}
            PUBLIC ${OPENTRACKCLIENT_INCLUDE_DIRS}
        )

        install(TARGETS
            ${name}
            ARCHIVE DESTINATION "SDK/${otr_clientlibs_install_dir}"
            LIBRARY DESTINATION "SDK/${otr_clientlibs_install_dir}"
            PUBLIC_HEADER DESTINATION "SDK/include"
            PRIVATE_HEADER DESTINATION "SDK/include"
        )


    endfunction()

    if(APPLE)
        # we can build this as universal binary
        set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
        # static lib
        add_lib(opentrackclient_static STATIC "" "" "")
        #shared lib
        add_lib(opentrackclient SHARED "" "" "")
    else()
        # static lib
        add_lib(opentrackclient_static STATIC "-m32" "-m32" "")
        add_lib(opentrackclient_static64 STATIC "-m64" "-m64" "64")
        #shared lib
        add_lib(opentrackclient SHARED "-m32" "-m32" "")
        add_lib(opentrackclient64 SHARED "-m64" "-m64" "64")
    endif()






    add_subdirectory("posix/test")


endif()






