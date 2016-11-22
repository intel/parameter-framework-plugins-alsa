/*
 * Copyright (c) 2011-2016, Intel Corporation
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
#include "TinyAlsaCtlPortConfig.hpp"
#include "MappingContext.h"
#include "AlsaMappingKeys.hpp"

#include <tinyalsa/asoundlib.h>
#include <string>
#include <sstream>
#include <limits>

#define base AlsaCtlPortConfig

const int TinyAlsaCtlPortConfig::_nbRingBuffer = 2;

// Translation table between Alsa and TinyAlsa enums.
const AlsaCtlPortConfig::FormatTranslation pcmFormatTranslationTable[] = {

    // TinyAlsa Value,                               // Litteral Value,     // Alsa Value
    { std::numeric_limits<uint8_t>::max(),    "PCM_FORMAT_S8" },     // SND_PCM_FORMAT_S8
    { std::numeric_limits<uint8_t>::max(),    "PCM_FORMAT_U8" },     // SND_PCM_FORMAT_U8
    { PCM_FORMAT_S16_LE                  ,    "PCM_FORMAT_S16_LE" }, // SND_PCM_FORMAT_S16_LE
    { std::numeric_limits<uint8_t>::max(),    "PCM_FORMAT_S16_BE" }, // SND_PCM_FORMAT_S16_BE
    { std::numeric_limits<uint8_t>::max(),    "PCM_FORMAT_U16_LE" }, // SND_PCM_FORMAT_U16_LE
    { std::numeric_limits<uint8_t>::max(),    "PCM_FORMAT_U16_BE" }, // SND_PCM_FORMAT_U16_BE
    { PCM_FORMAT_S24_LE                  ,    "PCM_FORMAT_S24_LE" }, // SND_PCM_FORMAT_S24_LE
    { std::numeric_limits<uint8_t>::max(),    "PCM_FORMAT_S24_BE" }, // SND_PCM_FORMAT_S24_BE
    { std::numeric_limits<uint8_t>::max(),    "PCM_FORMAT_U24_LE" }, // SND_PCM_FORMAT_U24_LE
    { std::numeric_limits<uint8_t>::max(),    "PCM_FORMAT_U24_BE" }, // SND_PCM_FORMAT_U24_BE
    { PCM_FORMAT_S32_LE                  ,    "PCM_FORMAT_S32_LE" }  // SND_PCM_FORMAT_S32_LE
};

const size_t pcmFormatTranslationTableSize =
    sizeof(pcmFormatTranslationTable) / sizeof(pcmFormatTranslationTable[0]);

const AlsaCtlPortConfig::PortConfig TinyAlsaCtlPortConfig::_defaultPortConfig = {
    { false, false },
    PCM_FORMAT_S16_LE,
    2,
    48000
};

TinyAlsaCtlPortConfig::TinyAlsaCtlPortConfig(
    const std::string &mappingValue,
    CInstanceConfigurableElement *instanceConfigurableElement,
    const CMappingContext &context,
    core::log::Logger& logger)
    : base(mappingValue, instanceConfigurableElement, context, logger, _defaultPortConfig)
{
    // Init stream handle array
    _streamHandle[Playback] = NULL;
    _streamHandle[Capture] = NULL;
}

bool TinyAlsaCtlPortConfig::doOpenStream(StreamDirection streamDirection, std::string &error)
{
    struct pcm *&streamHandle = _streamHandle[streamDirection];
    struct pcm_config pcmConfig;

    const AlsaCtlPortConfig::PortConfig &portConfig = getPortConfig();

    // Fill PCM configuration structure
    pcmConfig.channels = portConfig.channelNumber;
    pcmConfig.rate = portConfig.sampleRate;

    // Check Format is supported by the plugin
    if (portConfig.format >= pcmFormatTranslationTableSize) {

        error = "The format n°" + std::to_string(int{portConfig.format}) +
                " is not supported by the TinyAlsa plugin";
        return false;
    }

    uint8_t format = pcmFormatTranslationTable[portConfig.format].formatAsNumerical;

    // Check format is supported by Tinyalsa
    if (format == std::numeric_limits<uint8_t>::max()) {

        error = "The format " + pcmFormatTranslationTable[portConfig.format].formatAsString +
                " is not supported by Tinyalsa";
        return false;
    }

    pcmConfig.format = static_cast<pcm_format>(format);

    pcmConfig.period_size       = _periodTimeMs * pcmConfig.rate / _msPerSec;
    pcmConfig.period_count      = _nbRingBuffer;
    pcmConfig.start_threshold   = 0;
    pcmConfig.stop_threshold    = 0;
    pcmConfig.silence_threshold = 0;
    pcmConfig.silence_size      = 0;
    pcmConfig.avail_min         = 0;

    // Open and configure
    streamHandle = pcm_open(getCardNumber(),
                            getDeviceNumber(),
                            streamDirection == Capture ? PCM_IN : PCM_OUT,
                            &pcmConfig);

    // Prepare the stream
    if (!pcm_is_ready(streamHandle) || (pcm_prepare(streamHandle) != 0)) {

        // Format error
        error = formatAlsaError(streamDirection, "open", pcm_get_error(streamHandle));

        doCloseStream(streamDirection);
        return false;
    }

    return true;
}

void TinyAlsaCtlPortConfig::doCloseStream(StreamDirection streamDirection)
{
    struct pcm *&streamHandle = _streamHandle[streamDirection];

    if (streamHandle) {

        pcm_close(streamHandle);
        streamHandle = NULL;
    }
}
