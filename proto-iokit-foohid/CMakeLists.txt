# foohid is DEAD so don't build by default
set(WITH_IOKIT_FOOHID FALSE CACHE BOOL "Should proto-iokit-foohid be built - it's practically dead by now.")
if(APPLE AND WITH_IOKIT_FOOHID)
    otr_module(proto-iokit-foohid)
    target_link_options(${self} PRIVATE -framework IOKit)
endif()
