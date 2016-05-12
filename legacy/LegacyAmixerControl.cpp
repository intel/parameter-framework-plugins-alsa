/*
 * Copyright (c) 2011-2015, Intel Corporation
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
#include "LegacyAmixerControl.hpp"
#include "InstanceConfigurableElement.h"
#include "ParameterType.h"
#include "BitParameterBlockType.h"
#include "MappingContext.h"
#include "AlsaMappingKeys.hpp"
#include <convert.hpp>
#include <assert.h>
#include <string.h>
#include <string>
#include <vector>
#include <errno.h>
#include <ctype.h>
#include <alsa/asoundlib.h>
#include <sstream>
#include <numeric>

/* from sound/asound.h, header is not compatible with alsa/asoundlib.h
 */
struct snd_ctl_tlv {
    unsigned int numid;     /* control element numeric identification */
    unsigned int length;    /* in bytes aligned to 4 */
    unsigned char tlv[];    /* first TLV */
};


#define base AmixerControl

LegacyAmixerControl::LegacyAmixerControl(
    const std::string &mappingValue,
    CInstanceConfigurableElement *instanceConfigurableElement,
    const CMappingContext &context,
    core::log::Logger& logger)
    : base(mappingValue, instanceConfigurableElement, context, logger)
{

}

bool LegacyAmixerControl::accessHW(bool receive, std::string &error)
{
#ifdef SIMULATION
    if (receive) {

        memset(getBlackboardLocation(), 0, getSize());
    }
    logControlInfo(receive);

    return true;
#endif

    int ret;
    // Mixer handle
    snd_ctl_t *sndCtrl;
    uint32_t value;
    uint32_t index;
    uint32_t elementCount;
    snd_ctl_elem_id_t *id;
    snd_ctl_elem_info_t *info;
    snd_ctl_elem_value_t *control;

    logControlInfo(receive);

    // Check parameter type is ok (deferred error, no exceptions available :-()
    if (!isTypeSupported()) {

        error = "Parameter type not supported.";

        return false;
    }

    int cardNumber = getCardNumber();

    if (cardNumber < 0) {

        error = "Card " + getCardName() + " not found. Error: " + strerror(cardNumber);

        return false;
    }
    // Create device name
    std::ostringstream deviceName;

    deviceName << "hw:" << cardNumber;

    // Open sound control
    if ((ret = snd_ctl_open(&sndCtrl, deviceName.str().c_str(), 0)) < 0) {

        error = snd_strerror(ret);

        return false;
    }

    // Allocate in stack
    snd_ctl_elem_id_alloca(&id);
    snd_ctl_elem_info_alloca(&info);
    snd_ctl_elem_value_alloca(&control);

    // Set interface
    snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_MIXER);

    std::string controlName = getControlName();

    // Set name or id
    if (isdigit(controlName[0])) {

        unsigned int controlId = 0;
        // TODO: error checking
        convertTo(controlName, controlId);
        snd_ctl_elem_id_set_numid(id, controlId);
    } else {

        snd_ctl_elem_id_set_name(id, controlName.c_str());
    }
    // Init info id
    snd_ctl_elem_info_set_id(info, id);

    // Get info
    if ((ret = snd_ctl_elem_info(sndCtrl, info)) < 0) {

        error = "ALSA: Unable to get element info " + controlName +
                ": " + snd_strerror(ret);

        // Close sound control
        snd_ctl_close(sndCtrl);

        return false;
    }
    // Get type
    snd_ctl_elem_type_t eType = snd_ctl_elem_info_get_type(info);

    // Get element count
    elementCount = snd_ctl_elem_info_get_count(info);

    uint32_t scalarSize = getScalarSize();
    // For Bytes control force scalar size to 1 byte
    if (eType == SND_CTL_ELEM_TYPE_BYTES) {
        scalarSize = 1;
    }

    // If size defined in the PFW different from alsa mixer control size, return an error
    if (elementCount * scalarSize != getSize()) {

        error = "ALSA: Control element count (" + std::to_string(elementCount) +
                ") and configurable scalar element count (" +
                std::to_string(getSize() / scalarSize) + ") mismatch";

        // Close sound control
        snd_ctl_close(sndCtrl);

        return false;
    }
    // Set value id
    snd_ctl_elem_value_set_id(control, id);

    if (receive) {

        // Special hook for TLV Bytes Control
        if ((eType == SND_CTL_ELEM_TYPE_BYTES) &&
          snd_ctl_elem_info_is_tlv_readable(info)) {

            std::vector<unsigned char> rawTlv(sizeof(struct snd_ctl_tlv) + elementCount);

            struct snd_ctl_tlv *tlv = reinterpret_cast<struct snd_ctl_tlv *>(rawTlv.data());

            ret = snd_ctl_elem_tlv_read(sndCtrl, id, reinterpret_cast<unsigned int *>(tlv),
                                        rawTlv.size());
            if (ret < 0) {

                error = "ALSA: Unable to read element " + controlName +
                        ": " + snd_strerror(ret);

            } else {
                blackboardWrite(tlv->tlv, elementCount);
            }

            // Close sound control
            snd_ctl_close(sndCtrl);

            return ret == 0;
        }

        // Read element
        if ((ret = snd_ctl_elem_read(sndCtrl, control)) < 0) {

            error = "ALSA: Unable to read element " + controlName +
                    ": " + snd_strerror(ret);

            // Close sound control
            snd_ctl_close(sndCtrl);

            return false;
        }

        if (eType == SND_CTL_ELEM_TYPE_BYTES) {
            const void *data = snd_ctl_elem_value_get_bytes(control);

            if (isDebugEnabled()) {
                const unsigned char *first = reinterpret_cast<const unsigned char *>(data);
                const unsigned char *last = first + elementCount;

                // The "info" method has been shadowed by a local variable
                this->info() << "Reading alsa element " << controlName << ": "
                             << std::accumulate(first, last, std::string{},
                    [](const std::string& a, std::vector<unsigned char>::value_type b) {
                        return a.empty() ? std::to_string(b) : a + ',' + std::to_string(b);
                    });
            }

            blackboardWrite(data, elementCount);

        } else {

            // Go through all indexes
            for (index = 0; index < elementCount; index++) {

                switch (eType) {
                case SND_CTL_ELEM_TYPE_BOOLEAN:
                    value = snd_ctl_elem_value_get_boolean(control, index);
                    break;
                case SND_CTL_ELEM_TYPE_INTEGER:
                    value = snd_ctl_elem_value_get_integer(control, index);
                    break;
                case SND_CTL_ELEM_TYPE_INTEGER64:
                    value = snd_ctl_elem_value_get_integer64(control, index);
                    break;
                case SND_CTL_ELEM_TYPE_ENUMERATED:
                    value = snd_ctl_elem_value_get_enumerated(control, index);
                    break;
                default:
                    error = "ALSA: Unknown control element type while reading alsa element " +
                            controlName;
                    return false;
                }

                if (isDebugEnabled()) {

                    // The "info" method has been shadowed by a local variable
                    this->info() << "Reading alsa element " << controlName
                                 << ", index " << index << " with value " << value;
                }

                // Write data to blackboard (beware this code is OK on Little Endian machines only)
                toBlackboard(value);
            }
        }

    } else {

        // Special hook for TLV Bytes Control
        if ((eType == SND_CTL_ELEM_TYPE_BYTES) &&
            snd_ctl_elem_info_is_tlv_writable(info)) {

            std::vector<unsigned char> rawTlv(sizeof(struct snd_ctl_tlv) + elementCount);

            struct snd_ctl_tlv *tlv = reinterpret_cast<struct snd_ctl_tlv *>(rawTlv.data());

            tlv->numid = 0;
            tlv->length = elementCount;

            blackboardRead(tlv->tlv, elementCount);

            ret = snd_ctl_elem_tlv_write(sndCtrl, id, reinterpret_cast<unsigned int *>(tlv));
            if (ret < 0) {

                error = "ALSA: Unable to write element " + controlName +
                        ": " + snd_strerror(ret);
            }

            // Close sound control
            snd_ctl_close(sndCtrl);

            return ret == 0;
        }

        if (eType == SND_CTL_ELEM_TYPE_BYTES) {
            std::vector<unsigned char> rawData(elementCount);

            blackboardRead(rawData.data(), elementCount);

            if (isDebugEnabled()) {

                // The "info" method has been shadowed by a local variable
                this->info() << "Writing alsa element " << controlName << ": "
                             << std::accumulate(begin(rawData), end(rawData), std::string{},
                    [](const std::string& a, std::vector<unsigned char>::value_type b) {
                        return a.empty() ? std::to_string(b) : a + ',' + std::to_string(b);
                    });

            }

            snd_ctl_elem_set_bytes(control, rawData.data(), elementCount);

        } else {
            // Go through all indexes
            for (index = 0; index < elementCount; index++) {

                // Read data from blackboard (beware this code is OK on Little Endian machines only)
                value = fromBlackboard();

                if (isDebugEnabled()) {

                    // The "info" method has been shadowed by a local variable
                    this->info() << "Writing alsa element " << controlName
                                 << ", index " << index << " with value " << value;
                }

                switch (eType) {
                case SND_CTL_ELEM_TYPE_BOOLEAN:
                    snd_ctl_elem_value_set_boolean(control, index, value);
                    break;
                case SND_CTL_ELEM_TYPE_INTEGER:
                    snd_ctl_elem_value_set_integer(control, index, value);
                    break;
                case SND_CTL_ELEM_TYPE_INTEGER64:
                    snd_ctl_elem_value_set_integer64(control, index, value);
                    break;
                case SND_CTL_ELEM_TYPE_ENUMERATED:
                    snd_ctl_elem_value_set_enumerated(control, index, value);
                    break;
                default:
                    error = "ALSA: Unknown control element type while writing alsa element " +
                        controlName;
                    return false;
                }
            }
        }

        // Write element
        if ((ret = snd_ctl_elem_write(sndCtrl, control)) < 0) {

            error = "ALSA: Unable to write element " + controlName +
                    ": " + snd_strerror(ret);


            // Close sound control
            snd_ctl_close(sndCtrl);

            return false;
        }
    }
    // Close sound control
    snd_ctl_close(sndCtrl);

    return true;
}
