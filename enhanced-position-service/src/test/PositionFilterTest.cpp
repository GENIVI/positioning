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

#include <gtest/gtest.h>
#include <stdio.h>

#include "PositionFilter.h"
#include "genivi-navigationcore-enum.h"
#include "gnss/gnss-util.h"
#include "QtCore/qdebug.h"

TEST(PositionFilterTest, newPositionFilter) {
    ASSERT_FALSE(PositionFilter::getInstance() == NULL);
}

TEST(PositionFilterTest, singeltonCheck) {
    PositionFilter* inst1 = PositionFilter::getInstance();
    PositionFilter* inst2 = PositionFilter::getInstance();
    ASSERT_EQ(inst1, inst2);
}

TEST(PositionFilterTest, runFiltering_plain) {
    QList<ushort> changeList;
    PositionFilter::getInstance()->runFiltering(changeList);
    EXPECT_TRUE(true);
}

TEST(PositionFilterTest, injectPosition) {
    GNSSPosition pos;
    pos.latitude = 11.0f;
    pos.longitude = 48.0f;
    pos.altitude = 512.0f;
    pos.timestamp = 12345;
    pos.validField = {true, true, true}; // lat, lon, alt

    PositionFilter::getInstance()->onPositionUpdate(pos);
    EXPECT_TRUE(true);
}

TEST(PositionFilterTest, injectAndAppendPosition) {
    PositionFilter* inst = PositionFilter::getInstance();
    ASSERT_FALSE(inst == NULL);

    GNSSPosition pos;
    pos.latitude = 11.0f;
    pos.longitude = 48.0f;
    pos.altitude = 512.0f;
    pos.timestamp = 12345;
    pos.validField = {true, true, true}; // lat, lon, alt

    inst->onPositionUpdate(pos);
    QList<ushort> changeList;
    inst->runFiltering(changeList);

    MapUShortVariant map;
    EXPECT_TRUE(inst->appendPositionTo(map));
    EXPECT_GT(map.size(), 0);
}

TEST(PositionFilterTest, appendSomeValidLatLon) {
    PositionFilter* inst = PositionFilter::getInstance();
    ASSERT_FALSE(inst == NULL);

    GNSSPosition pos;
    pos.latitude = 11.0f;
    pos.longitude = 48.0f;
    pos.altitude = 512.0f;
    pos.timestamp = 12345;
    pos.validField = {true, true, true}; // lat, lon, alt

    inst->onPositionUpdate(pos);
    QList<ushort> changeList;
    inst->runFiltering(changeList);

    MapUShortVariant map;
    EXPECT_TRUE(inst->appendPositionTo(map));
    EXPECT_GT(map.size(), 0);

    EXPECT_FALSE(map.value(EnhancedPosition::INVALID).isValid());
    EXPECT_TRUE(map.value(EnhancedPosition::LATITUDE).isValid());
    EXPECT_TRUE(map.value(EnhancedPosition::LONGITUDE).isValid());
}

TEST(PositionFilterTest, appendTheGivenLatLon) {
    PositionFilter* inst = PositionFilter::getInstance();
    ASSERT_FALSE(inst == NULL);

    GNSSPosition pos;
    pos.latitude = 48.367472f;
    pos.longitude = 11.846595f;
    pos.altitude = 512.0f;
    pos.timestamp = 12345;
    pos.validField = {true, true, true}; // lat, lon, alt

    // inject position in PositionFilter
    inst->onPositionUpdate(pos);
    // give chance to do sth.
    QList<ushort> changeList;
    inst->runFiltering(changeList);

    // general checks
    MapUShortVariant map;
    EXPECT_TRUE(inst->appendPositionTo(map));
    EXPECT_GT(map.size(), 0);
    EXPECT_FALSE(map.value(EnhancedPosition::INVALID).isValid());

    // content checks
    EXPECT_TRUE(map.value(EnhancedPosition::LATITUDE).isValid());
    EXPECT_NEAR(map.value(EnhancedPosition::LATITUDE).toFloat(),
            pos.latitude, 0.01f);

    EXPECT_TRUE(map.value(EnhancedPosition::LONGITUDE).isValid());
    EXPECT_NEAR(map.value(EnhancedPosition::LONGITUDE).toFloat(),
            pos.longitude, 0.01f);
}

