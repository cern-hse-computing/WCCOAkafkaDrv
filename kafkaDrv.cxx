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

#include <kafkaDrv.hxx>
#include <kafkaHWMapper.hxx>
#include <kafkaHWService.hxx>

#include <HWObject.hxx>

//------------------------------------------------------------------------------------

void kafkaDrv::install_HWMapper()
{
  hwMapper = new kafkaHWMapper;
}

//--------------------------------------------------------------------------------

void kafkaDrv::install_HWService()
{
  hwService = new kafkaHWService;
}

//--------------------------------------------------------------------------------

HWObject * kafkaDrv::getHWObject() const
{
  return new HWObject();
}

//--------------------------------------------------------------------------------

void kafkaDrv::install_AlertService()
{
  DrvManager::install_AlertService();
}
