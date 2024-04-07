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
#include "res_config_impl_test.h"

#include <climits>
#include <cstring>
#include <gtest/gtest.h>

#include "res_config_impl.h"
#include "test_common.h"

using namespace OHOS::Global::Resource;
using namespace testing::ext;

class ResConfigImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void ResConfigImplTest::SetUpTestCase(void)
{
    // step 1: input testsuit setup step
}

void ResConfigImplTest::TearDownTestCase(void)
{
    // step 2: input testsuit teardown step
}

void ResConfigImplTest::SetUp()
{
}

void ResConfigImplTest::TearDown()
{
}

ResConfigImpl *CreateResConfigImpl(const char *language, const char *script, const char *region)
{
    ResConfigImpl *resConfigImpl = new ResConfigImpl;
    resConfigImpl->SetLocaleInfo(language, script, region);
    return resConfigImpl;
}

HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest001, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl(nullptr, nullptr, nullptr);
    ResConfigImpl *current = CreateResConfigImpl("fr", nullptr, "CA");
    EXPECT_TRUE(current->Match(other));
    delete current;
    delete other;
};

HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest002, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "CA");
    ResConfigImpl *current = CreateResConfigImpl("fr", nullptr, "CA");
    EXPECT_FALSE(current->Match(other));
    delete current;
    delete other;
};

HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest003, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("tl", nullptr, "PH");
    ResConfigImpl *current = CreateResConfigImpl("fil", nullptr, "PH");
    EXPECT_TRUE(current->Match(other));
    delete current;
    delete other;
};

HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest004, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("qaa", nullptr, nullptr);
    ResConfigImpl *current = CreateResConfigImpl("qaa", nullptr, "CA");
    EXPECT_TRUE(current->Match(other));
    delete current;
    delete other;
};

HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest005, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("qaa", nullptr, "CA");
    ResConfigImpl *current = CreateResConfigImpl("qaa", nullptr, "CA");
    EXPECT_TRUE(current->Match(other));
    delete current;
    delete other;
};

HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest006, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("az", nullptr, nullptr);
    ResConfigImpl *current = CreateResConfigImpl("az", "Latn", nullptr);
    EXPECT_TRUE(current->Match(other));
    delete current;
    delete other;
};

HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest007, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("az", nullptr, "IR");
    ResConfigImpl *current = CreateResConfigImpl("az", "Arab", nullptr);
    EXPECT_TRUE(current->Match(other));
    delete current;
    delete other;
};

HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest008, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("ar", nullptr, "EG");
    ResConfigImpl *current = CreateResConfigImpl("ar", nullptr, "TN");
    EXPECT_TRUE(current->Match(other));
    delete current;
    delete other;
};

HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest009, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("qaa", "Latn", "FR");
    ResConfigImpl *current = CreateResConfigImpl("qaa", nullptr, "CA");
    EXPECT_FALSE(current->Match(other));
    delete current;
    delete other;
};

HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest010, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("qaa", nullptr, "FR");
    ResConfigImpl *current = CreateResConfigImpl("qaa", "Latn", "CA");
    EXPECT_FALSE(current->Match(other));
    delete current;
    delete other;
};

HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest011, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("az", nullptr, nullptr);
    ResConfigImpl *current = CreateResConfigImpl("az", "Cyrl", nullptr);
    EXPECT_FALSE(current->Match(other));
    delete current;
    delete other;
};

HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest012, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("az", nullptr, nullptr);
    ResConfigImpl *current = CreateResConfigImpl("az", nullptr, "IR");
    EXPECT_FALSE(current->Match(other));
    delete current;
    delete other;
};

HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest013, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("qaa", nullptr, "FR");
    ResConfigImpl *current = CreateResConfigImpl("qaa", nullptr, "CA");
    EXPECT_FALSE(current->Match(other));
    delete current;
    delete other;
};

HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest014, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("he", nullptr, nullptr);
    ResConfigImpl *current = CreateResConfigImpl("iw", nullptr, nullptr);
    EXPECT_TRUE(current->Match(other));
    EXPECT_TRUE(other->Match(current));
    delete current;
    delete other;
};

HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest015, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("ji", nullptr, nullptr);
    ResConfigImpl *current = CreateResConfigImpl("yi", nullptr, nullptr);
    EXPECT_TRUE(current->Match(other));
    EXPECT_TRUE(other->Match(current));
    delete current;
    delete other;
};

HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest016, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("jw", nullptr, nullptr);
    ResConfigImpl *current = CreateResConfigImpl("jv", nullptr, nullptr);
    EXPECT_TRUE(current->Match(other));
    EXPECT_TRUE(other->Match(current));
    delete current;
    delete other;
};

HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest017, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("in", nullptr, nullptr);
    ResConfigImpl *current = CreateResConfigImpl("id", nullptr, nullptr);
    EXPECT_TRUE(current->Match(other));
    EXPECT_TRUE(other->Match(current));
    delete current;
    delete other;
};

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest001, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl(nullptr, nullptr, nullptr);
    ResConfigImpl *current = CreateResConfigImpl("fr", nullptr, "FR");
    ResConfigImpl *other = CreateResConfigImpl("fr", nullptr, "CA");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_TRUE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest002, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("fr", nullptr, "CA");
    ResConfigImpl *current = CreateResConfigImpl(nullptr, nullptr, nullptr);
    ResConfigImpl *other = CreateResConfigImpl(nullptr, nullptr, nullptr);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_TRUE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest003, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("fr", nullptr, "CA");
    ResConfigImpl *current = CreateResConfigImpl("fr", nullptr, "FR");
    ResConfigImpl *other = CreateResConfigImpl(nullptr, nullptr, nullptr);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest004, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("fil", nullptr, "PH");
    ResConfigImpl *current = CreateResConfigImpl("tl", nullptr, "PH");
    ResConfigImpl *other = CreateResConfigImpl("fil", nullptr, "US");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest005, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("fil", nullptr, "PH");
    ResConfigImpl *current = CreateResConfigImpl("fil", nullptr, "PH");
    ResConfigImpl *other = CreateResConfigImpl("tl", nullptr, "PH");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest006, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("es", nullptr, "AR");
    ResConfigImpl *current = CreateResConfigImpl("es", nullptr, "419");
    ResConfigImpl *other = CreateResConfigImpl("es", nullptr, "419");
    EXPECT_FALSE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest007, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("es", nullptr, "AR");
    ResConfigImpl *current = CreateResConfigImpl("es", nullptr, "AR");
    ResConfigImpl *other = CreateResConfigImpl("es", nullptr, "419");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest008, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("es", nullptr, "AR");
    ResConfigImpl *current = CreateResConfigImpl("es", nullptr, "419");
    ResConfigImpl *other = CreateResConfigImpl("es", nullptr, nullptr);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest009, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("es", nullptr, "AR");
    ResConfigImpl *current = CreateResConfigImpl("es", nullptr, "419");
    ResConfigImpl *other = CreateResConfigImpl("es", nullptr, "ES");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest010, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("es", nullptr, "AR");
    ResConfigImpl *current = CreateResConfigImpl("es", nullptr, nullptr);
    ResConfigImpl *other = CreateResConfigImpl("es", nullptr, "ES");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest011, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("es", nullptr, "AR");
    ResConfigImpl *current = CreateResConfigImpl("es", nullptr, "PE");
    ResConfigImpl *other = CreateResConfigImpl("es", nullptr, "ES");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest012, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("es", nullptr, "AR");
    ResConfigImpl *current = CreateResConfigImpl("es", nullptr, "AR");
    ResConfigImpl *other = CreateResConfigImpl("es", nullptr, nullptr);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest013, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("es", nullptr, "AR");
    ResConfigImpl *current = CreateResConfigImpl("es", nullptr, "US");
    ResConfigImpl *other = CreateResConfigImpl("es", nullptr, "BO");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest014, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("es", nullptr, "IC");
    ResConfigImpl *current = CreateResConfigImpl("es", nullptr, "ES");
    ResConfigImpl *other = CreateResConfigImpl("es", nullptr, "GQ");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest015, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("es", nullptr, "GQ");
    ResConfigImpl *current = CreateResConfigImpl("es", nullptr, "IC");
    ResConfigImpl *other = CreateResConfigImpl("es", nullptr, "419");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest016, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "GB");
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, "001");
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, nullptr);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest017, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "PR");
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, nullptr);
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "001");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest018, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "DE");
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, "150");
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "001");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest019, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "IN");
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, "AU");
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "US");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest020, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "PR");
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, "001");
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "GB");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest021, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "IN");
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, "GB");
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "AU");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest022, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "IN");
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, "AU");
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "CA");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest023, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("pt", nullptr, "MZ");
    ResConfigImpl *current = CreateResConfigImpl("pt", nullptr, "PT");
    ResConfigImpl *other = CreateResConfigImpl("pt", nullptr, nullptr);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest024, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("pt", nullptr, "MZ");
    ResConfigImpl *current = CreateResConfigImpl("pt", nullptr, "PT");
    ResConfigImpl *other = CreateResConfigImpl("pt", nullptr, "BR");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest025, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("zh", "Hant", "MO");
    ResConfigImpl *current = CreateResConfigImpl("zh", "Hant", "HK");
    ResConfigImpl *other = CreateResConfigImpl("zh", "Hant", "TW");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest026, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("zh", "Hant", "US");
    ResConfigImpl *current = CreateResConfigImpl("zh", "Hant", "TW");
    ResConfigImpl *other = CreateResConfigImpl("zh", "Hant", "HK");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest027, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("ar", nullptr, "DZ");
    ResConfigImpl *current = CreateResConfigImpl("ar", nullptr, "015");
    ResConfigImpl *other = CreateResConfigImpl("ar", nullptr, nullptr);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest028, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("ar", nullptr, "EG");
    ResConfigImpl *current = CreateResConfigImpl("ar", nullptr, nullptr);
    ResConfigImpl *other = CreateResConfigImpl("ar", nullptr, "015");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest029, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("ar", nullptr, "QA");
    ResConfigImpl *current = CreateResConfigImpl("ar", nullptr, "EG");
    ResConfigImpl *other = CreateResConfigImpl("ar", nullptr, "BH");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest030, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("ar", nullptr, "QA");
    ResConfigImpl *current = CreateResConfigImpl("ar", nullptr, "SA");
    ResConfigImpl *other = CreateResConfigImpl("ar", nullptr, "015");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest031, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "US");
    ResConfigImpl *current = CreateResConfigImpl(nullptr, nullptr, nullptr);
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "001");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest032, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "US");
    ResConfigImpl *current = CreateResConfigImpl(nullptr, nullptr, nullptr);
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "GB");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest033, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "PR");
    ResConfigImpl *current = CreateResConfigImpl(nullptr, nullptr, nullptr);
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "001");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest034, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "US");
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, nullptr);
    ResConfigImpl *other = CreateResConfigImpl(nullptr, nullptr, nullptr);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest035, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "PR");
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, nullptr);
    ResConfigImpl *other = CreateResConfigImpl(nullptr, nullptr, nullptr);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest036, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "US");
    ResConfigImpl *current = CreateResConfigImpl(nullptr, nullptr, nullptr);
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "PR");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest037, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "CN");
    ResConfigImpl *current = CreateResConfigImpl(nullptr, nullptr, nullptr);
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "GB");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest038, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", "Qaag", nullptr);
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, "GB");
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "CA");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest039, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", "Qaag", nullptr);
    ResConfigImpl *current = CreateResConfigImpl(nullptr, nullptr, nullptr);
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "CA");
    EXPECT_FALSE(current->IsMoreSuitable(other, request));
    EXPECT_TRUE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest040, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "US");
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, "CN");
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "GB");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}