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

#include "ConfigurationStubImpl.hpp"
#include "log.h"

//Configuration-interface version
#define VER_MAJOR 3
#define VER_MINOR 0
#define VER_MICRO 0
#define VER_DATE "05-08-2014"

DLT_IMPORT_CONTEXT(gCtx);

using namespace org::genivi::EnhancedPositionService;

ConfigurationStubImpl::ConfigurationStubImpl() {

   setUpdateIntervalAttribute(1000); //set default time interval

   mSupportedSatSystems.push_back(EnhancedPositionServiceTypes::SatelliteSystem::GALILEO);
   mSupportedSatSystems.push_back(EnhancedPositionServiceTypes::SatelliteSystem::GPS);

   setSatSystemAttribute(EnhancedPositionServiceTypes::SatelliteSystem::GALILEO); //set default GNSS system
}

ConfigurationStubImpl::~ConfigurationStubImpl() {
}

void ConfigurationStubImpl::GetVersion(const std::shared_ptr<CommonAPI::ClientId> _client, GetVersionReply_t _reply) {
    LOG_INFO_MSG(gCtx,"GetVersion");

    EnhancedPositionServiceTypes::Version ConfigurationVersion(VER_MAJOR,VER_MINOR,VER_MICRO,std::string(VER_DATE));
    
    _reply(ConfigurationVersion);
}

//check if the value belongs to the list of supported satellite systems
bool ConfigurationStubImpl::validateSatSystemAttributeRequestedValue(const EnhancedPositionServiceTypes::SatelliteSystem& value) {

    for (unsigned int i = 0; i < mSupportedSatSystems.size(); i++)
    {
       if(mSupportedSatSystems[i] == value)
       {   
           return true; //found
       }
    }
    return false; //not found
}

void ConfigurationStubImpl::GetSupportedSatelliteSystems(const std::shared_ptr<CommonAPI::ClientId> _client, GetSupportedSatelliteSystemsReply_t _reply) {
	//add a list of supported satellite systems
    _reply(mSupportedSatSystems);
}

void ConfigurationStubImpl::run()
{
    LOG_INFO_MSG(gCtx,"Starting Configuration dispatcher...");
    setUpdateIntervalAttribute(1000); //set default time interval
    setSatSystemAttribute(EnhancedPositionServiceTypes::SatelliteSystem::GALILEO); //set default GNSS system
}

void ConfigurationStubImpl::shutdown()
{
    LOG_INFO_MSG(gCtx,"Shutting down Configuration dispatcher...");
}




