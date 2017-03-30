/*
 * Copyright (c) 2011-2017, Intel Corporation
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

#include "TinyAlsaSubsystem.hpp"
#include "TinyAmixerControlArray.hpp"
#include "TinyAmixerControlValue.hpp"
#include "TinyAlsaCtlPortConfig.hpp"
#include "SubsystemObjectFactory.h"
#include "AlsaMappingKeys.hpp"
#include "AmixerMutableVolume.hpp"
#include <string>

TinyAlsaSubsystem::TinyAlsaSubsystem(const std::string &name, core::log::Logger &logger)
    : AlsaSubsystem(name, logger), mMixers()
{
    // Provide creators to upper layer
    addSubsystemObjectFactory(
        new TSubsystemObjectFactory<TinyAmixerControlValue>("Control", 1 << AlsaCard));

    addSubsystemObjectFactory(
        new TSubsystemObjectFactory<TinyAmixerControlArray>("ByteControl", 1 << AlsaCard));

    addSubsystemObjectFactory(
        new TSubsystemObjectFactory<AmixerMutableVolume<TinyAmixerControlValue>>("Volume",
                                                                                 1 << AlsaCard));

    addSubsystemObjectFactory(new TSubsystemObjectFactory<TinyAlsaCtlPortConfig>(
        "PortConfig", (1 << AlsaCard) | (1 << AlsaCtlDevice)));
}

TinyAlsaSubsystem::~TinyAlsaSubsystem()
{
    MixerMap::const_iterator it;

    for (it = mMixers.begin(); it != mMixers.end(); ++it) {
        mixer_close(it->second);
    }
}

struct mixer *TinyAlsaSubsystem::getMixerHandle(int32_t cardNumber)
{
    MixerMap::const_iterator it = mMixers.find(cardNumber);
    if (it != mMixers.end()) {
        return it->second;
    }

    // create handle
    struct mixer *newMixer = mixer_open(cardNumber);
    if (newMixer == NULL) {
        return NULL;
    }
    mMixers.insert(std::make_pair(cardNumber, newMixer));

    return newMixer;
}
