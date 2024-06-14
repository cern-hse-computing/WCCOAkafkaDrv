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

#include <kafkaHWService.hxx>

#include <DrvManager.hxx>
#include <PVSSMacros.hxx>     // DEBUG macros
#include "Common/Logger.hxx"
#include "Common/Constants.hxx"
#include "Common/Utils.hxx"
#include "Common/AsyncRecurringTask.hxx"

#include "kafkaHWMapper.hxx"
#include "kafkaConsumerFacade.hxx"


#include <signal.h>
#include <execinfo.h>
#include <exception>
#include <chrono>
#include <thread>

static std::atomic<bool> _consumerRun{true};

//--------------------------------------------------------------------------------
// called after connect to data

kafkaHWService::kafkaHWService()
{
  signal(SIGSEGV, handleSegfault);
}

PVSSboolean kafkaHWService::initialize(int argc, char *argv[])
{
  // use this function to initialize internals
  // if you don't need it, you can safely remove the whole method
  Common::Logger::globalInfo(Common::Logger::L1,__PRETTY_FUNCTION__,"start");

  //Constants (at this point, the config file has been read)
  Common::Constants::setProducerKafkaStatsInterval();
  Common::Constants::setConsumerKafkaStatsInterval();

  // add callback for new consumer topics
  static_cast<kafkaHWMapper*>(DrvManager::getHWMapperPtr())->setNewConsumerTopicCallback(_newConsumerTopicCB);

  Common::Logger::globalInfo(Common::Logger::L1,__PRETTY_FUNCTION__,"end");
  // To stop driver return PVSS_FALSE
  return PVSS_TRUE;
}

// in order to get statistics, poll() has to be called in the producer facade.
void kafkaHWService::doPoll()
{
    Common::Logger::globalInfo(Common::Logger::L3,__PRETTY_FUNCTION__,"Calling poll");
    this->_producerFacade->poll();
}

void kafkaHWService::doStream()
{
    Common::Logger::globalInfo(Common::Logger::L4,__PRETTY_FUNCTION__,"Calling doStream");
    auto now = std::chrono::high_resolution_clock::now();
    for(auto& kv : _streamMap)
    {
        if(kv.second.JSONvalue.length()>0)
        {
            // Check debouncing time
            auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now - kv.second.time).count();
            if(millis >= kv.second.debouncing_timeframe)
            {
                streamStruct s ;
                { //lock_guard scope
                    std::lock_guard<std::mutex> lck(_streamMutex);
                    s = std::move(kv.second);
                }
                Common::Logger::globalInfo(Common::Logger::L2,"<stream><out>",(s.topic+"$"+s.key).c_str(), s.JSONvalue.c_str());
                _producerFacade->stream(s.topic,/* partition,*/ s.key,std::move(s.JSONvalue));
            }
        }

    }
}

void kafkaHWService::handleProducerConfigError(int code, const std::string& str)
{
    Common::Logger::globalWarning(__PRETTY_FUNCTION__,CharString(code),str.c_str());
    if(code == -187)
    {
        _brokersDown = true;
        _producerFacade->setProducerSuccessCallback(_producerSuccessCB);
        insertInDataToDp(PRODUCER_ALL_BROKERS_DOWN_DP, "TRUE");
    }
}


void kafkaHWService::handleProducerConfigStats(const std::string& jsonStats)
{
    Common::Logger::globalInfo(Common::Logger::L1, __PRETTY_FUNCTION__,"Statistics: ",jsonStats.c_str());
    insertInDataToDp(PRODUCER_STATS_DP,CharString(jsonStats.c_str(),jsonStats.length()));
}


void kafkaHWService::handleProducerSuccess(const std::string& message)
{
    Common::Logger::globalInfo(Common::Logger::L3,__PRETTY_FUNCTION__, message.c_str());
    if(_brokersDown)
    {
        _brokersDown = false;
        _producerFacade->setProducerSuccessCallback(nullptr);
        insertInDataToDp(PRODUCER_ALL_BROKERS_DOWN_DP, "FALSE");
    }
}



void kafkaHWService::handleConsumerConfigError(const std::string& topic, int code, const std::string& str)
{
     Common::Logger::globalWarning(__PRETTY_FUNCTION__, CharString(topic.c_str(), topic.length()), str.c_str());
}

void kafkaHWService::handleConsumerConfigStats(const std::string&topic, const std::string& jsonStats)
{
    Common::Logger::globalInfo(Common::Logger::L1, __PRETTY_FUNCTION__,topic.c_str(), jsonStats.c_str());

    insertInDataToDp(CONSUMER_STATS_DP,CharString(jsonStats.c_str(),jsonStats.length()));
    //insertInDataToDp(CharString(CONSUMER_STATS_DP_BASE)+CharString(topic.c_str(), topic.length()),CharString(jsonStats.c_str(),jsonStats.length())); //TODO - consumer logs per topic
}

