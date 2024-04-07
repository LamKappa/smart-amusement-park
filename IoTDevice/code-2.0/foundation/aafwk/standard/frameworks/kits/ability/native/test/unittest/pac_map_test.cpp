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
#include <memory>
#include "pac_map.h"
#include "ohos/aafwk/base/pac_map_node_user_object.h"

namespace OHOS {
namespace AppExecFwk {
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

#define PAC_MPA_TEST_INT 1000
#define PAC_MAP_TEST_LONG -1000
#define PAC_MAP_TEST_FLOAT 1.0f
#define PAC_MAP_TEST_DOUBLE 3.1415926

class TUserObjectTest : public TUserMapObject {
public:
    TUserObjectTest() : TUserMapObject("TUserObjectTest"), str_data_("用户自定义对象"), int_data_(0)
    {}
    virtual ~TUserObjectTest()
    {}

    virtual bool Equals(const TUserMapObject *other) override
    {
        TUserObjectTest *pobject = (TUserObjectTest *)other;
        if (pobject == nullptr) {
            return false;
        }
        return ((str_data_ == pobject->str_data_) && (int_data_ == pobject->int_data_));
    }

    virtual void DeepCopy(const TUserMapObject *other) override
    {
        TUserObjectTest *pobject = (TUserObjectTest *)other;
        if (pobject != nullptr) {
            str_data_ = pobject->str_data_;
            int_data_ = pobject->int_data_;
        }
    }

    virtual bool Marshalling(Parcel &parcel) const override
    {
        return true;
    }

    virtual bool Unmarshalling(Parcel &parcel) override
    {
        return true;
    }

private:
    std::string str_data_ = "";
    int int_data_ = 0;
};
REGISTER_USER_MAP_OBJECT(TUserObjectTest);

/*
 * Description：Test for data type of base: like int, short, long std::string etc.
 */
class PacMapTest : public testing::Test {
public:
    PacMapTest() : pacmap_(nullptr)
    {}
    ~PacMapTest()
    {}

    std::shared_ptr<PacMap> pacmap_ = nullptr;

