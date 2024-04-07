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
#include <cstring>

#include "ohos/aafwk/content/intent.h"
#include "ohos/aafwk/base/array_wrapper.h"
#include "ohos/aafwk/base/float_wrapper.h"
#include "ohos/aafwk/base/string_wrapper.h"
#include "refbase.h"

using namespace testing::ext;
using namespace OHOS::AAFwk;
using namespace OHOS;
using OHOS::AppExecFwk::ElementName;

class IntentParseToUriTest : public testing::Test {
public:
    IntentParseToUriTest()
    {}
    ~IntentParseToUriTest()
    {}
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    static const std::string URI_STRING_HEAD;
    static const std::string URI_STRING_END;
};

void IntentParseToUriTest::SetUpTestCase(void)
{}

void IntentParseToUriTest::TearDownTestCase(void)
{}

void IntentParseToUriTest::SetUp(void)
{}

void IntentParseToUriTest::TearDown(void)
{}

const std::string IntentParseToUriTest::URI_STRING_HEAD("#Intent;");
const std::string IntentParseToUriTest::URI_STRING_END(";end");

/*
 * Feature: Intent
 * Function: ParseUri and ToUri
 * SubFunction: NA
 * FunctionPoints: ParseUri and ToUri
 * EnvConditions: NA
 * CaseDescription: Verify the function when Intent is empty
 */
HWTEST_F(IntentParseToUriTest, AaFwk_Intent_ParseUri_ToUri_001, TestSize.Level1)
{
    std::size_t pos = 0;
    std::size_t content = 0;
    std::size_t head = 0;
    Intent intentOrigin;

    // ToUri
    std::string uri = intentOrigin.ToUri();

    // check head
    head = uri.find(IntentParseToUriTest::URI_STRING_HEAD, pos);
    EXPECT_EQ(head, pos);
    if (head != std::string::npos) {
        pos += head + IntentParseToUriTest::URI_STRING_HEAD.length() - 1;
    }

    // check end
    content = uri.find(IntentParseToUriTest::URI_STRING_END, pos);
    EXPECT_EQ(content, pos);
    if (content != std::string::npos) {
        pos += IntentParseToUriTest::URI_STRING_END.length();
    }

    // check uri length
    EXPECT_EQ(uri.length(), pos);

    // ParseUri
    Intent *intentNew = Intent::ParseUri(uri);
    EXPECT_NE(intentNew, nullptr);

    if (intentNew != nullptr) {
        // check new Intent
        EXPECT_EQ(intentNew->GetAction(), std::string(""));
        EXPECT_EQ(intentNew->GetEntity(), std::string(""));
        ElementName element;
        EXPECT_EQ(intentNew->GetElement(), element);
        EXPECT_EQ(static_cast<int>(intentNew->GetFlags()), 0);

        delete intentNew;
    }
}

/*
 * Feature: Intent
 * Function: ParseUri and ToUri
 * SubFunction: NA
 * FunctionPoints: ParseUri and ToUri
 * EnvConditions: NA
 * CaseDescription: Verify the function when Intent only has action/entity/flag/element
 */
