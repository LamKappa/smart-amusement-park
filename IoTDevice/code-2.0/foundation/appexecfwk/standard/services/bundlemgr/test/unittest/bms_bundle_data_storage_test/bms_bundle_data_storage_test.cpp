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
#include <fstream>

#include "app_log_wrapper.h"
#include "json_constants.h"
#include "json_serializer.h"
#include "nlohmann/json.hpp"
#include "ability_info.h"
#include "bundle_constants.h"
#include "bundle_data_storage.h"
#include "bundle_info.h"
#include "inner_bundle_info.h"

using namespace testing::ext;
using namespace OHOS::AppExecFwk;
using namespace OHOS::AppExecFwk::JsonConstants;

namespace {
const std::string NORMAL_BUNDLE_NAME{"com.example.test"};
}  // namespace

class BmsBundleDataStorageTest : public testing::Test {
public:
    BmsBundleDataStorageTest();
    ~BmsBundleDataStorageTest();
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    void ClearJsonFile() const;

protected:
    enum class InfoType {
        BUNDLE_INFO,
        APPLICATION_INFO,
        ABILITY_INFO,
    };

    void CheckBundleSaved(const InnerBundleInfo &innerBundleInfo) const;
    void CheckBundleDeleted(const InnerBundleInfo &innerBundleInfo) const;
    void CheckInvalidPropDeserialize(const nlohmann::json infoJson, const InfoType infoType) const;

protected:
    nlohmann::json innerBundleInfoJson_ = R"(
        {
            "appFeature": "ohos_system_app",
            "appType": 2,
            "baseAbilityInfos": {
                "com.ohos.launchercom.ohos.launchercom.ohos.launcher.MainAbility": {
                    "applicationName": "com.ohos.launcher",
                    "bundleName": "com.ohos.launcher",
                    "codePath": "",
                    "description": "$string:mainability_description",
                    "deviceCapabilities": [],
                    "deviceId": "",
                    "deviceTypes": [
                        "phone"
                    ],
                    "iconPath": "$media:icon",
                    "isLauncherAbility": true,
                    "isNativeAbility": false,
                    "kind": "page",
                    "label": "Launcher",
                    "launchMode": 0,
                    "libPath": "",
                    "moduleName": ".MyApplication",
                    "name": "com.ohos.launcher.MainAbility",
                    "orientation": 0,
                    "package": "com.ohos.launcher",
                    "permissions": [],
                    "process": "",
                    "resourcePath": "/data/accounts/account_0/applications/com.ohos.launcher/com.ohos.launcher/assets/launcher/resources.index",
                    "type": 1,
                    "uri": "",
                    "visible": false
                }
            },
            "baseApplicationInfo": {
                "bundleName": "com.ohos.launcher",
                "cacheDir": "/data/accounts/account_0/appdata/com.ohos.launcher/cache",
                "codePath": "/data/accounts/account_0/applications/com.ohos.launcher",
                "dataBaseDir": "/data/accounts/account_0/appdata/com.ohos.launcher/database",
                "dataDir": "/data/accounts/account_0/appdata/com.ohos.launcher/files",
                "description": "$string:mainability_description",
                "descriptionId": 16777217,
                "deviceId": "PHONE-001",
                "entryDir": "",
                "iconId": 16777218,
                "iconPath": "$media:icon",
                "isLauncherApp": true,
                "isSystemApp": false,
                "label": "Launcher",
                "labelId": 0,
                "moduleInfos": [],
                "moduleSourceDirs": [],
                "name": "com.ohos.launcher",
                "permissions": [],
                "process": "",
                "signatureKey": "",
                "supportedModes": 0
            },
            "baseBundleInfo": {
                "abilityInfos": [],
                "appId": "",
                "applicationInfo": {
                    "bundleName": "",
                    "cacheDir": "",
                    "codePath": "",
                    "dataBaseDir": "",
                    "dataDir": "",
                    "description": "",
                    "descriptionId": 0,
                    "deviceId": "",
                    "entryDir": "",
                    "iconId": 0,
                    "iconPath": "",
                    "isLauncherApp": false,
                    "isSystemApp": false,
                    "label": "",
                    "labelId": 0,
                    "moduleInfos": [],
                    "moduleSourceDirs": [],
                    "name": "",
                    "permissions": [],
                    "process": "",
                    "signatureKey": "",
                    "supportedModes": 0
                },
                "compatibleVersion": 3,
                "cpuAbi": "",
                "defPermissions": [],
                "description": "",
                "entryModuleName": "",
                "gid": 10000,
                "hapModuleNames": [],
                "installTime": 17921,
                "isKeepAlive": false,
                "isNativeApp": false,
                "jointUserId": "",
                "label": "Launcher",
                "mainEntry": "",
                "maxSdkVersion": 0,
                "minSdkVersion": 0,
                "moduleDirs": [],
                "moduleNames": [],
                "modulePublicDirs": [],
                "moduleResPaths": [],
                "name": "com.ohos.launcher",
                "releaseType": "Release",
                "reqPermissions": [],
                "seInfo": "",
                "targetVersion": 3,
                "uid": 10000,
                "updateTime": 17921,
                "vendor": "ohos",
                "versionCode": 1,
                "versionName": "1.0"
            },
            "baseDataDir": "/data/accounts/account_0/appdata/com.ohos.launcher",
            "bundleStatus": 1,
            "gid": 10000,
            "hasEntry": true,
            "innerModuleInfos": {
                "com.ohos.launcher": {
                    "abilityKeys": [
                        "com.ohos.launchercom.ohos.launchercom.ohos.launcher.MainAbility"
                    ],
                    "defPermissions": [],
                    "description": "",
                    "distro": {
                        "deliveryWithInstall": true,
                        "moduleName": "launcher",
                        "moduleType": "entry"
                    },
                    "isEntry": true,
                    "metaData": {
                        "customizeData": [],
                        "parameters": [],
                        "results": []
                    },
                    "moduleDataDir": "/data/accounts/account_0/appdata/com.ohos.launcher/com.ohos.launcher",
                    "moduleName": ".MyApplication",
                    "modulePackage": "com.ohos.launcher",
                    "modulePath": "/data/accounts/account_0/applications/com.ohos.launcher/com.ohos.launcher",
                    "moduleResPath": "/data/accounts/account_0/applications/com.ohos.launcher/com.ohos.launcher/assets/launcher/resources.index",
                    "reqCapabilities": [],
                    "reqPermissions": [],
                    "skillKeys": [
                        "com.ohos.launchercom.ohos.launchercom.ohos.launcher.MainAbility"
                    ]
                }
            },
            "isKeepData": false,
            "isSupportBackup": false,
            "mainAbility": "com.ohos.launchercom.ohos.launchercom.ohos.launcher.MainAbility",
            "provisionId": "BNtg4JBClbl92Rgc3jm/RfcAdrHXaM8F0QOiwVEhnV5ebE5jNIYnAx+weFRT3QTyUjRNdhmc2aAzWyi+5t5CoBM=",
            "skillInfos": {
                "com.ohos.launchercom.ohos.launchercom.ohos.launcher.MainAbility": [
                    {
                        "actions": [
                            "action.system.home",
                            "com.ohos.action.main"
                        ],
                        "entities": [
                            "entity.system.home",
                            "flag.home.intent.from.system"
                        ],
                        "uris": []
                    }
                ]
            },
            "uid": 10000,
            "userId_": 0
        }
    )"_json;

    nlohmann::json moduleInfoJson_ = R"(
        {
            "moduleName": "entry",
            "moduleSourceDir": ""
        }
    )"_json;
    const std::string deviceId_ = Constants::CURRENT_DEVICE_ID;
    const std::string BASE_ABILITY_INFO = "baseAbilityInfos";
    // need modify with innerBundleInfoJson_
    const std::string abilityName = "com.ohos.launchercom.ohos.launchercom.ohos.launcher.MainAbility";
    const std::string BASE_BUNDLE_INFO = "baseBundleInfo";
    const std::string BASE_APPLICATION_INFO = "baseApplicationInfo";
};

