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
#include "enhanced-position.h"
#include "positioning-constants.h"
#include "log.h"

DLT_IMPORT_CONTEXT(gCtx);

EnhancedPosition* EnhancedPosition::mpSelf = 0;

static DBus::Variant variant_double(double d)
{
  DBus::Variant variant;
  DBus::MessageIter iter=variant.writer();
  iter << d;
  return variant;
}

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

static DBus::Variant variant_string(std::string s)
{
  DBus::Variant variant;
  DBus::MessageIter iter=variant.writer();
  iter << s;
  return variant;
}

EnhancedPosition::EnhancedPosition(DBus::Connection & connection, const char * path)
  : DBus::ObjectAdaptor(connection, path)
{
    mpSelf = this;
}

EnhancedPosition::~EnhancedPosition()
{
    mpSelf = 0;
}

::DBus::Struct< uint16_t, uint16_t, uint16_t, std::string > EnhancedPosition::GetVersion()
{
  ::DBus::Struct< uint16_t, uint16_t, uint16_t, std::string > Version;

  Version._1 = 2;
  Version._2 = 0;
  Version._3 = 0;
  Version._4 = std::string("04-06-2013");

  return Version;
}
    
std::map< uint16_t, ::DBus::Variant > EnhancedPosition::GetData(const std::vector< uint16_t >& valuesToReturn)
{
  std::map< uint16_t, ::DBus::Variant > Data;

  TGNSSPosition pos;
  TGNSSCourse course;

  bool isPosRequested = false;
  bool isCourseRequested = false;

  for (int i = 0; i < valuesToReturn.size(); i++)
  {
    if ((valuesToReturn[i] == POS_LATITUDE) || 
        (valuesToReturn[i] == POS_LONGITUDE) || 
        (valuesToReturn[i] == POS_ALTITUDE))
    {
      //if any of the three IDs above is set, then gnssSimpleGetPosition will have to be called
      isPosRequested = true;
    }

    if ((valuesToReturn[i] == POS_HEADING) || 
        (valuesToReturn[i] == POS_SPEED) || 
        (valuesToReturn[i] == POS_CLIMB ))
    {
      //if any of the three IDs above is set, then gnssSimpleGetCourse will have to be called
      isCourseRequested = true;
    }
  }

  if(isPosRequested)
  {
    if(gnssSimpleGetPosition(&pos))
    {
      if (pos.validityBits && GNSS_POSITION_LATITUDE_VALID)
      {
        Data[POS_LATITUDE] = variant_double(pos.latitude);
      }

      if (pos.validityBits && GNSS_POSITION_LONGITUDE_VALID)
      {
        Data[POS_LONGITUDE] = variant_double(pos.longitude);
      }

      if (pos.validityBits && GNSS_POSITION_ALTITUDE_VALID)
      {
        Data[POS_ALTITUDE] = variant_double(pos.altitude);
      }
    }
  }

  if(isCourseRequested)
  {
    if(gnssSimpleGetCourse(&course))
    {
      if (course.validityBits && GNSS_COURSE_SPEED_VALID)
      {
        Data[POS_HEADING] = variant_double(course.heading);
      }

      if (course.validityBits && GNSS_COURSE_CLIMB_VALID)
      {
        Data[POS_SPEED] = variant_double(course.speed);
      }

      if (course.validityBits && GNSS_COURSE_HEADING_VALID)
      {
        Data[POS_CLIMB] = variant_double(course.climb);
      }
    }
  }

  return Data;
}   

std::map< uint16_t, ::DBus::Variant > EnhancedPosition::GetPosition()
{
  std::map< uint16_t, ::DBus::Variant > Position;
  TGNSSPosition pos;
  TGNSSCourse course;
   
  if (gnssSimpleGetPosition(&pos))
  {
    if (pos.validityBits && GNSS_POSITION_LATITUDE_VALID)
    {
      Position[POS_LATITUDE] = variant_double(pos.latitude);
    }

    if (pos.validityBits && GNSS_POSITION_LONGITUDE_VALID)
    {
      Position[POS_LONGITUDE] = variant_double(pos.longitude);
    }

    if (pos.validityBits && GNSS_POSITION_ALTITUDE_VALID)
    {
      Position[POS_ALTITUDE] = variant_double(pos.altitude);
    }
  }

  if(gnssSimpleGetCourse(&course))
  {
    if (pos.validityBits && GNSS_COURSE_SPEED_VALID)
    {
      Position[POS_HEADING] = variant_double(course.heading);
    }

    if (pos.validityBits && GNSS_COURSE_CLIMB_VALID)
    {
      Position[POS_SPEED] = variant_double(course.speed);
    }

    if (pos.validityBits && GNSS_COURSE_HEADING_VALID)
    {
      Position[POS_CLIMB] = variant_double(course.climb);
    }
  }

  return Position;
}  

