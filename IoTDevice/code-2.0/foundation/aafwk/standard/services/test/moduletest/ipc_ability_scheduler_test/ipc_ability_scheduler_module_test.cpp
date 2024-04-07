/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "uri.h"
#include "want.h"
#include "semaphore_ex.h"
#include "ability_scheduler_stub.h"
#include "ability_scheduler_proxy.h"
#include "mock_ability_scheduler_stub.h"

namespace {
constexpr int COUNT = 1;
}  // namespace

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AAFwk;
using OHOS::iface_cast;
using OHOS::sptr;
using testing::_;
using testing::Invoke;
using testing::InvokeWithoutArgs;

class IpcAbilitySchedulerModuleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void IpcAbilitySchedulerModuleTest::SetUpTestCase()
{}

void IpcAbilitySchedulerModuleTest::TearDownTestCase()
{}

void IpcAbilitySchedulerModuleTest::SetUp()
{}

void IpcAbilitySchedulerModuleTest::TearDown()
{}

/*
 * Feature: AAFwk
 * Function: AbilityScheduler
 * SubFunction: IPC of Aafwk and AppExecFwk
 * FunctionPoints: ScheduleAbilityTransaction
 * EnvConditions: NA
 * CaseDescription: verify ScheduleAbilityTransaction IPC between Aafwk and AppExecFwk.
 */
HWTEST_F(IpcAbilitySchedulerModuleTest, ScheduleAbilityTransaction_001, TestSize.Level3)
{
    Semaphore sem(0);
    struct {
        Want want;
        LifeCycleStateInfo lifeCycleStateInfo;
    } proxyState, stubState;

    proxyState.want.SetAction("test");
    proxyState.lifeCycleStateInfo.isNewWant = true;

    for (int i = 0; i < COUNT; i++) {
        sptr<MockAbilitySchedulerStub> stub(new MockAbilitySchedulerStub());
        sptr<IAbilityScheduler> proxy = iface_cast<IAbilityScheduler>(stub);

        Want::ClearWant(&stubState.want);
        stubState.lifeCycleStateInfo.isNewWant = false;

        auto stubHandler = [&](const Want &want, const LifeCycleStateInfo &lifeCycleStateInfo) {
            stubState.want.SetAction(want.GetAction());
            stubState.lifeCycleStateInfo.isNewWant = lifeCycleStateInfo.isNewWant;
            sem.Post();
        };

        EXPECT_CALL(*stub, ScheduleAbilityTransaction(_, _)).Times(1).WillOnce(Invoke(stubHandler));

        proxy->ScheduleAbilityTransaction(proxyState.want, proxyState.lifeCycleStateInfo);

        sem.Wait();

        EXPECT_EQ(stubState.want.GetAction(), proxyState.want.GetAction());
        EXPECT_EQ(stubState.lifeCycleStateInfo.isNewWant, proxyState.lifeCycleStateInfo.isNewWant);
    }
}

/*
 * Feature: AAFwk
 * Function: AbilityScheduler
 * SubFunction: IPC of Aafwk and AppExecFwk
 * FunctionPoints: SendResult
 * EnvConditions: NA
 * CaseDescription: verify SendResult IPC between Aafwk and AppExecFwk.
 */
HWTEST_F(IpcAbilitySchedulerModuleTest, SendResult_001, TestSize.Level3)
{
    Semaphore sem(0);
    struct {
        int requestCode;
        int resultCode;
        Want resultWant;
    } proxyState, stubState;

    proxyState.requestCode = 123;
    proxyState.resultCode = 456;
    proxyState.resultWant.SetAction("test");

    for (int i = 0; i < COUNT; i++) {
        sptr<MockAbilitySchedulerStub> stub(new MockAbilitySchedulerStub());
        sptr<IAbilityScheduler> proxy = iface_cast<IAbilityScheduler>(stub);

        stubState.requestCode = 0;
        Want::ClearWant(&stubState.resultWant);

        auto stubHandler = [&](int requestCode, int resultCode, const Want &resultWant) {
            stubState.requestCode = requestCode;
            stubState.resultCode = resultCode;
            stubState.resultWant.SetAction(resultWant.GetAction());
            sem.Post();
        };

        EXPECT_CALL(*stub, SendResult(_, _, _)).Times(1).WillOnce(Invoke(stubHandler));

        proxy->SendResult(proxyState.requestCode, proxyState.resultCode, proxyState.resultWant);

        sem.Wait();

        EXPECT_EQ(stubState.requestCode, proxyState.requestCode);
        EXPECT_EQ(stubState.resultCode, proxyState.resultCode);
        EXPECT_EQ(stubState.resultWant.GetAction(), proxyState.resultWant.GetAction());
    }
}

