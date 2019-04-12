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

#ifndef kafkaRESOURCES_H_
#define kafkaRESOURCES_H_

// Our Resources class
// This class has two tasks:
//  - Interpret commandline and read config file
//  - Be an interface to internal datapoints

#include <DrvRsrce.hxx>

class kafkaResources : public DrvRsrce
{
  public:

    static void init(int &argc, char *argv[]); // Initialize statics
    static PVSSboolean readSection();          // read config file
    static kafkaResources& GetInstance();

    // Get the number of names we need the DpId for
    virtual int getNumberOfDpNames();

    // TODO in this template we do not use internal DPs in the driver
    // If you need DPs, then also some other methods must be implemented
    kafkaResources(kafkaResources const&)   = delete;
    void operator= (kafkaResources const&)  = delete;
  private:
    kafkaResources(){}

};

#endif
