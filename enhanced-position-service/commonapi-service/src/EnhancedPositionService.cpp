/* Copyright (C) 2014 Mentor Graphics
 * Author: Marco Residori(marco_residori@mentor.com)
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <CommonAPI/CommonAPI.h>
#include "EnhancedPositionStubImpl.h"
#include "PositionFeedbackStubImpl.h"
#include "ConfigurationStubImpl.h"
#include "log.h"

DLT_DECLARE_CONTEXT(gCtx);

using namespace std;

int main() {

    DLT_REGISTER_APP("ENHS","ENHANCED-POSITION-SERVICE");
    DLT_REGISTER_CONTEXT(gCtx,"ENHS","Global Context");

    std::shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::load();
    std::shared_ptr<CommonAPI::Factory> factory = runtime->createFactory();
    std::shared_ptr<CommonAPI::ServicePublisher> servicePublisher = runtime->getServicePublisher();

    const std::string& serviceEnhancedPositionAddress = "local:org.genivi.positioning.EnhancedPosition:org.genivi.positioning.EnhancedPosition";
    std::shared_ptr<EnhancedPositionStubImpl> myServiceEnhancedPosition = std::make_shared<EnhancedPositionStubImpl>();
    servicePublisher->registerService(myServiceEnhancedPosition, serviceEnhancedPositionAddress, factory);
    myServiceEnhancedPosition->run();

    const std::string& servicePositionFeedbackAddress = "local:org.genivi.positioning.PositionFeedback:org.genivi.positioning.PositionFeedback";
    std::shared_ptr<PositionFeedbackStubImpl> myServicePositionFeedback = std::make_shared<PositionFeedbackStubImpl>();
    servicePublisher->registerService(myServicePositionFeedback, servicePositionFeedbackAddress, factory);
    myServicePositionFeedback->run();

    const std::string& serviceConfigurationAddress = "local:org.genivi.positioning.Configuration:org.genivi.positioning.Configuration";
    std::shared_ptr<ConfigurationStubImpl> myServiceConfiguration = std::make_shared<ConfigurationStubImpl>();
    servicePublisher->registerService(myServiceConfiguration, serviceConfigurationAddress, factory);
    myServiceConfiguration->run();
    
    while (true) {
        LOG_INFO_MSG(gCtx,"Waiting for calls... (Abort with CTRL+C)");
        std::this_thread::sleep_for(std::chrono::seconds(60));
    }

    myServiceEnhancedPosition->shutdown();
    myServicePositionFeedback->shutdown();
    myServiceConfiguration->shutdown();

    return 0;
}





