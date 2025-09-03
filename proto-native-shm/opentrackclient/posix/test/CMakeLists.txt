cmake_minimum_required(VERSION 3.10)

if(LINUX OR APPLE)

    if(APPLE)
        # we can build this as universal binary
        set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
    endif()

    function(add_test_exe cflags lflags suffix)
        set(targetname "opentrackclient${suffix}")
        add_executable(${targetname}.test_dep test.c)
        set_target_properties(${targetname}.test_dep PROPERTIES OUTPUT_NAME test_dep${suffix})
        target_compile_definitions(${targetname}.test_dep PRIVATE LINK_DEPENDENT)
        target_compile_options(${targetname}.test_dep PRIVATE ${cflags})
        target_link_options(${targetname}.test_dep PRIVATE ${lflags} ) #-Wl,-rpath,lib
        set_target_properties(${targetname}.test_dep PROPERTIES INSTALL_RPATH "${otr_clientlibs_install_dir}")
        target_link_libraries(${targetname}.test_dep opentrackclient${suffix})



        add_executable(${targetname}.test test.c)
        target_include_directories(${targetname}.test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/..")
        set_target_properties(${targetname}.test PROPERTIES OUTPUT_NAME test${suffix})
        target_compile_options(${targetname}.test PRIVATE ${cflags})
        target_link_options(${targetname}.test PRIVATE ${lflags})

        install(TARGETS
            ${targetname}.test_dep ${targetname}.test
            RUNTIME DESTINATION "SDK"
            LIBRARY DESTINATION "SDK"
            PUBLIC_HEADER DESTINATION "SDK"
        )

    endfunction()

    if(APPLE)
        add_test_exe("" "" "")
    elseif(LINUX)
        add_test_exe("-m64" "-m64" "64")
        add_test_exe("-m32" "-m32" "")
    endif()
endif()






