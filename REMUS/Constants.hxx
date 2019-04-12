/*!
 *  \file Constants.hxx
 *  \author Kacper "szkudi" Szkudlarek
 *  \brief File contain definitions of some constants used in driver
 *
 *  Changelog:
 *
 */


#ifndef CONSTANTS_HXX_
#define CONSTANTS_HXX_

#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory>
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

#include <map>
#include <REMUS/Utils.hxx>
#include <REMUS/Logger.hxx>

namespace REMUS{

/*!
 * \class Constants
 * \brief Class containing constant values used in driver
 */
class Constants{
public:

    //producer
    constexpr static const char* PRODUCER_CONFIG_KEYWORD {"PRODUCER.CONFIG."};
    constexpr static const char* CONFIG_STATISTICS_INTERVAL_MS {"statistics.interval.ms"};
    constexpr static const int CONFIG_STATISTICS_INTERVAL_MS_DEFAULT = 60 * 1000; // 60 seconds

    //consumer
    constexpr static const char* CONSUMER_CONFIG_KEYWORD {"CONSUMER.CONFIG."};


    constexpr static const char* GROUP_ID_KEYWORD {"group.id"};


    static void setLogpath(std::string lp);

    static std::string& getLogpath();

    static void setDrvName(std::string lp);

    static std::string& getDrvName();

    // called in driver init to set the driver number dynamically
    static void setDrvNo(uint32_t no);
    // subsequentally called when writing buffers etc.
    static uint32_t getDrvNo();


    static void setDebouncingThreadInterval(uint32_t no);
    static const uint32_t& getDebouncingThreadInterval();

    static void setProducerKafkaStatsInterval();
    static const uint32_t& getProducerKafkaStatsInterval();

    static void setConsumerKafkaStatsInterval();
    static const uint32_t& getConsumerKafkaStatsInterval();

    static void setConsumerMaxPollRecords(size_t max);
    static const size_t& getConsumerMaxPollRecords();

    // slave mode functions (listens but does not speak)
	static bool isSlaveMode() {
		return slave_mode;
	}

	static void setSlaveMode(bool slaveMode) {
		slave_mode = slaveMode;
	}

    static const std::map<std::string,std::string>& GetProducerConfig();
    static const std::map<std::string,std::function<void(const char *)>>& GetParseMap();
    static void setProducerConfig(std::string& key, std::string& value);
    static bool hasProducerConfig();

    static const std::map<std::string,std::string>& GetConsumerConfig();
    static void setConsumerConfig(std::string& key, std::string& value);
    static bool hasConsumerConfig();

private:
    static std::string drv_name;

    static uint32_t DRV_NO;   // WinCC OA manager number
    static uint32_t DEBOUNCING_THREAD_INTERVAL;
    static uint32_t PRODUCER_KAFKA_STATS_INTERVAL;
    static uint32_t CONSUMER_KAFKA_STATS_INTERVAL;
    static size_t CONSUMER_MAX_POLL_RECORDS;

    static bool slave_mode;
    static std::map<std::string, std::string> producer_config;
    static std::map<std::string, std::string> consumer_config;

    static std::map<std::string, std::function<void(const char*)>> parse_map;
};

inline const std::map<std::string, std::string>& Constants::GetProducerConfig()
{
    return producer_config;
}

inline const std::map<std::string, std::string>& Constants::GetConsumerConfig()
{
    return consumer_config;
}



inline const std::map<std::string,std::function<void(const char *)>>& Constants::GetParseMap()
{
    return parse_map;
}



inline void Constants::setProducerConfig(std::string& key, std::string& value)
{
    producer_config[key] = value;
}


inline void Constants::setConsumerConfig(std::string& key, std::string& value)
{
    consumer_config[key] = value;
}

inline bool Constants::hasProducerConfig()
{
    return producer_config.size() > 0;
}


inline bool Constants::hasConsumerConfig()
{
    return consumer_config.size() > 0;
}


inline void Constants::setDrvName(std::string dname){
	drv_name = dname;
}

inline std::string& Constants::getDrvName(){
	return drv_name;
}

inline void Constants::setDrvNo(uint32_t no){
	DRV_NO = no;
}

inline uint32_t Constants::getDrvNo(){
	return DRV_NO;
}

inline void Constants::setDebouncingThreadInterval(uint32_t no){
    DEBOUNCING_THREAD_INTERVAL = no;
}

inline const uint32_t& Constants::getDebouncingThreadInterval(){
    return DEBOUNCING_THREAD_INTERVAL;
}

inline void Constants::setConsumerMaxPollRecords(size_t max)
{
    CONSUMER_MAX_POLL_RECORDS = max;
}

inline const size_t& Constants::getConsumerMaxPollRecords()
{
    return CONSUMER_MAX_POLL_RECORDS;
}

inline void Constants::setProducerKafkaStatsInterval(){
    int milis;
    try
    {
        if(!REMUS::Utils::convertToInt(REMUS::Constants::GetProducerConfig().at(REMUS::Constants::CONFIG_STATISTICS_INTERVAL_MS), milis))
        {
            milis = REMUS::Constants::CONFIG_STATISTICS_INTERVAL_MS_DEFAULT; // DEFAULT
        }
    }
    catch(std::exception& e)
    {
        REMUS::Logger::globalWarning(__PRETTY_FUNCTION__," Constants ProducerConfig does not have ", REMUS::Constants::CONFIG_STATISTICS_INTERVAL_MS);
        milis = REMUS::Constants::CONFIG_STATISTICS_INTERVAL_MS_DEFAULT; // DEFAULT
    }
    PRODUCER_KAFKA_STATS_INTERVAL = milis;
}

inline const uint32_t& Constants::getProducerKafkaStatsInterval(){
    return PRODUCER_KAFKA_STATS_INTERVAL;
}

inline void Constants::setConsumerKafkaStatsInterval(){
    int milis;
    try
    {
        if(!REMUS::Utils::convertToInt(REMUS::Constants::GetConsumerConfig().at(REMUS::Constants::CONFIG_STATISTICS_INTERVAL_MS), milis))
        {
            milis = REMUS::Constants::CONFIG_STATISTICS_INTERVAL_MS_DEFAULT; // DEFAULT
        }
    }
    catch(std::exception& e)
    {
        REMUS::Logger::globalWarning(__PRETTY_FUNCTION__," Constants ConsumerConfig does not have ", REMUS::Constants::CONFIG_STATISTICS_INTERVAL_MS);
        milis = REMUS::Constants::CONFIG_STATISTICS_INTERVAL_MS_DEFAULT; // DEFAULT
    }
    CONSUMER_KAFKA_STATS_INTERVAL = milis;
}

inline const uint32_t& Constants::getConsumerKafkaStatsInterval(){
    return CONSUMER_KAFKA_STATS_INTERVAL;
}


}//namespace
#endif /* CONSTANTS_HXX_ */
