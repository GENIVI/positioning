/**************************************************************************
* @licence app begin@
*
* SPDX-License-Identifier: MPL-2.0
*
* \ingroup EnhancedPositionService
* \author Marco Residori <marco.residori@xse.de>
*
* \copyright Copyright (C) 2015, XS Embedded GmbH
* 
* \license
* This Source Code Form is subject to the terms of the
* Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
* this file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* @licence end@
**************************************************************************/

#include "EnhancedPositionStubImpl.h"
#include "log.h"

//EnhancedPosition-interface version
#define VER_MAJOR 3
#define VER_MINOR 0
#define VER_MICRO 0
#define VER_DATE "05-08-2014"

DLT_IMPORT_CONTEXT(gCtx);

using namespace org::genivi::EnhancedPositionService::EnhancedPositionServiceTypes;

EnhancedPositionStubImpl* EnhancedPositionStubImpl::mpSelf = 0;

EnhancedPositionStubImpl::EnhancedPositionStubImpl() {
    mpSelf = this;
}

EnhancedPositionStubImpl::~EnhancedPositionStubImpl() {
}

void EnhancedPositionStubImpl::sigPositionUpdate(const TGNSSPosition position[], uint16_t numElements)
{
  bool latChanged = false;
  bool lonChanged = false;
  bool altChanged = false;
  bool speedChanged = false;
  bool headingChanged = false;
  bool climbChanged = false;
  bool pdopChanged = false;
  bool hdopChanged = false;
  bool vdopChanged = false;
  bool usatChanged = false; //used satellites
  bool fixStatusChanged = false;
  bool fixTypeBitsChanged = false;
  
  Bitmask changedValues = 0; 

  if (position == NULL || numElements < 1)
  {
    LOG_ERROR_MSG(gCtx,"sigPositionUpdate failed!");
    return;
  }

  for (int i = 0; i< numElements; i++)
  {
    if (latChanged == false)
    {
      latChanged = (position[i].validityBits & GNSS_POSITION_LATITUDE_VALID);
    }

    if (lonChanged == false)
    {
      lonChanged = (position[i].validityBits & GNSS_POSITION_LONGITUDE_VALID);
    }

    if (altChanged == false)
    {
      altChanged = (position[i].validityBits & GNSS_POSITION_ALTITUDEMSL_VALID);
    }

    if (speedChanged == false)
    {
      speedChanged = (position[i].validityBits & GNSS_POSITION_HSPEED_VALID);
    }

    if (headingChanged == false)
    {
      headingChanged = (position[i].validityBits & GNSS_POSITION_HEADING_VALID);
    }

    if (climbChanged == false)
    {
      climbChanged = (position[i].validityBits & GNSS_POSITION_VSPEED_VALID);
    }

    if (pdopChanged == false)
    {
      pdopChanged = (position[i].validityBits & GNSS_POSITION_PDOP_VALID);
    }

    if (hdopChanged == false)
    {
      hdopChanged = (position[i].validityBits & GNSS_POSITION_HDOP_VALID);
    }

    if (vdopChanged == false)
    {
      vdopChanged = (position[i].validityBits & GNSS_POSITION_VDOP_VALID);
    }

    if (usatChanged == false)
    {
      usatChanged = (position[i].validityBits & GNSS_POSITION_USAT_VALID);
    }

    if (fixStatusChanged == false)
    {
      fixStatusChanged = (position[i].validityBits & GNSS_POSITION_STAT_VALID);
    }

    if (fixTypeBitsChanged == false)
    {
      fixTypeBitsChanged = (position[i].validityBits & GNSS_POSITION_TYPE_VALID);
    }

    //extend this OR statement if necessary (when more notifications are supported)
    if(latChanged || lonChanged || altChanged)
    {
        LOG_INFO(gCtx,"Position Update[%d/%d]: lat=%f, lon=%f, alt=%f",
             i+1,
       		 numElements,
       		 position[i].latitude,
       		 position[i].longitude,
       		 position[i].altitudeMSL);

        /*LOG_INFO(gCtx,"Position Update[%d/%d]: \n \
                     lat=%f \n \
            		 lon=%f \n \
            		 alt=%f \n \
            		 speed=%f \n \
            		 heading=%f \n \
            		 climb=%f \n \
            		 pdop=%f \n \
            		 hdop=%f \n \
            		 vdop=%f \n \
            		 sigmaHPosition=%f \n \
            		 sigmaAltitude=%f \n \
            		 usedSatellites=%d \n \
            		 fixStatus=%d \n \
            		 fixTypeBits=0x%08X \n",
            		 i+1,
            		 numElements,
            		 position[i].latitude,
            		 position[i].longitude,
            		 position[i].altitudeMSL,
            		 position[i].hSpeed,
            		 position[i].heading,
            		 position[i].vSpeed,
            		 position[i].pdop,
            		 position[i].hdop,
            		 position[i].vdop,
            		 position[i].sigmaHPosition,
            		 position[i].sigmaAltitude,
            		 position[i].usedSatellites,
            		 position[i].fixStatus,
            		 position[i].fixTypeBits);*/
     }
  }

  //in a real product, the coordinates would be used for dead-reckoning.
  //in this proof of concept, the client application is simply notified 
  //about changes of latitude, longitude and/or altitude

  if (latChanged)
  {
     changedValues |= static_cast<Bitmask>(PositionInfoKey::LATITUDE);
  }

  if (lonChanged)
  {
     changedValues |= static_cast<Bitmask>(PositionInfoKey::LONGITUDE);
  }

  if (altChanged)
  {
     changedValues |= static_cast<Bitmask>(PositionInfoKey::ALTITUDE);
  }

  if (speedChanged)
  {
     changedValues |= static_cast<Bitmask>(PositionInfoKey::SPEED);
  }

  if (headingChanged)
  {
     changedValues |= static_cast<Bitmask>(PositionInfoKey::HEADING);
  }

  if (climbChanged)
  {
     changedValues |= static_cast<Bitmask>(PositionInfoKey::CLIMB);
  }

  if (pdopChanged)
  {
    changedValues |= static_cast<Bitmask>(PositionInfoKey::PDOP);
  }

  if (hdopChanged)
  {
    changedValues |= static_cast<Bitmask>(PositionInfoKey::HDOP);
  }
  
  if (vdopChanged)
  {
    changedValues |= static_cast<Bitmask>(PositionInfoKey::VDOP);
  }  

  if (usatChanged)
  {
    changedValues |= static_cast<Bitmask>(PositionInfoKey::USED_SATELLITES);
  }

  if (fixStatusChanged)
  {
    changedValues |= static_cast<Bitmask>(PositionInfoKey::GNSS_FIX_STATUS);
  }

  if (!mpSelf)
  {
      LOG_ERROR_MSG(gCtx,"Null pointer!");    
      return;
  }

  //extend this OR statement if necessary (when more notifications are supported)
  if(latChanged || lonChanged || altChanged)
  {
    mpSelf->firePositionUpdateEvent(changedValues);
  }

}