TEST(PositionFilterTest, notAppendTheGivenLatLon) {
    PositionFilter* inst = PositionFilter::getInstance();
    ASSERT_FALSE(inst == NULL);

    GNSSPosition pos;
    pos.timestamp = 12345;
    pos.validField = {false, false, false}; // lat, lon, alt

    // inject position in PositionFilter
    inst->onPositionUpdate(pos);
    // give chance to do sth.
    QList<ushort> changeList;
    inst->runFiltering(changeList);

    // general checks
    MapUShortVariant map;
    map.clear();

    EXPECT_FALSE(inst->appendPositionTo(map));
    EXPECT_EQ(map.size(), 1);
    EXPECT_TRUE(map.value(EnhancedPosition::INVALID).isValid());

    EXPECT_FALSE(map.value(EnhancedPosition::LATITUDE).isValid());
    EXPECT_FALSE(map.value(EnhancedPosition::LONGITUDE).isValid());
}

TEST(PositionFilterTest, appendTheGivenTimestamp) {
    PositionFilter* inst = PositionFilter::getInstance();
    ASSERT_FALSE(inst == NULL);

    GNSSPosition pos;
    pos.latitude = 48.367472f;
    pos.longitude = 11.846595f;
    pos.altitude = 512.0f;
    pos.timestamp = 45677;
    pos.validField = {true, true, true}; // lat, lon, alt

    // inject position in PositionFilter
    inst->onPositionUpdate(pos);
    // give chance to do sth.
    QList<ushort> changeList;
    inst->runFiltering(changeList);

    // general checks
    MapUShortVariant map;
    map.clear();
    EXPECT_TRUE(inst->appendTimestampTo(map));
    EXPECT_GT(map.size(), 0);
    EXPECT_FALSE(map.value(EnhancedPosition::INVALID).isValid());

    // content checks
    EXPECT_TRUE(map.value(EnhancedPosition::TIMESTAMP).isValid());
    EXPECT_NEAR(map.value(EnhancedPosition::TIMESTAMP).toULongLong(),
            pos.timestamp, 100); // can differ by 100 ms
}

TEST(PositionFilterTest, appendTheGivenCourse) {
    PositionFilter* inst = PositionFilter::getInstance();
    ASSERT_FALSE(inst == NULL);

    GNSSCourse course;
    course.timestamp = 45677;
    course.speed = 10.0f;
    course.climb = 0.0f;
    course.heading = 180.0f;
    course.validField = {true, true, true}; // speed, climb, heading

    // inject course in PositionFilter
    inst->onCourseUpdate(course);
    // give chance to do sth.
    QList<ushort> changeList;
    inst->runFiltering(changeList);

    // general checks
    MapUShortVariant map;
    map.clear();
    EXPECT_TRUE(inst->appendCourseTo(map));
    EXPECT_GT(map.size(), 0);
    EXPECT_FALSE(map.value(EnhancedPosition::INVALID).isValid());

    // content checks
    EXPECT_TRUE(map.value(EnhancedPosition::HEADING).isValid());
    EXPECT_NEAR(map.value(EnhancedPosition::HEADING).toFloat(),
            course.heading, 18.0f); // can differ by 18 [degree] = 1/20 of circle

    EXPECT_TRUE(map.value(EnhancedPosition::CLIMB).isValid());
    EXPECT_NEAR(map.value(EnhancedPosition::CLIMB).toFloat(),
            course.climb, 5.0f);

    EXPECT_TRUE(map.value(EnhancedPosition::SPEED).isValid());
    EXPECT_NEAR(map.value(EnhancedPosition::SPEED).toFloat(),
            course.speed, 1.0f);
}

