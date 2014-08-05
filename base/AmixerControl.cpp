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
#include "AmixerControl.hpp"
#include "InstanceConfigurableElement.h"
#include "TypeElement.h"
#include "ParameterType.h"
#include "BitParameterBlockType.h"
#include "ParameterBlockType.h"
#include "MappingContext.h"
#include "AlsaMappingKeys.hpp"
#include "AutoLog.h"
#include <string.h>
#include <ctype.h>
#include <algorithm>

#define base AlsaSubsystemObject

AmixerControl::AmixerControl(const string &mappingValue,
                             CInstanceConfigurableElement *instanceConfigurableElement,
                             const CMappingContext &context)
    : base(mappingValue, instanceConfigurableElement,
           AlsaAmend1,
           gNbAlsaAmends,
           context),
      _scalarSize(0),
      _hasWrongElementTypeError(false),
      _isDebugEnabled(context.iSet(AlsaDebugEnable))
{
    // Check we are able to handle elements (no exception support, defer the error)
    switch (instanceConfigurableElement->getType()) {
    case CInstanceConfigurableElement::EParameter: {

        // Get actual element type
        const CParameterType *parameterType = static_cast<const CParameterType *>(
            instanceConfigurableElement->getTypeElement());

        // Get scalar parameter size
        // this size indicates elementary size, regardless of ArrayLength
        _scalarSize = parameterType->getSize();
        break;
    }
    case CInstanceConfigurableElement::EBitParameterBlock: {

        // Get actual element type
        const CBitParameterBlockType *bitParameterBlockType =
            static_cast<const CBitParameterBlockType *>(
                instanceConfigurableElement->getTypeElement());

        // Get scalar parameter size
        // this size indicates elementary size, regardless of ArrayLength
        _scalarSize = bitParameterBlockType->getSize();
        break;
    }
    case CInstanceConfigurableElement::EParameterBlock: {

        // Get actual element type
        const CParameterBlockType *parameterType = static_cast<const CParameterBlockType *>(
            instanceConfigurableElement->getTypeElement());

        // If the parameter is a scalar its array size is 0, not 1.
        _scalarSize = instanceConfigurableElement->getFootPrint() /
                      std::max(parameterType->getArrayLength(), 1U);
        break;
    }
    default: {
        setTypeIsSupported(false);
    }
    }
}

AmixerControl::AmixerControl(const string &mappingValue,
                             CInstanceConfigurableElement *instanceConfigurableElement,
                             const CMappingContext &context, uint32_t scalarSize)
    : base(mappingValue, instanceConfigurableElement,
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

        string controlName = getFormattedMappingValue();
        log_info("%s ALSA Element Instance: %s\t\t(Control Element: %s)",
                 receive ? "Reading" : "Writing",
                 getConfigurableElement()->getPath().c_str(), controlName.c_str());
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
