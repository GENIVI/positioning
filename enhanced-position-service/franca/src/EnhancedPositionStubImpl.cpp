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

#include "EnhancedPositionStubImpl.hpp"
#include "log.h"

//EnhancedPosition-interface version
#define VER_MAJOR 4
#define VER_MINOR 0
#define VER_MICRO 0
#define VER_DATE "25-04-2016"

DLT_IMPORT_CONTEXT(gCtx);

using namespace org::genivi::EnhancedPositionService;

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
  
  EnhancedPositionServiceTypes::Bitmask changedValues = 0; 

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
     changedValues |= static_cast<EnhancedPositionServiceTypes::Bitmask>(EnhancedPositionServiceTypes::PositionInfoKey::LATITUDE);
  }

  if (lonChanged)
  {
     changedValues |= static_cast<EnhancedPositionServiceTypes::Bitmask>(EnhancedPositionServiceTypes::PositionInfoKey::LONGITUDE);
  }

  if (altChanged)
  {
     changedValues |= static_cast<EnhancedPositionServiceTypes::Bitmask>(EnhancedPositionServiceTypes::PositionInfoKey::ALTITUDE);
  }

  if (speedChanged)
  {
     changedValues |= static_cast<EnhancedPositionServiceTypes::Bitmask>(EnhancedPositionServiceTypes::PositionInfoKey::SPEED);
  }

  if (headingChanged)
  {
     changedValues |= static_cast<EnhancedPositionServiceTypes::Bitmask>(EnhancedPositionServiceTypes::PositionInfoKey::HEADING);
  }

  if (climbChanged)
  {
     changedValues |= static_cast<EnhancedPositionServiceTypes::Bitmask>(EnhancedPositionServiceTypes::PositionInfoKey::CLIMB);
  }

  if (pdopChanged)
  {
    changedValues |= static_cast<EnhancedPositionServiceTypes::Bitmask>(EnhancedPositionServiceTypes::PositionInfoKey::PDOP);
  }

  if (hdopChanged)
  {
    changedValues |= static_cast<EnhancedPositionServiceTypes::Bitmask>(EnhancedPositionServiceTypes::PositionInfoKey::HDOP);
  }
  
  if (vdopChanged)
  {
    changedValues |= static_cast<EnhancedPositionServiceTypes::Bitmask>(EnhancedPositionServiceTypes::PositionInfoKey::VDOP);
  }  

  if (usatChanged)
  {
    changedValues |= static_cast<EnhancedPositionServiceTypes::Bitmask>(EnhancedPositionServiceTypes::PositionInfoKey::USED_SATELLITES);
  }

  if (fixStatusChanged)
  {
    changedValues |= static_cast<EnhancedPositionServiceTypes::Bitmask>(EnhancedPositionServiceTypes::PositionInfoKey::GNSS_FIX_STATUS);
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
    LOG_INFO(gCtx,"SatelliteDetail Update[%d/%d]: satelliteId=%d azimuth=%d elevation=%d CNo=%d",
             i+1,
             numElements,
             satelliteDetail[i].satelliteId, 
             satelliteDetail[i].azimuth,
             satelliteDetail[i].elevation,                                   
             satelliteDetail[i].CNo);
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
    if(!checkMajorVersion(GENIVI_GNSS_API_MAJOR))
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

void EnhancedPositionStubImpl::GetVersion(const std::shared_ptr<CommonAPI::ClientId> _client, GetVersionReply_t _reply)
{
    LOG_INFO_MSG(gCtx,"GetVersion");
    
    EnhancedPositionServiceTypes::Version EnhancedPositionVersion(VER_MAJOR,VER_MINOR,VER_MICRO,std::string(VER_DATE));
    
    _reply(EnhancedPositionVersion);
}

void EnhancedPositionStubImpl::GetPositionInfo(const std::shared_ptr<CommonAPI::ClientId> _client, ::org::genivi::EnhancedPositionService::EnhancedPositionServiceTypes::Bitmask _valuesToReturn, GetPositionInfoReply_t _reply)
{
    TGNSSPosition position;
    EnhancedPositionServiceTypes::PositionInfo data;

    bool isPosRequested = false;
    bool isCourseRequested = false;

    if ((_valuesToReturn & static_cast<EnhancedPositionServiceTypes::Bitmask>(EnhancedPositionServiceTypes::PositionInfoKey::LATITUDE)) ||
        (_valuesToReturn & static_cast<EnhancedPositionServiceTypes::Bitmask>(EnhancedPositionServiceTypes::PositionInfoKey::LONGITUDE)) ||
        (_valuesToReturn & static_cast<EnhancedPositionServiceTypes::Bitmask>(EnhancedPositionServiceTypes::PositionInfoKey::ALTITUDE)))
    {
        isPosRequested = true;
    }

    if ((_valuesToReturn & static_cast<EnhancedPositionServiceTypes::Bitmask>(EnhancedPositionServiceTypes::PositionInfoKey::HEADING)) ||
        (_valuesToReturn & static_cast<EnhancedPositionServiceTypes::Bitmask>(EnhancedPositionServiceTypes::PositionInfoKey::SPEED)) ||
        (_valuesToReturn & static_cast<EnhancedPositionServiceTypes::Bitmask>(EnhancedPositionServiceTypes::PositionInfoKey::CLIMB)))
    {
        isCourseRequested = true;
    }

    if(gnssGetPosition(&position))
    {
        if(isPosRequested)
        {
            if (position.validityBits & GNSS_POSITION_LATITUDE_VALID)
            {
                data[EnhancedPositionServiceTypes::PositionInfoKey::LATITUDE] = position.latitude;
            }

            if (position.validityBits & GNSS_POSITION_LONGITUDE_VALID)
            {
                data[EnhancedPositionServiceTypes::PositionInfoKey::LONGITUDE] = position.longitude;
            }

            if (position.validityBits & GNSS_POSITION_ALTITUDEMSL_VALID)
            {
                data[EnhancedPositionServiceTypes::PositionInfoKey::ALTITUDE] = (double) position.altitudeMSL;
            }
        }

        if(isCourseRequested)
        {
            if (position.validityBits & GNSS_POSITION_HEADING_VALID)
            {
                data[EnhancedPositionServiceTypes::PositionInfoKey::HEADING] = (double) position.heading;
            }

            if (position.validityBits & GNSS_POSITION_HSPEED_VALID)
            {
                data[EnhancedPositionServiceTypes::PositionInfoKey::SPEED] = (double) position.hSpeed;
            }

            if (position.validityBits & GNSS_POSITION_VSPEED_VALID)
            {
                data[EnhancedPositionServiceTypes::PositionInfoKey::CLIMB] = (double) position.vSpeed;
            }
        }
    }

    _reply(position.timestamp,data);
}

void EnhancedPositionStubImpl::shutdown()
{
  LOG_INFO_MSG(gCtx,"Shutting down EnhancedPosition dispatcher...");

  gnssDeregisterPositionCallback(&cbPosition);
  gnssDeregisterSatelliteDetailCallback(&cbSatelliteDetail);
  gnssDestroy();
}