TEST(PositionFilterTest, appendPositionCourseAndTime) {
    PositionFilter* inst = PositionFilter::getInstance();
    ASSERT_FALSE(inst == NULL);

    // inject course in PositionFilter
    GNSSCourse course;
    course.timestamp = 45677;
    course.speed = 10.0f;
    course.climb = 0.0f;
    course.heading = 180.0f;
    course.validField = {true, true, true}; // speed, climb, heading
    inst->onCourseUpdate(course);

    // inject position in PositionFilter
    GNSSPosition pos;
    pos.timestamp = 45690; // small diff in timestamp!
    pos.latitude = 48.367472f;
    pos.longitude = 11.846595f;
    pos.altitude = 512.0f;
    pos.validField = {true, true, true}; // lat, lon, alt
    inst->onPositionUpdate(pos);

    // give chance to do sth.
    QList<ushort> changeList;
    inst->runFiltering(changeList);

    // general checks
    MapUShortVariant map;
    map.clear();

    EXPECT_TRUE(inst->appendPositionTo(map));
    EXPECT_TRUE(inst->appendCourseTo(map));
    EXPECT_TRUE(inst->appendTimestampTo(map));

    EXPECT_GT(map.size(), 0);
    EXPECT_FALSE(map.value(EnhancedPosition::INVALID).isValid());

    // content checks
    EXPECT_TRUE(map.value(EnhancedPosition::HEADING).isValid());
    EXPECT_NEAR(map.value(EnhancedPosition::HEADING).toFloat(),
            course.heading, 18.0f); // can differ by 18 [degree] = 1/20 of circle

    EXPECT_TRUE(map.value(EnhancedPosition::CLIMB).isValid());
    EXPECT_NEAR(map.value(EnhancedPosition::CLIMB).toFloat(),
            course.climb, 5.0f);

    EXPECT_TRUE(map.value(EnhancedPosition::SPEED).isValid());
    EXPECT_NEAR(map.value(EnhancedPosition::SPEED).toFloat(),
            course.speed, 1.0f);

    // content checks
    EXPECT_TRUE(map.value(EnhancedPosition::LATITUDE).isValid());
    EXPECT_NEAR(map.value(EnhancedPosition::LATITUDE).toFloat(),
            pos.latitude, 0.01f);

    EXPECT_TRUE(map.value(EnhancedPosition::LONGITUDE).isValid());
    EXPECT_NEAR(map.value(EnhancedPosition::LONGITUDE).toFloat(),
            pos.longitude, 0.01f);

    // content checks
    EXPECT_TRUE(map.value(EnhancedPosition::TIMESTAMP).isValid());
    EXPECT_NEAR(map.value(EnhancedPosition::TIMESTAMP).toULongLong(),
            pos.timestamp, 100); // can differ by 100 ms
    EXPECT_NEAR(map.value(EnhancedPosition::TIMESTAMP).toULongLong(),
            course.timestamp, 100); // can differ by 100 ms
}

TEST(PositionFilterTest, removeAppendWhenAddingValidCourse) {
    PositionFilter* inst = PositionFilter::getInstance();
    ASSERT_FALSE(inst == NULL);

    // inject course in PositionFilter
    GNSSCourse course;
    course.timestamp = 45677;
    course.speed = 10.0f;
    course.climb = 0.0f;
    course.heading = 180.0f;
    course.validField = {true, true, true}; // speed, climb, heading
    inst->onCourseUpdate(course);

    // inject position in PositionFilter
    GNSSPosition pos;
    pos.timestamp = 45690; // small diff in timestamp!
    pos.validField = {false, false, false}; // lat, lon, alt
    inst->onPositionUpdate(pos);

    // give chance to do sth.
    QList<ushort> changeList;
    inst->runFiltering(changeList);

    // general checks
    MapUShortVariant map;
    map.clear();

    // appending position should not work. --> only one key: INVALID
    EXPECT_FALSE(inst->appendPositionTo(map));
    EXPECT_EQ(map.size(), 1);
    EXPECT_TRUE(map.value(EnhancedPosition::INVALID).isValid());

    // now append something with valid content --> remove INVALID value!
    EXPECT_TRUE(inst->appendCourseTo(map));

    EXPECT_GT(map.size(), 0);
    EXPECT_FALSE(map.value(EnhancedPosition::INVALID).isValid());
    // qDebug() << map;
}

TEST(PositionFilterTest, removeAppendWhenAddingValidPosition) {
    PositionFilter* inst = PositionFilter::getInstance();
    ASSERT_FALSE(inst == NULL);

    // inject course in PositionFilter
    GNSSCourse course;
    course.timestamp = 45677;
    course.validField = {false, false, false}; // speed, climb, heading
    inst->onCourseUpdate(course);

    // inject position in PositionFilter
    GNSSPosition pos;
    pos.timestamp = 45690; // small diff in timestamp!
    pos.latitude = 48.367472f;
    pos.longitude = 11.846595f;
    pos.altitude = 512.0f;
    pos.validField = {true, true, true}; // lat, lon, alt
    inst->onPositionUpdate(pos);

    // give chance to do sth.
    QList<ushort> changeList;
    inst->runFiltering(changeList);

    // general checks
    MapUShortVariant map;
    map.clear();

    // appending course should not work.
    // --> only one map entry with INVALID
    EXPECT_FALSE(inst->appendCourseTo(map));
    EXPECT_EQ(map.size(), 1);
    EXPECT_TRUE(map.value(EnhancedPosition::INVALID).isValid());

    // now append something with valid content (course)
    // --> remove INVALID value!
    EXPECT_TRUE(inst->appendPositionTo(map));

    EXPECT_GT(map.size(), 0);
    EXPECT_FALSE(map.value(EnhancedPosition::INVALID).isValid());
}

