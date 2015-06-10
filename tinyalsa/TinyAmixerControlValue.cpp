/*
 * Copyright (c) 2011-2014, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "TinyAmixerControlValue.hpp"
#include "InstanceConfigurableElement.h"
#include "MappingContext.h"
#include <tinyalsa/asoundlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <string>
#include <sstream>

#define base TinyAmixerControl

TinyAmixerControlValue::TinyAmixerControlValue(
    const std::string &mappingValue,
    CInstanceConfigurableElement *instanceConfigurableElement,
    const CMappingContext &context,
    core::log::ILogger& logger)
    : base(mappingValue, instanceConfigurableElement, context, logger)
{
}

bool TinyAmixerControlValue::readControl(struct mixer_ctl *mixerControl,
                                         size_t elementCount,
                                         std::string &error)
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

            std::ostringstream log;
            log << "Reading alsa element " << getControlName() << ", index " << elementNumber
                << " with value " << value;
            _Logger.info(log.str());
        }

        toBlackboard(value);
    }
    return true;
}

bool TinyAmixerControlValue::writeControl(struct mixer_ctl *mixerControl,
                                          size_t elementCount,
                                          std::string &error)
{
    uint32_t elementNumber;

    // Write element
    // Go through all elements
    for (elementNumber = 0; elementNumber < elementCount; elementNumber++) {

        int32_t value;

        // Read data from blackboard (beware this code is OK on Little Endian machines only)
        value = fromBlackboard();

        if (isDebugEnabled()) {

            std::ostringstream log;
            log << "Writing alsa element " << getControlName() << ", index " << elementNumber
                << " with value " << value;
            _Logger.info(log.str());
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
