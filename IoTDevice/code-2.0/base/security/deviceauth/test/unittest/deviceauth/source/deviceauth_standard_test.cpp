/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "deviceauth_standard_test.h"
#include "deviceauth_test_mock.h"
#include <ctime>
extern "C" {
#include "common_defs.h"
#include "json_utils.h"
#include "device_auth.h"
#include "device_auth_defines.h"
#include "database_manager.h"
#include "hc_condition.h"
#include "hc_mutex.h"
#include "hc_types.h"
}

using namespace std;

static bool g_isNeedContinue = false;
static int64_t g_requestId = 0L;
static int g_operationCode = -1;
static int g_errorCode = 1;
static char *g_tempStr = nullptr;
static int g_messageCode = -1;
static char g_dataBuffer[BUFFER_SIZE] = { 0 };
static HcCondition g_testCondition;
static const DeviceGroupManager *g_testGm = nullptr;
static const GroupAuthManager *g_testGa = nullptr;
static DeviceAuthCallback g_gaCallback;

static void DelayWithMSec(int32_t mSecs)
{
    struct timeval out;
    out.tv_sec = 0;
    out.tv_usec = mSecs * 1000;
    (void)select(1, NULL, NULL, NULL, &out);
    return;
}

void ClearTempValue()
{
    g_isNeedContinue = false;
    g_requestId = 0L;
    g_operationCode = -1;
    g_errorCode = 1;
    g_tempStr = nullptr;
    g_messageCode = -1;
}

/* delete file path success */
static int32_t RemoveDir(const char *path)
{
    char strBuf[BUFFER_SIZE] = {0};
    if (path == nullptr) {
        return -1;
    }
    sprintf_s(strBuf, sizeof(strBuf) - 1, "rm -rf %s", path);
    cout << strBuf << endl;
    system(strBuf);
    return 0;
}

static void RemoveHuks(void)
{
    int ret;
    ret = RemoveDir("/data/data/maindata");
    cout << "[Clear] clear huks:maindata done: " << ret << endl;
    ret = RemoveDir("/data/data/bakdata");
    cout << "[Clear] clear huks:bakdata done: " << ret << endl;
}

static int DeleteDatabase()
{
    const char *groupPath = "/data/data/deviceauth/hcgroup.dat";
    int ret;
    ret = RemoveDir(groupPath);
    cout << "[Clear] clear db: done: " << ret << endl;
    RemoveHuks();
    /* wait for delete data */
    DelayWithMSec(500);
    return 0;
}

#define CHECK_GROUP_ID(groupInfo) const char *groupId = GetStringFromJson(groupInfo, FIELD_GROUP_ID); \
ret = strcmp(groupId, "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B"); \
EXPECT_EQ(ret == HC_SUCCESS, true);

#define PRINT_COST_TIME(costTime) cout << "[   TIME   ] used time: " << costTime << "(ms)" << endl;

#define CHECK_EXPIRE_TIME(groupInfo, tmpExpireTime) int expireTime = 0; \
GetIntFromJson(groupInfo, FIELD_EXPIRE_TIME, &expireTime); \
EXPECT_EQ(expireTime, tmpExpireTime);

static char *OnRequestNormal(int64_t requestId, int operationCode, const char *reqParams)
{
    g_messageCode = ON_REQUEST;
    g_requestId = requestId;
    g_operationCode = operationCode;
    CJson *json = CreateJson();
    AddIntToJson(json, FIELD_CONFIRMATION, REQUEST_ACCEPTED);
    AddStringToJson(json, FIELD_PIN_CODE, "123456");
    AddStringToJson(json, FIELD_DEVICE_ID, SERVER_AUTH_ID);
    AddIntToJson(json, FIELD_USER_TYPE, DEVICE_TYPE_ACCESSORY);
    AddIntToJson(json, FIELD_GROUP_VISIBILITY, GROUP_VISIBILITY_PUBLIC);
    AddIntToJson(json, FIELD_EXPIRE_TIME, 90);
    char *returnDataStr = PackJsonToString(json);
    FreeJson(json);
    return returnDataStr;
}

static char *OnRequestError1(int64_t requestId, int operationCode, const char *reqParams)
{
    g_messageCode = ON_REQUEST;
    g_requestId = requestId;
    g_operationCode = operationCode;
    CJson *json = CreateJson();
    AddIntToJson(json, FIELD_CONFIRMATION, REQUEST_REJECTED);
    AddStringToJson(json, FIELD_PIN_CODE, "123456");
    AddStringToJson(json, FIELD_DEVICE_ID, SERVER_AUTH_ID);
    AddIntToJson(json, FIELD_USER_TYPE, DEVICE_TYPE_ACCESSORY);
    AddIntToJson(json, FIELD_GROUP_VISIBILITY, GROUP_VISIBILITY_PUBLIC);
    AddIntToJson(json, FIELD_EXPIRE_TIME, 90);
    char *returnDataStr = PackJsonToString(json);
    FreeJson(json);
    return returnDataStr;
}

static char *OnRequestError2(int64_t requestId, int operationCode, const char *reqParams)
{
    g_messageCode = ON_REQUEST;
    g_requestId = requestId;
    g_operationCode = operationCode;
    CJson *json = CreateJson();
    AddIntToJson(json, FIELD_CONFIRMATION, REQUEST_ACCEPTED);
    AddStringToJson(json, FIELD_DEVICE_ID, SERVER_AUTH_ID);
    AddIntToJson(json, FIELD_USER_TYPE, DEVICE_TYPE_ACCESSORY);
    AddIntToJson(json, FIELD_GROUP_VISIBILITY, GROUP_VISIBILITY_PUBLIC);
    AddIntToJson(json, FIELD_EXPIRE_TIME, 90);
    char *returnDataStr = PackJsonToString(json);
    FreeJson(json);
    return returnDataStr;
}

static char *OnRequestError3(int64_t requestId, int operationCode, const char *reqParams)
{
    g_messageCode = ON_REQUEST;
    g_requestId = requestId;
    g_operationCode = operationCode;
    CJson *json = CreateJson();
    AddIntToJson(json, FIELD_CONFIRMATION, REQUEST_ACCEPTED);
    AddStringToJson(json, FIELD_PIN_CODE, "123456");
    AddStringToJson(json, FIELD_DEVICE_ID, SERVER_AUTH_ID);
    AddIntToJson(json, FIELD_USER_TYPE, -1);
    AddIntToJson(json, FIELD_GROUP_VISIBILITY, GROUP_VISIBILITY_PUBLIC);
    AddIntToJson(json, FIELD_EXPIRE_TIME, 90);
    char *returnDataStr = PackJsonToString(json);
    FreeJson(json);
    return returnDataStr;
}

static char *OnRequestError4(int64_t requestId, int operationCode, const char *reqParams)
{
    g_messageCode = ON_REQUEST;
    g_requestId = requestId;
    g_operationCode = operationCode;
    CJson *json = CreateJson();
    AddIntToJson(json, FIELD_CONFIRMATION, REQUEST_ACCEPTED);
    AddStringToJson(json, FIELD_PIN_CODE, "123456");
    AddStringToJson(json, FIELD_DEVICE_ID, SERVER_AUTH_ID);
    AddIntToJson(json, FIELD_USER_TYPE, DEVICE_TYPE_ACCESSORY);
    AddIntToJson(json, FIELD_GROUP_VISIBILITY, GROUP_VISIBILITY_SIGNATURE);
    AddIntToJson(json, FIELD_EXPIRE_TIME, 90);
    char *returnDataStr = PackJsonToString(json);
    FreeJson(json);
    return returnDataStr;
}

static char *OnRequestError5(int64_t requestId, int operationCode, const char *reqParams)
{
    g_messageCode = ON_REQUEST;
    g_requestId = requestId;
    g_operationCode = operationCode;
    CJson *json = CreateJson();
    AddIntToJson(json, FIELD_CONFIRMATION, REQUEST_ACCEPTED);
    AddStringToJson(json, FIELD_PIN_CODE, "123456");
    AddStringToJson(json, FIELD_DEVICE_ID, SERVER_AUTH_ID);
    AddIntToJson(json, FIELD_USER_TYPE, DEVICE_TYPE_ACCESSORY);
    AddIntToJson(json, FIELD_GROUP_VISIBILITY, GROUP_VISIBILITY_PUBLIC);
    AddIntToJson(json, FIELD_EXPIRE_TIME, -2);
    char *returnDataStr = PackJsonToString(json);
    FreeJson(json);
    return returnDataStr;
}

static void OnError(int64_t requestId, int operationCode, int errorCode, const char *errorReturn)
{
    (void)errorReturn;
    g_messageCode = ON_ERROR;
    g_requestId = requestId;
    g_operationCode = operationCode;
    g_errorCode = errorCode;
    g_testCondition.notify(&g_testCondition);
}

static void OnError2(int64_t requestId, int operationCode, int errorCode, const char *errorReturn)
{
    (void)errorReturn;
    g_messageCode = ON_ERROR;
    g_requestId = requestId;
    g_operationCode = operationCode;
    g_errorCode = errorCode;
}

