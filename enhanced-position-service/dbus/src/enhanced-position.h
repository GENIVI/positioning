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
#ifndef ___ENHANCED_POSITION_H
#define ___ENHANCED_POSITION_H

#include <dbus-c++/dbus.h>

#include "enhanced-position-adaptor.h"
#include "gnss-init.h"
#include "gnss.h"

class EnhancedPosition
  : public org::genivi::positioning::EnhancedPosition_adaptor
  , public DBus::IntrospectableAdaptor
  , public DBus::ObjectAdaptor
{
public:

  EnhancedPosition(DBus::Connection & connection, const char * path);

  ~EnhancedPosition();

  ::DBus::Struct< uint16_t, uint16_t, uint16_t, std::string > GetVersion();

  void GetPositionInfo(const uint64_t& valuesToReturn, uint64_t& timestamp, std::map< uint64_t, ::DBus::Variant >& data);
  void GetSatelliteInfo(uint64_t& timestamp, std::vector< ::DBus::Struct< uint16_t, uint16_t, uint16_t, uint16_t, uint16_t, bool > >& satelliteInfo);
  void GetTime(uint64_t& timestamp, std::map< uint64_t, ::DBus::Variant >& time);

  void run();

  void shutdown();

private:

  static void cbSatelliteDetail(const TGNSSSatelliteDetail satelliteDetail[], uint16_t numElements);
  static void cbPosition(const TGNSSPosition position[], uint16_t numElements);

  static void sigPositionUpdate(const TGNSSPosition position[], uint16_t numElements);

  static EnhancedPosition* mpSelf;
};

#endif//__ENHANCED_POSITION_H