BmsBundleDataStorageTest::BmsBundleDataStorageTest()
{}

BmsBundleDataStorageTest::~BmsBundleDataStorageTest()
{}

void BmsBundleDataStorageTest::CheckBundleSaved(const InnerBundleInfo &innerBundleInfo) const
{
    BundleDataStorage bundleDataStorage;
    ASSERT_TRUE(bundleDataStorage.SaveStorageBundleInfo(Constants::CURRENT_DEVICE_ID, innerBundleInfo));
    std::map<std::string, std::map<std::string, InnerBundleInfo>> bundleData;
    ASSERT_TRUE(bundleDataStorage.LoadAllData(bundleData));

    // search allDeviceInfos by bundle name
    std::string bundleName = innerBundleInfo.GetBundleName();
    auto bundleDataIter = bundleData.find(bundleName);
    ASSERT_TRUE(bundleDataIter != bundleData.end());

    // search InnerBundleInfo by device id
    auto allDeviceInfos = bundleDataIter->second;
    auto devicesInfosIter = allDeviceInfos.find(deviceId_);
    ASSERT_TRUE(devicesInfosIter != allDeviceInfos.end());

    InnerBundleInfo afterLoadInfo = devicesInfosIter->second;
    ASSERT_TRUE(innerBundleInfo.ToString() == afterLoadInfo.ToString());
}

