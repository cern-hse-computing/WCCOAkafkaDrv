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

#include "kafkaBoolTrans.hxx"

#include "kafkaHWMapper.hxx"

#include "Common/Logger.hxx"

#include <cmath>

#include "BitVar.hxx"

namespace Transformations{

TransformationType kafkaBoolTrans::isA() const {
    return (TransformationType) kafkaDrvBoolTransType;
}

TransformationType kafkaBoolTrans::isA(TransformationType type) const {
	if (type == isA())
		return type;
	else
		return Transformation::isA(type);
}

Transformation *kafkaBoolTrans::clone() const {
    return new kafkaBoolTrans;
}

int kafkaBoolTrans::itemSize() const {
	return size;
}

VariableType kafkaBoolTrans::getVariableType() const {
	return BIT_VAR;
}

PVSSboolean kafkaBoolTrans::toPeriph(PVSSchar *buffer, PVSSuint len,	const Variable &var, const PVSSuint subix) const {

	if(var.isA() != BIT_VAR){
		ErrHdl::error(ErrClass::PRIO_SEVERE, // Data will be lost
				ErrClass::ERR_PARAM, // Wrong parametrization
				ErrClass::UNEXPECTEDSTATE, // Nothing else appropriate
                "kafkaBoolTrans", "toPeriph", // File and function name
				"Wrong variable type "  + CharString(subix)// Unfortunately we don't know which DP
				);
		return PVSS_FALSE;
	}

	*reinterpret_cast<bool *>(buffer) = (reinterpret_cast<const BitVar&>(var)).getValue();

	return PVSS_TRUE;
}

VariablePtr kafkaBoolTrans::toVar(const PVSSchar *buffer, const PVSSuint dlen, const PVSSuint subix) const {

    if(!dlen || buffer == NULL)
    {

        ErrHdl::error(ErrClass::PRIO_SEVERE, // Data will be lost
                ErrClass::ERR_PARAM, // Wrong parametrization
                ErrClass::UNEXPECTEDSTATE, // Nothing else appropriate
                "kafkaBoolTrans", "toVar", // File and function name
                "Wrong buffer" // Unfortunately we don't know which DP
                );
    }
    else
    {
        if(!strcmp((const char*)buffer,"FALSE"))
            return new BitVar(false);
        else if(!strcmp((const char*)buffer,"TRUE"))
            return new BitVar(true);
        else
        {
            Common::Logger::globalWarning(__PRETTY_FUNCTION__, "Unable to parse boolean for buffer:", (const char*)buffer);
        }
    }
    return NULL;
}

}//namespace
