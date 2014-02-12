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

#include "AmixerControl.hpp"
#include "InstanceConfigurableElement.h"
#include "MappingContext.h"
#include <string>

/** This class implements a mutable volume.
 *
 * The template parameter must be a subsystemObject
 * using the virtual from and to blackboard functions.
 */
template <class SubsystemObjectBase>
class AmixerMutableVolume : public SubsystemObjectBase
{
private:
    /** The blackboard memory representation of a mutable volume. */
    struct MutableVolume
    {
        int8_t muted;
        int level;
    } __attribute__((packed));

    /** Indexes of mute and level children parameters in the MutableVolume parameter tree */
    enum MutableVolumeChildren
    {
        muted = 0,
        level
    };

public:
    /**
     * AMixerMutableVolume Class constructor
     *
     * @param[in] mappingValue instantiation mapping value
     * @param[in] instConfigElement pointer to configurable element instance
     * @param[in] context contains the context mappings
     */
    AmixerMutableVolume(const string &mappingValue,
                        CInstanceConfigurableElement *instConfigElement,
                        const CMappingContext &context)
        : SubsystemObjectBase(mappingValue, instConfigElement, context),
          _volumeLevelConfigurableElement(NULL)
    {
        if ((instConfigElement->getType() == CInstanceConfigurableElement::EParameterBlock) &&
            (this->getScalarSize() <= sizeof(MutableVolume)) &&
            (this->getScalarSize() > sizeof(MutableVolume::muted)) &&
            instConfigElement->getNbChildren() == 2) {

            // Get Actual volume level element
            _volumeLevelConfigurableElement = static_cast<const CInstanceConfigurableElement *>(
                instConfigElement->getChild(level));

        } else {

            this->setTypeIsSupported(false);
        }
    }

protected:
    virtual int fromBlackboard();
    virtual void toBlackboard(int volumeLevel);

private:
    static const int muteLevelValue = 0;
    /** Pointer on configurable element corresponding to volume level */
    const CInstanceConfigurableElement *_volumeLevelConfigurableElement;
};

#include <cassert>

template <class SubsystemObjectBase>
int AmixerMutableVolume<SubsystemObjectBase>::fromBlackboard()
{
    const size_t volumeSize = this->getScalarSize();
    assert(volumeSize <= sizeof(MutableVolume));

    // Be aware that this code does not work in big endian if volumeSize < sizeof(MutableVolume)
    MutableVolume volume = {
        false, 0
    };
    this->blackboardRead(&volume, volumeSize);

    // Take care of sign extension
    return this->toPlainInteger(_volumeLevelConfigurableElement,
                                volume.muted ? muteLevelValue : volume.level);
}

template <class SubsystemObjectBase>
void AmixerMutableVolume<SubsystemObjectBase>::toBlackboard(int volumeLevel)
{
    const size_t volumeSize = this->getScalarSize();
    assert(volumeSize <= sizeof(MutableVolume));

    // Be aware that this code does not work in big endian if volumeSize < sizeof(MutableVolume)
    const MutableVolume volume = {
        false, volumeLevel
    };
    this->blackboardWrite(&volume, volumeSize);
}
