# INTEL CONFIDENTIAL
#
# Copyright 2013 Intel Corporation All Rights Reserved.
# The source code contained or described herein and all documents
# related to the source code ("Material") are owned by Intel
# Corporation or its suppliers or licensors. Title to the Material
# remains with Intel Corporation or its suppliers and licensors. The
# Material contains trade secrets and proprietary and confidential
# information of Intel or its suppliers and licensors. The Material is
# protected by worldwide copyright and trade secret laws and treaty
# provisions. No part of the Material may be used, copied, reproduced,
# modified, published, uploaded, posted, transmitted, distributed, or
# disclosed in any way without Intel's prior express written permission.
#
# No license under any patent, copyright, trade secret or other
# intellectual property right is granted to or conferred upon you by
# disclosure or delivery of the Materials, either expressly, by
# implication, inducement, estoppel or otherwise. Any license under
# such intellectual property rights must be express and approved by
# Intel in writing.

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    TinyAlsaCtlPortConfig.cpp \
    TinyAlsaCtlSubsystemBuilder.cpp \

LOCAL_C_INCLUDES := \
    external/parameter-framework/core/parameter \
    external/tinyalsa/include \
    external/stlport/stlport \
    bionic/ \

LOCAL_SHARED_LIBRARIES := \
    libparameter \
    libstlport \
    libcutils \
    libicuuc \
    libtinyalsa \
    libtinyamixer-subsystem \

LOCAL_STATIC_LIBRARIES := \
    libalsa-subsystem \
    libxmlserializer \
    libxml2 \

LOCAL_CFLAGS := \
    -Wall \
    -Werror \
    -Wextra \
    -Wno-unused-parameter \

LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/parameter-framework-plugins/Audio
LOCAL_MODULE := libtinyalsactl-subsystem
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
