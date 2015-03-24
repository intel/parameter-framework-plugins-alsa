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
#include "TinyAmixerControlArray.hpp"
#include "InstanceConfigurableElement.h"
#include "MappingContext.h"
#include <tinyalsa/asoundlib.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <sstream>

#define base TinyAmixerControl
// Maximum characters per line
#define maxLogLine 64

TinyAmixerControlArray::TinyAmixerControlArray(
    const std::string &mappingValue,
    CInstanceConfigurableElement *instanceConfigurableElement,
    const CMappingContext &context,
    core::log::Logger& logger)
    : base(mappingValue, instanceConfigurableElement, context, logger,  _byteScalarSize)
{
}

int TinyAmixerControlArray::doGetArrayMixer(struct mixer_ctl *mixerControl,
                                            void *array,
                                            size_t elementCount)
{
    int ret = mixer_ctl_get_array(mixerControl, array, elementCount);

    if (isDebugEnabled() && ret == 0) {
        logControlValues(true, array, elementCount);
    }

    return ret;
}

int TinyAmixerControlArray::doSetArrayMixer(struct mixer_ctl *mixerControl,
                                            const void *array,
                                            size_t elementCount)
{
    if (isDebugEnabled()) {
        logControlValues(false, array, elementCount);
    }

    return mixer_ctl_set_array(mixerControl, array, elementCount);
}

int TinyAmixerControlArray::getArrayMixer(struct mixer_ctl *mixerControl, size_t elementCount)
{
    return doGetArrayMixer(mixerControl, getBlackboardLocation(), elementCount);
}

int TinyAmixerControlArray::setArrayMixer(struct mixer_ctl *mixerControl, size_t elementCount)
{
    return doSetArrayMixer(mixerControl, getBlackboardLocation(), elementCount);
}

bool TinyAmixerControlArray::readControl(struct mixer_ctl *mixerControl,
                                         uint32_t elementCount,
                                         std::string &error)
{
    int err;

    if ((err = getArrayMixer(mixerControl, elementCount)) < 0) {

        error = "Failed to read value in mixer control: " + getControlName() + ": " +
                strerror(-err);
        return false;
    }

    return true;
}

bool TinyAmixerControlArray::writeControl(struct mixer_ctl *mixerControl,
                                          uint32_t elementCount,
                                          std::string &error)
{
    int err;

    // Write element
    if ((err = setArrayMixer(mixerControl, elementCount)) < 0) {

        error = "Failed to write value in mixer control: " + getControlName() + ": " +
                strerror(-err);
        return false;
    }

    return true;
}

void TinyAmixerControlArray::displayAndCleanString(std::stringstream &stringValue) const
{
    _logger.info() << stringValue;
    stringValue.str(std::string());
}

void TinyAmixerControlArray::logControlValues(bool receive,
                                              const void *array,
                                              uint32_t elementCount) const
{
    const unsigned char *buffer = reinterpret_cast<const unsigned char *>(array);
    unsigned int idx;
    std::stringstream log;

    log << (receive ? "Reading" : "Writing");
    log << " alsa element: " << getControlName() << " with value: ";
    displayAndCleanString(log);
    for (idx = 0; idx < elementCount; idx++) {
        log.width(2);
        log.fill('0');
        // cast to uint16_t necessary in order to avoid 'buffer[idx]' to be
        // treated as a printable character, apparently
        log << std::hex << static_cast<unsigned short>(buffer[idx]) << " ";
        if ((idx != 0) && ((idx % maxLogLine) == 0)) {
            log << '\n';
            displayAndCleanString(log);
        }
    }

    if (log.str().length() > 0) {
        log << '\n';
        displayAndCleanString(log);
    }

    log << "[" << std::dec << elementCount << " bytes]" << std::endl;
    displayAndCleanString(log);
}
