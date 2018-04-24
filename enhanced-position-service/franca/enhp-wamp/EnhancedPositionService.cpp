/**
* @licence app begin@
* SPDX-License-Identifier: MPL-2.0
*
* \copyright Copyright (C) 2018, PSA Groupe
*
* \file EnhancedPositionService.cpp
*
* \brief This file is part of the enhp Wamp proof of concept.
*
* \author Philippe Colliot <philippe.colliot@mpsa.com>
*
* \version 0.1
*
* This Source Code Form is subject to the terms of the
* Mozilla Public License (MPL), v. 2.0.
* If a copy of the MPL was not distributed with this file,
* You can obtain one at http://mozilla.org/MPL/2.0/.
*
* For further information see http://www.genivi.org/.
*
* List of changes:
* <date>, <name>, <description of change>
*
* @licence end@
*/

#include <iostream>
#include <thread>

#include <CommonAPI/CommonAPI.hpp>

#include "EnhancedPositionStubImpl.hpp"

#define REGISTER_COUNTER 10
#define REGISTER_SLEEP 50
#define LOOP_GNSS 500

using namespace v5::org::genivi::enhancedpositionservice;

int main(int argc, const char * const argv[])
{
    CommonAPI::Runtime::setProperty("LibraryBase", "EnhancedPositionService");

	std::shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();

	std::string domain = "local";
    std::string instanceEnhp = "ENHP";

	// create service and register it at runtime
    std::shared_ptr<EnhancedPositionStubImpl> serviceEnhp = std::make_shared<EnhancedPositionStubImpl>();
    uint8_t counter=0;
    bool successfullyRegistered = runtime->registerService(domain, instanceEnhp, serviceEnhp);
    while (!successfullyRegistered) {
        if(++counter>REGISTER_COUNTER){
            std::cout << "unable to register service: "<< instanceEnhp << std::endl;
            return EXIT_FAILURE;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(REGISTER_SLEEP));
        successfullyRegistered = runtime->registerService(domain, instanceEnhp, serviceEnhp);
    }

    std::cout << "Waiting for calls... (Abort with CTRL+C)" << std::endl;
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(LOOP_GNSS));
    }

	return 0;
}

