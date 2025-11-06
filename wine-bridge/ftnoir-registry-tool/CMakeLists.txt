cmake_minimum_required(VERSION 3.10)

project(ftnoir_tool)

set(prognamname ftnoir-registry-tool)

if((LINUX OR APPLE) AND MINGW_CXX_COMPILER AND MINGW_CXX_COMPILER64)

    file(GLOB tool-deps-src "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
    file(GLOB tool-deps-hdrs "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

    add_custom_command(
        OUTPUT ${prognamname}.exe
            DEPENDS ${tool-deps-src} ${tool-deps-hdrs}
        COMMAND
           ${MINGW_CXX_COMPILER} -static -municode -mconsole  ${tool-deps-src} -o ${prognamname}
    )

    add_custom_command(
        OUTPUT ${prognamname}64.exe
            DEPENDS ${tool-deps-src} ${tool-deps-hdrs}
        COMMAND
           ${MINGW_CXX_COMPILER64} -static -municode -mconsole  ${tool-deps-src} -o ${prognamname}64
    )

    add_custom_target(console-tools ALL DEPENDS ${prognamname}.exe ${prognamname}64.exe)

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${prognamname}.exe"
        "${CMAKE_CURRENT_BINARY_DIR}/${prognamname}64.exe"
        DESTINATION "${opentrack-install-winebridge-bin}"
    )

    add_subdirectory(frontend)

elseif(WIN32)
    # We don't do/need this on windows
endif()
