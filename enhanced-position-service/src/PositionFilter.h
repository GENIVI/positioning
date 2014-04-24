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

#ifndef POSITIONFILTER_H
#define POSITIONFILTER_H

#include "gnss-simple.h"

#include "QtCore/qmap.h"
#include "QtCore/qvariant.h"
typedef QMap<ushort,QVariant> MapUShortVariant;

/**
 * gets the updates and manages results.
 */
class PositionFilter
{
public:
    PositionFilter();
    virtual ~PositionFilter();

    void runFiltering(QList<ushort>& changedElements);
    bool appendPositionTo(MapUShortVariant& map);
    bool appendTimestampTo(MapUShortVariant& map);
    bool appendCourseTo(MapUShortVariant& map);
    bool appendElementTo(MapUShortVariant& map, ushort key);

    static PositionFilter* getInstance();
    static void onPositionUpdate(const TGNSSPosition pos[], uint16_t num_elements);
    static void onCourseUpdate(const TGNSSCourse course[], uint16_t num_elements);

private:
    bool diffPositions(QList<ushort>& list, const TGNSSPosition& pos1, const TGNSSPosition& pos2);
    bool diffCourses(QList<ushort>& list, const TGNSSCourse& course1, const TGNSSCourse& course2);

private:
    static PositionFilter* filterInstanct;
    TGNSSPosition mPos;
    TGNSSPosition mRawPos;
    TGNSSCourse mCourse;
    TGNSSCourse mRawCourse;
};

#endif /* POSITIONFILTER_H */
