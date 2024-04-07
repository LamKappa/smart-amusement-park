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

#include <fstream>
#include <gtest/gtest.h>

#include "app_log_wrapper.h"
#include "appexecfwk_errors.h"
#include "bundle_constants.h"
#include "bundle_data_mgr.h"
#include "bundle_data_storage.h"
#include "bundle_mgr_service.h"
#include "bundle_profile.h"
#include "common_tool.h"
#include "directory_ex.h"
#include "inner_bundle_info.h"
#include "installd/installd_service.h"
#include "installd_client.h"
#include "mock_status_receiver.h"

using namespace testing::ext;
using namespace OHOS::AppExecFwk;
using namespace std::chrono_literals;
using namespace OHOS;
using OHOS::DelayedSingleton;
using OHOS::AAFwk::Want;

namespace {
const std::string BUNDLE_TMPPATH = "/data/test/bms_bundle/";
const std::string THIRD_BUNDLE_NAME = "com.third.hiworld.example";
const std::string SYSTEM_BUNDLE_NAME = "com.system.hiworld.example";
const std::string BUNDLE_CODE_PATH = "/data/accounts/account_0/applications/";
const std::string BUNDLE_DATA_PATH = "/data/accounts/account_0/appdata/";
const std::string ROOT_DIR = "/data/accounts";
const std::string ERROR_SUFFIX = ".rpk";
const int32_t ROOT_UID = 0;
}  // namespace

class BmsBundleInstallerModuleTest : public testing::Test {
public:
    BmsBundleInstallerModuleTest();
    ~BmsBundleInstallerModuleTest();
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    void StartBundleMgrService()
    {
        if (!bms_) {
            bms_ = DelayedSingleton<BundleMgrService>::GetInstance();
        }
        if (bms_) {
            bms_->OnStart();
        }
    }

    void StopBundleMgrService()
    {
        if (bms_) {
            bms_->OnStop();
            bms_ = nullptr;
        }
    }

    void StartInstalld()
    {
        installdService_->Start();
    }

    void StopInstalld()
    {
        installdService_->Stop();
        InstalldClient::GetInstance()->ResetInstalldProxy();
    }

    std::shared_ptr<BundleDataMgr> GetBundleDataMgr()
    {
        return bms_->GetDataMgr();
    }

    bool GetServiceStatus()
    {
        return bms_->IsServiceReady();
    }

    void CheckBundleSaved(const InnerBundleInfo &innerBundleInfo) const;
    void CheckBundleDeleted(const InnerBundleInfo &innerBundleInfo) const;
    static void ClearJsonFile();
    void CheckFileExist(const std::string &bundleName) const;
    void CheckFileExist(const std::string &bundleName, const std::string &modulePackage) const;
    void CheckFileExist(const std::string &bundleName, const std::string &modulePackage,
        const std::vector<std::string> &abilityNames) const;
    void CheckFileNonExist(const std::string &bundleName) const;

protected:
    nlohmann::json innerBundleInfoJson_ = R"(
        {
            "appFeature": "ohos_system_app",
            "appType": 0,
            "baseAbilityInfos": {
                "com.system.hiworld.examples1com.system.hiworld.example.h2bmsSystemBundle_A1": {
                    "applicationName": "com.system.hiworld.examples1",
                    "bundleName": "com.system.hiworld.examples1",
                    "codePath": "",
                    "description": "",
                    "deviceCapabilities": [],
                    "deviceId": "",
                    "deviceTypes": [
                        "tv",
                        "car"
                    ],
                    "iconPath": "$media:snowball",
                    "isLauncherAbility": false,
                    "isNativeAbility": true,
                    "kind": "page",
                    "label": "bmsSystemBundle_A1 Ability",
                    "launchMode": 0,
                    "libPath": "",
                    "moduleName": "bmsSystemBundle1",
                    "name": "bmsSystemBundle_A1",
                    "orientation": 0,
                    "package": "com.system.hiworld.example.h2",
                    "permissions": [],
                    "process": "",
                    "resourcePath": "",
                    "type": 1,
                    "uri": "",
                    "visible": true
                }
            },
            "baseApplicationInfo": {
                "bundleName": "com.system.hiworld.examples1",
                "cacheDir": "/data/accounts/account_0/appdata/com.system.hiworld.examples1/cache",
                "codePath": "/data/accounts/account_0/applications/com.system.hiworld.examples1",
                "dataBaseDir": "/data/accounts/account_0/appdata/com.system.hiworld.examples1/database",
                "dataDir": "/data/accounts/account_0/appdata/com.system.hiworld.examples1/files",
                "description": "",
                "descriptionId": 0,
                "deviceId": "PHONE-001",
                "entryDir": "",
                "iconId": 0,
                "iconPath": "",
                "isLauncherApp": false,
                "isSystemApp": true,
                "label": "",
                "labelId": 0,
                "moduleInfos": [],
                "moduleSourceDirs": [],
                "name": "com.system.hiworld.examples1",
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
                "gid": 2101,
                "hapModuleNames": [],
                "installTime": 105590,
                "isKeepAlive": false,
                "isNativeApp": true,
                "jointUserId": "",
                "label": "bmsSystemBundle_A1 Ability",
                "mainEntry": "",
                "maxSdkVersion": 0,
                "minSdkVersion": 0,
                "moduleDirs": [],
                "moduleNames": [],
                "modulePublicDirs": [],
                "moduleResPaths": [],
                "name": "com.system.hiworld.examples1",
                "releaseType": "Release",
                "reqPermissions": [],
                "seInfo": "",
                "targetVersion": 3,
                "uid": 2101,
                "updateTime": 105590,
                "vendor": "example",
                "versionCode": 1,
                "versionName": "1.0"
            },
            "baseDataDir": "/data/accounts/account_0/appdata/com.system.hiworld.examples1",
            "bundleStatus": 1,
            "gid": 2101,
            "hasEntry": true,
            "innerModuleInfos": {
                "com.system.hiworld.example.h2": {
                    "abilityKeys": [
                        "com.system.hiworld.examples1com.system.hiworld.example.h2bmsSystemBundle_A1"
                    ],
                    "defPermissions": [],
                    "description": "",
                    "distro": {
                        "deliveryWithInstall": true,
                        "moduleName": "testability",
                        "moduleType": "entry"
                    },
                    "isEntry": true,
                    "metaData": {
                        "customizeData": [],
                        "parameters": [],
                        "results": []
                    },
                    "moduleDataDir": "/data/accounts/account_0/appdata/com.system.hiworld.examples1/com.system.hiworld.example.h2",
                    "moduleName": "bmsSystemBundle1",
                    "modulePackage": "com.system.hiworld.example.h2",
                    "modulePath": "/data/accounts/account_0/applications/com.system.hiworld.examples1/com.system.hiworld.example.h2",
                    "moduleResPath": "",
                    "reqCapabilities": [],
                    "reqPermissions": [],
                    "skillKeys": [
                        "com.system.hiworld.examples1com.system.hiworld.example.h2bmsSystemBundle_A1"
                    ]
                }
            },
            "isKeepData": false,
            "isSupportBackup": false,
            "mainAbility": "",
            "provisionId": "BNtg4JBClbl92Rgc3jm/RfcAdrHXaM8F0QOiwVEhnV5ebE5jNIYnAx+weFRT3QTyUjRNdhmc2aAzWyi+5t5CoBM=",
            "skillInfos": {
                "com.system.hiworld.examples1com.system.hiworld.example.h2bmsSystemBundle_A1": []
            },
            "uid": 2101,
            "userId_": 0
        }
        )"_json;
    nlohmann::json bundleInfoJson_ = R"(
            {
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
                "gid": 2101,
                "hapModuleNames": [],
                "installTime": 105590,
                "isKeepAlive": false,
                "isNativeApp": true,
                "jointUserId": "",
                "label": "bmsSystemBundle_A1 Ability",
                "mainEntry": "",
                "maxSdkVersion": 0,
                "minSdkVersion": 0,
                "moduleDirs": [],
                "moduleNames": [],
                "modulePublicDirs": [],
                "moduleResPaths": [],
                "name": "com.system.hiworld.examples1",
                "releaseType": "Release",
                "reqPermissions": [],
                "seInfo": "",
                "targetVersion": 3,
                "uid": 2101,
                "updateTime": 105590,
                "vendor": "example",
                "versionCode": 1,
                "versionName": "1.0"
            }
        )"_json;
    std::string deviceId_{};

