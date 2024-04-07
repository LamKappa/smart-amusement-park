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
#include <tuple>

#include "ohos/aafwk/content/intent.h"
#include "parcel.h"

using namespace testing::ext;
using namespace OHOS::AAFwk;
using OHOS::Parcel;
using OHOS::AppExecFwk::ElementName;

class IntentParcelableTest : public testing::Test {
public:
    IntentParcelableTest()
    {}
    ~IntentParcelableTest()
    {}
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    enum {
        FLAG_TEST_SINGLE = 0x01,
        FLAG_TEST_ARRAY = 0x02,
        FLAG_TEST_BOTH = 0x03,
    };

    bool CompareIntent(Intent &intent1, Intent &intent2, std::map<std::string, std::string> &keys);
    void SendParcelTest(Intent &intent, std::map<std::string, std::string> &keys);
    void AddBoolParams(Intent &intent, std::map<std::string, std::string> &keys, int loop, unsigned int flag);
    void AddByteParams(Intent &intent, std::map<std::string, std::string> &keys, int loop, unsigned int flag);
    void AddCharParams(Intent &intent, std::map<std::string, std::string> &keys, int loop, unsigned int flag);
    void AddShortParams(Intent &intent, std::map<std::string, std::string> &keys, int loop, unsigned int flag);
    void AddIntParams(Intent &intent, std::map<std::string, std::string> &keys, int loop, unsigned int flag);
    void AddLongParams(Intent &intent, std::map<std::string, std::string> &keys, int loop, unsigned int flag);
    void AddFloatParams(Intent &intent, std::map<std::string, std::string> &keys, int loop, unsigned int flag);
    void AddDoubleParams(Intent &intent, std::map<std::string, std::string> &keys, int loop, unsigned int flag);
    void AddStringParams(Intent &intent, std::map<std::string, std::string> &keys, int loop, unsigned int flag);

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
};

void IntentParcelableTest::SetUpTestCase(void)
{}

void IntentParcelableTest::TearDownTestCase(void)
{}

void IntentParcelableTest::SetUp(void)
{}

void IntentParcelableTest::TearDown(void)
{}

