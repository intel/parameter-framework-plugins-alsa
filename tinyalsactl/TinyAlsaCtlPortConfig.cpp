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
#include "TinyAlsaCtlPortConfig.hpp"
#include "MappingContext.h"
#include "AlsaCtlMappingKeys.hpp"

#include <string.h>
#include <tinyalsa/asoundlib.h>
#include <sstream>

#define base AlsaCtlPortConfig

const int TinyAlsaCtlPortConfig::_nbRingBuffer = 2;

// Translation table between Alsa and TinyAlsa enums.
const AlsaCtlPortConfig::FormatTranslation pcmFormatTranslationTable[] = {

    // TinyAlsa Value,                               // Litteral Value,     // Alsa Value
    { AlsaCtlPortConfig::_tinyAlsaFormatInvalid,    "PCM_FORMAT_S8" },     // SND_PCM_FORMAT_S8
    { AlsaCtlPortConfig::_tinyAlsaFormatInvalid,    "PCM_FORMAT_U8" },     // SND_PCM_FORMAT_U8
    { PCM_FORMAT_S16_LE                         ,    "PCM_FORMAT_S16_LE" }, // SND_PCM_FORMAT_S16_LE
    { AlsaCtlPortConfig::_tinyAlsaFormatInvalid,    "PCM_FORMAT_S16_BE" }, // SND_PCM_FORMAT_S16_BE
    { AlsaCtlPortConfig::_tinyAlsaFormatInvalid,    "PCM_FORMAT_U16_LE" }, // SND_PCM_FORMAT_U16_LE
    { AlsaCtlPortConfig::_tinyAlsaFormatInvalid,    "PCM_FORMAT_U16_BE" }, // SND_PCM_FORMAT_U16_BE
    { PCM_FORMAT_S24_LE                         ,    "PCM_FORMAT_S24_LE" }, // SND_PCM_FORMAT_S24_LE
    { AlsaCtlPortConfig::_tinyAlsaFormatInvalid,    "PCM_FORMAT_S24_BE" }, // SND_PCM_FORMAT_S24_BE
    { AlsaCtlPortConfig::_tinyAlsaFormatInvalid,    "PCM_FORMAT_U24_LE" }, // SND_PCM_FORMAT_U24_LE
    { AlsaCtlPortConfig::_tinyAlsaFormatInvalid,    "PCM_FORMAT_U24_BE" }, // SND_PCM_FORMAT_U24_BE
    { PCM_FORMAT_S32_LE                         ,    "PCM_FORMAT_S32_LE" }  // SND_PCM_FORMAT_S32_LE
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
    const string &mappingValue,
    CInstanceConfigurableElement *instanceConfigurableElement,
    const CMappingContext &context)
    : base(mappingValue, instanceConfigurableElement, context, _defaultPortConfig)
{
    // Init stream handle array
    _streamHandle[Playback] = NULL;
    _streamHandle[Capture] = NULL;
}

bool TinyAlsaCtlPortConfig::doOpenStream(StreamDirection streamDirection, string &error)
{
    struct pcm *&streamHandle = _streamHandle[streamDirection];
    struct pcm_config pcmConfig;

    const AlsaCtlPortConfig::PortConfig &portConfig = getPortConfig();

    // Fill PCM configuration structure
    pcmConfig.channels = portConfig.channelNumber;
    pcmConfig.rate = portConfig.sampleRate;

    // Check Format is supported by the plugin
    if (portConfig.format >= pcmFormatTranslationTableSize) {

        error = "The format " + pcmFormatTranslationTable[portConfig.format].formatAsString +
                " is not supported by the TinyAlsa plugin";
        return false;
    }

    int8_t format = pcmFormatTranslationTable[portConfig.format].formatAsNumerical;

    // Check format is supported by Tinyalsa
    if (format < 0) {

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
