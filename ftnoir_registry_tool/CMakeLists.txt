cmake_minimum_required(VERSION 3.10)

project(ftnoir_tool)

set(prognamname FTnoirRegistryTool)

if((LINUX OR APPLE) AND MINGW_GPP_COMPILER AND MINGW_GPP_COMPILER64)

    file(GLOB tool-deps-src "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
    file(GLOB tool-deps-hdrs "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

    add_custom_command(
        OUTPUT ${prognamname}.exe
            DEPENDS ${tool-deps-src} ${tool-deps-hdrs}
        COMMAND
            i686-w64-mingw32-g++ -static -municode -mconsole  ${tool-deps-src} -o ${prognamname}
    )

    add_custom_command(
        OUTPUT ${prognamname}64.exe
            DEPENDS ${tool-deps-src} ${tool-deps-hdrs}
        COMMAND
            x86_64-w64-mingw32-g++ -static -municode -mconsole  ${tool-deps-src} -o ${prognamname}64
    )

    add_custom_target(console-tools ALL DEPENDS ${prognamname}.exe ${prognamname}64.exe)




    # install into libexec so it could be used int the origina proto-wine
    # the *.dlls are already there
    # install(FILES
    #     "${CMAKE_CURRENT_BINARY_DIR}/${prognamname}.exe"
    #     "${CMAKE_CURRENT_BINARY_DIR}/${prognamname}64.exe"
    #     DESTINATION "${opentrack-libexec}"
    # )

    # Also install as separate tool and copy the native wine dlls as well since the executables directory is the default location
    # that gets written into the registry
    add_dependencies(console-tools freetrackclient.dlls npclient.dlls)
    install(FILES
        "${CMAKE_SOURCE_DIR}/bin/freetrackclient.dll"
        "${CMAKE_SOURCE_DIR}/bin/freetrackclient64.dll"
        "${CMAKE_SOURCE_DIR}/bin/npclient.dll"
        "${CMAKE_SOURCE_DIR}/bin/npclient64.dll"
        "${CMAKE_CURRENT_BINARY_DIR}/${prognamname}.exe"
        "${CMAKE_CURRENT_BINARY_DIR}/${prognamname}64.exe"
        DESTINATION "${opentrack-install-ftnoir-registry-tool}"
    )
elseif(WIN32)
    # We don't do/need this on windows
endif()
