otr_module(proto-crossover-trackir)

set(CROSSOVER_TRACKIR_BRIDGE_DLL
    "${CMAKE_CURRENT_SOURCE_DIR}/resources/NPClient64.dll"
)

install(
    FILES "${CROSSOVER_TRACKIR_BRIDGE_DLL}"
    DESTINATION "opentrack.app/Contents/Resources/crossover-trackir"
)