private:
    std::shared_ptr<InstalldService> installdService_ = std::make_unique<InstalldService>();
    std::shared_ptr<BundleMgrService> bms_ = DelayedSingleton<BundleMgrService>::GetInstance();
};

BmsBundleInstallerModuleTest::BmsBundleInstallerModuleTest()
{
    deviceId_ = Constants::CURRENT_DEVICE_ID;
    innerBundleInfoJson_["baseBundleInfo"] = bundleInfoJson_;
}

BmsBundleInstallerModuleTest::~BmsBundleInstallerModuleTest()
{
    bms_.reset();
}

void BmsBundleInstallerModuleTest::CheckBundleSaved(const InnerBundleInfo &innerBundleInfo) const
{
    BundleDataStorage bundleDataStorage;
    EXPECT_TRUE(bundleDataStorage.SaveStorageBundleInfo(Constants::CURRENT_DEVICE_ID, innerBundleInfo));
    std::map<std::string, std::map<std::string, InnerBundleInfo>> bundleData;
    EXPECT_TRUE(bundleDataStorage.LoadAllData(bundleData));
    std::string bundleName = innerBundleInfo.GetBundleName();
    auto bundleDataIter = bundleData.find(bundleName);
    EXPECT_TRUE(bundleDataIter != bundleData.end());
    auto allDeviceInfos = bundleDataIter->second;
    auto devicesInfosIter = allDeviceInfos.find(deviceId_);
    EXPECT_TRUE(devicesInfosIter != allDeviceInfos.end());
    InnerBundleInfo afterLoadInfo = devicesInfosIter->second;
    EXPECT_TRUE(innerBundleInfo.ToString() == afterLoadInfo.ToString());
}

void BmsBundleInstallerModuleTest::CheckBundleDeleted(const InnerBundleInfo &innerBundleInfo) const
{
    BundleDataStorage bundleDataStorage;
    EXPECT_TRUE(bundleDataStorage.DeleteStorageBundleInfo(Constants::CURRENT_DEVICE_ID, innerBundleInfo));
    std::map<std::string, std::map<std::string, InnerBundleInfo>> bundleData;
    EXPECT_FALSE(bundleDataStorage.LoadAllData(bundleData));
}

void BmsBundleInstallerModuleTest::ClearJsonFile()
{
    std::string fileName = Constants::BUNDLE_DATA_BASE_FILE;
    std::ofstream o(fileName);
    if (!o.is_open()) {
        std::cout << "failed to open as out" << fileName << std::endl;
    } else {
        std::cout << "clear" << fileName << std::endl;
    }
    o.close();
}

void BmsBundleInstallerModuleTest::CheckFileExist(const std::string &bundleName) const
{
    int bundleDataExist = access((BUNDLE_DATA_PATH + bundleName).c_str(), F_OK);
    EXPECT_EQ(bundleDataExist, 0) << "the bundle data dir does not exists: " << bundleName;
    int codeExist = access((BUNDLE_CODE_PATH + bundleName).c_str(), F_OK);
    EXPECT_EQ(codeExist, 0) << "the ability code file does not exist: " << bundleName;
}

