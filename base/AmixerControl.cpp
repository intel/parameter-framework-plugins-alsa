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
#include "AmixerControl.hpp"
#include "InstanceConfigurableElement.h"
#include "TypeElement.h"
#include "ParameterType.h"
#include "BitParameterBlockType.h"
#include "ParameterBlockType.h"
#include "MappingContext.h"
#include "AmixerMappingKeys.hpp"
#include "AutoLog.h"
#include <string.h>
#include <ctype.h>
#include <algorithm>

#define base AlsaSubsystemObject

AmixerControl::AmixerControl(const string &mappingValue,
                             CInstanceConfigurableElement *instanceConfigurableElement,
                             const CMappingContext &context)
    : base(mappingValue, instanceConfigurableElement,
           AmixerAmend1,
           (AmixerAmendEnd - AmixerAmend1 + 1),
           context),
      _scalarSize(0),
      _hasWrongElementTypeError(false),
      _isDebugEnabled(context.iSet(AmixerDebugEnable)),
      _hasIndex(context.iSet(AmixerIndex)),
      _index(context.getItemAsInteger(AmixerIndex))
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
           AmixerAmend1,
           (AmixerAmendEnd - AmixerAmend1 + 1),
           context),
      _scalarSize(scalarSize),
      _hasWrongElementTypeError(false),
      _isDebugEnabled(context.iSet(AmixerDebugEnable)),
      _hasIndex(context.iSet(AmixerIndex)),
      _index(context.getItemAsInteger(AmixerIndex))
{
}

void AmixerControl::logControlInfo(bool receive) const
{
    if (_isDebugEnabled) {

        string controlName = getFormattedMappingValue();
        log_info("%s AMIXER Element Instance: %s\t\t(Control Element: %s, index:%d)",
                 receive ? "Reading" : "Writing",
                 getConfigurableElement()->getPath().c_str(), controlName.c_str(),
                 _hasIndex ? _index : 0);
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