void BmsBundleDataStorageTest::CheckBundleDeleted(const InnerBundleInfo &innerBundleInfo) const
{
    BundleDataStorage bundleDataStorage;
    ASSERT_TRUE(bundleDataStorage.DeleteStorageBundleInfo(Constants::CURRENT_DEVICE_ID, innerBundleInfo));
    std::map<std::string, std::map<std::string, InnerBundleInfo>> bundleDates;
    ASSERT_FALSE(bundleDataStorage.LoadAllData(bundleDates));
}

void BmsBundleDataStorageTest::CheckInvalidPropDeserialize(const nlohmann::json infoJson, const InfoType infoType) const
{
    APP_LOGI("deserialize infoJson = %{public}s", infoJson.dump().c_str());
    bool throwError = false;
    nlohmann::json innerBundleInfoJson;
    nlohmann::json bundleInfoJson = innerBundleInfoJson_.at(BASE_BUNDLE_INFO);
    try {
        switch (infoType) {
            case InfoType::BUNDLE_INFO: {
                bundleInfoJson = infoJson;
                BundleInfo bundleInfo = infoJson;
                break;
            }
            case InfoType::APPLICATION_INFO: {
                bundleInfoJson["appInfo"] = infoJson;
                ApplicationInfo applicationInfo = infoJson;
                break;
            }
            case InfoType::ABILITY_INFO: {
                bundleInfoJson["abilityInfos"].push_back(infoJson);
                AbilityInfo abilityInfo = infoJson;
                break;
            }
            default:
                break;
        }
    } catch (nlohmann::detail::type_error exception) {
        APP_LOGI("has a type_error: %{public}s", exception.what());
        throwError = true;
    }

    ASSERT_TRUE(throwError);
    if (!throwError) {
        GTEST_LOG_(ERROR) << "not catch any type_error";
    }

    innerBundleInfoJson["baseBundleInfo"] = bundleInfoJson;
    InnerBundleInfo fromJsonInfo;
    ASSERT_FALSE(fromJsonInfo.FromJson(innerBundleInfoJson));
}

void BmsBundleDataStorageTest::SetUpTestCase()
{}

void BmsBundleDataStorageTest::TearDownTestCase()
{}

void BmsBundleDataStorageTest::SetUp()
{
    // clean bmsdb.json
    ClearJsonFile();
}

void BmsBundleDataStorageTest::TearDown()
{}

