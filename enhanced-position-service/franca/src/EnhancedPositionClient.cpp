/* Copyright (C) 2014 Mentor Graphics
 * Author: Marco Residori(marco_residori@mentor.com)
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <CommonAPI/CommonAPI.h>
#include <org/genivi/EnhancedPositionService/EnhancedPositionProxy.h>
#include "log.h"

DLT_DECLARE_CONTEXT(gCtx);

using namespace org::genivi::EnhancedPositionService;
using namespace org::genivi::EnhancedPositionService::EnhancedPositionServiceTypes;

void getPositionInfoAsyncCallback(const CommonAPI::CallStatus& callStatus, const EnhancedPositionServiceTypes::Timestamp& timestamp, const EnhancedPositionServiceTypes::PositionInfo& posInfo)
{
    if (callStatus != CommonAPI::CallStatus::SUCCESS) {
        LOG_ERROR_MSG(gCtx,"Remote call failed!\n");
        return;
    }

    for ( auto it = posInfo.begin(); it != posInfo.end(); ++it ) {
        if (it->first == PositionInfoKey::LATITUDE)
        {
            LOG_INFO(gCtx,"LAT=%lf", it->second.get<double>());
        }
        if (it->first == PositionInfoKey::LONGITUDE)
        {
    	    LOG_INFO(gCtx,"LON=%lf", it->second.get<double>());
        }
        if (it->first == PositionInfoKey::ALTITUDE)
        {
            LOG_INFO(gCtx,"ALT=%lf", it->second.get<float>());
        }
        if (it->first == PositionInfoKey::SPEED)
        {
            LOG_INFO(gCtx,"SPEED=%lf", it->second.get<float>());
        }
        if (it->first == PositionInfoKey::CLIMB)
        {
            LOG_INFO(gCtx,"CLIMB=%lf", it->second.get<float>());
        }
        if (it->first == PositionInfoKey::HEADING)
        {
            LOG_INFO(gCtx,"HEADING=%lf", it->second.get<float>());
        }
    }
}

void positionUpdate(std::shared_ptr<EnhancedPositionProxyDefault> proxy, const EnhancedPositionServiceTypes::Bitmask& changedValues)
{
    LOG_INFO_MSG(gCtx,"Position Update");

    if(proxy == NULL)
    {
	    LOG_ERROR_MSG(gCtx,"Null pointer!");
	    return;
    }

    std::function<void(const CommonAPI::CallStatus&,
		               const EnhancedPositionServiceTypes::Timestamp&,
		               const EnhancedPositionServiceTypes::PositionInfo&)> fcb = getPositionInfoAsyncCallback;

    proxy->GetPositionInfoAsync(changedValues,getPositionInfoAsyncCallback);

}

int main() {

    DLT_REGISTER_APP("ENHC","ENHANCED-POSITION-CLIENT");
    DLT_REGISTER_CONTEXT(gCtx,"ENHC","Global Context");

    std::shared_ptr < CommonAPI::Runtime > runtime = CommonAPI::Runtime::load();
    std::shared_ptr < CommonAPI::Factory > factory = runtime->createFactory();

    const std::string& serviceAddress = "local:org.genivi.positioning.EnhancedPosition:org.genivi.positioning.EnhancedPosition";
    std::shared_ptr<EnhancedPositionProxyDefault> myProxy = factory->buildProxy<EnhancedPositionProxy>(serviceAddress);

    LOG_INFO_MSG(gCtx,"EnhancedPositionClient");

    while (!myProxy->isAvailable()) {
        usleep(10);
    }

    myProxy->getPositionUpdateEvent().subscribe([&](const EnhancedPositionServiceTypes::Bitmask& changedValues) {
        positionUpdate(myProxy, changedValues);
    });

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    return 0;
}
