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

#include "AmixerControl.hpp"
#include <string>

/**
 * Base class for alsa mixer controls through tiny alsa.
 * Handles the operations that are common to all mixers.
 */
class TinyAmixerControl : public AmixerControl
{
public:
    /**
     * TinyAmixerControl Class constructor
     *
     * @param[in] mappingValue instantiation mapping value
     * @param[in] instanceConfigurableElement pointer to configurable element instance
     * @param[in] context contains the context mappings
     */
    TinyAmixerControl(const string &mappingValue,
                      CInstanceConfigurableElement *instanceConfigurableElement,
                      const CMappingContext &context);

    /**
     * TinyAMixerControl Class constructor
     *
     * @param[in] mappingValue instantiation mapping value
     * @param[in] instanceConfigurableElement pointer to configurable element instance
     * @param[in] context contains the context mappings
     * @param[in] scalarSize used to force scalarSize value
     */
    TinyAmixerControl(const string &mappingValue,
                      CInstanceConfigurableElement *instanceConfigurableElement,
                      const CMappingContext &context,
                      uint32_t scalarSize);

protected:
    virtual bool accessHW(bool receive, string &error);

    /**
     * Reads the value(s) of an alsa mixer
     *
     * @param[in] mixerControl handle on the mixer control
     * @param[in] elementCount number of elements to write
     * @param[out] error string containing error description
     *
     * @return true if no error
     */
    virtual bool readControl(struct mixer_ctl *mixerControl,
                             uint32_t elementCount,
                             string &error) = 0;

    /**
     * Writes the value(s) of an alsa mixer
     *
     * @param[in] mixerControl handle on the mixer control
     * @param[in] elementCount number of elements to write
     * @param[out] error string containing error description
     *
     * @return true if no error
     */
    virtual bool writeControl(struct mixer_ctl *mixerControl,
                              uint32_t elementCount,
                              string &error) = 0;
};
