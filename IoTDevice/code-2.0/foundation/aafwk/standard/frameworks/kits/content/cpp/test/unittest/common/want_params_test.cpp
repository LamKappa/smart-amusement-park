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

#include "ohos/aafwk/base/string_wrapper.h"
#include "ohos/aafwk/base/bool_wrapper.h"
#include "ohos/aafwk/base/int_wrapper.h"
#include "ohos/aafwk/base/long_wrapper.h"

#include "ohos/aafwk/content/want_params.h"

using namespace testing::ext;
using namespace OHOS::AAFwk;
using OHOS::Parcel;

namespace OHOS {
namespace AAFwk {
class WantParamsBaseTest : public testing::Test {
public:
    WantParamsBaseTest() 
    {}
    ~WantParamsBaseTest()
    {
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    std::shared_ptr<WantParams> wantParamsIn_ = nullptr;
    std::shared_ptr<WantParams> wantParamsOut_ = nullptr;
};

void WantParamsBaseTest::SetUpTestCase(void)
{}

void WantParamsBaseTest::TearDownTestCase(void)
{}

void WantParamsBaseTest::SetUp(void)
{
    wantParamsIn_ = std::make_shared<WantParams>();
    wantParamsOut_ = std::make_shared<WantParams>();
}

void WantParamsBaseTest::TearDown(void)
{
}

/**
 * @tc.number: AaFwk_WantParams_Parcelable_0100
 * @tc.name: Marshalling/Unmarshalling
 * @tc.desc: marshalling WantParams, and then check result.
 */
HWTEST_F(WantParamsBaseTest, AaFwk_WantParams_Parcelable_0100, Function | MediumTest | Level1)
{
    std::string keyStr = "12345667";
    std::string valueStr = "sdasdfdsffdgfdg";
    wantParamsIn_->SetParam(keyStr, String::Box(valueStr));

    Parcel in;
    if(wantParamsOut_ != nullptr){
        wantParamsIn_->Marshalling(in);
        std::shared_ptr<WantParams> wantParamsOut_(WantParams::Unmarshalling(in));
        EXPECT_EQ(valueStr, String::Unbox(IString::Query(wantParamsOut_->GetParam(keyStr))));
    }  
}

/**
 * @tc.number: AaFwk_WantParams_Parcelable_0200
 * @tc.name: Marshalling/Unmarshalling
 * @tc.desc: marshalling WantParams, and then check result.
 */
HWTEST_F(WantParamsBaseTest, AaFwk_WantParams_Parcelable_0200, Function | MediumTest | Level1)
{
    std::string keyStr = "12345667";
    bool valueBool = true;
    wantParamsIn_->SetParam(keyStr, Boolean::Box(valueBool));

    Parcel in;
    if(wantParamsOut_ != nullptr){
        wantParamsIn_->Marshalling(in);
        std::shared_ptr<WantParams> wantParamsOut_(WantParams::Unmarshalling(in));
        EXPECT_EQ(valueBool, Boolean::Unbox(IBoolean::Query(wantParamsOut_->GetParam(keyStr))));
    }  
}

/**
 * @tc.number: AaFwk_WantParams_Parcelable_0300
 * @tc.name: Marshalling/Unmarshalling
 * @tc.desc: marshalling WantParams, and then check result.
 */
HWTEST_F(WantParamsBaseTest, AaFwk_WantParams_Parcelable_0300, Function | MediumTest | Level1)
{
    std::string keyStr = "12345667";
    int valueInteger = 12345;
    wantParamsIn_->SetParam(keyStr, Integer::Box(valueInteger));
    int right = Integer::Unbox(IInteger::Query(wantParamsIn_->GetParam(keyStr)));

    Parcel in;
    wantParamsIn_->Marshalling(in);
    std::shared_ptr<WantParams> wantParamsOut_(WantParams::Unmarshalling(in));
    if (wantParamsOut_ != nullptr) {
        right = Integer::Unbox(IInteger::Query(wantParamsOut_->GetParam(keyStr)));
        EXPECT_EQ(valueInteger, right);

        wantParamsOut_ = nullptr;     
    }
}

/**
 * @tc.number: AaFwk_WantParams_Parcelable_0400
 * @tc.name: Marshalling/Unmarshalling
 * @tc.desc: marshalling WantParams, and then check result.
 */
HWTEST_F(WantParamsBaseTest, AaFwk_WantParams_Parcelable_0400, Function | MediumTest | Level1)
{
    std::string keyStr = "12345667";
    long valueLong = 1234567;
    wantParamsIn_->SetParam(keyStr, Long::Box(valueLong));

    Parcel in;
    wantParamsIn_->Marshalling(in);
    std::shared_ptr<WantParams> wantParamsOut_(WantParams::Unmarshalling(in));
    EXPECT_EQ(valueLong, Long::Unbox(ILong::Query(wantParamsOut_->GetParam(keyStr))));
}
}
}