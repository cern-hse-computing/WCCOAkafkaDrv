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

#include "Logger.hxx"
#include "REMUS/Constants.hxx"
#include <mutex>

namespace REMUS {

int Logger::loggingLevel = 2;
const char * Logger::timestrformat = "%a, %d.%m.%Y %H:%M:%S";

void Logger::globalInfo(int lvl, const char *note1, const char* note2, const char* note3){
	if(loggingLevel > 0 && loggingLevel >= lvl){
		ErrHdl::error(
				ErrClass::PRIO_INFO,
				ErrClass::ERR_CONTROL,
				ErrClass::NOERR,
				note1,
				note2,
				note3);
	}
}

void Logger::globalWarning(const char *note1, const char* note2, const char* note3){
	if(loggingLevel > L0){
		ErrHdl::error(
				ErrClass::PRIO_WARNING,
				ErrClass::ERR_CONTROL,
				ErrClass::NOERR,
				note1,
				note2,
				note3);
	}
}

void Logger::globalError(const char *note1, const char* note2, const char* note3){
	if(loggingLevel > L0){
		ErrHdl::error(
				ErrClass::PRIO_FATAL,
				ErrClass::ERR_CONTROL,
				ErrClass::NOERR,
				note1,
				note2,
				note3);
	}
}
}
