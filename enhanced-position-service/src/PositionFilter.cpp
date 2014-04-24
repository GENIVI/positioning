/**************************************************************************
* @licence app begin@
*
* SPDX-License-Identifier: MPL-2.0
*
* \ingroup EnhancedPositionService
* \author thomas.bader@bmw.de
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

#include "PositionFilter.h"
#include <iostream>
#include <cassert>
#include "genivi-navigationcore-enum.h"
#include "QtCore/qdebug.h"

PositionFilter* PositionFilter::filterInstanct = 0;

PositionFilter::PositionFilter()
{

}

PositionFilter::~PositionFilter()
{
    if (0 != filterInstanct)
        delete filterInstanct;
}

bool PositionFilter::diffPositions(QList<ushort>& list, const TGNSSPosition& pos1, const TGNSSPosition& pos2)
{
    if ( ((pos1.validityBits & GNSS_POSITION_LATITUDE_VALID) != (pos2.validityBits & GNSS_POSITION_LATITUDE_VALID)) ||
        (pos1.latitude != pos2.latitude) )
        list.push_back(EnhancedPosition::LATITUDE);

    if ( ((pos1.validityBits & GNSS_POSITION_LONGITUDE_VALID) != (pos2.validityBits & GNSS_POSITION_LONGITUDE_VALID)) ||
        (pos1.longitude != pos2.longitude))
        list.push_back(EnhancedPosition::LONGITUDE);

    if ( ((pos1.validityBits & GNSS_POSITION_ALTITUDE_VALID) != (pos2.validityBits & GNSS_POSITION_ALTITUDE_VALID)) ||
        (pos1.altitude != pos2.altitude) )
        list.push_back(EnhancedPosition::ALTITUDE);

    if (pos1.timestamp != pos2.timestamp)
        list.push_back(EnhancedPosition::TIMESTAMP);

    return false;
}

bool PositionFilter::diffCourses(QList<ushort>& list, const TGNSSCourse& course1, const TGNSSCourse& course2) {
    
   if ( ((course1.validityBits & GNSS_COURSE_SPEED_VALID) != (course2.validityBits & GNSS_COURSE_SPEED_VALID)) ||
        (course1.speed != course2.speed) )
        list.push_back(EnhancedPosition::SPEED);

   return false;
}

void PositionFilter::runFiltering(QList<ushort>& changedElements)
{
    // *** Position ***
    diffPositions(changedElements, mPos, mRawPos);
    mPos = mRawPos;

    // *** Course ***
    uint64_t tsDiff = abs(mRawCourse.timestamp - mPos.timestamp);

    if (mPos.timestamp > 0 && tsDiff > 500) // 500 [ms]
        qDebug() << "warning: between course and position is diff in timestamps > 500 ms";

    diffCourses(changedElements, mCourse, mRawCourse);
    mCourse = mRawCourse;
}

bool PositionFilter::appendPositionTo(MapUShortVariant& map)
{
    map.remove(EnhancedPosition::INVALID);
    int count = map.size();

    if ((GNSS_POSITION_LATITUDE_VALID == (mPos.validityBits & GNSS_POSITION_LATITUDE_VALID))
      && (GNSS_POSITION_LONGITUDE_VALID == (mPos.validityBits & GNSS_POSITION_LONGITUDE_VALID)) 
      && (GNSS_POSITION_ALTITUDE_VALID == (mCourse.validityBits & GNSS_POSITION_ALTITUDE_VALID))
                   ) {
        map.insert(EnhancedPosition::LATITUDE, qreal(mPos.latitude));
        map.insert(EnhancedPosition::LONGITUDE, qreal(mPos.longitude));
        map.insert(EnhancedPosition::ALTITUDE, qreal(mPos.altitude));
    }

    if (map.empty()) {
        map.insert(EnhancedPosition::INVALID, "invalid");
        return false;
    }

    return (count < map.size());
}

bool PositionFilter::appendTimestampTo(MapUShortVariant& map)
{
    map.remove(EnhancedPosition::INVALID);
    int count = map.size();

    if (mPos.timestamp > 0) {
        map.insert(EnhancedPosition::TIMESTAMP,
                QVariant((qlonglong)mPos.timestamp));
    }

    if (map.empty()) {
        map.insert(EnhancedPosition::INVALID, QVariant("invalid"));
        return false;
    }

    return (count < map.size());
}

bool PositionFilter::appendCourseTo(MapUShortVariant& map)
{
    map.remove(EnhancedPosition::INVALID);
    int count = map.size();

    if (GNSS_COURSE_HEADING_VALID == (mCourse.validityBits & GNSS_COURSE_HEADING_VALID))
        map.insert(EnhancedPosition::HEADING, qreal(mCourse.heading));
    if (GNSS_COURSE_SPEED_VALID == (mCourse.validityBits & GNSS_COURSE_SPEED_VALID))
        map.insert(EnhancedPosition::SPEED, qreal(mCourse.speed));
    if (GNSS_COURSE_CLIMB_VALID == (mCourse.validityBits & GNSS_COURSE_CLIMB_VALID))
        map.insert(EnhancedPosition::CLIMB, qreal(mCourse.climb));

    if (map.empty()) {
        map.insert(EnhancedPosition::INVALID, QVariant("invalid"));
        return false;
    }

    return (count < map.size());
}

bool PositionFilter::appendElementTo(MapUShortVariant& map, ushort key)
{
    int count = map.size();

    switch(key) {
    case EnhancedPositionData::INVALID:
        std::cout << "append INVALID to map" << std::endl;
        break;

    case EnhancedPositionData::ALL:
        appendPositionTo(map);
        appendCourseTo(map);
        appendTimestampTo(map);
        break;

    case EnhancedPositionData::LATITUDE:
        map.insert(EnhancedPositionData::LATITUDE, qreal(mPos.latitude));
        break;

    case EnhancedPositionData::LONGITUDE:
        map.insert(EnhancedPositionData::LONGITUDE, qreal(mPos.longitude));
        break;

    case EnhancedPositionData::ALTITUDE:
        map.insert(EnhancedPositionData::ALTITUDE, qreal(mPos.altitude));
        break;

   case EnhancedPositionData::SPEED:
        map.insert(EnhancedPositionData::SPEED, qreal(mCourse.speed));
        break;

    case EnhancedPositionData::TIMESTAMP:
        map.insert(EnhancedPositionData::TIMESTAMP, qreal(mPos.timestamp));
        break;

    default:
        std::cout << __FUNCTION__
            << ": no valid KEY or not implemented ("<< key << ")" << std::endl;
    }

    if (map.size() > count) { // success
        std::cout << __FUNCTION__ << ": added " << map.size() - count << std::endl;
        map.remove(EnhancedPosition::INVALID);
        return true;
    }

    return false;
}

// static
PositionFilter* PositionFilter::getInstance()
{
    if (0 == filterInstanct) {
        filterInstanct = new PositionFilter();
    }
    assert(filterInstanct);
    return filterInstanct;
}

// static
void PositionFilter::onPositionUpdate(const TGNSSPosition pos[], uint16_t numElements)
{
    if (numElements > 0) {
        filterInstanct->mRawPos = pos[0];
    }
    //std::cout << "latitude : " << std::fixed << pos.latitude << std::endl;
    //std::cout << "longitude: " << std::fixed << pos.longitude << std::endl;
    return;
}

// static
void PositionFilter::onCourseUpdate(const TGNSSCourse course[], uint16_t numElements)
{
    if (numElements > 0) {
        filterInstanct->mRawCourse = course[0];
    }
    return;
}
