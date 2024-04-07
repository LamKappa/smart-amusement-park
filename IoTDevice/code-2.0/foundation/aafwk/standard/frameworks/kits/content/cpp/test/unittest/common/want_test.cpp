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

#include "ohos/aafwk/content/want.h"
#include "ohos/aafwk/base/string_wrapper.h"
#include "ohos/aafwk/base/bool_wrapper.h"
#include "ohos/aafwk/base/int_wrapper.h"
#include "ohos/aafwk/base/float_wrapper.h"
#include "ohos/aafwk/base/array_wrapper.h"
#include "ohos/aafwk/base/long_wrapper.h"

using namespace testing::ext;
using namespace OHOS::AAFwk;
using namespace OHOS;
using OHOS::Parcel;
using OHOS::AppExecFwk::ElementName;

namespace OHOS {
namespace AAFwk {
class WantBaseTest : public testing::Test {
public:
    WantBaseTest()
    {}
    ~WantBaseTest()
    {}
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    std::shared_ptr<Want> want_ = nullptr;

    void CompareWant(const std::shared_ptr<Want> &want1, const std::shared_ptr<Want> &want2) const;
    bool CompareWant(const std::shared_ptr<Want> &want1, const std::shared_ptr<Want> &want2,
        std::map<std::string, std::string> &keys) const;
    void SendParcelTest(const std::shared_ptr<Want> &want, std::map<std::string, std::string> &keys) const;
    void AddBoolParams(Want &want, std::map<std::string, std::string> &keys, int loop, unsigned int flag) const;
    void AddByteParams(Want &want, std::map<std::string, std::string> &keys, int loop, unsigned int flag) const;
    void AddCharParams(Want &want, std::map<std::string, std::string> &keys, int loop, unsigned int flag) const;
    void AddShortParams(Want &want, std::map<std::string, std::string> &keys, int loop, unsigned int flag) const;
    void AddIntParams(Want &want, std::map<std::string, std::string> &keys, int loop, unsigned int flag) const;
    void AddLongParams(Want &want, std::map<std::string, std::string> &keys, int loop, unsigned int flag) const;
    void AddFloatParams(Want &want, std::map<std::string, std::string> &keys, int loop, unsigned int flag) const;
    void AddDoubleParams(Want &want, std::map<std::string, std::string> &keys, int loop, unsigned int flag) const;
    void AddStringParams(Want &want, std::map<std::string, std::string> &keys, int loop, unsigned int flag) const;

    std::string boolType = "bool";
    std::string boolArrayType = "boolArray";
    std::string byteType = "byte";
    std::string byteArrayType = "byteArray";
    std::string charType = "char";
    std::string charArrayType = "charArray";
    std::string shortType = "short";
    std::string shortArrayType = "shortArray";
    std::string intType = "int";
    std::string intArrayType = "intArray";
    std::string longType = "long";
    std::string longArrayType = "longArray";
    std::string floatType = "float";
    std::string floatArrayType = "floatArray";
    std::string doubleType = "double";
    std::string doubleArrayType = "doubleArray";
    std::string stringType = "string";
    std::string stringArrayType = "stringArray";

