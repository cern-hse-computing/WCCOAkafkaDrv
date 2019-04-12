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

#include "Constants.hxx"
#include "Logger.hxx"
#include "Utils.hxx"

namespace REMUS {

    // Common
    std::string Constants::drv_name = "kafka";
    uint32_t Constants::DRV_NO = 0;                                 // Read from PVSS on driver startup

    // Producer
    uint32_t Constants::PRODUCER_KAFKA_STATS_INTERVAL = 60;         // Read from PVSS on driver startup
    uint32_t Constants::DEBOUNCING_THREAD_INTERVAL = 50;            // Read from PVSS on driver startup

    // Consumer
    size_t Constants::CONSUMER_MAX_POLL_RECORDS = 1000;             // Read from PVSS on driver startup
    uint32_t Constants::CONSUMER_KAFKA_STATS_INTERVAL = 60;         // Read from PVSS on driver startup


    bool Constants::slave_mode = false;                             // Slave mode will accept all packages and speak only when spoken to

    std::map<std::string, std::string> Constants::producer_config;
    std::map<std::string, std::string> Constants::consumer_config;

    // The map can be used to map a callback to a HwObject address
    std::map<std::string, std::function<void(const char*)>> Constants::parse_map =
    {
        {   "DEBUGLVL",
            [](const char* data)
            {
                REMUS::Logger::setLogLvl((int)*reinterpret_cast<const int32_t*>(data));
            }
        },
        {   "DEBOUNCINGTHREADINTERVAL",
            [](const char* data)
            {
              REMUS::Constants::setDebouncingThreadInterval((int)*reinterpret_cast<const int32_t*>(data));
            }
        },
        {   "MAXPOLLRECORDS",
            [](const char* data)
            {
              REMUS::Constants::setConsumerMaxPollRecords((size_t)*reinterpret_cast<const int32_t*>(data));
            }
        }

    };
}