void kafkaHWService::handleConsumeNewMessage(const std::string& topic, const std::string& key, std::string&& payload )
{
    Common::Logger::globalInfo(Common::Logger::L3, __PRETTY_FUNCTION__, (topic + ":" + key + ":" + payload).c_str());
    insertInDataToDp(std::move(CharString((topic+"$"+key).c_str())),std::move(CharString(payload.c_str(),payload.length())));
}

void kafkaHWService::handleNewConsumerTopic(const std::string& topic)
{
    Common::Logger::globalInfo(Common::Logger::L1,__PRETTY_FUNCTION__, "New topic:", topic.c_str());
    _consumerThreads.emplace_back(
                [&]
        {

            kafkaConsumerFacade aConsumerFacade(topic,this->_configConsumeCB, this->_configErrorConsumerCB, this->_configStatsConsumerCB);
            if(!aConsumerFacade.isInitialized())
            {
                 Common::Logger::globalError("Unable to initialize topic:", topic.c_str());
            }
            else
            {
                while(_consumerRun)
                {
                      aConsumerFacade.poll(Common::Constants::getConsumerMaxPollRecords());
                }
            }

        });
    Common::Logger::globalInfo(Common::Logger::L1,__PRETTY_FUNCTION__, "New topic done:", topic.c_str());
}

//--------------------------------------------------------------------------------
// called after connect to event

PVSSboolean kafkaHWService::start()
{
  // use this function to start your hardware activity.
  Common::Logger::globalInfo(Common::Logger::L1,__PRETTY_FUNCTION__);

  // See if we need to launch a producer
  if(Common::Constants::hasProducerConfig())
  {
       // kafka Callbacks
      _producerFacade.reset(new kafkaProducerFacade(_configErrorProducerCB, _configStatsProducerCB));
      if(!_producerFacade)
      {
          Common::Logger::globalError("kafkaHWService::initialize Unable to intialize the producer!");
          return PVSS_FALSE;
      }

      // Async Recurring tasks
      _poolAsyncTask.reset(new Common::AsyncRecurringTask<std::function<void()>>(std::move([this]() { this->doPoll(); }), Common::Constants::getProducerKafkaStatsInterval()));
      _streamAsyncTask.reset(new Common::AsyncRecurringTask<std::function<void()>>(std::move([this]() { this->doStream(); }) , Common::Constants::getDebouncingThreadInterval()));
  }
  else
  {
       Common::Logger::globalInfo(Common::Logger::L1,__PRETTY_FUNCTION__,"No producer configuration provided");
  }

   // Check if we need to launch consumer(s)
   // This list is automatically built by exisiting addresses sent at driver startup
   // new top
   for (const auto& topic : static_cast<kafkaHWMapper*>(DrvManager::getHWMapperPtr())->getConsumerTopics() )
   {
        this->handleNewConsumerTopic(topic);
   }

  return PVSS_TRUE;
}

//--------------------------------------------------------------------------------

void kafkaHWService::stop()
{
  // use this function to stop your hardware activity.
  Common::Logger::globalInfo(Common::Logger::L1,__PRETTY_FUNCTION__,"Stop");
  _consumerRun = false;
  for(auto& ct : _consumerThreads)
  {
    if(ct.joinable())
        ct.join();
  }
}

//--------------------------------------------------------------------------------

void kafkaHWService::workProc()
{
  HWObject obj;

  // TODO somehow receive a message from your device
  std::lock_guard<std::mutex> lock{_toDPmutex};
  Common::Logger::globalInfo(Common::Logger::L3,__PRETTY_FUNCTION__,"Size", CharString(_toDPqueue.size()));
  while (!_toDPqueue.empty())
  {
    auto pair = std::move(_toDPqueue.front());
    _toDPqueue.pop();


    obj.setAddress(pair.first);


//    // a chance to see what's happening
//    if ( Resources::isDbgFlag(Resources::DBG_DRV_USR1) )
//      obj.debugPrint();

    // find the HWObject via the periphery address in the HWObject list,
    HWObject *addrObj = DrvManager::getHWMapperPtr()->findHWObject(&obj);

    // ok, we found it; now send to the DPEs
    if ( addrObj )
    {
      Common::Logger::globalInfo(Common::Logger::L2,__PRETTY_FUNCTION__, pair.first, pair.second);
      //addrObj->debugPrint();
      obj.setOrgTime(TimeVar());  // current time
      obj.setDlen(pair.second.len()); // data
      obj.setData((PVSSchar*)pair.second.cutCharPtr());

      DrvManager::getSelfPtr()->toDp(&obj, addrObj);
    }
  }
}