void BmsBundleDataStorageTest::ClearJsonFile() const
{
    std::string fileName = Constants::BUNDLE_DATA_BASE_FILE;
    std::ofstream o(fileName);
    if (!o.is_open()) {
        return;
    }
    o.close();
}

/**
 * @tc.number: BundleInfoJsonSerializer_0100
 * @tc.name: save bundle installation information to persist storage
 * @tc.desc: 1.system running normally
 *           2.successfully serialize and deserialize all right props in BundleInfo
 */
HWTEST_F(BmsBundleDataStorageTest, BundleInfoJsonSerializer_0100, Function | SmallTest | Level1)
{
    nlohmann::json sourceInfoJson = innerBundleInfoJson_.at(BASE_BUNDLE_INFO);
    // deserialize BundleInfo from json
    BundleInfo fromJsonInfo = sourceInfoJson;
    // serialize fromJsonInfo to json
    nlohmann::json toJsonObject = fromJsonInfo;

    ASSERT_TRUE(toJsonObject.dump() == sourceInfoJson.dump());
}

/**
 * @tc.number: BundleInfoJsonSerializer_0200
 * @tc.name: save bundle installation information to persist storage
 * @tc.desc: 1.system running normally
 *           2.test can catch deserialize error for type error for name prop in BundleInfo
 */
HWTEST_F(BmsBundleDataStorageTest, BundleInfoJsonSerializer_0200, Function | SmallTest | Level1)
{
    nlohmann::json typeErrorProps;
    typeErrorProps["name"] = NOT_STRING_TYPE;
    typeErrorProps["label"] = NOT_STRING_TYPE;
    typeErrorProps["description"] = NOT_STRING_TYPE;
    typeErrorProps["vendor"] = NOT_STRING_TYPE;
    typeErrorProps["mainEntry"] = NOT_STRING_TYPE;
    typeErrorProps["versionName"] = NOT_STRING_TYPE;
    typeErrorProps["versionCode"] = NOT_NUMBER_TYPE;
    typeErrorProps["minSdkVersion"] = NOT_NUMBER_TYPE;
    typeErrorProps["minSdkVersion"] = NOT_NUMBER_TYPE;

    for (nlohmann::json::iterator iter = typeErrorProps.begin(); iter != typeErrorProps.end(); iter++) {
        for (auto valueIter = iter.value().begin(); valueIter != iter.value().end(); valueIter++) {
            nlohmann::json infoJson = innerBundleInfoJson_.at(BASE_BUNDLE_INFO);
            infoJson[iter.key()] = valueIter.value();
        }
    }
}

/**
 * @tc.number: AbilityInfoJsonSerializer_0100
 * @tc.name: save bundle installation information to persist storage
 * @tc.desc: 1.system running normally
 *           2.successfully serialize and deserialize all right props in AbilityInfo
 */
HWTEST_F(BmsBundleDataStorageTest, AbilityInfoJsonSerializer_0100, Function | SmallTest | Level1)
{
    nlohmann::json sourceInfoJson = innerBundleInfoJson_.at(BASE_ABILITY_INFO).at(abilityName);
    // deserialize AbilityInfo from json
    AbilityInfo fromJsonInfo = sourceInfoJson;
    // serialize fromJsonInfo to json
    nlohmann::json toJsonObject = fromJsonInfo;
    ASSERT_TRUE(toJsonObject.dump() == sourceInfoJson.dump());
}

/**
 * @tc.number: AbilityInfoJsonSerializer_0200
 * @tc.name: save bundle installation information to persist storage
 * @tc.desc: 1.system running normally
 *           2.test can catch deserialize error for type error for name prop in AbilityInfo
 */