HWTEST_F(IntentParseToUriTest, AaFwk_Intent_ParseUri_ToUri_002, TestSize.Level1)
{
    std::string search;
    std::size_t pos = 0;
    std::size_t length = 0;
    std::size_t result = 0;
    std::size_t head = 0;

    std::string action = Intent::ACTION_PLAY;
    std::string entity = Intent::ENTITY_VIDEO;
    unsigned int flag = 0x0f0f0f0f;
    std::string flagStr = "0x0f0f0f0f";
    std::string device = "device1";
    std::string bundle = "bundle1";
    std::string ability = "ability1";
    ElementName element(device, bundle, ability);

    Intent intentOrigin;
    intentOrigin.SetAction(action);
    intentOrigin.SetEntity(entity);
    intentOrigin.SetFlag(flag);
    intentOrigin.SetElement(element);

    // ToUri
    std::string uri = intentOrigin.ToUri();

    // check head
    search = IntentParseToUriTest::URI_STRING_HEAD;
    result = uri.find(search, pos);
    EXPECT_EQ(result, pos);
    if (result != std::string::npos) {
        head = result + search.length();
    }
    length += head;

    // check action
    search = std::string("action=") + action + std::string(";");
    result = uri.find(search);
    EXPECT_NE(result, std::string::npos);
    EXPECT_GE(result, head);
    if (result != std::string::npos) {
        length += search.length();
    }

    // check entity
    search = std::string("entity=") + entity + std::string(";");
    result = uri.find(search);
    EXPECT_NE(result, std::string::npos);
    EXPECT_GE(result, head);
    if (result != std::string::npos) {
        length += search.length();
    }

    // check flag
    search = std::string("flag=") + flagStr + std::string(";");
    result = uri.find(search);
    EXPECT_NE(result, std::string::npos);
    EXPECT_GE(result, head);
    if (result != std::string::npos) {
        length += search.length();
    }

    // check element device
    search = std::string("device=") + device + std::string(";");
    result = uri.find(search);
    EXPECT_NE(result, std::string::npos);
    EXPECT_GE(result, head);
    if (result != std::string::npos) {
        length += search.length();
    }

    // check element bundle
    search = std::string("bundle=") + bundle + std::string(";");
    result = uri.find(search);
    EXPECT_NE(result, std::string::npos);
    EXPECT_GE(result, head);
    if (result != std::string::npos) {
        length += search.length();
    }

    // check element ability
    search = std::string("ability=") + ability + std::string(";");
    result = uri.find(search);
    EXPECT_NE(result, std::string::npos);
    EXPECT_GE(result, head);
    if (result != std::string::npos) {
        length += search.length();
    }

    // check end
    search = IntentParseToUriTest::URI_STRING_END;
    result = uri.find(search);
    EXPECT_NE(result, std::string::npos);
    EXPECT_GE(result, head);
    if (result != std::string::npos) {
        length += search.length() - 1;
    }

    // check uri length
    EXPECT_EQ(uri.length(), length);

    // ParseUri
    Intent *intentNew = Intent::ParseUri(uri);
    EXPECT_NE(intentNew, nullptr);

    if (intentNew != nullptr) {
        // check new intent
        EXPECT_EQ(intentNew->GetAction(), action);
        EXPECT_EQ(intentNew->GetEntity(), entity);
        EXPECT_EQ(intentNew->GetElement().GetDeviceID(), device);
        EXPECT_EQ(intentNew->GetElement().GetBundleName(), bundle);
        EXPECT_EQ(intentNew->GetElement().GetAbilityName(), ability);
        EXPECT_EQ(intentNew->GetFlags(), flag);

        delete intentNew;
    }
}

/*
 * Feature: Intent
 * Function: ParseUri and ToUri
 * SubFunction: NA
 * FunctionPoints: ParseUri and ToUri
 * EnvConditions: NA
 * CaseDescription: Verify the function when Intent only has parameter and the parameter
 * has only 1 float type element
 */
HWTEST_F(IntentParseToUriTest, AaFwk_Intent_ParseUri_ToUri_003, TestSize.Level1)
{
    std::string search;
    std::size_t pos = 0;
    std::size_t length = 0;
    std::size_t result = 0;
    std::size_t delims = 0;
    std::size_t head = 0;
    Intent intentOrigin;
    std::string keyFloat = "keyFloat";
    float valueFloatOrigin = 123.4;
    intentOrigin.SetFloatParam(keyFloat, valueFloatOrigin);

    // ToUri
    std::string uri = intentOrigin.ToUri();

    // check head
    search = IntentParseToUriTest::URI_STRING_HEAD;
    result = uri.find(search, pos);
    EXPECT_EQ(result, pos);
    if (result != std::string::npos) {
        head = result + search.length();
    }
    length += head;

    // check float parameter
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

    // check end
    search = IntentParseToUriTest::URI_STRING_END;
    result = uri.find(search);
    EXPECT_NE(result, std::string::npos);
    EXPECT_GE(result, head);
    if (result != std::string::npos) {
        length += search.length() - 1;
    }

    // check uri length
    EXPECT_EQ(uri.length(), length);

    // ParseUri
    Intent *intentNew = Intent::ParseUri(uri);
    EXPECT_NE(intentNew, nullptr);

    if (intentNew != nullptr) {
        // check new intent
        float floatNew = intentNew->GetFloatParam(keyFloat, 0.1);
        float floatOld = intentOrigin.GetFloatParam(keyFloat, 1.1);
        EXPECT_EQ(floatNew, floatOld);
        delete intentNew;
    }
}