/*
 * Feature: AAFwk
 * Function: AbilityScheduler
 * SubFunction: IPC of Aafwk and AppExecFwk
 * FunctionPoints: ScheduleConnectAbility
 * EnvConditions: NA
 * CaseDescription: verify ScheduleConnectAbility IPC between Aafwk and AppExecFwk.
 */
HWTEST_F(IpcAbilitySchedulerModuleTest, ScheduleConnectAbility_001, TestSize.Level3)
{
    Semaphore sem(0);
    struct {
        Want want;
    } proxyState, stubState;

    proxyState.want.SetAction("test");

    for (int i = 0; i < COUNT; i++) {
        sptr<MockAbilitySchedulerStub> stub(new MockAbilitySchedulerStub());
        sptr<IAbilityScheduler> proxy = iface_cast<IAbilityScheduler>(stub);

        Want::ClearWant(&stubState.want);

        auto stubHandler = [&](const Want &want) {
            stubState.want.SetAction(want.GetAction());
            sem.Post();
        };

        EXPECT_CALL(*stub, ScheduleConnectAbility(_)).Times(1).WillOnce(Invoke(stubHandler));

        proxy->ScheduleConnectAbility(proxyState.want);

        sem.Wait();

        EXPECT_EQ(stubState.want.GetAction(), proxyState.want.GetAction());
    }
}

/*
 * Feature: AAFwk
 * Function: AbilityScheduler
 * SubFunction: IPC of Aafwk and AppExecFwk
 * FunctionPoints: ScheduleDisconnectAbility
 * EnvConditions: NA
 * CaseDescription: verify ScheduleDisconnectAbility IPC between Aafwk and AppExecFwk.
 */
HWTEST_F(IpcAbilitySchedulerModuleTest, ScheduleDisconnectAbility_001, TestSize.Level3)
{
    Semaphore sem(0);
    struct {
        Want want;
    } proxyState, stubState;

    proxyState.want.SetAction("test");

    for (int i = 0; i < COUNT; i++) {
        sptr<MockAbilitySchedulerStub> stub(new MockAbilitySchedulerStub());
        sptr<IAbilityScheduler> proxy = iface_cast<IAbilityScheduler>(stub);

        Want::ClearWant(&stubState.want);
        auto stubHandler = [&](const Want &want) {
            stubState.want.SetAction(want.GetAction());
            sem.Post();
        };
        EXPECT_CALL(*stub, ScheduleDisconnectAbility(_)).Times(1).WillOnce(Invoke(stubHandler));

        proxy->ScheduleDisconnectAbility(proxyState.want);

        sem.Wait();
    }
}

/*
 * Feature: AAFwk
 * Function: AbilityScheduler
 * SubFunction: IPC of Aafwk and AppExecFwk
 * FunctionPoints: ScheduleSaveAbilityState
 * EnvConditions: NA
 * CaseDescription: verify ScheduleSaveAbilityState IPC between Aafwk and AppExecFwk.
 */
HWTEST_F(IpcAbilitySchedulerModuleTest, ScheduleSaveAbilityState_001, TestSize.Level3)
{
    Semaphore sem(0);

    for (int i = 0; i < COUNT; i++) {
        sptr<MockAbilitySchedulerStub> stub(new MockAbilitySchedulerStub());
        sptr<IAbilityScheduler> proxy = iface_cast<IAbilityScheduler>(stub);

        bool testResult = false;
        auto mockHandler = [&](const PacMap &pacMap) {
            testResult = true;
            sem.Post();
        };

        EXPECT_CALL(*stub, ScheduleSaveAbilityState(_)).Times(1).WillOnce(Invoke(mockHandler));

        PacMap pacMap;
        proxy->ScheduleSaveAbilityState(pacMap);

        sem.Wait();

        EXPECT_TRUE(testResult);
    }
}

/*
 * Feature: AAFwk
 * Function: AbilityScheduler
 * SubFunction: IPC of Aafwk and AppExecFwk
 * FunctionPoints: ScheduleRestoreAbilityState
 * EnvConditions: NA
 * CaseDescription: verify ScheduleRestoreAbilityState IPC between Aafwk and AppExecFwk.
 */
HWTEST_F(IpcAbilitySchedulerModuleTest, ScheduleRestoreAbilityState_001, TestSize.Level3)
{
    Semaphore sem(0);

    for (int i = 0; i < COUNT; i++) {
        sptr<MockAbilitySchedulerStub> stub(new MockAbilitySchedulerStub());
        sptr<IAbilityScheduler> proxy = iface_cast<IAbilityScheduler>(stub);

        bool testResult = false;
        auto mockHandler = [&](const PacMap &pacMap) {
            testResult = true;
            sem.Post();
        };

        EXPECT_CALL(*stub, ScheduleRestoreAbilityState(_)).Times(1).WillOnce(Invoke(mockHandler));

        PacMap pacMap;
        proxy->ScheduleRestoreAbilityState(pacMap);

        sem.Wait();

        EXPECT_TRUE(testResult);
    }
}

