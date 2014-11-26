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

#include "position-feedback.h"
#include "log.h"

DLT_IMPORT_CONTEXT(gCtx);

PositionFeedback::PositionFeedback(DBus::Connection & connection, const char * path)
  : DBus::ObjectAdaptor(connection, path) 
{
}  

PositionFeedback::~PositionFeedback()
{
}

::DBus::Struct< uint16_t, uint16_t, uint16_t, std::string > PositionFeedback::GetVersion()
{
  ::DBus::Struct< uint16_t, uint16_t, uint16_t, std::string > Version;

  Version._1 = 3;
  Version._2 = 0;
  Version._3 = 0;
  Version._4 = std::string("05-08-2014");

  return Version;
}

void PositionFeedback::SetPositionFeedback(const std::vector< std::map< uint64_t, ::DBus::Variant > >& feedback, const uint64_t& timestamp, const uint16_t& feedbackType)
{
  throw DBus::ErrorNotSupported("Method not supported yet");
}

void PositionFeedback::run()
{
  LOG_INFO_MSG(gCtx,"Starting PositionFeedback dispatcher...");
}

void PositionFeedback::shutdown()
{
  LOG_INFO_MSG(gCtx,"Shutting down PositionFeedback dispatcher...");
}


