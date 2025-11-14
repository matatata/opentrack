cmake_minimum_required(VERSION 3.10)

project(ftnoir_tool)

set(programname ftnoir-registry-tool)

if((LINUX OR APPLE) AND MINGW_CXX_COMPILER AND MINGW_CXX_COMPILER64)

    set(tool-lib-src
        "${CMAKE_CURRENT_SOURCE_DIR}/registry.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/tool.cpp"
    )
    file(GLOB tool-lib-hdrs "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

    function(mk_reg_lib suffix)
        add_custom_command(
            OUTPUT registry${suffix}.o tool${suffix}.o
                DEPENDS ${tool-lib-src} ${tool-lib-hdrs} "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
            COMMAND
                ${MINGW_CXX_COMPILER${suffix}}
                    -c "${CMAKE_CURRENT_SOURCE_DIR}/registry.cpp" -o registry${suffix}.o
            COMMAND
                ${MINGW_CXX_COMPILER${suffix}}
                    -c "${CMAKE_CURRENT_SOURCE_DIR}/tool.cpp" -o tool${suffix}.o
        )

        add_custom_command(
            OUTPUT lib${programname}${suffix}.a
                DEPENDS registry${suffix}.o tool${suffix}.o
            COMMAND
                ${MINGW_CXX_AR${suffix}} rcs -o lib${programname}${suffix}.a registry${suffix}.o tool${suffix}.o
        )

    endfunction()

    function(mk_reg_tool suffix)
        add_custom_command(
            OUTPUT "${programname}${suffix}.exe"
                DEPENDS lib${programname}${suffix}.a "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
            COMMAND
                ${MINGW_CXX_COMPILER${suffix}} -O -s -mconsole "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp" -o ${programname}${suffix} -static-libstdc++ -static-libgcc -Wl,-Bstatic,--whole-archive -lpthread -Wl,-Bdynamic,--no-whole-archive
                    -L. -l${programname}${suffix}
        )
    endfunction()

    add_custom_target(ftnoir-registry-tool-target ALL DEPENDS
         ${programname}.exe
         ${programname}64.exe
         lib${programname}.a
         lib${programname}64.a
         )

    mk_reg_lib("")
    mk_reg_lib("64")
    mk_reg_tool("")
    mk_reg_tool("64")

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${programname}.exe"
        "${CMAKE_CURRENT_BINARY_DIR}/${programname}64.exe"
        DESTINATION "${opentrack-install-winebridge-bin}"
    )

elseif(WIN32)
    # We don't do/need this on windows
endif()
