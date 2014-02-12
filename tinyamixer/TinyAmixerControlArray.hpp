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

#include "TinyAmixerControl.hpp"
#include <string>

/**
 * Class to handle alsa mixer controls of type BYTE.
 */
class TinyAmixerControlArray : public TinyAmixerControl
{
public:
    /**
     * TinyAMixerByteControl Class constructor
     *
     * @param[in] mappingValue instantiation mapping value
     * @param[in] instanceConfigurableElement pointer to configurable element instance
     * @param[in] context contains the context mappings
     */
    TinyAmixerControlArray(const string &mappingValue,
                           CInstanceConfigurableElement *instanceConfigurableElement,
                           const CMappingContext &context);

protected:
    virtual bool readControl(struct mixer_ctl *mixerControl,
                             uint32_t elementCount,
                             string &error);

    virtual bool writeControl(struct mixer_ctl *mixerControl,
                              uint32_t elementCount,
                              string &error);

private:
    /**
     * Log the values read or written in an alsa mixer BYTE control
     *
     * @param[in] receive a boolean indicating if we receive or send the values from/to tinyalsa
     * @param[in] elementCount the number of element to log
     */
    void logControlValues(bool receive, uint32_t elementCount) const;

private:
    /** Scalar size for byte mixer controls */
    static const uint32_t _byteScalarSize = 1;
};
