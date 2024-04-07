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

#include <chrono>
#include <thread>
#include <gtest/gtest.h>
#include <sys/stat.h>
#include <unistd.h>

#include "directory_ex.h"
#include "appexecfwk_errors.h"
#include "common_tool.h"
#include "bundle_info.h"
#include "bundle_installer_host.h"
#include "bundle_mgr_service.h"
#include "installd/installd_service.h"
#include "installd_client.h"
#include "mock_status_receiver.h"

using namespace testing::ext;
using namespace std::chrono_literals;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

namespace {

const std::string PACKAGE_NAME = "com.example.l3jsdemo";
const std::string BUNDLE_NAME = "com.example.l3jsdemo";
const std::string ERROR_BUNDLE_NAME = "com.example.bundle.update.error";
const std::string BUNDLE_FILE_DIR = "/data/test/resource/bms/update_bundle/";
const std::string V1_BUNDLE = "version1.hap";
const std::string V2_BUNDLE = "version2.hap";
const std::string V3_BUNDLE = "version3.hap";
const std::string ERROR_FORMART_BUNDLE = "format_error_profile.hap";
const std::string BUNDLE_DATA_DIR = "/data/accounts/account_0/appdata/com.example.l3jsdemo";
const std::string BUNDLE_CODE_DIR = "/data/accounts/account_0/applications/com.example.l3jsdemo";
const std::string ROOT_DIR = "/data/accounts";
const std::string PROFILE_FILE = "config.json";
const std::string SEPARATOR = "/";
const std::chrono::seconds SLEEP_TIME {2};
const int32_t ROOT_UID = 0;
const int32_t USERID = 0;
const uint32_t VERSION_1 = 1;
const uint32_t VERSION_2 = 2;
const uint32_t VERSION_3 = 3;
const int32_t MAX_TRY_TIMES = 1000;

}  // namespace

class BmsBundleUpdaterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    const std::shared_ptr<BundleDataMgr> GetBundleDataMgr() const;
    ErrCode InstallBundle(const std::string &bundlePath) const;
    ErrCode UninstallBundle(const std::string &bundleName) const;
    ErrCode UpdateBundle(const std::string &bundlePath) const;
    ErrCode UpdateBundle(const std::string &bundlePath, const bool needCheckInfo) const;

    void StopInstalldService() const;
    void StartInstalldService() const;
    void StopBundleService() const;

    void CheckFileExist() const;
    bool CheckApplicationInfo() const;
    bool CheckBundleInfo(const uint32_t versionCode, const bool needCheckVersion) const;

private:
    std::shared_ptr<InstalldService> installdService_ = std::make_unique<InstalldService>();
    std::shared_ptr<BundleMgrService> bundleMgrService_ = DelayedSingleton<BundleMgrService>::GetInstance();
};

void BmsBundleUpdaterTest::SetUpTestCase()
{
    if (access(ROOT_DIR.c_str(), F_OK) != 0) {
        bool result = OHOS::ForceCreateDirectory(ROOT_DIR);
        ASSERT_TRUE(result) << "fail to create root dir";
    }
    if (chown(ROOT_DIR.c_str(), ROOT_UID, ROOT_UID) != 0) {
        ASSERT_TRUE(false) << "fail to change root dir own ship";
    }
    if (chmod(ROOT_DIR.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) != 0) {
        ASSERT_TRUE(false) << "fail to change root dir mode";
    }
}

void BmsBundleUpdaterTest::TearDownTestCase()
{}

void BmsBundleUpdaterTest::SetUp()
{
    installdService_->Start();
    if (!DelayedSingleton<BundleMgrService>::GetInstance()->IsServiceReady()) {
        DelayedSingleton<BundleMgrService>::GetInstance()->OnStart();
    }
}

void BmsBundleUpdaterTest::TearDown()
{
    // reset the case.
    UninstallBundle(BUNDLE_NAME);

    StopInstalldService();
    StopBundleService();

    // clear files.
    OHOS::ForceRemoveDirectory(BUNDLE_DATA_DIR);
    OHOS::ForceRemoveDirectory(BUNDLE_CODE_DIR);
}

const std::shared_ptr<BundleDataMgr> BmsBundleUpdaterTest::GetBundleDataMgr() const
{
    return bundleMgrService_->GetDataMgr();
}

