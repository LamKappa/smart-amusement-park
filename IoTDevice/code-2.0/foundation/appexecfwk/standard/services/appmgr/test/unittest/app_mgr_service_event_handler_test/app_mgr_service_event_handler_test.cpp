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

#define private public
#include "app_mgr_service_event_handler.h"
#undef private

#include "app_log_wrapper.h"
#include "app_mgr_service_inner.h"
#include <gtest/gtest.h>
#include <memory>
#include "mock_app_scheduler.h"
#include "inner_event.h"

#include <gtest/gtest.h>

using namespace testing::ext;

namespace OHOS {
namespace AppExecFwk {

static bool eventHandlerFlag_ = false;
class MockAMSEventHandler : public AMSEventHandler {

public:
    MockAMSEventHandler(const std::shared_ptr<EventRunner> &runner, const std::shared_ptr<AppMgrServiceInner> &ams);
    virtual ~MockAMSEventHandler();

    virtual void ProcessEvent(const InnerEvent::Pointer &event) override
    {
        if (event->GetInnerEventId() == 10) {
            eventHandlerFlag_ = true;
        }
    }
};

class AMSEventHandlerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

public:
    std::shared_ptr<AppMgrServiceInner> testAms_;
    std::shared_ptr<MockAMSEventHandler> eventHandler_;
    std::shared_ptr<EventRunner> runner_;
};

static void WaitUntilTaskFinished(std::shared_ptr<AMSEventHandler> handler)
{
    if (!handler) {
        return;
    }

    const uint32_t MAX_RETRY_COUNT = 1000;
    const uint32_t SLEEP_TIME = 1000;
    uint32_t count = 0;
    std::atomic<bool> taskCalled(false);
    auto f = [&taskCalled]() { taskCalled.store(true); };
    if (handler->PostTask(f)) {
        while (!taskCalled.load()) {
            ++count;
            // if delay more than 1 second, break
            if (count >= MAX_RETRY_COUNT) {
                break;
            }

            usleep(SLEEP_TIME);
        }
    }
}

void AMSEventHandlerTest::SetUpTestCase()
{}

void AMSEventHandlerTest::TearDownTestCase()
{}

void AMSEventHandlerTest::SetUp()
{
    runner_ = EventRunner::Create("AMSEventHandlerTest");
    testAms_ = std::make_shared<AppMgrServiceInner>();
}

void AMSEventHandlerTest::TearDown()
{}

MockAMSEventHandler ::MockAMSEventHandler(
    const std::shared_ptr<EventRunner> &runner, const std::shared_ptr<AppMgrServiceInner> &ams)
    : AMSEventHandler(runner, ams)
{}

MockAMSEventHandler::~MockAMSEventHandler()
{}

/*
 * Feature: AMS
 * Function: AMSEventHandler
 * SubFunction: AMSEventHandler
 * FunctionPoints: init.
 * EnvConditions: need to start a runner
 * CaseDescription: Initialize message class
 */

HWTEST_F(AMSEventHandlerTest, app_mgr_service_event_handler_test_001, TestSize.Level1)
{
    APP_LOGI("app_mgr_service_event_handler_test start");

    if (!runner_) {
        APP_LOGI("app_mgr_service_event_handler_test : runner_ is null");
    }

    if (!testAms_) {
        APP_LOGI("app_mgr_service_event_handler_test : testAms_ is null");
    }

    EXPECT_FALSE(eventHandler_);

    // init
    eventHandler_ = std::make_shared<MockAMSEventHandler>(runner_, testAms_);

    EXPECT_TRUE(eventHandler_);
    EXPECT_TRUE(eventHandler_->ams_);

    APP_LOGI("app_mgr_service_event_handler_test end");
}

/*
 * Feature: AMS
 * Function: ProcessEvent
 * SubFunction: AMSEventHandler
 * FunctionPoints: postTask.
 * EnvConditions: need to start a runner
 * CaseDescription: Notification message
 */

HWTEST_F(AMSEventHandlerTest, app_mgr_service_event_handler_test_002, TestSize.Level1)
{
    APP_LOGI("app_mgr_service_event_handler_test start");

    if (!eventHandler_) {
        eventHandler_ = std::make_shared<MockAMSEventHandler>(runner_, testAms_);
    }

    // Error testing
    eventHandler_->SendEvent(20);

    // waiting callback
    WaitUntilTaskFinished(eventHandler_);
    EXPECT_FALSE(eventHandlerFlag_);

    // test num == 10
    eventHandler_->SendEvent(10);

    // waiting callback
    WaitUntilTaskFinished(eventHandler_);
    EXPECT_TRUE(eventHandlerFlag_);

    APP_LOGI("app_mgr_service_event_handler_test end");
}

}  // namespace AppExecFwk
}  // namespace OHOS
