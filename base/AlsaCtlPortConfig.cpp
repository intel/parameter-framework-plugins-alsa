/*
 * Copyright (c) 2011-2015, Intel Corporation
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
#include "AlsaCtlPortConfig.hpp"
#include "MappingContext.h"
#include "AlsaMappingKeys.hpp"
#include <string.h>
#include <string>
#include <assert.h>
#include <sstream>
#include <limits>

using std::string;

#define base AlsaSubsystemObject

AlsaCtlPortConfig::AlsaCtlPortConfig(const string &mappingValue,
                                     CInstanceConfigurableElement *instanceConfigurableElement,
                                     const CMappingContext &context,
                                     core::log::Logger& logger,
                                     const PortConfig &defaultPortConfig)
    : base(mappingValue,
           instanceConfigurableElement,
           context,
           logger),
      _device(context.getItemAsInteger(AlsaCtlDevice)),
      _portConfig(defaultPortConfig)
{

}

bool AlsaCtlPortConfig::receiveFromHW(string &/*error*/)
{
    blackboardWrite(&_portConfig, sizeof(_portConfig));

    return true;
}

bool AlsaCtlPortConfig::sendToHW(string &error)
{
    PortConfig portConfig;
    blackboardRead(&portConfig, sizeof(portConfig));

    // If device update is needed, close all the stream
    if (isDeviceUpdateNeeded(portConfig)) {

        // Close playback and capture
        closeStream(Playback);
        closeStream(Capture);

        // Save new configuration
        _portConfig.channelNumber = portConfig.channelNumber;
        _portConfig.format = portConfig.format;
        _portConfig.sampleRate = portConfig.sampleRate;

    } else {

        // Close playback/capture streams if asked for
        if (!portConfig.isStreamEnabled[Playback]) {

            closeStream(Playback);
        }
        if (!portConfig.isStreamEnabled[Capture]) {

            closeStream(Capture);
        }
    }

    // Open and configure required streams
    if (portConfig.isStreamEnabled[Playback]) {

        if (!openStream(Playback, error)) {

            return false;
        }
    }
    if (portConfig.isStreamEnabled[Capture]) {

        if (!openStream(Capture, error)) {

            return false;
        }
    }

    // Check port configuration has been considered
    assert(!memcmp(&_portConfig, &portConfig, sizeof(_portConfig)));

    return true;
}

bool AlsaCtlPortConfig::openStream(StreamDirection streamDirection, string &error)
{
    // Stream needs to be opened only if it was closed previously
    if (!_portConfig.isStreamEnabled[streamDirection]) {

        if (!doOpenStream(streamDirection, error)) {

            return false;
        }

        // Stream has to be opened
        _portConfig.isStreamEnabled[streamDirection] = true;
    }

    return true;
}

void AlsaCtlPortConfig::closeStream(StreamDirection streamDirection)
{
    // Stream needs to be closed only if it was opened previously
    if (_portConfig.isStreamEnabled[streamDirection]) {

        // Stream has to be closed
        doCloseStream(streamDirection);

        _portConfig.isStreamEnabled[streamDirection] = false;
    }
}

string AlsaCtlPortConfig::formatAlsaError(StreamDirection streamDirection,
                                          const string &functionName,
                                          const string &error)
{
    std::ostringstream stringStream;

    stringStream << (streamDirection ? "Capture" : "Playback") << " " <<
        functionName << " error: " << error;

    return stringStream.str();
}

bool AlsaCtlPortConfig::isStreamEnabled(StreamDirection streamDirection) const
{
    return _portConfig.isStreamEnabled[streamDirection] != 0;

}

bool AlsaCtlPortConfig::isDeviceUpdateNeeded(const PortConfig &portConfig) const
{
    return (_portConfig.channelNumber != portConfig.channelNumber) ||
           (_portConfig.format != portConfig.format) ||
           (_portConfig.sampleRate != portConfig.sampleRate);
}
