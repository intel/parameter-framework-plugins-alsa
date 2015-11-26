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
#include "AmixerControl.hpp"
#include "InstanceConfigurableElement.h"
#include "TypeElement.h"
#include "ParameterType.h"
#include "BitParameterBlockType.h"
#include "ParameterBlockType.h"
#include "MappingContext.h"
#include "AlsaMappingKeys.hpp"
#include <string.h>
#include <string>
#include <ctype.h>
#include <algorithm>

#define base AlsaSubsystemObject

AmixerControl::AmixerControl(const std::string &mappingValue,
                             CInstanceConfigurableElement *instanceConfigurableElement,
                             const CMappingContext &context,
                             core::log::Logger& logger)
    : base(mappingValue, instanceConfigurableElement, logger,
           AlsaAmend1,
           gNbAlsaAmends,
           context),
      _scalarSize(0),
      _hasWrongElementTypeError(false),
      _isDebugEnabled(context.iSet(AlsaDebugEnable))
{
    // Check we are able to handle elements (no exception support, defer the error)
    switch (instanceConfigurableElement->getType()) {

    case CInstanceConfigurableElement::EParameter:
    case CInstanceConfigurableElement::EBitParameterBlock:
    case CInstanceConfigurableElement::EComponent:
    case CInstanceConfigurableElement::EParameterBlock: {

        // Get actual element type
        const CTypeElement *element = instanceConfigurableElement->getTypeElement();

        // If the parameter is a scalar its array size is 0, not 1.
        _scalarSize = instanceConfigurableElement->getFootPrint() /
                      std::max(element->getArrayLength(), size_t{1});
        break;
    }
    default: {
        setTypeIsSupported(false);
    }
    }
}

AmixerControl::AmixerControl(const std::string &mappingValue,
                             CInstanceConfigurableElement *instanceConfigurableElement,
                             const CMappingContext &context,
                             core::log::Logger& logger,
                             uint32_t scalarSize)
    : base(mappingValue, instanceConfigurableElement, logger,
           AlsaAmend1,
           gNbAlsaAmends,
           context),
      _scalarSize(scalarSize),
      _hasWrongElementTypeError(false),
      _isDebugEnabled(context.iSet(AlsaDebugEnable))
{
}

void AmixerControl::logControlInfo(bool receive) const
{
    if (_isDebugEnabled) {

        std::string controlName = getFormattedMappingValue();
        info() << (receive ? "Reading" : "Writing")
               << " ALSA Element Instance: " << getConfigurableElement()->getPath()
               << "\t\t(Control Element: " << controlName << ")";
    }
}

int AmixerControl::fromBlackboard()
{
    int value = 0;

    blackboardRead(&value, getScalarSize());

    // Take care of sign extension
    return toPlainInteger(getConfigurableElement(), value);
}

void AmixerControl::toBlackboard(int value)
{
    blackboardWrite(&value, getScalarSize());
}