/*
 * Feature: Intent
 * Function: ParseUri and ToUri
 * SubFunction: NA
 * FunctionPoints: ParseUri and ToUri
 * EnvConditions: NA
 * CaseDescription: Verify the function when Intent only has parameter and the parameter
 * has only one float and one string type element
 */
HWTEST_F(IntentParseToUriTest, AaFwk_Intent_ParseUri_ToUri_004, TestSize.Level1)
{
    std::string search;
    std::size_t pos = 0;
    std::size_t length = 0;
    std::size_t result = 0;
    std::size_t delims = 0;
    std::size_t head = 0;
    Intent intentOrigin;
    std::string keyFloat = "keyFloat";
    std::string keyString = "keyString";
    float valueFloatOrigin = 123.4;
    std::string valueStringOrigin = "abcd";
    intentOrigin.SetFloatParam(keyFloat, valueFloatOrigin);
    intentOrigin.SetStringParam(keyString, valueStringOrigin);

    // ToUri
    std::string uri = intentOrigin.ToUri();

    // check head
    search = IntentParseToUriTest::URI_STRING_HEAD;
    result = uri.find(search, pos);
    EXPECT_NE(result, std::string::npos);
    EXPECT_EQ(result, pos);
    if (result != std::string::npos) {
        head = result + search.length();
    }
    length += head;

    // check float parameter
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

    // check string parameter
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

    // check end
    search = IntentParseToUriTest::URI_STRING_END;
    result = uri.find(search);
    EXPECT_NE(result, std::string::npos);
    EXPECT_GE(result, head);
    if (result != std::string::npos) {
        length += search.length() - 1;
    }

    // check uri length
    EXPECT_EQ(uri.length(), length);

    // ParseUri
    Intent *intentNew = Intent::ParseUri(uri);
    EXPECT_NE(intentNew, nullptr);

    if (intentNew != nullptr) {
        // check new intent
        float floatNew = intentNew->GetFloatParam(keyFloat, 0);
        float floatOld = intentOrigin.GetFloatParam(keyFloat, 1);
        EXPECT_EQ(floatNew, floatOld);

        std::string stringNew = intentNew->GetStringParam(keyString);
        std::string stringOld = intentOrigin.GetStringParam(keyString);
        EXPECT_EQ(stringNew, stringOld);

        delete intentNew;
    }
}

/*
 * Feature: Intent
 * Function: ParseUri and ToUri
 * SubFunction: NA
 * FunctionPoints: ParseUri and ToUri
 * EnvConditions: NA
 * CaseDescription: Verify the function when Intent only has parameter and the parameter
 * has only one float array type element
 */
HWTEST_F(IntentParseToUriTest, AaFwk_Intent_ParseUri_ToUri_005, TestSize.Level1)
{
    std::string search;
    std::size_t pos = 0;
    std::size_t length = 0;
    std::size_t result = 0;
    std::size_t delims = 0;
    std::size_t head = 0;
    Intent intentOrigin;
    std::string keyFloatArray = "keyFloatArray";
    std::vector<float> valueFloatArrayOrigin = {1.1, 2.1, 3.1};
    intentOrigin.SetFloatArrayParam(keyFloatArray, valueFloatArrayOrigin);

    // ToUri
    std::string uri = intentOrigin.ToUri();

    // check head
    search = IntentParseToUriTest::URI_STRING_HEAD;
    result = uri.find(search, pos);
    EXPECT_NE(result, std::string::npos);
    EXPECT_EQ(result, pos);
    if (result != std::string::npos) {
        head = result + search.length();
    }
    length += head;

    // check float array parameter
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
            auto func = [&](IInterface *object) { valueFloatArrayNew.push_back(Float::Unbox(IFloat::Query(object))); };
            Array::ForEach(array, func);
            EXPECT_EQ(valueFloatArrayNew, valueFloatArrayOrigin);
            length += substring.length() + 1;
        }
    }

    // check end
    search = IntentParseToUriTest::URI_STRING_END;
    result = uri.find(search);
    EXPECT_NE(result, std::string::npos);
    EXPECT_GE(result, head);
    if (result != std::string::npos) {
        length += search.length() - 1;
    }

    // check uri length
    EXPECT_EQ(uri.length(), length);

    // ParseUri
    Intent *intentNew = Intent::ParseUri(uri);
    EXPECT_NE(intentNew, nullptr);

    if (intentNew != nullptr) {
        // check new intent
        std::vector<float> arrayNew = intentNew->GetFloatArrayParam(keyFloatArray);
        std::vector<float> arrayOld = intentOrigin.GetFloatArrayParam(keyFloatArray);
        EXPECT_EQ(arrayNew, arrayOld);
        delete intentNew;
    }
}

