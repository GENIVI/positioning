/**************************************************************************
* @licence app begin@
*
* SPDX-License-Identifier: MPL-2.0
*
* \ingroup GNSSService
* \author Marco Residori <marco.residori@xse.de>
*
* \copyright Copyright (C) 2013, XS Embedded GmbH
* 
* \license
* This Source Code Form is subject to the terms of the
* Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
* this file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* @licence end@
**************************************************************************/

#include "globals.h"
#include "gnss-meta-data.h"
#include "gnss.h"

const TGnssMetaData gGnssMetaData = {
    GENIVI_GNSS_API_MAJOR,  //version
    GNSS_CATEGORY_PHYSICAL, //category
    GNSS_TYPE_GNSS,         //typeBits
    1000,                   //cycleTime in ms
    4                       //number of channels 
};

bool gnssGetMetaData(TGnssMetaData *data)
{
    if(!data)
    {
        return false;
    }

    *data = gGnssMetaData;

    return true;
}
