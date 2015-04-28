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

#include "ConfigurationStubImpl.h"
#include "log.h"

//Configuration-interface version
#define VER_MAJOR 3
#define VER_MINOR 0
#define VER_MICRO 0
#define VER_DATE "05-08-2014"

DLT_IMPORT_CONTEXT(gCtx);

using namespace org::genivi::EnhancedPositionService::EnhancedPositionServiceTypes;

ConfigurationStubImpl::ConfigurationStubImpl() {

   setUpdateIntervalAttribute(1000); //set default time interval

   mSupportedSatSystems.push_back(SatelliteSystem::GALILEO);
   mSupportedSatSystems.push_back(SatelliteSystem::GPS);

   setSatSystemAttribute(SatelliteSystem::GALILEO); //set default GNSS system
}

ConfigurationStubImpl::~ConfigurationStubImpl() {
}

void ConfigurationStubImpl::GetVersion(EnhancedPositionServiceTypes::Version& version) {
    LOG_INFO_MSG(gCtx,"GetVersion");

    version.maj = VER_MAJOR;
    version.min = VER_MINOR;
    version.mic = VER_MICRO;
    version.date = std::string(VER_DATE);
}

//check if the value belongs to the list of supported satellite systems
bool ConfigurationStubImpl::validateSatSystemAttributeRequestedValue(const EnhancedPositionServiceTypes::SatelliteSystem& value) {
    return std::find(mSupportedSatSystems.begin(), mSupportedSatSystems.end(), value) != mSupportedSatSystems.end();
}

void ConfigurationStubImpl::GetSupportedSatelliteSystems(std::vector<EnhancedPositionServiceTypes::SatelliteSystem>& satelliteSystems) {
	//add a list of supported satellite systems
	satelliteSystems = mSupportedSatSystems;
}

void ConfigurationStubImpl::run()
{
    LOG_INFO_MSG(gCtx,"Starting Configuration dispatcher...");
    setUpdateIntervalAttribute(1000); //set default time interval
    setSatSystemAttribute(SatelliteSystem::GALILEO); //set default GNSS system
}

void ConfigurationStubImpl::shutdown()
{
    LOG_INFO_MSG(gCtx,"Shutting down Configuration dispatcher...");
}




