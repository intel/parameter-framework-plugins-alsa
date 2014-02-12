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
#include "TinyAmixerControl.hpp"
#include "InstanceConfigurableElement.h"
#include "MappingContext.h"
#include "AutoLog.h"
#include <tinyalsa/asoundlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>

#ifdef __USE_GCOV__
extern "C" void __gcov_flush();
#endif // __USE_GCOV__

#define base AmixerControl

TinyAmixerControl::TinyAmixerControl(const string &mappingValue,
                                     CInstanceConfigurableElement *instanceConfigurableElement,
                                     const CMappingContext &context)
    : base(mappingValue, instanceConfigurableElement, context)
{
#ifdef __USE_GCOV__
    atexit(__gcov_flush);
#endif // __USE_GCOV__
}

TinyAmixerControl::TinyAmixerControl(const string &mappingValue,
                                     CInstanceConfigurableElement *instanceConfigurableElement,
                                     const CMappingContext &context, uint32_t scalarSize)
    : base(mappingValue, instanceConfigurableElement, context, scalarSize)
{
}

bool TinyAmixerControl::accessHW(bool receive, string &error)
{
    CAutoLog autoLog(getConfigurableElement(), "AMIXER", isDebugEnabled());

    // Mixer handle
    struct mixer *mixer;
    // Mixer control handle
    struct mixer_ctl *mixerControl;
    uint32_t elementCount;
    string controlName = getControlName();

    // Debug conditionnaly enabled in XML
    logControlInfo(receive);

    // Check parameter type is ok (deferred error, no exceptions available :-()
    if (!isTypeSupported()) {

        error = "Parameter type not supported.";
        return false;
    }

    // Check card number
    int32_t cardIndex = getCardNumber();
    if (cardIndex < 0) {

        error = "Card " + getCardName() + " not found. Error: " + strerror(-cardIndex);
        return false;
    }

    // Open alsa mixer
    mixer = mixer_open(cardIndex);

    if (!mixer) {

        error = "Failed to open mixer for card: " + getCardName();
        return false;
    }

    // Get control handle
    if (isdigit(controlName[0])) {

        mixerControl = mixer_get_ctl(mixer, asInteger(controlName));
    } else {

        if (hasIndex()) {

            mixerControl = mixer_get_ctl_by_name_and_index(mixer, controlName.c_str(), getIndex());

        } else {

            mixerControl = mixer_get_ctl_by_name(mixer, controlName.c_str());
        }
    }

    // Check control has been found
    if (!mixerControl) {
        error = "Failed to open mixer control: " + controlName;

        // Close mixer
        mixer_close(mixer);
        return false;
    }

    // Get element count
    elementCount =  mixer_ctl_get_num_values(mixerControl);

    uint32_t scalarSize = getScalarSize();

    // Check available size
    if (elementCount * scalarSize != getSize()) {

        error = "AMIXER: Control element count (" + asString(elementCount) +
                ") and configurable scalar element count (" +
                asString(getSize() / scalarSize) + ") mismatch";

        // Close mixer
        mixer_close(mixer);
        return false;
    }

    // Read/Write element
    bool success;
    if (receive) {

        success = readControl(mixerControl, elementCount, error);

    } else {

        success = writeControl(mixerControl, elementCount, error);

    }

    // Close mixer
    mixer_close(mixer);

    return success;
}
