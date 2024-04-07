/**
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"
#include "discovery_service.h"
#include "session.h"
#include "securec.h"
#include <unistd.h>
#include <stdlib.h>
#include <climits>

using namespace std;
using namespace testing::ext;

static const int PUB_SUCCESS = 1;
static const int PUB_FAIL = -1;
static const int TESTCASE_COUNT = 18;
static const int ERROR_SESSION_ID = -1;
static const int SESSION_NAME_LEN = 64;
static const int DEVID_MAX_LEN = 96;
static const int SOFTBUS_TEST_SUCCESS = 0;
static const int SOFTBUS_TEST_FAIL = -1;
static const int ONE_SECOND = 1;
static const int DEF_TIMEOUT = 6;
static const int DEF_PUB_ID = 33113322;
static const int DEF_PUB_CAPABILITY_DATA_LEN = 2;

static int g_pubFlag = 0;
static const char* g_devId = "sb_test_default_devid";
static const char* g_pubModuleName = "sb_pub_module_name";
static const char* g_pubCapability = "ddmpCapability";
static unsigned char* g_pubCapabilityData = (unsigned char*)"Hi";
static IPublishCallback g_pubCallback = {0};
static struct ISessionListener* g_sessionListenerCallback = nullptr;

static void ResetPubFlag(void)
{
    g_pubFlag = 0;
}

static void WaitPublishResult(void)
{
    int timeout = DEF_TIMEOUT;
    while (timeout > 0) {
        sleep(ONE_SECOND);
        timeout--;
        if (g_pubFlag == PUB_SUCCESS || g_pubFlag == PUB_FAIL) {
            printf("checkPublish:wait[%d].\n", DEF_TIMEOUT - timeout);
            break;
        }
    }
    if (timeout <= 0) {
        printf("checkPublish:timeout!\n");
    }
}

/**
 * callback of publish success
 */
void SbPublishSuccess(int pubId)
{
    printf("[PubSuccess]publish success id[%d].\n", pubId);
    g_pubFlag = PUB_SUCCESS;
}

/**
 * callback of publish fail
 */
void SbPublishFail(int pubId, PublishFailReason reason)
{
    printf("[PubFail]publish fail id[%d],reason[%d].\n", pubId, reason);
    g_pubFlag = PUB_FAIL;
}

/**
 * callback of session opened
 */
int SbSessionOpened(int sessionId)
{
    (void)sessionId;
    printf("[Session] opened.\n");
    return SOFTBUS_TEST_SUCCESS;
}

/**
 * callback of session closed
 */
void SbSessionClosed(int sessionId)
{
    (void)sessionId;
    printf("[Session] closed.\n");
}

/**
 * callback of received data
 */
void SbOnBytesReceived(int sessionId, const void *data, unsigned int len)
{
    (void)sessionId;
    printf("[Session] receive bytes, data len[%u].\n", len);
}

/**
 * init service during first publish
 */
static void DefaultPublishToInitService(void)
{
    PublishInfo* pubInfo = NULL;
    pubInfo = (PublishInfo *)malloc(sizeof(PublishInfo));
    if (pubInfo == NULL) {
        printf("[DefaultPublishToInitService]malloc fail!\n");
        return;
    }
    (void)memset_s(pubInfo, sizeof(PublishInfo), 0, sizeof(PublishInfo));
    pubInfo->publishId = DEF_PUB_ID;
    pubInfo->mode = DISCOVER_MODE_PASSIVE;
    pubInfo->medium = COAP;
    pubInfo->freq = MID;
    pubInfo->capability = g_pubCapability;
    pubInfo->capabilityData = g_pubCapabilityData;
    pubInfo->dataLen = DEF_PUB_CAPABILITY_DATA_LEN;

    ResetPubFlag();
    int ret = PublishService(g_pubModuleName, pubInfo, &g_pubCallback);
    if (ret != SOFTBUS_TEST_SUCCESS) {
        printf("[DefaultPublishToInitService]call PublishService fail!\n");
    } else {
        WaitPublishResult();
        if (g_pubFlag != PUB_SUCCESS) {
            printf("[DefaultPublishToInitService]call PublishService fail!\n");
            ret  = SOFTBUS_TEST_FAIL;
        }
    }
    free(pubInfo);
}