void kafkaHWService::insertInDataToDp(CharString&& address, CharString&& value)
{
    std::lock_guard<std::mutex> lock{_toDPmutex};
    _toDPqueue.push(std::move(std::make_pair<CharString,CharString>(std::move(address),std::move(value))));
}

//--------------------------------------------------------------------------------
// we get data from PVSS and shall send it to the periphery

PVSSboolean kafkaHWService::writeData(HWObject *objPtr)
{
//  Common::Logger::globalInfo(Common::Logger::L2,__PRETTY_FUNCTION__,"Incoming obj address",objPtr->getAddress());

  std::vector<std::string> streamOptions = Common::Utils::split(objPtr->getAddress().c_str());

  // CONFIG DPs have just 1
  if(streamOptions.size() == 1)
  {
      try
      {
        Common::Logger::globalInfo(Common::Logger::L2,"Incoming CONFIG address",objPtr->getAddress(), objPtr->getInfo() );
        Common::Constants::GetParseMap().at(std::string(objPtr->getAddress().c_str()))((const char*)objPtr->getData());
      }
      catch (std::exception& e)
      {
          Common::Logger::globalWarning(__PRETTY_FUNCTION__," No configuration handling for address:", objPtr->getAddress().c_str());
      }
  }
  else if (streamOptions.size() == STREAM_OPTIONS_SIZE || streamOptions.size() == STREAM_OPTIONS_SIZE - 1) // streaming
  {

    if(!streamOptions[STREAM_OPTIONS_TOPIC].length())
    {
        Common::Logger::globalWarning(__PRETTY_FUNCTION__," Empty topic name");
        return PVSS_FALSE;
    }
    int debouncing_timeframe = 0; // default value is 0 for debouncing timeframe
    if(streamOptions.size() == STREAM_OPTIONS_SIZE && !Common::Utils::convertToInt(streamOptions[STREAM_OPTIONS_DEBOUNCING_TIMEFRAME], debouncing_timeframe))
    {
        Common::Logger::globalWarning(__PRETTY_FUNCTION__," Cannot parse debouncing timeframe", streamOptions[STREAM_OPTIONS_DEBOUNCING_TIMEFRAME].c_str());
        return PVSS_FALSE;
    }
    //    int partition = -1;
    //    if(!Common::Utils::convertToInt(streamOptions[STREAM_OPTIONS_PARTITION], partition))
    //    {
    //        Common::Logger::globalWarning(__PRETTY_FUNCTION__," Cannot parse partition", streamOptions[STREAM_OPTIONS_PARTITION].c_str());
    //        return PVSS_FALSE;
    //    }

    if(!debouncing_timeframe) // no debouncing timeframe, stream directly
    {
         _producerFacade->stream(streamOptions[STREAM_OPTIONS_TOPIC],streamOptions[STREAM_OPTIONS_KEY], /* partition,*/std::move(std::string((char*)objPtr->getDataPtr())));
         Common::Logger::globalInfo(Common::Logger::L2,"<stream><in-out>",objPtr->getAddress(), (char*)objPtr->getDataPtr() );

    }
    else // debouncing time frame, add it to stream map. The async task doStream will take care of streaming
    {
        std::lock_guard<std::mutex> lck(_streamMutex);
        _streamMap[std::string(objPtr->getAddress())] =  {std::chrono::high_resolution_clock::now(),
                                                          streamOptions[STREAM_OPTIONS_TOPIC],
                                                          streamOptions[STREAM_OPTIONS_KEY],
                                                          debouncing_timeframe,
                                                          std::move(std::string((char*)objPtr->getDataPtr()/*, objPtr->getDlen()*/))
                                                          };
        Common::Logger::globalInfo(Common::Logger::L2,"<stream><in>",objPtr->getAddress(), (char*)objPtr->getDataPtr() );

    }

   // _producerFacade->stream(streamOptions[STREAM_OPTIONS_TOPIC],/* partition,*/ streamOptions[STREAM_OPTIONS_KEY]  ,std::move(std::string((char *)objPtr->getDataPtr(), objPtr->getDlen())));
  }
  else
  {
      Common::Logger::globalWarning(__PRETTY_FUNCTION__," Invalid stream options size for address: ", objPtr->getAddress().c_str());
  }

  return PVSS_TRUE;
}

//--------------------------------------------------------------------------------


void handleSegfault(int signal_code){
    void *array[50];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 50);

    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", signal_code);
    Common::Logger::globalWarning("kafkaHWService suffered a segmentation fault, code " + CharString(signal_code));
    backtrace_symbols_fd(array, size, STDERR_FILENO);

    // restore and trigger default handle (to get the core dump)
    signal(signal_code, SIG_DFL);
    kill(getpid(), signal_code);

    exit(1);
}