bool IntentParcelableTest::CompareIntent(Intent &intent1, Intent &intent2, std::map<std::string, std::string> &keys)
{
    EXPECT_EQ(intent1.GetAction(), intent2.GetAction());
    EXPECT_EQ(intent1.GetEntity(), intent2.GetEntity());
    EXPECT_EQ(intent1.GetFlags(), intent2.GetFlags());
    EXPECT_EQ(intent1.GetElement(), intent2.GetElement());

    for (auto it = keys.begin(); it != keys.end(); it++) {
        if (it->second == boolType) {
            bool v1 = intent1.GetBoolParam(it->first, false);
            bool v2 = intent2.GetBoolParam(it->first, false);
            EXPECT_EQ(v1, v2);
            EXPECT_EQ(v1, true);
        } else if (it->second == boolArrayType) {
            std::vector<bool> v1 = intent1.GetBoolArrayParam(it->first);
            std::vector<bool> v2 = intent2.GetBoolArrayParam(it->first);
            EXPECT_EQ(v1, v2);
        } else if (it->second == byteType) {
            byte v1 = intent1.GetByteParam(it->first, 'j');
            byte v2 = intent2.GetByteParam(it->first, 'k');
            EXPECT_EQ(v1, v2);
        } else if (it->second == byteArrayType) {
            std::vector<byte> v1 = intent1.GetByteArrayParam(it->first);
            std::vector<byte> v2 = intent2.GetByteArrayParam(it->first);
            EXPECT_EQ(v1, v2);
        } else if (it->second == charType) {
            zchar v1 = intent1.GetCharParam(it->first, 0x01AB);
            zchar v2 = intent2.GetCharParam(it->first, 0x02CD);
            EXPECT_EQ(v1, v2);
        } else if (it->second == charArrayType) {
            std::vector<zchar> v1 = intent1.GetCharArrayParam(it->first);
            std::vector<zchar> v2 = intent2.GetCharArrayParam(it->first);
            EXPECT_EQ(v1, v2);
        } else if (it->second == shortType) {
            short default1 = 123;
            short default2 = 456;
            short v1 = intent1.GetShortParam(it->first, default1);
            short v2 = intent2.GetShortParam(it->first, default2);
            EXPECT_EQ(v1, v2);
        } else if (it->second == shortArrayType) {
            std::vector<short> v1 = intent1.GetShortArrayParam(it->first);
            std::vector<short> v2 = intent2.GetShortArrayParam(it->first);
            EXPECT_EQ(v1, v2);
        } else if (it->second == intType) {
            int default1 = 1230000;
            int default2 = 4560000;
            int v1 = intent1.GetIntParam(it->first, default1);
            int v2 = intent2.GetIntParam(it->first, default2);
            EXPECT_EQ(v1, v2);
        } else if (it->second == intArrayType) {
            std::vector<int> v1 = intent1.GetIntArrayParam(it->first);
            std::vector<int> v2 = intent2.GetIntArrayParam(it->first);
            EXPECT_EQ(v1, v2);
        } else if (it->second == longType) {
            long default1 = 1e10;
            long default2 = 2e10;
            long v1 = intent1.GetLongParam(it->first, default1);
            long v2 = intent2.GetLongParam(it->first, default2);
            EXPECT_EQ(v1, v2);
        } else if (it->second == longArrayType) {
            std::vector<long> v1 = intent1.GetLongArrayParam(it->first);
            std::vector<long> v2 = intent2.GetLongArrayParam(it->first);
            EXPECT_EQ(v1, v2);
        } else if (it->second == floatType) {
            float default1 = 12.3;
            float default2 = 45.6;
            float v1 = intent1.GetFloatParam(it->first, default1);
            float v2 = intent2.GetFloatParam(it->first, default2);
            EXPECT_EQ(v1, v2);
        } else if (it->second == floatArrayType) {
            std::vector<float> v1 = intent1.GetFloatArrayParam(it->first);
            std::vector<float> v2 = intent2.GetFloatArrayParam(it->first);
            EXPECT_EQ(v1, v2);
        } else if (it->second == doubleType) {
            double default1 = 12.3;
            double default2 = 45.6;
            double v1 = intent1.GetDoubleParam(it->first, default1);
            double v2 = intent2.GetDoubleParam(it->first, default2);
            EXPECT_EQ(v1, v2);
        } else if (it->second == doubleArrayType) {
            std::vector<double> v1 = intent1.GetDoubleArrayParam(it->first);
            std::vector<double> v2 = intent2.GetDoubleArrayParam(it->first);
            EXPECT_EQ(v1, v2);
        } else if (it->second == stringType) {
            std::string v1 = intent1.GetStringParam(it->first);
            std::string v2 = intent2.GetStringParam(it->first);
            EXPECT_EQ(v1, v2);
        } else if (it->second == stringArrayType) {
            std::vector<std::string> v1 = intent1.GetStringArrayParam(it->first);
            std::vector<std::string> v2 = intent2.GetStringArrayParam(it->first);
            EXPECT_EQ(v1, v2);
        }
    }

    return true;
}

void IntentParcelableTest::SendParcelTest(Intent &intent, std::map<std::string, std::string> &keys)
{
    size_t pos1;
    size_t pos2;
    Parcel data;
    bool result;

    pos1 = data.GetWritePosition();
    result = data.WriteParcelable(&intent);
    pos2 = data.GetWritePosition();
    EXPECT_EQ(result, true);
    GTEST_LOG_(INFO) << "SendParcelTest: pos1: " << pos1 << ", pos2: " << pos2 << ", result: " << result;

    Intent *intentNew = nullptr;
    intentNew = data.ReadParcelable<Intent>();
    EXPECT_NE(intentNew, nullptr);

    if (intentNew != nullptr) {
        result = CompareIntent(intent, *intentNew, keys);
        EXPECT_EQ(result, true);
        delete intentNew;
    }
}

