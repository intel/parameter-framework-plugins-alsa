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
#include "AlsaSubsystemObject.hpp"
#include "MappingContext.h"
#include "AmixerMappingKeys.hpp"

#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define base CFormattedSubsystemObject

const char AlsaSubsystemObject::_soundCardPath[] = "/proc/asound/";

AlsaSubsystemObject::AlsaSubsystemObject(const string &mappingValue,
                                         CInstanceConfigurableElement *instanceConfigurableElement,
                                         const CMappingContext &context)
    : base(instanceConfigurableElement, mappingValue),
      _cardName(context.getItem(AmixerCard)),
      _cardIndex(getCardNumberByName(context.getItem(AmixerCard)))
{

}

AlsaSubsystemObject::AlsaSubsystemObject(const string &mappingValue,
                                         CInstanceConfigurableElement *instanceConfigurableElement,
                                         uint32_t firstAmendKey,
                                         uint32_t nbAmendKeys,
                                         const CMappingContext &context)
    : base(instanceConfigurableElement, mappingValue, firstAmendKey, nbAmendKeys, context),
      _cardName(context.getItem(AmixerCard)),
      _cardIndex(getCardNumberByName(context.getItem(AmixerCard)))
{

}
int32_t AlsaSubsystemObject::getCardNumberByName(const string &cardName)
{
    string idFilePath;
    char numberFilepath[PATH_MAX] = "";
    ssize_t writtenSize;

    // Compute card path (Example: /proc/asound/cloverviewaudio)
    idFilePath = string(_soundCardPath) + cardName;

    // Read corresponding link (Example: card5)
    writtenSize = readlink(idFilePath.c_str(), numberFilepath, sizeof(numberFilepath));

    if (writtenSize < 0) {

        // Sound card does not exist
        return -errno;
    }

    if (static_cast<size_t>(writtenSize) >= sizeof(numberFilepath)) {

        // buffer too small
        return -ENAMETOOLONG;
    }

    // Extract card number from link (Example: 5 from card5)
    return asInteger(numberFilepath + strlen("card"));

}