std::map< uint16_t, ::DBus::Variant > EnhancedPosition::GetRotationRate()
{
  std::map< uint16_t, ::DBus::Variant > RotationRate;

  throw DBus::ErrorNotSupported("Method not supported yet");

  return RotationRate;
}

std::map< uint16_t, ::DBus::Variant > EnhancedPosition::GetAccuracy()
{
  std::map< uint16_t, ::DBus::Variant > Accuracy;

  throw DBus::ErrorNotSupported("Method not supported yet");

  return Accuracy;
}

std::map< uint16_t, ::DBus::Variant > EnhancedPosition::GetSatelliteInfo()
{
  std::map< uint16_t, ::DBus::Variant > SatelliteInfo;

  throw DBus::ErrorNotSupported("Method not supported yet");

  return SatelliteInfo;
}

std::map< uint16_t, ::DBus::Variant > EnhancedPosition::GetStatus()
{
  std::map< uint16_t, ::DBus::Variant > Status;

  throw DBus::ErrorNotSupported("Method not supported yet");

  return Status;
}

std::map< uint16_t, ::DBus::Variant > EnhancedPosition::GetTime()
{
  std::map< uint16_t, ::DBus::Variant > Time;

  throw DBus::ErrorNotSupported("Method not supported yet");

  return Time;
}

void EnhancedPosition::cbPosition(const TGNSSPosition pos[], uint16_t numElements)
{
  bool latChanged = false;
  bool lonChanged = false;
  bool altChanged = false;

  std::vector< uint16_t > position;

  if (pos == NULL || numElements < 1)
  {
    LOG_ERROR_MSG(gCtx,"cbPosition failed!");
    return;
  }

  for (int i = 0; i< numElements; i++)
  {
    LOG_INFO(gCtx,"Position Update[%d/%d]: lat=%f lon=%f alt=%f",
             i+1,
             numElements,
             pos[i].latitude, 
             pos[i].longitude,
             pos[i].altitude);
        
    if (latChanged == false)
    {
      latChanged = (pos[i].validityBits && GNSS_POSITION_LATITUDE_VALID);
    }

    if (lonChanged == false)
    {
      lonChanged = (pos[i].validityBits && GNSS_POSITION_LONGITUDE_VALID);
    }

    if (altChanged == false)
    {
      altChanged = (pos[i].validityBits && GNSS_POSITION_ALTITUDE_VALID);
    }

  }

  //in a real product, the coordinates would be used for dead-reckoning.
  //in this proof of concept, the client application is simply notified 
  //about changes of latitude, longitude and/or altitude
  std::vector< uint16_t > changedValues;
  std::vector<uint16_t>::iterator it;
  it = changedValues.begin();

  if (latChanged)
  {
     it = changedValues.insert(it,POS_LATITUDE);
  }

  if (lonChanged)
  {
     it = changedValues.insert(it,POS_LONGITUDE);
  }

  if (altChanged)
  {
     it = changedValues.insert(it,POS_ALTITUDE);
  }

  if (!mpSelf)
  {
    LOG_ERROR_MSG(gCtx,"Null pointer!");    
    return;
  }
    
  //notify clients
  mpSelf->PositionUpdate(changedValues);
}

