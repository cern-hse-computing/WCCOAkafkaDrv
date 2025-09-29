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

#include "kafkaInt64Trans.hxx"

#include "kafkaHWMapper.hxx"

#include "Common/Logger.hxx"

#include <cmath>

#include <IntegerVar.hxx>

namespace Transformations {

TransformationType kafkaInt64Trans::isA() const {
    return (TransformationType) kafkaDrvInt64TransType;
}

TransformationType kafkaInt64Trans::isA(TransformationType type) const {
	if (type == isA())
		return type;
	else
		return Transformation::isA(type);
}

Transformation *kafkaInt64Trans::clone() const {
    return new kafkaInt64Trans;
}

int kafkaInt64Trans::itemSize() const {
	return size;
}

VariableType kafkaInt64Trans::getVariableType() const {
	return INTEGER_VAR;
}

PVSSboolean kafkaInt64Trans::toPeriph(PVSSchar *buffer, PVSSushort len, const Variable &var, const PVSSushort subix) const {

	if(var.isA() != INTEGER_VAR /* || subix >= Transformation::getNumberOfElements() */){
		ErrHdl::error(ErrClass::PRIO_SEVERE, // Data will be lost
				ErrClass::ERR_PARAM, // Wrong parametrization
				ErrClass::UNEXPECTEDSTATE, // Nothing else appropriate
                "kafkaInt64Trans", "toPeriph", // File and function name
				"Wrong variable type or wrong length: " + CharString(len) + ", subix: " + CharString(subix) // Unfortunately we don't know which DP
				);

		return PVSS_FALSE;
	}
	reinterpret_cast<int64_t *>(buffer)[subix] = reinterpret_cast<const IntegerVar &>(var).getValue();
	return PVSS_TRUE;
}

VariablePtr kafkaInt64Trans::toVar(const PVSSchar *buffer, const PVSSushort dlen, const PVSSushort subix) const {

    if(buffer == NULL || !dlen){
		ErrHdl::error(ErrClass::PRIO_SEVERE, // Data will be lost
				ErrClass::ERR_PARAM, // Wrong parametrization
				ErrClass::UNEXPECTEDSTATE, // Nothing else appropriate
                "kafkaInt64Trans", "toVar", // File and function name
				"Null buffer pointer or wrong length: " + CharString(dlen) // Unfortunately we don't know which DP
				);
	}
    else
    {
        try
        {
            int64_t ll = std::stoll((char*)buffer);
            return new IntegerVar(ll);
        }
        catch(std::exception& e)
        {
            Common::Logger::globalWarning(__PRETTY_FUNCTION__, e.what());
        }
    }
    return NULL;

}

}
