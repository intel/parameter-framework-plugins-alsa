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
