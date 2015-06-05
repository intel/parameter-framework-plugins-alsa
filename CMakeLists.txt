# Copyright (c) 2011-2014, Intel Corporation
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors
# may be used to endorse or promote products derived from this software without
# specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# has been tested on 2.8 only - might work on older versions
cmake_minimum_required(VERSION 2.8)

project(parameter-framework-plugins-alsa)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra -std=c++11")

#
# Find PFW libraries and include directories
#
find_path(PFW_CORE_ROOT_DIR NAMES include/parameter/plugin/Subsystem.h)

find_library(PFW_CORE_LIBRARY NAMES parameter
    HINTS ${PFW_CORE_ROOT_DIR}/lib)

find_path(PFW_CORE_INCLUDE_DIR NAMES Subsystem.h
    HINTS ${PFW_CORE_ROOT_DIR}/include/parameter/plugin)
find_path(PFW_XMLSERIALIZER_INCLUDE_DIR NAMES XmlSink.h
    HINTS ${PFW_CORE_ROOT_DIR}/include/xmlserializer)

set(PFW_INCLUDE_DIRS ${PFW_CORE_INCLUDE_DIR} ${PFW_XMLSERIALIZER_INCLUDE_DIR})
set(PFW_LIBRARIES ${PFW_CORE_LIBRARY})

#
# Find (regular) alsa-lib
#
include(FindALSA)

add_subdirectory(base)
add_subdirectory(legacy)

# tinyalsa libraries not compiled yet since its use outside of Android seems
# very limited. Besides, at the time of writing, tinyalsa's Makefile does not
# have any "install" target. If needed, it should be quite simple to write the
# CMakeLists for tinyalsa.