/*
 * Feature: AAFwk
 * Function: AbilityScheduler
 * SubFunction: IPC of AMS and App
 * FunctionPoints: GetFileType
 * EnvConditions: NA
 * CaseDescription: verify GetFileType IPC between AMS and AppFwk.
 */
HWTEST_F(IpcAbilitySchedulerModuleTest, GetFileType_001, TestSize.Level1)
{
    Semaphore sem(0);

    for (int i = 0; i < COUNT; i++) {
        sptr<MockAbilitySchedulerStub> stub(new MockAbilitySchedulerStub());
        sptr<IAbilityScheduler> proxy = iface_cast<IAbilityScheduler>(stub);

        bool testResult = false;
        std::string testUri("dataability://test/path");
        std::string testMime("text");

        auto mockHandler = [&](const Uri &uri, const std::string &mime) {
            testResult = (mime == testMime);
            std::vector<std::string> ret;
            ret.emplace_back("type1");
            ret.emplace_back("type2");
            sem.Post();
            return ret;
        };

        EXPECT_CALL(*stub, GetFileTypes(_, _)).Times(1).WillOnce(Invoke(mockHandler));

        auto result = proxy->GetFileTypes(Uri(testUri), testMime);

        sem.Wait();

        EXPECT_TRUE(testResult);
        EXPECT_EQ(result.size(), size_t(2));
        EXPECT_EQ(result[0], "type1");
        EXPECT_EQ(result[1], "type2");
    }
}

/*
 * Feature: AAFwk
 * Function: AbilityScheduler
 * SubFunction: IPC of AMS and App
 * FunctionPoints: OpenFile
 * EnvConditions: NA
 * CaseDescription: verify OpenFile IPC between AMS and AppFwk.
 */
HWTEST_F(IpcAbilitySchedulerModuleTest, OpenFile_001, TestSize.Level1)
{
    Semaphore sem(0);

    for (int i = 0; i < COUNT; i++) {
        sptr<MockAbilitySchedulerStub> stub(new MockAbilitySchedulerStub());
        sptr<IAbilityScheduler> proxy = iface_cast<IAbilityScheduler>(stub);

        bool testResult = false;
        std::string testUri("dataability://test/path");
        std::string testMode("RW");
        int testRet = 123;

        auto mockHandler = [&](const Uri &uri, const std::string &mode) {
            testResult = (mode == testMode);
            sem.Post();
            return testRet;
        };

        EXPECT_CALL(*stub, OpenFile(_, _)).Times(1).WillOnce(Invoke(mockHandler));

        auto result = proxy->OpenFile(Uri(testUri), testMode);

        sem.Wait();

        EXPECT_TRUE(testResult);
        EXPECT_EQ(result, testRet);
    }
}

/*
 * Feature: AAFwk
 * Function: AbilityScheduler
 * SubFunction: IPC of AMS and App
 * FunctionPoints: Insert
 * EnvConditions: NA
 * CaseDescription: verify Insert IPC between AMS and AppFwk.
 */
HWTEST_F(IpcAbilitySchedulerModuleTest, Insert_001, TestSize.Level1)
{
    Semaphore sem(0);

    for (int i = 0; i < COUNT; i++) {
        sptr<MockAbilitySchedulerStub> stub(new MockAbilitySchedulerStub());
        sptr<IAbilityScheduler> proxy = iface_cast<IAbilityScheduler>(stub);

        bool testResult = false;
        std::string testUri("dataability://test/path");
        ValuesBucket testValues;
        int testRet = 123;

        auto mockHandler = [&](const Uri &uri, const ValuesBucket &vb) {
            testResult = true;
            sem.Post();
            return testRet;
        };

        EXPECT_CALL(*stub, Insert(_, _)).Times(1).WillOnce(Invoke(mockHandler));

        auto result = proxy->Insert(Uri(testUri), testValues);

        sem.Wait();

        EXPECT_TRUE(testResult);
        EXPECT_EQ(result, testRet);
    }
}

/*
 * Feature: AAFwk
 * Function: AbilityScheduler
 * SubFunction: IPC of AMS and App
 * FunctionPoints: Update
 * EnvConditions: NA
 * CaseDescription: verify Update IPC between AMS and AppFwk.
 */
