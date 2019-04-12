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

#ifndef KAFKACONSUMERFACADE_HXX
#define KAFKACONSUMERFACADE_HXX

#include <string>

#include <stdexcept>
#include <iostream>


#include <cppkafka/utils/consumer_dispatcher.h>
#include <cppkafka/consumer.h>

using cppkafka::ConsumerDispatcher;
using cppkafka::Consumer;

using statsCallbackConsumer = std::function<void(const std::string& topic, const std::string& statsJson)>;
using errorCallbackConsumer = std::function<void(const std::string& topic, int error,  const std::string& reason)>;
using consumeCallbackConsumer = std::function<void(const std::string& topic, const std::string& key, std::string&& payload)>;


/**
 * @brief The kafkaConsumerFacade class is a facade and encompasses all the consumer interaction with librdkafka and cppkafka
 */

class kafkaConsumerFacade
{
public:
    /**
     * @brief kafkaConsumerFacade constructor
     * @param topic : the topic name
     * @param consumeCallbackConsumer : a callback that will be called for eached polled message
     * @param errorCallbackConsumer : an error callback for underlying kafka errors
     * @param statsCallbackConsumer : a callback for handling consumer statistics
     */
    kafkaConsumerFacade(const std::string& topic, consumeCallbackConsumer, errorCallbackConsumer, statsCallbackConsumer);

    kafkaConsumerFacade(const kafkaConsumerFacade&) = delete;
    kafkaConsumerFacade& operator=(const kafkaConsumerFacade&) = delete;

    bool isInitialized(){return _initialized;}
    void poll(const size_t&);
private:
    std::unique_ptr<Consumer> _consumer;
    std::string _topic;
    consumeCallbackConsumer _consumeCB;
    errorCallbackConsumer _errorCB;
    statsCallbackConsumer _statsCB;
    bool _initialized{false};
};

#endif // KAFKACONSUMERFACADE_HXX

