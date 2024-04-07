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
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <gtest/gtest.h>

#include "directory_ex.h"
#include "appexecfwk_errors.h"
#include "bundle_info.h"
#include "bundle_installer_host.h"
#include "bundle_mgr_service.h"
#include "installd/installd_service.h"
#include "installd_client.h"
#include "mock_status_receiver.h"
#include "install_param.h"
#include "system_bundle_installer.h"

using namespace testing::ext;
using namespace std::chrono_literals;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

namespace {

const std::string BUNDLE_NAME = "com.example.l3jsdemo";
const std::string MODULE_PACKAGE = "com.example.l3jsdemo";
const std::string MODULE_PACKAGE1 = "com.example.l3jsdemo1";
const std::string ERROR_BUNDLE_NAME = "com.example.bundle.uninstall.error";
const std::string ERROR_MODULE_PACKAGE_NAME = "com.example.module.uninstall.error";
const std::string BUNDLE_FILE_PATH = "/data/test/resource/bms/uninstall_bundle/right.hap";
const std::string BUNDLE_FILE_PATH1 = "/data/test/resource/bms/uninstall_bundle/right1.hap";
const std::string BUNDLE_DATA_DIR = "/data/accounts/account_0/appdata/com.example.l3jsdemo";
const std::string BUNDLE_CODE_DIR = "/data/accounts/account_0/applications/com.example.l3jsdemo";
const std::string MODULE_DATA_DIR = "/data/accounts/account_0/appdata/com.example.l3jsdemo/com.example.l3jsdemo";
const std::string MODULE_CODE_DIR = "/data/accounts/account_0/applications/com.example.l3jsdemo/com.example.l3jsdemo";
const std::string MODULE_DATA_DIR1 = "/data/accounts/account_0/appdata/com.example.l3jsdemo/com.example.l3jsdemo1";
const std::string MODULE_CODE_DIR1 = "/data/accounts/account_0/applications/com.example.l3jsdemo/com.example.l3jsdemo1";
const std::string ROOT_DIR = "/data/accounts";
const std::string DB_FILE_PATH = "/data/bundlemgr";
const int32_t ROOT_UID = 0;

}  // namespace

class BmsBundleUninstallerTest : public testing::Test {
public:
    BmsBundleUninstallerTest();
    ~BmsBundleUninstallerTest();

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    ErrCode InstallBundle(const std::string &bundlePath) const;
    ErrCode UninstallBundle(const std::string &bundleName) const;
    ErrCode UninstallModule(const std::string &bundleName, const std::string &modulePackage) const;
    void CheckFileExist() const;
    void CheckModuleFileExist() const;
    void CheckModuleFileExist1() const;
    void CheckFileNonExist() const;
    void CheckModuleFileNonExist() const;
    void StopInstalldService() const;
    void StartInstalldService() const;
    void StartBundleService();
    void StopBundleService();
    void CheckBundleInfoExist() const;
    void CheckBundleInfoNonExist() const;
    void ClearJsonFile() const;
    const std::shared_ptr<BundleMgrService> GetBundleMgrService() const;
    void DeleteInstallFiles();

private:
    std::shared_ptr<InstalldService> installdService_ = std::make_unique<InstalldService>();
    std::shared_ptr<BundleMgrService> bundleMgrService_ = DelayedSingleton<BundleMgrService>::GetInstance();
};

BmsBundleUninstallerTest::BmsBundleUninstallerTest()
{}

BmsBundleUninstallerTest::~BmsBundleUninstallerTest()
{}

void BmsBundleUninstallerTest::SetUpTestCase()
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

void BmsBundleUninstallerTest::TearDownTestCase()
{}

void BmsBundleUninstallerTest::SetUp()
{
    StartBundleService();
    StartInstalldService();
}

void BmsBundleUninstallerTest::TearDown()
{
    StopInstalldService();
    StopBundleService();
}

void BmsBundleUninstallerTest::ClearJsonFile() const
{
    OHOS::RemoveFile(Constants::BUNDLE_DATA_BASE_FILE);
    std::fstream o(Constants::BUNDLE_DATA_BASE_FILE);
    if (!o.is_open()) {
        return;
    }
    o.close();
}

