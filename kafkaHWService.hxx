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


#ifndef kafkaHWSERVICE_H_
#define kafkaHWSERVICE_H_

#include <HWService.hxx>
#include <memory>
#include "kafkaProducerFacade.hxx"
#include "kafkaConsumerFacade.hxx"

#include "REMUS/Logger.hxx"
#include "REMUS/AsyncRecurringTask.hxx"
#include <queue>
#include <chrono>
#include <unordered_map>

class kafkaProducerFacade;

class kafkaHWService : public HWService
{
  public:
    kafkaHWService();
    virtual PVSSboolean initialize(int argc, char *argv[]);
    virtual PVSSboolean start();
    virtual void stop();
    virtual void workProc();
    virtual PVSSboolean writeData(HWObject *objPtr);



private:
    //Producer related
    std::unique_ptr<kafkaProducerFacade> _producerFacade{nullptr};
    void handleProducerConfigError(int, const std::string&);
    void handleProducerConfigStats(const std::string&);
    void handleProducerSuccess(const std::string&);
    void doPoll();
    //TODO handleQueueFullError
    successCallback _producerSuccessCB{[this](const std::string& str){this->handleProducerSuccess(str);}};
    errorCallback _configErrorProducerCB{[this](int err, const string& reason) { this->handleProducerConfigError(err, reason);}};
    statsCallback _configStatsProducerCB{[this](const std::string& json){this->handleProducerConfigStats(json);}};


    // Consumer related
    void handleConsumerConfigError(const std::string&, int, const std::string&);
    void handleConsumerConfigStats(const std::string&, const std::string&);
    void handleConsumeNewMessage(const std::string&, const std::string&, std::string&&);
    void handleNewConsumerTopic(const std::string& topic);

    errorCallbackConsumer _configErrorConsumerCB{[this](const std::string& topic, int err, const string& reason) { this->handleConsumerConfigError(topic, err, reason);}};
    statsCallbackConsumer _configStatsConsumerCB{[this](const std::string& topic, const std::string& json){this->handleConsumerConfigStats(topic, json);}};
    consumeCallbackConsumer  _configConsumeCB{[this](const std::string& topic, const std::string& key, std::string&& payload){this->handleConsumeNewMessage(topic, key, std::move(payload));}};
    std::function<void(const std::string&)> _newConsumerTopicCB{[this](const std::string& topic){this->handleNewConsumerTopic(topic);}};


    //Common
    void insertInDataToDp(CharString&& address, CharString&& value);
    std::mutex _toDPmutex;

    std::queue<std::pair<CharString,CharString>> _toDPqueue;
    std::atomic<bool> _brokersDown{false};

    enum
    {
       STREAM_OPTIONS_TOPIC = 0,
       STREAM_OPTIONS_KEY,
       STREAM_OPTIONS_DEBOUNCING_TIMEFRAME,
       //STREAM_OPTIONS_PARTITION,
       STREAM_OPTIONS_SIZE
    } STREAM_OPTIONS;

    constexpr static const char* PRODUCER_ALL_BROKERS_DOWN_DP = "PRODUCER_ALL_BROKERS_DOWN"; // Used to signal if all brokers are down (due to different reasons, i.e. configuration is not OK)
    constexpr static const char* NO_PRODUCER_CONFIG= "PRODUCER_NO_CONFIG"; // Usded to signal if producer config is absent and the driver is addressed for producing messages
    constexpr static const char* PRODUCER_STATS_DP= "PRODUCER_STATISTICS";


    constexpr static const char* NO_CONSUMER_CONFIG= "CONSUMER_NO_CONFIG"; // Used to signal if consumer config is absent and the driver is addressed for consuming messages
    constexpr static const char* CONSUMER_STATS_DP = "CONSUMER_STATISTICS";



    std::mutex _streamMutex;
    struct streamStruct
    {
        std::chrono::high_resolution_clock::time_point time;
        std::string topic;
        std::string key;
        int debouncing_timeframe;
        std::string JSONvalue;
    };

    std::unordered_map<std::string, streamStruct> _streamMap;
    std::unique_ptr<REMUS::AsyncRecurringTask<std::function<void()>>> _streamAsyncTask;
    std::unique_ptr<REMUS::AsyncRecurringTask<std::function<void()>>> _poolAsyncTask;
    void doStream();

    std::vector<std::thread> _consumerThreads;
};


void handleSegfault(int signal_code);

#endif