/*
 * Feature: Intent
 * Function: parcelable
 * SubFunction: NA
 * FunctionPoints: parcelable
 * EnvConditions: NA
 * CaseDescription: Verify parcelable
 */
HWTEST_F(IntentParcelableTest, AaFwk_Intent_Parcelable_001, TestSize.Level1)
{
    std::map<std::string, std::string> keys;

    Intent intent;

    SendParcelTest(intent, keys);
}

void IntentParcelableTest::AddBoolParams(
    Intent &intent, std::map<std::string, std::string> &keys, int loop, unsigned unsigned int flag)
{
    std::string key;
    std::string boolKey = "boolKey";
    std::string boolArrayKey = "boolArrayKey";
    for (int i = 0; i < loop; i++) {
        if (flag & FLAG_TEST_SINGLE) {
            // bool type value
            bool boolValue = true;
            key = boolKey + std::to_string(i);
            keys[key] = boolType;
            intent.SetBoolParam(key, boolValue);
        }

        if (flag & FLAG_TEST_ARRAY) {
            // bool array type value
            std::vector<bool> boolArrayValue = {true, false, true};
            key = boolArrayKey + std::to_string(i);
            keys[key] = boolArrayType;
            intent.SetBoolArrayParam(key, boolArrayValue);
        }
    }
}

void IntentParcelableTest::AddByteParams(
    Intent &intent, std::map<std::string, std::string> &keys, int loop, unsigned unsigned int flag)
{
    std::string key;
    std::string byteKey = "byteKey";
    std::string byteArrayKey = "byteArrayKey";
    for (int i = 0; i < loop; i++) {
        if (flag & FLAG_TEST_SINGLE) {
            // byte type value
            byte byteValue = 'z';
            key = byteKey + std::to_string(i);
            keys[key] = byteType;
            intent.SetByteParam(key, byteValue);
        }

        if (flag & FLAG_TEST_ARRAY) {
            // byte array type value
            std::vector<byte> byteArrayValue = {'?', 'a', '\\'};
            key = byteArrayKey + std::to_string(i);
            keys[key] = byteArrayType;
            intent.SetByteArrayParam(key, byteArrayValue);
        }
    }
}

void IntentParcelableTest::AddCharParams(
    Intent &intent, std::map<std::string, std::string> &keys, int loop, unsigned int flag)
{
    std::string key;
    std::string charKey = "charKey";
    std::string charArrayKey = "charArrayKey";
    for (int i = 0; i < loop; i++) {
        if (flag & FLAG_TEST_SINGLE) {
            // char type value
            zchar charValue = U'世';
            key = charKey + std::to_string(i);
            keys[key] = charType;
            intent.SetCharParam(key, charValue);
        }

        if (flag & FLAG_TEST_ARRAY) {
            // char array type value
            std::vector<zchar> charArrayValue = {U'界', U'和', U'平'};
            key = charArrayKey + std::to_string(i);
            keys[key] = charArrayType;
            intent.SetCharArrayParam(key, charArrayValue);
        }
    }
}

void IntentParcelableTest::AddShortParams(
    Intent &intent, std::map<std::string, std::string> &keys, int loop, unsigned int flag)
{
    std::string key;
    std::string shortKey = "shortKey";
    std::string shortArrayKey = "shortArrayKey";
    for (int i = 0; i < loop; i++) {
        if (flag & FLAG_TEST_SINGLE) {
            // short type value
            short shortValue = 1;
            key = shortKey + std::to_string(i);
            keys[key] = shortType;
            intent.SetShortParam(key, shortValue);
        }

        if (flag & FLAG_TEST_ARRAY) {
            // short array type value
            std::vector<short> shortArrayValue = {-1, 0, 1};
            key = shortArrayKey + std::to_string(i);
            keys[key] = shortArrayType;
            intent.SetShortArrayParam(key, shortArrayValue);
        }
    }
}

