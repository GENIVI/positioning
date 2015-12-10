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

#include "configuration.h"
#include "positioning-constants.h"
#include "log.h"
#include "gnss.h"

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

static DBus::Variant variant_uint32(uint32_t i)
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

static DBus::Variant variant_array_uint32(std::vector< uint32_t > i)
{
  DBus::Variant variant;
  DBus::MessageIter iter=variant.writer();
  iter << i;
  return variant;
}

Configuration::Configuration(DBus::Connection &connection, const char * path)
: DBus::ObjectAdaptor(connection, path)
, mUpdateInterval(1000)
, mSatelliteSystem(GENIVI_ENHANCEDPOSITIONSERVICE_GPS)
{
}

Configuration::~Configuration()
{
}

::DBus::Struct< uint16_t, uint16_t, uint16_t, std::string > Configuration::GetVersion()
{
  ::DBus::Struct< uint16_t, uint16_t, uint16_t, std::string > Version;

  Version._1 = 4;
  Version._2 = 0;
  Version._3 = 0;
  Version._4 = std::string("28-10-2015alpha");

  return Version;
}
    
std::map< std::string, ::DBus::Variant > Configuration::GetProperties()
{
  std::map< std::string, ::DBus::Variant > Properties;

  Properties["UpdateInterval"] = variant_int32(mUpdateInterval);
  Properties["SatelliteSystem"] = variant_uint32(mSatelliteSystem);

  return Properties;
}

void Configuration::SetProperty(const std::string& name, const ::DBus::Variant& value)
{
  if(name == "UpdateInterval")
  {
    mUpdateInterval = value;

    LOG_INFO(gCtx,"UpdateInterval = %d", mUpdateInterval);

    PropertyChanged("UpdateInterval", value);
  }

  if(name == "SatelliteSystem")
  {
    uint32_t requestedSystems = value;  //magic conversion variant -> uint32_t
    uint32_t activateSystems = 0;
    if (requestedSystems & GENIVI_ENHANCEDPOSITIONSERVICE_GPS)
    {
        activateSystems |= GNSS_SYSTEM_GPS;
    }
    if (requestedSystems & GENIVI_ENHANCEDPOSITIONSERVICE_GLONASS)
    {
        activateSystems |= GNSS_SYSTEM_GLONASS;
    }
    if (requestedSystems & GENIVI_ENHANCEDPOSITIONSERVICE_GALILEO)
    {
        activateSystems |= GNSS_SYSTEM_GALILEO;
    }
    if (requestedSystems & GENIVI_ENHANCEDPOSITIONSERVICE_GPS)
    {
        activateSystems |= GNSS_SYSTEM_BEIDOU;
    }
    gnssSetGNSSSystems(activateSystems);

    //For a real implementation the property should only be changed 
    //when the the configuration request has become effective as reported
    //by the TGNSSPosition.activatedSystems from the GNSSPositionCallback.
    //As currently the GNSS PoC has only dummy implementations of 
    //gnssConfigGNSSSystems(), the property change is triggered immediately
    //for demonstration purposes.
    mSatelliteSystem = value;
    LOG_INFO(gCtx,"SatelliteSystem = %d", mSatelliteSystem);
    PropertyChanged("SatelliteSystem", value);
  }

}

std::map< std::string, ::DBus::Variant > Configuration::GetSupportedProperties()
{
  std::map< std::string, ::DBus::Variant > SupportedProperties;

  std::vector< int32_t > updateIntervals;
  updateIntervals.push_back(1000);
  updateIntervals.push_back(1500);

  uint32_t supportedSystems;
  TGNSSConfiguration gnssConfig = {0}; 

  if (gnssGetConfiguration(&gnssConfig) && (gnssConfig.validityBits & GNSS_CONFIG_SATSYS_VALID))
  { //assume GPS at least
    supportedSystems = gnssConfig.supportedSystems;
  }
  else    
  { //assume GPS at least
    supportedSystems = GNSS_SYSTEM_GPS;
  }
  
  //test hack: add GLONASS to have 2 systems
  supportedSystems = supportedSystems|GNSS_SYSTEM_GLONASS;

  std::vector< uint32_t > satelliteSystems;
  if (supportedSystems & GNSS_SYSTEM_GPS)
  {
    satelliteSystems.push_back(GENIVI_ENHANCEDPOSITIONSERVICE_GPS);
  }
  if (supportedSystems & GNSS_SYSTEM_GLONASS)
  {
    satelliteSystems.push_back(GENIVI_ENHANCEDPOSITIONSERVICE_GLONASS);
  }
  if (supportedSystems & GNSS_SYSTEM_GALILEO)
  {
    satelliteSystems.push_back(GENIVI_ENHANCEDPOSITIONSERVICE_GALILEO);
  }
  if (supportedSystems & GNSS_SYSTEM_BEIDOU)
  {
    satelliteSystems.push_back(GENIVI_ENHANCEDPOSITIONSERVICE_COMPASS);
  }

  SupportedProperties["UpdateInterval"] = variant_array_int32(updateIntervals);
  SupportedProperties["SatelliteSystem"] =  variant_array_uint32(satelliteSystems);

  return SupportedProperties;
}

void Configuration::run()
{
  LOG_INFO_MSG(gCtx,"Starting Configuration dispatcher...");
}

void Configuration::shutdown()
{
  LOG_INFO_MSG(gCtx,"Shutting down Configuration dispatcher...");
}