/**
 * undo the first publish
 */
static void UnDefaultPublish(void)
{
    int ret = UnPublishService(g_pubModuleName, DEF_PUB_ID);
    if (ret != SOFTBUS_TEST_SUCCESS) {
        printf("[UnDefaultPublish]unpublish fail!\n");
    }
}

class ActsSoftBusTest : public testing::Test {
protected:
    // SetUpTestCase: Testsuit setup, run before 1st testcase
    static void SetUpTestCase(void)
    {
        g_pubCallback.onPublishSuccess = SbPublishSuccess;
        g_pubCallback.onPublishFail = SbPublishFail;
        g_sessionListenerCallback = (struct ISessionListener*)malloc(sizeof(struct ISessionListener));
        ASSERT_EQ(true, g_sessionListenerCallback != nullptr);

        g_sessionListenerCallback->onSessionOpened = SbSessionOpened;
        g_sessionListenerCallback->onSessionClosed = SbSessionClosed;
        g_sessionListenerCallback->onBytesReceived = SbOnBytesReceived;
        DefaultPublishToInitService();
        UnDefaultPublish();
    }
    // TearDownTestCase: Testsuit teardown, run after last testcase
    static void TearDownTestCase(void)
    {
        if (g_sessionListenerCallback != nullptr) {
            free(g_sessionListenerCallback);
            g_sessionListenerCallback = nullptr;
        }
    }
    // Testcase setup
    virtual void SetUp() {}
    // Testcase teardown
    virtual void TearDown() {}
};
/**
 * @tc.number    : SUB_COMMUNICATION_SOFTBUS_SDK_0100
 * @tc.name      : abnormal parameter test
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsSoftBusTest, testSetNumGreaterThanMax, Function | MediumTest | Level3)
{
    CommonDeviceInfo* devInfo = NULL;
    devInfo = (CommonDeviceInfo *)malloc(sizeof(CommonDeviceInfo));
    ASSERT_EQ(true, devInfo != NULL);
    (void)memset_s(devInfo, sizeof(CommonDeviceInfo), 0, sizeof(CommonDeviceInfo));
    devInfo->key = COMM_DEVICE_KEY_DEVID;
    devInfo->value = g_devId;
    
    int ret = SetCommonDeviceInfo(devInfo, COMM_DEVICE_KEY_MAX + 1);
    EXPECT_NE(SOFTBUS_TEST_SUCCESS, ret);
    ret  = SetCommonDeviceInfo(devInfo, 0);
    EXPECT_NE(SOFTBUS_TEST_SUCCESS, ret);
    ret  = SetCommonDeviceInfo(NULL, 1);
    EXPECT_NE(SOFTBUS_TEST_SUCCESS, ret);
    free(devInfo);
}

/**
 * @tc.number    : SUB_COMMUNICATION_SOFTBUS_SDK_0300
 * @tc.name      : set ID value equal and greater maximum value
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsSoftBusTest, testSetDevIdEqualMax, Function | MediumTest | Level3)
{
    const char* info1 =
        "abcdef123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
    const char* info2 =
        "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890abcdefg";
    CommonDeviceInfo* devInfo = NULL;
    devInfo = (CommonDeviceInfo *)malloc(sizeof(CommonDeviceInfo));
    ASSERT_EQ(true, devInfo != NULL);
    (void)memset_s(devInfo, sizeof(CommonDeviceInfo), 0, sizeof(CommonDeviceInfo));
    devInfo->key = COMM_DEVICE_KEY_DEVID;
    devInfo->value = info1;

    int ret = SetCommonDeviceInfo(devInfo, 1);
    EXPECT_NE(SOFTBUS_TEST_SUCCESS, ret);
    devInfo->value = info2;
    ret = SetCommonDeviceInfo(devInfo, 1);
    EXPECT_NE(SOFTBUS_TEST_SUCCESS, ret);
    free(devInfo);
}

/**
 * @tc.number    : SUB_COMMUNICATION_SOFTBUS_SDK_0600
 * @tc.name      : set name value equal and greater maximum value
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsSoftBusTest, testSetDevNameEqualMax, Function | MediumTest | Level3)
{
    const char* info1 = "abcd123456789012345678901234567890123456789012345678901234567890";
    const char* info2 = "123456789012345678901234567890123456789012345678901234567890abcde";
    CommonDeviceInfo* devInfo = NULL;
    devInfo = (CommonDeviceInfo *)malloc(sizeof(CommonDeviceInfo));
    ASSERT_EQ(true, devInfo != NULL);
    (void)memset_s(devInfo, sizeof(CommonDeviceInfo), 0, sizeof(CommonDeviceInfo));
    devInfo->key = COMM_DEVICE_KEY_DEVNAME;
    devInfo->value = info1;
    int ret = SetCommonDeviceInfo(devInfo, 1);
    EXPECT_NE(SOFTBUS_TEST_SUCCESS, ret);

    devInfo->value = info2;
    ret = SetCommonDeviceInfo(devInfo, 1);
    EXPECT_NE(SOFTBUS_TEST_SUCCESS, ret);
    free(devInfo);
}

/**
 * @tc.number    : SUB_COMMUNICATION_SOFTBUS_SDK_0800
 * @tc.name      : set type value not in enum
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsSoftBusTest, testSetDevTypeError, Function | MediumTest | Level3)
{
    const char* info = "error";
    CommonDeviceInfo* devInfo = NULL;
    devInfo = (CommonDeviceInfo *)malloc(sizeof(CommonDeviceInfo));
    ASSERT_EQ(true, devInfo != NULL);
    (void)memset_s(devInfo, sizeof(CommonDeviceInfo), 0, sizeof(CommonDeviceInfo));
    devInfo->key = COMM_DEVICE_KEY_DEVTYPE;
    devInfo->value = info;
    int ret = SetCommonDeviceInfo(devInfo, 1);
    EXPECT_NE(SOFTBUS_TEST_SUCCESS, ret);
    free(devInfo);
}

/**
 * @tc.number    : SUB_COMMUNICATION_SOFTBUS_SDK_1000
 * @tc.name      : set key is error
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsSoftBusTest, testSetDevKeyIsError, Function | MediumTest | Level2)
{
    CommonDeviceInfo* devInfo = NULL;
    devInfo = (CommonDeviceInfo *)malloc(sizeof(CommonDeviceInfo));
    ASSERT_EQ(true, devInfo != NULL);
    (void)memset_s(devInfo, sizeof(CommonDeviceInfo), 0, sizeof(CommonDeviceInfo));
    devInfo->key = COMM_DEVICE_KEY_MAX;
    devInfo->value = g_devId;
    unsigned int num = 1;
    int ret = SetCommonDeviceInfo(devInfo, num);
    EXPECT_NE(SOFTBUS_TEST_SUCCESS, ret);
    free(devInfo);
}

/**
 * @tc.number    : SUB_COMMUNICATION_SOFTBUS_SDK_1300
 * @tc.name      : Test publish with invalid parameter
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsSoftBusTest, testPublishParameterIsNull, Function | MediumTest | Level2)
{
    PublishInfo* pubInfo = NULL;
    pubInfo = (PublishInfo *)malloc(sizeof(PublishInfo));
    ASSERT_EQ(true, pubInfo!= NULL);
    (void)memset_s(pubInfo, sizeof(PublishInfo), 0, sizeof(PublishInfo));
    pubInfo->publishId = DEF_PUB_ID;
    pubInfo->mode = DISCOVER_MODE_PASSIVE;
    pubInfo->medium = COAP;
    pubInfo->freq = MID;
    pubInfo->capability = g_pubCapability;
    pubInfo->capabilityData = g_pubCapabilityData;
    pubInfo->dataLen = DEF_PUB_CAPABILITY_DATA_LEN;

    ResetPubFlag();
    int ret = PublishService(NULL, pubInfo, &g_pubCallback);
    EXPECT_NE(SOFTBUS_TEST_SUCCESS, ret);
    WaitPublishResult();
    EXPECT_EQ(PUB_FAIL, g_pubFlag);
    ret = PublishService(g_pubModuleName, NULL, &g_pubCallback);
    EXPECT_NE(SOFTBUS_TEST_SUCCESS, ret);
    ret = PublishService(g_pubModuleName, pubInfo, NULL);
    EXPECT_NE(SOFTBUS_TEST_SUCCESS, ret);
    ret = PublishService(NULL, NULL, NULL);
    EXPECT_NE(SOFTBUS_TEST_SUCCESS, ret);
    free(pubInfo);
}

/**
 * @tc.number    : SUB_COMMUNICATION_SOFTBUS_SDK_1500
 * @tc.name      : set package name value equal and greater than maximum value
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsSoftBusTest, testPublishPkgNameAbnormal, Function | MediumTest | Level2)
{
    PublishInfo* pubInfo = NULL;
    pubInfo = (PublishInfo *)malloc(sizeof(PublishInfo));
    ASSERT_EQ(true, pubInfo!= NULL);
    (void)memset_s(pubInfo, sizeof(PublishInfo), 0, sizeof(PublishInfo));
    pubInfo->publishId = DEF_PUB_ID;
    pubInfo->mode = DISCOVER_MODE_PASSIVE;
    pubInfo->medium = COAP;
    pubInfo->freq = MID;
    pubInfo->capability = g_pubCapability;
    pubInfo->capabilityData = g_pubCapabilityData;
    pubInfo->dataLen = DEF_PUB_CAPABILITY_DATA_LEN;

    ResetPubFlag();
    int ret = PublishService(NULL, pubInfo, &g_pubCallback);
    EXPECT_NE(SOFTBUS_TEST_SUCCESS, ret);
    WaitPublishResult();
    EXPECT_EQ(PUB_FAIL, g_pubFlag);

    ResetPubFlag();
    const char* pkgNameMax = "123456789012345678901234567890123456789012345678901234567890abcd";
    ret = PublishService(pkgNameMax, pubInfo, &g_pubCallback);
    EXPECT_NE(SOFTBUS_TEST_SUCCESS, ret);
    WaitPublishResult();
    EXPECT_EQ(PUB_FAIL, g_pubFlag);

    ResetPubFlag();
    const char* pkgNameMoreMax = "abcde123456789012345678901234567890123456789012345678901234567890";
    ret = PublishService(pkgNameMoreMax, pubInfo, &g_pubCallback);
    EXPECT_NE(SOFTBUS_TEST_SUCCESS, ret);
    WaitPublishResult();
    EXPECT_EQ(PUB_FAIL, g_pubFlag);

    ret = UnPublishService(pkgNameMax, DEF_PUB_ID);
    EXPECT_NE(SOFTBUS_TEST_SUCCESS, ret);
    ret = UnPublishService(pkgNameMoreMax, DEF_PUB_ID);
    EXPECT_NE(SOFTBUS_TEST_SUCCESS, ret);
    free(pubInfo);
}

/**
 * @tc.number    : SUB_COMMUNICATION_SOFTBUS_SDK_1700
 * @tc.name      : set capability value not in list
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsSoftBusTest, testPublishCapabilityError, Function | MediumTest | Level3)
{
    PublishInfo* pubInfo = NULL;
    pubInfo = (PublishInfo *)malloc(sizeof(PublishInfo));
    EXPECT_EQ(true, pubInfo!= NULL);
    (void)memset_s(pubInfo, sizeof(PublishInfo), 0, sizeof(PublishInfo));
    pubInfo->publishId = DEF_PUB_ID;
    pubInfo->mode = DISCOVER_MODE_PASSIVE;
    pubInfo->medium = COAP;
    pubInfo->freq = MID;
    pubInfo->capability = (char *)"error capability";
    pubInfo->capabilityData = g_pubCapabilityData;
    pubInfo->dataLen = DEF_PUB_CAPABILITY_DATA_LEN;

    ResetPubFlag();
    int ret = PublishService(g_pubModuleName, pubInfo, &g_pubCallback);
    EXPECT_NE(SOFTBUS_TEST_SUCCESS, ret);
    WaitPublishResult();
    EXPECT_EQ(PUB_FAIL, g_pubFlag);
    free(pubInfo);
}