void EnhancedPosition::cbCourse(const TGNSSCourse course[], uint16_t numElements)
{
  bool speedChanged = false;
  bool headingChanged = false;
  bool climbChanged = false;
  
  if (course == NULL || numElements < 1)
  {
    LOG_ERROR_MSG(gCtx,"cbCourse failed!");
    return;
  }

  for (int i = 0; i < numElements; i++)
  {
    LOG_INFO(gCtx,"Course Update[%d/%d]: speed=%f heading=%f climb=%f",
             i+1,
             numElements,
             course[i].speed, 
             course[i].heading, 
             course[i].climb);        

    if (speedChanged == false)
    {
      speedChanged = (course[i].validityBits && GNSS_COURSE_SPEED_VALID);
    }

    if (headingChanged == false)
    {
      headingChanged = (course[i].validityBits && GNSS_COURSE_HEADING_VALID);
    }

    if (climbChanged == false)
    {
      climbChanged = (course[i].validityBits && GNSS_COURSE_CLIMB_VALID);
    }
  }

  //in a real product, the course would be used for dead-reckoning.
  //in this proof of concept, the client application is simply notified 
  //about changes of speed, heading and/or climb
  std::vector< uint16_t > changedValues;
  std::vector<uint16_t>::iterator it;
  it = changedValues.begin();
    
  if (speedChanged)
  {
     it = changedValues.insert(it,POS_SPEED);
  }

  if (headingChanged)
  {
     it = changedValues.insert(it,POS_HEADING);
  }

  if (climbChanged)
  {
     it = changedValues.insert(it,POS_CLIMB);
  }

  if (!mpSelf)
  {
    LOG_ERROR_MSG(gCtx,"Null pointer!");    
    return;
  }
    
  //notify clients
  mpSelf->PositionUpdate(changedValues);
}

void EnhancedPosition::cbAccuracy(const TGNSSAccuracy accuracy[], uint16_t numElements)
{ 
  //satellite info
  bool usedSatellitesChanged = false;
  bool trackedSatellitesChanged = false;
  bool visibleSatellitesChanged = false;
  
  //status
  bool fixStatusChanged = false;
  bool fixTypeBitsChanged = false;
  
  //accuracy
  bool pdopChanged = false;
  bool hdopChanged = false;
  bool vdopChanged = false;
  bool sigmaLatitudeChanged = false;
  bool sigmaLongitudeChanged = false;
  bool sigmaAltitudeChanged = false;

  if (accuracy == NULL || numElements < 1)
  {
      LOG_ERROR_MSG(gCtx,"cbAccuracy failed!");
      return;
  }

  for (int i = 0; i< numElements; i++)
  {
      LOG_INFO(gCtx,"Accuracy Update[%d/%d]: usedSatellites=%d trackedSatellites=%d visibleSatellites=%d",
               i+1,
               numElements,
               accuracy[i].usedSatellites, 
               accuracy[i].trackedSatellites,
               accuracy[i].visibleSatellites);

      LOG_INFO(gCtx,"Accuracy Update[%d/%d]: fixStatus=%d fixTypeBits=0x%08X",
               i+1,
               numElements,
               accuracy[i].fixStatus,
               accuracy[i].fixTypeBits);

      LOG_INFO(gCtx,"Accuracy Update[%d/%d]: pdop=%f hdop=%f vdop=%f \
               sigmaLatitude=%f sigmaLongitude=%f sigmaAltitude=%f",
               i+1,
               numElements,
               accuracy[i].pdop, 
               accuracy[i].hdop,
               accuracy[i].vdop,
               accuracy[i].sigmaLatitude,
               accuracy[i].sigmaLongitude,
               accuracy[i].sigmaAltitude);

    if (usedSatellitesChanged == false)
    {
      usedSatellitesChanged = (accuracy[i].validityBits && GNSS_ACCURACY_USAT_VALID);
    }

    if (trackedSatellitesChanged == false)
    {
      trackedSatellitesChanged = (accuracy[i].validityBits && GNSS_ACCURACY_TSAT_VALID);
    }

    if (visibleSatellitesChanged == false)
    {
      visibleSatellitesChanged = (accuracy[i].validityBits && GNSS_ACCURACY_VSAT_VALID);
    }

    if (fixStatusChanged == false)
    {
      fixStatusChanged = (accuracy[i].validityBits && GNSS_ACCURACY_STAT_VALID);
    }
    
    if (fixTypeBitsChanged == false)
    {
      fixTypeBitsChanged = (accuracy[i].validityBits && GNSS_ACCURACY_TYPE_VALID);
    }

    if (pdopChanged == false)
    {
      pdopChanged = (accuracy[i].validityBits && GNSS_ACCURACY_PDOP_VALID);
    }

    if (hdopChanged == false)
    {
      hdopChanged = (accuracy[i].validityBits && GNSS_ACCURACY_HDOP_VALID);
    }

    if (vdopChanged == false)
    {
      vdopChanged = (accuracy[i].validityBits && GNSS_ACCURACY_VDOP_VALID);
    }

    if (sigmaLatitudeChanged == false)
    {
      sigmaLatitudeChanged = (accuracy[i].validityBits && GNSS_ACCURACY_SLAT_VALID);
    }

    if (sigmaLongitudeChanged == false)
    {
      sigmaLongitudeChanged = (accuracy[i].validityBits && GNSS_ACCURACY_SLON_VALID);
    }

    if (sigmaAltitudeChanged == false)
    {
      sigmaAltitudeChanged = (accuracy[i].validityBits && GNSS_ACCURACY_SALT_VALID);
    }
  }

  //in a real product, the accuracy would be used for dead-reckoning.
  //in this proof of concept, the client application is simply notified 
  //about accuracy changes
  std::vector<uint16_t>::iterator it;
  
  std::vector< uint16_t > changedSatelliteInfoValues;
  it = changedSatelliteInfoValues.begin();

  if (usedSatellitesChanged)
  {
    it = changedSatelliteInfoValues.insert(it,POS_USED_SATELLITES);
  }

  if (trackedSatellitesChanged)
  {
    it = changedSatelliteInfoValues.insert(it,POS_TRACKED_SATELLITES);
  }

  if (visibleSatellitesChanged)
  {
    it = changedSatelliteInfoValues.insert(it,POS_VISIBLE_SATELLITES);
  }

  //todo: handle other field-changes here (accuracy and status)

  if (!mpSelf)
  {
    LOG_ERROR_MSG(gCtx,"Null pointer!");    
    return;
  }
  
  mpSelf->SatelliteInfoUpdate(changedSatelliteInfoValues);

  //todo: send other notifications
  //mpSelf->AccuracyUpdate(changedAccuracyValues);
  //mpSelf->StatusUpdate(changedStatusValues);
}

