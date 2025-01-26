if(NOT WIN32)
    otr_module(proto-native-shm)
    target_link_libraries(opentrack-proto-native-shm PRIVATE opentrack-csv)
endif()