HWTEST_F(BmsBundleDataStorageTest, AbilityInfoJsonSerializer_0200, Function | SmallTest | Level1)
{
    nlohmann::json typeErrorProps;
    typeErrorProps["package"] = NOT_STRING_TYPE;
    typeErrorProps["name"] = NOT_STRING_TYPE;
    typeErrorProps["bundleName"] = NOT_STRING_TYPE;
    typeErrorProps["applicationName"] = NOT_STRING_TYPE;
    typeErrorProps["label"] = NOT_STRING_TYPE;
    typeErrorProps["description"] = NOT_STRING_TYPE;
    typeErrorProps["iconPath"] = NOT_STRING_TYPE;
    typeErrorProps["visible"] = NOT_BOOL_TYPE;
    typeErrorProps["kind"] = NOT_STRING_TYPE;
    typeErrorProps["type"] = NOT_NUMBER_TYPE;
    typeErrorProps["orientation"] = NOT_NUMBER_TYPE;
    typeErrorProps["launchMode"] = NOT_NUMBER_TYPE;
    typeErrorProps["codePath"] = NOT_STRING_TYPE;
    typeErrorProps["resourcePath"] = NOT_STRING_TYPE;
    typeErrorProps["libPath"] = NOT_STRING_TYPE;

    for (nlohmann::json::iterator iter = typeErrorProps.begin(); iter != typeErrorProps.end(); iter++) {
        for (auto valueIter = iter.value().begin(); valueIter != iter.value().end(); valueIter++) {
            APP_LOGD("deserialize check prop key = %{public}s, type = %{public}s",
                iter.key().c_str(),
                valueIter.key().c_str());
            nlohmann::json infoJson = innerBundleInfoJson_.at(BASE_ABILITY_INFO).at(abilityName);
            infoJson[iter.key()] = valueIter.value();
        }
    }
}

/**
 * @tc.number: ApplicationInfoJsonSerializer_0100
 * @tc.name: save bundle installation information to persist storage
 * @tc.desc: 1.system running normally
 *           2.successfully serialize and deserialize all right props in ApplicationInfo
 */
HWTEST_F(BmsBundleDataStorageTest, ApplicationInfoJsonSerializer_0100, Function | SmallTest | Level1)
{
    nlohmann::json sourceInfoJson = innerBundleInfoJson_.at(BASE_APPLICATION_INFO);
    // deserialize ApplicationInfo from json
    ApplicationInfo fromJsonInfo = sourceInfoJson;
    // serialize fromJsonInfo to json
    nlohmann::json toJsonObject = fromJsonInfo;

    ASSERT_TRUE(toJsonObject.dump() == sourceInfoJson.dump());
}

/**
 * @tc.number: ApplicationInfoJsonSerializer_0200
 * @tc.name: save bundle installation information to persist storage
 * @tc.desc: 1.system running normally
 *           2.test can catch deserialize error for type error for name prop in ApplicationInfo
 */
HWTEST_F(BmsBundleDataStorageTest, ApplicationInfoJsonSerializer_0200, Function | SmallTest | Level1)
{
    nlohmann::json typeErrorProps;
    typeErrorProps["name"] = NOT_STRING_TYPE;
    typeErrorProps["bundleName"] = NOT_STRING_TYPE;
    typeErrorProps["sandboxId"] = NOT_NUMBER_TYPE;
    typeErrorProps["signatureKey"] = NOT_STRING_TYPE;

    for (nlohmann::json::iterator iter = typeErrorProps.begin(); iter != typeErrorProps.end(); iter++) {
        for (auto valueIter = iter.value().begin(); valueIter != iter.value().end(); valueIter++) {
            APP_LOGD("deserialize check prop key = %{public}s, type = %{public}s",
                iter.key().c_str(),
                valueIter.key().c_str());
            nlohmann::json infoJson = innerBundleInfoJson_.at(BASE_APPLICATION_INFO);
            infoJson[iter.key()] = valueIter.value();
        }
    }
}

/**
 * @tc.number: ModuleInfoJsonSerializer_0100
 * @tc.name: save bundle installation information to persist storage
 * @tc.desc: 1.system running normally
 *           2.successfully serialize and deserialize all right props in ModuleInfo
 */
