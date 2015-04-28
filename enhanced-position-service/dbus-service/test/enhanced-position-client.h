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
#ifndef __ENHANCED_POSITION_CLIENT_H
#define __ENHANCED_POSITION_CLIENT_H

#include <dbus-c++/dbus.h>
#include "enhanced-position-proxy.h"

class EnhancedPositionClient
  : public org::genivi::positioning::EnhancedPosition_proxy,
  public DBus::IntrospectableProxy,
  public DBus::ObjectProxy
{
public:

  EnhancedPositionClient(DBus::Connection &connection, const char *path, const char *name);

  void PositionUpdate(const uint64_t& changedValues);

};

#endif//__ENHANCED_POSITION_CLIENT_H
