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

#include "FormattedSubsystemObject.h"
#include <stdint.h>

/**
 * Alsa subsystem object class.
 * This class handles an alsa card, this is the base class for all alsa parameters.
 */
class AlsaSubsystemObject : public CFormattedSubsystemObject
{
public:
    AlsaSubsystemObject(const string &mappingValue,
                        CInstanceConfigurableElement *instanceConfigurableElement,
                        const CMappingContext &context);
    AlsaSubsystemObject(const string &strMappingValue,
                        CInstanceConfigurableElement *instanceConfigurableElement,
                        uint32_t firstAmendKey,
                        uint32_t nbAmendKeys,
                        const CMappingContext &context);

protected:
    /**
     * Get card number
     *
     * @return the number of the alsa card
     */
    int32_t getCardNumber() const { return _cardIndex; }

    /**
     * Get card name
     *
     * @return the name of the alsa card
     */
    const string &getCardName() const { return _cardName; }

private:
    /**
     * This function return the card number associated with the card ID (name) passed as argument
     *
     * @param[in] cardName an alsa card name
     *
     * @return the number of the corresponding alsa card
     */
    static int getCardNumberByName(const string &cardName);

    /** Path of the sound card in the file system */
    static const char _soundCardPath[];
    /** Card name to which the Alsa device belong */
    string _cardName;
    /** Card Index to which the Alsa device belong */
    int32_t _cardIndex;
};
