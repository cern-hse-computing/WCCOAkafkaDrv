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

#include "kafkaInt32Trans.hxx"

#include "kafkaHWMapper.hxx"

#include "REMUS/Logger.hxx"

#include <cmath>

#include <IntegerVar.hxx>

namespace Transformations {

TransformationType kafkaInt32Trans::isA() const {
    return (TransformationType) kafkaDrvInt32TransType;
}

TransformationType kafkaInt32Trans::isA(TransformationType type) const {
	if (type == isA())
		return type;
	else
		return Transformation::isA(type);
}

Transformation *kafkaInt32Trans::clone() const {
    return new kafkaInt32Trans;
}

int kafkaInt32Trans::itemSize() const {
	return size;
}

VariableType kafkaInt32Trans::getVariableType() const {
	return INTEGER_VAR;
}

PVSSboolean kafkaInt32Trans::toPeriph(PVSSchar *buffer, PVSSuint len, const Variable &var, const PVSSuint subix) const {

	if(var.isA() != INTEGER_VAR /* || subix >= Transformation::getNumberOfElements() */){
		ErrHdl::error(ErrClass::PRIO_SEVERE, // Data will be lost
				ErrClass::ERR_PARAM, // Wrong parametrization
				ErrClass::UNEXPECTEDSTATE, // Nothing else appropriate
                "kafkaInt32Trans", "toPeriph", // File and function name
				"Wrong variable type or wrong length: " + CharString(len) + ", subix: " + CharString(subix) // Unfortunately we don't know which DP
				);

		return PVSS_FALSE;
	}
	reinterpret_cast<int32_t *>(buffer)[subix] = reinterpret_cast<const IntegerVar &>(var).getValue();
	return PVSS_TRUE;
}

VariablePtr kafkaInt32Trans::toVar(const PVSSchar *buffer, const PVSSuint dlen, const PVSSuint subix) const {

    if(buffer == NULL || !dlen){
		ErrHdl::error(ErrClass::PRIO_SEVERE, // Data will be lost
				ErrClass::ERR_PARAM, // Wrong parametrization
				ErrClass::UNEXPECTEDSTATE, // Nothing else appropriate
                "kafkaInt32Trans", "toVar", // File and function name
				"Null buffer pointer or wrong length: " + CharString(dlen) // Unfortunately we don't know which DP
				);
	}
    else
    {
        try
        {
            int32_t i = std::stoi((char*)buffer);
            return new IntegerVar(i);
        }
        catch(std::exception& e)
        {
            REMUS::Logger::globalWarning(__PRETTY_FUNCTION__, e.what());
        }
    }

    return NULL;
}

}
