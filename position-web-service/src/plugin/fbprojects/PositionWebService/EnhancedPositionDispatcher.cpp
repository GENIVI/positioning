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
#include <stdio.h>

#include "EnhancedPositionDispatcher.h"
#include "PositionWebServiceAPI.h"
#include <dbus-c++/dbus.h>

pthread_t dispatcherThread;
DBus::BusDispatcher dispatcher;

void *dispatchdbusmsg( void *ptr )
{ 
  dispatcher.enter();
}

EnhancedPositionDispatcher::EnhancedPositionDispatcher(PositionWebServiceAPI& positionWebServiceAPI)
{
  DBus::default_dispatcher = &dispatcher;

  DBus::Connection conn = DBus::Connection::SessionBus();

  mpEnhancedPositionClient = new EnhancedPositionClient(conn, "/org/genivi/positioning/EnhancedPosition",
                                "org.genivi.positioning.EnhancedPosition", positionWebServiceAPI);
}

EnhancedPositionDispatcher::~EnhancedPositionDispatcher()
{
  stop();

  if(mpEnhancedPositionClient)
  {
    delete mpEnhancedPositionClient;
  }
}

void EnhancedPositionDispatcher::dispatch()
{
  pthread_create(&dispatcherThread, NULL, dispatchdbusmsg, NULL);
}

void EnhancedPositionDispatcher::stop()
{
  dispatcher.leave();

  if(dispatcherThread)
  {
    pthread_join(dispatcherThread, NULL);
  }
}

EnhancedPositionClient * EnhancedPositionDispatcher::getClient()
{ 
  return mpEnhancedPositionClient; 
}
