/**************************************************************************
* @licence app begin@
*
* SPDX-License-Identifier: MPL-2.0
*
* \ingroup EnhancedPositionService
*
* \copyright Copyright (C) BMW Car IT GmbH 2011, 2012
* 
* \license
* This Source Code Form is subject to the terms of the
* Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
* this file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* @licence end@
**************************************************************************/


#ifndef GENIVI_NAVIGATIONCORE_ENUM
#define GENIVI_NAVIGATIONCORE_ENUM

class Base {
public:
    enum {
        INVALID = 0x0000,
        TIMESTAMP = 0x0001,
        ALL = 0xffff
    };
};

class Position {
public:
    enum {
        LATITUDE = 0x0020,
        LONGITUDE = 0x0021,
        ALTITUDE = 0x0022
    };
};

class Course {
public:
    enum {
        HEADING = 0x0030,
        SPEED = 0x0031,
        CLIMB = 0x0032
    };
};

class Address : public Base {
public:
    enum {
        COUNTRY = 0x0040,
        CITY = 0x0041,
        STREET = 0x0042,
        NUMBER = 0x0043,
        CROSSING = 0x0044,
        DISTRICT = 0x0045,
        TIMEZONE_OFFSET = 0x0046,
        DAYLIGHT_OFFSET = 0x0047,
        MATCH_TYPE = 0x0048
    };
};

class RotationRate : public Base {
public:
    enum {
        ROLL_RATE = 0x0060,
        PITCH_RATE = 0x0061,
        YAW_RATE = 0x0062
    };
};

class EnhancedPositionStatus : public Base {
public:
    enum {
        GNSS_FIX_STATUS = 0x0070,
        DR_STATUS = 0x0071
    };
};

class MapMatchedPositionStatus : public EnhancedPositionStatus {
public:
    enum {
        MM_STATUS = 0x0072
    };
};

class Accuracy : public EnhancedPositionStatus {
public:
    enum {
        PDOP = 0x0080,
        HDOP = 0x0081,
        VDOP = 0x0082,
        SIGMA_LATITUDE = 0x0083,
        SIGMA_LONGITUDE = 0x0084,
        SIGMA_ALTITUDE = 0x0085,
        SIGMA_HEADING = 0x0086,
        FILTER_STATUS = 0x0087
    };
};

class SatelliteInfo : public Base {
public:
    enum  {
        USED_SATELLITES = 0x0090,
        TRACKED_SATELLITES = 0x0091,
        VISIBLE_SATELLITES = 0x0092,
        SATELLITE_DETAILS = 0x0093
    };
};

class EnhancedPosition : public Base, public Position, public Course {
public:
    enum {
        // emtpy
    };
};

class EnhancedPositionData :
    public EnhancedPosition,
    public Accuracy
{
public:
    enum {
        // empty
    };
};

class Time : public Base {
public:
    enum {
        YEAR = 0x00a0,
        MONTH = 0x00a1,
        DAY = 0x00a2,
        HOUR = 0x00a3,
        MINUTE = 0x00a4,
        SECOND = 0x00a5,
        MS = 0x00a6
    };
};


#endif