void BmsBundleInstallerModuleTest::CheckFileExist(const std::string &bundleName, const std::string &modulePackage) const
{
    int bundleDataExist = access((BUNDLE_DATA_PATH + bundleName + "/" + modulePackage).c_str(), F_OK);
    EXPECT_EQ(bundleDataExist, 0) << "the bundle data dir does not exists: " << modulePackage;
    int codeExist = access((BUNDLE_DATA_PATH + bundleName + "/" + modulePackage).c_str(), F_OK);
    EXPECT_EQ(codeExist, 0) << "the ability code file does not exist: " << modulePackage;
}

void BmsBundleInstallerModuleTest::CheckFileNonExist(const std::string &bundleName) const
{
    int bundleDataExist = access((BUNDLE_DATA_PATH + bundleName).c_str(), F_OK);
    EXPECT_NE(bundleDataExist, 0) << "the bundle data dir exists: " << bundleName;
    int codeExist = access((BUNDLE_CODE_PATH + bundleName).c_str(), F_OK);
    EXPECT_NE(codeExist, 0) << "the ability code exists: " << bundleName;
}

void BmsBundleInstallerModuleTest::CheckFileExist(
    const std::string &bundleName, const std::string &modulePackage, const std::vector<std::string> &abilityNames) const
{
    int bundleDataExist = 1;
    int codeExist = 1;
    if (abilityNames.size() == 0) {
        CheckFileExist(bundleName, modulePackage);
    }
    for (auto iter = abilityNames.begin(); iter != abilityNames.end(); iter++) {
        bundleDataExist = access((BUNDLE_DATA_PATH + bundleName + "/" + modulePackage + "/" + *iter).c_str(), F_OK);
        EXPECT_EQ(bundleDataExist, 0) << "the bundle data dir does not exists: " << *iter;
        codeExist = access((BUNDLE_DATA_PATH + bundleName + "/" + modulePackage + "/" + *iter).c_str(), F_OK);
        EXPECT_EQ(codeExist, 0) << "the ability code file does not exist: " << *iter;
    }
}

void BmsBundleInstallerModuleTest::SetUpTestCase()
{
    if (access(ROOT_DIR.c_str(), F_OK) != 0) {
        bool result = OHOS::ForceCreateDirectory(ROOT_DIR);
        ASSERT_TRUE(result);
    }
    if (chown(ROOT_DIR.c_str(), ROOT_UID, ROOT_UID) != 0) {
        ASSERT_TRUE(false);
    }
    if (chmod(ROOT_DIR.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) != 0) {
        ASSERT_TRUE(false);
    }
}

void BmsBundleInstallerModuleTest::TearDownTestCase()
{}

void BmsBundleInstallerModuleTest::SetUp()
{
    std::cout << "BmsBundleInstallerModuleTest SetUp Begin" << std::endl;
    StartInstalld();
    StartBundleMgrService();
    std::cout << "BmsBundleInstallerModuleTest SetUp End" << std::endl;
}

void BmsBundleInstallerModuleTest::TearDown()
{
    std::cout << "BmsBundleInstallerModuleTest TearDown Begin" << std::endl;
    StopInstalld();
    StopBundleMgrService();
    std::cout << "BmsBundleInstallerModuleTest TearDown End" << std::endl;
}

/**
 * @tc.number: SystemAppInstall_0100
 * @tc.name: test a system bundle can be installed and bundle files exist
 * @tc.desc: 1.under '/system/app/',there is a hap
 *           2.TriggerScan and check install results
 */
HWTEST_F(BmsBundleInstallerModuleTest, SystemAppInstall_0100, Function | MediumTest | Level1)
{
    std::string bundleName = SYSTEM_BUNDLE_NAME + "s1";
    std::shared_ptr<BundleDataMgr> dataMgr = GetBundleDataMgr();
    ASSERT_NE(dataMgr, nullptr);
    BundleInfo bundleInfo;
    bool ret = false;
    int checkCount = 0;
    do {
        std::this_thread::sleep_for(50ms);
        ret = dataMgr->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
        checkCount++;
    } while (!ret && (checkCount < 100));

    CheckFileExist(bundleName);
    EXPECT_FALSE((bundleInfo.name).empty());
}

/**
 * @tc.number: SystemAppInstall_0200
 * @tc.name: test install ten system bundles when the system starts
 * @tc.desc: 1.under '/system/app/',there are two haps
 *           2.TriggerScan and check install results
 */
HWTEST_F(BmsBundleInstallerModuleTest, SystemAppInstall_0200, Function | MediumTest | Level1)
{
    int bundleNum = 2;
    std::shared_ptr<BundleDataMgr> dataMgr = GetBundleDataMgr();
    ASSERT_NE(dataMgr, nullptr);
    BundleInfo bundleInfo;

    for (int i = 1; i <= bundleNum; i++) {
        std::string bundleName = SYSTEM_BUNDLE_NAME + "s" + std::to_string(i);

        bool ret = false;
        int checkCount = 0;
        do {
            std::this_thread::sleep_for(50ms);
            ret = dataMgr->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
            checkCount++;
        } while (!ret && (checkCount < 100));
        CheckFileExist(bundleName);
        EXPECT_FALSE((bundleInfo.name).empty());
    }
}

/**
 * @tc.number: SystemAppInstall_0300
 * @tc.name: test install abnormal system bundles, and check bundle's informations
 * @tc.desc: 1.under '/system/app/',there are three hap bundles, one of them is normal,
 *             others are abnormal
 *           2.TriggerScan and check install results
 */
