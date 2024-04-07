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

#include "data_uri_utils.h"
#include <gtest/gtest.h>
#include "uri.h"

namespace OHOS {
namespace AppExecFwk {
using namespace testing::ext;
using namespace OHOS::AppExecFwk;

class DataUriUtilsTest : public testing::Test {
public:
    DataUriUtilsTest()
    {}
    ~DataUriUtilsTest()
    {}

    std::unique_ptr<DataUriUtils> data_uri_util_ = nullptr;
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void DataUriUtilsTest::SetUpTestCase(void)
{}
void DataUriUtilsTest::TearDownTestCase(void)
{}
void DataUriUtilsTest::SetUp()
{
    data_uri_util_ = std::make_unique<DataUriUtils>();
}

void DataUriUtilsTest::TearDown()
{}

// URI: scheme://authority/path1/path2/path3?id = 1&name = mingming&old#fragment
/**
 * @tc.number: AaFwk_DataUriUtils_AttachId_GetId_0100
 * @tc.name: AttachId/GetId
 * @tc.desc: Test if attachd and getid return values are correct.
 */
HWTEST_F(DataUriUtilsTest, AaFwk_DataUriUtils_AttachId_Get001, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_DataUriUtils_001 start";

    Uri uri("scheme://authority/path1/path2/path3?id = 1&name = mingming&old#fragment");
    Uri uriRet1 = DataUriUtils::AttachId(uri, 1000);

    long ret1 = DataUriUtils::GetId(uriRet1);
    EXPECT_EQ(ret1, 1000);

    GTEST_LOG_(INFO) << "AaFwk_DataUriUtils_001 end";
}

/**
 * @tc.number: AaFwk_DataUriUtils_AttachId_GetId_0100
 * @tc.name: AttachId/DeleteId/IsAttachedId
 * @tc.desc: Test whether the return values of attachid, deleteid and isattachedidare correct. 
 */
HWTEST_F(DataUriUtilsTest, AaFwk_DataUriUtils_DeleteId_IsAttachedId001, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_DataUriUtils_DeleteId_IsAttachedId001 start";

    Uri uri("scheme://authority/path1/path2/path3?id = 1&name = mingming&old#fragment");
    Uri uriRet1 = DataUriUtils::AttachId(uri, 1000);

    Uri uriRet2 = DataUriUtils::DeleteId(uriRet1);

    bool ret2 = DataUriUtils::IsAttachedId(uriRet2);
    EXPECT_EQ(ret2, false);

    GTEST_LOG_(INFO) << "AaFwk_DataUriUtils_DeleteId_IsAttachedId001 end";
}

/**
 * @tc.number: AaFwk_DataUriUtils_AttachIdUpdateId_0100
 * @tc.name: AttachId/UpdateId/GetId
 * @tc.desc: Test whether the return values of attachid, updateid and getid are correct.
 */
HWTEST_F(DataUriUtilsTest, AaFwk_DataUriUtils_AttachIdUpdateId001, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_DataUriUtils_AttachIdUpdateId001 start";

    Uri uri("scheme://authority/path1/path2/path3?id = 1&name = mingming&old#fragment");
    // case 3
    Uri uriRet3 = DataUriUtils::AttachId(uri, 100);
    Uri uriRet4 = DataUriUtils::UpdateId(uriRet3, 800);
    long ret4Id = DataUriUtils::GetId(uriRet4);

    EXPECT_EQ(ret4Id, 800);

    GTEST_LOG_(INFO) << "AaFwk_DataUriUtils_AttachIdUpdateId001 end";
}
}  // namespace AppExecFwk
}  // namespace OHOS