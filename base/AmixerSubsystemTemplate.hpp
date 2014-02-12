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
#pragma once

#include "Subsystem.h"
#include "AmixerMappingKeys.hpp"
#include "SubsystemObjectFactory.h"
#include "AmixerMutableVolume.hpp"

/**
 * Template class for Alsa mixer subsystems.
 * This class is a template for alsa mixer control subsystems, it will be used by both legacy alsa
 * and tiny alsa subsystems.
 */
template <class AmixerControlType, class AmixerByteControlType>
class AmixerSubsystem : public CSubsystem
{
public:
    AmixerSubsystem(const string &name) : CSubsystem(name)
    {
        // Provide mapping keys to upper layer
        addContextMappingKey("Card");
        addContextMappingKey("Index");
        addContextMappingKey("Debug");
        addContextMappingKey("Amend1");
        addContextMappingKey("Amend2");
        addContextMappingKey("Amend3");
        addContextMappingKey("Amend4");

        // Provide creators to upper layer
        addSubsystemObjectFactory(
            new TSubsystemObjectFactory<AmixerControlType>("Control", 1 << AmixerCard)
            );

        addSubsystemObjectFactory(
            new TSubsystemObjectFactory<AmixerByteControlType>(
                "ByteControl", 1 << AmixerCard)
            );

        addSubsystemObjectFactory(
            new TSubsystemObjectFactory<
                AmixerMutableVolume<AmixerControlType> >("Volume", 1 << AmixerCard)
            );
    }
};