HWTEST_F(BmsBundleInstallerModuleTest, SystemAppInstall_0300, Function | MediumTest | Level2)
{
    std::string norBundleName = SYSTEM_BUNDLE_NAME + "s2";
    std::shared_ptr<BundleDataMgr> dataMgr = GetBundleDataMgr();
    ASSERT_NE(dataMgr, nullptr);
    BundleInfo bundleInfo;
    bool ret = false;
    int checkCount = 0;
    do {
        std::this_thread::sleep_for(50ms);
        ret = dataMgr->GetBundleInfo(norBundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
        checkCount++;
    } while (!ret && (checkCount < 100));
    CheckFileExist(norBundleName);
    EXPECT_FALSE((bundleInfo.name).empty());

    int bundleNum = 5;
    for (int i = 3; i < bundleNum; i++) {
        std::string invalidBundleName = SYSTEM_BUNDLE_NAME + "s" + std::to_string(i);
        CheckFileNonExist(invalidBundleName);
        BundleInfo invalidBundleInfo;
        dataMgr->GetBundleInfo(invalidBundleName, BundleFlag::GET_BUNDLE_DEFAULT, invalidBundleInfo);
        EXPECT_TRUE((invalidBundleInfo.name).empty());
    }
}

/**
 * @tc.number: SystemAppInstall_0400
 * @tc.name: test install an abnormal system bundle, and check bundle's informations
 * @tc.desc: 1.under '/system/app/',there is a hap, the config.json file of which
 *             does not contain bundle name
 *           2.TriggerScan and check install result
 */
HWTEST_F(BmsBundleInstallerModuleTest, SystemAppInstall_0400, Function | MediumTest | Level2)
{
    std::string bundleName = SYSTEM_BUNDLE_NAME + "s3";
    CheckFileNonExist(bundleName);
    std::shared_ptr<BundleDataMgr> dataMgr = GetBundleDataMgr();
    ASSERT_NE(dataMgr, nullptr);
    BundleInfo bundleInfo;
    bool ret = false;
    int checkCount = 0;
    do {
        std::this_thread::sleep_for(1ms);
        ret = dataMgr->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
        checkCount++;
    } while (!ret && (checkCount < 100));

    EXPECT_TRUE((bundleInfo.name).empty());
}

/**
 * @tc.number: SystemAppInstall_0500
 * @tc.name: test install an abnormal system bundle, and check bundle's informations
 * @tc.desc: 1.under '/system/app/',there is a hap, which doesn't have the config.json
 *           2.TriggerScan and check install result
 */
HWTEST_F(BmsBundleInstallerModuleTest, SystemAppInstall_0500, Function | MediumTest | Level2)
{
    std::string bundleName = SYSTEM_BUNDLE_NAME + "s4";
    CheckFileNonExist(bundleName);
    std::shared_ptr<BundleDataMgr> dataMgr = GetBundleDataMgr();
    ASSERT_NE(dataMgr, nullptr);
    BundleInfo bundleInfo;
    bool ret = false;
    int checkCount = 0;
    do {
        std::this_thread::sleep_for(1ms);
        ret = dataMgr->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
        checkCount++;
    } while (!ret && (checkCount < 100));

    EXPECT_TRUE((bundleInfo.name).empty());
}

/**
 * @tc.number: ThirdAppInstall_0100
 * @tc.name: test third-party bundle install
 * @tc.desc: 1.under '/data/test/bms_bundle/',there is a hap
 *           2.install the bundle and check results
 */
HWTEST_F(BmsBundleInstallerModuleTest, ThirdAppInstall_0100, Function | MediumTest | Level1)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    ASSERT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(receiver, nullptr);
    InstallParam installParam;

    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle1" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver);
    ASSERT_EQ(receiver->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string bundleName = THIRD_BUNDLE_NAME + "1";
    CheckFileExist(bundleName);
    OHOS::sptr<MockStatusReceiver> recUninstall = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(recUninstall, nullptr);
    installParam.userId = Constants::DEFAULT_USERID;
    installer->Uninstall(bundleName, installParam, recUninstall);
    EXPECT_EQ(recUninstall->GetResultCode(), ERR_OK) << "uninstall fail!" << bundleName;
}

/**
 * @tc.number: ThirdAppInstall_0200
 * @tc.name: test third-party bundle install and bundle info store
 * @tc.desc: 1.under '/data/test/bms_bundle/',there is a hap
 *           2.check install results
 *           3.restart bundmgrservice
 */
HWTEST_F(BmsBundleInstallerModuleTest, ThirdAppInstall_0200, Function | MediumTest | Level1)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    ASSERT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(receiver, nullptr);
    InstallParam installParam;
    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle1" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver);
    ASSERT_EQ(receiver->GetResultCode(), ERR_OK);
    std::string bundleName = THIRD_BUNDLE_NAME + "1";
    CheckFileExist(bundleName);

    StopBundleMgrService();
    StartBundleMgrService();
    CheckFileExist(bundleName);
    std::shared_ptr<BundleDataMgr> dataMgr = GetBundleDataMgr();
    ASSERT_NE(dataMgr, nullptr);
    BundleInfo bundleInfo;
    bool ret = false;
    ret = dataMgr->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    EXPECT_TRUE(ret);
    EXPECT_EQ(bundleInfo.name, bundleName);

    installParam.userId = Constants::DEFAULT_USERID;
    OHOS::sptr<MockStatusReceiver> recUninstall = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(recUninstall, nullptr);
    installer->Uninstall(bundleName, installParam, recUninstall);
    EXPECT_EQ(recUninstall->GetResultCode(), ERR_OK);
}

/**
 * @tc.number: ThirdAppInstall_0300
 * @tc.name: test third-party bundle install and bundle info store
 * @tc.desc: 1.under '/data/test/bms_bundle/',there a hap,the suffix
 *             of which is wrong
 *           2.check install results
 */
HWTEST_F(BmsBundleInstallerModuleTest, ThirdAppInstall_0300, Function | MediumTest | Level2)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    ASSERT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(receiver, nullptr);
    InstallParam installParam;
    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle12" + ERROR_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver);
    ASSERT_EQ(receiver->GetResultCode(), ERR_APPEXECFWK_INSTALL_INVALID_HAP_NAME);
    std::string bundleName = THIRD_BUNDLE_NAME + "12";
    CheckFileNonExist(bundleName);

    BundleInfo info;
    std::shared_ptr<BundleMgrService> bms = DelayedSingleton<BundleMgrService>::GetInstance();
    auto dataMgr = bms->GetDataMgr();
    ASSERT_NE(dataMgr, nullptr);
    bool isBundleExist = dataMgr->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, info);
    EXPECT_FALSE(isBundleExist);
}

