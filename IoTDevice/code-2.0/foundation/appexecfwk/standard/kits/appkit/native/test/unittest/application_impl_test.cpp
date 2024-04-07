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

#include "application_impl.h"
#include "mock_application.h"

namespace OHOS {
namespace AppExecFwk {
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

class ApplicationImplTest : public testing::Test {
public:
    ApplicationImplTest() : applicationImpl_(nullptr)
    {}
    ~ApplicationImplTest()
    {
        applicationImpl_ = nullptr;
    }
    ApplicationImpl *applicationImpl_ = nullptr;
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void ApplicationImplTest::SetUpTestCase(void)
{}

void ApplicationImplTest::TearDownTestCase(void)
{}

void ApplicationImplTest::SetUp(void)
{
    applicationImpl_ = new (std::nothrow) ApplicationImpl();
}

void ApplicationImplTest::TearDown(void)
{
    delete applicationImpl_;
    applicationImpl_ = nullptr;
}

/**
 * @tc.number: AppExecFwk_ApplicationImpl_PerformAppReady_0100
 * @tc.name: PerformAppReady
 * @tc.desc: Test whether the performapready return value is false.
 */
HWTEST_F(ApplicationImplTest, AppExecFwk_ApplicationImpl_PerformAppReady_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_ApplicationImpl_PerformAppReady_0100 start";

    EXPECT_NE(applicationImpl_, nullptr);
    if (applicationImpl_ != nullptr) {
        std::shared_ptr<MockApplication> application = std::make_shared<MockApplication>();
        applicationImpl_->SetApplication(application);
        applicationImpl_->SetState(MockApplication::APP_STATE_CREATE);
        bool ret = applicationImpl_->PerformAppReady();
        EXPECT_EQ(MockApplication::APP_STATE_READY, applicationImpl_->GetState());
        EXPECT_EQ(MockApplication::APP_STATE_READY, application->state_);
        EXPECT_EQ(true, ret);
    }
    GTEST_LOG_(INFO) << "AppExecFwk_ApplicationImpl_PerformAppReady_0100 end";
}

/**
 * @tc.number: AppExecFwk_ApplicationImpl_PerformAppReady_0200
 * @tc.name: PerformAppReady
 * @tc.desc: Test whether the performapready return value is false.
 */
HWTEST_F(ApplicationImplTest, AppExecFwk_ApplicationImpl_PerformAppReady_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_ApplicationImpl_PerformAppReady_0200 start";

    EXPECT_NE(applicationImpl_, nullptr);
    if (applicationImpl_ != nullptr) {
        std::shared_ptr<MockApplication> application = std::make_shared<MockApplication>();
        applicationImpl_->SetApplication(application);
        applicationImpl_->SetState(MockApplication::APP_STATE_BACKGROUND);
        bool ret = applicationImpl_->PerformAppReady();
        EXPECT_EQ(false, ret);
    }
    GTEST_LOG_(INFO) << "AppExecFwk_ApplicationImpl_PerformAppReady_0200 end";
}

/**
 * @tc.number: AppExecFwk_ApplicationImpl_PerformForeground_0100
 * @tc.name: PerformForeground
 * @tc.desc: Test whether setapplication and setstate are called normally,
 *           and verify whether the return value of performforegroup is true.
 */
HWTEST_F(ApplicationImplTest, AppExecFwk_ApplicationImpl_PerformForeground_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_ApplicationImpl_PerformForeground_0100 start";

    EXPECT_NE(applicationImpl_, nullptr);
    if (applicationImpl_ != nullptr) {
        std::shared_ptr<MockApplication> application = std::make_shared<MockApplication>();
        applicationImpl_->SetApplication(application);
        applicationImpl_->SetState(MockApplication::APP_STATE_READY);
        bool ret = applicationImpl_->PerformForeground();
        EXPECT_EQ(MockApplication::APP_STATE_FOREGROUND, applicationImpl_->GetState());
        EXPECT_EQ(MockApplication::APP_STATE_FOREGROUND, application->state_);
        EXPECT_EQ(true, ret);
    }
    GTEST_LOG_(INFO) << "AppExecFwk_ApplicationImpl_PerformForeground_0100 end";
}

/**
 * @tc.number: AppExecFwk_ApplicationImpl_PerformForeground_0200
 * @tc.name: PerformForeground
 * @tc.desc: Test whether setapplication and setstate are called normally,
 *           and verify whether the return value of performforegroup is false.
 */
HWTEST_F(ApplicationImplTest, AppExecFwk_ApplicationImpl_PerformForeground_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_ApplicationImpl_PerformForeground_0200 start";

    EXPECT_NE(applicationImpl_, nullptr);
    if (applicationImpl_ != nullptr) {
        std::shared_ptr<MockApplication> application = std::make_shared<MockApplication>();
        applicationImpl_->SetApplication(application);
        applicationImpl_->SetState(MockApplication::APP_STATE_CREATE);
        bool ret = applicationImpl_->PerformForeground();
        EXPECT_EQ(false, ret);
    }
    GTEST_LOG_(INFO) << "AppExecFwk_ApplicationImpl_PerformForeground_0200 end";
}

/**
 * @tc.number: AppExecFwk_ApplicationImpl_PerformForeground_0300
 * @tc.name: PerformForeground
 * @tc.desc: Test whether setapplication and setstate are called normally,
 *           and verify whether the return value of performforegroup is true.
 */
HWTEST_F(ApplicationImplTest, AppExecFwk_ApplicationImpl_PerformForeground_0300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_ApplicationImpl_PerformForeground_0300 start";

    EXPECT_NE(applicationImpl_, nullptr);
    if (applicationImpl_ != nullptr) {
        std::shared_ptr<MockApplication> application = std::make_shared<MockApplication>();
        applicationImpl_->SetApplication(application);
        applicationImpl_->SetState(MockApplication::APP_STATE_BACKGROUND);
        bool ret = applicationImpl_->PerformForeground();
        EXPECT_EQ(MockApplication::APP_STATE_FOREGROUND, applicationImpl_->GetState());
        EXPECT_EQ(MockApplication::APP_STATE_FOREGROUND, application->state_);
        EXPECT_EQ(true, ret);
    }
    GTEST_LOG_(INFO) << "AppExecFwk_ApplicationImpl_PerformForeground_0300 end";
}

/**
 * @tc.number: AppExecFwk_ApplicationImpl_PerformBackground_0100
 * @tc.name: PerformBackground
 * @tc.desc: Test whether setapplication and setstate are called normally,
 *           and verify whether the return value of performbackground is false.
 */
HWTEST_F(ApplicationImplTest, AppExecFwk_ApplicationImpl_PerformBackground_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_ApplicationImpl_PerformBackground_0100 start";

    EXPECT_NE(applicationImpl_, nullptr);
    if (applicationImpl_ != nullptr) {
        std::shared_ptr<MockApplication> application = std::make_shared<MockApplication>();
        applicationImpl_->SetApplication(application);
        applicationImpl_->SetState(MockApplication::APP_STATE_CREATE);
        bool ret = applicationImpl_->PerformBackground();
        EXPECT_EQ(false, ret);
    }
    GTEST_LOG_(INFO) << "AppExecFwk_ApplicationImpl_PerformBackground_0100 end";
}

/**
 * @tc.number: AppExecFwk_ApplicationImpl_PerformBackground_0200
 * @tc.name: PerformBackground
 * @tc.desc: Test whether setapplication and setstate are called normally,
 *           and verify whether the return value of performbackground is true.
 */
HWTEST_F(ApplicationImplTest, AppExecFwk_ApplicationImpl_PerformBackground_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_ApplicationImpl_PerformBackground_0200 start";

    EXPECT_NE(applicationImpl_, nullptr);
    if (applicationImpl_ != nullptr) {
        std::shared_ptr<MockApplication> application = std::make_shared<MockApplication>();
        applicationImpl_->SetApplication(application);
        applicationImpl_->SetState(MockApplication::APP_STATE_FOREGROUND);
        bool ret = applicationImpl_->PerformBackground();
        EXPECT_EQ(MockApplication::APP_STATE_BACKGROUND, applicationImpl_->GetState());
        EXPECT_EQ(MockApplication::APP_STATE_BACKGROUND, application->state_);
        EXPECT_EQ(true, ret);
    }
    GTEST_LOG_(INFO) << "AppExecFwk_ApplicationImpl_PerformBackground_0200 end";
}

/**
 * @tc.number: AppExecFwk_ApplicationImpl_PerformTerminate_0100
 * @tc.name: PerformTerminate
 * @tc.desc: Test whether setapplication and setstate are called normally,
 *           and verify whether the return value of performterminate is false.
 */
HWTEST_F(ApplicationImplTest, AppExecFwk_ApplicationImpl_PerformTerminate_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_ApplicationImpl_PerformTerminate_0100 start";
    EXPECT_NE(applicationImpl_, nullptr);
    if (applicationImpl_ != nullptr) {
        std::shared_ptr<MockApplication> application = std::make_shared<MockApplication>();
        applicationImpl_->SetApplication(application);
        applicationImpl_->SetState(MockApplication::APP_STATE_CREATE);
        bool ret = applicationImpl_->PerformTerminate();
        EXPECT_EQ(false, ret);
    }
    GTEST_LOG_(INFO) << "AppExecFwk_ApplicationImpl_PerformTerminate_0100 end";
}

/**
 * @tc.number: AppExecFwk_ApplicationImpl_PerformTerminate_0200
 * @tc.name: PerformTerminate
 * @tc.desc: Test whether setapplication and setstate are called normally,
 *           and verify whether the return value of performterminate is true.
 */
HWTEST_F(ApplicationImplTest, AppExecFwk_ApplicationImpl_PerformTerminate_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_ApplicationImpl_PerformTerminate_0200 start";
    EXPECT_NE(applicationImpl_, nullptr);
    if (applicationImpl_ != nullptr) {
        std::shared_ptr<MockApplication> application = std::make_shared<MockApplication>();
        applicationImpl_->SetApplication(application);
        applicationImpl_->SetState(MockApplication::APP_STATE_BACKGROUND);
        bool ret = applicationImpl_->PerformTerminate();
        EXPECT_EQ(MockApplication::APP_STATE_TERMINATED, applicationImpl_->GetState());
        EXPECT_EQ(MockApplication::APP_STATE_TERMINATED, application->state_);
        EXPECT_EQ(true, ret);
    }
    GTEST_LOG_(INFO) << "AppExecFwk_ApplicationImpl_PerformTerminate_0200 end";
}

/**
 * @tc.number: AppExecFwk_ApplicationImpl_PerformMemoryLevel_0100
 * @tc.name: PerformMemoryLevel
 * @tc.desc: Test whether setapplication and performmemorylevel are called normally.
 */
HWTEST_F(ApplicationImplTest, AppExecFwk_ApplicationImpl_PerformMemoryLevel_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_ApplicationImpl_PerformMemoryLevel_0100 start";
    EXPECT_NE(applicationImpl_, nullptr);
    if (applicationImpl_ != nullptr) {
        std::shared_ptr<MockApplication> application = std::make_shared<MockApplication>();
        applicationImpl_->SetApplication(application);
        applicationImpl_->PerformMemoryLevel(1);
        EXPECT_EQ(true, application->onMemoryLevelCalled_);
    }
    GTEST_LOG_(INFO) << "AppExecFwk_ApplicationImpl_PerformMemoryLevel_0100 end";
}

/**
 * @tc.number: AppExecFwk_ApplicationImpl_PerformConfigurationUpdated_0100
 * @tc.name: PerformConfigurationUpdated
 * @tc.desc: Test whether setapplication and performmemorylevel are called normally.
 */
HWTEST_F(
    ApplicationImplTest, AppExecFwk_ApplicationImpl_PerformConfigurationUpdated_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_ApplicationImpl_PerformConfigurationUpdated_0100 start";
    EXPECT_NE(applicationImpl_, nullptr);
    if (applicationImpl_ != nullptr) {
        std::shared_ptr<MockApplication> application = std::make_shared<MockApplication>();
        applicationImpl_->SetApplication(application);
        Configuration config;
        applicationImpl_->PerformConfigurationUpdated(config);
        EXPECT_EQ(true, application->onConfigurationUpdatedCalled_);
    }
    GTEST_LOG_(INFO) << "AppExecFwk_ApplicationImpl_PerformConfigurationUpdated_0100 end";
}

/**
 * @tc.number: AppExecFwk_ApplicationImpl_GetRecordId_0100
 * @tc.name: GetRecordId
 * @tc.desc: Test whether setrecordid is called normally, and verify whether the return value of getrecordid is correct.
 */
HWTEST_F(ApplicationImplTest, AppExecFwk_ApplicationImpl_GetRecordId_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_ApplicationImpl_GetRecordId_0100 start";
    EXPECT_NE(applicationImpl_, nullptr);
    if (applicationImpl_ != nullptr) {
        applicationImpl_->SetRecordId(1);
        EXPECT_EQ(1, applicationImpl_->GetRecordId());
    }
    GTEST_LOG_(INFO) << "AppExecFwk_ApplicationImpl_GetRecordId_0100 end";
}
}  // namespace AppExecFwk
}  // namespace OHOS