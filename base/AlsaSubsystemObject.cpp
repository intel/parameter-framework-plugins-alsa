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
#include "AlsaSubsystemObject.hpp"
#include "MappingContext.h"
#include "AlsaMappingKeys.hpp"
#include <convert.hpp>

#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <string>

using std::string;

#define base CFormattedSubsystemObject

const char AlsaSubsystemObject::_soundCardPath[] = "/proc/asound/";

AlsaSubsystemObject::AlsaSubsystemObject(const string &mappingValue,
                                         CInstanceConfigurableElement *instanceConfigurableElement,
                                         const CMappingContext &context,
                                         core::log::Logger& logger)
    : base(instanceConfigurableElement, logger, mappingValue),
      _cardName(context.getItem(AlsaCard)),
      _cardIndex(getCardNumberByName(context.getItem(AlsaCard)))
{

}

AlsaSubsystemObject::AlsaSubsystemObject(const string &mappingValue,
                                         CInstanceConfigurableElement *instanceConfigurableElement,
                                         core::log::Logger& logger,
                                         uint32_t firstAmendKey,
                                         uint32_t nbAmendKeys,
                                         const CMappingContext &context)
    : base(instanceConfigurableElement, logger, mappingValue, firstAmendKey, nbAmendKeys, context),
      _cardName(context.getItem(AlsaCard)),
      _cardIndex(getCardNumberByName(context.getItem(AlsaCard)))
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
    int32_t cardNumber = 0;
    if (convertTo(numberFilepath + strlen("card"), cardNumber)) {
        return cardNumber;
    }

    return -1; // A negative value indicates a failure
}
