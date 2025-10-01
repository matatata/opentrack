if(NOT WIN32)
    otr_module(proto-freetrack-posix)
    target_link_libraries(opentrack-proto-freetrack-posix PRIVATE opentrack-csv libfreetrackclient_headers)
endif()
