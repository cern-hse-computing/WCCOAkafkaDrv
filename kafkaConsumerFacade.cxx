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

#include <csignal>

#include "kafkaConsumerFacade.hxx"
#include "Common/Constants.hxx"
#include "Common/Logger.hxx"

#include "cppkafka/message.h"
#include "cppkafka/kafka_handle_base.h"
#include "cppkafka/error.h"
#include "cppkafka/configuration.h"
#include "cppkafka/topic.h"
#include "cppkafka/consumer.h"
#include "cppkafka/error.h"

#include <vector>


using cppkafka::Consumer;
using cppkafka::Message;
using cppkafka::Error;
using cppkafka::Configuration;
using cppkafka::KafkaHandleBase;
using cppkafka::ConfigOptionNotFound;

kafkaConsumerFacade::kafkaConsumerFacade(const std::string& topic, consumeCallbackConsumer cb, errorCallbackConsumer erc = nullptr, statsCallbackConsumer sc = nullptr)
    : _topic(topic), _consumeCB(cb), _errorCB(erc), _statsCB(sc)
{

    try{
        //Create kafka configuration
        Configuration config;
        for (const auto & kv : Common::Constants::GetConsumerConfig())
        {
            std::cerr << kv.first << " - "<< kv.second << std::endl;
            config.set(kv.first, kv.second);

        }
        //Update consumer group id to be unique by appending topic and current timestamp
        try
        {
            config.set(Common::Constants::GROUP_ID_KEYWORD, config.get(Common::Constants::GROUP_ID_KEYWORD) + "_" + topic  + "_" + std::to_string(std::chrono::seconds(std::time(NULL)).count()));
        }
        catch (ConfigOptionNotFound& e)
        {
             Common::Logger::globalWarning("No group id found! Adding default one: topicname_unixtimestamp", e.what());
             config.set(Common::Constants::GROUP_ID_KEYWORD, topic + "_" + std::to_string(std::chrono::seconds(std::time(NULL)).count()));
        }

        // set the error and statistics callbacks on the kafka config
        if(_errorCB)
            config.set_error_callback([this](KafkaHandleBase& handle, int error , const std::string& reason)
            {
                this->_errorCB(_topic, error, reason);
            });
        if(_statsCB)
            config.set_stats_callback([this](KafkaHandleBase& handle, const std::string& json)
            {
                this->_statsCB(_topic, json);
            });

        // Create Consumer
        _consumer.reset(new Consumer(config));
        _consumer->subscribe({_topic});

        _initialized = true;
    }
    catch(std::exception& e)
    {
        Common::Logger::globalWarning("Unable to initialize consumer!", e.what());
    }
}

void kafkaConsumerFacade::poll(const size_t& maxPollRecords)
{
    Common::Logger::globalInfo(Common::Logger::L2, __PRETTY_FUNCTION__, "Poll batch with maxPollRecords:", CharString(maxPollRecords));
    std::vector<Message> msgVec = std::move(_consumer->poll_batch(maxPollRecords));
    for (auto & msg : msgVec )
    {
        if (msg) {
            // If we managed to get a message
            if (msg.get_error()) {
                // Ignore EOF notifications from rdkafka
                if (!msg.is_eof()) {
                    this->_errorCB(_topic, msg.get_error().get_error(), msg.get_error().to_string());
                }
            }
            else {
                this->_consumeCB(msg.get_topic(), msg.get_key(), msg.get_payload());
                //_consumer->commit(msg);
            }
        }
    }
}