/*
 * Feature: Intent
 * Function: ParseUri and ToUri
 * SubFunction: NA
 * FunctionPoints: ParseUri and ToUri
 * EnvConditions: NA
 * CaseDescription: Verify the function when Intent only has parameter and the parameter
 * has only one int array and one string array type element
 */
HWTEST_F(IntentParseToUriTest, AaFwk_Intent_ParseUri_ToUri_006, TestSize.Level1)
{
    std::string search;
    std::size_t pos = 0;
    std::size_t length = 0;
    std::size_t result = 0;
    std::size_t delims = 0;
    std::size_t head = 0;
    Intent intentOrigin;
    std::string keyFloatArray = "keyFloatArray";
    std::string keyStringArray = "keyStringArray";
    std::vector<float> valueFloatArrayOrigin = {1.1, 2.1, 3.1};
    std::vector<std::string> valueStringArrayOrigin = {"aa", "bb", "cc"};
    intentOrigin.SetFloatArrayParam(keyFloatArray, valueFloatArrayOrigin);
    intentOrigin.SetStringArrayParam(keyStringArray, valueStringArrayOrigin);

    // ToUri
    std::string uri = intentOrigin.ToUri();

    // check head
    search = IntentParseToUriTest::URI_STRING_HEAD;
    result = uri.find(search, pos);
    EXPECT_NE(result, std::string::npos);
    EXPECT_EQ(result, pos);
    if (result != std::string::npos) {
        head = result + search.length();
    }
    length += head;

    // check int array parameter
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
            auto func = [&](IInterface *object) { valueFloatArrayNew.push_back(Float::Unbox(IFloat::Query(object))); };
            Array::ForEach(array, func);
            EXPECT_EQ(valueFloatArrayNew, valueFloatArrayOrigin);
            length += substring.length() + 1;
        }
    }

    // check string array parameter
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
            auto func = [&](IInterface *object) {
                valueStringArrayNew.push_back(String::Unbox(IString::Query(object)));
            };
            Array::ForEach(array, func);
            EXPECT_EQ(valueStringArrayNew, valueStringArrayOrigin);
            length += substring.length() + 1;
        }
    }

    // check end
    search = IntentParseToUriTest::URI_STRING_END;
    result = uri.find(search);
    EXPECT_NE(result, std::string::npos);
    EXPECT_GE(result, head);
    if (result != std::string::npos) {
        length += search.length() - 1;
    }

    // check uri length
    EXPECT_EQ(uri.length(), length);

    // ParseUri
    Intent *intentNew = Intent::ParseUri(uri);
    EXPECT_NE(intentNew, nullptr);

    if (intentNew != nullptr) {
        // check new intent
        std::vector<float> arrayFloatNew = intentNew->GetFloatArrayParam(keyFloatArray);
        std::vector<float> arrayFloatOld = intentOrigin.GetFloatArrayParam(keyFloatArray);
        EXPECT_EQ(arrayFloatNew, arrayFloatOld);

        std::vector<std::string> arrayStringNew = intentNew->GetStringArrayParam(keyStringArray);
        std::vector<std::string> arrayStringOld = intentOrigin.GetStringArrayParam(keyStringArray);
        EXPECT_EQ(arrayStringNew, arrayStringOld);

        delete intentNew;
    }
}

