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

// Our Resource class.
// This class will interpret the command line and read the config file

#include "kafkaResources.hxx"
#include "Common/Logger.hxx"
#include "Common/Constants.hxx"
#include <ErrHdl.hxx>

//-------------------------------------------------------------------------------
// init is a wrapper around begin, readSection and end

void kafkaResources::init(int &argc, char *argv[])
{
  // Prepass of commandline arguments, e.g. get the arguments needed to
  // find the config file.
  begin(argc, argv);

  // Read the config file
  while ( readSection() || generalSection() ) ;

  // Postpass of the commandline arguments, e.g. get the arguments that
  // will override entries in the config file
  end(argc, argv);
}

//-------------------------------------------------------------------------------

PVSSboolean kafkaResources::readSection()
{
  // Are we in our section ?
  if ( !isSection("kafka") ) return PVSS_FALSE;

  // skip "[kafka]"
  getNextEntry();

  // Now read the section until new section or end of file
  try{
    while ( (cfgState != CFG_SECT_START) && (cfgState != CFG_EOF) )
    {
      // TODO whatever you have to read from the config file
      if( keyWord.startsWith(Common::Constants::PRODUCER_CONFIG_KEYWORD))
      {
          std::string key = keyWord.substring(strlen(Common::Constants::PRODUCER_CONFIG_KEYWORD)).operator const char *();
          std::string value;
          cfgStream >> value;
          Common::Constants::setProducerConfig(key, value);
          Common::Logger::globalInfo(Common::Logger::L1, " kafkaResources::readSection Added PRODUCER key/value:", key.c_str(), value.c_str());
      }
      else if (keyWord.startsWith(Common::Constants::CONSUMER_CONFIG_KEYWORD))
      {
          std::string key = keyWord.substring(strlen(Common::Constants::CONSUMER_CONFIG_KEYWORD)).operator const char *();
          std::string value;
          cfgStream >> value;
          Common::Constants::setConsumerConfig(key, value);
          Common::Logger::globalInfo(Common::Logger::L1, " kafkaResources::readSection CONSUMER Added key/value:", key.c_str(), value.c_str());
      }
      else if ( !commonKeyWord() )
      {
        ErrHdl::error(ErrClass::PRIO_WARNING,    // Not that bad
                      ErrClass::ERR_PARAM,       // Error is with config file, not with driver
                      ErrClass::ILLEGAL_KEYWORD, // Illegal keyword in res.
                      keyWord);

        // signal Error, so we stop later
        Resources::setCfgError();
       }

      // get next entry
      getNextEntry();
    }
  }
  catch(std::runtime_error& e){
      Common::Logger::globalError(e.what());
      return PVSS_FALSE;
  }
  // So the loop will stop at the end of the file
  return cfgState != CFG_EOF;
}


kafkaResources& kafkaResources::GetInstance()
{
    static kafkaResources krs;
    return krs;
}

//-------------------------------------------------------------------------------
// Interface to internal Datapoints
// Get the number of names we need the DpId for

int kafkaResources::getNumberOfDpNames()
{
  // TODO if you use internal DPs
  return 0;
}

//-------------------------------------------------------------------------------