void IntentParcelableTest::AddIntParams(
    Intent &intent, std::map<std::string, std::string> &keys, int loop, unsigned int flag)
{
    std::string key;
    std::string intKey = "intKey";
    std::string intArrayKey = "intArrayKey";
    for (int i = 0; i < loop; i++) {
        if (flag & FLAG_TEST_SINGLE) {
            // int type value
            int intValue = 10;
            key = intKey + std::to_string(i);
            keys[key] = intType;
            intent.SetIntParam(key, intValue);
        }

        if (flag & FLAG_TEST_ARRAY) {
            // int array type value
            std::vector<int> intArrayValue = {-10, 0, 10};
            key = intArrayKey + std::to_string(i);
            keys[key] = intArrayType;
            intent.SetIntArrayParam(key, intArrayValue);
        }
    }
}

void IntentParcelableTest::AddLongParams(
    Intent &intent, std::map<std::string, std::string> &keys, int loop, unsigned int flag)
{
    std::string key;
    std::string longKey = "longKey";
    std::string longArrayKey = "longArrayKey";
    for (int i = 0; i < loop; i++) {
        if (flag & FLAG_TEST_SINGLE) {
            // long type value
            long longValue = 100;
            key = longKey + std::to_string(i);
            keys[key] = longType;
            intent.SetLongParam(key, longValue);
        }

        if (flag & FLAG_TEST_ARRAY) {
            // long array type value
            std::vector<long> longArrayValue = {-100, 0, 100};
            key = longArrayKey + std::to_string(i);
            keys[key] = longArrayType;
            intent.SetLongArrayParam(key, longArrayValue);
        }
    }
}

void IntentParcelableTest::AddFloatParams(
    Intent &intent, std::map<std::string, std::string> &keys, int loop, unsigned int flag)
{
    std::string key;
    std::string floatKey = "floatKey";
    std::string floatArrayKey = "floatArrayKey";
    for (int i = 0; i < loop; i++) {
        if (flag & FLAG_TEST_SINGLE) {
            // float type value
            float floatValue = 100.1;
            key = floatKey + std::to_string(i);
            keys[key] = floatType;
            intent.SetFloatParam(key, floatValue);
        }

        if (flag & FLAG_TEST_ARRAY) {
            // float array type value
            std::vector<float> floatArrayValue = {-100.1, 0.1, 100.1};
            key = floatArrayKey + std::to_string(i);
            keys[key] = floatArrayType;
            intent.SetFloatArrayParam(key, floatArrayValue);
        }
    }
}

void IntentParcelableTest::AddDoubleParams(
    Intent &intent, std::map<std::string, std::string> &keys, int loop, unsigned int flag)
{
    std::string key;
    std::string doubleKey = "doubleKey";
    std::string doubleArrayKey = "doubleArrayKey";
    for (int i = 0; i < loop; i++) {
        if (flag & FLAG_TEST_SINGLE) {
            // double type value
            double doubleValue = 1000.1;
            key = doubleKey + std::to_string(i);
            keys[key] = doubleType;
            intent.SetDoubleParam(key, doubleValue);
        }

        if (flag & FLAG_TEST_ARRAY) {
            // double array type value
            std::vector<double> doubleArrayValue = {-1000.1, 0.1, 1000.1};
            key = doubleArrayKey + std::to_string(i);
            keys[key] = doubleArrayType;
            intent.SetDoubleArrayParam(key, doubleArrayValue);
        }
    }
}

void IntentParcelableTest::AddStringParams(
    Intent &intent, std::map<std::string, std::string> &keys, int loop, unsigned int flag)
{
    std::string key;
    std::string stringKey = "stringKey";
    std::string stringArrayKey = "stringArrayKey";
    for (int i = 0; i < loop; i++) {
        if (flag & FLAG_TEST_SINGLE) {
            // string type value
            string stringValue = "zzzz";
            key = stringKey + std::to_string(i);
            keys[key] = stringType;
            intent.SetStringParam(key, stringValue);
        }

        if (flag & FLAG_TEST_ARRAY) {
            // string array type value
            std::vector<std::string> stringArrayValue = {"??", "aa", "\\\\"};
            key = stringArrayKey + std::to_string(i);
            keys[key] = stringArrayType;
            intent.SetStringArrayParam(key, stringArrayValue);
        }
    }
}

