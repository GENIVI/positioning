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
#ifndef ___CONFIGURATION_H
#define ___CONFIGURATION_H

#include <dbus-c++/dbus.h>
#include "configuration-adaptor.h"

class Configuration
  : public org::genivi::positioning::Configuration_adaptor
  , public DBus::IntrospectableAdaptor
  , public DBus::ObjectAdaptor
{
public:

  Configuration(DBus::Connection &connection, const char * path);

  ~Configuration();
  
  ::DBus::Struct< uint16_t, uint16_t, uint16_t, std::string > GetVersion();
    
  std::map< std::string, ::DBus::Variant > GetProperties();

  void SetProperty(const std::string& name, const ::DBus::Variant& value);

  std::map< std::string, ::DBus::Variant > GetSupportedProperties();

  void run();

  void shutdown();

private:

  int32_t mUpdateInterval;
  uint32_t mSatelliteSystem;
};

#endif//__CONFIGURATION_H
