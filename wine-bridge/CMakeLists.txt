if(LINUX OR APPLE)
    set(OTR_WINE_BRIDGE_DEMO_PERIOD "30" CACHE STRING "Demo runtime period of wine-bridge in minutes. Set to a positive number to enable it or leave empty for the full version.")

    if(OTR_WINE_BRIDGE_DEMO_PERIOD GREATER 0)
        message(WARNING "wine-bridge demo mode activated. Is this what you want?")
    endif()

    execute_process(COMMAND winepath -0 -w "/${opentrack-install-winebridge}"
                    OUTPUT_VARIABLE opentrack-install-winebridge-winepath)
    string(REPLACE "\\" "\\\\" opentrack-install-winebridge-winepath-quoted "${opentrack-install-winebridge-winepath}")
    execute_process(COMMAND winepath -0 -w "/${opentrack-install-winebridge-bin}"
                    OUTPUT_VARIABLE opentrack-install-winebridge-bin-winepath)
    string(REPLACE "\\" "\\\\" opentrack-install-winebridge-bin-winepath-quoted "${opentrack-install-winebridge-bin-winepath}")


    add_subdirectory(wine-builtin-dlls)
    add_subdirectory(wine-unix-lib)
    add_subdirectory(fakedlls)
    add_subdirectory(ftnoir-registry-tool)
    add_subdirectory(wine-bridge-gui)
    # could remove this:
    add_subdirectory(bridge)



    otr_install_md_as_html("${CMAKE_CURRENT_SOURCE_DIR}/README.md" "${opentrack-install-winebridge}")

    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/otrwine.sh"
                   DESTINATION "${opentrack-install-winebridge-bin}" PERMISSIONS ${opentrack-perms-exec})

    install(FILES
        "${CMAKE_SOURCE_DIR}/bin/freetrackclient.dll"
        "${CMAKE_SOURCE_DIR}/bin/freetrackclient64.dll"
        "${CMAKE_SOURCE_DIR}/bin/NPClient.dll"
        "${CMAKE_SOURCE_DIR}/bin/NPClient64.dll"
        DESTINATION  "${opentrack-install-winebridge-bin}"
    )

    # add_subdirectory(setup)
    add_subdirectory(macosx)

endif()






