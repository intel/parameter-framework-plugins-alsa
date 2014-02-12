/*
 * INTEL CONFIDENTIAL
 * Copyright © 2013 Intel
 * Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its suppliers
 * or licensors. Title to the Material remains with Intel Corporation or its
 * suppliers and licensors. The Material contains trade secrets and proprietary
 * and confidential information of Intel or its suppliers and licensors. The
 * Material is protected by worldwide copyright and trade secret laws and
 * treaty provisions. No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intel’s prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by Intel in writing.
 *
 */
#include "TinyAmixerControlValue.hpp"
#include "InstanceConfigurableElement.h"
#include "MappingContext.h"
#include <tinyalsa/asoundlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define base TinyAmixerControl

TinyAmixerControlValue::TinyAmixerControlValue(
    const string &mappingValue,
    CInstanceConfigurableElement *instanceConfigurableElement,
    const CMappingContext &context)
    : base(mappingValue, instanceConfigurableElement, context)
{
}

bool TinyAmixerControlValue::readControl(struct mixer_ctl *mixerControl,
                                         uint32_t elementCount,
                                         string &error)
{
    uint32_t elementNumber;

    // Read element
    // Go through all elements
    for (elementNumber = 0; elementNumber < elementCount; elementNumber++) {

        int32_t value;
        if ((value = mixer_ctl_get_value(mixerControl, elementNumber)) < 0) {

            error = "Failed to read value in mixer control: " + getControlName();
            return false;
        }

        if (isDebugEnabled()) {

            log_info("Reading alsa element %s,%d, index %u with value %u",
                     getControlName().c_str(), getIndex(), elementNumber, value);
        }

        toBlackboard(value);
    }
    return true;
}

bool TinyAmixerControlValue::writeControl(struct mixer_ctl *mixerControl,
                                          uint32_t elementCount,
                                          string &error)
{
    uint32_t elementNumber;

    // Write element
    // Go through all elements
    for (elementNumber = 0; elementNumber < elementCount; elementNumber++) {

        int32_t value;

        // Read data from blackboard (beware this code is OK on Little Endian machines only)
        value = fromBlackboard();

        if (isDebugEnabled()) {

            log_info("Writing alsa element %s,%d, index %u with value %u",
                     getControlName().c_str(), getIndex(), elementNumber, value);
        }

        // Write element
        int err;
        if ((err = mixer_ctl_set_value(mixerControl, elementNumber, value)) < 0) {

            error = "Failed to write value in mixer control: " + getControlName() + ": " +
                    strerror(-err);
            return false;
        }
    }
    return true;
}