void EnhancedPosition::cbSatelliteDetail(const TGNSSSatelliteDetail satelliteDetail[], uint16_t numElements)
{
  if (satelliteDetail == NULL || numElements < 1)
  {
    LOG_ERROR_MSG(gCtx,"cbSatelliteDetail failed!");
    return;
  }

  for (int i = 0; i<numElements; i++)
  {
    LOG_INFO(gCtx,"SatelliteDetail Update[%d/%d]: satelliteId=%d azimuth=%d elevation=%d SNR=%d",
             i+1,
             numElements,
             satelliteDetail[i].satelliteId, 
             satelliteDetail[i].azimuth,
             satelliteDetail[i].elevation,                                   
             satelliteDetail[i].SNR);
  }  

  if (!mpSelf)
  {
    LOG_ERROR_MSG(gCtx,"Null pointer!");    
    return;
  }
    
  //todo: notify clients
  //mpSelf->SatelliteInfoUpdate(changedValues);
}

bool EnhancedPosition::checkMajorVersion(int expectedMajor)
{
  int major = -1;

  gnssGetVersion(&major, 0, 0);

  if (major != expectedMajor)
  {
    LOG_ERROR(gCtx,"Wrong API version: gnssGetVersion returned unexpected value %d != %d",
              major, 
              expectedMajor);

    return false;
  }

  return true;
}

void EnhancedPosition::run()
{
  if(!checkMajorVersion(2))
  {
    exit(EXIT_FAILURE);
  }

  if(!gnssInit())
  {
    exit(EXIT_FAILURE);
  }

  if(!gnssSimpleInit())
  {
    exit(EXIT_FAILURE);
  }

  LOG_INFO_MSG(gCtx,"Starting EnhancedPosition dispatcher...");

  gnssSimpleRegisterPositionCallback(&cbPosition);
  gnssSimpleRegisterCourseCallback(&cbCourse);
  gnssExtendedRegisterAccuracyCallback(&cbAccuracy);
  gnssExtendedRegisterSatelliteDetailCallback(&cbSatelliteDetail);
}

void EnhancedPosition::shutdown()
{
  LOG_INFO_MSG(gCtx,"shutting down...");

  gnssSimpleDeregisterPositionCallback(&cbPosition);
  gnssSimpleDeregisterCourseCallback(&cbCourse);
  gnssExtendedDeregisterAccuracyCallback(&cbAccuracy);
  gnssExtendedDeregisterSatelliteDetailCallback(&cbSatelliteDetail);
  gnssSimpleDestroy();
  gnssDestroy();
}


