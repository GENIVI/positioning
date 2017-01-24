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

#ifndef CONFIGURATIONSTUBIMPL_H_
#define CONFIGURATIONSTUBIMPL_H_

#include <CommonAPI/CommonAPI.hpp>
#include <v5/org/genivi/EnhancedPositionService/ConfigurationStubDefault.hpp>
#include <org/genivi/EnhancedPositionService/EnhancedPositionServiceTypes.hpp>

using namespace v5::org::genivi::EnhancedPositionService;
using namespace org::genivi::EnhancedPositionService;

class ConfigurationStubImpl: public ConfigurationStubDefault {

public:
    ConfigurationStubImpl();
    virtual ~ConfigurationStubImpl();
    void GetVersion(const std::shared_ptr<CommonAPI::ClientId> _client, GetVersionReply_t _reply);
    void GetSupportedSatelliteSystems(const std::shared_ptr<CommonAPI::ClientId> _client, GetSupportedSatelliteSystemsReply_t _reply);
    void run();
    void shutdown();

protected:
    bool validateSatSystemAttributeRequestedValue(const EnhancedPositionServiceTypes::SatelliteSystem& value);

private:
    std::vector<EnhancedPositionServiceTypes::SatelliteSystem> mSupportedSatSystems;
};

#endif /* CONFIGURATIONSTUBIMPL_H_ */