ErrCode BmsBundleUpdaterTest::InstallBundle(const std::string &bundlePath) const
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    if (!installer) {
        EXPECT_FALSE(true) << "the installer is nullptr";
        return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
    }

    sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    if (!receiver) {
        EXPECT_FALSE(true) << "the receiver is nullptr";
        return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
    }
    InstallParam installParam;
    installParam.installFlag = InstallFlag::NORMAL;
    bool result = installer->Install(bundlePath, installParam, receiver);
    EXPECT_TRUE(result);
    return receiver->GetResultCode();
}

ErrCode BmsBundleUpdaterTest::UninstallBundle(const std::string &bundleName) const
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    if (!installer) {
        EXPECT_FALSE(true) << "the installer is nullptr";
        return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
    }

    sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    if (!receiver) {
        EXPECT_FALSE(true) << "the receiver is nullptr";
        return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
    }
    InstallParam installParam;
    installParam.installFlag = InstallFlag::NORMAL;
    bool result = installer->Uninstall(bundleName, installParam, receiver);
    EXPECT_TRUE(result);
    return receiver->GetResultCode();
}

ErrCode BmsBundleUpdaterTest::UpdateBundle(const std::string &bundlePath) const
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    if (!installer) {
        EXPECT_FALSE(true) << "the installer is nullptr";
        return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
    }

    sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    if (!receiver) {
        EXPECT_FALSE(true) << "the receiver is nullptr";
        return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
    }
    InstallParam installParam;
    installParam.installFlag = InstallFlag::REPLACE_EXISTING;
    bool result = installer->Install(bundlePath, installParam, receiver);
    EXPECT_TRUE(result);
    return receiver->GetResultCode();
}

ErrCode BmsBundleUpdaterTest::UpdateBundle(const std::string &bundlePath, const bool needCheckInfo) const
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    if (!installer) {
        EXPECT_FALSE(true) << "the installer is nullptr";
        return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
    }
    sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    if (!receiver) {
        EXPECT_FALSE(true) << "the receiver is nullptr";
        return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
    }
    InstallParam installParam;
    installParam.installFlag = InstallFlag::REPLACE_EXISTING;
    bool result = installer->Install(bundlePath, installParam, receiver);
    EXPECT_TRUE(result);

    // check can not access the application info between updating.
    if (needCheckInfo) {
        bool isBlock = false;
        for (int32_t i = 0; i < MAX_TRY_TIMES; i++) {
            std::this_thread::sleep_for(10ms);
            bool isExist = CheckApplicationInfo();
            if (!isExist) {
                isBlock = true;
                break;
            }
        }
        if (!isBlock) {
            EXPECT_FALSE(true) << "the bundle info is not disable during updating";
            return ERR_APPEXECFWK_INSTALL_BUNDLE_MGR_SERVICE_ERROR;
        }
    }

    return receiver->GetResultCode();
}

void BmsBundleUpdaterTest::StopInstalldService() const
{
    installdService_->Stop();
    InstalldClient::GetInstance()->ResetInstalldProxy();
}

void BmsBundleUpdaterTest::StartInstalldService() const
{
    installdService_->Start();
}

void BmsBundleUpdaterTest::StopBundleService() const
{
    DelayedSingleton<BundleMgrService>::GetInstance()->OnStop();
}

void BmsBundleUpdaterTest::CheckFileExist() const
{
    CommonTool tool;
    bool isCodeExist = tool.CheckFilePathISExist(BUNDLE_CODE_DIR + SEPARATOR + PACKAGE_NAME);
    EXPECT_TRUE(isCodeExist);
    bool isDataExist = tool.CheckFilePathISExist(BUNDLE_DATA_DIR + SEPARATOR + PACKAGE_NAME);
    EXPECT_TRUE(isDataExist);
}

bool BmsBundleUpdaterTest::CheckBundleInfo(const uint32_t versionCode, const bool needCheckVersion) const
{
    BundleInfo info;
    auto dataMgr = GetBundleDataMgr();
    if (dataMgr == nullptr) {
        return false;
    }
    bool isExist = dataMgr->GetBundleInfo(BUNDLE_NAME, BundleFlag::GET_BUNDLE_DEFAULT, info);
    if (!isExist) {
        return false;
    }
    if (needCheckVersion) {
        if (info.versionCode != versionCode) {
            return false;
        }
    }
    return true;
}

