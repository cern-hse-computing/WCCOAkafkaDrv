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

#include "kafkaUint8Trans.hxx"

#include "kafkaHWMapper.hxx"

#include "REMUS/Logger.hxx"

#include <cmath>

#include <IntegerVar.hxx>

namespace Transformations{

TransformationType kafkaUint8Trans::isA() const {
    return (TransformationType) kafkaDrvUint8TransType;
}

TransformationType kafkaUint8Trans::isA(TransformationType type) const {
	if (type == isA())
		return type;
	else
		return Transformation::isA(type);
}

Transformation *kafkaUint8Trans::clone() const {
    return new kafkaUint8Trans;
}

int kafkaUint8Trans::itemSize() const {
	return size;
}

VariableType kafkaUint8Trans::getVariableType() const {
	return INTEGER_VAR;
}

PVSSboolean kafkaUint8Trans::toPeriph(PVSSchar *buffer, PVSSuint len,	const Variable &var, const PVSSuint subix) const {

	if(var.isA() != INTEGER_VAR){
		ErrHdl::error(ErrClass::PRIO_SEVERE, // Data will be lost
				ErrClass::ERR_PARAM, // Wrong parametrization
				ErrClass::UNEXPECTEDSTATE, // Nothing else appropriate
                "kafkaUint8Trans", "toPeriph", // File and function name
				"Wrong variable type" // Unfortunately we don't know which DP
				);

		return PVSS_FALSE;
	}
	// this one is a bit special as the number is handled by wincc oa as int32, but we handle it as 8 bit unsigned integer
	// thus any info above the 8 first bits is lost
	*(reinterpret_cast<uint8_t *>(buffer + (subix * size))) = (uint8_t)(reinterpret_cast<const IntegerVar &>(var)).getValue();
	return PVSS_TRUE;
}

VariablePtr kafkaUint8Trans::toVar(const PVSSchar *buffer, const PVSSuint dlen, const PVSSuint subix) const {

    if(buffer == NULL || !dlen){
		ErrHdl::error(ErrClass::PRIO_SEVERE, // Data will be lost
				ErrClass::ERR_PARAM, // Wrong parametrization
				ErrClass::UNEXPECTEDSTATE, // Nothing else appropriate
                "kafkaUint8Trans", "toVar", // File and function name
				"Null buffer pointer or wrong length: " + CharString(dlen) // Unfortunately we don't know which DP
				);

	}
    else
    {
        try
        {
            uint8_t c = std::stoul((char*)buffer);
            return new IntegerVar(c);
        }
        catch(std::exception& e)
        {
            REMUS::Logger::globalWarning(__PRETTY_FUNCTION__, e.what());
        }

    }

    return NULL;
}

}//namespace
