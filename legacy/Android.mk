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

LOCAL_PATH := $(call my-dir)

ifeq ($(PFW_LEGACY_ALSA),true)

#######################################################################
# libalsa-subsystem

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    LegacyAlsaSubsystem.cpp \
    LegacyAlsaSubsystemBuilder.cpp \
    LegacyAmixerControl.cpp \
    LegacyAlsaCtlPortConfig.cpp \

LOCAL_C_INCLUDES += external/alsa-lib/include

LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)

LOCAL_SHARED_LIBRARIES := \
    libparameter \
    libasound \

LOCAL_STATIC_LIBRARIES := \
    libalsabase-subsystem \
    libparameter_includes \
    libxmlserializer_includes \

# -D_POSIX_C_SOURCE=200809 is needed because alsa-lib is redefining
# the timeval and timespec structures
LOCAL_CFLAGS += \
    -D_POSIX_C_SOURCE=200809 \
    -Wall \
    -Werror \
    -Wextra \
    -Wno-unused-parameter    # Needed to workaround STL bug

LOCAL_MODULE := libalsa-subsystem
LOCAL_MODULE_TAGS := optional

include external/stlport/libstlport.mk
include $(BUILD_SHARED_LIBRARY)

endif # $(PFW_LEGACY_ALSA)
