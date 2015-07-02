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

#include <signal.h>
#include <stdio.h>
#include "EnhancedPositionClient.h"
#include "PositionWebServiceAPI.h"
#include "positioning-constants.h"
#include "SharedData.h"
#include "Log.h"

using namespace std;

EnhancedPositionClient * EnhancedPositionClient::mEnhancedPositionClient = 0;

DLT_IMPORT_CONTEXT(gCtx);

EnhancedPositionClient::EnhancedPositionClient(DBus::Connection &connection, const char *path, const char *name, PositionWebServiceAPI& positionWebServiceAPI)
: DBus::ObjectProxy(connection, path, name)
, mPositionWebServiceAPI(positionWebServiceAPI)
{
  mEnhancedPositionClient = this;
}

void EnhancedPositionClient::PositionUpdate(const uint64_t& changedValues)
{
  LOG_INFO_MSG(gCtx,"Position Update");
  
  std::map< uint64_t, ::DBus::Variant > posData;
  uint64_t timestamp;
  
  GetPositionInfo(changedValues, timestamp, posData);

  FB::VariantList changedValuesList;

  SharedData& PositioningData = SharedData::getInstance();
  
  if (changedValues & GENIVI_ENHANCEDPOSITIONSERVICE_LATITUDE)
  {
    changedValuesList.push_back("LATITUDE");
    double latitude = posData[(uint64_t)GENIVI_ENHANCEDPOSITIONSERVICE_LATITUDE].reader().get_double();
    PositioningData.setLatitude(latitude);
    LOG_INFO(gCtx,"LAT=%lf", latitude);
  }

  if (changedValues & GENIVI_ENHANCEDPOSITIONSERVICE_LONGITUDE)
  {
    changedValuesList.push_back("LONGITUDE");
    double longitude = posData[(uint64_t)GENIVI_ENHANCEDPOSITIONSERVICE_LONGITUDE].reader().get_double();
    PositioningData.setLongitude(longitude);
    LOG_INFO(gCtx,"LON=%lf", longitude);
  }

  if (changedValues & GENIVI_ENHANCEDPOSITIONSERVICE_ALTITUDE)
  {
    changedValuesList.push_back("ALTITUDE");
    double altitude = posData[(uint64_t)GENIVI_ENHANCEDPOSITIONSERVICE_ALTITUDE].reader().get_double();
    PositioningData.setAltitude(altitude);
    LOG_INFO(gCtx,"ALT=%lf", altitude);  
  }

  if (changedValues & GENIVI_ENHANCEDPOSITIONSERVICE_SPEED)
  {
    changedValuesList.push_back("SPEED");
    double speed = posData[(uint64_t)GENIVI_ENHANCEDPOSITIONSERVICE_SPEED].reader().get_double();
    PositioningData.setSpeed(speed);
    LOG_INFO(gCtx,"SPEED=%lf", speed);
  }
  
  LOG_INFO_MSG(gCtx,"fire_PositionUpdate");

  mPositionWebServiceAPI.fire_PositionUpdate(changedValuesList);
}






