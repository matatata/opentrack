set(SDK_OSCPACK "" CACHE PATH "oscpack build directory")
if(SDK_OSCPACK)
    if(WIN32)
        if(NOT EXISTS "${SDK_OSCPACK}/include/.")
            message(FATAL_ERROR "SDK_OSCPACK should have 'include' subdirectory (or symlink) to src dir")
        endif()
        link_directories("${SDK_OSCPACK}")
        include_directories("${SDK_OSCPACK}/include" "${SDK_OSCPACK}/include/oscpack")
    elseif(APPLE)
         # When built with "make lib" ... make sure to use matatata's fork of ocapack
          link_directories("${SDK_OSCPACK}/lib")
          include_directories("${SDK_OSCPACK}/include")
    else()
        link_directories("${SDK_OSCPACK}/lib" "${SDK_OSCPACK}/lib32")
        include_directories("${SDK_OSCPACK}/include/oscpack")
    endif()


    if(APPLE)
        # I don't know how to make the .so file end up in the .app bundle, So I link statically:
        find_library(oscpack liboscpack.a)
        link_libraries(liboscpack.a)
    else()
        link_libraries(oscpack)
    endif()

    otr_module(proto-osc)
endif()