ErrCode BmsBundleUninstallerTest::InstallBundle(const std::string &bundlePath) const
{
    if (!bundleMgrService_) {
        return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
    }
    auto installer = bundleMgrService_->GetBundleInstaller();
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

ErrCode BmsBundleUninstallerTest::UninstallBundle(const std::string &bundleName) const
{
    if (!bundleMgrService_) {
        return ERR_APPEXECFWK_UNINSTALL_BUNDLE_MGR_SERVICE_ERROR;
    }
    auto installer = bundleMgrService_->GetBundleInstaller();
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

ErrCode BmsBundleUninstallerTest::UninstallModule(const std::string &bundleName, const std::string &modulePackage) const
{
    if (!bundleMgrService_) {
        return ERR_APPEXECFWK_UNINSTALL_BUNDLE_MGR_SERVICE_ERROR;
    }
    auto installer = bundleMgrService_->GetBundleInstaller();
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
    bool result = installer->Uninstall(bundleName, modulePackage, installParam, receiver);
    EXPECT_TRUE(result);
    return receiver->GetResultCode();
}

void BmsBundleUninstallerTest::CheckFileExist() const
{
    int bundleDataExist = access(BUNDLE_DATA_DIR.c_str(), F_OK);
    ASSERT_EQ(bundleDataExist, 0);

    int bundleCodeExist = access(BUNDLE_CODE_DIR.c_str(), F_OK);
    ASSERT_EQ(bundleCodeExist, 0);
}

void BmsBundleUninstallerTest::CheckModuleFileExist() const
{
    int moduleDataExist = access(MODULE_DATA_DIR.c_str(), F_OK);
    ASSERT_EQ(moduleDataExist, 0);

    int moduleCodeExist = access(MODULE_CODE_DIR.c_str(), F_OK);
    ASSERT_EQ(moduleCodeExist, 0);
}

void BmsBundleUninstallerTest::CheckModuleFileExist1() const
{
    int moduleDataExist1 = access(MODULE_DATA_DIR1.c_str(), F_OK);
    ASSERT_EQ(moduleDataExist1, 0);

    int moduleCodeExist1 = access(MODULE_CODE_DIR1.c_str(), F_OK);
    ASSERT_EQ(moduleCodeExist1, 0);
}

void BmsBundleUninstallerTest::CheckFileNonExist() const
{
    int bundleDataExist = access(BUNDLE_DATA_DIR.c_str(), F_OK);
    ASSERT_NE(bundleDataExist, 0);

    int bundleCodeExist = access(BUNDLE_CODE_DIR.c_str(), F_OK);
    ASSERT_NE(bundleCodeExist, 0);
}

void BmsBundleUninstallerTest::CheckModuleFileNonExist() const
{
    int moduleDataExist = access(MODULE_DATA_DIR.c_str(), F_OK);
    ASSERT_NE(moduleDataExist, 0);

    int moduleCodeExist = access(MODULE_CODE_DIR.c_str(), F_OK);
    ASSERT_NE(moduleCodeExist, 0);
}

void BmsBundleUninstallerTest::StopInstalldService() const
{
    installdService_->Stop();
    InstalldClient::GetInstance()->ResetInstalldProxy();
}

void BmsBundleUninstallerTest::StartInstalldService() const
{
    installdService_->Start();
}

void BmsBundleUninstallerTest::StartBundleService()
{
    if (!bundleMgrService_) {
        bundleMgrService_ = DelayedSingleton<BundleMgrService>::GetInstance();
    }
    if (bundleMgrService_) {
        bundleMgrService_->OnStart();
    }
}

void BmsBundleUninstallerTest::StopBundleService()
{
    if (bundleMgrService_) {
        bundleMgrService_->OnStop();
        bundleMgrService_ = nullptr;
    }
}

void BmsBundleUninstallerTest::CheckBundleInfoExist() const
{
    ASSERT_NE(bundleMgrService_, nullptr);
    auto dataMgr = bundleMgrService_->GetDataMgr();
    ASSERT_NE(dataMgr, nullptr);
    BundleInfo info;
    bool isBundleExist = dataMgr->GetBundleInfo(BUNDLE_NAME, BundleFlag::GET_BUNDLE_DEFAULT, info);
    ASSERT_TRUE(isBundleExist);
}

void BmsBundleUninstallerTest::CheckBundleInfoNonExist() const
{
    ASSERT_NE(bundleMgrService_, nullptr);
    auto dataMgr = bundleMgrService_->GetDataMgr();
    ASSERT_NE(dataMgr, nullptr);
    BundleInfo info;
    bool isBundleExist = dataMgr->GetBundleInfo(BUNDLE_NAME, BundleFlag::GET_BUNDLE_DEFAULT, info);
    ASSERT_FALSE(isBundleExist);
}

const std::shared_ptr<BundleMgrService> BmsBundleUninstallerTest::GetBundleMgrService() const
{
    return bundleMgrService_;
}

void BmsBundleUninstallerTest::DeleteInstallFiles()
{
    DelayedSingleton<BundleMgrService>::DestroyInstance();
    // clear files.
    ClearJsonFile();
    OHOS::ForceRemoveDirectory(BUNDLE_DATA_DIR);
    OHOS::ForceRemoveDirectory(BUNDLE_CODE_DIR);
    bundleMgrService_ = DelayedSingleton<BundleMgrService>::GetInstance();
}

/**
 * @tc.number: Bundle_Uninstall_0100
 * @tc.name: test the installed bundle can be uninstalled
 * @tc.desc: 1. the bundle is already installed
 *           2. the installed bundle can be uninstalled successfully
 */
HWTEST_F(BmsBundleUninstallerTest, Bundle_Uninstall_0100, Function | SmallTest | Level0)
{
    ErrCode installResult = InstallBundle(BUNDLE_FILE_PATH);
    ASSERT_EQ(installResult, ERR_OK);
    CheckBundleInfoExist();
    CheckFileExist();

    ErrCode uninstallResult = UninstallBundle(BUNDLE_NAME);
    ASSERT_EQ(uninstallResult, ERR_OK);
    CheckFileNonExist();
    CheckBundleInfoNonExist();
}

/**
 * @tc.number: Bundle_Uninstall_0200
 * @tc.name: test the empty bundle name will return fail
 * @tc.desc: 1. the bundle name is empty
 *           2. the empty bundle name will return fail
 */
HWTEST_F(BmsBundleUninstallerTest, Bundle_Uninstall_0200, Function | SmallTest | Level0)
{
    ErrCode result = UninstallBundle("");
    EXPECT_EQ(result, ERR_APPEXECFWK_UNINSTALL_INVALID_NAME);
}

/**
 * @tc.number: Bundle_Uninstall_0300
 * @tc.name: test the error bundle name will return fail
 * @tc.desc: 1. the bundle name is error
 *           2. the error bundle name will return fail
 */
HWTEST_F(BmsBundleUninstallerTest, Bundle_Uninstall_0300, Function | SmallTest | Level0)
{
    ErrCode result = UninstallBundle(ERROR_BUNDLE_NAME);
    EXPECT_EQ(result, ERR_APPEXECFWK_UNINSTALL_MISSING_INSTALLED_BUNDLE);
}

/**
 * @tc.number: Bundle_Uninstall_0400
 * @tc.name: test the bundle mgr service error case
 * @tc.desc: 1. the bundle mgr service error
 *           2. the uninstall operation will return fail and the next uninstall operation will success after restart
 *              bundle mgr service
 */
HWTEST_F(BmsBundleUninstallerTest, Bundle_Uninstall_0400, Function | SmallTest | Level0)
{
    ErrCode installResult = InstallBundle(BUNDLE_FILE_PATH);
    ASSERT_EQ(installResult, ERR_OK);
    CheckFileExist();
    CheckBundleInfoExist();

    StopBundleService();
    ErrCode secondResult = UninstallBundle(BUNDLE_NAME);
    ASSERT_EQ(secondResult, ERR_APPEXECFWK_UNINSTALL_BUNDLE_MGR_SERVICE_ERROR);
    CheckFileExist();
    StartBundleService();
    CheckBundleInfoExist();

    ErrCode thirdResult = UninstallBundle(BUNDLE_NAME);
    ASSERT_EQ(thirdResult, ERR_OK);
    CheckFileNonExist();
    CheckBundleInfoNonExist();
}

/**
 * @tc.number: Bundle_Uninstall_0500
 * @tc.name: test the installd service error
 * @tc.desc: 1. the installd service error
 *           2. the uninstall operation will return fail.
 */
HWTEST_F(BmsBundleUninstallerTest, Bundle_Uninstall_0500, Function | SmallTest | Level0)
{
    ErrCode installResult = InstallBundle(BUNDLE_FILE_PATH);
    ASSERT_EQ(installResult, ERR_OK);
    CheckFileExist();
    CheckBundleInfoExist();

    StopInstalldService();
    ErrCode uninstallResult = UninstallBundle(BUNDLE_NAME);
    ASSERT_EQ(uninstallResult, ERR_APPEXECFWK_INSTALLD_GET_PROXY_ERROR);
    CheckFileExist();

    StartInstalldService();
    CheckFileExist();
    DeleteInstallFiles();
}

/**
 * @tc.number: Bundle_Uninstall_0600
 * @tc.name: test when status receiver is null will not uninstall bundle
 * @tc.desc: 1. the status receiver is null
 *           2. the uninstall bundle operation will not execute when status receiver is null
 */
HWTEST_F(BmsBundleUninstallerTest, Bundle_Uninstall_0600, Function | SmallTest | Level1)
{
    ErrCode installResult = InstallBundle(BUNDLE_FILE_PATH);
    ASSERT_EQ(installResult, ERR_OK);
    CheckFileExist();
    CheckBundleInfoExist();

    InstallParam installParam;
    installParam.installFlag = InstallFlag::NORMAL;
    auto bms = GetBundleMgrService();
    ASSERT_NE(bms, nullptr);
    auto installer = bms->GetBundleInstaller();
    ASSERT_NE(installer, nullptr);
    bool result = installer->Uninstall(BUNDLE_NAME, installParam, nullptr);
    EXPECT_FALSE(result);

    std::this_thread::sleep_for(100ms);
    CheckFileExist();
    CheckBundleInfoExist();
    DeleteInstallFiles();
}

/**
 * @tc.number: Bundle_Uninstall_0700
 * @tc.name: test the installed system bundle can't be uninstalled
 * @tc.desc: 1. the system bundle is already installed
 *           2. the installed system bundle can't be uninstalled
 */
HWTEST_F(BmsBundleUninstallerTest, Bundle_Uninstall_0700, Function | SmallTest | Level0)
{
    auto installer = std::make_unique<SystemBundleInstaller>(BUNDLE_FILE_PATH);
    ASSERT_NE(installer, nullptr);
    bool installResult = installer->InstallSystemBundle(Constants::AppType::SYSTEM_APP);
    ASSERT_EQ(installResult, true);
    CheckFileExist();
    CheckBundleInfoExist();

    ErrCode uninstallResult = UninstallBundle(BUNDLE_NAME);
    ASSERT_EQ(uninstallResult, ERR_APPEXECFWK_UNINSTALL_SYSTEM_APP_ERROR);
    CheckFileExist();
    CheckBundleInfoExist();
    DeleteInstallFiles();
}

/**
 * @tc.number: Module_Uninstall_0100
 * @tc.name: test the installed Module can be uninstalled
 * @tc.desc: 1. the Module is already installed
 *           2. the installed Module can be uninstalled successfully
 */
HWTEST_F(BmsBundleUninstallerTest, Module_Uninstall_0100, Function | SmallTest | Level0)
{
    ErrCode installResult = InstallBundle(BUNDLE_FILE_PATH);
    ASSERT_EQ(installResult, ERR_OK);
    CheckBundleInfoExist();
    CheckFileExist();

    ErrCode uninstallResult = UninstallModule(BUNDLE_NAME, MODULE_PACKAGE);
    ASSERT_EQ(uninstallResult, ERR_OK);
    CheckFileNonExist();
    CheckBundleInfoNonExist();
}

/**
 * @tc.number: Module_Uninstall_0200
 * @tc.name: test the installed Module can be uninstalled
 * @tc.desc: 1. the Module name is empty
 *           2. the empty Module name will return fail
 */
HWTEST_F(BmsBundleUninstallerTest, Module_Uninstall_0200, Function | SmallTest | Level0)
{
    ErrCode result = UninstallModule(BUNDLE_NAME, "");
    EXPECT_EQ(result, ERR_APPEXECFWK_UNINSTALL_INVALID_NAME);
}

/**
 * @tc.number: Module_Uninstall_0300
 * @tc.name: test the error Module name will return fail.
 * @tc.desc: 1. the Module name is error.
 *           2. the error Module name will return fail.
 */
HWTEST_F(BmsBundleUninstallerTest, Module_Uninstall_0300, Function | SmallTest | Level0)
{
    ErrCode result = UninstallModule(BUNDLE_NAME, ERROR_MODULE_PACKAGE_NAME);
    EXPECT_EQ(result, ERR_APPEXECFWK_UNINSTALL_MISSING_INSTALLED_BUNDLE);
}

/**
 * @tc.number: Module_Uninstall_0400
 * @tc.name: test the bundle mgr service error case.
 * @tc.desc: 1. the bundle mgr service error.
 *           2. the uninstall operation will return fail and the next uninstall operation will success after
 *              restart bundle mgr service.
 */
HWTEST_F(BmsBundleUninstallerTest, Module_Uninstall_0400, Function | SmallTest | Level0)
{
    ErrCode installResult = InstallBundle(BUNDLE_FILE_PATH);
    ASSERT_EQ(installResult, ERR_OK);
    CheckFileExist();
    CheckBundleInfoExist();

    StopBundleService();
    ErrCode secondResult = UninstallModule(BUNDLE_NAME, MODULE_PACKAGE);
    ASSERT_EQ(secondResult, ERR_APPEXECFWK_UNINSTALL_BUNDLE_MGR_SERVICE_ERROR);
    CheckFileExist();
    StartBundleService();
    CheckBundleInfoExist();

    ErrCode thirdResult = UninstallModule(BUNDLE_NAME, MODULE_PACKAGE);
    ASSERT_EQ(thirdResult, ERR_OK);
    CheckFileNonExist();
    CheckBundleInfoNonExist();
}

/**
 * @tc.number: Module_Uninstall_0500
 * @tc.name: test the installd service error case.
 * @tc.desc: 1. the installd service error.
 *           2. the uninstall module operation will return fail.
 */
HWTEST_F(BmsBundleUninstallerTest, Module_Uninstall_0500, Function | SmallTest | Level0)
{
    ErrCode installResult = InstallBundle(BUNDLE_FILE_PATH);
    ASSERT_EQ(installResult, ERR_OK);
    CheckFileExist();
    CheckBundleInfoExist();

    StopInstalldService();
    ErrCode uninstallResult = UninstallModule(BUNDLE_NAME, MODULE_PACKAGE);
    ASSERT_EQ(uninstallResult, ERR_APPEXECFWK_INSTALLD_GET_PROXY_ERROR);
    CheckFileExist();

    StartInstalldService();
    CheckFileExist();
    DeleteInstallFiles();
}

/**
 * @tc.number: Module_Uninstall_0600
 * @tc.name: test when status receiver is null will not uninstall Module.
 * @tc.desc: 1. the status receiver is null.
 *           2. the uninstall Module operation will not execute when status receiver is null.
 */
HWTEST_F(BmsBundleUninstallerTest, Module_Uninstall_0600, Function | SmallTest | Level1)
{
    ErrCode installResult = InstallBundle(BUNDLE_FILE_PATH);
    ASSERT_EQ(installResult, ERR_OK);
    CheckFileExist();
    CheckBundleInfoExist();

    InstallParam installParam;
    installParam.installFlag = InstallFlag::NORMAL;
    auto bms = GetBundleMgrService();
    ASSERT_NE(bms, nullptr);
    auto installer = bms->GetBundleInstaller();
    ASSERT_NE(installer, nullptr);
    bool result = installer->Uninstall(BUNDLE_NAME, MODULE_PACKAGE, installParam, nullptr);
    EXPECT_FALSE(result);

    std::this_thread::sleep_for(100ms);
    CheckFileExist();
    CheckBundleInfoExist();
    DeleteInstallFiles();
}

/**
 * @tc.number: Module_Uninstall_0700
 * @tc.name: test the installed system bundle can't be uninstalled.
 * @tc.desc: 1. the system bundle is already installed.
 *           2. the installed system bundle can't be uninstalled.
 */
HWTEST_F(BmsBundleUninstallerTest, Module_Uninstall_0700, Function | SmallTest | Level0)
{
    auto installer = std::make_unique<SystemBundleInstaller>(BUNDLE_FILE_PATH);
    ASSERT_NE(installer, nullptr);
    bool installResult = installer->InstallSystemBundle(Constants::AppType::SYSTEM_APP);
    ASSERT_EQ(installResult, true);
    CheckFileExist();
    CheckBundleInfoExist();

    ErrCode uninstallResult = UninstallModule(BUNDLE_NAME, MODULE_PACKAGE);
    ASSERT_EQ(uninstallResult, ERR_APPEXECFWK_UNINSTALL_SYSTEM_APP_ERROR);
    CheckFileExist();
    CheckBundleInfoExist();
    DeleteInstallFiles();
}

/**
 * @tc.number: Module_Uninstall_0800
 * @tc.name: test the installed Module can be uninstalled.
 * @tc.desc: 1. the two Module is already installed.
 *           2. the installed one Module can be uninstalled successfully.
 */
HWTEST_F(BmsBundleUninstallerTest, Module_Uninstall_0800, Function | SmallTest | Level0)
{
    ErrCode installResult = InstallBundle(BUNDLE_FILE_PATH);
    ASSERT_EQ(installResult, ERR_OK);
    ErrCode installResult1 = InstallBundle(BUNDLE_FILE_PATH1);
    ASSERT_EQ(installResult1, ERR_OK);
    CheckBundleInfoExist();
    CheckFileExist();
    CheckModuleFileExist();
    CheckModuleFileExist1();

    ErrCode uninstallResult = UninstallModule(BUNDLE_NAME, MODULE_PACKAGE);
    ASSERT_EQ(uninstallResult, ERR_OK);
    CheckModuleFileNonExist();
    CheckModuleFileExist1();
    CheckBundleInfoExist();
    DeleteInstallFiles();
}

/**
 * @tc.number: Module_Uninstall_0900
 * @tc.name: test the installed Module can be uninstalled.
 * @tc.desc: 1. the two Module is already installed.
 *           2. the installed two Module can be uninstalled successfully.
 */
HWTEST_F(BmsBundleUninstallerTest, Module_Uninstall_0900, Function | SmallTest | Level0)
{
    ErrCode installResult = InstallBundle(BUNDLE_FILE_PATH);
    ASSERT_EQ(installResult, ERR_OK);
    ErrCode installResult1 = InstallBundle(BUNDLE_FILE_PATH1);
    ASSERT_EQ(installResult1, ERR_OK);
    CheckBundleInfoExist();
    CheckFileExist();
    CheckModuleFileExist();
    CheckModuleFileExist1();

    ErrCode uninstallResult = UninstallModule(BUNDLE_NAME, MODULE_PACKAGE);
    ASSERT_EQ(uninstallResult, ERR_OK);
    ErrCode uninstallResult1 = UninstallModule(BUNDLE_NAME, MODULE_PACKAGE1);
    ASSERT_EQ(uninstallResult1, ERR_OK);
    CheckFileNonExist();
    CheckBundleInfoNonExist();
    DeleteInstallFiles();
}