    static const std::string URI_STRING_HEAD;
    static const std::string URI_STRING_END;
};

template <typename T>
bool CompareArrayData(const std::vector<T> &arr1, const std::vector<T> &arr2)
{
    if (arr1.size() != arr2.size()) {
        return false;
    }

    for (std::uint32_t i = 0; i < arr1.size(); i++) {
        if (arr1[i] != arr2[i]) {
            return false;
        }
    }

    return true;
};

enum type { FLAG_TEST_SINGLE = 0x01, FLAG_TEST_ARRAY, FLAG_TEST_BOTH };

void WantBaseTest::SetUpTestCase(void)
{}

void WantBaseTest::TearDownTestCase(void)
{}

void WantBaseTest::SetUp(void)
{
    want_ = std::make_shared<Want>();
}

void WantBaseTest::TearDown(void)
{}

const std::string WantBaseTest::URI_STRING_HEAD("#Want;");
const std::string WantBaseTest::URI_STRING_END(";end");

/**
 * @tc.number: AaExecFwk_Want_Type_0100
 * @tc.name: SetType/GetType
 * @tc.desc: Validate when normally entering a string
 */
HWTEST_F(WantBaseTest, AaExecFwk_Want_Type_0100, Function | MediumTest | Level1)
{
    if (want_ != nullptr) {
        std::string description = "liuuy";
        want_->SetType(description);
        EXPECT_EQ(description, want_->GetType());
    }
}

/**
 * @tc.number: AaFwk_Want_Action_0100
 * @tc.name: SetAction/GetAction
 * @tc.desc: Validate when normally entering a string
 */
HWTEST_F(WantBaseTest, AaFwk_Want_Action_0100, Function | MediumTest | Level1)
{
    if (want_ != nullptr) {
        std::string actiondescription = "liuuy";
        want_->SetAction(actiondescription);
        EXPECT_EQ(actiondescription, want_->GetAction());
    }
}

/**
 * @tc.number: AaFwk_Want_Bundle_0100
 * @tc.name: SetBundle/GetBundle
 * @tc.desc: Validate when normally entering a string
 */
HWTEST_F(WantBaseTest, AaFwk_Want_Bundle_0100, Function | MediumTest | Level1)
{
    if (want_ != nullptr) {
        std::string bundleName = "liuuy";
        want_->SetBundle(bundleName);
        EXPECT_EQ(bundleName, want_->GetBundle());
    }
}

/**
 * @tc.number: AaFwk_Want_Parcelable_0100
 * @tc.name: Marshalling/Unmarshalling
 * @tc.desc: marshalling Want, and then check result.
 */
HWTEST_F(WantBaseTest, AaFwk_Want_Parcelable_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<Want> WantIn_ = std::make_shared<Want>();
    if (WantIn_ == nullptr) {
        return;
    }

    WantIn_->SetAction("12345");
    WantIn_->SetFlags(123);

    WantIn_->SetAction("12345");
    WantIn_->SetFlags(123);
    WantIn_->AddEntity("12345");
    WantParams wantParams;
    std::string keyStr = "12345667";
    bool valueBool = true;
    wantParams.SetParam(keyStr, Boolean::Box(valueBool));
    WantIn_->SetParams(wantParams);
    OHOS::AppExecFwk::ElementName element;
    element.SetAbilityName("12345");
    element.SetBundleName("12345");
    element.SetDeviceID("12345");
    WantIn_->SetElement(element);
    WantIn_->SetType("12345");
    size_t pos1;
    size_t pos2;
    bool result = false;

    Parcel in;
    pos1 = in.GetWritePosition();
    result = WantIn_->Marshalling(in);
    pos2 = in.GetWritePosition();
    EXPECT_EQ(result, true);
    GTEST_LOG_(INFO) << " Marshalling: pos1: " << pos1 << ", pos2: " << pos2 << ", result: " << result;

    pos1 = in.GetWritePosition();
    result = WantIn_->Marshalling(in);
    pos2 = in.GetWritePosition();
    EXPECT_EQ(result, true);
    GTEST_LOG_(INFO) << " Marshalling: pos1: " << pos1 << ", pos2: " << pos2 << ", result: " << result;

    pos1 = in.GetReadPosition();
    std::shared_ptr<Want> WantOut_(Want::Unmarshalling(in));
    if (WantOut_ != nullptr) {
        pos2 = in.GetReadPosition();
        CompareWant(WantIn_, WantOut_);
        EXPECT_EQ(valueBool, Boolean::Unbox(IBoolean::Query(WantOut_->GetParams().GetParam(keyStr))));
        GTEST_LOG_(INFO) << " Unmarshalling: pos1: " << pos1 << ", pos2: " << pos2 << ", result: " << result;
    }

    pos1 = in.GetReadPosition();
    std::shared_ptr<Want> WantOut2_(Want::Unmarshalling(in));
    if (WantOut2_ != nullptr) {
        pos2 = in.GetReadPosition();
        CompareWant(WantIn_, WantOut2_);
        EXPECT_EQ(valueBool, Boolean::Unbox(IBoolean::Query(WantOut2_->GetParams().GetParam(keyStr))));
        GTEST_LOG_(INFO) << " Unmarshalling: pos1: " << pos1 << ", pos2: " << pos2 << ", result: " << result;
    }
}
/**
 * @tc.number: AaFwk_Want_Parcelable_0200
 * @tc.name: Marshalling/Unmarshalling
 * @tc.desc: marshalling Want, and then check result.
 */
HWTEST_F(WantBaseTest, AaFwk_Want_Parcelable_0200, Function | MediumTest | Level1)
{
    std::shared_ptr<Want> WantIn_ = std::make_shared<Want>();
    if (WantIn_ == nullptr) {
        return;
    }

    WantIn_->SetAction("@#￥#3243adsafdf_中文");
    WantIn_->SetFlags(123);
    WantIn_->AddEntity("@#￥#3243adsafdf_中文");
    WantParams wantParams;
    std::string keyStr = "@#￥#3243adsafdf_中文";
    long valueLong = 123;
    wantParams.SetParam(keyStr, Long::Box(valueLong));
    WantIn_->SetParams(wantParams);
    OHOS::AppExecFwk::ElementName element;
    element.SetAbilityName("@#￥#3243adsafdf_中文");
    element.SetBundleName("@#￥#3243adsafdf_中文");
    element.SetDeviceID("@#￥#3243adsafdf_中文");
    WantIn_->SetElement(element);
    WantIn_->SetType("@#￥#3243adsafdf_中文");

    size_t pos1;
    size_t pos2;
    bool result = false;

    Parcel in;
    pos1 = in.GetWritePosition();
    result = WantIn_->Marshalling(in);
    pos2 = in.GetWritePosition();
    EXPECT_EQ(result, true);
    GTEST_LOG_(INFO) << "Marshalling: pos1: " << pos1 << ", pos2: " << pos2 << ", result: " << result;

    pos1 = in.GetWritePosition();
    result = WantIn_->Marshalling(in);
    pos2 = in.GetWritePosition();
    EXPECT_EQ(result, true);
    GTEST_LOG_(INFO) << "Marshalling: pos1: " << pos1 << ", pos2: " << pos2 << ", result: " << result;

    pos1 = in.GetReadPosition();

    std::shared_ptr<Want> WantOut_(Want::Unmarshalling(in));
    if (WantOut_ != nullptr) {
        pos2 = in.GetReadPosition();
        CompareWant(WantIn_, WantOut_);
        EXPECT_EQ(valueLong, Long::Unbox(ILong::Query(WantOut_->GetParams().GetParam(keyStr))));
        GTEST_LOG_(INFO) << "Unmarshalling: pos1: " << pos1 << ", pos2: " << pos2 << ", result: " << result;
    }

    pos1 = in.GetReadPosition();
    std::shared_ptr<Want> WantOut2_(Want::Unmarshalling(in));
    if (WantOut2_ != nullptr) {
        pos2 = in.GetReadPosition();
        CompareWant(WantIn_, WantOut2_);
        EXPECT_EQ(valueLong, Long::Unbox(ILong::Query(WantOut2_->GetParams().GetParam(keyStr))));
        GTEST_LOG_(INFO) << "Unmarshalling: pos1: " << pos1 << ", pos2: " << pos2 << ", result: " << result;
    }
}

/**
 * @tc.number: AaFwk_Want_Parcelable_0300
 * @tc.name: Marshalling/Unmarshalling
 * @tc.desc: marshalling Want, and then check result.
 */
HWTEST_F(WantBaseTest, AaFwk_Want_Parcelable_0300, Function | MediumTest | Level1)
{
    std::shared_ptr<Want> WantIn_ = std::make_shared<Want>();
    if (WantIn_ == nullptr) {
        return;
    }

    WantIn_->SetAction("");
    WantIn_->SetFlags(123);
    WantIn_->AddEntity("");
    WantParams wantParams;
    std::string keyStr = "";
    int valueInt = 123;
    wantParams.SetParam(keyStr, Integer::Box(valueInt));
    WantIn_->SetParams(wantParams);
    OHOS::AppExecFwk::ElementName element;
    element.SetAbilityName("");
    element.SetBundleName("");
    element.SetDeviceID("");
    WantIn_->SetElement(element);
    WantIn_->SetType("");

    size_t pos1;
    size_t pos2;
    bool result = false;

    Parcel in;
    pos1 = in.GetWritePosition();
    result = WantIn_->Marshalling(in);
    pos2 = in.GetWritePosition();
    EXPECT_EQ(result, true);
    GTEST_LOG_(INFO) << "Marshalling: pos1: " << pos1 << ", pos2: " << pos2 << ", result: " << result;

    pos1 = in.GetWritePosition();
    result = WantIn_->Marshalling(in);
    pos2 = in.GetWritePosition();
    EXPECT_EQ(result, true);
    GTEST_LOG_(INFO) << "Marshalling: pos1: " << pos1 << ", pos2: " << pos2 << ", result: " << result;

    pos1 = in.GetReadPosition();
    std::shared_ptr<Want> WantOut_(Want::Unmarshalling(in));
    if (WantOut_ != nullptr) {
        pos2 = in.GetReadPosition();
        CompareWant(WantIn_, WantOut_);
        EXPECT_EQ(valueInt, Integer::Unbox(IInteger::Query(WantOut_->GetParams().GetParam(keyStr))));
        GTEST_LOG_(INFO) << "Unmarshalling: pos1: " << pos1 << ", pos2: " << pos2 << ", result: " << result;
    }

    pos1 = in.GetReadPosition();
    std::shared_ptr<Want> WantOut2_(Want::Unmarshalling(in));
    if (WantOut2_ != nullptr) {
        pos2 = in.GetReadPosition();
        CompareWant(WantIn_, WantOut2_);
        EXPECT_EQ(valueInt, Integer::Unbox(IInteger::Query(WantOut2_->GetParams().GetParam(keyStr))));
        GTEST_LOG_(INFO) << "Unmarshalling: pos1: " << pos1 << ", pos2: " << pos2 << ", result: " << result;
    }
}

/**
 * @tc.number: AaFwk_Want_Parcelable_0400
 * @tc.name: Marshalling/Unmarshalling
 * @tc.desc: marshalling Want, and then check result.
 */
HWTEST_F(WantBaseTest, AaFwk_Want_Parcelable_0400, Function | MediumTest | Level1)
{
    std::shared_ptr<Want> WantIn_ = std::make_shared<Want>();
    if (WantIn_ == nullptr) {
        return;
    }

    WantIn_->SetAction("12345");
    WantIn_->SetFlags(123);
    WantIn_->AddEntity("12345");
    WantIn_->AddEntity("@#￥#3243adsafdf_中文");
    WantIn_->AddEntity("");
    WantParams wantParams;
    std::string keyStr = "12345667";
    std::string valueString = "123";
    wantParams.SetParam(keyStr, String::Box(valueString));
    WantIn_->SetParams(wantParams);
    OHOS::AppExecFwk::ElementName element;
    element.SetAbilityName("12345");
    element.SetBundleName("12345");
    element.SetDeviceID("12345");
    WantIn_->SetElement(element);
    WantIn_->SetType("12345");

    size_t pos1;
    size_t pos2;
    bool result = false;

    Parcel in;
    pos1 = in.GetWritePosition();
    result = WantIn_->Marshalling(in);
    pos2 = in.GetWritePosition();
    EXPECT_EQ(result, true);
    GTEST_LOG_(INFO) << "Marshalling: pos1: " << pos1 << ", pos2: " << pos2 << ", result: " << result;

    pos1 = in.GetWritePosition();
    result = WantIn_->Marshalling(in);
    pos2 = in.GetWritePosition();
    EXPECT_EQ(result, true);
    GTEST_LOG_(INFO) << "Marshalling: pos1: " << pos1 << ", pos2: " << pos2 << ", result: " << result;

    pos1 = in.GetReadPosition();
    std::shared_ptr<Want> WantOut_(Want::Unmarshalling(in));
    if (WantOut_ != nullptr) {
        pos2 = in.GetReadPosition();
        CompareWant(WantIn_, WantOut_);
        EXPECT_EQ(valueString, String::Unbox(IString::Query(WantOut_->GetParams().GetParam(keyStr))));
        GTEST_LOG_(INFO) << "Unmarshalling: pos1: " << pos1 << ", pos2: " << pos2 << ", result: " << result;
    }

    pos1 = in.GetReadPosition();
    std::shared_ptr<Want> WantOut2_(Want::Unmarshalling(in));
    if (WantOut2_ != nullptr) {
        pos2 = in.GetReadPosition();
        CompareWant(WantIn_, WantOut2_);
        EXPECT_EQ(valueString, String::Unbox(IString::Query(WantOut2_->GetParams().GetParam(keyStr))));
        GTEST_LOG_(INFO) << "Unmarshalling: pos1: " << pos1 << ", pos2: " << pos2 << ", result: " << result;
    }
}

/**
 * @tc.number: AaFwk_Want_Parcelable_0500
 * @tc.name: Marshalling/Unmarshalling
 * @tc.desc: marshalling Want, and then check result.
 */
HWTEST_F(WantBaseTest, AaFwk_Want_Parcelable_0500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Want_Parcelable_005 start";
    std::shared_ptr<Want> WantIn_ = std::make_shared<Want>();
    if (WantIn_ == nullptr) {
        return;
    }

    WantIn_->SetAction("system.test.action");
    WantIn_->SetFlags(64);
    WantIn_->AddEntity("system.test.entity");

    OHOS::AppExecFwk::ElementName element;
    element.SetAbilityName("system.test.abilityname");
    element.SetBundleName("system.test.bundlename");
    element.SetDeviceID("system.test.deviceid");
    WantIn_->SetElement(element);

    WantParams wantParams;

    std::string keyStr = "system.test.wantparams.key";
    std::string MIMEKEY = "mime-type";
    wantParams.SetParam(MIMEKEY, String::Box("system.test.uritype"));

    std::string valueString = "system.wantparams.value.content.test";
    wantParams.SetParam(keyStr, String::Box(valueString));
    WantIn_->SetParams(wantParams);

    // want SetParam  arraydata  test
    std::vector<bool> boolArrayValue = {true, false, true};
    WantIn_->SetParam(std::string("bool_arraykey"), boolArrayValue);

    std::vector<byte> byteArrayValue = {'?', 'a', '\\'};
    WantIn_->SetParam(std::string("byte_arraykey"), byteArrayValue);

    std::vector<zchar> charArrayValue = {U'e', U'l', U'l', U'o'};
    WantIn_->SetParam(std::string("char_arraykey"), charArrayValue);

    std::vector<short> shortArrayValue = {-1, 0, 1};
    WantIn_->SetParam(std::string("short_arraykey"), shortArrayValue);

    std::vector<int> intArrayValue = {-10, 0, 10};
    WantIn_->SetParam(std::string("int_arraykey"), intArrayValue);

    std::vector<long> longArrayValue = {-100, 0, 100};
    WantIn_->SetParam(std::string("long_arraykey"), longArrayValue);

    std::vector<float> floatArrayValue = {-100.1, 0.1, 100.1};
    WantIn_->SetParam(std::string("float_arraykey"), floatArrayValue);

    std::vector<double> doubleArrayValue = {-1000.1, 0.1, 1000.1};
    WantIn_->SetParam(std::string("double_arraykey"), doubleArrayValue);

    std::vector<std::string> stringArrayValue = {"stringtest1", "string@test2", "string@!#test2"};
    WantIn_->SetParam(std::string("string_arraykey"), stringArrayValue);

    Parcel in;
    bool result = false;
    result = WantIn_->Marshalling(in);
    EXPECT_EQ(result, true);
    std::shared_ptr<Want> WantOut_(Want::Unmarshalling(in));
    if (WantOut_ != nullptr) {
        GTEST_LOG_(INFO) << "WantOut_->GetAction().c_str(): " << WantOut_->GetAction().c_str();
        EXPECT_STREQ(WantOut_->GetAction().c_str(), std::string("system.test.action").c_str());

        int flags = WantOut_->GetFlags();
        GTEST_LOG_(INFO) << "WantOut_->GetFlags(): " << flags;
        EXPECT_EQ(((int)(flags)), 64);

        bool hasentity = WantOut_->HasEntity("system.test.entity");
        GTEST_LOG_(INFO) << "WantOut_->HasEntity(system.test.entity)" << hasentity;
        EXPECT_EQ(hasentity, true);

        WantOut_->RemoveEntity(std::string("system.test.entity"));
        hasentity = WantOut_->HasEntity(std::string("system.test.entity"));
        GTEST_LOG_(INFO) << "WantOut_->RemoveEntity(system.test.entity)" << hasentity;
        EXPECT_EQ(hasentity, false);

        std::string outtype = WantOut_->GetType();
        GTEST_LOG_(INFO) << "WantOut_->GetType()" << outtype.c_str();
        EXPECT_STREQ(outtype.c_str(), std::string("system.test.uritype").c_str());

        element = WantOut_->GetElement();
        GTEST_LOG_(INFO) << "element.GetAbilityName().c_str(): " << element.GetAbilityName().c_str();
        EXPECT_STREQ(element.GetAbilityName().c_str(), std::string("system.test.abilityname").c_str());

        GTEST_LOG_(INFO) << "element->GetBundleName().c_str(): " << element.GetBundleName().c_str();
        EXPECT_STREQ(element.GetBundleName().c_str(), std::string("system.test.bundlename").c_str());

        GTEST_LOG_(INFO) << "element.GetDeviceID().c_str(): " << element.GetDeviceID().c_str();
        EXPECT_STREQ(element.GetDeviceID().c_str(), std::string("system.test.deviceid").c_str());

        std::string param_content = WantOut_->GetStringParam(keyStr);
        GTEST_LOG_(INFO) << "WantOut_->GetStringParam(keyStr): " << param_content.c_str();

        // want SetParam  arraydata test
        std::vector<bool> retboolArray;  // boolArrayValue = {true, false, true};
        retboolArray = WantOut_->GetBoolArrayParam(std::string("bool_arraykey"));

        bool arraycompare = CompareArrayData<bool>(retboolArray, boolArrayValue);
        EXPECT_EQ(arraycompare, true);

        std::vector<byte> retbyteArrayValue;  // byteArrayValue = {'?', 'a', '\\'};
        retbyteArrayValue = WantOut_->GetByteArrayParam(std::string("byte_arraykey"));
        arraycompare = CompareArrayData<byte>(retbyteArrayValue, byteArrayValue);
        EXPECT_EQ(arraycompare, true);

        std::vector<zchar> retcharArrayValue;  // charArrayValue = {U'e', U'l', U'l', U'o'};
        retcharArrayValue = WantOut_->GetCharArrayParam(std::string("char_arraykey"));
        arraycompare = CompareArrayData<zchar>(retcharArrayValue, charArrayValue);
        EXPECT_EQ(arraycompare, true);

        std::vector<short> retshortArrayValue;  // shortArrayValue = {-1, 0, 1};
        retshortArrayValue = WantOut_->GetShortArrayParam(std::string("short_arraykey"));
        arraycompare = CompareArrayData<short>(retshortArrayValue, shortArrayValue);
        EXPECT_EQ(arraycompare, true);

        std::vector<int> retintArrayValue;  // intArrayValue = {-10, 0, 10};
        retintArrayValue = WantOut_->GetIntArrayParam(std::string("int_arraykey"));
        arraycompare = CompareArrayData<int>(retintArrayValue, intArrayValue);
        EXPECT_EQ(arraycompare, true);

        std::vector<long> retlonArrayValue;  // longArrayValue = {-100, 0, 100};
        retlonArrayValue = WantOut_->GetLongArrayParam(std::string("long_arraykey"));
        arraycompare = CompareArrayData<long>(retlonArrayValue, longArrayValue);
        EXPECT_EQ(arraycompare, true);

        std::vector<float> retfloatArrayValue;  // floatArrayValue = {-100.1, 0.1, 100.1};
        retfloatArrayValue = WantOut_->GetFloatArrayParam(std::string("float_arraykey"));
        arraycompare = CompareArrayData<float>(retfloatArrayValue, floatArrayValue);
        EXPECT_EQ(arraycompare, true);

        std::vector<double> retdoubleArrayValue;  // doubleArrayValue = {-1000.1, 0.1, 1000.1};
        retdoubleArrayValue = WantOut_->GetDoubleArrayParam(std::string("double_arraykey"));
        arraycompare = CompareArrayData<double>(retdoubleArrayValue, doubleArrayValue);
        EXPECT_EQ(arraycompare, true);

        std::vector<std::string> retstringArrayValue;  // stringArrayValue = {"stringtest1", "string@test2",
                                                       // "string@!#test2"};
        retstringArrayValue = WantOut_->GetStringArrayParam(std::string("string_arraykey"));
        arraycompare = CompareArrayData<std::string>(retstringArrayValue, stringArrayValue);
        EXPECT_EQ(arraycompare, true);

        GTEST_LOG_(INFO) << "AaFwk_Want_Parcelable_005 end";
    }
}

void WantBaseTest::CompareWant(const std::shared_ptr<Want> &want1, const std::shared_ptr<Want> &want2) const
{
    EXPECT_EQ(want1->GetAction(), want2->GetAction());
    EXPECT_EQ(want1->GetFlags(), want2->GetFlags());
    EXPECT_EQ(want1->GetType(), want2->GetType());
    EXPECT_EQ(want1->CountEntities(), want2->CountEntities());

    int count = want1->CountEntities();
    std::vector<std::string> entities1 = want1->GetEntities();
    std::vector<std::string> entities2 = want2->GetEntities();
    for (int i = 0; i < count; i++) {
        EXPECT_EQ(entities1.at(i), entities2.at(i));
    }

    OHOS::AppExecFwk::ElementName element1 = want1->GetElement();
    OHOS::AppExecFwk::ElementName element2 = want2->GetElement();
    EXPECT_EQ(element1.GetURI(), element1.GetURI());

    std::set<std::string> key1;
    std::set<std::string> key2;
    key1 = want1->GetParams().KeySet();
    key2 = want2->GetParams().KeySet();
    EXPECT_EQ(key1.size(), key2.size());

    std::set<std::string>::iterator iter1 = key1.begin();
    std::set<std::string>::iterator iter2 = key2.begin();
    for (; (iter1 != key1.end() && iter2 != key2.end()); iter1++, iter2++) {
        EXPECT_EQ(*iter1, *iter2);
    }
}

bool WantBaseTest::CompareWant(const std::shared_ptr<Want> &want1, const std::shared_ptr<Want> &want2,
    std::map<std::string, std::string> &keys) const
{
    if (want1 == nullptr || want2 == nullptr) {
        return false;
    }
    EXPECT_STREQ(want1->GetAction().c_str(), want2->GetAction().c_str());
    EXPECT_EQ(want1->CountEntities(), want2->CountEntities());

    if (want1->CountEntities() != want2->CountEntities()) {
        return false;
    }

    int count = want1->CountEntities();

    std::vector<std::string> entities1 = want1->GetEntities();
    std::vector<std::string> entities2 = want2->GetEntities();
    for (int i = 0; i < count; i++) {
        EXPECT_EQ(entities1.at(i), entities2.at(i));
    }
    EXPECT_EQ(want1->GetFlags(), want2->GetFlags());
    EXPECT_EQ(want1->GetElement(), want2->GetElement());

    for (auto it = keys.begin(); it != keys.end(); it++) {
        if (it->second == boolType) {
            bool v1 = want1->GetBoolParam(it->first, false);
            bool v2 = want2->GetBoolParam(it->first, false);
            EXPECT_EQ(v1, v2);
            EXPECT_EQ(v1, true);
        } else if (it->second == boolArrayType) {
            std::vector<bool> v1 = want1->GetBoolArrayParam(it->first);
            std::vector<bool> v2 = want2->GetBoolArrayParam(it->first);
            EXPECT_EQ(v1, v2);
        } else if (it->second == byteType) {
            byte v1 = want1->GetByteParam(it->first, 'j');
            byte v2 = want2->GetByteParam(it->first, 'k');
            EXPECT_EQ(v1, v2);
        } else if (it->second == byteArrayType) {
            std::vector<byte> v1 = want1->GetByteArrayParam(it->first);
            std::vector<byte> v2 = want2->GetByteArrayParam(it->first);
            EXPECT_EQ(v1, v2);
        } else if (it->second == charType) {
            zchar v1 = want1->GetCharParam(it->first, 0x01AB);
            zchar v2 = want2->GetCharParam(it->first, 0x02CD);
            EXPECT_EQ(v1, v2);
        } else if (it->second == charArrayType) {
            std::vector<zchar> v1 = want1->GetCharArrayParam(it->first);
            std::vector<zchar> v2 = want2->GetCharArrayParam(it->first);
            EXPECT_EQ(v1, v2);
        } else if (it->second == shortType) {
            short default1 = 123;
            short default2 = 456;
            short v1 = want1->GetShortParam(it->first, default1);
            short v2 = want2->GetShortParam(it->first, default2);
            EXPECT_EQ(v1, v2);
        } else if (it->second == shortArrayType) {
            std::vector<short> v1 = want1->GetShortArrayParam(it->first);
            std::vector<short> v2 = want2->GetShortArrayParam(it->first);
            EXPECT_EQ(v1, v2);
        } else if (it->second == intType) {
            int default1 = 1230000;
            int default2 = 4560000;
            int v1 = want1->GetIntParam(it->first, default1);
            int v2 = want2->GetIntParam(it->first, default2);
            EXPECT_EQ(v1, v2);
        } else if (it->second == intArrayType) {
            std::vector<int> v1 = want1->GetIntArrayParam(it->first);
            std::vector<int> v2 = want2->GetIntArrayParam(it->first);
            EXPECT_EQ(v1, v2);
        } else if (it->second == longType) {
            long default1 = 1e8;
            long default2 = 2e8;
            long v1 = want1->GetLongParam(it->first, default1);
            long v2 = want2->GetLongParam(it->first, default2);
            EXPECT_EQ(v1, v2);
        } else if (it->second == longArrayType) {
            std::vector<long> v1 = want1->GetLongArrayParam(it->first);
            std::vector<long> v2 = want2->GetLongArrayParam(it->first);
            EXPECT_EQ(v1, v2);
        } else if (it->second == floatType) {
            float default1 = 12.3;
            float default2 = 45.6;
            float v1 = want1->GetFloatParam(it->first, default1);
            float v2 = want2->GetFloatParam(it->first, default2);
            EXPECT_EQ(v1, v2);
        } else if (it->second == floatArrayType) {
            std::vector<float> v1 = want1->GetFloatArrayParam(it->first);
            std::vector<float> v2 = want2->GetFloatArrayParam(it->first);
            EXPECT_EQ(v1, v2);
        } else if (it->second == doubleType) {
            double default1 = 12.3;
            double default2 = 45.6;
            double v1 = want1->GetDoubleParam(it->first, default1);
            double v2 = want2->GetDoubleParam(it->first, default2);
            EXPECT_EQ(v1, v2);
        } else if (it->second == doubleArrayType) {
            std::vector<double> v1 = want1->GetDoubleArrayParam(it->first);
            std::vector<double> v2 = want2->GetDoubleArrayParam(it->first);
            EXPECT_EQ(v1, v2);
        } else if (it->second == stringType) {
            std::string v1 = want1->GetStringParam(it->first);
            std::string v2 = want2->GetStringParam(it->first);
            EXPECT_EQ(v1, v2);
        } else if (it->second == stringArrayType) {
            std::vector<std::string> v1 = want1->GetStringArrayParam(it->first);
            std::vector<std::string> v2 = want2->GetStringArrayParam(it->first);
            EXPECT_EQ(v1, v2);
        }
    }

    return true;
}

void WantBaseTest::SendParcelTest(const std::shared_ptr<Want> &want, std::map<std::string, std::string> &keys) const
{
    size_t pos1;
    size_t pos2;
    Parcel data;
    bool result = false;

    pos1 = data.GetWritePosition();
    result = data.WriteParcelable(want.get());
    pos2 = data.GetWritePosition();
    EXPECT_EQ(result, true);
    GTEST_LOG_(INFO) << "SendParcelTest: pos1: " << pos1 << ", pos2: " << pos2 << ", result: " << result;

    std::shared_ptr<Want> wantNew(data.ReadParcelable<Want>());
    EXPECT_NE(wantNew, nullptr);

    if (wantNew != nullptr) {
        result = CompareWant(want, wantNew, keys);
        EXPECT_EQ(result, true);
    }
}

void WantBaseTest::AddBoolParams(
    Want &want, std::map<std::string, std::string> &keys, int loop, unsigned int flag) const
{
    std::string key;
    std::string boolKey = "boolKey";
    std::string boolArrayKey = "boolArrayKey";
    for (int i = 0; i < loop; i++) {
        if (flag & FLAG_TEST_SINGLE) {
            bool boolValue = true;
            keys[boolKey + std::to_string(i)] = boolType;
            want.SetParam(boolKey + std::to_string(i), boolValue);
        }

        if (flag & FLAG_TEST_ARRAY) {
            std::vector<bool> boolArrayValue = {true, false, true};
            keys[key] = boolArrayType;
            want.SetParam(key, boolArrayValue);
        }
    }
}

void WantBaseTest::AddByteParams(
    Want &want, std::map<std::string, std::string> &keys, int loop, unsigned int flag) const
{
    std::string key;
    std::string byteKey = "byteKey";
    std::string byteArrayKey = "byteArrayKey";
    for (int i = 0; i < loop; i++) {
        if (flag & FLAG_TEST_SINGLE) {
            byte byteValue = 'z';
            key = byteKey + std::to_string(i);
            keys[key] = byteType;
            want.SetParam(key, byteValue);
        }

        if (flag & FLAG_TEST_ARRAY) {
            std::vector<byte> byteArrayValue = {'?', 'a', '\\'};
            key = byteArrayKey + std::to_string(i);
            keys[key] = byteArrayType;
            want.SetParam(key, byteArrayValue);
        }
    }
}

void WantBaseTest::AddCharParams(
    Want &want, std::map<std::string, std::string> &keys, int loop, unsigned int flag) const
{
    std::string key;
    std::string charKey = "charKey";
    std::string charArrayKey = "charArrayKey";
    for (int i = 0; i < loop; i++) {
        if (flag & FLAG_TEST_SINGLE) {
            zchar charValue = U'h';
            key = charKey + std::to_string(i);
            keys[key] = charType;
            want.SetParam(key, charValue);
        }

        if (flag & FLAG_TEST_ARRAY) {
            std::vector<zchar> charArrayValue = {U'e', U'l', U'l', U'o'};
            key = charArrayKey + std::to_string(i);
            keys[key] = charArrayType;
            want.SetParam(key, charArrayValue);
        }
    }
}

void WantBaseTest::AddShortParams(
    Want &want, std::map<std::string, std::string> &keys, int loop, unsigned int flag) const
{
    std::string key;
    std::string shortKey = "shortKey";
    std::string shortArrayKey = "shortArrayKey";
    for (int i = 0; i < loop; i++) {
        if (flag & FLAG_TEST_SINGLE) {
            short shortValue = 1;
            key = shortKey + std::to_string(i);
            keys[key] = shortType;
            want.SetParam(key, shortValue);
        }

        if (flag & FLAG_TEST_ARRAY) {
            std::vector<short> shortArrayValue = {-1, 0, 1};
            key = shortArrayKey + std::to_string(i);
            keys[key] = shortArrayType;
            want.SetParam(key, shortArrayValue);
        }
    }
}

void WantBaseTest::AddIntParams(Want &want, std::map<std::string, std::string> &keys, int loop, unsigned int flag) const
{
    std::string key;
    std::string intKey = "intKey";
    std::string intArrayKey = "intArrayKey";
    for (int i = 0; i < loop; i++) {
        if (flag & FLAG_TEST_SINGLE) {
            int intValue = 10;
            key = intKey + std::to_string(i);
            keys[key] = intType;
            want.SetParam(key, intValue);
        }

        if (flag & FLAG_TEST_ARRAY) {
            std::vector<int> intArrayValue = {-10, 0, 10};
            key = intArrayKey + std::to_string(i);
            keys[key] = intArrayType;
            want.SetParam(key, intArrayValue);
        }
    }
}

void WantBaseTest::AddLongParams(
    Want &want, std::map<std::string, std::string> &keys, int loop, unsigned int flag) const
{
    std::string key;
    std::string longKey = "longKey";
    std::string longArrayKey = "longArrayKey";
    for (int i = 0; i < loop; i++) {
        if (flag & FLAG_TEST_SINGLE) {
            long longValue = 100L;
            key = longKey + std::to_string(i);
            keys[key] = longType;
            want.SetParam(key, longValue);
        }

        if (flag & FLAG_TEST_ARRAY) {
            std::vector<long> longArrayValue = {-100, 0, 100};
            key = longArrayKey + std::to_string(i);
            keys[key] = longArrayType;
            want.SetParam(key, longArrayValue);
        }
    }
}

void WantBaseTest::AddFloatParams(
    Want &want, std::map<std::string, std::string> &keys, int loop, unsigned int flag) const
{
    std::string key;
    std::string floatKey = "floatKey";
    std::string floatArrayKey = "floatArrayKey";
    for (int i = 0; i < loop; i++) {
        if (flag & FLAG_TEST_SINGLE) {
            float floatValue = 100.1f;
            key = floatKey + std::to_string(i);
            keys[key] = floatType;
            want.SetParam(key, floatValue);
        }

        if (flag & FLAG_TEST_ARRAY) {
            std::vector<float> floatArrayValue = {-100.1, 0.1, 100.1};
            key = floatArrayKey + std::to_string(i);
            keys[key] = floatArrayType;
            want.SetParam(key, floatArrayValue);
        }
    }
}

void WantBaseTest::AddDoubleParams(
    Want &want, std::map<std::string, std::string> &keys, int loop, unsigned int flag) const
{
    std::string key;
    std::string doubleKey = "doubleKey";
    std::string doubleArrayKey = "doubleArrayKey";
    for (int i = 0; i < loop; i++) {
        if (flag & FLAG_TEST_SINGLE) {
            double doubleValue = 1000.1;
            key = doubleKey + std::to_string(i);
            keys[key] = doubleType;
            want.SetParam(key, doubleValue);
        }

        if (flag & FLAG_TEST_ARRAY) {
            std::vector<double> doubleArrayValue = {-1000.1, 0.1, 1000.1};
            key = doubleArrayKey + std::to_string(i);
            keys[key] = doubleArrayType;
            want.SetParam(key, doubleArrayValue);
        }
    }
}

void WantBaseTest::AddStringParams(
    Want &want, std::map<std::string, std::string> &keys, int loop, unsigned int flag) const
{
    std::string key;
    std::string stringKey = "stringKey";
    std::string stringArrayKey = "stringArrayKey";
    for (int i = 0; i < loop; i++) {
        if (flag & FLAG_TEST_SINGLE) {
            string stringValue = "zzzz";
            key = stringKey + std::to_string(i);
            keys[key] = stringType;
            want.SetParam(key, stringValue);
        }

        if (flag & FLAG_TEST_ARRAY) {
            std::vector<std::string> stringArrayValue = {"??", "aa", "\\\\"};
            key = stringArrayKey + std::to_string(i);
            keys[key] = stringArrayType;
            want.SetParam(key, stringArrayValue);
        }
    }
}

/**
 * @tc.number: AaFwk_Want_Parcelable_0600
 * @tc.name: parcelable
 * @tc.desc: Verify parcelable.
 */
HWTEST_F(WantBaseTest, AaFwk_Want_Parcelable_0600, Function | MediumTest | Level1)
{
    std::string action = "want.action.test";
    unsigned int flag = 0x789;
    std::string entity = "want.entity.test";
    OHOS::AppExecFwk::ElementName element("bundlename", "appname", "abilityname");

    std::shared_ptr<Want> want = std::make_shared<Want>();
    if (want != nullptr) {
        want->SetAction(action);
        want->AddEntity(entity);
        want->AddFlags(flag);
        want->SetElement(element);

        std::map<std::string, std::string> keys;

        SendParcelTest(want, keys);
    }
}

/**
 * @tc.number: AaFwk_Want_Parcelable_0700
 * @tc.name: parcelable
 * @tc.desc: Verify parcelable.
 */
HWTEST_F(WantBaseTest, AaFwk_Want_Parcelable_0700, Function | MediumTest | Level1)
{
    std::string action = "want.action.test";
    unsigned int flag = 0x789;
    std::string entity = "want.entity.test";
    OHOS::AppExecFwk::ElementName element("bundlename", "appname", "abilityname");

    std::shared_ptr<Want> want = std::make_shared<Want>();
    if (want != nullptr) {
        want->SetAction(action);
        want->AddEntity(entity);
        want->AddFlags(flag);
        want->SetElement(element);

        int loop = 1;
        std::map<std::string, std::string> keys;

        AddByteParams(*(want.get()), keys, loop, FLAG_TEST_BOTH);
        AddCharParams(*(want.get()), keys, loop, FLAG_TEST_BOTH);
        AddShortParams(*(want.get()), keys, loop, FLAG_TEST_BOTH);
        AddIntParams(*(want.get()), keys, loop, FLAG_TEST_BOTH);
        AddLongParams(*(want.get()), keys, loop, FLAG_TEST_BOTH);
        AddFloatParams(*(want.get()), keys, loop, FLAG_TEST_BOTH);
        AddDoubleParams(*(want.get()), keys, loop, FLAG_TEST_BOTH);
        AddStringParams(*(want.get()), keys, loop, FLAG_TEST_BOTH);

        SendParcelTest(want, keys);
    }
}

/**
 * @tc.number: AaFwk_Want_Parcelable_0800
 * @tc.name: parcelable
 * @tc.desc: Verify parcelable.
 */
HWTEST_F(WantBaseTest, AaFwk_Want_Parcelable_0800, Function | MediumTest | Level1)
{
    std::string action = "want.action.test";
    unsigned int flag = 0x789;
    std::string entity = "want.entity.test";
    OHOS::AppExecFwk::ElementName element("bundlename", "appname", "abilityname");

    std::shared_ptr<Want> want = std::make_shared<Want>();
    if (want != nullptr) {
        want->SetAction(action);
        want->AddEntity(entity);
        want->AddFlags(flag);
        want->SetElement(element);
        std::map<std::string, std::string> keys;

        SendParcelTest(want, keys);
    }
}

/**
 * @tc.number: AaFwk_Want_FormatMimeType_0100
 * @tc.name: formatMimeType
 * @tc.desc: formats data of a specified MIME type.
 */
HWTEST_F(WantBaseTest, AaFwk_Want_FormatMimeType_0100, Function | MediumTest | Level1)
{
    std::string mimeType = "Application/Envoy";
    std::string mimeTypeResult = "application/envoy";

    EXPECT_EQ(mimeTypeResult, Want::FormatMimeType(mimeType));
}

/**
 * @tc.number: AaFwk_Want_FormatMimeType_0200
 * @tc.name: formatMimeType
 * @tc.desc: formats data of a specified MIME type.
 */
HWTEST_F(WantBaseTest, AaFwk_Want_FormatMimeType_0200, Function | MediumTest | Level1)
{
    std::string mimeType = "APPLICATION/ENVOY";
    std::string mimeTypeResult = "application/envoy";

    EXPECT_EQ(mimeTypeResult, Want::FormatMimeType(mimeType));
}

/**
 * @tc.number: AaFwk_Want_FormatMimeType_0300
 * @tc.name: formatMimeType
 * @tc.desc: formats data of a specified MIME type.
 */
HWTEST_F(WantBaseTest, AaFwk_Want_FormatMimeType_0300, Function | MediumTest | Level1)
{
    std::string mimeType = " Appl icati on/ Envoy ";
    std::string mimeTypeResult = "application/envoy";

    EXPECT_EQ(mimeTypeResult, Want::FormatMimeType(mimeType));
}

/**
 * @tc.number: AaFwk_Want_FormatMimeType_0400
 * @tc.name: formatMimeType
 * @tc.desc: formats data of a specified MIME type.
 */
HWTEST_F(WantBaseTest, AaFwk_Want_FormatMimeType_0400, Function | MediumTest | Level1)
{
    std::string mimeType = " Appl icati on/ Envoy ; yovnE ;no itaci lppA ";
    std::string mimeTypeResult = "application/envoy";

    EXPECT_EQ(mimeTypeResult, Want::FormatMimeType(mimeType));
}

/**
 * @tc.number:  AaFwk_Want_ParseUri_ToUri_0100
 * @tc.name: ParseUri and ToUri
 * @tc.desc: Verify the function when Want is empty.
 */
HWTEST_F(WantBaseTest, AaFwk_Want_ParseUri_ToUri_0100, Function | MediumTest | Level1)
{
    std::size_t pos = 0;
    std::size_t content = 0;
    std::size_t head = 0;
    Want wantOrigin;

    std::string uri = wantOrigin.ToUri();

    head = uri.find(WantBaseTest::URI_STRING_HEAD, pos);
    EXPECT_EQ(head, pos);
    if (head != std::string::npos) {
        pos += head + WantBaseTest::URI_STRING_HEAD.length() - 1;
    }

    content = uri.find(WantBaseTest::URI_STRING_END, pos);
    EXPECT_EQ(content, pos);
    if (content != std::string::npos) {
        pos += WantBaseTest::URI_STRING_END.length();
    }

    EXPECT_EQ(uri.length(), pos);

    Want *wantNew = Want::ParseUri(uri);
    EXPECT_NE(wantNew, nullptr);

    if (wantNew != nullptr) {
        EXPECT_EQ(wantNew->GetAction(), std::string(""));
        for (auto entity : wantNew->GetEntities()) {
            EXPECT_EQ(entity, std::string(""));
        }
        OHOS::AppExecFwk::ElementName element;
        EXPECT_EQ(wantNew->GetElement(), element);
        EXPECT_EQ(static_cast<int>(wantNew->GetFlags()), 0);

        delete wantNew;
    }
}

/**
 * @tc.number:  AaFwk_Want_ParseUri_ToUri_0200
 * @tc.name: ParseUri and ToUri
 * @tc.desc: Verify the function when Want only has action/entity/flag/element.
 */
HWTEST_F(WantBaseTest, AaFwk_Want_ParseUri_ToUri_0200, Function | MediumTest | Level1)
{
    std::string search;
    std::size_t pos = 0;
    std::size_t length = 0;
    std::size_t result = 0;
    std::size_t head = 0;

    std::string action = Want::ACTION_PLAY;
    std::string entity = Want::ENTITY_VIDEO;
    unsigned int flag = 0x0f0f0f0f;
    std::string flagStr = "0x0f0f0f0f";
    std::string device = "device1";
    std::string bundle = "bundle1";
    std::string ability = "ability1";
    OHOS::AppExecFwk::ElementName element(device, bundle, ability);

    Want wantOrigin;
    wantOrigin.SetAction(action);
    wantOrigin.AddEntity(entity);
    wantOrigin.AddFlags(flag);
    wantOrigin.SetElement(element);

    std::string uri = wantOrigin.ToUri();

    search = WantBaseTest::URI_STRING_HEAD;
    result = uri.find(search, pos);
    EXPECT_EQ(result, pos);
    if (result != std::string::npos) {
        head = result + search.length();
    }
    length += head;

    search = std::string("action=") + action + std::string(";");
    result = uri.find(search);
    EXPECT_NE(result, std::string::npos);
    EXPECT_GE(result, head);
    if (result != std::string::npos) {
        length += search.length();
    }

    search = std::string("entity=") + entity + std::string(";");
    result = uri.find(search);
    EXPECT_NE(result, std::string::npos);
    EXPECT_GE(result, head);
    if (result != std::string::npos) {
        length += search.length();
    }

    search = std::string("flag=") + flagStr + std::string(";");
    result = uri.find(search);
    EXPECT_NE(result, std::string::npos);
    EXPECT_GE(result, head);
    if (result != std::string::npos) {
        length += search.length();
    }

    search = std::string("device=") + device + std::string(";");
    result = uri.find(search);
    EXPECT_NE(result, std::string::npos);
    EXPECT_GE(result, head);
    if (result != std::string::npos) {
        length += search.length();
    }

    search = std::string("bundle=") + bundle + std::string(";");
    result = uri.find(search);
    EXPECT_NE(result, std::string::npos);
    EXPECT_GE(result, head);
    if (result != std::string::npos) {
        length += search.length();
    }

    search = std::string("ability=") + ability + std::string(";");
    result = uri.find(search);
    EXPECT_NE(result, std::string::npos);
    EXPECT_GE(result, head);
    if (result != std::string::npos) {
        length += search.length();
    }

    search = WantBaseTest::URI_STRING_END;
    result = uri.find(search);
    EXPECT_NE(result, std::string::npos);
    EXPECT_GE(result, head);
    if (result != std::string::npos) {
        length += search.length() - 1;
    }

    EXPECT_EQ(uri.length(), length);

    Want *wantNew = Want::ParseUri(uri);
    EXPECT_NE(wantNew, nullptr);

    if (wantNew != nullptr) {
        EXPECT_EQ(wantNew->GetAction(), action);
        for (auto entityItem : wantNew->GetEntities()) {
            EXPECT_EQ(entityItem, entity);
        }
        EXPECT_EQ(wantNew->GetElement().GetDeviceID(), device);
        EXPECT_EQ(wantNew->GetElement().GetBundleName(), bundle);
        EXPECT_EQ(wantNew->GetElement().GetAbilityName(), ability);
        EXPECT_EQ(wantNew->GetFlags(), flag);

        delete wantNew;
    }
}

/**
 * @tc.number:  AaFwk_Want_ParseUri_ToUri_0300
 * @tc.name: ParseUri and ToUri
 * @tc.desc:  Verify the function when Want only has parameter and the parameter
 *            has only 1 float type element.
 */
HWTEST_F(WantBaseTest, AaFwk_Want_ParseUri_ToUri_0300, Function | MediumTest | Level1)
{
    std::string search;
    std::string substring;
    std::size_t pos = 0;
    std::size_t length = 0;
    std::size_t result = 0;
    std::size_t delims = 0;
    std::size_t head = 0;
    Want wantOrigin;
    std::string keyFloat = "keyFloat";
    float valueFloatOrigin = 123.4;
    wantOrigin.SetParam(keyFloat, valueFloatOrigin);

    std::string uri = wantOrigin.ToUri();

    search = WantBaseTest::URI_STRING_HEAD;
    result = uri.find(search, pos);
    EXPECT_EQ(result, pos);
    if (result != std::string::npos) {
        head = result + search.length();
    }
    length += head;

    search = Float::SIGNATURE + std::string(".") + keyFloat + std::string("=");
    result = uri.find(search);
    EXPECT_NE(result, std::string::npos);
    EXPECT_GE(result, head);
    length += search.length();
    if (result != std::string::npos) {
        pos = result + search.length();
        delims = uri.find(";", pos);
        if (delims != std::string::npos) {
            substring = uri.substr(pos, delims - pos);
            float valueFloatNew = Float::Unbox(Float::Parse(substring));
            EXPECT_EQ(valueFloatNew, valueFloatOrigin);
            length += substring.length() + 1;
        }
    }

    search = WantBaseTest::URI_STRING_END;
    result = uri.find(search);
    EXPECT_NE(result, std::string::npos);
    EXPECT_GE(result, head);
    if (result != std::string::npos) {
        length += search.length() - 1;
    }

    EXPECT_EQ(uri.length(), length);

    Want *wantNew = Want::ParseUri(uri);
    EXPECT_NE(wantNew, nullptr);

    if (wantNew != nullptr) {
        float floatNew = wantNew->GetFloatParam(keyFloat, 0.1);
        float floatOld = wantOrigin.GetFloatParam(keyFloat, 1.1);
        EXPECT_EQ(floatNew, floatOld);
        delete wantNew;
    }
}

/**
 * @tc.number:  AaFwk_Want_ParseUri_ToUri_0400
 * @tc.name: ParseUri and ToUri
 * @tc.desc: Verify the function when Want only has parameter and the parameter
 *           has only one float and one string type element.
 */
HWTEST_F(WantBaseTest, AaFwk_Want_ParseUri_ToUri_0400, Function | MediumTest | Level1)
{
    std::string search;
    std::string substring;
    std::size_t pos = 0;
    std::size_t length = 0;
    std::size_t result = 0;
    std::size_t delims = 0;
    std::size_t head = 0;
    Want wantOrigin;
    std::string keyFloat = "keyFloat";
    std::string keyString = "keyString";
    float valueFloatOrigin = 123.4;
    std::string valueStringOrigin = "abcd";
    wantOrigin.SetParam(keyFloat, valueFloatOrigin);
    wantOrigin.SetParam(keyString, valueStringOrigin);

    std::string uri = wantOrigin.ToUri();

    search = WantBaseTest::URI_STRING_HEAD;
    result = uri.find(search, pos);
    EXPECT_NE(result, std::string::npos);
    EXPECT_EQ(result, pos);
    if (result != std::string::npos) {
        head = result + search.length();
    }
    length += head;

    search = Float::SIGNATURE + std::string(".") + keyFloat + std::string("=");
    result = uri.find(search);
    EXPECT_NE(result, std::string::npos);
    EXPECT_GE(result, head);
    length += search.length();
    if (result != std::string::npos) {
        pos = result + search.length();
        delims = uri.find(";", pos);
        if (delims != std::string::npos) {
            substring = uri.substr(pos, delims - pos);
            float valueFloatNew = Float::Unbox(Float::Parse(substring));
            EXPECT_EQ(valueFloatNew, valueFloatOrigin);
            length += substring.length() + 1;
        }
    }

    search = String::SIGNATURE + std::string(".") + keyString + std::string("=");
    result = uri.find(search);
    EXPECT_NE(result, std::string::npos);
    EXPECT_GE(result, head);
    length += search.length();
    if (result != std::string::npos) {
        pos = result + search.length();
        delims = uri.find(";", result);
        if (delims != std::string::npos) {
            std::string substring = uri.substr(pos, delims - pos);
            std::string valueStringNew = String::Unbox(String::Parse(substring));
            EXPECT_EQ(valueStringNew, valueStringOrigin);
            length += substring.length() + 1;
        }
    }

    search = WantBaseTest::URI_STRING_END;
    result = uri.find(search);
    EXPECT_NE(result, std::string::npos);
    EXPECT_GE(result, head);
    if (result != std::string::npos) {
        length += search.length() - 1;
    }

    EXPECT_EQ(uri.length(), length);

    Want *wantNew = Want::ParseUri(uri);
    EXPECT_NE(wantNew, nullptr);

    if (wantNew != nullptr) {
        float floatNew = wantNew->GetFloatParam(keyFloat, 0);
        float floatOld = wantOrigin.GetFloatParam(keyFloat, 1);
        EXPECT_EQ(floatNew, floatOld);

        std::string stringNew = wantNew->GetStringParam(keyString);
        std::string stringOld = wantOrigin.GetStringParam(keyString);
        EXPECT_EQ(stringNew, stringOld);

        delete wantNew;
    }
}

/**
 * @tc.number:  AaFwk_Want_ParseUri_ToUri_0500
 * @tc.name: ParseUri and ToUri
 * @tc.desc: Verify the function when Want only has parameter and the parameter
 *           has only one float array type element.
 */
HWTEST_F(WantBaseTest, AaFwk_Want_ParseUri_ToUri_0500, Function | MediumTest | Level1)
{
    std::string search;
    std::string substring;
    std::size_t pos = 0;
    std::size_t length = 0;
    std::size_t result = 0;
    std::size_t delims = 0;
    std::size_t head = 0;
    Want wantOrigin;
    std::string keyFloatArray = "keyFloatArray";
    std::vector<float> valueFloatArrayOrigin = {1.1, 2.1, 3.1};
    wantOrigin.SetParam(keyFloatArray, valueFloatArrayOrigin);

    std::string uri = wantOrigin.ToUri();

    search = WantBaseTest::URI_STRING_HEAD;
    result = uri.find(search, pos);
    EXPECT_NE(result, std::string::npos);
    EXPECT_EQ(result, pos);
    if (result != std::string::npos) {
        head = result + search.length();
    }
    length += head;

    search = Array::SIGNATURE + std::string(".") + keyFloatArray + std::string("=");
    result = uri.find(search);
    EXPECT_NE(result, std::string::npos);
    EXPECT_GE(result, head);
    length += search.length();
    if (result != std::string::npos) {
        pos = result + search.length();
        delims = uri.find(";", result);
        if (delims != std::string::npos) {
            std::string substring = uri.substr(pos, delims - pos);
            sptr<IArray> array = Array::Parse(substring);
            std::vector<float> valueFloatArrayNew;
            auto func = [&valueFloatArrayNew](
                            IInterface *object) { valueFloatArrayNew.push_back(Float::Unbox(IFloat::Query(object))); };
            Array::ForEach(array, func);
            EXPECT_EQ(valueFloatArrayNew, valueFloatArrayOrigin);
            length += substring.length() + 1;
        }
    }

    search = WantBaseTest::URI_STRING_END;
    result = uri.find(search);
    EXPECT_NE(result, std::string::npos);
    EXPECT_GE(result, head);
    if (result != std::string::npos) {
        length += search.length() - 1;
    }

    EXPECT_EQ(uri.length(), length);

    Want *wantNew = Want::ParseUri(uri);
    EXPECT_NE(wantNew, nullptr);

    if (wantNew != nullptr) {
        std::vector<float> arrayNew = wantNew->GetFloatArrayParam(keyFloatArray);
        std::vector<float> arrayOld = wantOrigin.GetFloatArrayParam(keyFloatArray);
        EXPECT_EQ(arrayNew, arrayOld);
        delete wantNew;
    }
}

/**
 * @tc.number:  AaFwk_Want_ParseUri_ToUri_0600
 * @tc.name: ParseUri and ToUri
 * @tc.desc: Verify the function when Want only has parameter and the parameter
 *           has only one int array and one string array type element
 */
HWTEST_F(WantBaseTest, AaFwk_Want_ParseUri_ToUri_0600, Function | MediumTest | Level1)
{
    std::string search;
    std::string substring;
    std::size_t pos = 0;
    std::size_t length = 0;
    std::size_t result = 0;
    std::size_t delims = 0;
    std::size_t head = 0;
    Want wantOrigin;
    std::string keyFloatArray = "keyFloatArray";
    std::string keyStringArray = "keyStringArray";
    std::vector<float> valueFloatArrayOrigin = {1.1, 2.1, 3.1};
    std::vector<std::string> valueStringArrayOrigin = {"aa", "bb", "cc"};
    wantOrigin.SetParam(keyFloatArray, valueFloatArrayOrigin);
    wantOrigin.SetParam(keyStringArray, valueStringArrayOrigin);

    std::string uri = wantOrigin.ToUri();

    search = WantBaseTest::URI_STRING_HEAD;
    result = uri.find(search, pos);
    EXPECT_NE(result, std::string::npos);
    EXPECT_EQ(result, pos);
    if (result != std::string::npos) {
        head = result + search.length();
    }
    length += head;

    search = Array::SIGNATURE + std::string(".") + keyFloatArray + std::string("=");
    result = uri.find(search);
    EXPECT_NE(result, std::string::npos);
    EXPECT_GE(result, head);
    length += search.length();
    if (result != std::string::npos) {
        pos = result + search.length();
        delims = uri.find(";", result);
        if (delims != std::string::npos) {
            std::string substring = uri.substr(pos, delims - pos);
            sptr<IArray> array = Array::Parse(substring);
            std::vector<float> valueFloatArrayNew;
            auto func = [&valueFloatArrayNew](
                            IInterface *object) { valueFloatArrayNew.push_back(Float::Unbox(IFloat::Query(object))); };
            Array::ForEach(array, func);
            EXPECT_EQ(valueFloatArrayNew, valueFloatArrayOrigin);
            length += substring.length() + 1;
        }
    }

    search = Array::SIGNATURE + std::string(".") + keyStringArray + std::string("=");
    result = uri.find(search);
    EXPECT_NE(result, std::string::npos);
    EXPECT_GE(result, head);
    length += search.length();
    if (result != std::string::npos) {
        pos = result + search.length();
        delims = uri.find(";", result);
        if (delims != std::string::npos) {
            std::string substring = uri.substr(pos, delims - pos);
            sptr<IArray> array = Array::Parse(substring);
            std::vector<std::string> valueStringArrayNew;
            auto func = [&valueStringArrayNew](IInterface *object) {
                valueStringArrayNew.push_back(String::Unbox(IString::Query(object)));
            };
            Array::ForEach(array, func);
            EXPECT_EQ(valueStringArrayNew, valueStringArrayOrigin);
            length += substring.length() + 1;
        }
    }

    search = WantBaseTest::URI_STRING_END;
    result = uri.find(search);
    EXPECT_NE(result, std::string::npos);
    EXPECT_GE(result, head);
    if (result != std::string::npos) {
        length += search.length() - 1;
    }

    EXPECT_EQ(uri.length(), length);

    Want *wantNew = Want::ParseUri(uri);
    EXPECT_NE(wantNew, nullptr);

    if (wantNew != nullptr) {
        std::vector<float> arrayFloatNew = wantNew->GetFloatArrayParam(keyFloatArray);
        std::vector<float> arrayFloatOld = wantOrigin.GetFloatArrayParam(keyFloatArray);
        EXPECT_EQ(arrayFloatNew, arrayFloatOld);

        std::vector<std::string> arrayStringNew = wantNew->GetStringArrayParam(keyStringArray);
        std::vector<std::string> arrayStringOld = wantOrigin.GetStringArrayParam(keyStringArray);
        EXPECT_EQ(arrayStringNew, arrayStringOld);

        delete wantNew;
    }
}

/**
 * @tc.number:  AaFwk_Want_ParseUri_ToUri_0700
 * @tc.name: ParseUri and ToUri
 * @tc.desc: Verify the function when the length of input string is 0.
 */
HWTEST_F(WantBaseTest, AaFwk_Want_ParseUri_ToUri_0700, Function | MediumTest | Level1)
{
    std::string uri;
    EXPECT_EQ(static_cast<int>(uri.length()), 0);

    Want *want = Want::ParseUri(uri);
    EXPECT_EQ(want, nullptr);

    if (want != nullptr) {
        delete want;
    }
}

/**
 * @tc.number:  AaFwk_Want_ParseUri_ToUri_0800
 * @tc.name: ParseUri and ToUri
 * @tc.desc: Verify the function when the action etc. are empty.
 */
HWTEST_F(WantBaseTest, AaFwk_Want_ParseUri_ToUri_0800, Function | MediumTest | Level1)
{
    std::string empty;
    std::string uri = "#Want;action=;entity=;device=;bundle=;ability=;flag=;end";
    EXPECT_NE(static_cast<int>(uri.length()), 0);

    Want *want = Want::ParseUri(uri);
    EXPECT_NE(want, nullptr);

    if (want != nullptr) {
        EXPECT_EQ(want->GetAction(), empty);
        for (auto entityItem : want->GetEntities()) {
            EXPECT_EQ(entityItem, empty);
        }
        EXPECT_EQ(want->GetFlags(), (unsigned int)0);
        OHOS::AppExecFwk::ElementName element;
        EXPECT_EQ(want->GetElement(), element);
        EXPECT_EQ(want->HasParameter(empty), false);
        delete want;
    }
}

/**
 * @tc.number:  AaFwk_Want_ParseUri_ToUri_0900
 * @tc.name: ParseUri and ToUri
 * @tc.desc: Verify the function when flag is not number.
 */
HWTEST_F(WantBaseTest, AaFwk_Want_ParseUri_ToUri_0900, Function | MediumTest | Level1)
{
    std::string empty;
    std::string uri = "#Want;action=want.action.VIEW;flag=\"123\";end";
    EXPECT_NE(static_cast<int>(uri.length()), 0);

    Want *want = Want::ParseUri(uri);
    EXPECT_EQ(want, nullptr);
    if (want != nullptr) {
        delete want;
        want = nullptr;
    }
}

/**
 * @tc.number:  AaFwk_Want_ParseUri_ToUri_1000
 * @tc.name: ParseUri and ToUri
 * @tc.desc: Verify the function when head is not "#Want".
 */
HWTEST_F(WantBaseTest, AaFwk_Want_ParseUri_ToUri_1000, Function | MediumTest | Level1)
{
    std::string empty;
    std::string uri = "action=want.action.VIEW;end";
    EXPECT_NE(static_cast<int>(uri.length()), 0);

    Want *want = Want::ParseUri(uri);
    EXPECT_EQ(want, nullptr);
    if (want != nullptr) {
        delete want;
        want = nullptr;
    }
}

/**
 * @tc.number:  AaFwk_Want_ParseUri_ToUri_1100
 * @tc.name: ParseUri and ToUri
 * @tc.desc: Verify the function when flag is empty.
 */
HWTEST_F(WantBaseTest, AaFwk_Want_ParseUri_ToUri_1100, Function | MediumTest | Level1)
{
    std::string empty;
    std::string uri = "#Want;flag=;end";
    EXPECT_NE(static_cast<int>(uri.length()), 0);

    Want *want = Want::ParseUri(uri);
    EXPECT_NE(want, nullptr);

    if (want != nullptr) {
        EXPECT_EQ(want->GetFlags(), static_cast<unsigned int>(0));
        delete want;
    }
}

/**
 * @tc.number:  AaFwk_Want_ParseUri_ToUri_1200
 * @tc.name: ParseUri and ToUri
 * @tc.desc: Verify the function when x is capital.
 */
HWTEST_F(WantBaseTest, AaFwk_Want_ParseUri_ToUri_1200, Function | MediumTest | Level1)
{
    std::string empty;
    unsigned int flag = 0X12345678;
    std::string uri = "#Want;flag=0X12345678;end";
    EXPECT_NE(static_cast<int>(uri.length()), 0);

    Want *want = Want::ParseUri(uri);
    EXPECT_NE(want, nullptr);

    if (want != nullptr) {
        EXPECT_EQ(want->GetFlags(), flag);
        delete want;
    }
}

/**
 * @tc.number:  AaFwk_Want_ParseUri_ToUri_1300
 * @tc.name: ParseUri and ToUri
 * @tc.desc: Verify the function when special character.
 */
HWTEST_F(WantBaseTest, AaFwk_Want_ParseUri_ToUri_1300, Function | MediumTest | Level1)
{
    std::string action = "\\";
    std::string entity = "../../../jj/j=075/./.;;/07507399/\\\\;;--==.com.\a\b\tfoobar.vide\073\\075";
    unsigned int flag = 0x0f0f0f0f;
    std::string flagStr = "0x0f0f0f0f";
    std::string key = "\\kkk=.=;";
    std::string value = "==\\\\\\.;\\;\\;\\=\\\073075\\\\075073";

    Want wantOrigin;
    wantOrigin.SetAction(action);
    wantOrigin.AddEntity(entity);
    wantOrigin.AddFlags(flag);
    wantOrigin.SetParam(key, value);

    std::string uri = wantOrigin.ToUri();

    Want *wantNew = Want::ParseUri(uri);
    EXPECT_NE(wantNew, nullptr);

    if (wantNew != nullptr) {
        EXPECT_STREQ(wantNew->GetAction().c_str(), action.c_str());
        for (auto entityItem : wantNew->GetEntities()) {
            EXPECT_EQ(entityItem, entity);
        }
        EXPECT_EQ(wantNew->GetFlags(), flag);
        EXPECT_EQ(wantNew->GetStringParam(key), value);

        delete wantNew;
        wantNew = nullptr;
    }
}

/**
 * @tc.number:  AaFwk_Want_ParseUri_ToUri_1400
 * @tc.name: ParseUri and ToUri
 * @tc.desc: Verify the function when no '=' or only has a '='.
 */
HWTEST_F(WantBaseTest, AaFwk_Want_ParseUri_ToUri_1400, Function | MediumTest | Level1)
{
    std::string uri = "#Want;action;end";
    Want *want = Want::ParseUri(uri);
    EXPECT_EQ(want, nullptr);
    if (want != nullptr) {
        delete want;
        want = nullptr;
    }

    uri = "#Want;entity;end";
    want = Want::ParseUri(uri);
    EXPECT_EQ(want, nullptr);
    if (want != nullptr) {
        delete want;
        want = nullptr;
    }

    uri = "#Want;device;end";
    want = Want::ParseUri(uri);
    EXPECT_EQ(want, nullptr);
    if (want != nullptr) {
        delete want;
        want = nullptr;
    }

    uri = "#Want;bundle;end";
    want = Want::ParseUri(uri);
    EXPECT_EQ(want, nullptr);
    if (want != nullptr) {
        delete want;
        want = nullptr;
    }

    uri = "#Want;ability;end";
    want = Want::ParseUri(uri);
    EXPECT_EQ(want, nullptr);
    if (want != nullptr) {
        delete want;
        want = nullptr;
    }

    uri = "#Want;flag;end";
    want = Want::ParseUri(uri);
    EXPECT_EQ(want, nullptr);
    if (want != nullptr) {
        delete want;
        want = nullptr;
    }

    uri = "#Want;param;end";
    want = Want::ParseUri(uri);
    EXPECT_EQ(want, nullptr);
    if (want != nullptr) {
        delete want;
        want = nullptr;
    }

    uri = "#Want;=;end";
    want = Want::ParseUri(uri);
    EXPECT_NE(want, nullptr);
    if (want != nullptr) {
        delete want;
        want = nullptr;
    }

    uri = "#Want;abc=;end";
    want = Want::ParseUri(uri);
    EXPECT_NE(want, nullptr);
    if (want != nullptr) {
        delete want;
        want = nullptr;
    }

    uri = "#Want;=abc;end";
    want = Want::ParseUri(uri);
    EXPECT_NE(want, nullptr);
    if (want != nullptr) {
        delete want;
        want = nullptr;
    }

    uri = "#Want;xxxx=yyy;end";
    want = Want::ParseUri(uri);
    EXPECT_NE(want, nullptr);
    if (want != nullptr) {
        delete want;
        want = nullptr;
    }

    uri = "#Want;;;;;;end";
    want = Want::ParseUri(uri);
    EXPECT_NE(want, nullptr);
    if (want != nullptr) {
        delete want;
        want = nullptr;
    }
}

/**
 * @tc.number:  AaFwk_Want_Flags_0100
 * @tc.name: SetFlags/AddFlags/GetFlags/RemoveFlags
 * @tc.desc: Verify SetFlags/AddFlags/GetFlags/RemoveFlags.
 */
HWTEST_F(WantBaseTest, AaFwk_Want_Flags_0100, Function | MediumTest | Level1)
{
    int flags = 3;
    int returnsflags;
    int description = 8;

    want_->SetFlags(description);
    want_->AddFlags(flags);
    returnsflags = want_->GetFlags();
    EXPECT_EQ(11, returnsflags);

    want_->RemoveFlags(flags);
    returnsflags = want_->GetFlags();
    EXPECT_EQ(description, returnsflags);
}

/**
 * @tc.number:  AaFwk_Want_MakeMainAbility_0100
 * @tc.name: MakeMainAbility
 * @tc.desc: Verify MakeMainAbility.
 */
HWTEST_F(WantBaseTest, AaFwk_Want_MakeMainAbility_0100, Function | MediumTest | Level1)
{
    ElementName elementName;

    std::string action("action.system.home");
    std::string entity("entity.system.home");

    Want *wantNew = want_->MakeMainAbility(elementName);
    if (wantNew != nullptr) {
        std::vector<std::string> entities = wantNew->GetEntities();

        EXPECT_EQ((size_t)1, entities.size());
        if (entities.size() > 0) {
            EXPECT_EQ(entity, entities.at(0));
        }
        EXPECT_EQ(action, wantNew->GetAction());
        EXPECT_EQ(elementName, wantNew->GetElement());

        delete wantNew;
        wantNew = nullptr;
    }
}

/**
 * @tc.number:  AaFwk_Want_ClearWant_0100
 * @tc.name: ClearWant
 * @tc.desc: Verify ClearWant.
 */
HWTEST_F(WantBaseTest, AaFwk_Want_ClearWant_0100, Function | MediumTest | Level1)
{
    Want want;
    ElementName elementName;
    std::string empty = "";
    want_->ClearWant(&want);

    EXPECT_EQ((uint)0, want_->GetFlags());
    EXPECT_EQ(empty, want_->GetType());
    EXPECT_EQ(empty, want_->GetAction());
    EXPECT_EQ(elementName, want_->GetElement());
    EXPECT_EQ((size_t)0, want_->GetEntities().size());
    EXPECT_EQ(0, want_->CountEntities());
}

/**
 * @tc.number:  AaFwk_Want_replaceParams_0100
 * @tc.name: replaceParams(wantParams)
 * @tc.desc: Verify the function when the input string is empty.
 */
HWTEST_F(WantBaseTest, AaFwk_Want_replaceParams_0100, Function | MediumTest | Level1)
{
    WantParams wantParams;
    std::string keyStr = "123";
    std::string valueStr = "123";
    wantParams.SetParam(keyStr, String::Box(valueStr));
    want_->ReplaceParams(wantParams);

    EXPECT_EQ(valueStr, String::Unbox(IString::Query(want_->GetParams().GetParam(keyStr))));
}

/**
 * @tc.number:  AaFwk_Want_setElement_0100
 * @tc.name:setElement / setElementName
 * @tc.desc: Verify the function when the input string is empty.
 */
HWTEST_F(WantBaseTest, AaFwk_Want_setElement_0100, Function | MediumTest | Level1)
{
    std::string valueStr1 = "xxxxx";
    std::string valueStr2 = "uaid";
    std::string valueStr3 = "uaygfi";

    OHOS::AppExecFwk::ElementName elementname1;
    OHOS::AppExecFwk::ElementName elementname2;
    OHOS::AppExecFwk::ElementName elementname3;
    ElementName elementname4;
    elementname1.SetAbilityName(valueStr1);
    elementname2.SetDeviceID(valueStr2);
    elementname3.SetBundleName(valueStr3);
    want_->SetElement(elementname1);
    EXPECT_EQ(valueStr1, want_->GetElement().GetAbilityName());

    want_->SetElement(elementname2);
    EXPECT_EQ(valueStr2, want_->GetElement().GetDeviceID());

    want_->SetElement(elementname3);
    EXPECT_EQ(valueStr3, want_->GetElement().GetBundleName());

    want_->SetElementName(valueStr3, valueStr1);
    EXPECT_EQ(valueStr1, want_->GetElement().GetAbilityName());
    EXPECT_EQ(valueStr3, want_->GetElement().GetBundleName());

    want_->SetElementName(valueStr2, valueStr3, valueStr1);
    EXPECT_EQ(valueStr1, want_->GetElement().GetAbilityName());
    EXPECT_EQ(valueStr2, want_->GetElement().GetDeviceID());
    EXPECT_EQ(valueStr3, want_->GetElement().GetBundleName());
}

/**
 * @tc.number:  AaFwk_Want_Action_0200
 * @tc.name:SetAction / GetAction
 * @tc.desc: Verify the function when the input string is empty.
 */
HWTEST_F(WantBaseTest, AaFwk_Want_Action_0200, Function | MediumTest | Level1)
{
    std::string setValue;
    want_->SetAction(setValue);
    EXPECT_EQ(setValue, want_->GetAction());
}

/**
 * @tc.number:  AaFwk_Want_Action_0300
 * @tc.name:SetAction / GetAction
 * @tc.desc: Verify the function when the input string contains special characters.
 */
HWTEST_F(WantBaseTest, AaFwk_Want_Action_0300, Function | MediumTest | Level1)
{
    std::string setValue("action.system.com");
    want_->SetAction(setValue);
    EXPECT_STREQ(setValue.c_str(), want_->GetAction().c_str());
}

using testByteType = std::tuple<std::string, std::string, byte, byte, byte>;
class WantParametersBoolArrayTest : public testing::TestWithParam<testByteType> {
public:
    WantParametersBoolArrayTest()
    {
        want_ = nullptr;
    }
    ~WantParametersBoolArrayTest()
    {
        want_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    std::shared_ptr<Want> want_;
};

void WantParametersBoolArrayTest::SetUpTestCase(void)
{}

void WantParametersBoolArrayTest::TearDownTestCase(void)
{}

void WantParametersBoolArrayTest::SetUp(void)
{
    want_ = std::make_shared<Want>();
}

void WantParametersBoolArrayTest::TearDown(void)
{}

/**
 * @tc.number:  AaFwk_Want_BoolArray_0100
 * @tc.name:SetBoolArrayParam/GetBoolArrayParam
 * @tc.desc: Verify when parameter change.
 */
HWTEST_P(WantParametersBoolArrayTest, AaFwk_Want_BoolArray_0100, Function | MediumTest | Level1)
{
    std::string setKey = std::get<0>(GetParam());
    std::string getKey = std::get<1>(GetParam());
    byte setValue = std::get<2>(GetParam());
    byte defaultValue = std::get<3>(GetParam());
    byte result = std::get<4>(GetParam());
    want_->SetParam(setKey, setValue);
    EXPECT_EQ(result, want_->GetByteParam(getKey, defaultValue));
}

INSTANTIATE_TEST_CASE_P(WantParametersBoolArrayTestCaseP, WantParametersBoolArrayTest,
    testing::Values(testByteType("", "aa", '#', 'U', 'U'), testByteType("", "", 'N', 'K', 'N'),
        testByteType("1*中_aR", "aa", 'a', '%', '%'), testByteType("1*中_aR", "1*中_aR", 'a', 'z', 'a')));

using testBoolArrayType = std::tuple<std::string, std::string, std::vector<bool>, std::vector<bool>, std::vector<bool>>;
class WantBoolArrayParamTest : public testing::TestWithParam<testBoolArrayType> {
public:
    WantBoolArrayParamTest()
    {
        want_ = nullptr;
    }
    ~WantBoolArrayParamTest()
    {
        want_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    std::shared_ptr<Want> want_;
};

void WantBoolArrayParamTest::SetUpTestCase(void)
{}

void WantBoolArrayParamTest::TearDownTestCase(void)
{}

void WantBoolArrayParamTest::SetUp(void)
{
    want_ = std::make_shared<Want>();
}

void WantBoolArrayParamTest::TearDown(void)
{}

/**
 * @tc.number:  AaFwk_Want_BoolArray_0200
 * @tc.name:SetBoolArrayParam/GetBoolArrayParam
 * @tc.desc: Verify when parameter change.
 */
HWTEST_P(WantBoolArrayParamTest, AaFwk_Want_BoolArray_0200, Function | MediumTest | Level1)
{
    std::string setKey = std::get<0>(GetParam());
    std::string getKey = std::get<1>(GetParam());
    std::vector<bool> setValue = std::get<2>(GetParam());
    std::vector<bool> defaultValue = std::get<3>(GetParam());
    std::vector<bool> result = std::get<4>(GetParam());
    want_->SetParam(setKey, setValue);
    EXPECT_EQ(result, want_->GetBoolArrayParam(getKey));
}

INSTANTIATE_TEST_CASE_P(WantBoolArrayParamTestCaseP, WantBoolArrayParamTest,
    testing::Values(testBoolArrayType("", "aa", {true, false}, {}, {}),
        testBoolArrayType("", "", {true, false}, {}, {true, false}),
        testBoolArrayType("1*中_aR", "aa", {true, false}, {}, {}),
        testBoolArrayType("1*中_aR", "1*中_aR", {false, true}, {}, {false, true})));

using testCharArrayType =
    std::tuple<std::string, std::string, std::vector<zchar>, std::vector<zchar>, std::vector<zchar>>;
class WantCharArrayParamTest : public testing::TestWithParam<testCharArrayType> {
public:
    WantCharArrayParamTest()
    {
        want_ = nullptr;
    }
    ~WantCharArrayParamTest()
    {
        want_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    std::shared_ptr<Want> want_;
};

void WantCharArrayParamTest::SetUpTestCase(void)
{}

void WantCharArrayParamTest::TearDownTestCase(void)
{}

void WantCharArrayParamTest::SetUp(void)
{
    want_ = std::make_shared<Want>();
}

void WantCharArrayParamTest::TearDown(void)
{}

/**
 * @tc.number:  AaFwk_Want_Parameters_CharArray_0100
 * @tc.name: SetParam/GetCharArrayParam
 * @tc.desc: Verify when parameter change.
 */
HWTEST_P(WantCharArrayParamTest, AaFwk_Want_Parameters_CharArray_0100, Function | MediumTest | Level1)
{
    std::string setKey = std::get<0>(GetParam());
    std::string getKey = std::get<1>(GetParam());
    std::vector<zchar> setValue = std::get<2>(GetParam());
    std::vector<zchar> defaultValue = std::get<3>(GetParam());
    std::vector<zchar> result = std::get<4>(GetParam());
    want_->SetParam(setKey, setValue);
    EXPECT_EQ(result, want_->GetCharArrayParam(getKey));
}

INSTANTIATE_TEST_CASE_P(WantCharArrayParamTestCaseP, WantCharArrayParamTest,
    testing::Values(testCharArrayType("", "aa", {U'中', U'文'}, {}, {}),
        testCharArrayType("", "", {U'中', U'文'}, {}, {U'中', U'文'}),
        testCharArrayType("1*中_aR", "aa", {U'中', U'文'}, {}, {}),
        testCharArrayType("1*中_aR", "1*中_aR", {U'中', U'文'}, {}, {U'中', U'文'})));

/**
 * @tc.number:  AaFwk_Want_Parameters_CharArray_0200
 * @tc.name:  GetCharArrayParam
 * @tc.desc: Verify when the value is char array.
 */
HWTEST_F(WantCharArrayParamTest, AaFwk_Want_Parameters_CharArray_0200, Function | MediumTest | Level1)
{
    std::vector<zchar> defaultValue;
    std::string getKey("aa");
    EXPECT_EQ(defaultValue, want_->GetCharArrayParam(getKey));
}

/**
 * @tc.number:  AaFwk_Want_Parameters_CharArray_0300
 * @tc.name:  SetParam/GetCharArrayParam
 * @tc.desc: Verify when the value is char array.
 */
HWTEST_F(WantCharArrayParamTest, AaFwk_Want_Parameters_CharArray_0300, Function | MediumTest | Level1)
{
    std::string emptyStr("");
    std::vector<zchar> firstValue({U'中', U'文'});
    std::vector<zchar> secondValue({U'字', U'符'});
    std::vector<zchar> thirdValue({U'集', U'英'});
    std::string keyStr("aa");
    want_->SetParam(emptyStr, firstValue);
    want_->SetParam(emptyStr, firstValue);
    want_->SetParam(emptyStr, secondValue);
    std::vector<zchar> defaultValue;
    EXPECT_EQ(defaultValue, want_->GetCharArrayParam(keyStr));
    want_->SetParam(emptyStr, thirdValue);
    EXPECT_EQ(thirdValue, want_->GetCharArrayParam(emptyStr));
}

/**
 * @tc.number: AaFwk_Want_Parameters_CharArray_0400
 * @tc.name:  SetParam/GetCharArrayParam
 * @tc.desc: Verify when the value is char array.
 */
HWTEST_F(WantCharArrayParamTest, AaFwk_Want_Parameters_CharArray_0400, Function | MediumTest | Level1)
{
    std::string firstKey("%1uH3");
    std::vector<zchar> firstValue({U'中', U'文'});
    std::vector<zchar> secondValue({U'字', U'符'});
    std::vector<zchar> defaultValue;
    std::string secondKey("aa");
    want_->SetParam(firstKey, firstValue);
    want_->SetParam(firstKey, firstValue);
    want_->SetParam(firstKey, secondValue);
    EXPECT_EQ(secondValue, want_->GetCharArrayParam(firstKey));
    want_->SetParam(firstKey, firstValue);
    EXPECT_EQ(defaultValue, want_->GetCharArrayParam(secondKey));
}

using testCharType = std::tuple<std::string, std::string, zchar, zchar, zchar>;
class WantCharParamTest : public testing::TestWithParam<testCharType> {
public:
    WantCharParamTest()
    {
        want_ = nullptr;
    }
    ~WantCharParamTest()
    {
        want_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    std::shared_ptr<Want> want_;
};

void WantCharParamTest::SetUpTestCase(void)
{}

void WantCharParamTest::TearDownTestCase(void)
{}

void WantCharParamTest::SetUp(void)
{
    want_ = std::make_shared<Want>();
}

void WantCharParamTest::TearDown(void)
{}

/**
 * @tc.number: AaFwk_Want_Parameters_Char_0100
 * @tc.name:  SetParam/GetCharParam
 * @tc.desc: Verify when the value is char array.
 */
HWTEST_P(WantCharParamTest, AaFwk_Want_Parameters_Char_0100, Function | MediumTest | Level1)
{
    std::string setKey = std::get<0>(GetParam());
    std::string getKey = std::get<1>(GetParam());
    zchar setValue = std::get<2>(GetParam());
    zchar defaultValue = std::get<3>(GetParam());
    zchar result = std::get<4>(GetParam());
    want_->SetParam(setKey, setValue);
    EXPECT_EQ(result, want_->GetCharParam(getKey, defaultValue));
}

INSTANTIATE_TEST_CASE_P(WantParametersCharTestCaseP, WantCharParamTest,
    testing::Values(testCharType("", "aa", U'#', U'中', U'中'), testCharType("", "", U'中', U'K', U'中'),
        testCharType("1*中_aR", "aa", U'a', U'中', U'中'), testCharType("1*中_aR", "1*中_aR", U'中', U'z', U'中')));

/**
 * @tc.number: AaFwk_Want_Parameters_Char_0200
 * @tc.name:  SetParam/GetCharParam
 * @tc.desc: Verify when the value is char
 */
HWTEST_F(WantCharParamTest, AaFwk_Want_Parameters_Char_0200, Function | MediumTest | Level1)
{
    zchar defaultValue = U'文';
    std::string getKey("aa");
    EXPECT_EQ(defaultValue, want_->GetCharParam(getKey, defaultValue));
}

/**
 * @tc.number: AaFwk_Want_Parameters_Char_0300
 * @tc.name:  SetParam/GetCharParam
 * @tc.desc: Verify when the value is char.
 */
HWTEST_F(WantCharParamTest, AaFwk_Want_Parameters_Char_0300, Function | MediumTest | Level1)
{
    std::string emptyStr("");
    zchar firstValue = U'中';
    zchar secondValue = U'文';
    zchar thirdValue = U'字';
    zchar firstDefaultValue = U'符';
    zchar secondDefaultValue = U'集';
    std::string keyStr("aa");
    want_->SetParam(emptyStr, firstValue);
    want_->SetParam(emptyStr, firstValue);
    want_->SetParam(emptyStr, secondValue);
    EXPECT_EQ(firstDefaultValue, want_->GetCharParam(keyStr, firstDefaultValue));
    want_->SetParam(emptyStr, thirdValue);
    EXPECT_EQ(thirdValue, want_->GetCharParam(emptyStr, secondDefaultValue));
}

/**
 * @tc.number: AaFwk_Want_Parameters_Char_0400
 * @tc.name:  SetParam/GetCharParam
 * @tc.desc: Verify when the value is char.
 */
HWTEST_F(WantCharParamTest, AaFwk_Want_Parameters_Char_0400, Function | MediumTest | Level1)
{
    std::string firstKey("%1uH3");
    zchar firstValue = U'中';
    zchar secondValue = U'文';
    zchar firstDefaultValue = U'字';
    zchar secondDefaultValue = U'符';
    std::string secondKey("aa");
    want_->SetParam(firstKey, firstValue);
    want_->SetParam(firstKey, firstValue);
    want_->SetParam(firstKey, secondValue);
    EXPECT_EQ(secondValue, want_->GetCharParam(firstKey, firstDefaultValue));
    want_->SetParam(firstKey, firstValue);
    EXPECT_EQ(secondDefaultValue, want_->GetCharParam(secondKey, secondDefaultValue));
}

using testDoubleArrayType =
    std::tuple<std::string, std::string, std::vector<double>, std::vector<double>, std::vector<double>>;
class WantDoubleArrayParamTest : public testing::TestWithParam<testDoubleArrayType> {
public:
    WantDoubleArrayParamTest()
    {
        want_ = nullptr;
    }
    ~WantDoubleArrayParamTest()
    {
        want_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    std::shared_ptr<Want> want_;
};

void WantDoubleArrayParamTest::SetUpTestCase(void)
{}

void WantDoubleArrayParamTest::TearDownTestCase(void)
{}

void WantDoubleArrayParamTest::SetUp(void)
{
    want_ = std::make_shared<Want>();
}

void WantDoubleArrayParamTest::TearDown(void)
{}

/**
 * @tc.number: AaFwk_Want_DoubleArray_0100
 * @tc.name:  SetParam/GetDoubleArrayParam
 * @tc.desc: Verify when parameter change.
 */
HWTEST_P(WantDoubleArrayParamTest, AaFwk_Want_DoubleArray_0100, Function | MediumTest | Level1)
{
    std::string setKey = std::get<0>(GetParam());
    std::string getKey = std::get<1>(GetParam());
    std::vector<double> setValue = std::get<2>(GetParam());
    std::vector<double> defaultValue = std::get<3>(GetParam());
    std::vector<double> result = std::get<4>(GetParam());
    want_->SetParam(setKey, setValue);
    EXPECT_EQ(result, want_->GetDoubleArrayParam(getKey));
}

INSTANTIATE_TEST_CASE_P(WantDoubleArrayParamTestCaseP, WantDoubleArrayParamTest,
    testing::Values(testDoubleArrayType("", "aa", {-1.1, -2.1}, {}, {}),
        testDoubleArrayType("", "", {-41.1, -42.1}, {}, {-41.1, -42.1}),
        testDoubleArrayType("1*中_aR", "aa", {50.1, 51.1}, {}, {}),
        testDoubleArrayType("1*中_aR", "1*中_aR", {5000.1, 5001.1}, {}, {5000.1, 5001.1})));

/**
 * @tc.number: AaFwk_Want_DoubleArray_0200
 * @tc.name:  SetParam/GetDoubleArrayParam
 * @tc.desc: Verify when parameter change.
 */
HWTEST_F(WantDoubleArrayParamTest, AaFwk_Want_DoubleArray_0200, Function | MediumTest | Level1)
{
    std::vector<double> defaultValue;
    std::string key = "aa";
    EXPECT_EQ(defaultValue, want_->GetDoubleArrayParam(key));
}

/**
 * @tc.number: AaFwk_Want_DoubleArray_0300
 * @tc.name:  SetParam/GetDoubleArrayParam
 * @tc.desc: set empty-string key repeatedly, but get param of another nonexistent key
 */
HWTEST_F(WantDoubleArrayParamTest, AaFwk_Want_DoubleArray_0300, Function | MediumTest | Level1)
{
    std::vector<double> defaultValue;
    std::string setKey1 = "";
    std::string setKey2 = "aa";
    std::vector<double> setValue1 = {1.1, 2.1};
    std::vector<double> setValue2 = {5.1, 6.1};
    want_->SetParam(setKey1, setValue1);
    want_->SetParam(setKey1, setValue1);
    setValue1 = {2.1, 3.1};
    want_->SetParam(setKey1, setValue1);
    EXPECT_EQ(defaultValue, want_->GetDoubleArrayParam(setKey2));
    setValue1 = {4.1, 5.1};
    want_->SetParam(setKey1, setValue1);
    EXPECT_EQ(setValue1, want_->GetDoubleArrayParam(setKey1));
}

/**
 * @tc.number: AaFwk_Want_DoubleArray_0400
 * @tc.name:  SetParam/GetDoubleArrayParam
 * @tc.desc: set empty-string key repeatedly, then get param of the key
 */
HWTEST_F(WantDoubleArrayParamTest, AaFwk_Want_DoubleArray_0400, Function | MediumTest | Level1)
{
    std::vector<double> defaultValue;
    std::string setKey1 = "%1uH3";
    std::string setKey2 = "aa";
    std::vector<double> setValue1 = {-1.1, -2.1};
    std::vector<double> setValue2 = {9.1, 10.1};
    want_->SetParam(setKey1, setValue1);
    want_->SetParam(setKey1, setValue1);
    setValue1 = {0.1, 1.1};
    want_->SetParam(setKey1, setValue1);
    EXPECT_EQ(setValue1, want_->GetDoubleArrayParam(setKey1));
    setValue1 = {4.1, 5.1};
    want_->SetParam(setKey1, setValue1);
    setValue1 = {-10.1, -11.1};
    EXPECT_EQ(defaultValue, want_->GetDoubleArrayParam(setKey2));
}

using testFloatArrayType =
    std::tuple<std::string, std::string, std::vector<float>, std::vector<float>, std::vector<float>>;
class WantFloatArrayParamTest : public testing::TestWithParam<testFloatArrayType> {
public:
    WantFloatArrayParamTest()
    {
        want_ = nullptr;
    }
    ~WantFloatArrayParamTest()
    {
        want_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    std::shared_ptr<Want> want_;
};

void WantFloatArrayParamTest::SetUpTestCase(void)
{}

void WantFloatArrayParamTest::TearDownTestCase(void)
{}

void WantFloatArrayParamTest::SetUp(void)
{
    want_ = std::make_shared<Want>();
}

void WantFloatArrayParamTest::TearDown(void)
{}

/**
 * @tc.number: AaFwk_Want_FloatArray_0100
 * @tc.name:  SetParam/GetFloatArrayParam
 * @tc.desc: Verify when parameter change.
 */
HWTEST_P(WantFloatArrayParamTest, AaFwk_Want_FloatArray_0100, Function | MediumTest | Level1)
{
    std::string setKey = std::get<0>(GetParam());
    std::string getKey = std::get<1>(GetParam());
    std::vector<float> setValue = std::get<2>(GetParam());
    std::vector<float> defaultValue = std::get<3>(GetParam());
    std::vector<float> result = std::get<4>(GetParam());
    want_->SetParam(setKey, setValue);
    EXPECT_EQ(result, want_->GetFloatArrayParam(getKey));
}

INSTANTIATE_TEST_CASE_P(WantFloatArrayParamTestCaseP, WantFloatArrayParamTest,
    testing::Values(testFloatArrayType("", "aa", {-1.1, -2.1}, {}, {}),
        testFloatArrayType("", "", {-41.1, -42.1}, {}, {-41.1, -42.1}),
        testFloatArrayType("1*中_aR", "aa", {50.1, 51.1}, {}, {}),
        testFloatArrayType("1*中_aR", "1*中_aR", {5000.1, 5001.1}, {}, {5000.1, 5001.1})));

/**
 * @tc.number: AaFwk_Want_FloatArray_0200
 * @tc.name:  SetParam/GetFloatArrayParam
 * @tc.desc: get param when WantParam is empty
 */
HWTEST_F(WantFloatArrayParamTest, AaFwk_Want_FloatArray_0200, Function | MediumTest | Level1)
{
    std::vector<float> defaultValue;
    std::string key = "aa";
    EXPECT_EQ(defaultValue, want_->GetFloatArrayParam(key));
}

/**
 * @tc.number: AaFwk_Want_FloatArray_0300
 * @tc.name:  SetParam & GetFloatArrayParam
 * @tc.desc: set empty-string key repeatedly, but get param of another nonexistent key
 */
HWTEST_F(WantFloatArrayParamTest, AaFwk_Want_FloatArray_0300, Function | MediumTest | Level1)
{
    std::vector<float> defaultValue;
    std::string setKey1 = "";
    std::string setKey2 = "aa";
    std::vector<float> setValue1 = {1.1, 2.1};
    want_->SetParam(setKey1, setValue1);
    want_->SetParam(setKey1, setValue1);
    setValue1 = {2.1, 3.1};
    want_->SetParam(setKey1, setValue1);
    EXPECT_EQ(defaultValue, want_->GetFloatArrayParam(setKey2));
    setValue1 = {4.1, 5.1};
    want_->SetParam(setKey1, setValue1);
    EXPECT_EQ(setValue1, want_->GetFloatArrayParam(setKey1));
}

/**
 * @tc.number: AaFwk_Want_FloatArray_0400
 * @tc.name:  SetParam & GetFloatArrayParam
 * @tc.desc: set empty-string key repeatedly, then get param of the key
 */
HWTEST_F(WantFloatArrayParamTest, AaFwk_Want_FloatArray_0400, Function | MediumTest | Level1)
{
    std::vector<float> defaultValue;
    std::string setKey1 = "%1uH3";
    std::string setKey2 = "aa";
    std::vector<float> setValue1 = {-1.1, -2.1};
    std::vector<float> setValue2 = {9.1, 10.1};
    want_->SetParam(setKey1, setValue1);
    want_->SetParam(setKey1, setValue1);
    setValue1 = {0.1, 1.1};
    want_->SetParam(setKey1, setValue1);
    EXPECT_EQ(setValue1, want_->GetFloatArrayParam(setKey1));
    setValue1 = {4.1, 5.1};
    want_->SetParam(setKey1, setValue1);
    EXPECT_EQ(defaultValue, want_->GetFloatArrayParam(setKey2));
}

using testLongArrayType = std::tuple<std::string, std::string, std::vector<long>, std::vector<long>, std::vector<long>>;
class WantLongArrayParamTest : public testing::TestWithParam<testLongArrayType> {
public:
    WantLongArrayParamTest()
    {
        want_ = nullptr;
    }
    ~WantLongArrayParamTest()
    {
        want_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    std::shared_ptr<Want> want_;
};

void WantLongArrayParamTest::SetUpTestCase(void)
{}

void WantLongArrayParamTest::TearDownTestCase(void)
{}

void WantLongArrayParamTest::SetUp(void)
{
    want_ = std::make_shared<Want>();
}

void WantLongArrayParamTest::TearDown(void)
{}

/**
 * @tc.number: AaFwk_Want_LongArray_0100
 * @tc.name:  SetParam & GetLongArrayParam
 * @tc.desc: Verify when parameter change.
 */
HWTEST_P(WantLongArrayParamTest, AaFwk_Want_LongArray_0100, Function | MediumTest | Level1)
{
    std::string setKey = std::get<0>(GetParam());
    std::string getKey = std::get<1>(GetParam());
    std::vector<long> setValue = std::get<2>(GetParam());
    std::vector<long> defaultValue = std::get<3>(GetParam());
    std::vector<long> result = std::get<4>(GetParam());
    want_->SetParam(setKey, setValue);
    EXPECT_EQ(result, want_->GetLongArrayParam(getKey));
}

INSTANTIATE_TEST_CASE_P(WantLongArrayParamTestCaseP, WantLongArrayParamTest,
    testing::Values(testLongArrayType("", "aa", {-1, 3, 25, -9}, {}, {}),
        testLongArrayType("", "", {-41, 0, 0, 9}, {}, {-41, 0, 0, 9}),
        testLongArrayType("1*中_aR", "aa", {50, 2, -9}, {}, {}),
        testLongArrayType("1*中_aR", "1*中_aR", {-5000}, {}, {-5000})));

/**
 * @tc.number: AaFwk_Want_LongArray_0200
 * @tc.name:  SetParam & GetLongArrayParam
 * @tc.desc: get param when WantParam is empty
 */
HWTEST_F(WantLongArrayParamTest, AaFwk_Want_LongArray_0200, Function | MediumTest | Level1)
{
    std::vector<long> defaultValue;
    std::string key = "aa";
    EXPECT_EQ(defaultValue, want_->GetLongArrayParam(key));
}

/**
 * @tc.number: AaFwk_Want_LongArray_0300
 * @tc.name:  SetParam & GetLongArrayParam
 * @tc.desc: set empty-string key repeatedly, but get param of another nonexistent key
 */
HWTEST_F(WantLongArrayParamTest, AaFwk_Want_LongArray_0300, Function | MediumTest | Level1)
{
    std::vector<long> defaultValue;
    std::string setKey1 = "";
    std::string setKey2 = "aa";
    std::vector<long> setValue1 = {1, 2};
    want_->SetParam(setKey1, setValue1);
    want_->SetParam(setKey1, setValue1);
    setValue1 = {2, 3};
    want_->SetParam(setKey1, setValue1);
    EXPECT_EQ(defaultValue, want_->GetLongArrayParam(setKey2));
    setValue1 = {4, 5};
    want_->SetParam(setKey1, setValue1);
    EXPECT_EQ(setValue1, want_->GetLongArrayParam(setKey1));
}

/**
 * @tc.number: AaFwk_Want_LongArray_0400
 * @tc.name:  SetParam & GetLongArrayParam
 * @tc.desc: set empty-string key repeatedly, then get param of the key
 */
HWTEST_F(WantLongArrayParamTest, AaFwk_Want_LongArray_0400, Function | MediumTest | Level1)
{
    std::vector<long> defaultValue;
    std::string setKey1 = "%1uH3";
    std::string setKey2 = "aa";
    std::vector<long> setValue1 = {-1, -2};
    want_->SetParam(setKey1, setValue1);
    want_->SetParam(setKey1, setValue1);
    setValue1 = {0, 1};
    want_->SetParam(setKey1, setValue1);
    EXPECT_EQ(setValue1, want_->GetLongArrayParam(setKey1));
    setValue1 = {4, 5};
    want_->SetParam(setKey1, setValue1);
    EXPECT_EQ(defaultValue, want_->GetLongArrayParam(setKey2));
}

using testShortArrayType =
    std::tuple<std::string, std::string, std::vector<short>, std::vector<short>, std::vector<short>>;
class WantShortArrayParamTest : public testing::TestWithParam<testShortArrayType> {
public:
    WantShortArrayParamTest()
    {
        want_ = nullptr;
    }
    ~WantShortArrayParamTest()
    {
        want_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    std::shared_ptr<Want> want_;
};

void WantShortArrayParamTest::SetUpTestCase(void)
{}

void WantShortArrayParamTest::TearDownTestCase(void)
{}

void WantShortArrayParamTest::SetUp(void)
{
    want_ = std::make_shared<Want>();
}

void WantShortArrayParamTest::TearDown(void)
{}

/**
 * @tc.number: AaFwk_Want_ShortArray_0100
 * @tc.name:  SetParam/GetShortArrayParam
 * @tc.desc: Verify when parameter change.
 */
HWTEST_P(WantShortArrayParamTest, AaFwk_Want_ShortArray_0100, Function | MediumTest | Level1)
{
    std::string setKey = std::get<0>(GetParam());
    std::string getKey = std::get<1>(GetParam());
    std::vector<short> setValue = std::get<2>(GetParam());
    std::vector<short> defaultValue = std::get<3>(GetParam());
    std::vector<short> result = std::get<4>(GetParam());
    want_->SetParam(setKey, setValue);
    EXPECT_EQ(result, want_->GetShortArrayParam(getKey));
}

INSTANTIATE_TEST_CASE_P(WantShortArrayParamTestCaseP, WantShortArrayParamTest,
    testing::Values(testShortArrayType("", "aa", {-1, 3, 25, -9}, {}, {}),
        testShortArrayType("", "", {-41, 0, 0, 9}, {}, {-41, 0, 0, 9}),
        testShortArrayType("1*中_aR", "aa", {50, 2, -9}, {}, {}),
        testShortArrayType("1*中_aR", "1*中_aR", {-5000}, {}, {-5000})));

/**
 * @tc.number: AaFwk_Want_ShortArray_0200
 * @tc.name:  SetParam/GetShortArrayParam
 * @tc.desc: Verify when the value is short array
 */
HWTEST_F(WantShortArrayParamTest, AaFwk_Want_ShortArray_0200, Function | MediumTest | Level1)
{
    std::vector<short> defaultValue;
    std::string getKey("aa");
    EXPECT_EQ(defaultValue, want_->GetShortArrayParam(getKey));
}

/**
 * @tc.number: AaFwk_Want_ShortArray_0300
 * @tc.name:  SetParam/GetShortArrayParam
 * @tc.desc: Verify when the value is short array
 */
HWTEST_F(WantShortArrayParamTest, AaFwk_Want_ShortArray_0300, Function | MediumTest | Level1)
{
    std::string emptyStr("");
    std::vector<short> firstValue({1, 4, -9});
    std::vector<short> secondValue({1, 8, -9});
    std::vector<short> thirdValue({1, 4, 9});
    std::string keyStr("aa");
    want_->SetParam(emptyStr, firstValue);
    want_->SetParam(emptyStr, firstValue);
    want_->SetParam(emptyStr, secondValue);
    std::vector<short> defaultValue;
    EXPECT_EQ(defaultValue, want_->GetShortArrayParam(keyStr));
    want_->SetParam(emptyStr, thirdValue);
    EXPECT_EQ(thirdValue, want_->GetShortArrayParam(emptyStr));
}

/**
 * @tc.number: AaFwk_Want_ShortArray_0400
 * @tc.name:  SetParam/GetShortArrayParam
 * @tc.desc: Verify when the value is short array
 */
HWTEST_F(WantShortArrayParamTest, AaFwk_Want_ShortArray_0400, Function | MediumTest | Level1)
{
    std::string firstKey("%1uH3");
    std::vector<short> firstValue({-1, -2});
    std::vector<short> secondValue({-1, -2, -1, -2, 0});
    std::vector<short> thirdValue({-1, -2, 100});
    std::string secondKey("aa");
    want_->SetParam(firstKey, firstValue);
    want_->SetParam(firstKey, firstValue);
    want_->SetParam(firstKey, secondValue);
    EXPECT_EQ(secondValue, want_->GetShortArrayParam(firstKey));
    want_->SetParam(firstKey, thirdValue);
    std::vector<short> defaultValue;
    EXPECT_EQ(defaultValue, want_->GetShortArrayParam(secondKey));
}

using testShortType = std::tuple<std::string, std::string, short, short, short>;
class WantShortParamTest : public testing::TestWithParam<testShortType> {
public:
    WantShortParamTest()
    {
        want_ = nullptr;
    }
    ~WantShortParamTest()
    {
        want_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    std::shared_ptr<Want> want_;
};

void WantShortParamTest::SetUpTestCase(void)
{}

void WantShortParamTest::TearDownTestCase(void)
{}

void WantShortParamTest::SetUp(void)
{
    want_ = std::make_shared<Want>();
}

void WantShortParamTest::TearDown(void)
{}

/**
 * @tc.number: AaFwk_Want_Short_0100
 * @tc.name:  SetParam/GetShortParam
 * @tc.desc: Verify when parameter change.
 */
HWTEST_P(WantShortParamTest, AaFwk_Want_Short_0100, Function | MediumTest | Level1)
{
    std::string setKey = std::get<0>(GetParam());
    std::string getKey = std::get<1>(GetParam());
    short setValue = std::get<2>(GetParam());
    short defaultValue = std::get<3>(GetParam());
    short result = std::get<4>(GetParam());
    want_->SetParam(setKey, setValue);
    EXPECT_EQ(result, want_->GetShortParam(getKey, defaultValue));
}

INSTANTIATE_TEST_CASE_P(WantShortParamTestCaseP, WantShortParamTest,
    testing::Values(testShortType("", "aa", -1, 100, 100), testShortType("", "", -9, -41, -9),
        testShortType("1*中_aR", "aa", 50, 5, 5), testShortType("1*中_aR", "1*中_aR", -5000, 5000, -5000)));

/**
 * @tc.number: AaFwk_Want_Short_0200
 * @tc.name:  SetParam/GetShortParam
 * @tc.desc: Verify when the value is short
 */
HWTEST_F(WantShortParamTest, AaFwk_Want_Short_0200, Function | MediumTest | Level1)
{
    short defaultValue = 200;
    std::string getKey("aa");
    EXPECT_EQ(defaultValue, want_->GetShortParam(getKey, defaultValue));
}
/**
 * @tc.number: AaFwk_Want_Short_0300
 * @tc.name:  SetParam/GetShortParam
 * @tc.desc: Verify when the value is short
 */
HWTEST_F(WantShortParamTest, AaFwk_Want_Short_0300, Function | MediumTest | Level1)
{
    std::string emptyStr("");
    short firstValue = 1;
    short secondValue = 2;
    short thirdValue = 4;
    short firstDefaultValue = 3;
    short secondDefaultValue = 5;
    std::string keyStr("aa");
    want_->SetParam(emptyStr, firstValue);
    want_->SetParam(emptyStr, firstValue);
    want_->SetParam(emptyStr, secondValue);
    EXPECT_EQ(firstDefaultValue, want_->GetShortParam(keyStr, firstDefaultValue));
    want_->SetParam(emptyStr, thirdValue);
    EXPECT_EQ(thirdValue, want_->GetShortParam(emptyStr, secondDefaultValue));
}

/**
 * @tc.number: AaFwk_Want_Short_0400
 * @tc.name:  SetParam/GetShortParam
 * @tc.desc: Verify when the value is short
 */
HWTEST_F(WantShortParamTest, AaFwk_Want_Short_0400, Function | MediumTest | Level1)
{
    std::string firstKey("%1uH3");
    short firstValue = -1;
    short secondValue = 0;
    short thirdValue = 4;
    short firstDefaultValue = 9;
    short secondDefaultValue = -10;
    std::string secondKey("aa");
    want_->SetParam(firstKey, firstValue);
    want_->SetParam(firstKey, firstValue);
    want_->SetParam(firstKey, secondValue);
    EXPECT_EQ(secondValue, want_->GetShortParam(firstKey, firstDefaultValue));
    want_->SetParam(firstKey, thirdValue);
    EXPECT_EQ(secondDefaultValue, want_->GetShortParam(secondKey, secondDefaultValue));
}

using testStrArrayType =
    std::tuple<std::string, std::string, std::vector<std::string>, std::vector<std::string>, std::vector<std::string>>;
class WantStringArrayParamTest : public testing::TestWithParam<testStrArrayType> {
public:
    WantStringArrayParamTest()
    {
        want_ = nullptr;
    }
    ~WantStringArrayParamTest()
    {
        want_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    std::shared_ptr<Want> want_;
};

void WantStringArrayParamTest::SetUpTestCase(void)
{}

void WantStringArrayParamTest::TearDownTestCase(void)
{}

void WantStringArrayParamTest::SetUp(void)
{
    want_ = std::make_shared<Want>();
}

void WantStringArrayParamTest::TearDown(void)
{}

/**
 * @tc.number: AaFwk_Want_StringArray_0100
 * @tc.name:  SetParam/GetStringArrayParam
 * @tc.desc: Verify when parameter change.
 */
HWTEST_P(WantStringArrayParamTest, AaFwk_Want_StringArray_0100, Function | MediumTest | Level1)
{
    std::string setKey = std::get<0>(GetParam());
    std::string getKey = std::get<1>(GetParam());
    std::vector<std::string> setValue = std::get<2>(GetParam());
    std::vector<std::string> defaultValue = std::get<3>(GetParam());
    std::vector<std::string> result = std::get<4>(GetParam());
    want_->SetParam(setKey, setValue);
    EXPECT_EQ(result, want_->GetStringArrayParam(getKey));
}

INSTANTIATE_TEST_CASE_P(WantStringArrayParamTestCaseP, WantStringArrayParamTest,
    testing::Values(testStrArrayType("", "aa", {"1*中_aR", "dbdb"}, {}, {}),
        testStrArrayType("", "", {"1*中_aR", "dbdb"}, {}, {"1*中_aR", "dbdb"}),
        testStrArrayType("1*中_aR", "aa", {"1*中_aR", "dbdb"}, {}, {}),
        testStrArrayType("1*中_aR", "1*中_aR", {"1*中_aR", "dbdb"}, {}, {"1*中_aR", "dbdb"})));

/**
 * @tc.number: AaFwk_Want_StringArray_0200
 * @tc.name:  SetParam/GetStringArrayParam
 * @tc.desc: get param when WantParam is empty
 */
HWTEST_F(WantStringArrayParamTest, AaFwk_Want_StringArray_0200, Function | MediumTest | Level1)
{
    std::vector<std::string> defaultValue;
    std::string key = "aa";
    std::vector<std::string> resultValue = want_->GetStringArrayParam(key);
    EXPECT_EQ(defaultValue, resultValue);
}

/**
 * @tc.number: AaFwk_Want_StringArray_0300
 * @tc.name:  SetParam/GetStringArrayParam
 * @tc.desc: set empty-string key repeatedly, but get param of another nonexistent key
 */
HWTEST_F(WantStringArrayParamTest, AaFwk_Want_StringArray_0300, Function | MediumTest | Level1)
{
    std::vector<std::string> defaultValue;
    std::vector<std::string> setValue1 = {"aaa", "2132"};
    std::vector<std::string> setValue2 = {"1*中_aR", "dbdb"};
    std::string key1 = "";
    std::string key2 = "aa";
    want_->SetParam(key1, setValue1);
    want_->SetParam(key1, setValue1);
    want_->SetParam(key1, setValue2);
    std::vector<std::string> resultValue = want_->GetStringArrayParam(key2);
    EXPECT_EQ(defaultValue, resultValue);

    want_->SetParam(key1, setValue1);
    resultValue = want_->GetStringArrayParam(key1);
    EXPECT_EQ(setValue1, resultValue);
}

/**
 * @tc.number: AaFwk_Want_StringArray_0400
 * @tc.name:  SetParam/GetStringArrayParam
 * @tc.desc: set empty-string key repeatedly, then get param of the key
 */
HWTEST_F(WantStringArrayParamTest, AaFwk_Want_StringArray_0400, Function | MediumTest | Level1)
{
    std::vector<std::string> defaultValue;
    std::vector<std::string> setValue = {"aaa", "2132"};
    std::string key1 = "%1uH3";
    std::string key2 = "aa";
    want_->SetParam(key1, setValue);
    want_->SetParam(key1, setValue);
    setValue = {"1*中_aR", "3#$%"};
    want_->SetParam(key1, setValue);
    std::vector<std::string> resultValue = want_->GetStringArrayParam(key1);
    EXPECT_EQ(setValue, resultValue);

    setValue = {"aaa", "2132"};
    want_->SetParam(key1, setValue);
    resultValue = want_->GetStringArrayParam(key2);
    EXPECT_EQ(defaultValue, resultValue);
}

using testStrType = std::tuple<std::string, std::string, std::string, std::string, std::string>;
class WantStringParamTest : public testing::TestWithParam<testStrType> {
public:
    WantStringParamTest()
    {
        want_ = nullptr;
    }
    ~WantStringParamTest()
    {
        want_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    std::shared_ptr<Want> want_;
};

void WantStringParamTest::SetUpTestCase(void)
{}

void WantStringParamTest::TearDownTestCase(void)
{}

void WantStringParamTest::SetUp(void)
{
    want_ = std::make_shared<Want>();
}

void WantStringParamTest::TearDown(void)
{}

/**
 * @tc.number: AaFwk_Want_String_0100
 * @tc.name:  SetParam/GetStringParam
 * @tc.desc: Verify when parameter change.
 */
HWTEST_P(WantStringParamTest, AaFwk_Want_String_0100, Function | MediumTest | Level1)
{
    std::string setKey = std::get<0>(GetParam());
    std::string getKey = std::get<1>(GetParam());
    std::string setValue = std::get<2>(GetParam());
    std::string defaultValue = std::get<3>(GetParam());
    std::string result = std::get<4>(GetParam());
    want_->SetParam(setKey, setValue);
    EXPECT_EQ(result, want_->GetStringParam(getKey));
}

INSTANTIATE_TEST_CASE_P(WantStringParamTestCaseP, WantStringParamTest,
    testing::Values(testStrType("", "aa", "1*中_aR", "", ""), testStrType("", "", "1*中_aR", "", "1*中_aR"),
        testStrType("1*中_aR", "aa", "aaa", "", ""), testStrType("1*中_aR", "1*中_aR", "aaa", "", "aaa")));

/**
 * @tc.number: AaFwk_Want_String_0200
 * @tc.name:  SetParam/GetStringParam
 * @tc.desc: get param when WantParam is empty.
 */
HWTEST_F(WantStringParamTest, AaFwk_Want_String_0200, Function | MediumTest | Level1)
{
    std::string defaultStrValue;
    std::string key = "aa";
    EXPECT_EQ(defaultStrValue, want_->GetStringParam(key));
}

/**
 * @tc.number: AaFwk_Want_String_0300
 * @tc.name:  SetParam/GetStringParam
 * @tc.desc: set empty-string key repeatedly, but get param of another nonexistent key.
 */
HWTEST_F(WantStringParamTest, AaFwk_Want_String_0300, Function | MediumTest | Level1)
{
    std::string defaultStrValue;
    std::string setValue1 = "aaa";
    std::string setValue2 = "1*中_aR";
    std::string key1 = "";
    std::string key2 = "aa";
    want_->SetParam(key1, setValue1);
    want_->SetParam(key1, setValue1);
    want_->SetParam(key1, setValue2);
    EXPECT_EQ(defaultStrValue, want_->GetStringParam(key2));
    want_->SetParam(key1, setValue1);
    EXPECT_EQ(setValue1, want_->GetStringParam(key1));
}

/**
 * @tc.number: AaFwk_Want_String_0400
 * @tc.name:  SetParam/GetStringParam
 * @tc.desc: set empty-string key repeatedly, then get param of the key.
 */
HWTEST_F(WantStringParamTest, AaFwk_Want_String_0400, Function | MediumTest | Level1)
{
    std::string key1 = "%1uH3";
    std::string defaultStrValue;
    std::string setValue1 = "aaa";
    std::string setValue2 = "1*中_aR";
    std::string key2 = "aa";
    want_->SetParam(key1, setValue1);
    want_->SetParam(key1, setValue1);
    want_->SetParam(key1, setValue2);
    EXPECT_EQ("1*中_aR", want_->GetStringParam(key1));
    want_->SetParam(key1, setValue1);
    EXPECT_EQ(defaultStrValue, want_->GetStringParam(key2));
}

using testLongType = std::tuple<std::string, std::string, long, long, long>;
class WantLongParamTest : public testing::TestWithParam<testLongType> {
public:
    WantLongParamTest()
    {
        want_ = nullptr;
    }
    ~WantLongParamTest()
    {
        want_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    std::shared_ptr<Want> want_;
};

void WantLongParamTest::SetUpTestCase(void)
{}

void WantLongParamTest::TearDownTestCase(void)
{}

void WantLongParamTest::SetUp(void)
{
    want_ = std::make_shared<Want>();
}

void WantLongParamTest::TearDown(void)
{}

/**
 * @tc.number: AaFwk_Want_LongParam_0100
 * @tc.name:  SetParam/GetLongParam
 * @tc.desc: Verify when parameter change.
 */
HWTEST_P(WantLongParamTest, AaFwk_Want_LongParam_0100, Function | MediumTest | Level1)
{
    std::string setKey = std::get<0>(GetParam());
    std::string getKey = std::get<1>(GetParam());
    long setValue = std::get<2>(GetParam());
    long defaultValue = std::get<3>(GetParam());
    long result = std::get<4>(GetParam());
    want_->SetParam(setKey, setValue);
    EXPECT_EQ(result, want_->GetLongParam(getKey, defaultValue));
}

INSTANTIATE_TEST_CASE_P(WantLongParamTestCaseP, WantLongParamTest,
    testing::Values(testLongType("", "aa", -1, 100, 100), testLongType("", "", -9, -41, -9),
        testLongType("1*中_aR", "aa", 50, 5, 5), testLongType("1*中_aR", "1*中_aR", -5000, 5000, -5000)));

/**
 * @tc.number: AaFwk_Want_LongParam_0200
 * @tc.name:  SetParam/GetLongParam
 * @tc.desc:  get param when WantParam is empty.
 */
HWTEST_F(WantLongParamTest, AaFwk_Want_LongParam_0200, Function | MediumTest | Level1)
{
    long defaultValue = 100;
    std::string key = "aa";
    EXPECT_EQ(defaultValue, want_->GetLongParam(key, defaultValue));
}

/**
 * @tc.number: AaFwk_Want_LongParam_0300
 * @tc.name:  SetParam/GetLongParam
 * @tc.desc:  set empty-string key repeatedly, but get param of another nonexistent key.
 */
HWTEST_F(WantLongParamTest, AaFwk_Want_LongParam_0300, Function | MediumTest | Level1)
{
    std::string setKey1 = "";
    std::string setKey2 = "aa";
    long setValue1 = 1;
    long setValue2 = 5;
    want_->SetParam(setKey1, setValue1);
    want_->SetParam(setKey1, setValue1);
    setValue1 = 2;
    want_->SetParam(setKey1, setValue1);
    setValue1 = 3;
    EXPECT_EQ(setValue1, want_->GetLongParam(setKey2, setValue1));
    setValue1 = 4;
    want_->SetParam(setKey1, setValue1);
    EXPECT_EQ(setValue1, want_->GetLongParam(setKey1, setValue2));
}

/**
 * @tc.number: AaFwk_Want_LongParam_0400
 * @tc.name:  SetParam/GetLongParam
 * @tc.desc:  set empty-string key repeatedly, then get param of the key.
 */
HWTEST_F(WantLongParamTest, AaFwk_Want_LongParam_0400, Function | MediumTest | Level1)
{
    std::string setKey1 = "%1uH3";
    std::string setKey2 = "aa";
    long setValue1 = -1;
    long setValue2 = 9;
    want_->SetParam(setKey1, setValue1);
    want_->SetParam(setKey1, setValue1);
    setValue1 = 0;
    want_->SetParam(setKey1, setValue1);
    EXPECT_EQ(setValue1, want_->GetLongParam(setKey1, setValue2));
    setValue1 = 4;
    want_->SetParam(setKey1, setValue1);
    setValue1 = -10;
    EXPECT_EQ(setValue1, want_->GetLongParam(setKey2, setValue1));
}

using testIntType = std::tuple<std::string, std::string, int, int, int>;
class WantIntParamTest : public testing::TestWithParam<testIntType> {
public:
    WantIntParamTest()
    {
        want_ = nullptr;
    }
    ~WantIntParamTest()
    {
        want_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    std::shared_ptr<Want> want_;
};

void WantIntParamTest::SetUpTestCase(void)
{}

void WantIntParamTest::TearDownTestCase(void)
{}

void WantIntParamTest::SetUp(void)
{
    want_ = std::make_shared<Want>();
}

void WantIntParamTest::TearDown(void)
{}

/**
 * @tc.number: AaFwk_Want_IntParam_0100
 * @tc.name:  SetParam/GetIntParam
 * @tc.desc:  Verify when parameter change.
 */
HWTEST_P(WantIntParamTest, AaFwk_Want_IntParam_0100, Function | MediumTest | Level1)
{
    std::string setKey = std::get<0>(GetParam());
    std::string getKey = std::get<1>(GetParam());
    int setValue = std::get<2>(GetParam());
    int defaultValue = std::get<3>(GetParam());
    int result = std::get<4>(GetParam());
    want_->SetParam(setKey, setValue);
    EXPECT_EQ(result, want_->GetIntParam(getKey, defaultValue));
}

INSTANTIATE_TEST_CASE_P(WantParametersIntTestCaseP, WantIntParamTest,
    testing::Values(testIntType("", "aa", -1, 100, 100), testIntType("", "", -9, -41, -9),
        testIntType("1*中_aR", "aa", 50, 5, 5), testIntType("1*中_aR", "1*中_aR", -5000, 5000, -5000)));

/**
 * @tc.number: AaFwk_Want_IntParam_0200
 * @tc.name:  SetParam/GetIntParam
 * @tc.desc:  Verify when the value is integer.
 */
HWTEST_F(WantIntParamTest, AaFwk_Want_IntParam_0200, Function | MediumTest | Level1)
{
    int defaultValue = 200;
    std::string getKey("aa");
    EXPECT_EQ(defaultValue, want_->GetIntParam(getKey, defaultValue));
}

/**
 * @tc.number: AaFwk_Want_IntParam_0300
 * @tc.name:  SetParam/GetIntParam
 * @tc.desc:  Verify when the value is integer.
 */
HWTEST_F(WantIntParamTest, AaFwk_Want_IntParam_0300, Function | MediumTest | Level1)
{
    std::string emptyStr("");
    int firstValue = 1;
    int secondValue = 2;
    int thirdValue = 4;
    int firstDefaultValue = 3;
    int secondDefaultValue = 5;
    std::string keyStr("aa");
    want_->SetParam(emptyStr, firstValue);
    want_->SetParam(emptyStr, firstValue);
    want_->SetParam(emptyStr, secondValue);
    EXPECT_EQ(firstDefaultValue, want_->GetIntParam(keyStr, firstDefaultValue));
    want_->SetParam(emptyStr, thirdValue);
    EXPECT_EQ(thirdValue, want_->GetIntParam(emptyStr, secondDefaultValue));
}

/**
 * @tc.number: AaFwk_Want_IntParam_0400
 * @tc.name:  SetParam/GetIntParam
 * @tc.desc:  Verify when the value is integer.
 */
HWTEST_F(WantIntParamTest, AaFwk_Want_IntParam_0400, Function | MediumTest | Level1)
{
    std::string firstKey("%1uH3");
    int firstValue = -1;
    int secondValue = 0;
    int thirdValue = 4;
    int firstDefaultValue = 9;
    int secondDefaultValue = -10;
    std::string secondKey("aa");
    want_->SetParam(firstKey, firstValue);
    want_->SetParam(firstKey, firstValue);
    want_->SetParam(firstKey, secondValue);
    EXPECT_EQ(secondValue, want_->GetIntParam(firstKey, firstDefaultValue));
    want_->SetParam(firstKey, thirdValue);
    EXPECT_EQ(secondDefaultValue, want_->GetIntParam(secondKey, secondDefaultValue));
}

using testIntArrayType = std::tuple<std::string, std::string, std::vector<int>, std::vector<int>, std::vector<int>>;
class WantIntArrayParamTest : public testing::TestWithParam<testIntArrayType> {
public:
    WantIntArrayParamTest()
    {
        want_ = nullptr;
    }
    ~WantIntArrayParamTest()
    {
        want_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    std::shared_ptr<Want> want_;
};

void WantIntArrayParamTest::SetUpTestCase(void)
{}

void WantIntArrayParamTest::TearDownTestCase(void)
{}

void WantIntArrayParamTest::SetUp(void)
{
    want_ = std::make_shared<Want>();
}

void WantIntArrayParamTest::TearDown(void)
{}

/**
 * @tc.number: AaFwk_Want_IntArrayParam_0100
 * @tc.name:  SetParam/GetIntArrayParam
 * @tc.desc:  Verify when parameter change.
 */
HWTEST_P(WantIntArrayParamTest, AaFwk_Want_IntArrayParam_0100, Function | MediumTest | Level1)
{
    std::string setKey = std::get<0>(GetParam());
    std::string getKey = std::get<1>(GetParam());
    std::vector<int> setValue = std::get<2>(GetParam());
    std::vector<int> defaultValue = std::get<3>(GetParam());
    std::vector<int> result = std::get<4>(GetParam());
    want_->SetParam(setKey, setValue);
    EXPECT_EQ(result, want_->GetIntArrayParam(getKey));
}

INSTANTIATE_TEST_CASE_P(WantIntArrayParamTestCaseP, WantIntArrayParamTest,
    testing::Values(testIntArrayType("", "aa", {-1, 3, 25, -9}, {}, {}),
        testIntArrayType("", "", {-41, 0, 0, 9}, {}, {-41, 0, 0, 9}),
        testIntArrayType("1*中_aR", "aa", {50, 2, -9}, {}, {}),
        testIntArrayType("1*中_aR", "1*中_aR", {-5000}, {}, {-5000})));

/**
 * @tc.number: AaFwk_Want_IntArrayParam_0200
 * @tc.name:  SetParam/GetIntArrayParam
 * @tc.desc:  Verify when the value is integer array.
 */
HWTEST_F(WantIntArrayParamTest, AaFwk_Want_IntArrayParam_0200, Function | MediumTest | Level1)
{
    std::vector<int> defaultValue;
    std::string getKey("aa");
    EXPECT_EQ(defaultValue, want_->GetIntArrayParam(getKey));
}

/**
 * @tc.number: AaFwk_Want_IntArrayParam_0300
 * @tc.name:  SetParam/GetIntArrayParam
 * @tc.desc:  Verify when the value is integer array.
 */
HWTEST_F(WantIntArrayParamTest, AaFwk_Want_IntArrayParam_0300, Function | MediumTest | Level1)
{
    std::string emptyStr("");
    std::vector<int> firstValue({1, 4, -9});
    std::vector<int> secondValue({1, 8, -9});
    std::vector<int> thirdValue({1, 4, 9});
    std::string keyStr("aa");
    want_->SetParam(emptyStr, firstValue);
    want_->SetParam(emptyStr, firstValue);
    want_->SetParam(emptyStr, secondValue);
    std::vector<int> defaultValue;
    EXPECT_EQ(defaultValue, want_->GetIntArrayParam(keyStr));
    want_->SetParam(emptyStr, thirdValue);
    EXPECT_EQ(thirdValue, want_->GetIntArrayParam(emptyStr));
}

/**
 * @tc.number: AaFwk_Want_IntArrayParam_0400
 * @tc.name:  SetParam/GetIntArrayParam
 * @tc.desc:  Verify when the value is integer array.
 */
HWTEST_F(WantIntArrayParamTest, AaFwk_Want_IntArrayParam_0400, Function | MediumTest | Level1)
{
    std::string firstKey("%1uH3");
    std::vector<int> firstValue({-1, -2});
    std::vector<int> secondValue({-1, -2, -1, -2, 0});
    std::vector<int> thirdValue({-1, -2, 100});
    std::string secondKey("aa");
    want_->SetParam(firstKey, firstValue);
    want_->SetParam(firstKey, firstValue);
    want_->SetParam(firstKey, secondValue);
    EXPECT_EQ(secondValue, want_->GetIntArrayParam(firstKey));
    want_->SetParam(firstKey, thirdValue);
    std::vector<int> defaultValue;
    EXPECT_EQ(defaultValue, want_->GetIntArrayParam(secondKey));
}

using testFloatType = std::tuple<std::string, std::string, float, float, float>;
class WantFloatParamTest : public testing::TestWithParam<testFloatType> {
public:
    WantFloatParamTest()
    {
        want_ = nullptr;
    }
    ~WantFloatParamTest()
    {
        want_ = nullptr;
    }

    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    std::shared_ptr<Want> want_;
};

void WantFloatParamTest::SetUpTestCase(void)
{}

void WantFloatParamTest::TearDownTestCase(void)
{}

void WantFloatParamTest::SetUp(void)
{
    want_ = std::make_shared<Want>();
}

void WantFloatParamTest::TearDown(void)
{}

/**
 * @tc.number: AaFwk_Want_FloatParam_0100
 * @tc.name:  SetParam/GetFloatParam
 * @tc.desc:  Verify when parameter change.
 */
HWTEST_P(WantFloatParamTest, AaFwk_Want_FloatParam_0100, Function | MediumTest | Level1)
{
    std::string setKey = std::get<0>(GetParam());
    std::string getKey = std::get<1>(GetParam());
    float setValue = std::get<2>(GetParam());
    float defaultValue = std::get<3>(GetParam());
    float result = std::get<4>(GetParam());
    want_->SetParam(setKey, setValue);
    EXPECT_EQ(result, want_->GetFloatParam(getKey, defaultValue));
}

INSTANTIATE_TEST_CASE_P(WantFloatParamTestCaseP, WantFloatParamTest,
    testing::Values(testFloatType("", "aa", -1.1, 100.1, 100.1), testFloatType("", "", -9.1, -41.1, -9.1),
        testFloatType("1*中_aR", "aa", 50.1, 5.1, 5.1), testFloatType("1*中_aR", "1*中_aR", -5000.1, 5000.1, -5000.1)));

/**
 * @tc.number: AaFwk_Want_FloatParam_0200
 * @tc.name:  SetParam/GetFloatParam
 * @tc.desc:  get param when WantParam is empty.
 */
HWTEST_F(WantFloatParamTest, AaFwk_Want_FloatParam_0200, Function | MediumTest | Level1)
{
    float defaultValue = 100.1;
    std::string key = "aa";
    EXPECT_EQ(defaultValue, want_->GetFloatParam(key, defaultValue));
}

/**
 * @tc.number: AaFwk_Want_FloatParam_0300
 * @tc.name:  SetParam/GetFloatParam
 * @tc.desc:  set empty-string key repeatedly, but get param of another nonexistent key.
 */
HWTEST_F(WantFloatParamTest, AaFwk_Want_FloatParam_0300, Function | MediumTest | Level1)
{
    std::string setKey1 = "";
    std::string setKey2 = "aa";
    float setValue1 = 1.1;
    float setValue2 = 5.1;
    want_->SetParam(setKey1, setValue1);
    want_->SetParam(setKey1, setValue1);
    setValue1 = 2.1;
    want_->SetParam(setKey1, setValue1);
    setValue1 = 3.1;
    EXPECT_EQ(setValue1, want_->GetFloatParam(setKey2, setValue1));
    setValue1 = 4.1;
    want_->SetParam(setKey1, setValue1);
    EXPECT_EQ(setValue1, want_->GetFloatParam(setKey1, setValue2));
}

/**
 * @tc.number: AaFwk_Want_FloatParam_0400
 * @tc.name:  SetParam/GetFloatParam
 * @tc.desc:  set empty-string key repeatedly, but get param of another nonexistent key.
 */
HWTEST_F(WantFloatParamTest, AaFwk_Want_FloatParam_0400, Function | MediumTest | Level1)
{
    std::string setKey1 = "%1uH3";
    std::string setKey2 = "aa";
    float setValue1 = -1.1;
    float setValue2 = 9.1;
    want_->SetParam(setKey1, setValue1);
    want_->SetParam(setKey1, setValue1);
    setValue1 = 0.1;
    want_->SetParam(setKey1, setValue1);
    EXPECT_EQ(setValue1, want_->GetFloatParam(setKey1, setValue2));
    setValue1 = 4.1;
    want_->SetParam(setKey1, setValue1);
    setValue1 = -10.1;
    EXPECT_EQ(setValue1, want_->GetFloatParam(setKey2, setValue1));
}

using testDoubleType = std::tuple<std::string, std::string, double, double, double>;
class WantDoubleParamTest : public testing::TestWithParam<testDoubleType> {
public:
    WantDoubleParamTest()
    {
        want_ = nullptr;
    }
    ~WantDoubleParamTest()
    {
        want_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    std::shared_ptr<Want> want_;
};

void WantDoubleParamTest::SetUpTestCase(void)
{}

void WantDoubleParamTest::TearDownTestCase(void)
{}

void WantDoubleParamTest::SetUp(void)
{
    want_ = std::make_shared<Want>();
}

void WantDoubleParamTest::TearDown(void)
{}

/**
 * @tc.number: AaFwk_Want_DoubleParam_0100
 * @tc.name:  SetParam/GetDoubleParam
 * @tc.desc:  Verify when parameter change.
 */
HWTEST_P(WantDoubleParamTest, AaFwk_Want_DoubleParam_0100, Function | MediumTest | Level1)
{
    std::string setKey = std::get<0>(GetParam());
    std::string getKey = std::get<1>(GetParam());
    double setValue = std::get<2>(GetParam());
    double defaultValue = std::get<3>(GetParam());
    double result = std::get<4>(GetParam());
    want_->SetParam(setKey, setValue);
    EXPECT_EQ(result, want_->GetDoubleParam(getKey, defaultValue));
}

INSTANTIATE_TEST_CASE_P(WantDoubleParamTestCaseP, WantDoubleParamTest,
    testing::Values(testDoubleType("", "aa", -1.1, 100.1, 100.1), testDoubleType("", "", -9.1, -41.1, -9.1),
        testDoubleType("1*中_aR", "aa", 50.1, 5.1, 5.1),
        testDoubleType("1*中_aR", "1*中_aR", -5000.1, 5000.1, -5000.1)));

/**
 * @tc.number: AaFwk_Want_DoubleParam_0300
 * @tc.name:  SetParam & GetDoubleParam
 * @tc.desc:  set empty-string key repeatedly, but get param of another nonexistent key.
 */
HWTEST_F(WantDoubleParamTest, AaFwk_Want_DoubleParam_0300, Function | MediumTest | Level1)
{
    double defaultValue = 100.1;
    std::string key = "aa";
    EXPECT_EQ(defaultValue, want_->GetDoubleParam(key, defaultValue));
}

/**
 * @tc.number: AaFwk_Want_DoubleParam_0400
 * @tc.name:  SetParam & GetDoubleParam
 * @tc.desc:  set empty-string key repeatedly, then get param of the key.
 */
HWTEST_F(WantDoubleParamTest, AaFwk_Want_DoubleParam_0400, Function | MediumTest | Level1)
{
    std::string setKey1 = "";
    std::string setKey2 = "aa";
    double setValue1 = 1.1;
    double setValue2 = 5.1;
    want_->SetParam(setKey1, setValue1);
    want_->SetParam(setKey1, setValue1);
    setValue1 = 2.1;
    want_->SetParam(setKey1, setValue1);
    setValue1 = 3.1;
    EXPECT_EQ(setValue1, want_->GetDoubleParam(setKey2, setValue1));
    setValue1 = 4.1;
    want_->SetParam(setKey1, setValue1);
    EXPECT_EQ(setValue1, want_->GetDoubleParam(setKey1, setValue2));
}

/**
 * @tc.number: AaFwk_Want_ByteArray_0100
 * @tc.name:  SetParam/GetByteArrayParam
 * @tc.desc:  Verify when parameter change.
 */
HWTEST_F(WantDoubleParamTest, AaFwk_Want_ByteArray_0100, Function | MediumTest | Level1)
{
    std::string setKey1 = "%1uH3";
    std::string setKey2 = "aa";
    double setValue1 = -1.1;
    double setValue2 = 9.1;
    want_->SetParam(setKey1, setValue1);
    want_->SetParam(setKey1, setValue1);
    setValue1 = 0.1;
    want_->SetParam(setKey1, setValue1);
    EXPECT_EQ(setValue1, want_->GetDoubleParam(setKey1, setValue2));
    setValue1 = 4.1;
    want_->SetParam(setKey1, setValue1);
    setValue1 = -10.1;
    EXPECT_EQ(setValue1, want_->GetDoubleParam(setKey2, setValue1));
}

using testByteArrayType = std::tuple<std::string, std::string, std::vector<byte>, std::vector<byte>, std::vector<byte>>;
class WantByteArrayParamTest : public testing::TestWithParam<testByteArrayType> {
public:
    WantByteArrayParamTest()
    {
        want_ = nullptr;
    }
    ~WantByteArrayParamTest()
    {
        want_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    std::shared_ptr<Want> want_;
};

void WantByteArrayParamTest::SetUpTestCase(void)
{}

void WantByteArrayParamTest::TearDownTestCase(void)
{}

void WantByteArrayParamTest::SetUp(void)
{
    want_ = std::make_shared<Want>();
}

void WantByteArrayParamTest::TearDown(void)
{}

/**
 * @tc.number: AaFwk_Want_ByteArray_0100
 * @tc.name:  SetParam/GetByteArrayParam
 * @tc.desc:  Verify when parameter change.
 */
HWTEST_P(WantByteArrayParamTest, AaFwk_Want_ByteArray_0100, Function | MediumTest | Level1)
{
    std::string setKey = std::get<0>(GetParam());
    std::string getKey = std::get<1>(GetParam());
    std::vector<byte> setValue = std::get<2>(GetParam());
    std::vector<byte> defaultValue = std::get<3>(GetParam());
    std::vector<byte> result = std::get<4>(GetParam());
    want_->SetParam(setKey, setValue);
    EXPECT_EQ(result, want_->GetByteArrayParam(getKey));
}

INSTANTIATE_TEST_CASE_P(WantByteArrayParamTestCaseP, WantByteArrayParamTest,
    testing::Values(testByteArrayType("", "aa", {'*', 'D'}, {}, {}),
        testByteArrayType("", "", {'%', ')'}, {}, {'%', ')'}), testByteArrayType("1*中_aR", "aa", {'R', '.'}, {}, {}),
        testByteArrayType("1*中_aR", "1*中_aR", {'R', 'b'}, {}, {'R', 'b'})));

/**
 * @tc.number: AaFwk_Want_ByteArray_0200
 * @tc.name:  SetParam/GetByteArrayParam
 * @tc.desc:  Verify when the value is byte array.
 */
HWTEST_F(WantByteArrayParamTest, AaFwk_Want_ByteArray_0200, Function | MediumTest | Level1)
{
    std::vector<byte> defaultValue;
    std::string getKey("aa");
    EXPECT_EQ(defaultValue, want_->GetByteArrayParam(getKey));
}

/**
 * @tc.number: AaFwk_Want_ByteArray_0300
 * @tc.name:  SetParam/GetByteArrayParam
 * @tc.desc:  Verify when the value is byte array.
 */
HWTEST_F(WantByteArrayParamTest, AaFwk_Want_ByteArray_0300, Function | MediumTest | Level1)
{
    std::string emptyStr("");
    std::vector<byte> firstValue({'a', '2'});
    std::vector<byte> secondValue({'1', 'd'});
    std::vector<byte> thirdValue({'t', '3'});
    std::string keyStr("aa");
    want_->SetParam(emptyStr, firstValue);
    want_->SetParam(emptyStr, firstValue);
    want_->SetParam(emptyStr, secondValue);
    std::vector<byte> defaultValue;
    EXPECT_EQ(defaultValue, want_->GetByteArrayParam(keyStr));
    want_->SetParam(emptyStr, thirdValue);
    EXPECT_EQ(thirdValue, want_->GetByteArrayParam(emptyStr));
}

/**
 * @tc.number: AaFwk_Want_ByteArray_0400
 * @tc.name:  SetParam/GetByteArrayParam
 * @tc.desc:  Verify when the value is byte array.
 */
HWTEST_F(WantByteArrayParamTest, AaFwk_Want_ByteArray_0400, Function | MediumTest | Level1)
{
    std::string firstKey("%1uH3");
    std::vector<byte> firstValue({'a', '2'});
    std::vector<byte> secondValue({'w', '$'});
    std::vector<byte> defaultValue;
    std::string secondKey("aa");
    want_->SetParam(firstKey, firstValue);
    want_->SetParam(firstKey, firstValue);
    want_->SetParam(firstKey, secondValue);
    EXPECT_EQ(secondValue, want_->GetByteArrayParam(firstKey));
    want_->SetParam(firstKey, firstValue);
    EXPECT_EQ(defaultValue, want_->GetByteArrayParam(secondKey));
}

using testBoolType = std::tuple<std::string, std::string, bool, bool, bool>;
class WantBoolParamTest : public testing::TestWithParam<testBoolType> {
public:
    WantBoolParamTest()
    {
        want_ = nullptr;
    }
    ~WantBoolParamTest()
    {
        want_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    std::shared_ptr<Want> want_;
};

void WantBoolParamTest::SetUpTestCase(void)
{}

void WantBoolParamTest::TearDownTestCase(void)
{}

void WantBoolParamTest::SetUp(void)
{
    want_ = std::make_shared<Want>();
}

void WantBoolParamTest::TearDown(void)
{}

/**
 * @tc.number: AaFwk_Want_BoolParam_0100
 * @tc.name: SetParam/GetBoolParam
 * @tc.desc: Verify when parameter change.
 */
HWTEST_P(WantBoolParamTest, AaFwk_Want_BoolParam_0100, Function | MediumTest | Level1)
{
    std::string setKey = std::get<0>(GetParam());
    std::string getKey = std::get<1>(GetParam());
    bool setValue = std::get<2>(GetParam());
    bool defaultValue = std::get<3>(GetParam());
    bool result = std::get<4>(GetParam());
    want_->SetParam(setKey, setValue);
    EXPECT_EQ(result, want_->GetBoolParam(getKey, defaultValue));
}

INSTANTIATE_TEST_CASE_P(WantBoolParamTestCaseP, WantBoolParamTest,
    testing::Values(testBoolType("", "aa", true, true, true), testBoolType("", "aa", true, false, false),
        testBoolType("", "", true, true, true), testBoolType("", "", true, false, true),
        testBoolType("123", "123", true, false, true), testBoolType("123", "aa", true, false, false),
        testBoolType("-~*&%￥", "-~*&%￥", true, false, true), testBoolType("-~*&%￥", "aa", true, false, false),
        testBoolType("中文", "中文", true, false, true), testBoolType("中文", "aa", true, false, false),
        testBoolType("_中文ddPEJKJ#(&*~#^%", "_中文ddPEJKJ#(&*~#^%", true, false, true),
        testBoolType("_中文ddPEJKJ#(&*~#^%", "123", true, false, false)));

/**
 * @tc.number: AaFwk_Want_BoolParam_0200
 * @tc.name:   SetParam/GetBoolParam
 * @tc.desc: Verify when set twice and get twice.
 */
HWTEST_F(WantBoolParamTest, AaFwk_Want_BoolParam_0200, Function | MediumTest | Level1)
{
    std::string firstKey("_中文ddPEJKJ#(&*~#^%");
    std::string secondKey("key33");
    want_->SetParam(firstKey, true);
    want_->SetParam(secondKey, true);
    EXPECT_EQ(true, want_->GetBoolParam(firstKey, false));
    EXPECT_EQ(true, want_->GetBoolParam(secondKey, false));
}

/**
 * @tc.number: AaFwk_Want_BoolParam_0300
 * @tc.name:   SetParam/GetBoolParam
 * @tc.desc: Verify when set 20 times, and get once.
 */
HWTEST_F(WantBoolParamTest, AaFwk_Want_BoolParam_0300, Function | MediumTest | Level1)
{
    std::string keyStr("_中文ddPEJKJ#(&*~#^%");
    for (int i = 0; i < 20; i++) {
        want_->SetParam(keyStr, true);
    }
    EXPECT_EQ(true, want_->GetBoolParam(keyStr, false));
}

/**
 * @tc.number: AaFwk_Want_Want_0100
 * @tc.name:   Want() and Want(want)
 * @tc.desc: Verify Want()
 */
HWTEST_F(WantBaseTest, AaFwk_Want_Want_0100, Function | MediumTest | Level1)
{
    Want want;

    EXPECT_EQ((uint)0, want.GetFlags());
    EXPECT_EQ(std::string(""), want.GetAction());

    std::vector<std::string> vect = want.GetEntities();
    EXPECT_EQ((size_t)0, vect.size());
    EXPECT_EQ(std::string(""), want.GetType());

    want.SetFlags(10);
    want.SetAction("system.Action.test");
    want.AddEntity("system.Entity.test");
    want.SetType("system.Type.test");

    Want want2(want);
    EXPECT_EQ("system.Action.test", want2.GetAction());
    EXPECT_EQ(true, want2.HasEntity("system.Entity.test"));
    EXPECT_EQ("system.Type.test", want2.GetType());
}

/**
 * @tc.number: AaFwk_Want_Entity_0100
 * @tc.name:    [AddEntity or RemoveEntity] & HasEntity &CountEntities
 * @tc.desc: Verify [AddEntity or RemoveEntity] & HasEntity &CountEntities
 */
HWTEST_F(WantBaseTest, AaFwk_Want_Entity_0100, Function | MediumTest | Level1)
{
    std::string entity1 = "entity.system.entity1";

    want_->AddEntity(entity1);

    EXPECT_EQ(true, want_->HasEntity(entity1));
    EXPECT_EQ(1, want_->CountEntities());
    want_->RemoveEntity(entity1);
    EXPECT_EQ(false, want_->HasEntity(entity1));
    EXPECT_EQ(0, want_->CountEntities());
    int length = want_->GetEntities().size();
    EXPECT_EQ(0, length);

    std::string entity2 = "entity.system.entity2";

    want_->AddEntity(entity1);
    want_->AddEntity(entity2);

    EXPECT_EQ(true, want_->HasEntity(entity1));
    EXPECT_EQ(2, want_->CountEntities());
    EXPECT_EQ(true, want_->HasEntity(entity2));
    EXPECT_EQ(2, want_->CountEntities());

    want_->RemoveEntity(entity1);
    want_->RemoveEntity(entity2);
    EXPECT_EQ(0, want_->CountEntities());
    int length2 = want_->GetEntities().size();

    EXPECT_EQ(0, length2);
}

/**
 * @tc.number: AaFwk_Want_HasParameter_0100
 * @tc.name:    SetParam and HasParameter
 * @tc.desc: Verify HasParameter()
 */
HWTEST_F(WantBaseTest, AaFwk_Want_HasParameter_0100, Function | MediumTest | Level1)
{

    std::vector<std::string> vector;
    std::string key = "system.want.test.key";
    std::string key2 = "system.want.test.key2";

    vector.push_back("system.want.test.content");
    want_->SetParam(key, vector);
    EXPECT_EQ(true, want_->HasParameter(key));

    want_->SetParam(key2, vector);
    EXPECT_EQ(true, want_->HasParameter(key2));
}
/**
 * @tc.number: AaFwk_Want_HasParameter_0200
 * @tc.name:    SetParam and HasParameter
 * @tc.desc: Verify HasParameter()
 */
HWTEST_F(WantBaseTest, AaFwk_Want_HasParameter_0200, Function | MediumTest | Level1)
{
    std::string key = std::to_string(Array::SIGNATURE) + ".#Want;key=3{\"\\b\\\";end";
    std::vector<zchar> arrayValue = {'.', '=', ';'};
    std::shared_ptr<Want> p1 = std::make_shared<Want>();
    if (p1 == nullptr) {
        return;
    }
    p1->SetParam(key, arrayValue);
    Want *newWant = nullptr;
    newWant = Want::ParseUri(p1->ToUri());
    if (newWant == nullptr) {
        return;
    }
    std::shared_ptr<Want> p2(newWant);
    CompareWant(p1, p2);
}
}  // namespace AAFwk
}  // namespace OHOS
