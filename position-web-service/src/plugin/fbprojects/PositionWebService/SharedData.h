/**************************************************************************
* @licence app begin@
*
* SPDX-License-Identifier: MPL-2.0
*
* \ingroup PositionWebService
* \author Marco Residori <marco.residori@xse.de>
*
* \copyright Copyright (C) 2014, XS Embedded GmbH
* 
* \license
* This Source Code Form is subject to the terms of the
* Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
* this file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* @licence end@
**************************************************************************/
#ifndef __SHARED_DATA_H
#define __SHARED_DATA_H

#include <pthread.h>

class SharedData
{

public:

  static SharedData& getInstance( )
  {
     static SharedData mInstance;
     return mInstance;
  }
  
  void setLatitude(double val);
  void getLatitude(double& val);

  void setLongitude(double val);
  void getLongitude(double& val);

  void setAltitude(double val);
  void getAltitude(double& val);

  void setSpeed(double val);
  void getSpeed(double& val);

private:

  SharedData();
  ~SharedData(){};
  SharedData(SharedData const&){};
  SharedData& operator=(SharedData const&){};

  double mLatitude;
  double mLongitude;
  double mAltitude;
  double mSpeed;

  pthread_mutex_t mMutex;

};

#endif//__SHARED_DATA_H
