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

#include "testclient.h"
#include "genivi-version.h"

#include "genivi-navigationcore-enum.h"

class EnhPosTest : public testing::Test {
protected:  // You should make the members protected s.t. they can be
    // accessed from sub-classes.

    EnhPosTest() : testClient(NULL) {
        QCoreApplication a();
        registerCommTypes();
        testClient = new TestClient();
    }

    // virtual void SetUp() { }
    // virtual void TearDown() { }

    TestClient* testClient;
};


TEST_F(EnhPosTest, getVersionShouldPass) {

    DemoIf* pClient = testClient->getClient();
    ASSERT_FALSE(pClient == NULL);
    ASSERT_TRUE(pClient->isValid());

    // request api version
    QDBusReply<GeniviVersion> replyResult;
    replyResult = pClient->GetVersion();

    ASSERT_TRUE(replyResult.isValid());

    GeniviVersion version = replyResult.value();
    qDebug("API version: %d.%d.%d", version.getMajorVersion(),
               version.getMinorVersion(), version.getMicroVersion());
               // versionDate.toUtf8().data());

    EXPECT_EQ(version.getMajorVersion(), 1);
    EXPECT_EQ(version.getMinorVersion(), 0);
    EXPECT_EQ(version.getMicroVersion(), 0);
}

TEST_F(EnhPosTest, getDataWithAllShouldPass) {
    DemoIf* pClient = testClient->getClient();
    ASSERT_FALSE(pClient == NULL);
    ASSERT_TRUE(pClient->isValid());

    // request api version
    QList<ushort> requestVales;
    requestVales.push_back(EnhancedPositionData::ALL);

    QDBusReply<MapUShortVariant> replyResult;
    replyResult = pClient->GetData(requestVales);

    ASSERT_TRUE(replyResult.isValid());

    MapUShortVariant list = replyResult.value();
}

TEST_F(EnhPosTest, getDataShouldReturnValues) {
    DemoIf* pClient = testClient->getClient();
    ASSERT_FALSE(pClient == NULL);
    ASSERT_TRUE(pClient->isValid());

    QList<ushort> requestVales;
    requestVales.push_back(EnhancedPositionData::ALL);

    QDBusReply<MapUShortVariant> replyResult;
    replyResult = pClient->GetData(requestVales);

    ASSERT_TRUE(replyResult.isValid());

    MapUShortVariant list = replyResult.value();
    qDebug() << "list: " << list;
    EXPECT_FALSE(list.value(EnhancedPositionData::INVALID).isValid());
}

TEST_F(EnhPosTest, getPositionShouldPass) {
    DemoIf* pClient = testClient->getClient();
    ASSERT_FALSE(pClient == NULL);
    ASSERT_TRUE(pClient->isValid());

    QList<ushort> requestVales;
    requestVales.push_back(EnhancedPositionData::ALL);

    QDBusReply<MapUShortVariant> replyResult;
    replyResult = pClient->GetPosition();

    ASSERT_TRUE(replyResult.isValid());

    MapUShortVariant list = replyResult.value();
    qDebug() << "list: " << list;
    EXPECT_FALSE(list.value(EnhancedPositionData::INVALID).isValid());

    EXPECT_TRUE(list.value(EnhancedPositionData::LATITUDE).isValid());
    EXPECT_TRUE(list.value(EnhancedPositionData::LONGITUDE).isValid());
    EXPECT_TRUE(list.value(EnhancedPositionData::SPEED).isValid());
    EXPECT_TRUE(list.value(EnhancedPositionData::HEADING).isValid());

    EXPECT_FALSE(list.value(Accuracy::FILTER_STATUS).isValid());
}

TEST_F(EnhPosTest, getRotationRateShouldPass) {
    DemoIf* pClient = testClient->getClient();
    ASSERT_FALSE(pClient == NULL);
    ASSERT_TRUE(pClient->isValid());

    QDBusReply<MapUShortVariant> replyResult;
    replyResult = pClient->GetRotationRate();

    ASSERT_TRUE(replyResult.isValid());

    MapUShortVariant list = replyResult.value();
    qDebug() << "GetRotationRate: " << list;

    EXPECT_GT(list.size(), 0);
    EXPECT_FALSE(list.value(RotationRate::INVALID).isValid());
}

