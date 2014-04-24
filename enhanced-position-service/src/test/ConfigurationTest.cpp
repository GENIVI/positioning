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

#include "ConfigurationClient.h"
#include "genivi-version.h"

#include "genivi-navigationcore-enum.h"

class ConfigurationTest : public testing::Test {
protected:  // You should make the members protected s.t. they can be
    // accessed from sub-classes.

    ConfigurationTest() : confClient(NULL) {
        QCoreApplication a();
        registerCommTypes();
        confClient = new ConfigurationClient();
    }

    // virtual void SetUp() { }
    // virtual void TearDown() { }

    ConfigurationClient* confClient;
};


TEST_F(ConfigurationTest, getVersionShouldPass) {

    ConfigurationIf* pClient = confClient->getClient();
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

TEST_F(ConfigurationTest, getConfigurationShouldPass) {

    ConfigurationIf* pClient = confClient->getClient();
    ASSERT_FALSE(pClient == NULL);
    ASSERT_TRUE(pClient->isValid());

    // request api version
    QDBusReply<QVariantMap> replyResult;
    replyResult = pClient->GetProperties();

    ASSERT_TRUE(replyResult.isValid());

    QVariantMap map = replyResult.value();
    EXPECT_TRUE(map.value("UpdateInterval").isValid());
    EXPECT_TRUE(map.value("SatelliteSystem").isValid());
}