/**
 * @tc.number: ThirdAppInstall_0400
 * @tc.name: test third bundle install and bundle info store
 * @tc.desc: 1.under '/data/test/bms_bundle/',there is a big hap
 *           2.install and check install results
 */
HWTEST_F(BmsBundleInstallerModuleTest, ThirdAppInstall_0400, Function | MediumTest | Level1)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    ASSERT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(receiver, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver2 = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(receiver2, nullptr);
    InstallParam installParam;
    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleName = THIRD_BUNDLE_NAME + "5";
    std::string bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle13" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver);
    StopInstalld();
    std::this_thread::sleep_for(1ms);

    EXPECT_NE(receiver->GetResultCode(), ERR_OK);

    std::this_thread::sleep_for(5ms);
    CheckFileNonExist(bundleName);

    StartInstalld();
    std::this_thread::sleep_for(1ms);
    installer->Install(bundleFilePath, installParam, receiver2);
    ASSERT_EQ(receiver2->GetResultCode(), ERR_OK);
    CheckFileExist(bundleName);
    OHOS::sptr<MockStatusReceiver> recUninstall = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(recUninstall, nullptr);
    installer->Uninstall(bundleName, installParam, recUninstall);
    EXPECT_EQ(recUninstall->GetResultCode(), ERR_OK);
}

/**
 * @tc.number: ThirdAppInstall_0500
 * @tc.name: test third-party bundle upgrade
 * @tc.desc: 1.one hap has been installed in system
 *           2.under '/data/test/bms_bundle',there a hap,the version of which is lower than the installed one
 *           3.check install results
 */
HWTEST_F(BmsBundleInstallerModuleTest, ThirdAppInstall_0500, Function | MediumTest | Level1)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    ASSERT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> normalInstall = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(normalInstall, nullptr);

    InstallParam installParam;
    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle9" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, normalInstall);
    ASSERT_EQ(normalInstall->GetResultCode(), ERR_OK);
    std::string bundleName = THIRD_BUNDLE_NAME + "2";
    CheckFileExist(bundleName);
    std::string upgradeFilePath = BUNDLE_TMPPATH + "bmsThirdBundle7" + Constants::INSTALL_FILE_SUFFIX;
    OHOS::sptr<MockStatusReceiver> replaceInstall = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(replaceInstall, nullptr);
    installParam.installFlag = InstallFlag::REPLACE_EXISTING;
    installer->Install(upgradeFilePath, installParam, replaceInstall);
    ASSERT_EQ(replaceInstall->GetResultCode(), ERR_APPEXECFWK_INSTALL_VERSION_DOWNGRADE);
    CheckFileExist(bundleName);

    OHOS::sptr<MockStatusReceiver> uninstall = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(uninstall, nullptr);
    installParam.userId = Constants::DEFAULT_USERID;
    installer->Uninstall(bundleName, installParam, uninstall);
    EXPECT_EQ(uninstall->GetResultCode(), ERR_OK) << "uninstall fail!" << bundleName;
}

/**
 * @tc.number: ThirdAppInstall_0600
 * @tc.name: test third-party bundle upgrade
 * @tc.desc: 1.one hap has been installed in system
 *           2.under '/data/test/bms_bundle',there a hap,the version of which is higher than the installed one
 *           3.check install results
 */
HWTEST_F(BmsBundleInstallerModuleTest, ThirdAppInstall_0600, Function | MediumTest | Level1)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    ASSERT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> normalInstall = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(normalInstall, nullptr);

    InstallParam installParam;
    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle7" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, normalInstall);
    ASSERT_EQ(normalInstall->GetResultCode(), ERR_OK);
    std::string bundleName = THIRD_BUNDLE_NAME + "2";
    CheckFileExist(bundleName);
    std::string upgradeFilePath = BUNDLE_TMPPATH + "bmsThirdBundle9" + Constants::INSTALL_FILE_SUFFIX;
    OHOS::sptr<MockStatusReceiver> replaceInstall = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(replaceInstall, nullptr);
    installParam.installFlag = InstallFlag::REPLACE_EXISTING;
    installer->Install(upgradeFilePath, installParam, replaceInstall);
    ASSERT_EQ(replaceInstall->GetResultCode(), ERR_OK);
    CheckFileExist(bundleName);

    StopBundleMgrService();
    StartBundleMgrService();
    CheckFileExist(bundleName);

    BundleInfo info;
    std::shared_ptr<BundleMgrService> bms = DelayedSingleton<BundleMgrService>::GetInstance();
    auto dataMgr = bms->GetDataMgr();
    ASSERT_NE(dataMgr, nullptr);
    bool isBundleExist = dataMgr->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, info);
    EXPECT_TRUE(isBundleExist);

    std::cout << "info-name:" << info.name << std::endl;

    OHOS::sptr<MockStatusReceiver> uninstall = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(uninstall, nullptr);
    installParam.userId = Constants::DEFAULT_USERID;
    installer->Uninstall(bundleName, installParam, uninstall);
    EXPECT_EQ(uninstall->GetResultCode(), ERR_OK) << "uninstall fail!" << bundleName;
}

/**
 * @tc.number: ThirdAppInstall_0700
 * @tc.name: test third-party bundle upgrade
 * @tc.desc: 1.one hap has been installed in system
 *           2.under '/data/test/bms_bundle',there a hap,the version of which is equal to the installed one
 *           3.check install results
 */
