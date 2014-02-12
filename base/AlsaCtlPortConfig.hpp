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
#include <stdint.h>

/**
 * Port configuration for alsa device class.
 * This class handles the configuration of an alsa device through the PFW. It will be derivated
 * for both Legacy alsa subsystem and tiny alsa subsystem.
 */
class AlsaCtlPortConfig : public AlsaSubsystemObject
{
protected:
    /** Mapped control structure */
    struct PortConfig
    {
        uint8_t isStreamEnabled[2]; /**< [1] == Capture stream, [0] == Playback stream */
        uint8_t format;            /**< S16LE,... */
        uint8_t channelNumber;     /**< 1 == Mono.. */
        uint16_t sampleRate;       /**< 16000, 48000... */
    } __attribute__((packed));

public:
    /** Structure to convert alsa formats in tinyalsa formats */
    struct FormatTranslation
    {
        uint8_t formatAsNumerical; /**< Tiny alsa format value */
        string formatAsString; /**< Litteral value */
    };

    AlsaCtlPortConfig(const string &mappingValue,
                      CInstanceConfigurableElement *instanceConfigurableElement,
                      const CMappingContext &context,
                      const PortConfig &defaultPortConfig);

    /** Invalid TinyAlsa format for the SFormatTranslation table */
    static const uint8_t _tinyAlsaFormatInvalid;

protected:
    // Sync to/from HW
    virtual bool receiveFromHW(string &error);
    virtual bool sendToHW(string &error);

    enum StreamDirection
    {
        Playback = 0, /**< Output stream */
        Capture = 1   /**< Input stream */
    };

    /** Stream direction enum element count */
    static const uint8_t _streamDirectionCount = Capture + 1;

    /**
     * Open a stream
     * This function is implemented in daughter classes to actually open a stream
     *
     * @param[in] streamDirection Either Capture or Playback
     * @param[out] error string containing the error in case of failure
     *
     * @return true or false in case of failure
     */
    virtual bool doOpenStream(StreamDirection streamDirection, string &error) = 0;

    /**
     * Close a stream
     * This function is implemented in daughter classes to actually close a stream
     *
     * @param[in] streamDirection Either Capture or Playback
     */
    virtual void doCloseStream(StreamDirection streamDirection) = 0;

    /**
     * Get device number.
     *
     * @return the alsa device number
     */
    uint32_t getDeviceNumber() const { return _device; }

    /**
     * Get port config.
     *
     * @return the port configuration structure
     */
    const PortConfig &getPortConfig() const { return _portConfig; }

    /**
     * Format an error string with alsa error.
     *
     * @param[in] streamDirection Either Capture or Playback
     * @param[in] functionName string containing the function where the error occurs
     * @param[in] error string containing the alsa error in case of failure
     *
     * @return the string containing the error formatted
     */
    string formatAlsaError(StreamDirection streamDirection,
                           const string &functionName,
                           const string &error);

private:
    /**
     * Close and re-open a stream to configure it if needed.
     *
     * @param[in] streamDirection Either Capture or Playback
     * @param[out] error string containing the alsa error in case of failure
     *
     * @return true or false in case of failure
     */
    bool updateStream(StreamDirection streamDirection, string &error);

    /**
     * Check if the stream is enabled.
     *
     * @param[in] streamDirection Either Capture or Playback
     *
     * @return true or false in case of failure
     */
    bool isStreamEnabled(StreamDirection streamDirection) const;

    /**
     * Open the stream if needed.
     * Check if the stream is not already open. and actually open it only if not already open.
     *
     * @param[in] streamDirection Either Capture or Playback
     * @param[out] strError the error string in case of error
     *
     * @return success/failure
     */
    bool openStream(StreamDirection streamDirection, string &error);

    /**
     * Close the stream if needed.
     * Check if the stream is not already closed. and actually close it only if not already closed.
     *
     * @param[in] streamDirection Either Capture or Playback
     */
    void closeStream(StreamDirection streamDirection);

    /**
     * Update needed check function.
     * check if a port configuration needs to be updated
     *
     * @param[in] portConfig the new port config structure
     *
     * @return success/failure
     */
    bool isDeviceUpdateNeeded(const PortConfig &portConfig) const;

    /** Device number */
    uint32_t _device;
    /** Port config structure*/
    PortConfig _portConfig;
};