/*
 * Feature: Intent
 * Function: parcelable
 * SubFunction: NA
 * FunctionPoints: parcelable
 * EnvConditions: NA
 * CaseDescription: Verify parcelable
 */
HWTEST_F(IntentParcelableTest, AaFwk_Intent_Parcelable_002, TestSize.Level1)
{
    std::string action = "intent.action.test";
    unsigned int flag = 0x789;
    std::string entity = "intent.entity.test";
    ElementName element("bundlename", "appname", "abilityname");

    Intent intent;
    intent.SetAction(action).SetEntity(entity).SetFlag(flag).SetElement(element);

    int loop = 1;
    std::map<std::string, std::string> keys;

    AddBoolParams(intent, keys, loop, FLAG_TEST_BOTH);
    AddByteParams(intent, keys, loop, FLAG_TEST_BOTH);
    AddCharParams(intent, keys, loop, FLAG_TEST_BOTH);
    AddShortParams(intent, keys, loop, FLAG_TEST_BOTH);
    AddIntParams(intent, keys, loop, FLAG_TEST_BOTH);
    AddLongParams(intent, keys, loop, FLAG_TEST_BOTH);
    AddFloatParams(intent, keys, loop, FLAG_TEST_BOTH);
    AddDoubleParams(intent, keys, loop, FLAG_TEST_BOTH);
    AddStringParams(intent, keys, loop, FLAG_TEST_BOTH);

    SendParcelTest(intent, keys);
}

/*
 * Feature: Intent
 * Function: parcelable
 * SubFunction: NA
 * FunctionPoints: parcelable
 * EnvConditions: NA
 * CaseDescription: Verify parcelable
 */
HWTEST_F(IntentParcelableTest, AaFwk_Intent_Parcelable_003, TestSize.Level1)
{
    std::string action = "intent.action.test";
    unsigned int flag = 0x789;
    std::string entity = "intent.entity.test";
    ElementName element("bundlename", "appname", "abilityname");

    Intent intent;
    intent.SetAction(action).SetEntity(entity).SetFlag(flag).SetElement(element);

    int loop = 1;
    std::map<std::string, std::string> keys;

    AddByteParams(intent, keys, loop, FLAG_TEST_BOTH);
    AddCharParams(intent, keys, loop, FLAG_TEST_BOTH);
    AddShortParams(intent, keys, loop, FLAG_TEST_BOTH);
    AddIntParams(intent, keys, loop, FLAG_TEST_BOTH);
    AddLongParams(intent, keys, loop, FLAG_TEST_BOTH);
    AddFloatParams(intent, keys, loop, FLAG_TEST_BOTH);
    AddDoubleParams(intent, keys, loop, FLAG_TEST_BOTH);
    AddStringParams(intent, keys, loop, FLAG_TEST_BOTH);

    SendParcelTest(intent, keys);
}

/*
 * Feature: Intent
 * Function: parcelable
 * SubFunction: NA
 * FunctionPoints: parcelable
 * EnvConditions: NA
 * CaseDescription: Verify parcelable
 */
HWTEST_F(IntentParcelableTest, AaFwk_Intent_Parcelable_004, TestSize.Level1)
{
    std::string action = "intent.action.test";
    unsigned int flag = 0x789;
    std::string entity = "intent.entity.test";
    ElementName element("bundlename", "appname", "abilityname");

    Intent intent;
    intent.SetAction(action).SetEntity(entity).SetFlag(flag).SetElement(element);

    int loop = 1;
    std::map<std::string, std::string> keys;

    AddBoolParams(intent, keys, loop, FLAG_TEST_BOTH);

    SendParcelTest(intent, keys);
}
