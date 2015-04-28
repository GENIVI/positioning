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

#ifndef POSITIONFEEDBACKSTUBIMPL_H_
#define POSITIONFEEDBACKSTUBIMPL_H_

#include <CommonAPI/CommonAPI.h>
#include <org/genivi/EnhancedPositionService/PositionFeedbackStubDefault.h>

using namespace org::genivi::EnhancedPositionService;

class PositionFeedbackStubImpl: public PositionFeedbackStubDefault {

public:
    PositionFeedbackStubImpl();
    virtual ~PositionFeedbackStubImpl();
    void GetVersion(EnhancedPositionServiceTypes::Version& version);
    void SetPositionFeedback(std::vector<PositionFeedback::PositionFeedbackInfo> feedback, uint64_t timestamp, EnhancedPositionServiceTypes::PositionFeedbackType feedbackType);
    void run();
    void shutdown();

};

#endif /* POSITIONFEEDBACKSTUBIMPL_H_ */


