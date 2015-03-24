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
#include "TinyAmixerControl.hpp"
#include "TinyAlsaSubsystem.hpp"
#include "InstanceConfigurableElement.h"
#include "MappingContext.h"
#include <tinyalsa/asoundlib.h>
#include <string>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>

#ifdef __USE_GCOV__
extern "C" void __gcov_flush();
#endif // __USE_GCOV__

#define base AmixerControl

TinyAmixerControl::TinyAmixerControl(const std::string &mappingValue,
                                     CInstanceConfigurableElement *instanceConfigurableElement,
                                     const CMappingContext &context,
                                     core::log::ILogger& logger)
    : base(mappingValue, instanceConfigurableElement, context, logger)
{
#ifdef __USE_GCOV__
    atexit(__gcov_flush);
#endif // __USE_GCOV__
}

TinyAmixerControl::TinyAmixerControl(const std::string &mappingValue,
                                     CInstanceConfigurableElement *instanceConfigurableElement,
                                     const CMappingContext &context,
                                     core::log::ILogger& logger,
                                     uint32_t scalarSize)
    : base(mappingValue, instanceConfigurableElement, context, logger, scalarSize)
{
}

uint32_t TinyAmixerControl::getNumValues(struct mixer_ctl *mixerControl)
{
    return mixer_ctl_get_num_values(mixerControl);
}

bool TinyAmixerControl::accessHW(bool receive, std::string &error)
{
    // Mixer handle
    struct mixer *mixer;
    // Mixer control handle
    struct mixer_ctl *mixerControl;
    uint32_t elementCount;
    std::string controlName = getControlName();

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
    // getMixerHandle is non-const; we need to forcefully remove the constness
    // then, we need to cast the generic subsystem into a TinyAlsaSubsystem.
    mixer = static_cast<TinyAlsaSubsystem *>(
        const_cast<CSubsystem *>(getSubsystem()))->getMixerHandle(cardIndex);

    if (!mixer) {

        error = "Failed to open mixer for card: " + getCardName();
        return false;
    }

    // Get control handle
    if (isdigit(controlName[0])) {

        mixerControl = mixer_get_ctl(mixer, asInteger(controlName));
    } else {

        mixerControl = mixer_get_ctl_by_name(mixer, controlName.c_str());
    }

    // Check control has been found
    if (!mixerControl) {
        error = "Failed to open mixer control: " + controlName;

        return false;
    }

    // Get element count
    elementCount = getNumValues(mixerControl);

    uint32_t scalarSize = getScalarSize();

    // Check available size
    if (elementCount * scalarSize != getSize()) {

        error = "ALSA: Control element count (" + asString(elementCount) +
                ") and configurable scalar element count (" +
                asString(getSize() / scalarSize) + ") mismatch";

        return false;
    }

    // Read/Write element
    bool success;
    if (receive) {

        success = readControl(mixerControl, elementCount, error);

    } else {

        success = writeControl(mixerControl, elementCount, error);

    }

    return success;
}
