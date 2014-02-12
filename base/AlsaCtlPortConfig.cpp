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
#include "AlsaCtlPortConfig.hpp"
#include "MappingContext.h"
#include "AlsaCtlMappingKeys.hpp"
#include <string.h>
#include <assert.h>
#include <sstream>
#include <limits>

#define base AlsaSubsystemObject

const uint8_t AlsaCtlPortConfig::_tinyAlsaFormatInvalid = numeric_limits<uint8_t>::max();

AlsaCtlPortConfig::AlsaCtlPortConfig(const string &mappingValue,
                                     CInstanceConfigurableElement *instanceConfigurableElement,
                                     const CMappingContext &context,
                                     const PortConfig &defaultPortConfig)
    : base(mappingValue,
           instanceConfigurableElement,
           context),
      _device(context.getItemAsInteger(AlsaCtlDevice)),
      _portConfig(defaultPortConfig)
{

}

bool AlsaCtlPortConfig::receiveFromHW(string &error)
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
    ostringstream stringStream;

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