bool BmsBundleUpdaterTest::CheckApplicationInfo() const
{
    ApplicationInfo info;
    auto dataMgr = GetBundleDataMgr();
    if (dataMgr == nullptr) {
        return false;
    }
    bool result = dataMgr->GetApplicationInfo(PACKAGE_NAME, ApplicationFlag::GET_BASIC_APPLICATION_INFO, USERID, info);
    return result;
}

/**
 * @tc.number: Update_0100
 * @tc.name: test the same version bundle can be reinstalled
 * @tc.desc: 1. the bundle is already installed
 *           2. the same version bundle can be reinstalled successfully
 */
HWTEST_F(BmsBundleUpdaterTest, Update_0100, Function | SmallTest | Level2)
{
    ErrCode installResult = InstallBundle(BUNDLE_FILE_DIR + V1_BUNDLE);
    ASSERT_EQ(installResult, ERR_OK);
    CommonTool tool;
    long codeDirFirstCreateTime = tool.GetFileBuildTime(BUNDLE_CODE_DIR.c_str());
    long dataDirFirstCreateTime = tool.GetFileBuildTime(BUNDLE_DATA_DIR.c_str());

    // need to wait for 1s since the file create time counts in second unit.
    std::this_thread::sleep_for(SLEEP_TIME);

    ErrCode updateResult = UpdateBundle(BUNDLE_FILE_DIR + V2_BUNDLE, true);
    ASSERT_EQ(updateResult, ERR_OK);

    long codeDirSecondCreateTime = tool.GetFileBuildTime(BUNDLE_CODE_DIR.c_str());
    long dataDirSecondCreateTime = tool.GetFileBuildTime(BUNDLE_DATA_DIR.c_str());
    ASSERT_EQ(dataDirFirstCreateTime, dataDirSecondCreateTime);
    ASSERT_NE(codeDirFirstCreateTime, codeDirSecondCreateTime);

    bool isExist = CheckBundleInfo(VERSION_2, true);
    EXPECT_TRUE(isExist);
}

/**
 * @tc.number: Update_0200
 * @tc.name: test the larger version bundle can be updated
 * @tc.desc: 1. the bundle is already installed
 *           2. the larger version bundle can be updated successfully
 */
HWTEST_F(BmsBundleUpdaterTest, Update_0200, Function | SmallTest | Level1)
{
    ErrCode installResult = InstallBundle(BUNDLE_FILE_DIR + V2_BUNDLE);
    ASSERT_EQ(installResult, ERR_OK);

    ErrCode updateResult = UpdateBundle(BUNDLE_FILE_DIR + V3_BUNDLE);
    ASSERT_EQ(updateResult, ERR_OK);
    CheckFileExist();

    bool result = CheckBundleInfo(VERSION_3, true);
    EXPECT_TRUE(result);
}

/**
 * @tc.number: Update_0300
 * @tc.name: test the empty path can't be updated
 * @tc.desc: 1. the bundle file path is empty
 *           2. the bundle can't be updated and the result is fail
 */
HWTEST_F(BmsBundleUpdaterTest, Update_0300, Function | SmallTest | Level0)
{
    ErrCode installResult = InstallBundle(BUNDLE_FILE_DIR + V1_BUNDLE);
    ASSERT_EQ(installResult, ERR_OK);

    ErrCode updateResult = UpdateBundle("");
    EXPECT_EQ(updateResult, ERR_APPEXECFWK_INSTALL_FILE_PATH_INVALID);
}

/**
 * @tc.number: Update_0400
 * @tc.name: test the wrong path can't be updated
 * @tc.desc: 1. the bundle file path is wrong
 *           2. the bundle can't be updated and the result is fail
 */
HWTEST_F(BmsBundleUpdaterTest, Update_0400, Function | SmallTest | Level0)
{
    ErrCode installResult = InstallBundle(BUNDLE_FILE_DIR + V1_BUNDLE);
    ASSERT_EQ(installResult, ERR_OK);

    ErrCode updateResult = UpdateBundle(BUNDLE_FILE_DIR + ERROR_BUNDLE_NAME);
    EXPECT_EQ(updateResult, ERR_APPEXECFWK_INSTALL_INVALID_HAP_NAME);
}

/**
 * @tc.number: Update_0500
 * @tc.name: test the wrong format bundle can't be updated
 * @tc.desc: 1. the bundle file is wrong format
 *           2. the bundle can't be updated and the result is fail
 */
