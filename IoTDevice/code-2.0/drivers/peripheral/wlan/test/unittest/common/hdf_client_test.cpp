/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hdf_wifi_test.h"
#include <gtest/gtest.h>
#include "hdf_uhdf_test.h"
#include "wifi_driver_client.h"

using namespace testing::ext;

namespace ClientTest {
static struct HdfDevEventlistener g_devEventListener;
static struct HdfDevEventlistener g_devEventListener1;
static struct HdfDevEventlistener g_devEventListener2;

class WifiClientTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void WifiClientTest::SetUpTestCase() {}

void WifiClientTest::TearDownTestCase() {}

void WifiClientTest::SetUp() {}

void WifiClientTest::TearDown() {}

static int OnWifiEventListener(struct HdfDevEventlistener *listener,
    struct HdfIoService *service, uint32_t id, struct HdfSBuf *data)
{
    (void)listener;
    (void)service;
    (void)id;
    if (data == NULL) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

/**
 * @tc.name: WifiClientInitAndDeinit001
 * @tc.desc: Initialize and uninitialize the WLAN client only once.
 * @tc.type: FUNC
 * @tc.require: AR000F869F, AR000F8QNL
 */
HWTEST_F(WifiClientTest, WifiClientInitAndDeinit001, TestSize.Level1)
{
    int ret;

    ret = WifiMsgServiceInit();
    EXPECT_EQ(HDF_SUCCESS, ret);
    WifiMsgServiceDeinit();
}

/**
 * @tc.name: WifiClientInitAndDeinit002
 * @tc.desc: Initialize and uninitialize the WLAN client for multiple times.
 * @tc.type: FUNC
 * @tc.require: AR000F869F, AR000F8QNL
 */
HWTEST_F(WifiClientTest, WifiClientInitAndDeinit002, TestSize.Level1)
{
    int ret;

    ret = WifiMsgServiceInit();
    EXPECT_EQ(HDF_SUCCESS, ret);
    WifiMsgServiceDeinit();

    ret = WifiMsgServiceInit();
    EXPECT_EQ(HDF_SUCCESS, ret);
    WifiMsgServiceDeinit();
}

/**
 * @tc.name: WifiClientInitAndDeinit003
 * @tc.desc: Initialize the WLAN client for multiple times.
 * @tc.type: FUNC
 * @tc.require: AR000F869F, AR000F8QNL
 */
HWTEST_F(WifiClientTest, WifiClientInitAndDeinit003, TestSize.Level1)
{
    int ret;

    ret = WifiMsgServiceInit();
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = WifiMsgServiceInit();
    EXPECT_EQ(HDF_SUCCESS, ret);
    WifiMsgServiceDeinit();
}

/**
 * @tc.name: WifiRegisterListener001
 * @tc.desc: Register the listener for single registration and unregistration.
 * @tc.type: FUNC
 * @tc.require: AR000F869F, AR000F8QNL
 */
HWTEST_F(WifiClientTest, WifiRegisterListener001, TestSize.Level1)
{
    int ret;

    g_devEventListener.onReceive = OnWifiEventListener;
    ret = WifiMsgServiceInit();
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = WifiMsgRegisterEventListener(&g_devEventListener);
    EXPECT_EQ(HDF_SUCCESS, ret);
    WifiMsgUnregisterEventListener(&g_devEventListener);
    WifiMsgServiceDeinit();
}

/**
 * @tc.name: WifiRegisterListener002
 * @tc.desc: Register the listener for repeated registration and unregistration.
 * @tc.type: FUNC
 * @tc.require: AR000F869F, AR000F8QNL
 */
HWTEST_F(WifiClientTest, WifiRegisterListener002, TestSize.Level1)
{
    int ret;

    g_devEventListener.onReceive = OnWifiEventListener;
    ret = WifiMsgServiceInit();
    EXPECT_EQ(HDF_SUCCESS, ret);

    ret = WifiMsgRegisterEventListener(&g_devEventListener);
    EXPECT_EQ(HDF_SUCCESS, ret);
    WifiMsgUnregisterEventListener(&g_devEventListener);
    ret = WifiMsgRegisterEventListener(&g_devEventListener);
    EXPECT_EQ(HDF_SUCCESS, ret);
    WifiMsgUnregisterEventListener(&g_devEventListener);

    WifiMsgServiceDeinit();
}

/**
 * @tc.name: WifiRegisterListener003
 * @tc.desc: Register multiple listeners for registration and unregistration.
 * @tc.type: FUNC
 * @tc.require: AR000F869F
 */
HWTEST_F(WifiClientTest, WifiRegisterListener003, TestSize.Level1)
{
    int ret;

    g_devEventListener1.onReceive = OnWifiEventListener;
    g_devEventListener2.onReceive = OnWifiEventListener;
    ret = WifiMsgServiceInit();
    EXPECT_EQ(HDF_SUCCESS, ret);

    ret = WifiMsgRegisterEventListener(&g_devEventListener1);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = WifiMsgRegisterEventListener(&g_devEventListener2);
    EXPECT_EQ(HDF_SUCCESS, ret);
    WifiMsgUnregisterEventListener(&g_devEventListener1);
    WifiMsgUnregisterEventListener(&g_devEventListener2);

    WifiMsgServiceDeinit();
}

/**
 * @tc.name: WifiRegisterListener004
 * @tc.desc: Repeatedly register a listener.
 * @tc.type: FUNC
 * @tc.require: AR000F869F
 */
HWTEST_F(WifiClientTest, WifiRegisterListener004, TestSize.Level1)
{
    int ret;

    g_devEventListener.onReceive = OnWifiEventListener;
    ret = WifiMsgServiceInit();
    EXPECT_EQ(HDF_SUCCESS, ret);

    ret = WifiMsgRegisterEventListener(&g_devEventListener);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = WifiMsgRegisterEventListener(&g_devEventListener);
    EXPECT_NE(HDF_SUCCESS, ret);
    WifiMsgUnregisterEventListener(&g_devEventListener);

    WifiMsgServiceDeinit();
}

/**
 * @tc.name: WifiRegisterListener005
 * @tc.desc: In abnormal condition, register the listener and no deregistration operation.
 * @tc.type: FUNC
 * @tc.require: AR000F869F
 */
HWTEST_F(WifiClientTest, WifiRegisterListener005, TestSize.Level1)
{
    int ret;

    g_devEventListener.onReceive = NULL;
    ret = WifiMsgServiceInit();
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = WifiMsgRegisterEventListener(&g_devEventListener);
    EXPECT_NE(HDF_SUCCESS, ret);
    WifiMsgServiceDeinit();
}

/**
 * @tc.name: WifiRegisterListener006
 * @tc.desc: Uninitialize a listener when the listener is not completely unregistered.
 * @tc.type: FUNC
 * @tc.require: AR000F869F
 */
HWTEST_F(WifiClientTest, WifiRegisterListener006, TestSize.Level1)
{
    int ret;

    g_devEventListener1.onReceive = OnWifiEventListener;
    g_devEventListener2.onReceive = OnWifiEventListener;
    ret = WifiMsgServiceInit();
    EXPECT_EQ(HDF_SUCCESS, ret);

    ret = WifiMsgRegisterEventListener(&g_devEventListener1);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = WifiMsgRegisterEventListener(&g_devEventListener2);
    EXPECT_EQ(HDF_SUCCESS, ret);
    WifiMsgUnregisterEventListener(&g_devEventListener1);
    WifiMsgServiceDeinit();
    WifiMsgUnregisterEventListener(&g_devEventListener2);
    WifiMsgServiceDeinit();
}

/**
 * @tc.name: WifiRegisterListener007
 * @tc.desc: Cancel a listener when no listener is registered.
 * @tc.type: FUNC
 * @tc.require: AR000F869F
 */
HWTEST_F(WifiClientTest, WifiRegisterListener007, TestSize.Level1)
{
    int ret;

    g_devEventListener.onReceive = OnWifiEventListener;
    ret = WifiMsgServiceInit();
    EXPECT_EQ(HDF_SUCCESS, ret);

    WifiMsgUnregisterEventListener(&g_devEventListener);
    ret = WifiMsgRegisterEventListener(&g_devEventListener);
    EXPECT_EQ(HDF_SUCCESS, ret);
    WifiMsgUnregisterEventListener(&g_devEventListener);

    WifiMsgServiceDeinit();
}
};
