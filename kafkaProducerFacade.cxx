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

#include "kafkaProducerFacade.hxx"
#include "Common/Constants.hxx"
#include "Common/Logger.hxx"

#include "cppkafka/message.h"
#include "cppkafka/kafka_handle_base.h"
#include "cppkafka/error.h"
#include "cppkafka/topic.h"
#include "cppkafka/configuration.h"
#include "cppkafka/producer.h"
#include "cppkafka/utils/buffered_producer.h"


#include <memory>

using cppkafka::Message;
using cppkafka::MessageBuilder;
using cppkafka::Error;
using cppkafka::KafkaHandleBase;
using cppkafka::Configuration;
using cppkafka::Exception;
using cppkafka::Producer;
using cppkafka::BufferedProducer;
using cppkafka::MessageBuilder;



kafkaProducerFacade::kafkaProducerFacade(errorCallback ecb=nullptr, statsCallback scb = nullptr)
    : _errorCB(ecb), _statsCB(scb)
{
    Configuration config;
    for (const auto & kv : Common::Constants::GetProducerConfig())
    {
        config.set(kv.first, kv.second);
    }
    if(_errorCB)
        config.set_error_callback([this](KafkaHandleBase& handle, int error , const std::string& reason)
        {
            this->_errorCB(error, reason);
        });
    if(_statsCB)
        config.set_stats_callback([this](KafkaHandleBase& handle, const std::string& json)
        {
            this->_statsCB(json);
        });
    _buffProducer.reset(new BufferedProducer<std::string>(config));
    _buffProducer->set_flush_method(BufferedProducer<std::string>::FlushMethod::Async);
}

kafkaProducerFacade::~kafkaProducerFacade()
{
    _buffProducer->async_flush();
}

void kafkaProducerFacade::poll()
{
    _buffProducer->get_producer().poll();
}

void kafkaProducerFacade::setProducerSuccessCallback(successCallback cb)
{
    if(cb)
        _buffProducer->set_produce_success_callback([cb](const Message& msg){
            std::string str = msg.get_payload().operator std::string();
            cb(str);
        });
    else
        _buffProducer->set_produce_success_callback(nullptr);
}

void kafkaProducerFacade::stream(const std::string& topic, const std::string& key, std::string&& message)
{
    // Actually produce the message we've built
    _buffProducer->produce(MessageBuilder(topic).key(key).payload(message));
}

void kafkaProducerFacade::stream(const std::string& topic, int partition, const std::string& key, std::string&& message)
{
    // Actually produce the message we've built
    _buffProducer->produce(MessageBuilder(topic).partition(partition).key(key).payload(message));
}
