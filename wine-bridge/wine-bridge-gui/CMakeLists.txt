cmake_minimum_required(VERSION 3.10)


if((LINUX OR APPLE) AND MINGW_CXX_COMPILER AND MINGW_CXX_COMPILER64 AND MINGW_WINDRES64 AND MINGW_WINDRES)
    project(opentrack-wine-bridge-frontend VERSION 0.9.2)
    configure_file(defs.in.h defs.h @ONLY)

    set(toolname "wine-bridge-tool")

    set(sources
        "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/log.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/bridge.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/app.cpp"
    )

    set(otherdeps  "${CMAKE_CURRENT_SOURCE_DIR}/frontend.rc"
            "${CMAKE_CURRENT_SOURCE_DIR}/resource.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/bridge.h"
            "${CMAKE_CURRENT_BINARY_DIR}/defs.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/app.h"
    )

    function(mk_tool suffix)
        set(registry-tool-lib "${CMAKE_CURRENT_BINARY_DIR}/../ftnoir-registry-tool/libftnoir-registry-tool${suffix}.a")
        add_custom_command(
            OUTPUT  "${toolname}${suffix}.exe"
            DEPENDS
                ${sources}
                ${otherdeps}
            COMMAND ${MINGW_WINDRES${suffix}} -I"${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/frontend.rc" -o frontend.rc${suffix}.o
            COMMAND ${MINGW_CXX_COMPILER${suffix}} -O -s -fno-permissive -mwindows  -I"${CMAKE_SOURCE_DIR}" -I"${CMAKE_CURRENT_SOURCE_DIR}" -I"${CMAKE_CURRENT_BINARY_DIR}" -DVERSION="\\"${PROJECT_VERSION}\\"" ${sources}  "${registry-tool-lib}"   frontend.rc${suffix}.o -o ${toolname}${suffix}.exe  -static-libstdc++ -static-libgcc -Wl,-Bstatic,--whole-archive -lpthread -Wl,-Bdynamic,--no-whole-archive
            COMMAND rm frontend.rc${suffix}.o
        )
    endfunction()

    mk_tool("")
    mk_tool("64")

    add_custom_target("wine-bridge-tool-target" ALL DEPENDS
        "${toolname}.exe"
        "${toolname}64.exe"
    )

    add_dependencies("wine-bridge-tool-target" "ftnoir-registry-tool-target")

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${toolname}.exe"
        "${CMAKE_CURRENT_BINARY_DIR}/${toolname}64.exe"
        DESTINATION "${opentrack-install-winebridge-bin}"
    )

endif()
