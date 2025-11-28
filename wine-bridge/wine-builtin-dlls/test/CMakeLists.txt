if((LINUX OR APPLE) AND MINGW_GCC_COMPILER64 AND MINGW_GCC_COMPILER)
    function(mk_test)
        cmake_parse_arguments(arg "NOINSTALL" "NAME;SUFFIX;INCLUDE;SOURCE" "" ${ARGN})

        set(fname "${arg_NAME}${arg_SUFFIX}.exe")
        add_custom_command(
            OUTPUT "${fname}"
            DEPENDS "${arg_SOURCE}"
            COMMAND
                ${MINGW_GCC_COMPILER${arg_SUFFIX}}
                    -I"${arg_INCLUDE}"
                    -Wl,--enable-stdcall-fixup
                    "${arg_SOURCE}" -o "${fname}"
        )
        if(NOT NOINSTALL)
            install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${fname}"
                DESTINATION "${opentrack-install-winebridge-bin}")
        endif()
    endfunction()

    mk_test(NAME ftclient_test INCLUDE "${CMAKE_SOURCE_DIR}" SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/ftclient_test.c")
    mk_test(NAME ftclient_test INCLUDE "${CMAKE_SOURCE_DIR}" SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/ftclient_test.c" SUFFIX "64")

    add_custom_target(ftclient-tests ALL  DEPENDS ftclient_test.exe ftclient_test64.exe)
    add_dependencies(ftclient-tests wine_freetrackclient.dlls)

    mk_test(NAME npclient_test INCLUDE "${CMAKE_SOURCE_DIR}/contrib/npclient" SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/npclient_test.c")
    mk_test(NAME npclient_test INCLUDE "${CMAKE_SOURCE_DIR}/contrib/npclient" SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/npclient_test.c" SUFFIX "64")

    add_custom_target(npclient-tests ALL  DEPENDS npclient_test.exe npclient_test64.exe)
    add_dependencies(npclient-tests wine_npclient.dlls)

    mk_test(NAME otrclient_test INCLUDE "${CMAKE_SOURCE_DIR}" SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/otrclient_test.c")
    mk_test(NAME otrclient_test INCLUDE "${CMAKE_SOURCE_DIR}" SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/otrclient_test.c" SUFFIX "64")

    add_custom_target(otrclient-tests ALL  DEPENDS otrclient_test.exe otrclient_test64.exe)
    add_dependencies(otrclient-tests wine_otrclient.dlls)

endif()
