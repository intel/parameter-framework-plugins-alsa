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
    TinyAmixerControlArray(const std::string &mappingValue,
                           CInstanceConfigurableElement *instanceConfigurableElement,
                           const CMappingContext &context,
                           core::log::Logger& logger);

protected:
    virtual bool readControl(struct mixer_ctl *mixerControl,
                             uint32_t elementCount,
                             std::string &error);

    virtual bool writeControl(struct mixer_ctl *mixerControl,
                              uint32_t elementCount,
                              std::string &error);

    /**
     * Derivable method for storing alsa ByteControls into blackboard
     *
     * @param[in] mixerControl the control to be read
     * @param[in] count size of the control
     *
     * @return 0 in case of success, negative errno else
     */
    virtual int getArrayMixer(struct mixer_ctl *mixerControl, size_t count);
    /**
     * Derivable method for syncing the blackboard to alsa ByteControls
     *
     * @param[in] mixerControl the control to be written
     * @param[in] count size of the control
     *
     * @return 0 in case of success, negative errno else
     */
    virtual int setArrayMixer(struct mixer_ctl *mixerControl, size_t count);
    /** Low-level (non-derivable) method used by getArrayMixer()
     *
     * @param[in] mixerControl the control to be read
     * @param[out] array where to store the control's content
     * @param[in] count size of the control
     *
     * @return 0 in case of success, negative errno else
     */
    int doGetArrayMixer(struct mixer_ctl *mixerControl, void *array, size_t count);
    /** Low-level (non-derivable) method used by setArrayMixer()
     *
     * @param[in] mixerControl the control to be written
     * @param[out] array content to be written
     * @param[in] count size of the control
     *
     * @return 0 in case of success, negative errno else
     */
    int doSetArrayMixer(struct mixer_ctl *mixerControl, const void *array, size_t count);

    /** Scalar size for byte mixer controls */
    static const uint32_t _byteScalarSize = 1;

private:
    /**
     * Log the values read or written in an alsa mixer BYTE control
     *
     * @param[in] receive a boolean indicating if we receive or send the values from/to tinyalsa
     * @param[in] elementCount the number of element to log
     */
    void logControlValues(bool receive, const void *array, uint32_t elementCount) const;
    /**
     * Method for displaying and cleaning stringstream objects
     *
     * @param[in] sstm stringstream object to be displayed and cleaned
     */
    void displayAndCleanString(std::stringstream &stringValue) const;
};
