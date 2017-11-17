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

#ifndef ___POSITION_FEEDBACK_H
#define ___POSITION_FEEDBACK_H

#ifndef DBUS_HAS_RECURSIVE_MUTEX
#define DBUS_HAS_RECURSIVE_MUTEX
#endif
#include <dbus-c++/dbus.h>
#include "position-feedback-adaptor.h"

class PositionFeedback
  : public org::genivi::positioning::PositionFeedback_adaptor
  , public DBus::IntrospectableAdaptor
  , public DBus::ObjectAdaptor
{

public:

  PositionFeedback(DBus::Connection & connection, const char * path);

  ~PositionFeedback();

  ::DBus::Struct< uint16_t, uint16_t, uint16_t, std::string > GetVersion();

  void SetPositionFeedback(const std::vector< std::map< uint64_t, ::DBus::Variant > >& feedback, const uint64_t& timestamp, const uint16_t& feedbackType);
  
  void run();

  void shutdown();

};

#endif//___POSITION_FEEDBACK_H
