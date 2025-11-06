cmake_minimum_required(VERSION 3.10)


if((LINUX OR APPLE) AND MINGW_CXX_COMPILER AND MINGW_CXX_COMPILER64 AND MINGW_WINDRES64 AND MINGW_WINDRES)
    configure_file(main.in.cpp main.cpp @ONLY)

    set(toolname "ftnoir-registry-tool-gui")

    add_custom_target(
        "ftnoir-registry-tool-gui" ALL

        DEPENDS
            "${CMAKE_CURRENT_BINARY_DIR}/main.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/frontend.rc"
            "${CMAKE_CURRENT_SOURCE_DIR}/resource.h"
        COMMAND ${MINGW_WINDRES64}  -I"${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/frontend.rc" -o frontend.rc.o
        COMMAND ${MINGW_CXX_COMPILER64} -static -fno-permissive -mwindows  -I"${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/main.cpp" frontend.rc.o -I "${CMAKE_CURRENT_SOURCE_DIR}"  -o ${toolname}64.exe

        COMMAND ${MINGW_WINDRES} -I"${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/frontend.rc" -o frontend.rc.o
        COMMAND ${MINGW_CXX_COMPILER} -static -fno-permissive -mwindows  -I"${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/main.cpp" frontend.rc.o -I "${CMAKE_CURRENT_SOURCE_DIR}"  -o ${toolname}.exe
    )

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${toolname}.exe" "${CMAKE_CURRENT_BINARY_DIR}/${toolname}64.exe"
        DESTINATION "${opentrack-install-winebridge-bin}"
    )

endif()
