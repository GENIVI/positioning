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
#include "configuration.h"
#include "positioning-constants.h"
#include "log.h"

DLT_IMPORT_CONTEXT(gCtx);

static DBus::Variant variant_uint16(uint16_t i)
{
  DBus::Variant variant;
  DBus::MessageIter iter=variant.writer();
  iter << i;
  return variant;
}

static DBus::Variant variant_int32(int32_t i)
{
  DBus::Variant variant;
  DBus::MessageIter iter=variant.writer();
  iter << i;
  return variant;
}

static DBus::Variant variant_array_uint16(std::vector< uint16_t > i)
{
	DBus::Variant variant;
	DBus::MessageIter iter=variant.writer();
	iter << i;
	return variant;
}

static DBus::Variant variant_array_int32(std::vector< int32_t > i)
{
	DBus::Variant variant;
	DBus::MessageIter iter=variant.writer();
	iter << i;
	return variant;
}

Configuration::Configuration(DBus::Connection &connection, const char * path)
  : DBus::ObjectAdaptor(connection, path)
{
}

Configuration::~Configuration()
{
}

::DBus::Struct< uint16_t, uint16_t, uint16_t, std::string > Configuration::GetVersion()
{
  ::DBus::Struct< uint16_t, uint16_t, uint16_t, std::string > Version;

  Version._1 = 2;
  Version._2 = 0;
  Version._3 = 0;
  Version._4 = std::string("04-06-2013");

  return Version;
}
    
std::map< std::string, ::DBus::Variant > Configuration::GetProperties()
{
  std::map< std::string, ::DBus::Variant > Properties;

  Properties["UpdateInterval"] = variant_int32(1000);
  Properties["SatelliteSystem"] = variant_uint16(POS_GPS);

  return Properties;
}

void Configuration::SetProperty(const std::string& name, const ::DBus::Variant& value)
{
  throw DBus::ErrorNotSupported("Method not supported yet");
}

std::map< std::string, ::DBus::Variant > Configuration::GetSupportedProperties()
{
  std::map< std::string, ::DBus::Variant > SupportedProperties;

  std::vector< int32_t > updateIntervals;
  updateIntervals.push_back(1000);

  std::vector< uint16_t > satelliteSystems;
  satelliteSystems.push_back(POS_GPS);
  satelliteSystems.push_back(POS_GLONASS);
  satelliteSystems.push_back(POS_GALILEO);
  satelliteSystems.push_back(POS_COMPASS);

  SupportedProperties["UpdateInterval"] = variant_array_int32(updateIntervals);
  SupportedProperties["SatelliteSystem"] =  variant_array_uint16(satelliteSystems);

  return SupportedProperties;
}

void Configuration::run()
{
  LOG_INFO_MSG(gCtx,"Starting Configuration dispatcher...");
}

void Configuration::shutdown()
{
}










