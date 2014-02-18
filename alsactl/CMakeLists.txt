add_library(alsactl-subsystem SHARED
    LegacyAlsaCtlSubsystemBuilder.cpp
    LegacyAlsaCtlPortConfig.cpp)

include_directories(
    ${PFW_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/base
    ${PROJECT_SOURCE_DIR}/amixer)

target_link_libraries(alsactl-subsystem
    alsa-subsystem
    ${ALSA_LIBRARIES}
    ${PFW_LIBRARIES})