static void OnFinish(int64_t requestId, int operationCode, const char *returnData)
{
    g_messageCode = ON_FINISH;
    g_requestId = requestId;
    g_operationCode = operationCode;
    if (operationCode == GROUP_CREATE) {
        CJson *json = CreateJsonFromString(returnData);
        const char *groupId = GetStringFromJson(json, FIELD_GROUP_ID);
        (void)memcpy_s(g_dataBuffer, BUFFER_SIZE, groupId, strlen(groupId));
        FreeJson(json);
    }
    g_testCondition.notify(&g_testCondition);
}

static void OnFinish2(int64_t requestId, int operationCode, const char *returnData)
{
    g_messageCode = ON_FINISH;
    g_requestId = requestId;
    g_operationCode = operationCode;
}

static void OnSessionKeyReturned(int64_t requestId, const uint8_t *sessionKey, uint32_t sessionKeyLen)
{
    g_messageCode = ON_SESSION_KEY_RETURNED;
    g_requestId = requestId;
}

static bool OnTransmit(int64_t requestId, const uint8_t *data, uint32_t dataLen)
{
    g_messageCode = ON_TRANSMIT;
    g_requestId = requestId;
    g_isNeedContinue = true;
    memset_s(g_dataBuffer, BUFFER_SIZE, 0, BUFFER_SIZE);
    memcpy_s(g_dataBuffer, BUFFER_SIZE, data, dataLen);
    return true;
}

static char *GaOnRequest(int64_t requestId, int operationCode, const char *reqParams)
{
    g_messageCode = ON_REQUEST;
    g_requestId = requestId;
    g_operationCode = operationCode;
    CJson *json = CreateJson();
    AddIntToJson(json, FIELD_CONFIRMATION, REQUEST_ACCEPTED);
    AddStringToJson(json, FIELD_SERVICE_PKG_NAME, TEST_APP_NAME);
    AddStringToJson(json, FIELD_PEER_CONN_DEVICE_ID, "udid1");
    char *returnDataStr = PackJsonToString(json);
    FreeJson(json);
    return returnDataStr;
}

enum {
    GROUP_CREATED = 0,
    GROUP_DELETED,
    DEVICE_BOUND,
    DEVICE_UNBOUND,
    DEVICE_NOT_TRUSTED,
    LAST_GROUP_DELETED,
    TRUSTED_DEVICE_NUM_CHANGED
};

static int g_receivedMessageNum[7] = {0};

void OnGroupCreated(const char *groupInfo)
{
    g_receivedMessageNum[GROUP_CREATED]++;
}

void OnGroupDeleted(const char *groupInfo)
{
    g_receivedMessageNum[GROUP_DELETED]++;
}

void OnDeviceBound(const char *peerUdid, const char *groupInfo)
{
    g_receivedMessageNum[DEVICE_BOUND]++;
}

void OnDeviceUnBound(const char *peerUdid, const char *groupInfo)
{
    g_receivedMessageNum[DEVICE_UNBOUND]++;
}

void OnDeviceNotTrusted(const char *peerUdid)
{
    g_receivedMessageNum[DEVICE_NOT_TRUSTED]++;
}

void OnLastGroupDeleted(const char *peerUdid, int groupType)
{
    g_receivedMessageNum[LAST_GROUP_DELETED]++;
}

void OnTrustedDeviceNumChanged(int curTrustedDeviceNum)
{
    g_receivedMessageNum[TRUSTED_DEVICE_NUM_CHANGED]++;
}

static void InitCaseResource(void)
{
    int32_t ret;
    const DeviceGroupManager *gmTmp = nullptr;
    ret = InitDeviceAuthService();
    EXPECT_EQ(ret == HC_SUCCESS, true);
    gmTmp = GetGmInstance();
    EXPECT_NE(gmTmp, nullptr);
    g_testGm = gmTmp;
    g_gaCallback.onRequest = OnRequestNormal;
    g_gaCallback.onTransmit = OnTransmit;
    g_gaCallback.onFinish = OnFinish;
    g_gaCallback.onError = OnError;
    g_gaCallback.onSessionKeyReturned = OnSessionKeyReturned;
    g_testGm->regCallback(TEST_APP_NAME, &g_gaCallback);
    DelayWithMSec(500);
}

static void DeInitCaseResource(void)
{
    DestroyDeviceAuthService();
    ClearTempValue();
    g_testGm = nullptr;
}

/* test suit - GET_INSTANCE */
void GET_INSTANCE::SetUpTestCase()
{
    int32_t ret;
    ret = InitDeviceAuthService();
    EXPECT_EQ(ret == HC_SUCCESS, true);
}

void GET_INSTANCE::TearDownTestCase()
{
    DestroyDeviceAuthService();
    ClearTempValue();
}

/* test suit - REGISTER_CALLBACK */
void REGISTER_CALLBACK::SetUp()
{
    int32_t ret;
    ret = InitDeviceAuthService();
    EXPECT_EQ(ret == HC_SUCCESS, true);
    gm = GetGmInstance();
    EXPECT_NE(gm, nullptr);
}

void REGISTER_CALLBACK::TearDown()
{
    DestroyDeviceAuthService();
    ClearTempValue();
    gm = nullptr;
}

/* test suit - CREATE_GROUP_P2P */
void CREATE_GROUP_P2P::SetUpTestCase()
{
    InitHcCond(&g_testCondition, nullptr);
}

void CREATE_GROUP_P2P::TearDownTestCase()
{
    DestroyHcCond(&g_testCondition);
}
void CREATE_GROUP_P2P::SetUp()
{
    SetClient(false);
    DeleteDatabase();
    InitCaseResource();
}

void CREATE_GROUP_P2P::TearDown()
{
    DeInitCaseResource();
}

/* test suit - AUTHENTICATE_GA */
void AUTHENTICATE_GA::SetUpTestCase()
{
    int32_t ret;
    ret = InitDeviceAuthService();
    EXPECT_EQ(ret == HC_SUCCESS, true);
    g_gaCallback = {
        OnTransmit,
        OnSessionKeyReturned,
        OnFinish,
        OnError,
        GaOnRequest
    };
    g_testGa = GetGaInstance();
    EXPECT_NE(g_testGa, nullptr);
    SetClient(false);
}

void AUTHENTICATE_GA::TearDownTestCase()
{
    DestroyDeviceAuthService();
    DeleteDatabase();
    ClearTempValue();
    g_testGa = nullptr;
}

/* test suit - ADD_MEMBER_TO_GROUP */
void ADD_MEMBER_TO_GROUP::SetUp()
{
    int32_t ret;
    const DeviceGroupManager *gmTmp = nullptr;

    DeleteDatabase();
    ret = InitDeviceAuthService();
    EXPECT_EQ(ret == HC_SUCCESS, true);
    g_gaCallback = {
        OnTransmit,
        OnSessionKeyReturned,
        OnFinish2,
        OnError2,
        OnRequestNormal
    };
    gmTmp = GetGmInstance();
    EXPECT_NE(gmTmp, nullptr);
    g_testGm = gmTmp;
    ret = g_testGm->regCallback(TEST_APP_NAME, &g_gaCallback);
    EXPECT_EQ(ret == HC_SUCCESS, true);
    SetClient(false);
}

void ADD_MEMBER_TO_GROUP::TearDown()
{
    DestroyDeviceAuthService();
    DeleteDatabase();
    ClearTempValue();
    g_testGm = nullptr;
}

/* test suit - REGISTER_LISTENER */
void REGISTER_LISTENER::SetUpTestCase()
{
    DeleteDatabase();
    InitHcCond(&g_testCondition, nullptr);
}

void REGISTER_LISTENER::TearDownTestCase()
{
    DestroyHcCond(&g_testCondition);
}

void REGISTER_LISTENER::SetUp()
{
    InitDeviceAuthService();
    g_gaCallback = {
        OnTransmit,
        OnSessionKeyReturned,
        OnFinish,
        OnError,
        OnRequestNormal
    };
    g_testGm = GetGmInstance();
    g_testGm->regCallback(TEST_APP_NAME, &g_gaCallback);
    DelayWithMSec(500);
}
void REGISTER_LISTENER::TearDown()
{
    DestroyDeviceAuthService();
    DeleteDatabase();
    ClearTempValue();
}

void QUERY_INTERFACE::SetUpTestCase()
{
    DeleteDatabase();
}

void QUERY_INTERFACE::TearDownTestCase() {}

void QUERY_INTERFACE::SetUp()
{
    InitDeviceAuthService();
    g_gaCallback = {
        OnTransmit,
        OnSessionKeyReturned,
        OnFinish2,
        OnError2,
        OnRequestNormal
    };
    g_testGm = GetGmInstance();
    g_testGm->regCallback(TEST_APP_NAME, &g_gaCallback);
}
void QUERY_INTERFACE::TearDown()
{
    DestroyDeviceAuthService();
    DeleteDatabase();
    ClearTempValue();
}