/*
 * Feature: Intent
 * Function: ParseUri and ToUri
 * SubFunction: NA
 * FunctionPoints: ParseUri and ToUri
 * EnvConditions: NA
 * CaseDescription: Verify the function when the length of input string is 0
 */
HWTEST_F(IntentParseToUriTest, AaFwk_Intent_ParseUri_ToUri_008, TestSize.Level1)
{
    std::string uri;
    EXPECT_EQ(static_cast<int>(uri.length()), 0);

    Intent *intent = Intent::ParseUri(uri);
    EXPECT_EQ(intent, nullptr);

    if (intent != nullptr) {
        delete intent;
    }
}

/*
 * Feature: Intent
 * Function: ParseUri and ToUri
 * SubFunction: NA
 * FunctionPoints: ParseUri and ToUri
 * EnvConditions: NA
 * CaseDescription: Verify the function when the action etc. are empty
 */
HWTEST_F(IntentParseToUriTest, AaFwk_Intent_ParseUri_ToUri_009, TestSize.Level1)
{
    std::string empty;
    std::string uri = "#Intent;action=;entity=;device=;bundle=;ability=;flag=;end";
    EXPECT_NE(static_cast<int>(uri.length()), 0);

    Intent *intent = Intent::ParseUri(uri);
    EXPECT_NE(intent, nullptr);

    if (intent != nullptr) {
        EXPECT_EQ(intent->GetAction(), empty);
        EXPECT_EQ(intent->GetEntity(), empty);
        EXPECT_EQ(intent->GetFlags(), (unsigned int)0);
        ElementName element;
        EXPECT_EQ(intent->GetElement(), element);
        EXPECT_EQ(intent->HasParameter(empty), false);
        delete intent;
    }
}

/*
 * Feature: Intent
 * Function: ParseUri and ToUri
 * SubFunction: NA
 * FunctionPoints: ParseUri and ToUri
 * EnvConditions: NA
 * CaseDescription: Verify the function when flag is not number
 */
HWTEST_F(IntentParseToUriTest, AaFwk_Intent_ParseUri_ToUri_010, TestSize.Level1)
{
    std::string uri = "#Intent;action=intent.action.VIEW;flag=\"123\";end";
    EXPECT_NE(static_cast<int>(uri.length()), 0);

    Intent *intent = Intent::ParseUri(uri);
    EXPECT_EQ(intent, nullptr);
}

/*
 * Feature: Intent
 * Function: ParseUri and ToUri
 * SubFunction: NA
 * FunctionPoints: ParseUri and ToUri
 * EnvConditions: NA
 * CaseDescription: Verify the function when head is not "#Intent"
 */
HWTEST_F(IntentParseToUriTest, AaFwk_Intent_ParseUri_ToUri_011, TestSize.Level1)
{
    std::string uri = "action=intent.action.VIEW;end";
    EXPECT_NE(static_cast<int>(uri.length()), 0);

    Intent *intent = Intent::ParseUri(uri);
    EXPECT_EQ(intent, nullptr);
}

/*
 * Feature: Intent
 * Function: ParseUri and ToUri
 * SubFunction: NA
 * FunctionPoints: ParseUri and ToUri
 * EnvConditions: NA
 * CaseDescription: Verify the function when flag is empty
 */
HWTEST_F(IntentParseToUriTest, AaFwk_Intent_ParseUri_ToUri_012, TestSize.Level1)
{
    std::string uri = "#Intent;flag=;end";
    EXPECT_NE(static_cast<int>(uri.length()), 0);

    Intent *intent = Intent::ParseUri(uri);
    EXPECT_NE(intent, nullptr);

    if (intent != nullptr) {
        EXPECT_EQ(intent->GetFlags(), static_cast<unsigned int>(0));
        delete intent;
    }
}

/*
 * Feature: Intent
 * Function: ParseUri and ToUri
 * SubFunction: NA
 * FunctionPoints: ParseUri and ToUri
 * EnvConditions: NA
 * CaseDescription: Verify the function when x is capital
 */
