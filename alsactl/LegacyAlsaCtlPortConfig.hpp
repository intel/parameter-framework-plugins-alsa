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

#include "AlsaCtlPortConfig.hpp"
#include <stdint.h>

struct _snd_pcm;

class LegacyAlsaCtlPortConfig : public AlsaCtlPortConfig
{
public:
    /**
     * LegacyAlsaCtlPortConfig Class constructor
     *
     * @param[in] mappingValue instantiation mapping value
     * @param[in] instanceConfigurableElement pointer to configurable element instance
     * @param[in] context contains the context mappings
     */
    LegacyAlsaCtlPortConfig(const string &mappingValue,
                            CInstanceConfigurableElement *instanceConfigurableElement,
                            const CMappingContext &context);

protected:
    // Stream operations
    virtual bool doOpenStream(StreamDirection streamDirection, string &error);
    virtual void doCloseStream(StreamDirection streamDirection);

private:
    /** Default port configuration */
    static const PortConfig _defaultPortConfig;
    /** Latency */
    static const uint32_t _latencyMicroSeconds;
    /** Stream Name */
    string _streamName;

    /**
     * Stream handles.
     * Uses StreamDirection as index.
     */
    _snd_pcm *_streamHandle[_streamDirectionCount];
};