/* start cases */
TEST_F(GET_INSTANCE, TC_GET_GM_INSTANCE)
{
    const DeviceGroupManager *gm = GetGmInstance();
    EXPECT_NE(gm, nullptr);
}

TEST_F(GET_INSTANCE, TC_GET_GA_INSTANCE)
{
    const GroupAuthManager *ga = GetGaInstance();
    EXPECT_NE(ga, nullptr);
}

TEST_F(REGISTER_CALLBACK, TC_REGISTER_CALLBACK_NORMAL)
{
    DeviceAuthCallback callback;
    callback.onRequest = OnRequestNormal;
    callback.onError = OnError;
    callback.onFinish = OnFinish;
    callback.onSessionKeyReturned = OnSessionKeyReturned;
    callback.onTransmit = OnTransmit;
    int ret = gm->regCallback(TEST_APP_NAME, &callback);
    EXPECT_EQ(ret == HC_SUCCESS, true);
}

TEST_F(REGISTER_CALLBACK, TC_DEREGISTER_CALLBACK_NORMAL)
{
    EXPECT_EQ(gm->unRegCallback(TEST_APP_NAME) == HC_SUCCESS, true);
}

TEST_F(CREATE_GROUP_P2P, TC_CREATE_P2P_GROUP)
{
    g_gaCallback.onRequest = OnRequestNormal;
    g_gaCallback.onTransmit = OnTransmit;
    g_gaCallback.onFinish = OnFinish;
    g_gaCallback.onError = OnError;
    g_gaCallback.onSessionKeyReturned = OnSessionKeyReturned;
    g_testGm->regCallback(TEST_APP_NAME, &g_gaCallback);
    DelayWithMSec(500);

    CJson *createParams = CreateJson();
    AddIntToJson(createParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(createParams, FIELD_DEVICE_ID, "3C58C27533D8");
    AddIntToJson(createParams, FIELD_USER_TYPE, DEVICE_TYPE_ACCESSORY);
    AddIntToJson(createParams, FIELD_GROUP_VISIBILITY, GROUP_VISIBILITY_PUBLIC);
    AddIntToJson(createParams, FIELD_EXPIRE_TIME, 90);
    AddStringToJson(createParams, FIELD_GROUP_NAME, "P2PGroup");
    char *createParamsStr = PackJsonToString(createParams);
    int ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    FreeJsonString(createParamsStr);
    FreeJson(createParams);
    g_testCondition.wait(&g_testCondition);
    EXPECT_EQ(ret == HC_SUCCESS, true);
    EXPECT_EQ(g_messageCode, ON_FINISH);
    EXPECT_EQ(g_operationCode, GROUP_CREATE);
    EXPECT_EQ(g_requestId, TEMP_REQUEST_ID);
    CJson *returnData = CreateJson();
    AddStringToJson(returnData, FIELD_GROUP_OWNER, TEST_APP_NAME);
    char *queryParams = PackJsonToString(returnData);
    FreeJson(returnData);
    char *groupVec = nullptr;
    uint32_t num = 0;
    ret = g_testGm->getGroupInfo(TEST_APP_NAME, queryParams, &groupVec, &num);
    FreeJsonString(queryParams);
    EXPECT_EQ(ret == HC_SUCCESS, true);
    EXPECT_EQ(num, 1u);
    CJson *groupVecJson = CreateJsonFromString(groupVec);
    CJson *groupInfo = GetItemFromArray(groupVecJson, 0);
    CHECK_GROUP_ID(groupInfo);
    FreeJson(groupVecJson);
    g_testGm->destroyInfo(&groupVec);
}

TEST_F(CREATE_GROUP_P2P, TC_INVALID_PARAM_01)
{
    CJson *createParams = CreateJson();
    AddIntToJson(createParams, FIELD_GROUP_TYPE, 258);
    AddStringToJson(createParams, FIELD_DEVICE_ID, "3C58C27533D8");
    AddIntToJson(createParams, FIELD_USER_TYPE, DEVICE_TYPE_ACCESSORY);
    AddIntToJson(createParams, FIELD_GROUP_VISIBILITY, GROUP_VISIBILITY_PUBLIC);
    AddIntToJson(createParams, FIELD_EXPIRE_TIME, 90);
    AddStringToJson(createParams, FIELD_GROUP_NAME, "P2PGroup");
    char *createParamsStr = PackJsonToString(createParams);
    FreeJson(createParams);
    int ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    FreeJsonString(createParamsStr);
    g_testCondition.wait(&g_testCondition);
    EXPECT_EQ(ret == HC_SUCCESS, true);
    EXPECT_EQ(g_messageCode, ON_ERROR);
    EXPECT_EQ(g_operationCode, GROUP_CREATE);
    EXPECT_EQ(g_requestId, TEMP_REQUEST_ID);
    EXPECT_EQ(g_errorCode == HC_ERR_INVALID_PARAMS, true);
}

TEST_F(CREATE_GROUP_P2P, TC_INVALID_PARAM_02)
{
    CJson *createParams = CreateJson();
    AddIntToJson(createParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddIntToJson(createParams, FIELD_USER_TYPE, DEVICE_TYPE_ACCESSORY);
    AddIntToJson(createParams, FIELD_GROUP_VISIBILITY, GROUP_VISIBILITY_PUBLIC);
    AddIntToJson(createParams, FIELD_EXPIRE_TIME, 90);
    AddStringToJson(createParams, FIELD_GROUP_NAME, "P2PGroup");
    char *createParamsStr = PackJsonToString(createParams);
    FreeJson(createParams);
    int ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    FreeJsonString(createParamsStr);
    g_testCondition.wait(&g_testCondition);
    EXPECT_EQ(ret == HC_SUCCESS, true);
    EXPECT_EQ(g_messageCode, ON_FINISH);
    EXPECT_EQ(g_operationCode, GROUP_CREATE);
    EXPECT_EQ(g_requestId, TEMP_REQUEST_ID);
    CJson *returnData = CreateJson();
    AddStringToJson(returnData, FIELD_GROUP_OWNER, TEST_APP_NAME);
    char *queryParams = PackJsonToString(returnData);
    FreeJson(returnData);
    char *groupVec = nullptr;
    uint32_t num = 0;
    ret = g_testGm->getGroupInfo(TEST_APP_NAME, queryParams, &groupVec, &num);
    FreeJsonString(queryParams);
    EXPECT_EQ(ret == HC_SUCCESS, true);
    EXPECT_EQ(num, 1u);
    g_testGm->destroyInfo(&groupVec);
}

TEST_F(CREATE_GROUP_P2P, TC_INVALID_PARAM_03)
{
    CJson *createParams = CreateJson();
    AddIntToJson(createParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(createParams, FIELD_DEVICE_ID, "3C58C27533D8");
    AddIntToJson(createParams, FIELD_USER_TYPE, 3);
    AddIntToJson(createParams, FIELD_GROUP_VISIBILITY, GROUP_VISIBILITY_PUBLIC);
    AddIntToJson(createParams, FIELD_EXPIRE_TIME, 90);
    AddStringToJson(createParams, FIELD_GROUP_NAME, "P2PGroup");
    char *createParamsStr = PackJsonToString(createParams);
    FreeJson(createParams);
    int ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    FreeJsonString(createParamsStr);
    g_testCondition.wait(&g_testCondition);
    EXPECT_EQ(ret == HC_SUCCESS, true);
    EXPECT_EQ(g_messageCode, ON_ERROR);
    EXPECT_EQ(g_operationCode, GROUP_CREATE);
    EXPECT_EQ(g_requestId, TEMP_REQUEST_ID);
    EXPECT_EQ(g_errorCode == HC_ERR_INVALID_PARAMS, true);
}

TEST_F(CREATE_GROUP_P2P, TC_INVALID_PARAM_04)
{
    CJson *createParams = CreateJson();
    AddIntToJson(createParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(createParams, FIELD_DEVICE_ID, "3C58C27533D8");
    AddIntToJson(createParams, FIELD_USER_TYPE, DEVICE_TYPE_ACCESSORY);
    AddIntToJson(createParams, FIELD_GROUP_VISIBILITY, 1);
    AddIntToJson(createParams, FIELD_EXPIRE_TIME, 90);
    AddStringToJson(createParams, FIELD_GROUP_NAME, "P2PGroup");
    char *createParamsStr = PackJsonToString(createParams);
    FreeJson(createParams);
    int ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    FreeJsonString(createParamsStr);
    g_testCondition.wait(&g_testCondition);
    EXPECT_EQ(ret == HC_SUCCESS, true);
    EXPECT_EQ(g_messageCode, ON_ERROR);
    EXPECT_EQ(g_operationCode, GROUP_CREATE);
    EXPECT_EQ(g_requestId, TEMP_REQUEST_ID);
    EXPECT_EQ(g_errorCode == HC_ERR_INVALID_PARAMS, true);
}

TEST_F(CREATE_GROUP_P2P, TC_INVALID_PARAM_05)
{
    CJson *createParams = CreateJson();
    AddIntToJson(createParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(createParams, FIELD_DEVICE_ID, "3C58C27533D8");
    AddIntToJson(createParams, FIELD_USER_TYPE, DEVICE_TYPE_ACCESSORY);
    AddIntToJson(createParams, FIELD_GROUP_VISIBILITY, GROUP_VISIBILITY_PUBLIC);
    AddIntToJson(createParams, FIELD_EXPIRE_TIME, 90);
    char *createParamsStr = PackJsonToString(createParams);
    FreeJson(createParams);
    int ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    FreeJsonString(createParamsStr);
    g_testCondition.wait(&g_testCondition);
    EXPECT_EQ(ret == HC_SUCCESS, true);
    EXPECT_EQ(g_messageCode, ON_ERROR);
    EXPECT_EQ(g_operationCode, GROUP_CREATE);
    EXPECT_EQ(g_requestId, TEMP_REQUEST_ID);
    EXPECT_EQ(g_errorCode == HC_ERR_JSON_GET, true);
}

TEST_F(CREATE_GROUP_P2P, TC_INVALID_PARAM_06)
{
    CJson *createParams = CreateJson();
    AddIntToJson(createParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(createParams, FIELD_DEVICE_ID, "3C58C27533D8");
    AddIntToJson(createParams, FIELD_USER_TYPE, DEVICE_TYPE_ACCESSORY);
    AddIntToJson(createParams, FIELD_GROUP_VISIBILITY, GROUP_VISIBILITY_PUBLIC);
    AddIntToJson(createParams, FIELD_EXPIRE_TIME, 90);
    AddStringToJson(createParams, FIELD_GROUP_NAME, "P2PGroup");
    char *createParamsStr = PackJsonToString(createParams);
    int ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    g_testCondition.wait(&g_testCondition);
    ClearTempValue();
    ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    FreeJsonString(createParamsStr);
    FreeJson(createParams);
    g_testCondition.wait(&g_testCondition);
    EXPECT_EQ(ret == HC_SUCCESS, true);
    EXPECT_EQ(g_messageCode, ON_ERROR);
    EXPECT_EQ(g_operationCode, GROUP_CREATE);
    EXPECT_EQ(g_requestId, TEMP_REQUEST_ID);
    EXPECT_EQ(g_errorCode == HC_ERR_INVALID_PARAMS, true);
}

TEST_F(CREATE_GROUP_P2P, TC_INVALID_PARAM_07)
{
    CJson *createParams = CreateJson();
    AddIntToJson(createParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(createParams, FIELD_DEVICE_ID, "3C58C27533D8");
    AddIntToJson(createParams, FIELD_USER_TYPE, DEVICE_TYPE_ACCESSORY);
    AddIntToJson(createParams, FIELD_GROUP_VISIBILITY, GROUP_VISIBILITY_PUBLIC);
    AddIntToJson(createParams, FIELD_EXPIRE_TIME, -2);
    AddStringToJson(createParams, FIELD_GROUP_NAME, "P2PGroup");
    char *createParamsStr = PackJsonToString(createParams);
    FreeJson(createParams);
    int ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    FreeJsonString(createParamsStr);
    g_testCondition.wait(&g_testCondition);
    EXPECT_EQ(ret == HC_SUCCESS, true);
    EXPECT_EQ(g_messageCode, ON_ERROR);
    EXPECT_EQ(g_operationCode, GROUP_CREATE);
    EXPECT_EQ(g_requestId, TEMP_REQUEST_ID);
    EXPECT_EQ(g_errorCode == HC_ERR_INVALID_PARAMS, true);
}

TEST_F(CREATE_GROUP_P2P, TC_INVALID_PARAM_08)
{
    CJson *createParams = CreateJson();
    AddIntToJson(createParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(createParams, FIELD_DEVICE_ID, "3C58C27533D8");
    AddIntToJson(createParams, FIELD_USER_TYPE, DEVICE_TYPE_ACCESSORY);
    AddIntToJson(createParams, FIELD_GROUP_VISIBILITY, GROUP_VISIBILITY_PUBLIC);
    AddIntToJson(createParams, FIELD_EXPIRE_TIME, 0);
    AddStringToJson(createParams, FIELD_GROUP_NAME, "P2PGroup");
    char *createParamsStr = PackJsonToString(createParams);
    FreeJson(createParams);
    int ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    FreeJsonString(createParamsStr);
    g_testCondition.wait(&g_testCondition);
    EXPECT_EQ(ret == HC_SUCCESS, true);
    EXPECT_EQ(g_messageCode, ON_ERROR);
    EXPECT_EQ(g_operationCode, GROUP_CREATE);
    EXPECT_EQ(g_requestId, TEMP_REQUEST_ID);
    EXPECT_EQ(g_errorCode == HC_ERR_INVALID_PARAMS, true);
}

TEST_F(CREATE_GROUP_P2P, TC_INVALID_PARAM_09)
{
    CJson *createParams = CreateJson();
    AddIntToJson(createParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(createParams, FIELD_DEVICE_ID, "3C58C27533D8");
    AddIntToJson(createParams, FIELD_USER_TYPE, DEVICE_TYPE_ACCESSORY);
    AddIntToJson(createParams, FIELD_GROUP_VISIBILITY, GROUP_VISIBILITY_PUBLIC);
    AddIntToJson(createParams, FIELD_EXPIRE_TIME, 91);
    AddStringToJson(createParams, FIELD_GROUP_NAME, "P2PGroup");
    char *createParamsStr = PackJsonToString(createParams);
    FreeJson(createParams);
    int ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    FreeJsonString(createParamsStr);
    g_testCondition.wait(&g_testCondition);
    EXPECT_EQ(ret == HC_SUCCESS, true);
    EXPECT_EQ(g_messageCode, ON_ERROR);
    EXPECT_EQ(g_operationCode, GROUP_CREATE);
    EXPECT_EQ(g_requestId, TEMP_REQUEST_ID);
    EXPECT_EQ(g_errorCode == HC_ERR_INVALID_PARAMS, true);
}

TEST_F(CREATE_GROUP_P2P, TC_VALID_PARAM_01)
{
    CJson *createParams = CreateJson();
    AddIntToJson(createParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(createParams, FIELD_DEVICE_ID, "3C58C27533D9");
    AddIntToJson(createParams, FIELD_USER_TYPE, DEVICE_TYPE_ACCESSORY);
    AddIntToJson(createParams, FIELD_GROUP_VISIBILITY, GROUP_VISIBILITY_PUBLIC);
    AddIntToJson(createParams, FIELD_EXPIRE_TIME, 1);
    AddStringToJson(createParams, FIELD_GROUP_NAME, "P2PGroup");
    char *createParamsStr = PackJsonToString(createParams);
    FreeJson(createParams);
    int ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    FreeJsonString(createParamsStr);
    g_testCondition.wait(&g_testCondition);
    EXPECT_EQ(ret == HC_SUCCESS, true);
    EXPECT_EQ(g_messageCode, ON_FINISH);
    EXPECT_EQ(g_operationCode, GROUP_CREATE);
    EXPECT_EQ(g_requestId, TEMP_REQUEST_ID);
}

TEST_F(CREATE_GROUP_P2P, TC_VALID_PARAM_02)
{
    CJson *createParams = CreateJson();
    AddIntToJson(createParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(createParams, FIELD_DEVICE_ID, "3C58C27533D9");
    AddIntToJson(createParams, FIELD_USER_TYPE, DEVICE_TYPE_ACCESSORY);
    AddIntToJson(createParams, FIELD_GROUP_VISIBILITY, GROUP_VISIBILITY_PUBLIC);
    AddIntToJson(createParams, FIELD_EXPIRE_TIME, -1);
    AddStringToJson(createParams, FIELD_GROUP_NAME, "P2PGroup");
    char *createParamsStr = PackJsonToString(createParams);
    FreeJson(createParams);
    int ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    FreeJsonString(createParamsStr);
    g_testCondition.wait(&g_testCondition);
    EXPECT_EQ(ret == HC_SUCCESS, true);
    EXPECT_EQ(g_messageCode, ON_FINISH);
    EXPECT_EQ(g_operationCode, GROUP_CREATE);
    EXPECT_EQ(g_requestId, TEMP_REQUEST_ID);
}

TEST_F(CREATE_GROUP_P2P, TC_MAX_GROUP_NUMBER)
{
    CJson *createParams = CreateJson();
    AddIntToJson(createParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(createParams, FIELD_DEVICE_ID, "7533D8");
    AddIntToJson(createParams, FIELD_USER_TYPE, DEVICE_TYPE_ACCESSORY);
    AddIntToJson(createParams, FIELD_GROUP_VISIBILITY, GROUP_VISIBILITY_PUBLIC);
    AddIntToJson(createParams, FIELD_EXPIRE_TIME, 90);
    for (int i = 1; i < 101; ++i) {
        char str[STR_BUFF_SZ_MIN] = {0};
        (void)sprintf_s(str, sizeof(str) - 1, "P2PGroup%d", i);
        AddStringToJson(createParams, FIELD_GROUP_NAME, str);
        char *createParamsStr = PackJsonToString(createParams);
        int ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
        FreeJsonString(createParamsStr);
        g_testCondition.wait(&g_testCondition);
        EXPECT_EQ(ret == HC_SUCCESS, true);
        EXPECT_EQ(g_messageCode, ON_FINISH);
        EXPECT_EQ(g_operationCode, GROUP_CREATE);
        EXPECT_EQ(g_requestId, TEMP_REQUEST_ID);
        (void)DelTrustedDevice(
            "ABCDEF00ABCDEF00ABCDEF00ABCDEF00ABCDEF00ABCDEF00ABCDEF00ABCDEF00", g_dataBuffer);
        DelayWithMSec(500);
    }
    char str[STR_BUFF_SZ_MIN] = {0};
    (void)sprintf_s(str, sizeof(str) - 1, "P2PGroup%d", 101);
    AddStringToJson(createParams, FIELD_GROUP_NAME, str);
    char *createParamsStr = PackJsonToString(createParams);
    FreeJson(createParams);
    int ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    FreeJsonString(createParamsStr);
    g_testCondition.wait(&g_testCondition);
    EXPECT_EQ(ret == HC_SUCCESS, true);
    EXPECT_EQ(g_messageCode, ON_ERROR);
    EXPECT_EQ(g_operationCode, GROUP_CREATE);
    EXPECT_EQ(g_requestId, TEMP_REQUEST_ID);
}

TEST_F(ADD_MEMBER_TO_GROUP, TC_DEV_P2P_BIND_01)
{
    SetClient(true);
    g_isNeedContinue = true;
    const char * createParamsStr = "{\"groupType\":256,\"deviceId\":\"3C58C27533D8\",\"userType\":0,\""
                                   "groupVisibility\":-1,\"expireTime\":90,\"groupName\":\"P2PGroup\"}";
    (void)g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    DelayWithMSec(500);
    DeviceInfo *devAuthParams = CreateDeviceInfoStruct();
    devAuthParams->devType = DEVICE_TYPE_ACCESSORY;
    StringSetPointer(&(devAuthParams->authId), "CAF34E13190CBA510AA8DABB70CDFF8E9F623656DED400EF0D4CFD9E88FD6202");
    StringSetPointer(&(devAuthParams->udid), "ABCDEF00ABCDEF00ABCDEF00ABCDEF00ABCDEF00ABCDEF00ABCDEF00ABCDEF00");
    StringSetPointer(&(devAuthParams->groupId), "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B");
    StringSetPointer(&(devAuthParams->serviceType), "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B");
    AddTrustedDevice(devAuthParams, NULL);
    DestroyDeviceInfoStruct(devAuthParams);
    CJson *addParams = CreateJson();
    AddStringToJson(addParams, FIELD_GROUP_ID, "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B");
    AddIntToJson(addParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(addParams, FIELD_PIN_CODE, "123456");
    AddBoolToJson(addParams, FIELD_IS_ADMIN, true);
    char *addParamsStr = PackJsonToString(addParams);
    FreeJson(addParams);
    (void)g_testGm->addMemberToGroup(CLIENT_REQUEST_ID, TEST_APP_NAME, addParamsStr);
    FreeJsonString(addParamsStr);
    DelayWithMSec(500);
    while (g_isNeedContinue)
    {
        SetClient(!GetClient());
        CJson *data = CreateJsonFromString(g_dataBuffer);
        int64_t req = DEFAULT_REQUEST_ID;
        GetInt64FromJson(data, FIELD_REQUEST_ID, &req);
        if (req == CLIENT_REQUEST_ID) {
            req = SERVER_REQUEST_ID;
        } else {
            req = CLIENT_REQUEST_ID;
        }
        AddInt64StringToJson(data, FIELD_REQUEST_ID, req);
        char *dataStr = PackJsonToString(data);
        FreeJson(data);
        memset_s(g_dataBuffer, BUFFER_SIZE, 0, BUFFER_SIZE);
        memcpy_s(g_dataBuffer, BUFFER_SIZE, dataStr, strlen(dataStr) + 1);
        FreeJsonString(dataStr);
        g_testGm->processData(req, (uint8_t *)g_dataBuffer, strlen(g_dataBuffer) + 1);
        g_isNeedContinue = false;
        DelayWithMSec(500);
    }
    EXPECT_EQ(g_messageCode, ON_FINISH);
    EXPECT_EQ(g_operationCode, MEMBER_INVITE);
}

static char *ConstructAddParams02()
{
    CJson *addParams = CreateJson();
    AddStringToJson(addParams, FIELD_GROUP_ID, "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B");
    AddIntToJson(addParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(addParams, FIELD_PIN_CODE, "123456");
    AddBoolToJson(addParams, FIELD_IS_ADMIN, false);
    AddStringToJson(addParams, FIELD_DEVICE_ID, CLIENT_AUTH_ID);
    AddStringToJson(addParams, FIELD_GROUP_NAME, "P2PGroup");
    char *addParamsStr = PackJsonToString(addParams);
    FreeJson(addParams);
    return addParamsStr;
}

TEST_F(ADD_MEMBER_TO_GROUP, TC_DEV_P2P_BIND_02)
{
    SetClient(false);
    g_isNeedContinue = true;
    const char * createParamsStr =
        "{\"groupType\":256,\"deviceId\":\"CAF34E13190CBA510AA8DABB70CDFF8E9F623656DED400EF0D4CFD9E88FD6202\","
        "\"userType\":0,\"groupVisibility\":-1,\"expireTime\":90,\"groupName\":\"P2PGroup\"}";
    g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    DelayWithMSec(500);
    SetClient(true);
    DeviceInfo *devAuthParams = CreateDeviceInfoStruct();
    devAuthParams->devType = DEVICE_TYPE_ACCESSORY;
    StringSetPointer(&(devAuthParams->authId), "3C58C27533D8");
    StringSetPointer(&(devAuthParams->udid), "D6350E39AD8F11963C181BEEDC11AC85158E04466B68F1F4E6D895237E0FE81C");
    StringSetPointer(&(devAuthParams->groupId), "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B");
    StringSetPointer(&(devAuthParams->serviceType), "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B");
    AddTrustedDevice(devAuthParams, NULL);
    DestroyDeviceInfoStruct(devAuthParams);
    char *addParamsStr = ConstructAddParams02();
    g_testGm->addMemberToGroup(CLIENT_REQUEST_ID, TEST_APP_NAME, addParamsStr);
    FreeJsonString(addParamsStr);
    DelayWithMSec(500);
    while (g_isNeedContinue)
    {
        SetClient(!GetClient());
        CJson *data = CreateJsonFromString(g_dataBuffer);
        int64_t req = DEFAULT_REQUEST_ID;
        GetInt64FromJson(data, FIELD_REQUEST_ID, &req);
        if (req == CLIENT_REQUEST_ID) {
            req = SERVER_REQUEST_ID;
        } else {
            req = CLIENT_REQUEST_ID;
        }
        AddInt64StringToJson(data, FIELD_REQUEST_ID, req);
        char *dataStr = PackJsonToString(data);
        FreeJson(data);
        memset_s(g_dataBuffer, BUFFER_SIZE, 0, BUFFER_SIZE);
        memcpy_s(g_dataBuffer, BUFFER_SIZE, dataStr, strlen(dataStr) + 1);
        FreeJsonString(dataStr);
        g_testGm->processData(req, (uint8_t *)g_dataBuffer, strlen(g_dataBuffer) + 1);
        g_isNeedContinue = false;
        DelayWithMSec(500);
    }
    EXPECT_EQ(g_messageCode, ON_FINISH);
    EXPECT_EQ(g_operationCode, MEMBER_JOIN);
}

TEST_F(ADD_MEMBER_TO_GROUP, TC_DEV_P2P_BIND_03)
{
    SetClient(true);
    g_isNeedContinue = true;
    const char * createParamsStr = "{\"groupType\":256,\"deviceId\":\"3C58C27533D8\",\"userType\":0,\""
                                   "groupVisibility\":-1,\"expireTime\":90,\"groupName\":\"P2PGroup\"}";
    g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    /* delay 1 seconds */
    DelayWithMSec(500);
    CJson *addParams = CreateJson();
    AddStringToJson(addParams, FIELD_GROUP_ID, "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B");
    AddIntToJson(addParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(addParams, FIELD_PIN_CODE, "123456");
    AddBoolToJson(addParams, FIELD_IS_ADMIN, true);
    char *addParamsStr = PackJsonToString(addParams);
    FreeJson(addParams);
    g_testGm->addMemberToGroup(CLIENT_REQUEST_ID, TEST_APP_NAME, addParamsStr);
    DelayWithMSec(500);
    g_testGm->addMemberToGroup(CLIENT_REQUEST_ID, TEST_APP_NAME, addParamsStr);
    FreeJsonString(addParamsStr);
    DelayWithMSec(500);
    EXPECT_EQ(g_messageCode, ON_ERROR);
    EXPECT_EQ(g_operationCode, MEMBER_INVITE);
}

TEST_F(ADD_MEMBER_TO_GROUP, TC_DEV_P2P_BIND_04)
{
    SetClient(true);
    g_isNeedContinue = true;
    const char * createParamsStr = "{\"groupType\":256,\"deviceId\":\"3C58C27533D8\",\"userType\":0,\""
                                   "groupVisibility\":-1,\"expireTime\":90,\"groupName\":\"P2PGroup\"}";
    g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    /* delay 1 seconds */
    DelayWithMSec(500);
    CJson *addParams = CreateJson();
    AddStringToJson(addParams, FIELD_GROUP_ID, "abc");
    AddIntToJson(addParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(addParams, FIELD_PIN_CODE, "123456");
    AddBoolToJson(addParams, FIELD_IS_ADMIN, true);
    char *addParamsStr = PackJsonToString(addParams);
    FreeJson(addParams);
    g_testGm->addMemberToGroup(CLIENT_REQUEST_ID, TEST_APP_NAME, addParamsStr);
    FreeJsonString(addParamsStr);
    DelayWithMSec(500);
    EXPECT_EQ(g_messageCode, ON_ERROR);
    EXPECT_EQ(g_operationCode, MEMBER_INVITE);
}

TEST_F(ADD_MEMBER_TO_GROUP, TC_DEV_P2P_BIND_05)
{
    SetClient(true);
    g_isNeedContinue = true;
    const char * createParamsStr = "{\"groupType\":256,\"deviceId\":\"3C58C27533D8\",\"userType\":0,\""
                                   "groupVisibility\":-1,\"expireTime\":90,\"groupName\":\"P2PGroup\"}";
    g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    /* delay 1 seconds */
    DelayWithMSec(500);
    CJson *addParams = CreateJson();
    AddStringToJson(addParams, FIELD_GROUP_ID, "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B");
    AddIntToJson(addParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(addParams, FIELD_PIN_CODE, "123456");
    AddBoolToJson(addParams, FIELD_IS_ADMIN, true);
    char *addParamsStr = PackJsonToString(addParams);
    FreeJson(addParams);
    int ret = g_testGm->addMemberToGroup(CLIENT_REQUEST_ID, "testApp2", addParamsStr);
    FreeJsonString(addParamsStr);
    DelayWithMSec(500);
    EXPECT_EQ(ret != HC_SUCCESS, true);
}

TEST_F(ADD_MEMBER_TO_GROUP, TC_DEV_P2P_BIND_06)
{
    SetClient(true);
    g_isNeedContinue = true;
    const char * createParamsStr = "{\"groupType\":256,\"deviceId\":\"3C58C27533D8\",\"userType\":0,\""
                                   "groupVisibility\":-1,\"expireTime\":90,\"groupName\":\"P2PGroup\"}";
    g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    /* delay 1 seconds */
    DelayWithMSec(500);
    CJson *addParams = CreateJson();
    AddStringToJson(addParams, FIELD_GROUP_ID, "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B");
    AddIntToJson(addParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddBoolToJson(addParams, FIELD_IS_ADMIN, true);
    char *addParamsStr = PackJsonToString(addParams);
    FreeJson(addParams);
    g_testGm->addMemberToGroup(CLIENT_REQUEST_ID, TEST_APP_NAME, addParamsStr);
    FreeJsonString(addParamsStr);
    DelayWithMSec(500);
    EXPECT_EQ(g_messageCode, ON_ERROR);
    EXPECT_EQ(g_operationCode, MEMBER_INVITE);
}

TEST_F(ADD_MEMBER_TO_GROUP, TC_DEV_P2P_BIND_07)
{
    SetClient(true);
    g_isNeedContinue = true;
    g_gaCallback.onRequest = OnRequestError1;
    const char * createParamsStr = "{\"groupType\":256,\"deviceId\":\"3C58C27533D8\",\"userType\":0,\""
                                   "groupVisibility\":-1,\"expireTime\":90,\"groupName\":\"P2PGroup\"}";
    g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    /* delay 1 seconds */
    DelayWithMSec(500);
    CJson *addParams = CreateJson();
    AddStringToJson(addParams, FIELD_GROUP_ID, "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B");
    AddIntToJson(addParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(addParams, FIELD_PIN_CODE, "123456");
    AddBoolToJson(addParams, FIELD_IS_ADMIN, true);
    char *addParamsStr = PackJsonToString(addParams);
    FreeJson(addParams);
    g_testGm->addMemberToGroup(CLIENT_REQUEST_ID, TEST_APP_NAME, addParamsStr);
    FreeJsonString(addParamsStr);
    DelayWithMSec(500);
    while (g_isNeedContinue)
    {
        SetClient(!GetClient());
        CJson *data = CreateJsonFromString(g_dataBuffer);
        int64_t req = DEFAULT_REQUEST_ID;
        GetInt64FromJson(data, FIELD_REQUEST_ID, &req);
        if (req == CLIENT_REQUEST_ID) {
            req = SERVER_REQUEST_ID;
        } else {
            req = CLIENT_REQUEST_ID;
        }
        AddInt64StringToJson(data, FIELD_REQUEST_ID, req);
        char *dataStr = PackJsonToString(data);
        FreeJson(data);
        memset_s(g_dataBuffer, BUFFER_SIZE, 0, BUFFER_SIZE);
        memcpy_s(g_dataBuffer, BUFFER_SIZE, dataStr, strlen(dataStr) + 1);
        FreeJsonString(dataStr);
        g_testGm->processData(req, (uint8_t *)g_dataBuffer, strlen(g_dataBuffer) + 1);
        g_isNeedContinue = false;
        DelayWithMSec(500);
    }
    EXPECT_EQ(g_messageCode, ON_ERROR);
    EXPECT_EQ(g_operationCode, MEMBER_INVITE);
}

TEST_F(ADD_MEMBER_TO_GROUP, TC_DEV_P2P_BIND_08)
{
    SetClient(true);
    g_isNeedContinue = true;
    g_gaCallback.onRequest = OnRequestError2;
    const char * createParamsStr = "{\"groupType\":256,\"deviceId\":\"3C58C27533D8\",\"userType\":0,\""
                                   "groupVisibility\":-1,\"expireTime\":90,\"groupName\":\"P2PGroup\"}";
    g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    /* delay 1 seconds */
    DelayWithMSec(500);
    CJson *addParams = CreateJson();
    AddStringToJson(addParams, FIELD_GROUP_ID, "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B");
    AddIntToJson(addParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(addParams, FIELD_PIN_CODE, "123456");
    AddBoolToJson(addParams, FIELD_IS_ADMIN, true);
    char *addParamsStr = PackJsonToString(addParams);
    FreeJson(addParams);
    g_testGm->addMemberToGroup(CLIENT_REQUEST_ID, TEST_APP_NAME, addParamsStr);
    FreeJsonString(addParamsStr);
    DelayWithMSec(500);
    while (g_isNeedContinue)
    {
        SetClient(!GetClient());
        CJson *data = CreateJsonFromString(g_dataBuffer);
        int64_t req = DEFAULT_REQUEST_ID;
        GetInt64FromJson(data, FIELD_REQUEST_ID, &req);
        if (req == CLIENT_REQUEST_ID) {
            req = SERVER_REQUEST_ID;
        } else {
            req = CLIENT_REQUEST_ID;
        }
        AddInt64StringToJson(data, FIELD_REQUEST_ID, req);
        char *dataStr = PackJsonToString(data);
        FreeJson(data);
        memset_s(g_dataBuffer, BUFFER_SIZE, 0, BUFFER_SIZE);
        memcpy_s(g_dataBuffer, BUFFER_SIZE, dataStr, strlen(dataStr) + 1);
        FreeJsonString(dataStr);
        g_testGm->processData(req, (uint8_t *)g_dataBuffer, strlen(g_dataBuffer) + 1);
        g_isNeedContinue = false;
        DelayWithMSec(500);
    }
    EXPECT_EQ(g_messageCode, ON_ERROR);
    EXPECT_EQ(g_operationCode, MEMBER_INVITE);
}

TEST_F(ADD_MEMBER_TO_GROUP, TC_DEV_P2P_BIND_10)
{
    SetClient(true);
    g_isNeedContinue = true;
    g_gaCallback.onRequest = OnRequestError3;
    const char * createParamsStr = "{\"groupType\":256,\"deviceId\":\"3C58C27533D8\",\"userType\":0,\""
                                   "groupVisibility\":-1,\"expireTime\":90,\"groupName\":\"P2PGroup\"}";
    g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    /* delay 1 seconds */
    DelayWithMSec(500);
    CJson *addParams = CreateJson();
    AddStringToJson(addParams, FIELD_GROUP_ID, "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B");
    AddIntToJson(addParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(addParams, FIELD_PIN_CODE, "123456");
    AddBoolToJson(addParams, FIELD_IS_ADMIN, true);
    char *addParamsStr = PackJsonToString(addParams);
    FreeJson(addParams);
    g_testGm->addMemberToGroup(CLIENT_REQUEST_ID, TEST_APP_NAME, addParamsStr);
    FreeJsonString(addParamsStr);
    DelayWithMSec(500);
    while (g_isNeedContinue)
    {
        SetClient(!GetClient());
        CJson *data = CreateJsonFromString(g_dataBuffer);
        int64_t req = DEFAULT_REQUEST_ID;
        GetInt64FromJson(data, FIELD_REQUEST_ID, &req);
        if (req == CLIENT_REQUEST_ID) {
            req = SERVER_REQUEST_ID;
        } else {
            req = CLIENT_REQUEST_ID;
        }
        AddInt64StringToJson(data, FIELD_REQUEST_ID, req);
        char *dataStr = PackJsonToString(data);
        FreeJson(data);
        memset_s(g_dataBuffer, BUFFER_SIZE, 0, BUFFER_SIZE);
        memcpy_s(g_dataBuffer, BUFFER_SIZE, dataStr, strlen(dataStr) + 1);
        FreeJsonString(dataStr);
        g_testGm->processData(req, (uint8_t *)g_dataBuffer, strlen(g_dataBuffer) + 1);
        g_isNeedContinue = false;
        DelayWithMSec(500);
    }
    EXPECT_EQ(g_messageCode, ON_ERROR);
    EXPECT_EQ(g_operationCode, MEMBER_INVITE);
}

TEST_F(ADD_MEMBER_TO_GROUP, TC_DEV_P2P_BIND_11)
{
    SetClient(true);
    g_isNeedContinue = true;
    g_gaCallback.onRequest = OnRequestError4;
    const char * createParamsStr = "{\"groupType\":256,\"deviceId\":\"3C58C27533D8\",\"userType\":0,\""
                                   "groupVisibility\":-1,\"expireTime\":90,\"groupName\":\"P2PGroup\"}";
    g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    /* delay 1 second */
    DelayWithMSec(500);
    CJson *addParams = CreateJson();
    AddStringToJson(addParams, FIELD_GROUP_ID, "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B");
    AddIntToJson(addParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(addParams, FIELD_PIN_CODE, "123456");
    AddBoolToJson(addParams, FIELD_IS_ADMIN, true);
    char *addParamsStr = PackJsonToString(addParams);
    FreeJson(addParams);
    g_testGm->addMemberToGroup(CLIENT_REQUEST_ID, TEST_APP_NAME, addParamsStr);
    FreeJsonString(addParamsStr);
    DelayWithMSec(500);
    while (g_isNeedContinue)
    {
        SetClient(!GetClient());
        CJson *data = CreateJsonFromString(g_dataBuffer);
        int64_t req = DEFAULT_REQUEST_ID;
        GetInt64FromJson(data, FIELD_REQUEST_ID, &req);
        if (req == CLIENT_REQUEST_ID) {
        req = SERVER_REQUEST_ID;
        } else {
        req = CLIENT_REQUEST_ID;
        }
        AddInt64StringToJson(data, FIELD_REQUEST_ID, req);
        char *dataStr = PackJsonToString(data);
        FreeJson(data);
        memset_s(g_dataBuffer, BUFFER_SIZE, 0, BUFFER_SIZE);
        memcpy_s(g_dataBuffer, BUFFER_SIZE, dataStr, strlen(dataStr) + 1);
        FreeJsonString(dataStr);
        g_testGm->processData(req, (uint8_t *)g_dataBuffer, strlen(g_dataBuffer) + 1);
        g_isNeedContinue = false;
        DelayWithMSec(500);
    }
    EXPECT_EQ(g_messageCode, ON_ERROR);
    EXPECT_EQ(g_operationCode, MEMBER_INVITE);
}

TEST_F(ADD_MEMBER_TO_GROUP, TC_DEV_P2P_BIND_12)
{
    SetClient(true);
    g_isNeedContinue = true;
    g_gaCallback.onRequest = OnRequestError5;
    const char * createParamsStr = "{\"groupType\":256,\"deviceId\":\"3C58C27533D8\",\"userType\":0,\""
                                   "groupVisibility\":-1,\"expireTime\":90,\"groupName\":\"P2PGroup\"}";
    g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    /* delay 1 second */
    DelayWithMSec(500);
    CJson *addParams = CreateJson();
    AddStringToJson(addParams, FIELD_GROUP_ID, "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B");
    AddIntToJson(addParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(addParams, FIELD_PIN_CODE, "123456");
    AddBoolToJson(addParams, FIELD_IS_ADMIN, true);
    char *addParamsStr = PackJsonToString(addParams);
    FreeJson(addParams);
    g_testGm->addMemberToGroup(CLIENT_REQUEST_ID, TEST_APP_NAME, addParamsStr);
    FreeJsonString(addParamsStr);
    DelayWithMSec(500);
    while (g_isNeedContinue)
    {
        SetClient(!GetClient());
        CJson *data = CreateJsonFromString(g_dataBuffer);
        int64_t req = DEFAULT_REQUEST_ID;
        GetInt64FromJson(data, FIELD_REQUEST_ID, &req);
        if (req == CLIENT_REQUEST_ID) {
            req = SERVER_REQUEST_ID;
        } else {
            req = CLIENT_REQUEST_ID;
        }
        AddInt64StringToJson(data, FIELD_REQUEST_ID, req);
        char *dataStr = PackJsonToString(data);
        FreeJson(data);
        memset_s(g_dataBuffer, BUFFER_SIZE, 0, BUFFER_SIZE);
        memcpy_s(g_dataBuffer, BUFFER_SIZE, dataStr, strlen(dataStr) + 1);
        FreeJsonString(dataStr);
        g_testGm->processData(req, (uint8_t *)g_dataBuffer, strlen(g_dataBuffer) + 1);
        g_isNeedContinue = false;
        DelayWithMSec(500);
    }
    EXPECT_EQ(g_messageCode, ON_ERROR);
    EXPECT_EQ(g_operationCode, MEMBER_INVITE);
}

static char *ConstructAddParams13()
{
    CJson *addParams = CreateJson();
    AddStringToJson(addParams, FIELD_GROUP_ID, "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B");
    AddIntToJson(addParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(addParams, FIELD_PIN_CODE, "123456");
    AddBoolToJson(addParams, FIELD_IS_ADMIN, true);
    char *addParamsStr = PackJsonToString(addParams);
    FreeJson(addParams);
    return addParamsStr;
}

TEST_F(ADD_MEMBER_TO_GROUP, TC_DEV_P2P_BIND_13)
{
    SetClient(true);
    g_isNeedContinue = true;
    const char * createParamsStr = "{\"groupType\":256,\"deviceId\":\"3C58C27533D8\",\"userType\":0,\""
                                   "groupVisibility\":-1,\"expireTime\":90,\"groupName\":\"P2PGroup\"}";
    g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    /* delay 1 second */
    DelayWithMSec(500);
    DeviceInfo *devAuthParams = CreateDeviceInfoStruct();
    devAuthParams->devType = DEVICE_TYPE_ACCESSORY;
    StringSetPointer(&(devAuthParams->authId), "CAF34E13190CBA510AA8DABB70CDFF8E9F623656DED400EF0D4CFD9E88FD6202");
    StringSetPointer(&(devAuthParams->udid), "ABCDEF00ABCDEF00ABCDEF00ABCDEF00ABCDEF00ABCDEF00ABCDEF00ABCDEF00");
    StringSetPointer(&(devAuthParams->groupId), "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B");
    StringSetPointer(&(devAuthParams->serviceType), "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B");
    AddTrustedDevice(devAuthParams, NULL);
    DestroyDeviceInfoStruct(devAuthParams);
    char *addParamsStr = ConstructAddParams13();
    g_testGm->addMemberToGroup(CLIENT_REQUEST_ID, TEST_APP_NAME, addParamsStr);
    FreeJsonString(addParamsStr);
    DelayWithMSec(500);
    while (g_isNeedContinue)
    {
        SetClient(!GetClient());
        CJson *data = CreateJsonFromString(g_dataBuffer);
        int64_t req = DEFAULT_REQUEST_ID;
        GetInt64FromJson(data, FIELD_REQUEST_ID, &req);
        if (req == CLIENT_REQUEST_ID) {
            req = SERVER_REQUEST_ID;
        } else {
            req = CLIENT_REQUEST_ID;
        }
        AddInt64StringToJson(data, FIELD_REQUEST_ID, req);
        char *dataStr = PackJsonToString(data);
        FreeJson(data);
        memset_s(g_dataBuffer, BUFFER_SIZE, 0, BUFFER_SIZE);
        memcpy_s(g_dataBuffer, BUFFER_SIZE, dataStr, strlen(dataStr) + 1);
        FreeJsonString(dataStr);
        g_testGm->processData(req, (uint8_t *)g_dataBuffer, strlen(g_dataBuffer) + 1);
        g_isNeedContinue = false;
        DelayWithMSec(500);
    }
    g_testGm->processData(CLIENT_REQUEST_ID, (uint8_t *)g_dataBuffer, strlen(g_dataBuffer) + 1);
    DelayWithMSec(500);
    EXPECT_EQ(g_messageCode, ON_FINISH);
    EXPECT_EQ(g_operationCode, MEMBER_INVITE);
}

TEST_F(REGISTER_LISTENER, TC_LISTENER_01)
{
    DataChangeListener listener;
    listener.onGroupCreated = OnGroupCreated;
    listener.onGroupDeleted = OnGroupDeleted;
    listener.onDeviceBound = OnDeviceBound;
    listener.onDeviceUnBound = OnDeviceUnBound;
    listener.onDeviceNotTrusted = OnDeviceNotTrusted;
    listener.onLastGroupDeleted = OnLastGroupDeleted;
    listener.onTrustedDeviceNumChanged = OnTrustedDeviceNumChanged;
    int ret = g_testGm->regDataChangeListener(TEST_APP_NAME, &listener);
    EXPECT_EQ(ret == HC_SUCCESS, true);
}

TEST_F(REGISTER_LISTENER, TC_LISTENER_02)
{
    DataChangeListener listener;
    listener.onGroupCreated = OnGroupCreated;
    listener.onGroupDeleted = OnGroupDeleted;
    listener.onDeviceBound = OnDeviceBound;
    listener.onDeviceUnBound = OnDeviceUnBound;
    listener.onDeviceNotTrusted = OnDeviceNotTrusted;
    listener.onLastGroupDeleted = OnLastGroupDeleted;
    listener.onTrustedDeviceNumChanged = OnTrustedDeviceNumChanged;
    int ret = g_testGm->regDataChangeListener(TEST_APP_NAME, &listener);
    EXPECT_EQ(ret == HC_SUCCESS, true);
    ret = g_testGm->regDataChangeListener(TEST_APP_NAME, &listener);
    EXPECT_EQ(ret == HC_SUCCESS, true);
}

TEST_F(REGISTER_LISTENER, TC_LISTENER_03)
{
    DataChangeListener listener;
    listener.onGroupCreated = OnGroupCreated;
    listener.onGroupDeleted = OnGroupDeleted;
    listener.onDeviceBound = OnDeviceBound;
    listener.onDeviceUnBound = OnDeviceUnBound;
    listener.onDeviceNotTrusted = OnDeviceNotTrusted;
    listener.onLastGroupDeleted = OnLastGroupDeleted;
    listener.onTrustedDeviceNumChanged = OnTrustedDeviceNumChanged;
    int ret = g_testGm->regDataChangeListener(TEST_APP_NAME, &listener);
    EXPECT_EQ(ret == HC_SUCCESS, true);
    ret = g_testGm->unRegDataChangeListener(TEST_APP_NAME);
    EXPECT_EQ(ret == HC_SUCCESS, true);
}

TEST_F(REGISTER_LISTENER, TC_LISTENER_04)
{
    DataChangeListener listener;
    listener.onGroupCreated = OnGroupCreated;
    listener.onGroupDeleted = OnGroupDeleted;
    listener.onDeviceBound = OnDeviceBound;
    listener.onDeviceUnBound = OnDeviceUnBound;
    listener.onDeviceNotTrusted = OnDeviceNotTrusted;
    listener.onLastGroupDeleted = OnLastGroupDeleted;
    listener.onTrustedDeviceNumChanged = OnTrustedDeviceNumChanged;
    int ret = g_testGm->regDataChangeListener(TEST_APP_NAME, &listener);
    EXPECT_EQ(ret == HC_SUCCESS, true);
    ret = g_testGm->unRegDataChangeListener(TEST_APP_NAME);
    EXPECT_EQ(ret == HC_SUCCESS, true);
    ret = g_testGm->unRegDataChangeListener(TEST_APP_NAME);
    EXPECT_EQ(ret == HC_SUCCESS, true);
}

TEST_F(REGISTER_LISTENER, TC_LISTENER_05)
{
    DataChangeListener listener;
    listener.onGroupCreated = OnGroupCreated;
    listener.onGroupDeleted = OnGroupDeleted;
    listener.onDeviceBound = OnDeviceBound;
    listener.onDeviceUnBound = OnDeviceUnBound;
    listener.onDeviceNotTrusted = OnDeviceNotTrusted;
    listener.onLastGroupDeleted = OnLastGroupDeleted;
    listener.onTrustedDeviceNumChanged = OnTrustedDeviceNumChanged;
    int ret = g_testGm->regDataChangeListener(TEST_APP_NAME, &listener);
    EXPECT_EQ(ret == HC_SUCCESS, true);
    const char * createParamsStr = "{\"groupType\":256,\"deviceId\":\"3C58C27533D8\",\"userType\":0,\""
                                   "groupVisibility\":-1,\"expireTime\":90,\"groupName\":\"P2PGroup\"}";
    g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    g_testCondition.wait(&g_testCondition);
    EXPECT_EQ(g_receivedMessageNum[GROUP_CREATED], 1);
}

TEST_F(REGISTER_LISTENER, TC_LISTENER_06)
{
    DataChangeListener listener;
    listener.onGroupCreated = OnGroupCreated;
    listener.onGroupDeleted = OnGroupDeleted;
    listener.onDeviceBound = OnDeviceBound;
    listener.onDeviceUnBound = OnDeviceUnBound;
    listener.onDeviceNotTrusted = OnDeviceNotTrusted;
    listener.onLastGroupDeleted = OnLastGroupDeleted;
    listener.onTrustedDeviceNumChanged = OnTrustedDeviceNumChanged;
    int ret = g_testGm->regDataChangeListener(TEST_APP_NAME, &listener);
    EXPECT_EQ(ret == HC_SUCCESS, true);
    const char * createParamsStr = "{\"groupType\":256,\"deviceId\":\"3C58C27533D8\",\"userType\":0,\""
                                   "groupVisibility\":-1,\"expireTime\":90,\"groupName\":\"P2PGroup\"}";
    g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    g_testCondition.wait(&g_testCondition);
    const char *deleteParamsStr = R"({"groupId": "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B"})";
    g_testGm->deleteGroup(TEMP_REQUEST_ID, TEST_APP_NAME, deleteParamsStr);
    DelayWithMSec(500);
    EXPECT_EQ(g_receivedMessageNum[DEVICE_UNBOUND], 1);
    EXPECT_EQ(g_receivedMessageNum[LAST_GROUP_DELETED], 1);
    EXPECT_EQ(g_receivedMessageNum[TRUSTED_DEVICE_NUM_CHANGED], 3);
    EXPECT_EQ(g_receivedMessageNum[DEVICE_NOT_TRUSTED], 1);
    EXPECT_EQ(g_receivedMessageNum[GROUP_DELETED], 1);
}

TEST_F(QUERY_INTERFACE, TC_QUERY_01)
{
    const char * createParamsStr =
        "{\"groupType\":256,\"deviceId\":\"3C58C27533D8\",\"userType\":0,\""
        "groupVisibility\":-1,\"expireTime\":90,\"groupName\":\"P2PGroup\"}";
    int ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    DelayWithMSec(500);
    char *returnDataStr = NULL;
    g_testGm->getGroupInfoById(TEST_APP_NAME,
        "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B", &returnDataStr);
    CJson *json = CreateJsonFromString(returnDataStr);
    const char *groupName = GetStringFromJson(json, FIELD_GROUP_NAME);
    ret = strcmp(groupName, "P2PGroup");
    FreeJson(json);
    g_testGm->destroyInfo(&returnDataStr);
    EXPECT_EQ(ret, 0);
}

TEST_F(QUERY_INTERFACE, TC_QUERY_02)
{
    const char * createParamsStr =
        "{\"groupType\":256,\"deviceId\":\"3C58C27533D8\",\"userType\":0,\""
        "groupVisibility\":-1,\"expireTime\":90,\"groupName\":\"P2PGroup\"}";
    int ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    DelayWithMSec(500);
    char *returnDataStr = NULL;
    ret = g_testGm->getGroupInfoById(TEST_APP_NAME, nullptr, &returnDataStr);
    EXPECT_NE(ret, 0);
}

TEST_F(QUERY_INTERFACE, TC_QUERY_03)
{
    const char * createParamsStr =
        "{\"groupType\":256,\"deviceId\":\"3C58C27533D8\",\"userType\":0,\""
        "groupVisibility\":-1,\"expireTime\":90,\"groupName\":\"P2PGroup\"}";
    int ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    DelayWithMSec(500);
    char *returnDataStr = NULL;
    ret = g_testGm->getGroupInfoById(TEST_APP_NAME, "GROUPID", &returnDataStr);
    EXPECT_NE(ret, 0);
}

TEST_F(QUERY_INTERFACE, TC_QUERY_04)
{
    const char * createParamsStr =
        "{\"groupType\":256,\"deviceId\":\"3C58C27533D8\",\"userType\":0,\""
        "groupVisibility\":-1,\"expireTime\":90,\"groupName\":\"P2PGroup\"}";
    int ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    DelayWithMSec(500);
    char *returnDataStr = NULL;
    ret = g_testGm->getGroupInfoById("tesstApp2",
        "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B", &returnDataStr);
    EXPECT_EQ(ret, 0);
}