    static void FillData(PacMap &pacmap);

    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void PacMapTest::SetUpTestCase(void)
{}

void PacMapTest::TearDownTestCase(void)
{}

void PacMapTest::SetUp()
{
    pacmap_ = std::make_shared<PacMap>();
}

void PacMapTest::TearDown()
{}

void PacMapTest::FillData(PacMap &pacmap)
{
    std::vector<short> arrayShort;
    std::vector<int> arrayInt;
    std::vector<long> arrayLong;
    std::vector<AAFwk::byte> arrayByte;
    std::vector<bool> arrayBool;
    std::vector<float> arrayFloat;
    std::vector<double> arrayDouble;
    std::vector<std::string> arrayString;

    arrayShort.push_back(PAC_MPA_TEST_INT);
    arrayInt.push_back(PAC_MPA_TEST_INT);
    arrayLong.push_back(PAC_MAP_TEST_LONG);
    arrayByte.push_back('a');
    arrayBool.push_back(true);
    arrayFloat.push_back(PAC_MAP_TEST_FLOAT);
    arrayDouble.push_back(PAC_MAP_TEST_DOUBLE);
    arrayString.push_back("<~!@#$%^&*()_+>特殊字符");

    pacmap.PutShortValue("key_short", PAC_MPA_TEST_INT);
    pacmap.PutIntValue("key_int", PAC_MPA_TEST_INT);
    pacmap.PutLongValue("key_long", PAC_MAP_TEST_LONG);
    pacmap.PutByteValue("key_byte", 'A');
    pacmap.PutBooleanValue("key_boolean", true);
    pacmap.PutFloatValue("key_float", PAC_MAP_TEST_FLOAT);
    pacmap.PutDoubleValue("key_double", PAC_MAP_TEST_DOUBLE);
    pacmap.PutStringValue("key_string", "test clone");

    std::shared_ptr<TUserObjectTest> pubObject = std::make_shared<TUserObjectTest>();
    pacmap.PutObject("key_object", pubObject);

    pacmap.PutShortValueArray("key_short_array", arrayShort);
    pacmap.PutIntValueArray("key_int_array", arrayInt);
    pacmap.PutLongValueArray("key_long_array", arrayLong);
    pacmap.PutByteValueArray("key_byte_array", arrayByte);
    pacmap.PutFloatValueArray("key_float_array", arrayFloat);
    pacmap.PutBooleanValueArray("key_boolean_array", arrayBool);
    pacmap.PutDoubleValueArray("key_double_array", arrayDouble);
    pacmap.PutStringValueArray("key_string_array", arrayString);
}

/**
 * @tc.number: AppExecFwk_PacMap_PutShortValue_0100
 * @tc.name: PutShortValue
 * @tc.desc: Verify PutShortValue() and GetShortValue().
 */
HWTEST_F(PacMapTest, AppExecFwk_PacMap_PutShortValue_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_PutShortValue_0100 start";
    short value = 1000;
    pacmap_->PutShortValue("key_short", value);
    EXPECT_EQ(value, pacmap_->GetShortValue("key_short"));
    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_PutShortValue_0100 end";
}

/**
 * @tc.number: AppExecFwk_PacMap_PutIntValue_0100
 * @tc.name: PutIntValue and GetIntValue
 * @tc.desc: Verify PutIntValue() and GetIntValue().
 */
HWTEST_F(PacMapTest, AppExecFwk_PacMap_PutIntValue_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_PutIntValue_0100 start";
    int value = 1000;
    pacmap_->PutIntValue("key_int", value);
    EXPECT_EQ(value, pacmap_->GetIntValue("key_int"));
    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_PutIntValue_0100 end";
}

/**
 * @tc.number: AppExecFwk_PacMap_PutLongValue_0100
 * @tc.name: PutLongValue and GetLongValue
 * @tc.desc: Verify PutLongValue() and GetLongValue().
 */
HWTEST_F(PacMapTest, AppExecFwk_PacMap_PutLongValue_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_PutLongValue_0100 start";
    long value = -1000;
    pacmap_->PutLongValue("key_long", value);
    EXPECT_EQ(value, pacmap_->GetLongValue("key_long"));
    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_PutLongValue_0100 end";
}

/**
 * @tc.number: AppExecFwk_PacMap_PutByteValue_0100
 * @tc.name: PutByteValue and GetByteValue
 * @tc.desc: Verify PutByteValue() and GetByteValue().
 */
HWTEST_F(PacMapTest, AppExecFwk_PacMap_PutByteValue_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_PutByteValue_0100 start";
    AAFwk::byte value = 'A';
    pacmap_->PutByteValue("key_byte", value);
    EXPECT_EQ(value, pacmap_->GetByteValue("key_byte"));
    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_PutByteValue_0100 end";
}

/**
 * @tc.number: AppExecFwk_PacMap_PutBooleanValue_0100
 * @tc.name: PutBooleanValue and GetBooleanValue
 * @tc.desc: Verify PutBooleanValue() and GetBooleanValue().
 */
HWTEST_F(PacMapTest, AppExecFwk_PacMap_PutBooleanValue_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_PutBooleanValue_0100 start";
    bool value = true;
    pacmap_->PutBooleanValue("key_boolean_true", value);
    EXPECT_EQ(value, pacmap_->GetBooleanValue("key_boolean_true"));

    value = false;
    pacmap_->PutBooleanValue("key_boolean_false", value);
    EXPECT_EQ(value, pacmap_->GetBooleanValue("key_boolean_false"));

    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_PutBooleanValue_0100 end";
}

/**
 * @tc.number: AppExecFwk_PacMap_PutFloatValue_0100
 * @tc.name: PutFloatValue and GetFloatValue
 * @tc.desc: Verify PutFloatValue() and GetFloatValue().
 */
HWTEST_F(PacMapTest, AppExecFwk_PacMap_PutFloatValue_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_PutFloatValue_0100 start";
    float value = 3.14f;
    pacmap_->PutFloatValue("key_float", value);
    EXPECT_EQ(value, pacmap_->GetFloatValue("key_float"));
    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_PutFloatValue_0100 end";
}

/**
 * @tc.number: AppExecFwk_PacMap_PutDoubleValue_0100
 * @tc.name: PutDoubleValue and GetDoubleValue
 * @tc.desc: Verify PutDoubleValue() and GetDoubleValue().
 */
HWTEST_F(PacMapTest, AppExecFwk_PacMap_PutDoubleValue_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_PutDoubleValue_0100 start";
    double value = 3.1415926;
    pacmap_->PutDoubleValue("key_double", value);
    EXPECT_EQ(value, pacmap_->GetDoubleValue("key_double"));
    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_PutDoubleValue_0100 end";
}

/**
 * @tc.number: AppExecFwk_PacMap_PutStringValue_0100
 * @tc.name: PutStringValue and GetStringValue
 * @tc.desc: Verify PutStringValue() and GetStringValue().
 */
HWTEST_F(PacMapTest, AppExecFwk_PacMap_PutStringValue_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_PutStringValue_0100 start";
    std::string value("AppExecFwk_PacMap_PutStringValue_0100  PACMAP测试");
    pacmap_->PutStringValue("key_string", value);
    EXPECT_STREQ(value.c_str(), pacmap_->GetStringValue("key_string").c_str());
    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_PutStringValue_0100 end";
}

/**
 * @tc.number: AppExecFwk_PacMap_PutShortValueArray_0100
 * @tc.name: PutShortValueArray and GetShortValueArray
 * @tc.desc: Verify PutShortValueArray() and GetShortValueArray().
 */
HWTEST_F(PacMapTest, AppExecFwk_PacMap_PutShortValueArray_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_PutShortValueArray_0100 start";

    std::vector<short> putValue;
    std::vector<short> getValue;
    for (int i = 0; i < 100; i++) {
        putValue.emplace_back(i + 1);
    }
    pacmap_->PutShortValueArray("key_short_array", putValue);
    pacmap_->GetShortValueArray("key_short_array", getValue);

    bool isEqual = (putValue == getValue);
    EXPECT_EQ(true, isEqual);

    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_PutShortValueArray_0100 end";
}

/**
 * @tc.number: AppExecFwk_PacMap_PutIntValueArray_0100
 * @tc.name: PutIntValueArray and GetIntValueArray
 * @tc.desc: Verify PutIntValueArray() and GetIntValueArray().
 */
HWTEST_F(PacMapTest, AppExecFwk_PacMap_PutIntValueArray_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_PutIntValueArray_0100 start";

    std::vector<int> putValue;
    std::vector<int> getValue;
    for (int i = 0; i < 100; i++) {
        putValue.emplace_back(i + 1);
    }
    pacmap_->PutIntValueArray("key_int_array", putValue);
    pacmap_->GetIntValueArray("key_int_array", getValue);

    bool isEqual = (putValue == getValue);
    EXPECT_EQ(true, isEqual);

    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_PutIntValueArray_0100 end";
}

/**
 * @tc.number: AppExecFwk_PacMap_PutLongArray_0100
 * @tc.name: PutLongValueArray and GetLongValueArray
 * @tc.desc: Verify PutLongValueArray() and GetLongValueArray().
 */
HWTEST_F(PacMapTest, AppExecFwk_PacMap_PutLongArray_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_PutLongArray_0100 start";

    std::vector<long> putValue;
    std::vector<long> getValue;
    for (int i = 0; i < 100; i++) {
        putValue.emplace_back(i + 1);
    }
    pacmap_->PutLongValueArray("key_long_array", putValue);
    pacmap_->GetLongValueArray("key_long_array", getValue);

    bool isEqual = (putValue == getValue);
    EXPECT_EQ(true, isEqual);

    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_PutLongArray_0100 end";
}

/**
 * @tc.number: AppExecFwk_PacMap_PutByteArray_0100
 * @tc.name: PutByteValueArray and GetByteValueArray
 * @tc.desc: Verify PutByteValueArray() and GetByteValueArray().
 */
HWTEST_F(PacMapTest, AppExecFwk_PacMap_PutByteArray_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_PutByteArray_0100 start";

    std::vector<AAFwk::byte> putValue;
    std::vector<AAFwk::byte> getValue;
    for (int i = 0; i < 26; i++) {
        putValue.emplace_back('A' + i);
    }
    pacmap_->PutByteValueArray("key_byte_array", putValue);
    pacmap_->GetByteValueArray("key_byte_array", getValue);

    bool isEqual = (putValue == getValue);
    EXPECT_EQ(true, isEqual);

    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_PutByteArray_0100 end";
}

/**
 * @tc.number: AppExecFwk_PacMap_PutFloatArray_0100
 * @tc.name: PutLongValueArray and GetLongValueArray
 * @tc.desc: Verify PutLongValueArray() and GetLongValueArray().
 */
HWTEST_F(PacMapTest, AppExecFwk_PacMap_PutFloatArray_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_PutFloatArray_0100 start";

    std::vector<float> putValue;
    std::vector<float> getValue;
    for (int i = 0; i < 100; i++) {
        putValue.emplace_back((i + 1) * 1.0f);
    }
    pacmap_->PutFloatValueArray("key_long_array", putValue);
    pacmap_->GetFloatValueArray("key_long_array", getValue);

    bool isEqual = (putValue == getValue);
    EXPECT_EQ(true, isEqual);

    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_PutFloatArray_0100 end";
}

/**
 * @tc.number: AppExecFwk_PacMap_PutDoubleArray_0100
 * @tc.name: PutDoubleValueArray and GetDoubleValueArray
 * @tc.desc: Verify PutDoubleValueArray() and GetDoubleValueArray().
 */
HWTEST_F(PacMapTest, AppExecFwk_PacMap_PutDoubleArray_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_PutDoubleArray_0100 start";

    std::vector<double> putValue;
    std::vector<double> getValue;
    for (int i = 0; i < 100; i++) {
        putValue.emplace_back((i + 1) * 1.0);
    }
    pacmap_->PutDoubleValueArray("key_double_array", putValue);
    pacmap_->GetDoubleValueArray("key_double_array", getValue);

    bool isEqual = (putValue == getValue);
    EXPECT_EQ(true, isEqual);

    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_PutDoubleArray_0100 end";
}

/**
 * @tc.number: AppExecFwk_PacMap_PutStringArray_0100
 * @tc.name: PutStringValueArray and GetStringValueArray
 * @tc.desc: Verify PutStringValueArray() and GetStringValueArray().
 */
HWTEST_F(PacMapTest, AppExecFwk_PacMap_PutStringArray_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_PutStringArray_0100 start";

    std::vector<std::string> tempValue;
    std::vector<std::string> putValue;
    std::vector<std::string> getValue;

    tempValue.emplace_back("Adds a String value matching a specified key.");
    tempValue.emplace_back("添加字符串");
    tempValue.emplace_back("<~!@#$%^&*()_+>特殊字符");

    for (int i = 0; i < 100; i++) {
        putValue.emplace_back(tempValue[i % 3]);
    }
    pacmap_->PutStringValueArray("key_string_array", putValue);
    pacmap_->GetStringValueArray("key_string_array", getValue);

    bool isEqual = (putValue == getValue);
    EXPECT_EQ(true, isEqual);

    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_PutStringArray_0100 end";
}

/**
 * @tc.number: AppExecFwk_PacMap_PutObject_0100
 * @tc.name: PutObject and GetObject
 * @tc.desc: Verify PutObject() and GetObject().
 */
HWTEST_F(PacMapTest, AppExecFwk_PacMap_PutObject_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_PutObject_0100 start";

    std::shared_ptr<TUserObjectTest> putObject = std::make_shared<TUserObjectTest>();
    pacmap_->PutObject("key_object", putObject);

    std::shared_ptr<TUserMapObject> getObject = pacmap_->GetObject("key_object");
    bool isEqual = false;
    if (getObject.get() != nullptr) {
        isEqual = getObject->Equals(getObject.get());
    }
    EXPECT_EQ(true, isEqual);

    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_PutObject_0100 end";
}

/**
 * @tc.number: AppExecFwk_PacMap_Clone_0100
 * @tc.name: Clone and Equals
 * @tc.desc: Verify Clone() and Equals().
 */
HWTEST_F(PacMapTest, AppExecFwk_PacMap_Clone_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_Clone_0100 start";

    PacMap otherMap;
    FillData(*pacmap_.get());
    otherMap = pacmap_->Clone();
    EXPECT_EQ(true, pacmap_->Equals(otherMap));

    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_Clone_0100 end";
}

/**
 * @tc.number: AppExecFwk_PacMap_DeepCopy_0100
 * @tc.name: DeepCopy
 * @tc.desc: Verify DeepCopy().
 */
HWTEST_F(PacMapTest, AppExecFwk_PacMap_DeepCopy_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_DeepCopy_0100 start";

    PacMap otherMap;
    FillData(*pacmap_.get());
    otherMap = pacmap_->DeepCopy();
    EXPECT_EQ(true, pacmap_->Equals(otherMap));

    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_DeepCopy_0100 end";
}

/**
 * @tc.number: AppExecFwk_PacMap_Clear_0100
 * @tc.name: Clear and GetSize
 * @tc.desc: Verify Clear() and GetSize().
 */
HWTEST_F(PacMapTest, AppExecFwk_PacMap_Clear_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_Clear_0100 start";

    FillData(*pacmap_.get());
    pacmap_->Clear();
    EXPECT_EQ(0, pacmap_->GetSize());

    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_Clear_0100 end";
}

/**
 * @tc.number: AppExecFwk_PacMap_PutAll_0100
 * @tc.name: PutAll
 * @tc.desc: Verify PutAll().
 */
HWTEST_F(PacMapTest, AppExecFwk_PacMap_PutAll_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_PutAll_0100 start";

    PacMap otherMap;
    FillData(otherMap);
    pacmap_->PutAll(otherMap);
    EXPECT_EQ(true, pacmap_->Equals(otherMap));

    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_PutAll_0100 end";
}

/**
 * @tc.number: AppExecFwk_PacMap_GetAll_0100
 * @tc.name: GetAll
 * @tc.desc: Verify GetAll().
 */
HWTEST_F(PacMapTest, AppExecFwk_PacMap_GetAll_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_GetAll_0100 start";

    FillData(*pacmap_.get());
    std::map<std::string, PacMapObject::Object> data = pacmap_->GetAll();

    EXPECT_EQ((int)data.size(), pacmap_->GetSize());

    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_GetAll_0100 end";
}

/**
 * @tc.number: AppExecFwk_PacMap_HasKey_0100
 * @tc.name: HasKey
 * @tc.desc: Verify HasKey().
 */
HWTEST_F(PacMapTest, AppExecFwk_PacMap_HasKey_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_HasKey_0100 start";

    FillData(*pacmap_.get());
    EXPECT_EQ(true, pacmap_->HasKey("key_short_array"));

    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_HasKey_0100 end";
}

/**
 * @tc.number: AppExecFwk_PacMap_GetKeys_0100
 * @tc.name: GetKeys
 * @tc.desc: Verify GetKeys().
 */
HWTEST_F(PacMapTest, AppExecFwk_PacMap_GetKeys_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_GetKeys_0100 start";

    FillData(*pacmap_.get());
    const std::set<std::string> keys = pacmap_->GetKeys();
    EXPECT_EQ((int)keys.size(), pacmap_->GetSize());

    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_GetKeys_0100 end";
}

/**
 * @tc.number: AppExecFwk_PacMap_Remove_0100
 * @tc.name: Remove
 * @tc.desc: Verify Remove().
 */
HWTEST_F(PacMapTest, AppExecFwk_PacMap_Remove_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_Remove_0100 start";

    FillData(*pacmap_.get());
    EXPECT_EQ(true, pacmap_->HasKey("key_short_array"));
    pacmap_->Remove("key_short_array");
    EXPECT_EQ(false, pacmap_->HasKey("key_short_array"));

    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_Remove_0100 end";
}

/**
 * @tc.number: AppExecFwk_PacMap_IsEmpty_0100
 * @tc.name: IsEmpty
 * @tc.desc: Verify IsEmpty().
 */
HWTEST_F(PacMapTest, AppExecFwk_PacMap_IsEmpty_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_IsEmpty_0100 start";

    FillData(*pacmap_.get());
    EXPECT_EQ(false, pacmap_->IsEmpty());
    pacmap_->Clear();
    EXPECT_EQ(true, pacmap_->IsEmpty());

    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_IsEmpty_0100 end";
}

/**
 * @tc.number: AppExecFwk_PacMap_Marshalling_0100
 * @tc.name: Marshalling and Unmarshalling
 * @tc.desc: Verify Marshalling() and Unmarshalling().
 */
HWTEST_F(PacMapTest, AppExecFwk_PacMap_Marshalling_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_Marshalling_0100 start";

    Parcel parcel;
    FillData(*pacmap_.get());
    EXPECT_EQ(true, pacmap_->Marshalling(parcel));

    PacMap *unmarshingMap = PacMap::Unmarshalling(parcel);
    EXPECT_EQ(true, unmarshingMap != nullptr);
    if (unmarshingMap != nullptr) {
        EXPECT_EQ(true, pacmap_->Equals(unmarshingMap));
        delete unmarshingMap;
        unmarshingMap = nullptr;
    }
    GTEST_LOG_(INFO) << "AppExecFwk_PacMap_Marshalling_0100 end";
}
}  // namespace AppExecFwk
}  // namespace OHOS