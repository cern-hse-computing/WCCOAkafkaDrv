/** © Copyright 2019 CERN
 *
 * This software is distributed under the terms of the
 * GNU Lesser General Public Licence version 3 (LGPL Version 3),
 * copied verbatim in the file “LICENSE”
 *
 * In applying this licence, CERN does not waive the privileges
 * and immunities granted to it by virtue of its status as an
 * Intergovernmental Organization or submit itself to any jurisdiction.
 *
 * Author: Alexandru Savulescu (HSE-CEN-CO)
 *
 **/

// Our transformation class PVSS <--> Hardware
#include "kafkaStringTrans.hxx"
#include <ErrHdl.hxx>     // The Error handler Basics/Utilities
#include <TextVar.hxx>


#include "kafkaHWMapper.hxx"

#include "Common/Logger.hxx"

//----------------------------------------------------------------------------
namespace Transformations {


//kafkaStringTrans::kafkaStringTrans() : Transformation() { }

TransformationType kafkaStringTrans::isA() const
{
  return (TransformationType) kafkaDrvStringTransType;
}

TransformationType kafkaStringTrans::isA(TransformationType type) const {
    if (type == isA())
        return type;
    else
        return Transformation::isA(type);
}



//----------------------------------------------------------------------------

Transformation *kafkaStringTrans::clone() const
{
  return new kafkaStringTrans;
}

//----------------------------------------------------------------------------
// Our item size. The max we will use is 256 Bytes.
// This is an arbitrary value! A Transformation for a long e.g. would return 4

int kafkaStringTrans::itemSize() const
{
  // TODO - check maximum possible size
  return _size;
}

//----------------------------------------------------------------------------
// Our preferred Variable type. Data will be converted to this type
// before toPeriph is called.

VariableType kafkaStringTrans::getVariableType() const
{
  return TEXT_VAR;
}

//----------------------------------------------------------------------------
// Convert data from PVSS to Hardware.

PVSSboolean kafkaStringTrans::toPeriph(PVSSchar *buffer, PVSSushort len,
                                      const Variable &var, const PVSSushort subix) const
{

  // Be paranoic, check variable type
  if ( var.isA() != TEXT_VAR )
  {
    // Throw error message
    ErrHdl::error(
      ErrClass::PRIO_SEVERE,             // Data will be lost
      ErrClass::ERR_PARAM,               // Wrong parametrization
      ErrClass::UNEXPECTEDSTATE,         // Nothing else appropriate
      "kafkaStringTrans", "toPeriph",     // File and function name
      "Wrong variable type for data"     // Unfortunately we don't know which DP
    );

    return PVSS_FALSE;
  }

  // Check data len. TextVar::getString returns a CharString
  const TextVar& tv = static_cast<const TextVar &>(var);
  if (len < tv.getString().len() + 1)
  {
    // Throw error message
    ErrHdl::error(
      ErrClass::PRIO_SEVERE,             // Data will be lost
      ErrClass::ERR_IMPL,                // Mus be implementation
      ErrClass::UNEXPECTEDSTATE,         // Nothing else appropriate
      "kafkaStringTrans::toPeriph",       // File and function name
      "Data buffer too small; need:" +
      CharString(tv.getString().len() + 1) +
      " have:" + CharString(len)
    );

    return PVSS_FALSE;
  }

  if(tv.getString().len() > _size){
      ErrHdl::error(ErrClass::PRIO_SEVERE, // Data will be lost
              ErrClass::ERR_PARAM, // Wrong parametrization
              ErrClass::UNEXPECTEDSTATE, // Nothing else appropriate
              "kafkaStringTrans::toPeriph",       // File and function name
              "String too long" // Unfortunately we don't know which DP
              );

      return PVSS_FALSE;
  }

  sprintf((char*)buffer, "%s", tv.getValue());

  return PVSS_TRUE;
}

//----------------------------------------------------------------------------
// Conversion from Hardware to PVSS

VariablePtr kafkaStringTrans::toVar(const PVSSchar *buffer, const PVSSushort dlen,
                                   const PVSSushort /* subix */) const
{

  return new TextVar((const char *)buffer, dlen);
}

//----------------------------------------------------------------------------
}
