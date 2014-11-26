/**************************************************************************
* @licence app begin@
*
* SPDX-License-Identifier: MPL-2.0
*
* \ingroup EnhancedPositionService
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

#include "enhanced-position-client.h"
#include "positioning-constants.h"
#include "log.h"

using namespace std;
DBus::BusDispatcher dispatcher;
DBus::DefaultTimeout *timeout;

DLT_DECLARE_CONTEXT(gCtx);

EnhancedPositionClient::EnhancedPositionClient(DBus::Connection &connection, const char *path, const char *name)
  : DBus::ObjectProxy(connection, path, name)
{
}

void EnhancedPositionClient::PositionUpdate(const uint64_t& changedValues)
{
  LOG_INFO_MSG(gCtx,"Position Update");
  
  // retrieve the data 
  uint64_t timestamp;
  std::map< uint64_t, ::DBus::Variant > posData;
    
  GetPositionInfo(changedValues, timestamp, posData);

  if (changedValues & POS_LATITUDE)
  {
    LOG_INFO(gCtx,"LAT=%lf", posData[POS_LATITUDE].reader().get_double());
  }

  if (changedValues & POS_LONGITUDE)
  {
    LOG_INFO(gCtx,"LON=%lf", posData[POS_LONGITUDE].reader().get_double());
  }

  if (changedValues & POS_ALTITUDE)
  {
    LOG_INFO(gCtx,"ALT=%lf", posData[POS_ALTITUDE].reader().get_double());
  }

  if (changedValues & POS_SPEED)
  {
    LOG_INFO(gCtx,"SPEED=%lf", posData[POS_SPEED].reader().get_double());
  }

  if (changedValues & POS_CLIMB)
  {
    LOG_INFO(gCtx,"CLIMB=%lf", posData[POS_CLIMB].reader().get_double());
  }

  if (changedValues & POS_HEADING)
  {
    LOG_INFO(gCtx,"HEADING=%lf", posData[POS_HEADING].reader().get_double());
  }


}

void signalhandler(int sig)
{
  LOG_INFO_MSG(gCtx,"Signal received");
  dispatcher.leave();
}

int main()
{
  DLT_REGISTER_APP("ENHPOS", "EnhancedPositionClient");
  DLT_REGISTER_CONTEXT(gCtx,"EPCL", "Global Context"); // EPCL = EnhancedPositionService Client Application

  LOG_INFO_MSG(gCtx,"Starting EnhancedPositionClient...");

  signal(SIGTERM, signalhandler);
  signal(SIGINT, signalhandler);

  DBus::default_dispatcher = &dispatcher;

  DBus::Connection conn = DBus::Connection::SessionBus();

  EnhancedPositionClient client(conn, 
                                "/org/genivi/positioning/EnhancedPosition",
                                "org.genivi.positioning.EnhancedPosition");

  // dispatch
  dispatcher.enter();

  return 0;
}
