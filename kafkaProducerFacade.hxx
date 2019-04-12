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


#ifndef KAFKAPRODUCERFACADE_HXX
#define KAFKAPRODUCERFACADE_HXX

#include <string>
#include <stdexcept>
#include <iostream>
#include <memory>
#include <cppkafka/utils/buffered_producer.h>

using std::string;


using cppkafka::BufferedProducer;

using statsCallback = std::function<void(const std::string&)>;
using errorCallback = std::function<void(int error,  const std::string& reason)>;
using successCallback = statsCallback;


/**
 * @brief The kafkaProducerFacade class is a facade and encompasses all the producer interaction with librdkafka and cppkafka
 */

class kafkaProducerFacade
{
public:
    /**
     * @brief kafkaProducerFacade constructor
     * @param errorCallbackConsumer : an error callback for underlying kafka errors
     * @param statsCallbackConsumer : a callback for handling producer statistics
     */
    kafkaProducerFacade(errorCallback, statsCallback);
    ~kafkaProducerFacade();

    kafkaProducerFacade(const kafkaProducerFacade& ) = delete;
    kafkaProducerFacade& operator=(const kafkaProducerFacade& ) = delete;

    void stream(const std::string& topic, int partition, const std::string&, std::string&& message);
    void stream(const std::string& topic, const std::string&, std::string&& message);

    void setProducerSuccessCallback(successCallback);

    void poll();

private:
    std::unique_ptr<BufferedProducer<std::string>> _buffProducer;
    errorCallback _errorCB;
    statsCallback _statsCB;
};


#endif // KAFKAPRODUCERFACADE_HXX
