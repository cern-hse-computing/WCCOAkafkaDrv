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

#ifndef kafkaDRV_H_
#define kafkaDRV_H_

#include <DrvManager.hxx>

class kafkaDrv : public DrvManager
{
  public:
    virtual void install_HWMapper();
    virtual void install_HWService();
    virtual void install_AlertService();

    virtual HWObject *getHWObject() const; 
};

#endif
