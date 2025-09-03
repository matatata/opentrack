if(LINUX OR APPLE)
    set(SDK_XPLANE "" CACHE PATH "Path to the X-Plane SDK")
    set(XPLANE_LINK_DYNAMIC FALSE CACHE BOOL "Link to opentrackclient dynamically - default is false")

    if(SDK_XPLANE AND (APPLE OR LINUX))


        if (APPLE)
            # we can build this as universal binary:
            set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
            otr_module(xplane-plugin NO-QT NO-INSTALL)
        else()
            otr_module(xplane-plugin NO-QT)
        endif()
        # probably librt already included
        #install(FILES ${opentrack-xplane-plugin-c} DESTINATION "opentrack-libexec")
        target_include_directories(opentrack-xplane-plugin SYSTEM PUBLIC ${SDK_XPLANE}/CHeaders/XPLM)

        # need to set rpath to relative location in order to load the opentrackclient shared libarary
        if(APPLE)
            set_target_properties(opentrack-xplane-plugin  PROPERTIES
                INSTALL_RPATH "@loader_path;/opt/local/lib")
        elseif(UNIX)
            set_target_properties(opentrack-xplane-plugin  PROPERTIES
                INSTALL_RPATH "$ORIGIN:/opt/local/lib")
        endif()

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

        if(XPLANE_LINK_DYNAMIC)
            if(APPLE)
                set(xplane_plugin_install_dir xplane/opentrack/mac_x64)
            elseif(LINUX)
                # X-Plane 32bit is dead since XP11 so we assume 64 bit now
                set(xplane_plugin_install_dir xplane/opentrack/lin_x64)
            endif()

            if(LINUX)
                target_link_libraries(${self} PRIVATE rt opentrackclient64)
            else()
                target_link_libraries(${self} PRIVATE opentrackclient)
            endif()

            install(FILES $<TARGET_FILE:opentrackclient> DESTINATION ${xplane_plugin_install_dir})
        else()
            set(xplane_plugin_install_dir xplane)

            if(LINUX)
                target_link_libraries(${self} PRIVATE rt opentrackclient_static64)
            else()
                target_link_libraries(${self} PRIVATE opentrackclient_static64)
            endif()
        endif()



        # include the opentrackclient shared library:
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
