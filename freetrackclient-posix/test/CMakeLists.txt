cmake_minimum_required(VERSION 3.10)

if(UNIX)

    if(APPLE)
        # we can build this as universal binary
        set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
    endif()

    function(add_test_exe cflags lflags arch)
        set(targetname "ftclienttest${arch}")
        add_executable("${targetname}_dep" test.c)
        set_target_properties("${targetname}_dep" PROPERTIES OUTPUT_NAME test_dep${arch})
        target_compile_definitions("${targetname}_dep" PRIVATE LINK_DEPENDENT)
        target_compile_options("${targetname}_dep" PRIVATE ${cflags})
        target_link_options("${targetname}_dep" PRIVATE ${lflags})
        if(APPLE)
            set_target_properties("${targetname}_dep" PROPERTIES INSTALL_RPATH "@executable_path/${ftx_clientlibs_install_dir}")
        else()
            set_target_properties("${targetname}_dep" PROPERTIES INSTALL_RPATH "$ORIGIN/${ftx_clientlibs_install_dir}")
        endif()
        target_link_libraries("${targetname}_dep" PUBLIC libfreetrackclient${arch})

        add_executable("${targetname}" test.c)
        target_include_directories("${targetname}" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../include" )
        set_target_properties("${targetname}" PROPERTIES OUTPUT_NAME test${arch})
        target_compile_options("${targetname}" PRIVATE ${cflags})
        target_link_options("${targetname}" PRIVATE ${lflags})
        if(NOT APPLE)
            target_link_libraries("${targetname}" PRIVATE dl)
        endif()

        # install(TARGETS
        #     "${targetname}_dep" "${targetname}"
        #     RUNTIME DESTINATION "SDK"
        #     LIBRARY DESTINATION "SDK"
        # )

        install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/test.c"
            DESTINATION "SDK"
        )

    endfunction()

    if(APPLE)
        add_test_exe("" "" "")
    else()
        add_test_exe("-m64" "-m64" "64")
        add_test_exe("-m32" "-m32" "")
    endif()
endif()