HWTEST_F(BmsBundleDataStorageTest, ModuleInfoJsonSerializer_0100, Function | SmallTest | Level1)
{
    nlohmann::json sourceInfoJson = moduleInfoJson_;
    // deserialize ModuleInfo from json
    ModuleInfo fromJsonInfo = sourceInfoJson;
    // serialize fromJsonInfo to json
    nlohmann::json toJsonObject = fromJsonInfo;

    ASSERT_TRUE(toJsonObject.dump() == sourceInfoJson.dump());
}

/**
 * @tc.number: SaveData_0100
 * @tc.name: save bundle installation information to persist storage
 * @tc.desc: 1.system running normally and no saved any bundle data
 *           2.successfully save a new bundle installation information for the first time
 */
HWTEST_F(BmsBundleDataStorageTest, SaveData_0100, Function | SmallTest | Level0)
{
    InnerBundleInfo innerBundleInfo;
    innerBundleInfo.FromJson(innerBundleInfoJson_);
    CheckBundleSaved(innerBundleInfo);
}

/**
 * @tc.number: SaveData_0200
 * @tc.name: save bundle installation information to persist storage
 * @tc.desc: 1.system running normally
 *           2.successfully save a new bundle installation information for not the first time
 */
HWTEST_F(BmsBundleDataStorageTest, SaveData_0200, Function | SmallTest | Level0)
{
    InnerBundleInfo innerBundleInfo;
    innerBundleInfo.FromJson(innerBundleInfoJson_);

    ApplicationInfo baseApp = innerBundleInfo.GetBaseApplicationInfo();
    baseApp.name = "com.example.other";
    baseApp.bundleName = baseApp.name;
    innerBundleInfo.SetBaseApplicationInfo(baseApp);

    InnerBundleInfo otherInnerBundleInfo = innerBundleInfo;
    CheckBundleSaved(otherInnerBundleInfo);
}

/**
 * @tc.number: SaveData_0300
 * @tc.name: save bundle installation information to persist storage
 * @tc.desc: 1.system running normally
 *           2.successfully update an already exist bundle installation information
 */
HWTEST_F(BmsBundleDataStorageTest, SaveData_0300, Function | SmallTest | Level0)
{
    InnerBundleInfo innerBundleInfo;
    innerBundleInfo.FromJson(innerBundleInfoJson_);
    CheckBundleSaved(innerBundleInfo);
    CheckBundleDeleted(innerBundleInfo);

    BundleInfo bundleInfo = innerBundleInfoJson_.at(BASE_BUNDLE_INFO);
    bundleInfo.description = "update test application";
    InnerBundleInfo otherInnerBundleInfo = innerBundleInfo;
    otherInnerBundleInfo.SetBaseBundleInfo(bundleInfo);

    CheckBundleSaved(otherInnerBundleInfo);
}

/**
 * @tc.number: SaveData_0400
 * @tc.name: save bundle installation information to persist storage
 * @tc.desc: 1.system running normally
 *           2.check all props can be serialize and deserialize
 */
HWTEST_F(BmsBundleDataStorageTest, SaveData_0400, Function | SmallTest | Level1)
{
    nlohmann::json sourceInfoJson = innerBundleInfoJson_;
    InnerBundleInfo fromJsonInfo;
    ASSERT_TRUE(fromJsonInfo.FromJson(innerBundleInfoJson_));
    ASSERT_TRUE(fromJsonInfo.ToString() == sourceInfoJson.dump());
}

/**
 * @tc.number: LoadAllData_0100
 * @tc.name: load all installed bundle information from persist storage
 * @tc.desc: 1.system running normally
 *           2.test can successfully load all installed bundle information from persist storage
 */