HWTEST_F(BmsBundleUpdaterTest, Update_0500, Function | SmallTest | Level0)
{
    ErrCode installResult = InstallBundle(BUNDLE_FILE_DIR + V1_BUNDLE);
    ASSERT_EQ(installResult, ERR_OK);

    ErrCode updateResult = UpdateBundle(BUNDLE_FILE_DIR + ERROR_FORMART_BUNDLE);
    EXPECT_EQ(updateResult, ERR_APPEXECFWK_PARSE_NO_PROFILE);
}

/**
 * @tc.number: Update_0600
 * @tc.name: test the lower version bundle can't be updated
 * @tc.desc: 1. the bundle file is the lower version
 *           2. the bundle can't be updated and the result is fail
 */
HWTEST_F(BmsBundleUpdaterTest, Update_0600, Function | SmallTest | Level0)
{
    ErrCode installResult = InstallBundle(BUNDLE_FILE_DIR + V2_BUNDLE);
    ASSERT_EQ(installResult, ERR_OK);

    ErrCode updateResult = UpdateBundle(BUNDLE_FILE_DIR + V1_BUNDLE);
    EXPECT_EQ(updateResult, ERR_APPEXECFWK_INSTALL_VERSION_DOWNGRADE);
}

/**
 * @tc.number: Update_0700
 * @tc.name: test the installd service error case
 * @tc.desc: 1. the installd service error
 *           2. the update operation will return fail and the next install operation will success after restart installd
 *              service
 */
HWTEST_F(BmsBundleUpdaterTest, Update_0700, Function | SmallTest | Level1)
{
    ErrCode installResult = InstallBundle(BUNDLE_FILE_DIR + V1_BUNDLE);
    ASSERT_EQ(installResult, ERR_OK);

    StopInstalldService();
    ErrCode updateResult = UpdateBundle(BUNDLE_FILE_DIR + V3_BUNDLE);
    ASSERT_EQ(updateResult, ERR_APPEXECFWK_INSTALLD_GET_PROXY_ERROR);

    BundleInfo info;
    auto dataMgr = GetBundleDataMgr();
    ASSERT_NE(dataMgr, nullptr);
    bool isInfoExist = dataMgr->GetBundleInfo(BUNDLE_NAME, BundleFlag::GET_BUNDLE_DEFAULT, info);
    ASSERT_TRUE(isInfoExist);
    ASSERT_EQ(info.versionCode, VERSION_1);

    StartInstalldService();
    updateResult = UpdateBundle(BUNDLE_FILE_DIR + V3_BUNDLE);
    ASSERT_EQ(updateResult, ERR_OK);
    ASSERT_EQ(info.versionCode, VERSION_1);
    CheckFileExist();
}

/**
 * @tc.number: Update_0800
 * @tc.name: test the bundle mgr service error case
 * @tc.desc: 1. the bundle mgr service error
 *           2. the update operation will return fail and the next install operation will success after restart bundle
 * mgr service
 */
HWTEST_F(BmsBundleUpdaterTest, Update_0800, Function | SmallTest | Level1)
{
    ErrCode installResult = InstallBundle(BUNDLE_FILE_DIR + V1_BUNDLE);
    ASSERT_EQ(installResult, ERR_OK);

    StopBundleService();
    DelayedSingleton<BundleMgrService>::DestroyInstance();

    sptr<BundleInstallerHost> installer = new (std::nothrow) BundleInstallerHost();
    ASSERT_NE(installer, nullptr);
    installer->Init();
    sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    ASSERT_NE(receiver, nullptr);
    InstallParam installParam;
    installParam.installFlag = InstallFlag::REPLACE_EXISTING;
    installer->Install(BUNDLE_FILE_DIR + V3_BUNDLE, installParam, receiver);
    ErrCode result = receiver->GetResultCode();
    ASSERT_EQ(result, ERR_APPEXECFWK_INSTALL_BUNDLE_MGR_SERVICE_ERROR);

    DelayedSingleton<BundleMgrService>::GetInstance()->OnStart();
    BundleInfo info;
    auto dataMgr = GetBundleDataMgr();
    ASSERT_NE(dataMgr, nullptr);
    bool isInfoExist = dataMgr->GetBundleInfo(BUNDLE_NAME, BundleFlag::GET_BUNDLE_DEFAULT, info);
    ASSERT_TRUE(isInfoExist);
    ASSERT_EQ(info.versionCode, VERSION_1);

    ErrCode updateResult = UpdateBundle(BUNDLE_FILE_DIR + V3_BUNDLE);
    ASSERT_EQ(updateResult, ERR_OK);
    CheckFileExist();
}

