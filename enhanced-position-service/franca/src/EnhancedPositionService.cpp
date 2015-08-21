/* Copyright (C) 2014 Mentor Graphics
 * Author: Marco Residori(marco_residori@mentor.com)
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <thread>
#include <CommonAPI/CommonAPI.hpp>
#include "EnhancedPositionStubImpl.hpp"
#include "PositionFeedbackStubImpl.hpp"
#include "ConfigurationStubImpl.hpp"
#include "log.h"

DLT_DECLARE_CONTEXT(gCtx);

using namespace std;

int main() {

    DLT_REGISTER_APP("ENHS","ENHANCED-POSITION-SERVICE");
    DLT_REGISTER_CONTEXT(gCtx,"ENHS","Global Context");

    std::shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();

    std::string domain = "local";
    std::string instance = "EnhancedPositionService";

    std::shared_ptr<EnhancedPositionStubImpl> myServiceEnhancedPosition = std::make_shared<EnhancedPositionStubImpl>();
    
    bool successfullyRegistered = runtime->registerService(domain, instance, myServiceEnhancedPosition);
    while (!successfullyRegistered) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		successfullyRegistered = runtime->registerService(domain, instance, myServiceEnhancedPosition);
	}
    myServiceEnhancedPosition->run();

    std::shared_ptr<PositionFeedbackStubImpl> myServicePositionFeedback = std::make_shared<PositionFeedbackStubImpl>();
    successfullyRegistered = runtime->registerService(domain, instance, myServicePositionFeedback);
    while (!successfullyRegistered) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		successfullyRegistered = runtime->registerService(domain, instance, myServicePositionFeedback);
	}
    myServicePositionFeedback->run();

    std::shared_ptr<ConfigurationStubImpl> myServiceConfiguration = std::make_shared<ConfigurationStubImpl>();
    successfullyRegistered = runtime->registerService(domain, instance, myServiceConfiguration);
    while (!successfullyRegistered) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		successfullyRegistered = runtime->registerService(domain, instance, myServiceConfiguration);
	}
    myServiceConfiguration->run();
    
    while (true) {
        //LOG_INFO_MSG(gCtx,"Waiting for calls... (Abort with CTRL+C)");
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    myServiceEnhancedPosition->shutdown();
    myServicePositionFeedback->shutdown();
    myServiceConfiguration->shutdown();

    return 0;
}