HWTEST_F(BmsBundleDataStorageTest, LoadAllData_0100, Function | SmallTest | Level0)
{
    BundleDataStorage bundleDataStorage;
    int count = 10;
    InnerBundleInfo innerBundleInfo;
    innerBundleInfo.FromJson(innerBundleInfoJson_);

    ApplicationInfo baseApp = innerBundleInfo.GetBaseApplicationInfo();

    for (int i = 0; i < count; i++) {
        baseApp.name = NORMAL_BUNDLE_NAME + std::to_string(i);
        baseApp.bundleName = baseApp.name;
        innerBundleInfo.SetBaseApplicationInfo(baseApp);
        ASSERT_TRUE(bundleDataStorage.SaveStorageBundleInfo(Constants::CURRENT_DEVICE_ID, innerBundleInfo));
    }

    std::map<std::string, std::map<std::string, InnerBundleInfo>> bundleDates;
    ASSERT_TRUE(bundleDataStorage.LoadAllData(bundleDates));

    for (int i = 0; i < count; i++) {
        std::string bundleName = NORMAL_BUNDLE_NAME + std::to_string(i);
        baseApp.name = NORMAL_BUNDLE_NAME + std::to_string(i);
        baseApp.bundleName = baseApp.name;
        innerBundleInfo.SetBaseApplicationInfo(baseApp);

        // search allDeviceInfos by bundle name
        auto bundleDatesIter = bundleDates.find(bundleName);
        ASSERT_TRUE(bundleDatesIter != bundleDates.end());
        // search InnerBundleInfo by device id
        auto allDeviceInfos = bundleDatesIter->second;
        auto devicesInfosIter = allDeviceInfos.find(deviceId_);
        ASSERT_TRUE(devicesInfosIter != allDeviceInfos.end());

        InnerBundleInfo afterLoadInfo = devicesInfosIter->second;
        ASSERT_TRUE(innerBundleInfo.ToString() == afterLoadInfo.ToString());
    }
}

/**
 * @tc.number: DeleteBundleData_0100
 * @tc.name: delete bundle installation information from persist storage
 * @tc.desc: 1.system running normally
 *           2.successfully delete a saved bundle installation information
 */
HWTEST_F(BmsBundleDataStorageTest, DeleteBundleData_0100, Function | SmallTest | Level0)
{
    InnerBundleInfo innerBundleInfo;
    innerBundleInfo.FromJson(innerBundleInfoJson_);

    BundleDataStorage bundleDataStorage;
    ASSERT_TRUE(bundleDataStorage.SaveStorageBundleInfo(Constants::CURRENT_DEVICE_ID, innerBundleInfo));

    CheckBundleDeleted(innerBundleInfo);
}

/**
 * @tc.number: DeleteBundleData_0200
 * @tc.name: delete bundle installation information from persist storage
 * @tc.desc: 1.system running normally and no saved the bundle to be deleted
 *           2.successfully delete an unsaved bundle installation information
 */
HWTEST_F(BmsBundleDataStorageTest, DeleteBundleData_0200, Function | SmallTest | Level1)
{
    InnerBundleInfo innerBundleInfo;
    innerBundleInfo.FromJson(innerBundleInfoJson_);
    BundleDataStorage bundleDataStorage;
    ASSERT_FALSE(bundleDataStorage.DeleteStorageBundleInfo(Constants::CURRENT_DEVICE_ID, innerBundleInfo));
}

/**
 * @tc.number: DeleteBundleData_0300
 * @tc.name: delete bundle installation information from persist storage
 * @tc.desc: 1.system running normally and no saved the bundle to be deleted
 *           2.Unsaved bundle installation information was not removed successfully
 */
HWTEST_F(BmsBundleDataStorageTest, DeleteBundleData_0300, Function | SmallTest | Level1)
{
    ASSERT_EQ(remove(Constants::BUNDLE_DATA_BASE_FILE.c_str()), 0);

    InnerBundleInfo innerBundleInfo;
    innerBundleInfo.FromJson(innerBundleInfoJson_);
    BundleDataStorage bundleDataStorage;
    ASSERT_FALSE(bundleDataStorage.DeleteStorageBundleInfo(Constants::CURRENT_DEVICE_ID, innerBundleInfo));

    std::map<std::string, std::map<std::string, InnerBundleInfo>> bundleDates;
    ASSERT_FALSE(bundleDataStorage.SaveStorageBundleInfo(Constants::CURRENT_DEVICE_ID, innerBundleInfo));
    ASSERT_FALSE(bundleDataStorage.LoadAllData(bundleDates));
}