void EnhancedPositionStubImpl::cbPosition(const TGNSSPosition position[], uint16_t numElements)
{
    sigPositionUpdate(position, numElements);
}

void EnhancedPositionStubImpl::cbSatelliteDetail(const TGNSSSatelliteDetail satelliteDetail[], uint16_t numElements)
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

  /*if (!mpSelf)
  {
      LOG_ERROR_MSG(gCtx,"Null pointer!");    
      return;
  }*/

  //mpSelf->fireSatelliteInfoUpdateEvent(changedValues);
}

bool EnhancedPositionStubImpl::checkMajorVersion(int expectedMajor)
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

void EnhancedPositionStubImpl::run()
{
    if(!checkMajorVersion(3))
    {
        exit(EXIT_FAILURE);
    }

    if(!gnssInit())
    {
        exit(EXIT_FAILURE);
    }

    LOG_INFO_MSG(gCtx,"Starting EnhancedPosition dispatcher...");

    gnssRegisterPositionCallback(&cbPosition);
    gnssRegisterSatelliteDetailCallback(&cbSatelliteDetail);
}

void EnhancedPositionStubImpl::GetVersion(EnhancedPositionServiceTypes::Version& version) {
    LOG_INFO_MSG(gCtx,"GetVersion");

    version.maj = VER_MAJOR;
    version.min = VER_MINOR;
    version.mic = VER_MICRO;
    version.date = std::string(VER_DATE);
}

void EnhancedPositionStubImpl::GetPositionInfo(EnhancedPositionServiceTypes::Bitmask valuesToReturn, EnhancedPositionServiceTypes::Timestamp& timestamp, EnhancedPositionServiceTypes::PositionInfo& data)
{
    TGNSSPosition position;

    bool isPosRequested = false;
    bool isCourseRequested = false;

    if ((valuesToReturn & static_cast<Bitmask>(PositionInfoKey::LATITUDE)) ||
        (valuesToReturn & static_cast<Bitmask>(PositionInfoKey::LONGITUDE)) ||
        (valuesToReturn & static_cast<Bitmask>(PositionInfoKey::ALTITUDE)))
    {
        isPosRequested = true;
    }

    if ((valuesToReturn & static_cast<Bitmask>(PositionInfoKey::HEADING)) ||
        (valuesToReturn & static_cast<Bitmask>(PositionInfoKey::SPEED)) ||
        (valuesToReturn & static_cast<Bitmask>(PositionInfoKey::CLIMB)))
    {
        isCourseRequested = true;
    }

    if(gnssGetPosition(&position))
    {
        timestamp = position.timestamp;

        if(isPosRequested)
        {
            if (position.validityBits & GNSS_POSITION_LATITUDE_VALID)
            {
                data[PositionInfoKey::LATITUDE] = position.latitude;
            }

            if (position.validityBits & GNSS_POSITION_LONGITUDE_VALID)
            {
                data[PositionInfoKey::LONGITUDE] = position.longitude;
            }

            if (position.validityBits & GNSS_POSITION_ALTITUDEMSL_VALID)
            {
                data[PositionInfoKey::ALTITUDE] = position.altitudeMSL;
            }
        }

        if(isCourseRequested)
        {
            if (position.validityBits & GNSS_POSITION_HEADING_VALID)
            {
                data[PositionInfoKey::HEADING] = position.heading;
            }

            if (position.validityBits & GNSS_POSITION_HSPEED_VALID)
            {
                data[PositionInfoKey::SPEED] = position.hSpeed;
            }

            if (position.validityBits & GNSS_POSITION_VSPEED_VALID)
            {
                data[PositionInfoKey::CLIMB] = position.vSpeed;
            }
        }
    }
}

void EnhancedPositionStubImpl::shutdown()
{
  LOG_INFO_MSG(gCtx,"Shutting down EnhancedPosition dispatcher...");

  gnssDeregisterPositionCallback(&cbPosition);
  gnssDeregisterSatelliteDetailCallback(&cbSatelliteDetail);
  gnssDestroy();
}