HWTEST_F(BmsBundleInstallerModuleTest, ThirdAppInstall_0700, Function | MediumTest | Level1)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    ASSERT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> normalInstall = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(normalInstall, nullptr);

    InstallParam installParam;
    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle7" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, normalInstall);
    ASSERT_EQ(normalInstall->GetResultCode(), ERR_OK);
    std::string bundleName = THIRD_BUNDLE_NAME + "2";
    CheckFileExist(bundleName);
    std::string upgradeFilePath = BUNDLE_TMPPATH + "bmsThirdBundle10" + Constants::INSTALL_FILE_SUFFIX;
    OHOS::sptr<MockStatusReceiver> replaceInstall = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(replaceInstall, nullptr);
    installParam.installFlag = InstallFlag::REPLACE_EXISTING;
    installer->Install(upgradeFilePath, installParam, replaceInstall);
    ASSERT_EQ(replaceInstall->GetResultCode(), ERR_OK);
    CheckFileExist(bundleName);

    OHOS::sptr<MockStatusReceiver> uninstall = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(uninstall, nullptr);
    installParam.userId = Constants::DEFAULT_USERID;
    installer->Uninstall(bundleName, installParam, uninstall);
    EXPECT_EQ(uninstall->GetResultCode(), ERR_OK) << "uninstall fail!" << bundleName;
}

/**
 * @tc.number: ThirdAppInstall_0800
 * @tc.name: test third-party bundle install
 * @tc.desc: 1.under '/data/test/bms_bundle/',there are two haps with one ability,whose appname is equal
 *           2.install the app
 */
