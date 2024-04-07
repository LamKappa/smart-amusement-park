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

#include "ohos/aafwk/content/skills.h"
#include "ohos/aafwk/base/string_wrapper.h"
#include "ohos/aafwk/base/bool_wrapper.h"
#include "ohos/aafwk/base/int_wrapper.h"
#include "ohos/aafwk/base/long_wrapper.h"
#include "utils/native/base/include/refbase.h"

using namespace testing::ext;
using namespace OHOS::AAFwk;
using OHOS::Parcel;


namespace OHOS {
namespace AAFwk {
static const int LARGE_STR_LEN = 65534;
static const int SET_COUNT = 20;
class SkillsBaseTest : public testing::Test {
public:
    SkillsBaseTest()
    {}
    ~SkillsBaseTest()
    {
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    std::shared_ptr<Skills> base_ = nullptr;
    void CompareSkills(const std::shared_ptr<Skills> &skills1, const std::shared_ptr<Skills> &skills2) const;
};

void SkillsBaseTest::SetUpTestCase(void)
{}

void SkillsBaseTest::TearDownTestCase(void)
{}

void SkillsBaseTest::SetUp(void)
{
    base_ = std::make_shared<Skills>();
}

void SkillsBaseTest::TearDown(void)
{
}

/**
 * @tc.number: AaFwk_Skills_Parcelable_0100
 * @tc.name: Marshalling/Unmarshalling
 * @tc.desc: marshalling Skills, and then check result.
 */
HWTEST_F(SkillsBaseTest, AaFwk_Skills_Parcelable_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<Skills> SkillsIn_ = std::make_shared<Skills>();
    if (SkillsIn_ == nullptr) {
        return;
    }
    SkillsIn_->AddEntity("12345");
    SkillsIn_->AddAction("12345");
    SkillsIn_->AddAuthority("12345");
    SkillsIn_->AddScheme("12345");
    SkillsIn_->AddPath("12345");
    SkillsIn_->AddSchemeSpecificPart("12345");
    SkillsIn_->AddType("12345");
    WantParams wantParams;
    std::string keyStr = "12345667";
    bool valueBool = true;
    wantParams.SetParam(keyStr, Boolean::Box(valueBool));
    SkillsIn_->SetWantParams(wantParams);

    Parcel in;
    std::shared_ptr<Skills> SkillsOut_(Skills::Unmarshalling(in));
    SkillsIn_->Marshalling(in);
    if(SkillsOut_ != nullptr){
        CompareSkills(SkillsIn_, SkillsOut_);
        EXPECT_EQ(valueBool, Boolean::Unbox(IBoolean::Query(SkillsOut_->GetWantParams().GetParam(keyStr))));
    }
}

/**
 * @tc.number: AaFwk_Skills_Parcelable_0200
 * @tc.name: Marshalling/Unmarshalling
 * @tc.desc: marshalling Skills, and then check result.
 */
HWTEST_F(SkillsBaseTest, AaFwk_Skills_Parcelable_0200, Function | MediumTest | Level1)
{
    std::shared_ptr<Skills> SkillsIn_ = std::make_shared<Skills>();
    if (SkillsIn_ == nullptr) {
        return;
    }

    SkillsIn_->AddEntity("@#￥#3243adsafdf_中文");
    SkillsIn_->AddAction("@#￥#3243adsafdf_中文");
    SkillsIn_->AddAuthority("@#￥#3243adsafdf_中文");
    SkillsIn_->AddScheme("@#￥#3243adsafdf_中文");
    SkillsIn_->AddPath("@#￥#3243adsafdf_中文");
    SkillsIn_->AddSchemeSpecificPart("@#￥#3243adsafdf_中文");
    SkillsIn_->AddType("@#￥#3243adsafdf_中文");
    WantParams wantParams;
    std::string keyStr = "@#￥#3243adsafdf_中文";
    long valueLong = 12345L;
    wantParams.SetParam(keyStr, Long::Box(valueLong));
    EXPECT_EQ(valueLong, Long::Unbox(ILong::Query(wantParams.GetParam(keyStr)))); 

    SkillsIn_->SetWantParams(wantParams);

    Parcel in;
    SkillsIn_->Marshalling(in);
    std::shared_ptr<Skills> SkillsOut_(Skills::Unmarshalling(in));
   
    if(SkillsOut_ != nullptr){
        CompareSkills(SkillsIn_, SkillsOut_);
        EXPECT_EQ(valueLong, Long::Unbox(ILong::Query(SkillsOut_->GetWantParams().GetParam(keyStr))));   
    }
}

/**
 * @tc.number: AaFwk_Skills_Parcelable_0300
 * @tc.name: Marshalling/Unmarshalling
 * @tc.desc: marshalling Skills, and then check result.
 */
HWTEST_F(SkillsBaseTest, AaFwk_Skills_Parcelable_0300, Function | MediumTest | Level1)
{
    std::shared_ptr<Skills> SkillsIn_ = std::make_shared<Skills>();
    if (SkillsIn_ == nullptr) {
        return;
    }

    SkillsIn_->AddEntity("");
    SkillsIn_->AddAction("");
    SkillsIn_->AddAuthority("");
    SkillsIn_->AddScheme("");
    SkillsIn_->AddPath("");
    SkillsIn_->AddSchemeSpecificPart("");
    SkillsIn_->AddType("");
    WantParams wantParams;
    std::string keyStr = "";
    int valueInt = 123;
    wantParams.SetParam(keyStr, Integer::Box(valueInt));

    
    SkillsIn_->SetWantParams(wantParams);
    EXPECT_EQ(valueInt, Integer::Unbox(IInteger::Query(wantParams.GetParam(keyStr)))); 

    Parcel in;
    SkillsIn_->Marshalling(in);
    std::shared_ptr<Skills> SkillsOut_(Skills::Unmarshalling(in));

    if(SkillsOut_ != nullptr){
        CompareSkills(SkillsIn_, SkillsOut_);
        EXPECT_EQ(valueInt, Integer::Unbox(IInteger::Query(SkillsOut_->GetWantParams().GetParam(keyStr))));
    }
}

/**
 * @tc.number: AaFwk_Skills_Parcelable_0400
 * @tc.name: Marshalling/Unmarshalling
 * @tc.desc: marshalling Skills, and then check result.
 */
HWTEST_F(SkillsBaseTest, AaFwk_Skills_Parcelable_0400, Function | MediumTest | Level1)
{
    std::shared_ptr<Skills> SkillsIn_ = std::make_shared<Skills>();
    if (SkillsIn_ == nullptr) {
        return;
    }
    SkillsIn_->AddEntity("12345");
    SkillsIn_->AddAction("12345");
    SkillsIn_->AddAuthority("12345");
    SkillsIn_->AddScheme("12345");
    SkillsIn_->AddPath("12345");
    SkillsIn_->AddSchemeSpecificPart("12345");
    SkillsIn_->AddType("12345");
    SkillsIn_->AddEntity("@#￥#3243adsafdf_中文");
    SkillsIn_->AddAction("@#￥#3243adsafdf_中文");
    SkillsIn_->AddAuthority("@#￥#3243adsafdf_中文");
    SkillsIn_->AddScheme("@#￥#3243adsafdf_中文");
    SkillsIn_->AddPath("@#￥#3243adsafdf_中文");
    SkillsIn_->AddSchemeSpecificPart("@#￥#3243adsafdf_中文");
    SkillsIn_->AddType("@#￥#3243adsafdf_中文");
    SkillsIn_->AddEntity("");
    SkillsIn_->AddAction("");
    SkillsIn_->AddAuthority("");
    SkillsIn_->AddScheme("");
    SkillsIn_->AddPath("");
    SkillsIn_->AddSchemeSpecificPart("");
    SkillsIn_->AddType("");
    WantParams wantParams;
    std::string keyStr = "12345667";
    std::string valueString = "123";
    wantParams.SetParam(keyStr, String::Box(valueString));
    SkillsIn_->SetWantParams(wantParams);

    Parcel in;
    SkillsIn_->Marshalling(in);
    std::shared_ptr<Skills> SkillsOut_(Skills::Unmarshalling(in));

    if(SkillsOut_ != nullptr){
        CompareSkills(SkillsIn_, SkillsOut_);
        EXPECT_EQ(valueString, String::Unbox(IString::Query(SkillsOut_->GetWantParams().GetParam(keyStr))));
    }
}

void SkillsBaseTest::CompareSkills(const std::shared_ptr<Skills> &skills1, const std::shared_ptr<Skills> &skills2) const
{
    EXPECT_EQ(skills1->CountEntities(), skills2->CountEntities());
    EXPECT_EQ(skills1->CountActions(), skills2->CountActions());
    EXPECT_EQ(skills1->CountAuthorities(), skills2->CountAuthorities());
    EXPECT_EQ(skills1->CountSchemes(), skills2->CountSchemes());
    EXPECT_EQ(skills1->CountPaths(), skills2->CountPaths());
    EXPECT_EQ(skills1->CountSchemeSpecificParts(), skills2->CountSchemeSpecificParts());
    EXPECT_EQ(skills1->CountTypes(), skills2->CountTypes());

    int count = skills1->CountEntities();
    for (int i = 0; i < count; i++) {
        EXPECT_EQ(skills1->GetEntity(i), skills1->GetEntity(i));
    }
    count = skills1->CountActions();
    for (int i = 0; i < count; i++) {
        EXPECT_EQ(skills1->GetAction(i), skills1->GetAction(i));
    }
    count = skills1->CountAuthorities();
    for (int i = 0; i < count; i++) {
        EXPECT_EQ(skills1->GetAuthority(i), skills1->GetAuthority(i));
    }
    count = skills1->CountSchemes();
    for (int i = 0; i < count; i++) {
        EXPECT_EQ(skills1->GetScheme(i), skills1->GetScheme(i));
    }
    count = skills1->CountPaths();
    for (int i = 0; i < count; i++) {
        EXPECT_EQ(skills1->GetPath(i), skills1->GetPath(i));
    }
    count = skills1->CountSchemeSpecificParts();
    for (int i = 0; i < count; i++) {
        EXPECT_EQ(skills1->GetSchemeSpecificPart(i), skills1->GetSchemeSpecificPart(i));
    }
    count = skills1->CountTypes();
    for (int i = 0; i < count; i++) {
        EXPECT_EQ(skills1->GetType(i), skills1->GetType(i));
    }

    std::set<std::string> key1;
    std::set<std::string> key2;
    key1 = skills1->GetWantParams().KeySet();
    key2 = skills2->GetWantParams().KeySet();
    EXPECT_EQ(key1.size(), key2.size());

    if (key1.size() > 0 && key2.size() > 0) {
        std::set<std::string>::iterator iter1 = key1.begin();
        std::set<std::string>::iterator iter2 = key2.begin();
        for (; (iter1 != key1.end() && iter2 != key2.end()); iter1++, iter2++) {
            EXPECT_EQ(*iter1, *iter2);
        }
    }
}

/**
 * @tc.number: AaFwk_Skills_Entities_0100
 * @tc.name: CountEntitie/HasEntity/GetEntity
 * @tc.desc: Verify the function when the input string contains special characters.
 */
HWTEST_F(SkillsBaseTest, AaFwk_Skills_Entities_0100, Function | MediumTest | Level1)
{
    std::string empty;
    std::string entities = "entities.system.test";
    EXPECT_EQ(0, base_->CountEntities());
    EXPECT_EQ(false, base_->HasEntity(entities));
    EXPECT_EQ(empty, base_->GetEntity(0));

    base_->RemoveEntity(entities);
    EXPECT_EQ(0, base_->CountEntities());
    EXPECT_EQ(false, base_->HasEntity(entities));
}
/**
 * @tc.number: AaFwk_Skills_GetEntities_0100
 * @tc.name: AddEntity and GetEntities
 * @tc.desc: Verify AddEntity and GetEntities.
 */
HWTEST_F(SkillsBaseTest, AaFwk_Skills_GetEntities_0100, Function | MediumTest | Level1)
{

    std::string entity = "12345667";
    base_->AddEntity(entity);

    size_t length = base_->GetEntities().size();

    EXPECT_EQ((size_t)1, length);
    EXPECT_EQ(entity, base_->GetEntities().at(0));
}

/**
 * @tc.number: AaFwk_Skills_Authorities_0100
 * @tc.name: CountEntitie/HasEntity/GetEntity
 * @tc.desc: Verify the function when the input string has a long size.
 */
HWTEST_F(SkillsBaseTest, AaFwk_Skills_Authorities_0100, Function | MediumTest | Level1)
{
    std::string empty;
    std::string authorities = "authorities.system.test";
    EXPECT_EQ(0, base_->CountAuthorities());
    EXPECT_EQ(false, base_->HasAuthority(authorities));
    EXPECT_EQ(empty, base_->GetAuthority(0));

    base_->RemoveAuthority(authorities);
    EXPECT_EQ(0, base_->CountAuthorities());
    EXPECT_EQ(false, base_->HasAuthority(authorities));
}

/**
 * @tc.number: AaFwk_Skills_Path_0300
 * @tc.name: CountPaths/HasPath/GetPath
 * @tc.desc: Verify the function when the input string is overrided.
 */
HWTEST_F(SkillsBaseTest, AaFwk_Skills_Path_0300, Function | MediumTest | Level1)
{
    std::string empty;
    std::string path = "paths.system.test";
    PatternsMatcher pm(path, MatchType::PATTERN_LITERAL);
    base_->AddPath(pm);

    EXPECT_EQ(1, base_->CountPaths());
    EXPECT_EQ(true, base_->HasPath(path));
    EXPECT_EQ(path, base_->GetPath(0));

    base_->RemovePath(pm);
    EXPECT_EQ(0, base_->CountPaths());
    EXPECT_EQ(false, base_->HasPath(path));
}

/**
 * @tc.number: AaFwk_Skills_Action_0100
 * @tc.name: AddAction/CountActions/HasAction/GetAction
 * @tc.desc: Verify the function when the input string is set 20 times.
 */
HWTEST_F(SkillsBaseTest, AaFwk_Skills_Action_0100, Function | MediumTest | Level1)
{
    std::string empty;
    std::string action = "action.system.test";
    int actionCount = 1;

    for (int i = 0; i < SET_COUNT; i++) {
        base_->AddAction(action);
    }

    EXPECT_EQ(actionCount, base_->CountActions());
    EXPECT_EQ(true, base_->HasAction(action));
    EXPECT_EQ(action, base_->GetAction(0));

    base_->RemoveAction(action);
    EXPECT_EQ(0, base_->CountActions());
    EXPECT_EQ(false, base_->HasAction(action));
}

/**
 * @tc.number: AaFwk_Skills_Entity_0100
 * @tc.name: CountEntities/HasEntity/CountEntities/GetEntity
 * @tc.desc: Verify the function when the input string is default.
 */
HWTEST_F(SkillsBaseTest, AaFwk_Skills_Entity_0100, Function | MediumTest | Level1)
{
    std::string empty;
    std::string entity = "entity.system.test";
    int entityCount = 1;

    for (int i = 0; i < SET_COUNT; i++) {
        base_->AddEntity(entity);
    }

    EXPECT_EQ(entityCount, base_->CountEntities());
    EXPECT_EQ(true, base_->HasEntity(entity));
    EXPECT_EQ(entity, base_->GetEntity(0));

    base_->RemoveEntity(entity);
    EXPECT_EQ(0, base_->CountEntities());
    EXPECT_EQ(false, base_->HasEntity(entity));
}

/**
 * @tc.number: AaFwk_Skills_Authority_0100
 * @tc.name: CountAuthorities/HasAuthority/GetAuthority
 * @tc.desc: Verify the function when the input string contains special characters.
 */
HWTEST_F(SkillsBaseTest, AaFwk_Skills_Authority_0100, Function | MediumTest | Level1)
{
    std::string empty;
    std::string authority = "Authority.system.test";
    int authorityCount = 1;

    for (int i = 0; i < SET_COUNT; i++) {
        base_->AddAuthority(authority);
    }

    EXPECT_EQ(authorityCount, base_->CountAuthorities());
    EXPECT_EQ(true, base_->HasAuthority(authority));
    EXPECT_EQ(authority, base_->GetAuthority(0));

    base_->RemoveAuthority(authority);
    EXPECT_EQ(0, base_->CountAuthorities());
    EXPECT_EQ(false, base_->HasAuthority(authority));
}

/**
 * @tc.number: AaFwk_Skills_Path_0100
 * @tc.name: CountPaths/HasPath/GetPath
 * @tc.desc: Verify the function when the input string contains special characters.
 */
HWTEST_F(SkillsBaseTest, AaFwk_Skills_Path_0100, Function | MediumTest | Level1)
{
    std::string empty;
    std::string path = "Path.system.test";
    int pathCount = 1;

    for (int i = 0; i < SET_COUNT; i++) {
        base_->AddPath(path);
    }

    EXPECT_EQ(pathCount, base_->CountPaths());
    EXPECT_EQ(true, base_->HasPath(path));
    EXPECT_EQ(path, base_->GetPath(0));

    base_->RemovePath(path);
    EXPECT_EQ(0, base_->CountPaths());
    EXPECT_EQ(false, base_->HasPath(path));
}

/**
 * @tc.number: AaFwk_Skills_Scheme_0100
 * @tc.name: CountSchemes/HasScheme/GetScheme
 * @tc.desc: Verify the function when the input string contains special characters.
 */
HWTEST_F(SkillsBaseTest, AaFwk_Skills_Scheme_0100, Function | MediumTest | Level1)
{
    std::string empty;
    std::string scheme = "scheme.system.test";
    int schemeCount = 1;

    for (int i = 0; i < SET_COUNT; i++) {
        base_->AddScheme(scheme);
    }

    EXPECT_EQ(schemeCount, base_->CountSchemes());
    EXPECT_EQ(true, base_->HasScheme(scheme));
    EXPECT_EQ(scheme, base_->GetScheme(0));

    base_->RemoveScheme(scheme);
    EXPECT_EQ(0, base_->CountSchemes());
    EXPECT_EQ(false, base_->HasScheme(scheme));
}

/**
 * @tc.number: AaFwk_Skills_SchemeSpecificPart_0100
 * @tc.name: CountSchemeSpecificParts/HasSchemeSpecificPart/GetSchemeSpecificPart
 * @tc.desc: Verify the function when the input string contains special characters.
 */
HWTEST_F(SkillsBaseTest, AaFwk_Skills_SchemeSpecificPart_0100, Function | MediumTest | Level1)
{
    std::string empty;
    std::string schemespecificpart = "schemespecificpart.system.test";
    int schemespecificpartCount = 1;

    for (int i = 0; i < SET_COUNT; i++) {
        base_->AddSchemeSpecificPart(schemespecificpart);
    }

    EXPECT_EQ(schemespecificpartCount, base_->CountSchemeSpecificParts());
    EXPECT_EQ(true, base_->HasSchemeSpecificPart(schemespecificpart));
    EXPECT_EQ(schemespecificpart, base_->GetSchemeSpecificPart(0));

    base_->RemoveSchemeSpecificPart(schemespecificpart);
    EXPECT_EQ(0, base_->CountSchemeSpecificParts());
    EXPECT_EQ(false, base_->HasSchemeSpecificPart(schemespecificpart));
}

/**
 * @tc.number: AaFwk_Skills_Type_0100
 * @tc.name: CountTypes/HasType/GetType
 * @tc.desc: Verify the function when the input string contains special characters.
 */
HWTEST_F(SkillsBaseTest, AaFwk_Skills_Type_0100, Function | MediumTest | Level1)
{
    std::string empty;
    std::string type = "type/system.test";
    int typeCount = 1;

    for (int i = 0; i < SET_COUNT; i++) {
        base_->AddType(type);
    }

    EXPECT_EQ(typeCount, base_->CountTypes());
    EXPECT_EQ(true, base_->HasType(type));
    EXPECT_EQ(type, base_->GetType(0));

    base_->RemoveType(type);
    EXPECT_EQ(0, base_->CountTypes());
    EXPECT_EQ(false, base_->HasType(type));
}

/**
 * @tc.number: AaFwk_Skills_Actions_0100
 * @tc.name: CountActions/HasAuthority/GetAuthority
 * @tc.desc: Verify the function when the input string contains special characters.
 */
HWTEST_F(SkillsBaseTest, AaFwk_Skills_Actions_0100, Function | MediumTest | Level1)
{
    std::string empty;
    std::string actions = "actions.system.test";
    EXPECT_EQ(0, base_->CountActions());
    EXPECT_EQ(false, base_->HasAuthority(actions));
    EXPECT_EQ(empty, base_->GetAuthority(0));

    base_->RemoveAuthority(actions);
    EXPECT_EQ(0, base_->CountActions());
    EXPECT_EQ(false, base_->HasAuthority(actions));
}

/**
 * @tc.number: AaFwk_Skills_Schemes_0100
 * @tc.name: CountSchemes/HasAuthority/GetAuthority
 * @tc.desc: Verify the function when the input string contains special characters.
 */
HWTEST_F(SkillsBaseTest, AaFwk_Skills_Schemes_0100, Function | MediumTest | Level1)
{
    std::string empty;
    std::string schemes = "schemes.system.test";
    EXPECT_EQ(0, base_->CountSchemes());
    EXPECT_EQ(false, base_->HasAuthority(schemes));
    EXPECT_EQ(empty, base_->GetAuthority(0));

    base_->RemoveAuthority(schemes);
    EXPECT_EQ(0, base_->CountSchemes());
    EXPECT_EQ(false, base_->HasAuthority(schemes));
}

/**
 * @tc.number: AaFwk_Skills_SchemeSpecificParts_0100
 * @tc.name: CountSchemeSpecificParts/HasAuthority/GetAuthority
 * @tc.desc: Verify the function when the input string contains special characters.
 */
HWTEST_F(SkillsBaseTest, AaFwk_Skills_SchemeSpecificParts_0100, Function | MediumTest | Level1)
{
    std::string empty;
    std::string schemespecificparts = "schemespecificparts.system.test";
    EXPECT_EQ(0, base_->CountSchemeSpecificParts());
    EXPECT_EQ(false, base_->HasAuthority(schemespecificparts));
    EXPECT_EQ(empty, base_->GetAuthority(0));

    base_->RemoveAuthority(schemespecificparts);
    EXPECT_EQ(0, base_->CountSchemeSpecificParts());
    EXPECT_EQ(false, base_->HasAuthority(schemespecificparts));
}

/**
 * @tc.number: AaFwk_Skills_Types_0100
 * @tc.name: CountTypes/HasAuthority/GetAuthority
 * @tc.desc: Verify the function when the input string contains special characters.
 */
HWTEST_F(SkillsBaseTest, AaFwk_Skills_Types_0100, Function | MediumTest | Level1)
{
    std::string empty;
    std::string types = "types.system.test";
    GTEST_LOG_(INFO) << "---------------a ";
    EXPECT_EQ(0, base_->CountTypes());
    GTEST_LOG_(INFO) << "---------------b ";
    EXPECT_EQ(false, base_->HasAuthority(types));
    GTEST_LOG_(INFO) << "---------------1 ";
    EXPECT_EQ(empty, base_->GetAuthority(0));
    GTEST_LOG_(INFO) << "---------------2 ";
    
    base_->RemoveAuthority(types);
    EXPECT_EQ(0, base_->CountTypes());
    EXPECT_EQ(false, base_->HasAuthority(types));
}

/**
 * @tc.number: AaFwk_Skills_Action_0200
 * @tc.name: CountActions/HasAction/GetAction
 * @tc.desc: Verify the function when action is not exist.
 */
HWTEST_F(SkillsBaseTest, AaFwk_Skills_Action_0200, Function | MediumTest | Level1)
{
    std::string empty;
    std::string action = "action.system.test";
    EXPECT_EQ(0, base_->CountActions());
    EXPECT_EQ(false, base_->HasAction(action));
    EXPECT_EQ(empty, base_->GetAction(0));

    base_->RemoveAction(action);
    EXPECT_EQ(0, base_->CountActions());
    EXPECT_EQ(false, base_->HasAction(action));
}

/**
 * @tc.number: AaFwk_Skills_Authority_0200
 * @tc.name: CountAuthorities/HasAuthority/GetAuthority
 * @tc.desc: Verify the function when action is not exist.
 */
HWTEST_F(SkillsBaseTest, AaFwk_Skills_Authority_0200, Function | MediumTest | Level1)
{
    std::string empty;
    std::string authority = "authority.system.test";
    EXPECT_EQ(0, base_->CountAuthorities());
    EXPECT_EQ(false, base_->HasAuthority(authority));
    EXPECT_EQ(empty, base_->GetAuthority(0));

    base_->RemoveAuthority(authority);
    EXPECT_EQ(0, base_->CountAuthorities());
    EXPECT_EQ(false, base_->HasAuthority(authority));
}

/**
 * @tc.number: AaFwk_Skills_Path_0200
 * @tc.name: CountPaths/HasPath/GetPath
 * @tc.desc: Verify the function when action is not exist.
 */
HWTEST_F(SkillsBaseTest, AaFwk_Skills_Path_0200, Function | MediumTest | Level1)
{
    std::string empty;
    std::string path = "path.system.test";
    base_->AddPath(path, MatchType::PATTERN_LITERAL);

    EXPECT_EQ(1, base_->CountPaths());
    EXPECT_EQ(true, base_->HasPath(path));
    EXPECT_EQ(path, base_->GetPath(0));

    base_->RemovePath(path, MatchType::PATTERN_LITERAL);
    EXPECT_EQ(0, base_->CountPaths());
    EXPECT_EQ(false, base_->HasPath(path));
}

/**
 * @tc.number: AaFwk_Skills_Scheme_0200
 * @tc.name: CountSchemes/HasScheme/GetScheme
 * @tc.desc: Verify the function when action is not exist.
 */
HWTEST_F(SkillsBaseTest, AaFwk_Skills_Scheme_0200, Function | MediumTest | Level1)
{
    std::string empty;
    std::string scheme = "scheme.system.test";
    EXPECT_EQ(0, base_->CountSchemes());
    EXPECT_EQ(false, base_->HasScheme(scheme));
    EXPECT_EQ(empty, base_->GetScheme(0));

    base_->RemoveScheme(scheme);
    EXPECT_EQ(0, base_->CountSchemes());
    EXPECT_EQ(false, base_->HasScheme(scheme));
}

/**
 * @tc.number: AaFwk_Skills_SchemeSpecificPart_0200
 * @tc.name: CountSchemeSpecificParts/HasSchemeSpecificPart/GetSchemeSpecificPart
 * @tc.desc: Verify the function when action is not exist.
 */
HWTEST_F(SkillsBaseTest, AaFwk_Skills_SchemeSpecificPart_0200, Function | MediumTest | Level1)
{
    std::string empty;
    std::string schemespecificpart = "schemespecificpart.system.test";
    EXPECT_EQ(0, base_->CountSchemeSpecificParts());
    EXPECT_EQ(false, base_->HasSchemeSpecificPart(schemespecificpart));
    EXPECT_EQ(empty, base_->GetSchemeSpecificPart(0));

    base_->RemoveSchemeSpecificPart(schemespecificpart);
    EXPECT_EQ(0, base_->CountSchemeSpecificParts());
    EXPECT_EQ(false, base_->HasSchemeSpecificPart(schemespecificpart));
}

/**
 * @tc.number: AaFwk_Skills_Type_0300
 * @tc.name: CountTypes/HasType/GetType
 * @tc.desc: Verify the function when action is not exist.
 */
HWTEST_F(SkillsBaseTest, AaFwk_Skills_Type_0300, Function | MediumTest | Level1)
{
    std::string empty;
    std::string type = "type.system.test";
    EXPECT_EQ(0, base_->CountTypes());

    EXPECT_EQ(false, base_->HasType(type));
    EXPECT_EQ(empty, base_->GetType(0));

    base_->RemoveType(type);
    EXPECT_EQ(0, base_->CountTypes());
    EXPECT_EQ(false, base_->HasType(type));
}

using SkillsMatchType = std::tuple<std::string, std::string, bool>;
class SkillsMatchTest : public testing::TestWithParam<SkillsMatchType> {
public:
    SkillsMatchTest() : skills_(nullptr)
    {}
    ~SkillsMatchTest()
    {
        skills_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    Skills *skills_;
};

void SkillsMatchTest::SetUpTestCase(void)
{}

void SkillsMatchTest::TearDownTestCase(void)
{}

void SkillsMatchTest::SetUp(void)
{
    skills_ = new Skills();
}

void SkillsMatchTest::TearDown(void)
{
    delete skills_;
    skills_ = nullptr;
}

/**
 * @tc.number: AaFwk_Skills_match_0100
 * @tc.name: CountTypes/HasType/GetType
 * @tc.desc: Verify whether parameter change.
 */
HWTEST_P(SkillsMatchTest, AaFwk_Skills_match_0100, Function | MediumTest | Level1)
{
    std::string filterEntity = "entity.system.entity1";
    std::string filterAction1 = "action.system.action1";
    std::string filterAction2 = "action.system.action2";
    std::string intentEntity = std::get<0>(GetParam());
    std::string intentAction = std::get<1>(GetParam());
    bool result = std::get<2>(GetParam());

    skills_->AddEntity(filterEntity);
    skills_->AddAction(filterAction1);
    skills_->AddAction(filterAction2);

    Want want;
    want.AddEntity(intentEntity);
    want.SetAction(intentAction);

    EXPECT_EQ(result, skills_->Match(want));
}

INSTANTIATE_TEST_CASE_P(SkillsMatchTestP, SkillsMatchTest,
    testing::Values(SkillsMatchType("entity.system.entityA", "action.system.actionA", false),
        SkillsMatchType("entity.system.entity1", "action.system.actionA", false),
        SkillsMatchType("entity.system.entityA", "action.system.action2", false),
        SkillsMatchType("entity.system.entity1", "action.system.action1", true)));

/**
 * @tc.number: AaFwk_Skills_Skills_0100
 * @tc.name: Skills() and Skills(Skills)
 * @tc.desc:  Verify Skills().
 */
HWTEST_F(SkillsBaseTest, AaFwk_Skills_Skills_0100, Function | MediumTest | Level1)
{
    Skills skills;

    EXPECT_EQ(0, skills.CountEntities());
    EXPECT_EQ(0, skills.CountActions());
    EXPECT_EQ(0, skills.CountAuthorities());
    EXPECT_EQ(0, skills.CountSchemes());

    EXPECT_EQ(0, skills.CountPaths());
    EXPECT_EQ(0, skills.CountSchemeSpecificParts());
    EXPECT_EQ(0, skills.CountTypes());
    EXPECT_EQ(0, skills.GetWantParams().Size());
}

/**
 * @tc.number: AaFwk_Skills_Skills_0200
 * @tc.name: Skills() and Skills(Skills)
 * @tc.desc:  Verify Skills().
 */
HWTEST_F(SkillsBaseTest, AaFwk_Skills_Skills_0200, Function | MediumTest | Level1)
{
    Skills skillsBase;
    std::string entityString = "entity";
    skillsBase.AddEntity(entityString);
    std::string actionString = "action";
    skillsBase.AddAction(actionString);
    std::string authorityString = "authority";
    skillsBase.AddAuthority(authorityString);
    std::string schemeString = "scheme";
    skillsBase.AddScheme(schemeString);
    std::string pathString = "path";
    skillsBase.AddPath(pathString);
    std::string schemeSpecificPartsString = "schemeSpecificParts";
    skillsBase.AddSchemeSpecificPart(schemeSpecificPartsString);
    std::string typeString = "/type";
    skillsBase.AddType(typeString);
    Skills skills(skillsBase);

    EXPECT_EQ(entityString, skills.GetEntity(0));
    EXPECT_EQ(actionString, skills.GetAction(0));
    EXPECT_EQ(authorityString, skills.GetAuthority(0));
    EXPECT_EQ(schemeString, skills.GetScheme(0));

    EXPECT_EQ(pathString, skills.GetPath(0));
    EXPECT_EQ(schemeSpecificPartsString, skills.GetSchemeSpecificPart(0));
    EXPECT_EQ(typeString, skills.GetType(0));
}

/**
 * @tc.number: AaFwk_Skills_addremoveType_0100
 * @tc.name: addType(PatternsMatcher)/ removeType(PatternsMatcher)
 * @tc.desc: Verify addType/removeType result.
 */
HWTEST_F(SkillsBaseTest, AaFwk_Skills_addremoveType_0100, Function | MediumTest | Level1)
{
    std::string patternStr = std::string("systems/*t");

    PatternsMatcher pattern(patternStr, MatchType::PATTERN_LITERAL);

    base_->AddType(pattern);
    std::string type1 = base_->GetType(0);
    EXPECT_EQ(patternStr, type1);

    base_->RemoveType(patternStr);

    EXPECT_EQ(0, base_->CountEntities());

    base_->AddType(pattern);

    std::string patternStr2 = std::string("systems/*test");
    PatternsMatcher pattern2(patternStr2, MatchType::PATTERN_PREFIX);
    base_->AddType(pattern2);
    std::string type2 = base_->GetType(1);
    EXPECT_EQ(patternStr2, type2);

    base_->RemoveType(pattern2);
    EXPECT_EQ(0, base_->CountEntities());

    std::string patternStr3 = std::string("systems/*test3");
    base_->AddType(patternStr3, MatchType::PATTERN_SIMPLE_GLOB);

    std::string type3 = base_->GetType(1);
    EXPECT_EQ(patternStr3, type3);

    base_->RemoveType(patternStr3, MatchType::PATTERN_SIMPLE_GLOB);

    EXPECT_EQ(0, base_->CountEntities());
}

using testParamsType = std::tuple<std::string, std::string>;
class SkillsParamsTest : public testing::TestWithParam<testParamsType> {
public:
    SkillsParamsTest() 
    {}
    ~SkillsParamsTest()
    {
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    std::shared_ptr<Skills> skills_ = nullptr;
};

void SkillsParamsTest::SetUpTestCase(void)
{}

void SkillsParamsTest::TearDownTestCase(void)
{}

void SkillsParamsTest::SetUp(void)
{
    skills_ = std::make_shared<Skills>();
}

void SkillsParamsTest::TearDown(void)
{
}

/**
 * @tc.number: AaFwk_Skills_Params_0100
 * @tc.name: SetWantParams/GetWantParams
 * @tc.desc: Verify addType/removeType result.
 */
HWTEST_P(SkillsParamsTest, AaFwk_Skills_Params_0100, Function | MediumTest | Level1)
{
    std::string keyStr = std::get<0>(GetParam());
    std::string valueStr = std::get<1>(GetParam());
    WantParams wantParams;
    wantParams.SetParam(keyStr, String::Box(valueStr));
    skills_->SetWantParams(wantParams);
    EXPECT_EQ(valueStr, String::Unbox(IString::Query(skills_->GetWantParams().GetParam(keyStr))));
}

INSTANTIATE_TEST_CASE_P(SkillsParamsTestCaseP, SkillsParamsTest,
    testing::Values(testParamsType("", "asdsdsdasa"), testParamsType(std::string(LARGE_STR_LEN + 1, 's'), "sadsdsdads"),
        testParamsType("#$%^&*(!@\":<>{}", "asdsdsdasa"), testParamsType("3456677", ""),
        testParamsType("1234", std::string(LARGE_STR_LEN + 1, 's')),
        testParamsType("2323sdasdZ", "#$%^&*(!@\":<>{}sadsdasdsaf"), testParamsType("12345667", "sdasdfdsffdgfdg"),
        testParamsType("", ""),
        testParamsType(std::string(LARGE_STR_LEN + 1, 'k'), std::string(LARGE_STR_LEN + 1, 'k')),
        testParamsType("#$%^&*(!@\":<>{},/", "#$%^&*(!@\":<>{},/")));

}
}