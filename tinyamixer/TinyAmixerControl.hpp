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
