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

#include "kafkaTimeTrans.hxx"

#include "kafkaHWMapper.hxx"

#include "Common/Logger.hxx"

#include <cmath>

#include <TimeVar.hxx>

#include <sstream>

namespace Transformations{

TransformationType kafkaTimeTrans::isA() const {
    return (TransformationType) kafkaDrvTimeTransType;
}

TransformationType kafkaTimeTrans::isA(TransformationType type) const {
	if (type == isA())
		return type;
	else
		return Transformation::isA(type);
}

Transformation *kafkaTimeTrans::clone() const {
	return new kafkaTimeTrans;
}

int kafkaTimeTrans::itemSize() const {
	return size;
}

VariableType kafkaTimeTrans::getVariableType() const {
	return TIME_VAR;
}

PVSSboolean kafkaTimeTrans::toPeriph(PVSSchar *buffer, PVSSushort len,	const Variable &var, const PVSSushort subix) const {

	if(var.isA() != TIME_VAR){
		ErrHdl::error(ErrClass::PRIO_SEVERE, // Data will be lost
				ErrClass::ERR_PARAM, // Wrong parametrization
				ErrClass::UNEXPECTEDSTATE, // Nothing else appropriate
				"kafkaTimeTrans", "toPeriph", // File and function name
				"THis is not a time variable" // Unfortunately we don't know which DP
			);
		return PVSS_FALSE;
	}
	reinterpret_cast<int64_t *>(buffer)[subix] = reinterpret_cast<const TimeVar &>(var).getSeconds() * 1000 + reinterpret_cast<const TimeVar &>(var).getMilli();
	return PVSS_TRUE;
}

VariablePtr kafkaTimeTrans::toVar(const PVSSchar *buffer, const PVSSushort dlen, const PVSSushort subix) const {

    if(buffer == NULL || !dlen){
		ErrHdl::error(ErrClass::PRIO_SEVERE, // Data will be lost
				ErrClass::ERR_PARAM, // Wrong parametrization
				ErrClass::UNEXPECTEDSTATE, // Nothing else appropriate
				"kafkaTimeTrans", "toVar", // File and function name
				"Null buffer pointer or wrong length: " + CharString(dlen) // Unfortunately we don't know which DP
				);
	}
    else
    {
        unsigned int miliseconds;
        std::tm t;
        if (std::sscanf((char*)buffer, "%4d.%2d.%2d %2d:%2d:%2d.%3d", &t.tm_year, &t.tm_mon,
                   &t.tm_mday, &t.tm_hour, &t.tm_min, &t.tm_sec, &miliseconds) == 7)
        {
            t.tm_year -= 1900;
            t.tm_mon  -= 1;
            return new TimeVar(PVSSTime(mktime(&t), miliseconds));
        }
    }

    return NULL;
}
}//namesapce