/**
 * @tc.number: Update_0900
 * @tc.name: test when status receiver is null will not update bundle
 * @tc.desc: 1. the status receiver is null
 *           2. the update bundle operation will not execute when status receiver is null
 */
HWTEST_F(BmsBundleUpdaterTest, Update_0900, Function | SmallTest | Level2)
{
    ErrCode installResult = InstallBundle(BUNDLE_FILE_DIR + V1_BUNDLE);
    ASSERT_EQ(installResult, ERR_OK);
    CommonTool tool;
    long codeDirFirstCreateTime = tool.GetFileBuildTime(BUNDLE_CODE_DIR.c_str());
    long dataDirFirstCreateTime = tool.GetFileBuildTime(BUNDLE_DATA_DIR.c_str());

    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    ASSERT_FALSE(!installer);
    InstallParam installParam;
    installParam.installFlag = InstallFlag::REPLACE_EXISTING;
    installer->Install(BUNDLE_FILE_DIR + V2_BUNDLE, installParam, nullptr);

    std::this_thread::sleep_for(50ms);

    long codeDirSecondCreateTime = tool.GetFileBuildTime(BUNDLE_CODE_DIR.c_str());
    long dataDirSecondCreateTime = tool.GetFileBuildTime(BUNDLE_DATA_DIR.c_str());

    ASSERT_EQ(dataDirFirstCreateTime, dataDirSecondCreateTime);
    ASSERT_EQ(codeDirFirstCreateTime, codeDirSecondCreateTime);

    bool isExist = CheckBundleInfo(VERSION_1, true);
    EXPECT_TRUE(isExist);
}

/**
 * @tc.number: Update_1000
 * @tc.name: test the larger version bundle can be installed continuously
 * @tc.desc: 1. the bundle is already installed
 *           2. the larger version bundle can be installed continuously
 */
HWTEST_F(BmsBundleUpdaterTest, Update_1000, Function | SmallTest | Level2)
{
    ErrCode installResult = InstallBundle(BUNDLE_FILE_DIR + V1_BUNDLE);
    ASSERT_EQ(installResult, ERR_OK);
    CommonTool tool;
    long codeDirFirstCreateTime = tool.GetFileBuildTime(BUNDLE_CODE_DIR.c_str());
    long dataDirFirstCreateTime = tool.GetFileBuildTime(BUNDLE_DATA_DIR.c_str());

    // need to wait for 1s since the file create time counts in second unit.
    std::this_thread::sleep_for(SLEEP_TIME);

    ErrCode updateResult = UpdateBundle(BUNDLE_FILE_DIR + V2_BUNDLE);
    ASSERT_EQ(updateResult, ERR_OK);

    long codeDirSecondCreateTime = tool.GetFileBuildTime(BUNDLE_CODE_DIR.c_str());
    long dataDirSecondCreateTime = tool.GetFileBuildTime(BUNDLE_DATA_DIR.c_str());
    ASSERT_EQ(dataDirFirstCreateTime, dataDirSecondCreateTime);
    ASSERT_NE(codeDirFirstCreateTime, codeDirSecondCreateTime);

    bool isExist = CheckBundleInfo(VERSION_2, true);
    EXPECT_TRUE(isExist);

    std::this_thread::sleep_for(SLEEP_TIME);

    updateResult = UpdateBundle(BUNDLE_FILE_DIR + V3_BUNDLE);
    ASSERT_EQ(updateResult, ERR_OK);

    long codeDirThirdCreateTime = tool.GetFileBuildTime(BUNDLE_CODE_DIR.c_str());
    long dataDirThirdCreateTime = tool.GetFileBuildTime(BUNDLE_DATA_DIR.c_str());
    ASSERT_EQ(dataDirSecondCreateTime, dataDirThirdCreateTime);
    ASSERT_NE(codeDirSecondCreateTime, codeDirThirdCreateTime);

    isExist = CheckBundleInfo(VERSION_3, true);
    EXPECT_TRUE(isExist);
}
