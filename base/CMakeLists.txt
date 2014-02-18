add_library(alsa-subsystem STATIC
    AlsaSubsystemObject.cpp
    AlsaCtlPortConfig.cpp
    AmixerControl.cpp)
    
include_directories(${PFW_INCLUDE_DIRS})

# FIXME: suppress the need for -Wno-unused-parameter
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter -fPIC")
