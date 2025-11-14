if(LINUX OR APPLE)
    set(SDK_XPLANE "" CACHE PATH "Path to the X-Plane SDK")



    if(SDK_XPLANE AND (APPLE OR LINUX))
        if (APPLE)
            # we can build this as universal binary:
            set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
        endif()

        otr_module(xplane-plugin NO-QT NO-INSTALL)

        target_include_directories(${self} SYSTEM PUBLIC ${SDK_XPLANE}/CHeaders/XPLM)


        set_target_properties(${self} PROPERTIES INSTALL_RPATH "")


        if(APPLE)
            target_compile_options(${self} PRIVATE
                                   -iframework "${SDK_XPLANE}/Libraries/Mac/"
                                   -DAPL -DXPLM200 -DXPLM210 -DXPLM300 -DXPLM303
                                   -framework XPLM)
            target_link_options(${self} PRIVATE
                                "-F${SDK_XPLANE}/Libraries/Mac/"
                                -framework XPLM)

        elseif(CMAKE_COMPILER_IS_GNUCXX)
            target_compile_options(${self} PRIVATE -DLIN -DXPLM200 -DXPLM210 -DXPLM300 -DXPLM303 )
            target_link_options(${self} PRIVATE -rdynamic -nodefaultlibs)
        endif()


        if(APPLE)
            set(xplane_plugin_install_dir "${opentrack-install-xplane}/opentrack/mac_x64")
        else()
            # X-Plane 32bit is dead since XP11 so we assume 64 bit now
            set(xplane_plugin_install_dir "${opentrack-install-xplane}/opentrack/lin_x64")
        endif()


        target_compile_options(${self} PRIVATE -DOTR_REQUIRED )

        if(APPLE)
            find_library(CoreFoundation CoreFoundation)
            target_link_libraries(${self} PRIVATE libopentrackclient_static ${CoreFoundation})
        else()
            target_link_libraries(${self} PRIVATE rt libopentrackclient_static64)
        endif()

        # install(FILES $<TARGET_FILE:libopentrackclient> DESTINATION ${xplane_plugin_install_dir})


        install(TARGETS "${self}"
            RUNTIME DESTINATION ${xplane_plugin_install_dir}
            BUNDLE	DESTINATION ${xplane_plugin_install_dir}
            LIBRARY DESTINATION ${xplane_plugin_install_dir}
            PERMISSIONS ${opentrack-perms-exec})

        if(CMAKE_COMPILER_IS_GNUCC AND NOT CMAKE_COMPILER_IS_CLANG)
            target_link_options(${self} PRIVATE -undefined_warning)
        endif()

        set_target_properties(${self} PROPERTIES
            LIBRARY_OUTPUT_NAME "opentrack.xpl"
            PREFIX "" SUFFIX "")

    endif()
endif()
