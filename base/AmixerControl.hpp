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

    /**
     * Does this control has an index
     *
     * @return true if the control has an index, false otherwise
     */
    bool hasIndex() const { return _hasIndex; }

    /**
     * Returns the index of the control
     *
     * @return the index
     */
    uint32_t getIndex() const { return _index; }

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
    /** An index can be used to complete the control name. It is possible to have multiple alsa
     *  controls with the same name, but with a different index. In this case, the control value
     *  can be accessed through the pair {control name,control index} */
    bool _hasIndex;
    /** The value of the control's index, 0 by default. */
    uint32_t _index;
};
