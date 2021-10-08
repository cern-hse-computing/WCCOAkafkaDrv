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

#ifndef DEBUGMETHODS_HXX
#define DEBUGMETHODS_HXX

#include <iostream>
#include <Resources.hxx>

#include <ErrHdl.hxx>
#include <ErrClass.hxx>

#include <mutex>

using std::mutex;
using std::lock_guard;

namespace Common {

/*!
 * \class Debug
 * \brief Class is handling writing to starndard output debuge informations provided by user.
 * Class is singleton and has overloaded operator << ().
 *
 * \see operator <<()
 */
class Logger{
public:
    Logger() : creationTime(0), logNum(0), devNum(0), prefix(""){
	};

    Logger(int devNum):creationTime(0), logNum(0), devNum(devNum){
		updatePrefix();
	};

    ~Logger();

	/*!
	 * Change of global logger info level
	 * \param new logging level
	 */
	static void setLogLvl(int lvl);

	/*!
	 * Setting number of device to which logger is binded
	 * \param device number
	 */
	void setDevNum(int num);

	static void globalInfo(int lvl, const char *note1 = NULL, const char* note2 = NULL, const char* note3 = NULL);

	static void globalWarning(const char *note1 = NULL, const char* note2 = NULL, const char* note3 = NULL);

	static void globalError(const char *note1 = NULL, const char* note2 = NULL, const char* note3 = NULL);

	static const int L0 = 0;
	static const int L1 = 1;
	static const int L2 = 2;
	static const int L3 = 3;
    static const int L4 = 4;

    static const int& getLogLevel();
private:

	std::fstream& getStream();

	void closeStream();

	void updatePrefix();

    static int loggingLevel;

	std::fstream f;
	long creationTime;
	int logNum;

	int devNum;

	std::string prefix;

	static const char * timestrformat;

    mutex localVariableDataAccess;
};


inline void Logger::setLogLvl(int lvl){
	loggingLevel = lvl;
}


inline const int& Logger::getLogLevel(){
    return loggingLevel;
}

inline void Logger::setDevNum(int num){
    lock_guard<mutex> raiiLock(localVariableDataAccess);
	devNum = num;
	updatePrefix();
}

inline void Logger::updatePrefix(){
	prefix = devNum? (char *)("[MS " + CharString(devNum)  + "] "): "";
}

}//namespace
#endif /* DEBUGMETHODS_HXX_ */