TEST(PositionFilterTest, appendElement) {
    PositionFilter* inst = PositionFilter::getInstance();
    ASSERT_FALSE(inst == NULL);

    // inject position in PositionFilter
    GNSSPosition pos;
    pos.timestamp = 45690; // small diff in timestamp!
    pos.latitude = 48.367472f;
    pos.longitude = 11.846595f;
    pos.altitude = 512.0f;
    pos.validField = {true, true, true}; // lat, lon, alt
    inst->onPositionUpdate(pos);

    // give chance to do sth.
    QList<ushort> changeList;
    inst->runFiltering(changeList);
    // general checks
    MapUShortVariant map;
    map.clear();

    EXPECT_TRUE(inst->appendElementTo(map, EnhancedPosition::LATITUDE));
    EXPECT_TRUE(map.value(EnhancedPosition::LATITUDE).isValid());
    EXPECT_TRUE(inst->appendElementTo(map, EnhancedPosition::LONGITUDE));
    EXPECT_TRUE(map.value(EnhancedPosition::LONGITUDE).isValid());
    EXPECT_TRUE(inst->appendElementTo(map, EnhancedPosition::ALTITUDE));
    EXPECT_TRUE(map.value(EnhancedPosition::ALTITUDE).isValid());
    EXPECT_TRUE(inst->appendElementTo(map, EnhancedPosition::TIMESTAMP));
    EXPECT_TRUE(map.value(EnhancedPosition::TIMESTAMP).isValid());

    // TODO
    //EXPECT_TRUE(inst->appendElementTo(map, EnhancedPosition::HEADING));
    //EXPECT_TRUE(map.value(EnhancedPosition::HEADING).isValid());

    EXPECT_FALSE(map.value(EnhancedPosition::INVALID).isValid());
}

TEST(PositionFilterTest, runFilteringCheckList) {
    PositionFilter* inst = PositionFilter::getInstance();
    ASSERT_FALSE(inst == NULL);

    GNSSPosition pos;
    QList<ushort> list;

    // inject 1st position in PositionFilter
    pos.timestamp = 12340;
    pos.latitude = 48.3670f;
    pos.validField = {true, false, false}; // lat, lon, alt
    inst->onPositionUpdate(pos);

    // TICK
    list.clear();
    inst->runFiltering(list);

    // inject a 2nd position in PositionFilter
    pos.timestamp = 12345; // small diff in timestamp!
    pos.latitude = 48.3675f;
    pos.validField = {true, false, false}; // lat, lon, alt
    inst->onPositionUpdate(pos);

    // TACK
    list.clear();
    inst->runFiltering(list);

    EXPECT_TRUE(list.contains(EnhancedPosition::TIMESTAMP));
    EXPECT_TRUE(list.contains(EnhancedPosition::LATITUDE));
    EXPECT_FALSE(list.contains(EnhancedPosition::LONGITUDE));
}


TEST(PositionFilterTest, runFilteringCheckList_invalid2valid) {
    PositionFilter* inst = PositionFilter::getInstance();
    ASSERT_FALSE(inst == NULL);

    GNSSPosition pos;
    QList<ushort> list;

    // inject INVALID data for position
    pos.timestamp = 0;
    pos.validField = {false, false, false}; // lat, lon, alt
    inst->onPositionUpdate(pos);

    // TICK
    list.clear();
    inst->runFiltering(list);

    // inject VALID data position
    pos.timestamp = 12345; // small diff in timestamp!
    pos.latitude = 48.3675f;
    pos.validField = {true, false, false}; // lat, lon, alt
    inst->onPositionUpdate(pos);

    // TACK
    list.clear();
    inst->runFiltering(list);

    EXPECT_TRUE(list.contains(EnhancedPosition::TIMESTAMP));
    EXPECT_TRUE(list.contains(EnhancedPosition::LATITUDE));
    EXPECT_FALSE(list.contains(EnhancedPosition::LONGITUDE));
}

TEST(PositionFilterTest, runFilteringCheckList_valid2invalid) {
    PositionFilter* inst = PositionFilter::getInstance();
    ASSERT_FALSE(inst == NULL);

    GNSSPosition pos;
    QList<ushort> list;

    // inject a VALID position in PositionFilter
    pos.timestamp = 12340;
    pos.latitude = 48.3670f;
    pos.validField = {true, false, false}; // lat, lon, alt
    inst->onPositionUpdate(pos);

    // TICK
    list.clear();
    inst->runFiltering(list);

    // inject a INVALID position in PositionFilter
    pos.timestamp = 0; // small diff in timestamp!
    pos.validField = {false, false, false}; // lat, lon, alt
    inst->onPositionUpdate(pos);

    // TACK
    list.clear();
    inst->runFiltering(list);

    EXPECT_TRUE(list.contains(EnhancedPosition::TIMESTAMP));
    EXPECT_TRUE(list.contains(EnhancedPosition::LATITUDE));
    EXPECT_FALSE(list.contains(EnhancedPosition::LONGITUDE));
}
