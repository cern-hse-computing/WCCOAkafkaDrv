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

#include "kafkaFloatTrans.hxx"

#include "kafkaHWMapper.hxx"

#include "REMUS/Logger.hxx"

#include <cmath>

#include <FloatVar.hxx>

namespace Transformations{

TransformationType kafkaFloatTrans::isA() const {
    return (TransformationType) kafkaDrvFloatTransType;
}

TransformationType kafkaFloatTrans::isA(TransformationType type) const {
	if (type == isA())
		return type;
	else
		return Transformation::isA(type);
}

Transformation *kafkaFloatTrans::clone() const {
	return new kafkaFloatTrans;
}

int kafkaFloatTrans::itemSize() const {
	return size;
}

VariableType kafkaFloatTrans::getVariableType() const {
	return FLOAT_VAR;
}

PVSSboolean kafkaFloatTrans::toPeriph(PVSSchar *buffer, PVSSuint len,	const Variable &var, const PVSSuint subix) const {

	if(var.isA() != FLOAT_VAR){
		ErrHdl::error(ErrClass::PRIO_SEVERE, // Data will be lost
				ErrClass::ERR_PARAM, // Wrong parametrization
				ErrClass::UNEXPECTEDSTATE, // Nothing else appropriate
				"kafkaFloatTrans", "toPeriph", // File and function name
				"Wrong variable type" // Unfortunately we don't know which DP
				);

		return PVSS_FALSE;
	}
	reinterpret_cast<float *>(buffer)[subix] = (float)(reinterpret_cast<const FloatVar &>(var)).getValue();
	return PVSS_TRUE;
}

VariablePtr kafkaFloatTrans::toVar(const PVSSchar *buffer, const PVSSuint dlen, const PVSSuint subix) const {

    if(buffer == NULL || !dlen/*|| dlen%size > 0 || dlen < size*(subix+1)*/){
		ErrHdl::error(ErrClass::PRIO_SEVERE, // Data will be lost
				ErrClass::ERR_PARAM, // Wrong parametrization
				ErrClass::UNEXPECTEDSTATE, // Nothing else appropriate
				"kafkaFloatTrans", "toVar", // File and function name
				"Null buffer pointer or wrong length: " + CharString(dlen) // Unfortunately we don't know which DP
				);
	}
    else
    {
        try
        {
            float f = std::stof((char*)buffer);
            return new FloatVar(f);
        }
        catch(std::exception& e)
        {
            REMUS::Logger::globalWarning(__PRETTY_FUNCTION__, e.what());
        }
    }

    return NULL;
}

}//namespace
