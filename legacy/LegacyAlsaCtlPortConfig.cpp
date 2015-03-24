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
#include "LegacyAlsaCtlPortConfig.hpp"
#include "MappingContext.h"
#include "AlsaMappingKeys.hpp"
#include <string.h>
#include <string>
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
    const std::string &mappingValue,
    CInstanceConfigurableElement *instanceConfigurableElement,
    const CMappingContext &context,
    core::log::Logger& logger)
    :  base(mappingValue, instanceConfigurableElement, context, logger, _defaultPortConfig)
{
    // Init stream handle array
    _streamHandle[Playback] = NULL;
    _streamHandle[Capture] = NULL;

    // Retrieve card index
    std::string cardIndex = context.getItem(AlsaCard);

    // Create device name
    std::ostringstream streamName;

    streamName << "hw:" << snd_card_get_index(cardIndex.c_str())
               << "," << context.getItem(AlsaCtlDevice);

    _streamName = streamName.str();

}

bool LegacyAlsaCtlPortConfig::doOpenStream(StreamDirection streamDirection, std::string &error)
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
