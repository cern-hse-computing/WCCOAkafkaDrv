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


#ifndef kafkaHWMAPPER_H_
#define kafkaHWMAPPER_H_

#include <HWMapper.hxx>
#include <unordered_set>
#include <functional>

// TODO: Write here all the Transformation types, one for every transformation
#define kafkaDrvBoolTransType (TransUserType)
#define kafkaDrvUint8TransType (TransUserType + 1)
#define kafkaDrvInt32TransType (TransUserType + 2)
#define kafkaDrvInt64TransType (TransUserType + 3)
#define kafkaDrvFloatTransType (TransUserType + 4)
#define kafkaDrvStringTransType (TransUserType + 5)
#define kafkaDrvTimeTransType (TransUserType + 6)


using newConsumerTopicCallback = std::function<void(const std::string& topic)>;

class kafkaHWMapper : public HWMapper
{
  public:
    virtual PVSSboolean addDpPa(DpIdentifier &dpId, PeriphAddr *confPtr);
    virtual PVSSboolean clrDpPa(DpIdentifier &dpId, PeriphAddr *confPtr);

    void setNewConsumerTopicCallback(newConsumerTopicCallback cb) {_newConsumerTopicCB = cb;}
    const std::unordered_set<std::string>& getConsumerTopics(){return consumer_topics;}

  private:
    void addTopic(const std::string& topic);

    std::unordered_set<std::string> consumer_topics;
    newConsumerTopicCallback _newConsumerTopicCB = {nullptr};

    enum Direction
    {
        DIRECTION_OUT = 1,
        DIRECTION_IN = 2
    };
};

#endif
