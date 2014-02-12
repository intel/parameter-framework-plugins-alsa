/*
 * INTEL CONFIDENTIAL
 * Copyright © 2011 Intel
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
#include "LegacyAlsaCtlPortConfig.hpp"
#include "MappingContext.h"
#include "AlsaCtlMappingKeys.hpp"
#include <string.h>
#include <alsa/asoundlib.h>
#include <sstream>

#define base AlsaCtlPortConfig

const AlsaCtlPortConfig::PortConfig LegacyAlsaCtlPortConfig::_defaultPortConfig = {
    { false, false },
    SND_PCM_FORMAT_S16_LE,
    2,
    48000
};

const uint32_t LegacyAlsaCtlPortConfig::_latencyMicroSeconds = 500000;

LegacyAlsaCtlPortConfig::LegacyAlsaCtlPortConfig(
    const string &mappingValue,
    CInstanceConfigurableElement *instanceConfigurableElement,
    const CMappingContext &context)
    :  base(mappingValue, instanceConfigurableElement, context, _defaultPortConfig)
{
    // Init stream handle array
    _streamHandle[Playback] = NULL;
    _streamHandle[Capture] = NULL;

    // Retrieve card index
    string cardIndex = context.getItem(AmixerCard);

    // Create device name
    ostringstream streamName;

    streamName << "hw:" << snd_card_get_index(cardIndex.c_str())
               << "," << context.getItem(AlsaCtlDevice);

    _streamName = streamName.str();

}

bool LegacyAlsaCtlPortConfig::doOpenStream(StreamDirection streamDirection, string &error)
{
    snd_pcm_t *&streamHandle = _streamHandle[streamDirection];
    int32_t errorId;

    if ((errorId = snd_pcm_open(
             &streamHandle,
             _streamName.c_str(),
             streamDirection == Capture ? SND_PCM_STREAM_CAPTURE : SND_PCM_STREAM_PLAYBACK,
             0)) < 0) {

        error = formatAlsaError(streamDirection, "open", snd_strerror(errorId));

        return false;
    }

    const AlsaCtlPortConfig::PortConfig &portConfig = getPortConfig();

    if ((errorId = snd_pcm_set_params(streamHandle,
                                      static_cast<_snd_pcm_format>(portConfig.format),
                                      SND_PCM_ACCESS_RW_INTERLEAVED,
                                      portConfig.channelNumber,
                                      portConfig.sampleRate,
                                      0,
                                      _latencyMicroSeconds)) < 0) {

        error = formatAlsaError(streamDirection, "set params", snd_strerror(errorId));

        doCloseStream(streamDirection);

        return false;
    }

    return true;
}

void LegacyAlsaCtlPortConfig::doCloseStream(StreamDirection streamDirection)
{
    snd_pcm_t *&hStream = _streamHandle[streamDirection];

    if (hStream) {

        snd_pcm_close(hStream);

        hStream = NULL;
    }
}
