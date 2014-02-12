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
#include "TinyAmixerControlArray.hpp"
#include "InstanceConfigurableElement.h"
#include "MappingContext.h"
#include <tinyalsa/asoundlib.h>
#include <errno.h>
#include <string.h>
#include <sstream>

#define base TinyAmixerControl

TinyAmixerControlArray::TinyAmixerControlArray(
    const string &mappingValue,
    CInstanceConfigurableElement *instanceConfigurableElement,
    const CMappingContext &context)
    : base(mappingValue, instanceConfigurableElement, context, _byteScalarSize)
{
}

bool TinyAmixerControlArray::readControl(struct mixer_ctl *mixerControl,
                                         uint32_t elementCount,
                                         string &error)
{
    int err;

    if ((err = mixer_ctl_get_array(mixerControl, getBlackboardLocation(), elementCount)) < 0) {

        error = "Failed to read value in mixer control: " + getControlName() + ": " +
                strerror(-err);
        return false;
    }

    if (isDebugEnabled()) {

        logControlValues(true, elementCount);
    }

    return true;
}

bool TinyAmixerControlArray::writeControl(struct mixer_ctl *mixerControl,
                                          uint32_t elementCount,
                                          string &error)
{
    int err;

    if (isDebugEnabled()) {

        logControlValues(false, elementCount);
    }

    // Write element
    if ((err = mixer_ctl_set_array(mixerControl, getBlackboardLocation(), elementCount)) < 0) {

        error = "Failed to write value in mixer control: " + getControlName() + ": " +
                strerror(-err);
        return false;
    }

    return true;
}

void TinyAmixerControlArray::logControlValues(bool receive, uint32_t elementCount) const
{
    const unsigned char *buffer =
        reinterpret_cast<const unsigned char *>(getBlackboardLocation());
    unsigned int idx;
    std::stringstream log;

    log << (receive ? "Reading" : "Writing");
    log << " alsa element: " << getControlName() << " " << getIndex() << " with value: ";
    for (idx = 0; idx < elementCount; idx++) {
        log.width(2);
        log.fill('0');
        // cast to uint16_t necessary in order to avoid 'buffer[idx]' to be
        // treated as a printable character, apparently
        log << hex << static_cast<unsigned short>(buffer[idx]) << " ";
    }
    log << "[" << dec << elementCount << " bytes]" << endl;

    log_info("%s", log.str().c_str());
}
