if((LINUX OR APPLE) AND MINGW_GCC_COMPILER64 AND MINGW_GCC_COMPILER)
    function(mk_test name include source suffix)
        set(fname "${name}${suffix}.exe")
        add_custom_command(
            OUTPUT "${fname}"
            DEPENDS "${source}"
            COMMAND
                ${MINGW_GCC_COMPILER${suffix}}
                    -I"${include}"
                    -Wl,--enable-stdcall-fixup
                    "${source}" -o "${fname}"
        )

        # install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${fname}"
        #     DESTINATION "${opentrack-install-winebridge-bin}")

    endfunction()

    mk_test(ftclient_test "${CMAKE_SOURCE_DIR}/freetrackclient" "${CMAKE_CURRENT_SOURCE_DIR}/ftclient_test.c" "")
    mk_test(ftclient_test "${CMAKE_SOURCE_DIR}/freetrackclient" "${CMAKE_CURRENT_SOURCE_DIR}/ftclient_test.c" "64")

    add_custom_target(ftclient-tests ALL  DEPENDS ftclient_test.exe ftclient_test64.exe)
    add_dependencies(ftclient-tests wine_freetrackclient.dlls)

    mk_test(npclient_test "${CMAKE_SOURCE_DIR}/contrib/npclient" "${CMAKE_CURRENT_SOURCE_DIR}/npclient_test.c" "")
    mk_test(npclient_test "${CMAKE_SOURCE_DIR}/contrib/npclient" "${CMAKE_CURRENT_SOURCE_DIR}/npclient_test.c" "64")

    add_custom_target(npclient-tests ALL  DEPENDS npclient_test.exe npclient_test64.exe)
    add_dependencies(npclient-tests wine_npclient.dlls)

endif()
