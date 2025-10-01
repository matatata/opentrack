cmake_minimum_required(VERSION 3.10)

if(LINUX OR APPLE)
    project(libfreetrackclient VERSION 0.9.0
        DESCRIPTION "This is a client library for the Opentrack protocol \"Opentrack Shared Memory\""
        LANGUAGES C
    )

    # we're not a part of opentrack at runtime                                                                                                     so don't inherit its rpath settings:
    set(CMAKE_INSTALL_RPATH "")

    set(ftx_clientlibs_install_dir "")

    add_library(libfreetrackclient_headers INTERFACE)
    target_include_directories(libfreetrackclient_headers
        INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include;${CMAKE_CURRENT_SOURCE_DIR}"
    )


    function(add_lib name libtype cflags lflags arch)
        set(targetname "${name}${arch}")
        add_library("${targetname}" ${libtype}
            freetrackclient_posix.c
            shm.c
        )
        target_compile_definitions("${targetname}" PRIVATE
            -DLIBFREETRACKCLIENT_VERSION="${PROJECT_VERSION}"
        )

        set_target_properties("${targetname}" PROPERTIES OUTPUT_NAME "freetrackclient${arch}")


        set_target_properties("${targetname}" PROPERTIES PUBLIC_HEADER "include/freetrackclient_posix.h")

        target_compile_options("${targetname}" PRIVATE ${cflags})
        target_link_options("${targetname}" PRIVATE ${lflags})
        if(NOT APPLE)
            target_link_libraries("${targetname}" PRIVATE rt)
        endif()

        target_include_directories("${targetname}"
                PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include;${CMAKE_CURRENT_SOURCE_DIR}"
                PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include;${CMAKE_CURRENT_SOURCE_DIR}"
        )

        install(TARGETS
            "${targetname}"
            ARCHIVE DESTINATION "SDK/${ftx_clientlibs_install_dir}"
            LIBRARY DESTINATION "SDK/${ftx_clientlibs_install_dir}"
            PUBLIC_HEADER DESTINATION "SDK/include"
        )


    endfunction()

    if(APPLE)
        # we can build this as universal binary
        set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
        # static lib
        add_lib(libfreetrackclient_static STATIC "" "" "")
        #shared lib
        add_lib(libfreetrackclient SHARED "" "" "")
    else()
        # static lib
        add_lib(libfreetrackclient_static STATIC "-m32" "-m32" "")
        add_lib(libfreetrackclient_static STATIC "-m64" "-m64" "64")
        #shared lib
        add_lib("libfreetrackclient" SHARED "-m32" "-m32" "")
        add_lib("libfreetrackclient" SHARED "-m64" "-m64" "64")
    endif()

    add_subdirectory("test")


endif()