TEST_F(EnhPosTest, getSatInfoShouldPass) {
    DemoIf* pClient = testClient->getClient();
    ASSERT_FALSE(pClient == NULL);
    ASSERT_TRUE(pClient->isValid());

    QDBusReply<MapUShortVariant> replyResult;
    replyResult = pClient->GetSatelliteInfo();

    ASSERT_TRUE(replyResult.isValid());

    MapUShortVariant list = replyResult.value();
    qDebug() << "GetSatelliteInfo: " << list;

    EXPECT_GT(list.size(), 0);
    EXPECT_FALSE(list.value(RotationRate::INVALID).isValid());
}

TEST_F(EnhPosTest, getAccuracyShouldPass) {
    DemoIf* pClient = testClient->getClient();
    ASSERT_FALSE(pClient == NULL);
    ASSERT_TRUE(pClient->isValid());

    QDBusReply<MapUShortVariant> replyResult;
    replyResult = pClient->GetAccuracy();

    ASSERT_TRUE(replyResult.isValid());

    MapUShortVariant list = replyResult.value();
    qDebug() << "GetAccuracy: " << list;

    EXPECT_GT(list.size(), 0);
    EXPECT_FALSE(list.value(Accuracy::INVALID).isValid());
}

TEST_F(EnhPosTest, getStatusShouldReturnSth) {
    DemoIf* pClient = testClient->getClient();
    ASSERT_FALSE(pClient == NULL);
    ASSERT_TRUE(pClient->isValid());

    QDBusReply<MapUShortVariant> replyResult;
    replyResult = pClient->GetStatus();

    ASSERT_TRUE(replyResult.isValid());

    MapUShortVariant list = replyResult.value();
    qDebug() << "GetStatus: " << list;

    EXPECT_GT(list.size(), 0);
    EXPECT_FALSE(list.value(Base::ALL).isValid());  // FIXME - constant define missing
}

TEST_F(EnhPosTest, getTimeShouldReturnValidTime) {
    DemoIf* pClient = testClient->getClient();
    ASSERT_FALSE(pClient == NULL);
    ASSERT_TRUE(pClient->isValid());

    QDBusReply<MapUShortVariant> replyResult;
    replyResult = pClient->GetTime();

    ASSERT_TRUE(replyResult.isValid());

    MapUShortVariant list = replyResult.value();
    qDebug() << "GetStatus: " << list;

    EXPECT_GT(list.size(), 0);
    EXPECT_FALSE(list.value(Time::INVALID).isValid());

    // date
    EXPECT_TRUE(list.value(Time::YEAR).isValid());
    EXPECT_GT(list.value(Time::YEAR).toUInt(), 1970);

    EXPECT_TRUE(list.value(Time::MONTH).isValid());
    EXPECT_GE(list.value(Time::MONTH).toUInt(), 1);
    EXPECT_LE(list.value(Time::MONTH).toUInt(), 12);

    EXPECT_TRUE(list.value(Time::DAY).isValid());
    EXPECT_GE(list.value(Time::DAY).toUInt(), 1);
    EXPECT_LE(list.value(Time::DAY).toUInt(), 31);

    // time
    EXPECT_TRUE(list.value(Time::HOUR).isValid());
    EXPECT_GE(list.value(Time::HOUR).toUInt(), 0);
    EXPECT_LE(list.value(Time::HOUR).toUInt(), 23);

    EXPECT_TRUE(list.value(Time::MINUTE).isValid());
    EXPECT_GE(list.value(Time::MINUTE).toUInt(), 0);
    EXPECT_LE(list.value(Time::MINUTE).toUInt(), 59);

    EXPECT_TRUE(list.value(Time::SECOND).isValid());
    EXPECT_GE(list.value(Time::SECOND).toUInt(), 0);
    EXPECT_LE(list.value(Time::SECOND).toUInt(), 60); // correction second included.

    EXPECT_TRUE(list.value(Time::MS).isValid());
    EXPECT_GE(list.value(Time::MS).toUInt(), 0);
    EXPECT_LE(list.value(Time::MS).toUInt(), 999); // correction second included.
}
