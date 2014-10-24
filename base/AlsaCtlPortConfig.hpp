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
#include <stdint.h>
#include <string>

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
        std::string formatAsString; /**< Litteral value */
    };

    AlsaCtlPortConfig(const std::string &mappingValue,
                      CInstanceConfigurableElement *instanceConfigurableElement,
                      const CMappingContext &context,
                      const PortConfig &defaultPortConfig);

    /** Invalid TinyAlsa format for the SFormatTranslation table */
    static const uint8_t _tinyAlsaFormatInvalid;

protected:
    // Sync to/from HW
    virtual bool receiveFromHW(std::string &error);
    virtual bool sendToHW(std::string &error);

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
    virtual bool doOpenStream(StreamDirection streamDirection, std::string &error) = 0;

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
    std::string formatAlsaError(StreamDirection streamDirection,
                           const std::string &functionName,
                           const std::string &error);

private:
    /**
     * Close and re-open a stream to configure it if needed.
     *
     * @param[in] streamDirection Either Capture or Playback
     * @param[out] error string containing the alsa error in case of failure
     *
     * @return true or false in case of failure
     */
    bool updateStream(StreamDirection streamDirection, std::string &error);

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
    bool openStream(StreamDirection streamDirection, std::string &error);

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