HWTEST_F(BmsBundleInstallerModuleTest, ThirdAppInstall_0800, Function | MediumTest | Level1)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    ASSERT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(receiver, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver2 = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(receiver2, nullptr);
    InstallParam installParam;

    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle1" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver);
    ASSERT_EQ(receiver->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string bundleName = THIRD_BUNDLE_NAME + "1";
    std::string modulePackage = "com.third.hiworld.example.h1";
    CheckFileExist(bundleName, modulePackage);

    bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle4" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver2);
    ASSERT_EQ(receiver2->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string modulePackage2 = "com.third.hiworld.example.h2";
    CheckFileExist(bundleName, modulePackage2);

    OHOS::sptr<MockStatusReceiver> recUninstall = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(recUninstall, nullptr);
    installParam.userId = Constants::DEFAULT_USERID;
    installer->Uninstall(bundleName, installParam, recUninstall);
    EXPECT_EQ(recUninstall->GetResultCode(), ERR_OK) << "uninstall fail!" << bundleName;
}

/**
 * @tc.number: ThirdAppInstall_0900
 * @tc.name: test third-party bundle install
 * @tc.desc: 1.under '/data/test/bms_bundle/',there are two haps without an ability,whose appname is equal
 *           2.install the app
 */
HWTEST_F(BmsBundleInstallerModuleTest, ThirdAppInstall_0900, Function | MediumTest | Level1)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    ASSERT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(receiver, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver2 = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(receiver2, nullptr);
    InstallParam installParam;

    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle3" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver);
    ASSERT_EQ(receiver->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string bundleName = THIRD_BUNDLE_NAME + "1";
    std::string modulePackage = "com.third.hiworld.example.h1";
    std::vector<std::string> hap1AbilityNames = {};
    CheckFileExist(bundleName, modulePackage, hap1AbilityNames);

    bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle6" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver2);
    ASSERT_EQ(receiver2->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string modulePackage2 = "com.third.hiworld.example.h2";
    std::vector<std::string> hap2AbilityNames = {};
    CheckFileExist(bundleName, modulePackage2, hap2AbilityNames);

    OHOS::sptr<MockStatusReceiver> recUninstall = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(recUninstall, nullptr);
    installParam.userId = Constants::DEFAULT_USERID;
    installer->Uninstall(bundleName, installParam, recUninstall);
    EXPECT_EQ(recUninstall->GetResultCode(), ERR_OK) << "uninstall fail!" << bundleName;
}

/**
 * @tc.number: ThirdAppInstall_1000
 * @tc.name: test third-party bundle upgrade
 * @tc.desc: 1.under '/data/test/bms_bundle/',there are two haps,one without an ability,the other with an ability
 *           2.install this app
 */
HWTEST_F(BmsBundleInstallerModuleTest, ThirdAppInstall_1000, Function | MediumTest | Level1)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    ASSERT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(receiver, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver2 = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(receiver2, nullptr);
    InstallParam installParam;

    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle3" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver);
    ASSERT_EQ(receiver->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string bundleName = THIRD_BUNDLE_NAME + "1";
    std::string modulePackage = "com.third.hiworld.example.h1";
    std::vector<std::string> hap1AbilityNames = {};
    CheckFileExist(bundleName, modulePackage, hap1AbilityNames);

    bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle4" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver2);
    ASSERT_EQ(receiver2->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string modulePackage2 = "com.third.hiworld.example.h2";
    std::vector<std::string> hap2AbilityNames = {"bmsThirdBundle_A1"};
    CheckFileExist(bundleName, modulePackage2, hap2AbilityNames);

    OHOS::sptr<MockStatusReceiver> recUninstall = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(recUninstall, nullptr);
    installParam.userId = Constants::DEFAULT_USERID;
    installer->Uninstall(bundleName, installParam, recUninstall);
    EXPECT_EQ(recUninstall->GetResultCode(), ERR_OK) << "uninstall fail!" << bundleName;
}

/**
 * @tc.number: ThirdAppInstall_1100
 * @tc.name: test third-party bundle upgrade
 * @tc.desc: 1.under '/data/test/bms_bundle/',there are two haps,
 *             one without an ability,the other with two abilities
 *           2.install this app
 */
HWTEST_F(BmsBundleInstallerModuleTest, ThirdAppInstall_1100, Function | MediumTest | Level1)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    ASSERT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(receiver, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver2 = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(receiver2, nullptr);
    InstallParam installParam;

    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle3" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver);
    ASSERT_EQ(receiver->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string bundleName = THIRD_BUNDLE_NAME + "1";
    std::string modulePackage = "com.third.hiworld.example.h1";
    std::vector<std::string> hap1AbilityNames = {};
    CheckFileExist(bundleName, modulePackage, hap1AbilityNames);

    bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle5" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver2);
    ASSERT_EQ(receiver2->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string modulePackage2 = "com.third.hiworld.example.h2";
    std::vector<std::string> hap2AbilityNames = {"bmsThirdBundle_A1", "bmsThirdBundle_A2"};
    CheckFileExist(bundleName, modulePackage2, hap2AbilityNames);

    OHOS::sptr<MockStatusReceiver> recUninstall = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(recUninstall, nullptr);
    installParam.userId = Constants::DEFAULT_USERID;
    installer->Uninstall(bundleName, installParam, recUninstall);
    EXPECT_EQ(recUninstall->GetResultCode(), ERR_OK) << "uninstall fail!" << bundleName;
}

/**
 * @tc.number: ThirdAppInstall_1200
 * @tc.name: test third-party bundle upgrade
 * @tc.desc: 1.under '/data/test/bms_bundle/',there are two haps,one without an ability,the other with an ability
 *           2.install this app
 */
HWTEST_F(BmsBundleInstallerModuleTest, ThirdAppInstall_1200, Function | MediumTest | Level1)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    ASSERT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(receiver, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver2 = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(receiver2, nullptr);
    InstallParam installParam;

    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle1" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver);
    ASSERT_EQ(receiver->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string bundleName = THIRD_BUNDLE_NAME + "1";
    std::string modulePackage = "com.third.hiworld.example.h1";
    std::vector<std::string> hap1AbilityNames = {"bmsThirdBundle_A1"};
    CheckFileExist(bundleName, modulePackage, hap1AbilityNames);

    bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle6" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver2);
    ASSERT_EQ(receiver2->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string modulePackage2 = "com.third.hiworld.example.h2";
    std::vector<std::string> hap2AbilityNames = {};
    CheckFileExist(bundleName, modulePackage2, hap2AbilityNames);

    OHOS::sptr<MockStatusReceiver> recUninstall = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(recUninstall, nullptr);
    installParam.userId = Constants::DEFAULT_USERID;
    installer->Uninstall(bundleName, installParam, recUninstall);
    EXPECT_EQ(recUninstall->GetResultCode(), ERR_OK) << "uninstall fail!" << bundleName;
}

/**
 * @tc.number: ThirdAppInstall_1300
 * @tc.name: test third-party bundle upgrade
 * @tc.desc: 1.under '/data/test/bms_bundle/',there are two haps with an ability,
 *                  2.install this app
 */
HWTEST_F(BmsBundleInstallerModuleTest, ThirdAppInstall_1300, Function | MediumTest | Level1)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    ASSERT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(receiver, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver2 = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(receiver2, nullptr);
    InstallParam installParam;

    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle1" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver);
    ASSERT_EQ(receiver->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string bundleName = THIRD_BUNDLE_NAME + "1";
    std::string modulePackage = "com.third.hiworld.example.h1";
    std::vector<std::string> hap1AbilityNames = {"bmsThirdBundle_A1"};
    CheckFileExist(bundleName, modulePackage, hap1AbilityNames);

    bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle4" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver2);
    ASSERT_EQ(receiver2->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string modulePackage2 = "com.third.hiworld.example.h2";
    std::vector<std::string> hap2AbilityNames = {"bmsThirdBundle_A1"};
    CheckFileExist(bundleName, modulePackage2, hap2AbilityNames);

    OHOS::sptr<MockStatusReceiver> recUninstall = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(recUninstall, nullptr);
    installParam.userId = Constants::DEFAULT_USERID;
    installer->Uninstall(bundleName, installParam, recUninstall);
    EXPECT_EQ(recUninstall->GetResultCode(), ERR_OK) << "uninstall fail!" << bundleName;
}

/**
 * @tc.number: ThirdAppInstall_1400
 * @tc.name: test third-party bundle upgrade
 * @tc.desc: 1.under '/data/test/bms_bundle/',there are two haps,one with an ability,the other with two abilities
 *           2.install this app
 */
HWTEST_F(BmsBundleInstallerModuleTest, ThirdAppInstall_1400, Function | MediumTest | Level1)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    ASSERT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(receiver, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver2 = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(receiver2, nullptr);
    InstallParam installParam;

    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle1" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver);
    ASSERT_EQ(receiver->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string bundleName = THIRD_BUNDLE_NAME + "1";
    std::string modulePackage = "com.third.hiworld.example.h1";
    std::vector<std::string> hap1AbilityNames = {"bmsThirdBundle_A1"};
    CheckFileExist(bundleName, modulePackage, hap1AbilityNames);

    bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle5" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver2);
    ASSERT_EQ(receiver2->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string modulePackage2 = "com.third.hiworld.example.h2";
    std::vector<std::string> hap2AbilityNames = {"bmsThirdBundle_A1", "bmsThirdBundle_A2"};
    CheckFileExist(bundleName, modulePackage2, hap2AbilityNames);

    OHOS::sptr<MockStatusReceiver> recUninstall = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(recUninstall, nullptr);
    installParam.userId = Constants::DEFAULT_USERID;
    installer->Uninstall(bundleName, installParam, recUninstall);
    EXPECT_EQ(recUninstall->GetResultCode(), ERR_OK) << "uninstall fail!" << bundleName;
}

/**
 * @tc.number: ThirdAppInstall_1500
 * @tc.name: test third-party bundle upgrade
 * @tc.desc: 1.under '/data/test/bms_bundle/',there are two haps,
 *             one without an ability,the other with two abilities
 *           2.install this app
 */
HWTEST_F(BmsBundleInstallerModuleTest, ThirdAppInstall_1500, Function | MediumTest | Level1)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    ASSERT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(receiver, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver2 = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(receiver2, nullptr);
    InstallParam installParam;

    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle2" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver);
    ASSERT_EQ(receiver->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string bundleName = THIRD_BUNDLE_NAME + "1";
    std::string modulePackage = "com.third.hiworld.example.h1";
    std::vector<std::string> hap1AbilityNames = {"bmsThirdBundle_A1", "bmsThirdBundle_A2"};
    CheckFileExist(bundleName, modulePackage, hap1AbilityNames);

    bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle6" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver2);
    ASSERT_EQ(receiver2->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string modulePackage2 = "com.third.hiworld.example.h2";
    std::vector<std::string> hap2AbilityNames = {};
    CheckFileExist(bundleName, modulePackage2, hap2AbilityNames);

    OHOS::sptr<MockStatusReceiver> recUninstall = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(recUninstall, nullptr);
    installParam.userId = Constants::DEFAULT_USERID;
    installer->Uninstall(bundleName, installParam, recUninstall);
    EXPECT_EQ(recUninstall->GetResultCode(), ERR_OK) << "uninstall fail!" << bundleName;
}

/**
 * @tc.number: ThirdAppInstall_1600
 * @tc.name: test third-party bundle upgrade
 * @tc.desc: 1.under '/data/test/bms_bundle/',there are two haps,one with an ability,the other with two abilities
 *           2.install this app
 */
HWTEST_F(BmsBundleInstallerModuleTest, ThirdAppInstall_1600, Function | MediumTest | Level1)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    ASSERT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(receiver, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver2 = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(receiver2, nullptr);
    InstallParam installParam;

    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle2" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver);
    ASSERT_EQ(receiver->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string bundleName = THIRD_BUNDLE_NAME + "1";
    std::string modulePackage = "com.third.hiworld.example.h1";
    std::vector<std::string> hap1AbilityNames = {"bmsThirdBundle_A1", "bmsThirdBundle_A2"};
    CheckFileExist(bundleName, modulePackage, hap1AbilityNames);

    bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle4" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver2);
    ASSERT_EQ(receiver2->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string modulePackage2 = "com.third.hiworld.example.h2";
    std::vector<std::string> hap2AbilityNames = {"bmsThirdBundle_A1"};
    CheckFileExist(bundleName, modulePackage2, hap2AbilityNames);

    OHOS::sptr<MockStatusReceiver> recUninstall = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(recUninstall, nullptr);
    installParam.userId = Constants::DEFAULT_USERID;
    installer->Uninstall(bundleName, installParam, recUninstall);
    EXPECT_EQ(recUninstall->GetResultCode(), ERR_OK) << "uninstall fail!" << bundleName;
}

/**
 * @tc.number: ThirdAppInstall_1700
 * @tc.name: test third-party bundle upgrade
 * @tc.desc: 1.under '/data/test/bms_bundle/',there are two haps with two abilities,
 *           2.install this app
 */
HWTEST_F(BmsBundleInstallerModuleTest, ThirdAppInstall_1700, Function | MediumTest | Level1)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    ASSERT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(receiver, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver2 = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(receiver2, nullptr);
    InstallParam installParam;

    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle2" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver);
    ASSERT_EQ(receiver->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string bundleName = THIRD_BUNDLE_NAME + "1";
    std::string modulePackage = "com.third.hiworld.example.h1";
    std::vector<std::string> hap1AbilityNames = {"bmsThirdBundle_A1", "bmsThirdBundle_A2"};
    CheckFileExist(bundleName, modulePackage, hap1AbilityNames);

    bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle5" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver2);
    ASSERT_EQ(receiver2->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string modulePackage2 = "com.third.hiworld.example.h2";
    std::vector<std::string> hap2AbilityNames = {"bmsThirdBundle_A1", "bmsThirdBundle_A2"};
    CheckFileExist(bundleName, modulePackage2, hap2AbilityNames);

    OHOS::sptr<MockStatusReceiver> recUninstall = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(recUninstall, nullptr);
    installParam.userId = Constants::DEFAULT_USERID;
    installer->Uninstall(bundleName, installParam, recUninstall);
    EXPECT_EQ(recUninstall->GetResultCode(), ERR_OK) << "uninstall fail!" << bundleName;
}

/**
 * @tc.number: BundleDataStorage_0100
 * @tc.name: check bundle data storage
 * @tc.desc: 1.save a new bundle installation information for the first time successfully
 */
HWTEST_F(BmsBundleInstallerModuleTest, BundleDataStorage_0100, Function | MediumTest | Level1)
{
    APP_LOGI("BmsBundleInstallerModuleTest::BundleDataStorage_0100 begin");
    InnerBundleInfo innerBundleInfo;
    bool result = innerBundleInfo.FromJson(innerBundleInfoJson_);
    EXPECT_TRUE(result);
    ClearJsonFile();
    CheckBundleSaved(innerBundleInfo);
    CheckBundleDeleted(innerBundleInfo);
    APP_LOGI("BmsBundleDataStorageTest::BundleDataStorage_0100 end");
}

/**
 * @tc.number: BundleDataStorage_0200
 * @tc.name: save bundle install information to persist storage
 * @tc.desc: 1.Update the existing db data
 */
HWTEST_F(BmsBundleInstallerModuleTest, BundleDataStorage_0200, Function | MediumTest | Level1)
{
    APP_LOGI("BmsBundleInstallerModuleTest::BundleDataStorage_0200 begin");

    InnerBundleInfo innerBundleInfo;
    innerBundleInfo.FromJson(innerBundleInfoJson_);
    ClearJsonFile();
    CheckBundleSaved(innerBundleInfo);
    CheckBundleDeleted(innerBundleInfo);

    BundleInfo bundleInfo = bundleInfoJson_;
    bundleInfo.description = "update test application";
    InnerBundleInfo otherInnerBundleInfo = innerBundleInfo;
    otherInnerBundleInfo.SetBaseBundleInfo(bundleInfo);

    CheckBundleSaved(otherInnerBundleInfo);
    ClearJsonFile();
    APP_LOGI("BmsBundleInstallerModuleTest::BundleDataStorage_0200 end");
}