HWTEST_F(IpcAbilitySchedulerModuleTest, Update_001, TestSize.Level1)
{
    Semaphore sem(0);

    for (int i = 0; i < COUNT; i++) {
        sptr<MockAbilitySchedulerStub> stub(new MockAbilitySchedulerStub());
        sptr<IAbilityScheduler> proxy = iface_cast<IAbilityScheduler>(stub);

        bool testResult = false;
        std::string testUri("dataability://test/path");
        ValuesBucket testValues;
        DataAbilityPredicates testPred;
        int testRet = 123;

        auto mockHandler = [&](const Uri &uri, const ValuesBucket &vb, const DataAbilityPredicates &pred) {
            testResult = true;
            sem.Post();
            return testRet;
        };

        EXPECT_CALL(*stub, Update(_, _, _)).Times(1).WillOnce(Invoke(mockHandler));

        auto result = proxy->Update(Uri(testUri), testValues, testPred);

        sem.Wait();

        EXPECT_TRUE(testResult);
        EXPECT_EQ(result, testRet);
    }
}

/*
 * Feature: AAFwk
 * Function: AbilityScheduler
 * SubFunction: IPC of AMS and App
 * FunctionPoints: Delete
 * EnvConditions: NA
 * CaseDescription: verify Delete IPC between AMS and AppFwk.
 */
HWTEST_F(IpcAbilitySchedulerModuleTest, Delete_001, TestSize.Level1)
{
    Semaphore sem(0);

    for (int i = 0; i < COUNT; i++) {
        sptr<MockAbilitySchedulerStub> stub(new MockAbilitySchedulerStub());
        sptr<IAbilityScheduler> proxy = iface_cast<IAbilityScheduler>(stub);

        bool testResult = false;
        std::string testUri("dataability://test/path");
        DataAbilityPredicates testPred;
        int testRet = 123;

        auto mockHandler = [&](const Uri &uri, const DataAbilityPredicates &pred) {
            testResult = true;
            sem.Post();
            return testRet;
        };

        EXPECT_CALL(*stub, Delete(_, _)).Times(1).WillOnce(Invoke(mockHandler));

        auto result = proxy->Delete(Uri(testUri), testPred);

        sem.Wait();

        EXPECT_TRUE(testResult);
        EXPECT_EQ(result, testRet);
    }
}

/*
 * Feature: AAFwk
 * Function: AbilityScheduler
 * SubFunction: IPC of AMS and App
 * FunctionPoints: Query
 * EnvConditions: NA
 * CaseDescription: verify Query IPC between AMS and AppFwk.
 */
HWTEST_F(IpcAbilitySchedulerModuleTest, Query_001, TestSize.Level1)
{
    Semaphore sem(0);

    for (int i = 0; i < COUNT; i++) {
        sptr<MockAbilitySchedulerStub> stub(new MockAbilitySchedulerStub());
        sptr<IAbilityScheduler> proxy = iface_cast<IAbilityScheduler>(stub);

        bool testResult = false;
        std::string testUri("dataability://test/path");
        std::vector<std::string> testColumns;
        testColumns.emplace_back("col1");
        testColumns.emplace_back("col2");
        DataAbilityPredicates testPred;

        auto mockHandler = [&](const Uri &uri, std::vector<std::string> &columns, const DataAbilityPredicates &pred) {
            testResult = (columns == testColumns);
            sem.Post();
            return std::make_shared<ResultSet>();
        };

        EXPECT_CALL(*stub, Query(_, _, _)).Times(1).WillOnce(Invoke(mockHandler));

        auto result = proxy->Query(Uri(testUri), testColumns, testPred);

        sem.Wait();

        EXPECT_TRUE(testResult);
        EXPECT_TRUE(result);
    }
}

/*
 * Feature: AAFwk
 * Function: AbilityScheduler
 * SubFunction: IPC of Aafwk and AppExecFwk
 * FunctionPoints: ScheduleCommandAbility
 * EnvConditions: NA
 * CaseDescription: verify ScheduleCommandAbility IPC between Aafwk and AppExecFwk.
 */
HWTEST_F(IpcAbilitySchedulerModuleTest, ScheduleCommandAbility_001, TestSize.Level1)
{
    Semaphore sem(0);
    struct {
        Want want;
    } proxyState, stubState;

    proxyState.want.SetAction("test");
    static int count = 0;
    for (int i = 0; i < COUNT; i++) {
        sptr<MockAbilitySchedulerStub> stub(new MockAbilitySchedulerStub());
        sptr<AbilitySchedulerProxy> proxy(new AbilitySchedulerProxy(stub));
        // sptr<IAbilityScheduler> proxy = iface_cast<IAbilityScheduler>(stub);

        Want::ClearWant(&stubState.want);
        auto stubHandler = [&](const Want &want, bool reStart, int startId) {
            stubState.want.SetAction(want.GetAction());
            count++;
            sem.Post();
        };
        EXPECT_CALL(*stub, ScheduleCommandAbility(_, _, _)).Times(1).WillOnce(Invoke(stubHandler));
        proxy->ScheduleCommandAbility(proxyState.want, false, i + 1);
        sem.Wait();
    }

    bool testResult = (count == COUNT);
    EXPECT_TRUE(testResult);
}