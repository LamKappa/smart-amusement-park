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

#include <chrono>
#include <string>
#include <thread>
#include <vector>

#include "hilog/log.h"
#include "hisysevent.h"

using namespace testing::ext;
using OHOS::HiviewDFX::HiLogLabel;
using OHOS::HiviewDFX::HiLog;
using OHOS::HiviewDFX::HiSysEvent;
static constexpr HiLogLabel LABEL = { LOG_CORE, 0xD002D08, "HISYSEVENTTEST" };

class HiSysEventTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};


void HiSysEventTest::SetUpTestCase(void)
{
}

void HiSysEventTest::TearDownTestCase(void)
{
}

void HiSysEventTest::SetUp(void)
{
}

void HiSysEventTest::TearDown(void)
{
}

/**
  * @tc.name: HiSysEvent001
  * @tc.desc: Test write.
  * @tc.type: FUNC
 */
HWTEST_F(HiSysEventTest, HiSysEvent001, TestSize.Level1)
{
    /**
     * @tc.steps: step1.make sure SystemAbilityManager is started.
    */
    std::string module = "demo";
    std::string eventName = "start_app";

    bool bValue = true;
    char cValue = 'a';
    short sValue = -100;
    int iValue = -200;
    long lValue = -300;
    long long llValue = -400;

    unsigned char ucValue = 'a';
    unsigned short usValue = 100;
    unsigned int uiValue = 200;
    unsigned long ulValue = 300;
    unsigned long long ullValue = 400;

    float fValue = 1.1;
    double dValue = 2.2;
    std::string strValue = "abc";

    std::vector<bool> bValues;
    bValues.push_back(true);
    bValues.push_back(true);
    bValues.push_back(false);

    std::vector<char> cValues;
    cValues.push_back('a');
    cValues.push_back('b');
    cValues.push_back('c');

    std::vector<unsigned char> ucValues;
    ucValues.push_back('a');
    ucValues.push_back('b');
    ucValues.push_back('c');

    std::vector<short> sValues;
    sValues.push_back(-100);
    sValues.push_back(-200);
    sValues.push_back(-300);

    std::vector<unsigned short> usValues;
    usValues.push_back(100);
    usValues.push_back(200);
    usValues.push_back(300);

    std::vector<int> iValues;
    iValues.push_back(-1000);
    iValues.push_back(-2000);
    iValues.push_back(-3000);

    std::vector<unsigned int> uiValues;
    uiValues.push_back(1000);
    uiValues.push_back(2000);
    uiValues.push_back(3000);

    std::vector<long> lValues;
    lValues.push_back(-10000);
    lValues.push_back(-20000);
    lValues.push_back(-30000);

    std::vector<unsigned long> ulValues;
    ulValues.push_back(10000);
    ulValues.push_back(20000);
    ulValues.push_back(30000);

    std::vector<long long> llValues;
    llValues.push_back(-100000);
    llValues.push_back(-200000);
    llValues.push_back(-300000);

    std::vector<unsigned long long> ullValues;
    ullValues.push_back(100000);
    ullValues.push_back(200000);
    ullValues.push_back(300000);

    std::vector<float> fValues;
    fValues.push_back(1.1);
    fValues.push_back(2.2);
    fValues.push_back(3.3);

    std::vector<double> dValues;
    dValues.push_back(10.1);
    dValues.push_back(20.2);
    dValues.push_back(30.3);

    std::vector<std::string> strValues;
    strValues.push_back(std::string("a"));
    strValues.push_back(std::string("b"));
    strValues.push_back(std::string("c"));

    HiLog::Info(LABEL, "test hisysevent write");
    int result = HiSysEvent::Write(module, eventName, HiSysEvent::EventType::FAULT,
        "keyBool", bValue, "keyChar", cValue, "keyShort", sValue, "keyInt", iValue,
        "KeyLong", lValue, "KeyLongLong", llValue,
        "keyUnsignedChar", ucValue, "keyUnsignedShort", usValue, "keyUnsignedInt", uiValue,
        "keyUnsignedLong", ulValue, "keyUnsignedLongLong", ullValue,
        "keyFloat", fValue, "keyDouble", dValue, "keyString1", strValue, "keyString2", "efg",
        "keyBools", bValues, "keyChars", cValues, "keyUnsignedChars", ucValues,
        "keyShorts", sValues, "keyUnsignedShorts", usValues, "keyInts", iValues, "keyUnsignedInts", uiValues,
        "keyLongs", lValues, "keyUnsignedLongs", ulValues, "keyLongLongs", llValues, "keyUnsignedLongLongs", ullValues,
        "keyFloats", fValues, "keyDoubles", dValues, "keyStrings", strValues);

    ASSERT_TRUE(result == 0);
}