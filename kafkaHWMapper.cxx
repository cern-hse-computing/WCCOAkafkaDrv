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

#include "kafkaHWMapper.hxx"
#include "Transformations/kafkaStringTrans.hxx"
#include "Transformations/kafkaInt32Trans.hxx"
#include "Transformations/kafkaInt64Trans.hxx"
#include "Transformations/kafkaFloatTrans.hxx"
#include "Transformations/kafkaBoolTrans.hxx"
#include "Transformations/kafkaUint8Trans.hxx"
#include "Transformations/kafkaTimeTrans.hxx"

#include "REMUS/Logger.hxx"
#include "REMUS/Utils.hxx"
#include <PVSSMacros.hxx>     // DEBUG macros

//--------------------------------------------------------------------------------
// We get new configs here. Create a new HW-Object on arrival and insert it.

PVSSboolean kafkaHWMapper::addDpPa(DpIdentifier &dpId, PeriphAddr *confPtr)
{
  // We don't use Subindices here, so its simple.
  // Otherwise we had to look if we already have a HWObject and adapt its length.

  REMUS::Logger::globalInfo(REMUS::Logger::L1,"addDpPa called for ", confPtr->getName().c_str());
  REMUS::Logger::globalInfo(REMUS::Logger::L1,"addDpPa direction ", CharString(confPtr->getDirection()));


  // tell the config how we will transform data to/from the device
  // by installing a Transformation object into the PeriphAddr
  // In this template, the Transformation type was set via the
  // configuration panel (it is already set in the PeriphAddr)

  // TODO this really depends on your protocol and is therefore just an example
  // in this example we use the ones from Pbus, as those can be selected
  // with the SIM driver parametrization panel
  switch ((uint32_t)confPtr->getTransformationType()) {
  case TransUndefinedType:
      REMUS::Logger::globalInfo(REMUS::Logger::L3,"Undefined transformation" + CharString(confPtr->getTransformationType()));
      return HWMapper::addDpPa(dpId, confPtr);
  case kafkaDrvBoolTransType:
        REMUS::Logger::globalInfo(REMUS::Logger::L3,"Bool transformation");
        confPtr->setTransform(new Transformations::kafkaBoolTrans);
        break;
  case kafkaDrvUint8TransType:
      REMUS::Logger::globalInfo(REMUS::Logger::L3,"Uint8 transformation");
      confPtr->setTransform(new Transformations::kafkaUint8Trans);
      break;
  case kafkaDrvInt32TransType:
      REMUS::Logger::globalInfo(REMUS::Logger::L3,"Int32 transformation");
      confPtr->setTransform(new Transformations::kafkaInt32Trans);
      break;
  case kafkaDrvInt64TransType:
      REMUS::Logger::globalInfo(REMUS::Logger::L3,"Int64 transformation");
      confPtr->setTransform(new Transformations::kafkaInt64Trans);
      break;
  case kafkaDrvFloatTransType:
      REMUS::Logger::globalInfo(REMUS::Logger::L3,"Float transformation");
      confPtr->setTransform(new Transformations::kafkaFloatTrans);
      break;
  case kafkaDrvStringTransType:
        REMUS::Logger::globalInfo(REMUS::Logger::L3,"String transformation");
        confPtr->setTransform(new Transformations::kafkaStringTrans);
        break;
  case kafkaDrvTimeTransType:
        REMUS::Logger::globalInfo(REMUS::Logger::L3,"Time transformation");
        confPtr->setTransform(new Transformations::kafkaTimeTrans);
        break;
  default:
      REMUS::Logger::globalError("kafkaHWMapper::addDpPa", CharString("Illegal transformation type ") + CharString((int) confPtr->getTransformationType()));
      return HWMapper::addDpPa(dpId, confPtr);
  }

  // First add the config, then the HW-Object
  if ( !HWMapper::addDpPa(dpId, confPtr) )  // FAILED !!
    return PVSS_FALSE;

  HWObject *hwObj = new HWObject;
  // Set Address and Subindex
  REMUS::Logger::globalInfo(REMUS::Logger::L3, "New Object", "name:" + confPtr->getName());
  hwObj->setConnectionId(confPtr->getConnectionId());
  hwObj->setAddress(confPtr->getName());       // Resolve the HW-Address, too

  // Set the data type.
  hwObj->setType(confPtr->getTransform()->isA());

  // Set the len needed for data from _all_ subindices of this PVSS-Address.
  // Because we will deal with subix 0 only this is the Transformation::itemSize
  hwObj->setDlen(confPtr->getTransform()->itemSize());
//TODO - number of elements?
  // Add it to the list
  addHWObject(hwObj);

  if(confPtr->getDirection() == DIRECTION_IN)
  {
      std::vector<std::string> streamOptions = REMUS::Utils::split(hwObj->getAddress().c_str());
      if (streamOptions.size() == 2) // TOPIC + KEY
      {
        addTopic(streamOptions[0]);
      }
  }

  return PVSS_TRUE;
}

//--------------------------------------------------------------------------------

PVSSboolean kafkaHWMapper::clrDpPa(DpIdentifier &dpId, PeriphAddr *confPtr)
{
  DEBUG_DRV_USR1("clrDpPa called for " << confPtr->getName());

  // Find our HWObject via a template
  HWObject adrObj;
  adrObj.setAddress(confPtr->getName());

  // Lookup HW-Object via the Name, not via the HW-Address
  // The class type isn't important here
  HWObject *hwObj = findHWAddr(&adrObj);

  if ( hwObj )
  {
    // Object exists, remove it from the list and delete it.
    clrHWObject(hwObj);
    delete hwObj;
  }

  // Call function of base class to remove config
  return HWMapper::clrDpPa(dpId, confPtr);
}

void kafkaHWMapper::addTopic(const std::string &topic)
{
    if(consumer_topics.find(topic) == consumer_topics.end())
    {
        consumer_topics.insert(topic);
        if(_newConsumerTopicCB)
        {
           _newConsumerTopicCB(topic);
        }
    }
}

//--------------------------------------------------------------------------------
