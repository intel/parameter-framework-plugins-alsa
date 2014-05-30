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

#include "AlsaSubsystemObject.hpp"

class CInstanceConfigurableElement;
class CMappingContext;

/**
 * Alsa mixer control class.
 * This class handles the configuration of an alsa mixer control through the PFW. It will be
 * derivated for both Legacy alsa subsystem and tiny alsa subsystem.
 */
class AmixerControl : public AlsaSubsystemObject
{
public:
    /**
     * AmixerControl Class constructor
     *
     * @param[in] mappingValue instantiation mapping value
     * @param[in] instanceConfigurableElement pointer to configurable element instance
     * @param[in] context contains the context mappings
     */
    AmixerControl(const string &mappingValue,
                  CInstanceConfigurableElement *instanceConfigurableElement,
                  const CMappingContext &context);

    /**
     * AmixerControl Class constructor
     *
     * @param[in] mappingValue instantiation mapping value
     * @param[in] instanceConfigurableElement pointer to configurable element instance
     * @param[in] context contains the context mappings
     * @param[in] scalarSize used to force scalarSize value
     */
    AmixerControl(const string &mappingValue,
                  CInstanceConfigurableElement *instanceConfigurableElement,
                  const CMappingContext &context, uint32_t scalarSize);

protected:
    virtual bool accessHW(bool receive, string &error) = 0;

    /**
     * Logging Control Info
     * When in debug mode, this function will log information on the parameter name and
     * corresponding mixer control mapping
     *
     * @param[in] receive is true for a read, false for a write
     */
    void logControlInfo(bool receive) const;

    /**
     * Return the name of the alsa mixer control
     *
     * @return the name of the control
     */
    const string getControlName() const { return getFormattedMappingValue(); }

    /**
     * Get the parameter scalar size for elementary access
     *
     * @return the parameter scalar size
     */
    uint32_t getScalarSize() const { return _scalarSize; }

    /**
     * Checks if type Supported
     * Used to delay error about supported parameter types
     *
     * @return true if type is supported, false otherwise
     */
    bool isTypeSupported() const { return !_hasWrongElementTypeError; }

    /**
     * Set if type Supported
     * Used to delay error about supported parameter types
     *
     * @param[in] supported: Is pInstanceConfigurableElement type supported.
     */
    void setTypeIsSupported(bool supported) { _hasWrongElementTypeError = !supported; }

    /**
     * Is Debug Enabled
     *
     * @return true if debug is enabled, false otherwise
     */
    bool isDebugEnabled() const { return _isDebugEnabled; }

protected:
    /** Read an integer from the blackboard
     *
     * @return the read int
     */
    virtual int fromBlackboard();

    /** Write an integer to the blackboard
     *
     * @param[in] value the control value to write
     */
    virtual void toBlackboard(int value);

private:
    /**
     * Format control name
     * Builds the name of the alsa control from the mapping read in XML file
     *
     * @param[in] context read in xml file containing the alsa mixer control name
     */
    void formatControlName(const CMappingContext &context);

    /** Scalar parameter size for elementary access */
    uint32_t _scalarSize;
    /** Delayed error about supported parameter types */
    bool _hasWrongElementTypeError;
    /** Debug on */
    bool _isDebugEnabled;
};
