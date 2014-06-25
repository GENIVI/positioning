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

#include <stdio.h>
#include <signal.h>
#include <dbus-c++/dbus.h>

#include "enhanced-position.h"
#include "position-feedback.h"
#include "configuration.h"
#include "log.h"

const char* ENHANCED_POSITION_SERVICE_NAME = "org.genivi.positioning.EnhancedPosition";
const char* ENHANCED_POSITION_OBJECT_PATH = "/org/genivi/positioning/EnhancedPosition";
const char* POSITION_FEEDBACK_OBJECT_PATH = "/org/genivi/positioning/PositionFeedback";
const char* CONFIGURATION_OBJECT_PATH = "/org/genivi/positioning/Configuration";

DLT_DECLARE_CONTEXT(gCtx);

DBus::BusDispatcher dispatcher;

static DBus::Connection *conn;

void sighandler(int sig)
{
  LOG_INFO_MSG(gCtx,"Signal received");

  dispatcher.leave();
}

int main()
{
  DLT_REGISTER_APP("ENHP", "EnhancedPositionService");
  DLT_REGISTER_CONTEXT(gCtx,"EPSR", "Global Context");
  LOG_INFO_MSG(gCtx,"Starting EnhancedPositionService...");

  signal(SIGTERM, sighandler);
  signal(SIGINT, sighandler);

  DBus::default_dispatcher = &dispatcher;

  conn = new DBus::Connection(DBus::Connection::SessionBus());

  if (conn == NULL)
  {
    LOG_ERROR_MSG(gCtx,"Null pointer!");
  }

  conn->setup(&dispatcher);
  conn->request_name(ENHANCED_POSITION_SERVICE_NAME);

  EnhancedPosition EnhancedPositionServer(*conn, ENHANCED_POSITION_OBJECT_PATH); 
  PositionFeedback PositionFeedbackServer(*conn, POSITION_FEEDBACK_OBJECT_PATH);
  Configuration ConfigurationServer(*conn, CONFIGURATION_OBJECT_PATH);
  
  EnhancedPositionServer.run();
  PositionFeedbackServer.run();
  ConfigurationServer.run();

  dispatcher.enter();

  ConfigurationServer.shutdown();
  PositionFeedbackServer.shutdown();
  EnhancedPositionServer.shutdown();
  
  return 0;
}
