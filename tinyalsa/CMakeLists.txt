# Copyright (c) 2011-2016, Intel Corporation
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

find_library(tinyalsa_lib tinyalsa)
find_path(tinyalsa_includes tinyalsa/asoundlib.h)

# TODO: when not found

add_library(tinyalsa-subsystem SHARED
    TinyAlsaCtlPortConfig.cpp
    TinyAlsaSubsystemBuilder.cpp
    TinyAlsaSubsystem.cpp
    TinyAmixerControlArray.cpp
    TinyAmixerControl.cpp
    TinyAmixerControlValue.cpp)

target_include_directories(tinyalsa-subsystem
    PUBLIC ${PROJECT_SOURCE_DIR}/base ${PROJECT_SOURCE_DIR}/legacy ${tinyalsa_includes})

target_link_libraries(tinyalsa-subsystem
    alsabase-subsystem
    ${tinyalsa_lib})

install(TARGETS tinyalsa-subsystem LIBRARY DESTINATION lib)
