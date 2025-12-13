cmake_minimum_required(VERSION 3.10)


if(LINUX OR APPLE)
    project(libopentrackclient VERSION 1.0.0
        DESCRIPTION "This is a client library for the Opentrack protocol"
        LANGUAGES C
    )

    function(add_lib)
        cmake_parse_arguments(
            arg # prefix of output variables
            "STATIC;FRAMEWORK" # list of names of the boolean arguments (only defined ones will be true)
            "NAME;ARCH;INSTALL_NAME_DIR" # list of names of mono-valued arguments
            "CFLAGS;LFLAGS;SOURCES;HEADERS" # list of names of multi-valued arguments (output variables are lists)
            ${ARGN} # arguments of the function to parse, here we take the all original ones
        )
        if(arg_STATIC)
            set(targetname "${arg_NAME}_static${arg_ARCH}")
        else()
            set(targetname "${arg_NAME}${arg_ARCH}")
        endif()

        if(arg_FRAMEWORK)
            set(targetname "framework_${targetname}")
        else()
            set(targetname "lib${targetname}")
        endif()

        set(link-mode SHARED)
        if (arg_STATIC)
            set(link-mode STATIC)
        endif()


        message("creating target ${targetname}")

        add_library("${targetname}" ${link-mode}
            ${arg_SOURCES} ${arg_HEADERS}
        )

        set_target_properties("${targetname}" PROPERTIES
            OUTPUT_NAME "${arg_NAME}${arg_ARCH}"
            INSTALL_NAME_DIR "${arg_INSTALL_NAME_DIR}"
            INSTALL_RPATH ""
        )

        if(APPLE AND arg_FRAMEWORK)
            set_target_properties("${targetname}" PROPERTIES
                FRAMEWORK TRUE
                FRAMEWORK_VERSION A
                MACOSX_FRAMEWORK_IDENTIFIER "com.github.opentrack.${targetname}"
                # "current version" in semantic format in Mach-O binary file
                VERSION ${PROJECT_VERSION}
                # "compatibility version" in semantic format in Mach-O binary file
                SOVERSION 1.0.0
            )
        endif()


        target_compile_definitions("${targetname}" PRIVATE
            -DOPENTRACKCLIENT_VERSION="${PROJECT_VERSION}"
        )

        target_compile_options("${targetname}" PRIVATE "${arg_CFLAGS}")
        target_link_options("${targetname}" PRIVATE -fPIC "${arg_LFLAGS}")
        if(APPLE)
            target_link_options("${targetname}" PRIVATE -framework CoreFoundation) #OTR_Priv_NotifyUser
        elseif(LINUX)
            target_link_libraries("${targetname}" PRIVATE rt)
        endif()

        target_include_directories("${targetname}"
                PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
                PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}"
        )
        set_target_properties("${targetname}" PROPERTIES PUBLIC_HEADER "${arg_HEADERS}")

    endfunction()


    set(client_deps "${CMAKE_CURRENT_SOURCE_DIR}/opentrackclient.c" "${CMAKE_CURRENT_SOURCE_DIR}/shm.c")

    # we can build this as universal binary
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")

    set(CLIENT_SOURCES opentrackclient.c shm.c)
    set(CLIENT_HEADERS opentrackclient.h)
    set(CLIENTLOADER_SOURCES opentrackclient-loader.c)
    set(CLIENTLOADER_HEADERS )

    if(APPLE)
        add_lib(NAME "opentrackclient"
            SOURCES "${CLIENT_SOURCES}"
            HEADERS "${CLIENT_HEADERS}"
            CFLAGS "-DOTR_REQUIRED"
            INSTALL_NAME_DIR "${opentrack-runtime-opentrackclient-lib-rpath}"
        )

        add_lib(NAME "opentrackclient"
            STATIC
            SOURCES "${CLIENT_SOURCES}"
            HEADERS "${CLIENT_HEADERS}"
            CFLAGS "-DOTR_REQUIRED"
        )

        # also create frameworks
        add_lib(NAME "opentrackclient"
            FRAMEWORK
            SOURCES "${CLIENT_SOURCES}"
            HEADERS "${CLIENT_HEADERS}"
            CFLAGS "-DOTR_REQUIRED"
            INSTALL_NAME_DIR "${opentrack-runtime-opentrackclient-framework-rpath}"
        )

        # also create frameworks
        add_lib(NAME "opentrackclient-loader"
            STATIC FRAMEWORK
            SOURCES "${CLIENT_SOURCES}"
            HEADERS "${CLIENT_HEADERS}"
            CFLAGS "-DOTR_REQUIRED"
        )


        install(TARGETS
            "libopentrackclient"  "framework_opentrackclient" "framework_opentrackclient-loader_static"
            FRAMEWORK DESTINATION "${opentrack-install-opentrackclient-framework}"
            LIBRARY DESTINATION "${opentrack-install-opentrackclient-lib}"
            PUBLIC_HEADER DESTINATION "${opentrack-install-opentrackclient-include}"
        )
    else()
        # LINUX
        # clientlib 32 and 64 bit shared libs
        add_lib(NAME "opentrackclient"
            SOURCES "${CLIENT_SOURCES}"
            HEADERS "${CLIENT_HEADERS}"
            CFLAGS "-m64;-DOTR_REQUIRED" LFLAGS "-m64" ARCH "64"
        )
        add_lib(NAME "opentrackclient"
            SOURCES "${CLIENT_SOURCES}"
            HEADERS "${CLIENT_HEADERS}"
            CFLAGS "-m32;-DOTR_REQUIRED" LFLAGS "-m32"
        )

        # clientlib 32 and 64 bit static libs
        add_lib(NAME "opentrackclient"
            STATIC
            SOURCES "${CLIENT_SOURCES}"
            HEADERS "${CLIENT_HEADERS}"
            CFLAGS "-m64;-DOTR_REQUIRED" LFLAGS "-m64" ARCH "64"
        )
        add_lib(NAME "opentrackclient"
            STATIC
            SOURCES "${CLIENT_SOURCES}"
            HEADERS "${CLIENT_HEADERS}"
            CFLAGS "-m32;-DOTR_REQUIRED" LFLAGS "-m32"
        )

        # loader 32 and 64 bit static libs
        add_lib(NAME "opentrackclient-loader"
            STATIC
            SOURCES "${CLIENTLOADER_SOURCES}"
            HEADERS "${CLIENTLOADER_HEADERS}"
            CFLAGS "-m64;-DOTR_OPTIONAL" LFLAGS "-m64" ARCH "64"
        )
        add_lib(NAME "opentrackclient-loader"
            STATIC
            SOURCES "${CLIENTLOADER_SOURCES}"
            HEADERS "${CLIENTLOADER_HEADERS}"
            CFLAGS "-m32;-DOTR_OPTIONAL" LFLAGS "-m32"
        )

        install(TARGETS
            "libopentrackclient" "libopentrackclient64" "libopentrackclient-loader_static" "libopentrackclient-loader_static64"
            ARCHIVE DESTINATION "${opentrack-install-opentrackclient-lib}"
            LIBRARY DESTINATION "${opentrack-install-opentrackclient-lib}"
            PUBLIC_HEADER DESTINATION "${opentrack-install-opentrackclient-include}/opentrackclient"
        )

        install(FILES
            ${CLIENTLOADER_SOURCES}
            DESTINATION "${opentrack-install-opentrackclient-sdk}"
        )
    endif()



    ######### TEST

    set(otrclient-tester-deps otrclient-tester.c)

    function(add_test)
        cmake_parse_arguments(arg "" "ARCH" "CFLAGS;LFLAGS" ${ARGN})
        set(targetname "otrclient-tester${arg_ARCH}")
        add_executable("${targetname}" ${otrclient-tester-deps})

        set_target_properties("${targetname}" PROPERTIES OUTPUT_NAME "${targetname}")
        target_compile_options("${targetname}" PRIVATE "${arg_CFLAGS}")
        target_link_options("${targetname}" PRIVATE "${arg_LFLAGS}")
        if(NOT APPLE)
            target_link_libraries("${targetname}" PRIVATE dl)
        endif()

        if(APPLE)
            # set_target_properties("${targetname}" PROPERTIES INSTALL_RPATH "")
            # target_link_libraries("${targetname}" PRIVATE $<LINK_LIBRARY:WEAK_FRAMEWORK,framework_opentrackclient>)
            target_link_libraries("${targetname}" PRIVATE framework_opentrackclient)
        else()
            set_target_properties("${targetname}" PROPERTIES INSTALL_RPATH "${opentrack-runtime-opentrackclient-lib-rpath}")
            target_link_libraries("${targetname}" PRIVATE "libopentrackclient-loader_static${arg_ARCH}")
        endif()

        install(FILES
            ${otrclient-tester-deps}
            DESTINATION "${opentrack-install-opentrackclient-sdk}"
        )
        install(TARGETS ${targetname}
            RUNTIME DESTINATION "${opentrack-install-opentrackclient-sdk}")

    endfunction()

    if(APPLE)
        add_test(CFLAGS "-DOTR_OPTIONAL")
    else()
        add_test(CFLAGS "-m64;-DOTR_OPTIONAL" LFLAGS "-m64" ARCH "64")
        add_test(CFLAGS "-m32;-DOTR_OPTIONAL" LFLAGS "-m32")
    endif()


endif()