HWTEST_F(IntentParseToUriTest, AaFwk_Intent_ParseUri_ToUri_013, TestSize.Level1)
{
    unsigned int flag = 0X12345678;
    std::string uri = "#Intent;flag=0X12345678;end";
    EXPECT_NE(static_cast<int>(uri.length()), 0);

    Intent *intent = Intent::ParseUri(uri);
    EXPECT_NE(intent, nullptr);

    if (intent != nullptr) {
        EXPECT_EQ(intent->GetFlags(), flag);
        delete intent;
    }
}

/*
 * Feature: Intent
 * Function: ParseUri and ToUri
 * SubFunction: NA
 * FunctionPoints: ParseUri and ToUri
 * EnvConditions: NA
 * CaseDescription: Verify the function when special character
 */
HWTEST_F(IntentParseToUriTest, AaFwk_Intent_ParseUri_ToUri_014, TestSize.Level1)
{
    std::string action = "\\";
    std::string entity = "../../../jj/j=075/./.;;/07507399/\\\\;;--==.com.\a\b\tfoobar.vide\073\\075";
    unsigned int flag = 0x0f0f0f0f;
    std::string flagStr = "0x0f0f0f0f";
    std::string key = "\\kkk=.=;";
    std::string value = "==\\\\\\.;\\;\\;\\=\\\073075\\\\075073";

    Intent intentOrigin;
    intentOrigin.SetAction(action);
    intentOrigin.SetEntity(entity);
    intentOrigin.SetFlag(flag);
    intentOrigin.SetStringParam(key, value);

    // ToUri
    std::string uri = intentOrigin.ToUri();

    // ParseUri
    Intent *intentNew = Intent::ParseUri(uri);
    EXPECT_NE(intentNew, nullptr);

    if (intentNew != nullptr) {
        // check new intent
        EXPECT_EQ(intentNew->GetAction(), action);
        EXPECT_EQ(intentNew->GetEntity(), entity);
        EXPECT_EQ(intentNew->GetFlags(), flag);
        EXPECT_EQ(intentNew->GetStringParam(key), value);

        delete intentNew;
    }
}

/*
 * Feature: Intent
 * Function: ParseUri and ToUri
 * SubFunction: NA
 * FunctionPoints: ParseUri and ToUri
 * EnvConditions: NA
 * CaseDescription: Verify the function when no '=' or only has a '='
 */
HWTEST_F(IntentParseToUriTest, AaFwk_Intent_ParseUri_ToUri_015, TestSize.Level1)
{
    std::string uri = "#Intent;action;end";
    Intent *intent = Intent::ParseUri(uri);
    EXPECT_EQ(intent, nullptr);

    uri = "#Intent;entity;end";
    intent = Intent::ParseUri(uri);
    EXPECT_EQ(intent, nullptr);

    uri = "#Intent;device;end";
    intent = Intent::ParseUri(uri);
    EXPECT_EQ(intent, nullptr);

    uri = "#Intent;bundle;end";
    intent = Intent::ParseUri(uri);
    EXPECT_EQ(intent, nullptr);

    uri = "#Intent;ability;end";
    intent = Intent::ParseUri(uri);
    EXPECT_EQ(intent, nullptr);

    uri = "#Intent;flag;end";
    intent = Intent::ParseUri(uri);
    EXPECT_EQ(intent, nullptr);

    uri = "#Intent;param;end";
    intent = Intent::ParseUri(uri);
    EXPECT_EQ(intent, nullptr);

    uri = "#Intent;=;end";
    intent = Intent::ParseUri(uri);
    EXPECT_NE(intent, nullptr);
    if (intent != nullptr) {
        delete intent;
    }

    uri = "#Intent;abc=;end";
    intent = Intent::ParseUri(uri);
    EXPECT_NE(intent, nullptr);
    if (intent != nullptr) {
        delete intent;
    }

    uri = "#Intent;=abc;end";
    intent = Intent::ParseUri(uri);
    EXPECT_NE(intent, nullptr);
    if (intent != nullptr) {
        delete intent;
    }

    uri = "#Intent;xxxx=yyy;end";
    intent = Intent::ParseUri(uri);
    EXPECT_NE(intent, nullptr);
    if (intent != nullptr) {
        delete intent;
    }

    uri = "#Intent;;;;;;end";
    intent = Intent::ParseUri(uri);
    EXPECT_NE(intent, nullptr);
    if (intent != nullptr) {
        delete intent;
    }
}
