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
