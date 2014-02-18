add_library(amixer-subsystem SHARED
    LegacyAmixerSubsystemBuilder.cpp
    LegacyAmixerControl.cpp)

include_directories(
    ${PFW_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/base)

target_link_libraries(amixer-subsystem
    alsa-subsystem
    ${ALSA_LIBRARIES}
    ${PFW_LIBRARIES})

install(TARGETS amixer-subsystem LIBRARY DESTINATION lib)
