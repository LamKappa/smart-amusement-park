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

#include <cstdio>
#include <gtest/gtest.h>
#include <sys/stat.h>
#include <unistd.h>

#include "directory_ex.h"
#include "installd/installd_service.h"
#include "installd_client.h"

using namespace testing::ext;
using namespace OHOS::AppExecFwk;
using namespace OHOS;

namespace {

const std::string BUNDLE_FILE = "/data/test/resource/bms/install_daemon/right.hap";
const std::string SYSTEM_DIR = "/sys/com.example.l3jsdemo";
const std::string TEMP_DIR = "/data/accounts/account_0/applications/com.example.l3jsdemo/temp";
const std::string MODULE_DIR = "/data/accounts/account_0/applications/com.example.l3jsdemo/com.example.l3jsdemo";
const std::string BUNDLE_DATA_DIR = "/data/accounts/account_0/appdata/com.example.l3jsdemo";
const std::string BUNDLE_CODE_DIR = "/data/accounts/account_0/applications/com.example.l3jsdemo";
const std::string ROOT_DIR = "/data/accounts";
const int32_t ROOT_UID = 0;
const int32_t UID = 1000;
const int32_t GID = 1000;

}  // namespace

class BmsInstallDaemonTest : public testing::Test {
public:
    BmsInstallDaemonTest();
    ~BmsInstallDaemonTest();
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    int CreateBundleDir(const std::string &bundleDir) const;
    int CreateBundleDataDir(const std::string &bundleDir, const int32_t uid, const int32_t gid) const;
    int RemoveBundleDir(const std::string &bundleDir) const;
    int RemoveBundleDataDir(const std::string &bundleDataDir) const;
    int CleanBundleDataDir(const std::string &bundleDataDir) const;
    int ExtractModuleFiles(const std::string &srcModulePath, const std::string &targetPath) const;
    int RenameModuleDir(const std::string &oldPath, const std::string &newPath) const;
    bool CheckBundleDirExist() const;
    bool CheckBundleDataDirExist() const;

private:
    std::shared_ptr<InstalldService> service_ = std::make_shared<InstalldService>();
};

BmsInstallDaemonTest::BmsInstallDaemonTest()
{}

BmsInstallDaemonTest::~BmsInstallDaemonTest()
{}

void BmsInstallDaemonTest::SetUpTestCase()
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

void BmsInstallDaemonTest::TearDownTestCase()
{}

void BmsInstallDaemonTest::SetUp()
{}

void BmsInstallDaemonTest::TearDown()
{
    // clear files.
    OHOS::ForceRemoveDirectory(BUNDLE_CODE_DIR);
    OHOS::ForceRemoveDirectory(BUNDLE_DATA_DIR);

    if (service_->IsServiceReady()) {
        service_->Stop();
        InstalldClient::GetInstance()->ResetInstalldProxy();
    }
}

int BmsInstallDaemonTest::CreateBundleDir(const std::string &bundleDir) const
{
    if (!service_->IsServiceReady()) {
        service_->Start();
    }
    return InstalldClient::GetInstance()->CreateBundleDir(bundleDir);
}

int BmsInstallDaemonTest::CreateBundleDataDir(
    const std::string &bundleDataDir, const int32_t uid, const int32_t gid) const
{
    if (!service_->IsServiceReady()) {
        service_->Start();
    }
    return InstalldClient::GetInstance()->CreateBundleDataDir(bundleDataDir, uid, gid);
}

int BmsInstallDaemonTest::RemoveBundleDir(const std::string &bundleDir) const
{
    if (!service_->IsServiceReady()) {
        service_->Start();
    }
    return InstalldClient::GetInstance()->RemoveBundleDir(bundleDir);
}

int BmsInstallDaemonTest::RemoveBundleDataDir(const std::string &bundleDataDir) const
{
    if (!service_->IsServiceReady()) {
        service_->Start();
    }
    return InstalldClient::GetInstance()->RemoveBundleDataDir(bundleDataDir);
}

int BmsInstallDaemonTest::CleanBundleDataDir(const std::string &bundleDataDir) const
{
    if (!service_->IsServiceReady()) {
        service_->Start();
    }
    return InstalldClient::GetInstance()->CleanBundleDataDir(bundleDataDir);
}

int BmsInstallDaemonTest::ExtractModuleFiles(const std::string &srcModulePath, const std::string &targetPath) const
{
    if (!service_->IsServiceReady()) {
        service_->Start();
    }
    return InstalldClient::GetInstance()->ExtractModuleFiles(srcModulePath, targetPath);
}

int BmsInstallDaemonTest::RenameModuleDir(const std::string &oldPath, const std::string &newPath) const
{
    if (!service_->IsServiceReady()) {
        service_->Start();
    }
    return InstalldClient::GetInstance()->RenameModuleDir(oldPath, newPath);
}

bool BmsInstallDaemonTest::CheckBundleDirExist() const
{
    int bundleCodeExist = access(BUNDLE_CODE_DIR.c_str(), F_OK);
    if (bundleCodeExist == 0) {
        return true;
    }
    return false;
}

bool BmsInstallDaemonTest::CheckBundleDataDirExist() const
{
    int bundleDataExist = access(BUNDLE_DATA_DIR.c_str(), F_OK);
    if (bundleDataExist == 0) {
        return true;
    }
    return false;
}

/**
* @tc.number: Startup_0100
* @tc.name: test the start function of the installd service when service is not ready
* @tc.desc: 1. the service is not initialized
*           2. the non initialized installd service can be started
*/
HWTEST_F(BmsInstallDaemonTest, Startup_0100, Function | SmallTest | Level0)
{
    std::shared_ptr<InstalldService> installdService = std::make_shared<InstalldService>();
    ASSERT_NE(installdService, nullptr);
    bool ready = installdService->IsServiceReady();
    ASSERT_EQ(false, ready);
    installdService->Start();
    ready = installdService->IsServiceReady();
    EXPECT_EQ(true, ready);
}

/**
* @tc.number: Startup_0200
* @tc.name: test the stop function of the installd service when service is ready
* @tc.desc: 1. the service is already initialized
*           2. the initialized installd service can be stopped
*/
HWTEST_F(BmsInstallDaemonTest, Startup_0200, Function | SmallTest | Level0)
{
    std::shared_ptr<InstalldService> installdService = std::make_shared<InstalldService>();
    ASSERT_NE(installdService, nullptr);
    installdService->Start();
    bool ready = installdService->IsServiceReady();
    ASSERT_EQ(true, ready);
    installdService->Stop();
    ready = installdService->IsServiceReady();
    EXPECT_EQ(false, ready);
}

/**
* @tc.number: Startup_0300
* @tc.name: test the restart function of the installd service
* @tc.desc: 1. the service is already initialized
*           2. the stopped installd service can be restarted
*/
HWTEST_F(BmsInstallDaemonTest, Startup_0300, Function | SmallTest | Level0)
{
    std::shared_ptr<InstalldService> installdService = std::make_shared<InstalldService>();
    ASSERT_NE(installdService, nullptr);
    installdService->Start();
    bool ready = installdService->IsServiceReady();
    ASSERT_EQ(true, ready);
    installdService->Stop();
    ready = installdService->IsServiceReady();
    ASSERT_EQ(false, ready);
    installdService->Start();
    ready = installdService->IsServiceReady();
    EXPECT_EQ(true, ready);
}

/**
* @tc.number: Startup_0400
* @tc.name: test the restart function of the installd service which is already initialized
* @tc.desc: 1. the service is already initialized
*           2. the recall start function will not affect the initialized installd service
*/
HWTEST_F(BmsInstallDaemonTest, Startup_0400, Function | SmallTest | Level0)
{
    std::shared_ptr<InstalldService> installdService = std::make_shared<InstalldService>();
    ASSERT_NE(installdService, nullptr);
    installdService->Start();
    bool ready = installdService->IsServiceReady();
    ASSERT_EQ(true, ready);
    installdService->Start();
    ready = installdService->IsServiceReady();
    EXPECT_EQ(true, ready);
}

/**
* @tc.number: Communication_0100
* @tc.name: test the communication of the installd service and installd client
* @tc.desc: 1. the service is already initialized
*           2. the installd client can send msg to the service and receive the right result
*/
HWTEST_F(BmsInstallDaemonTest, Communication_0100, Function | SmallTest | Level0)
{
    int result = CreateBundleDir(BUNDLE_CODE_DIR);
    EXPECT_EQ(result, 0);
}

/**
* @tc.number: Communication_0200
* @tc.name: test the communication of the installd service and installd client
* @tc.desc: 1. the service is not initialized
*           2. the installd client can't send msg to the service and receive the error result
*/
HWTEST_F(BmsInstallDaemonTest, Communication_0200, Function | SmallTest | Level0)
{
    std::shared_ptr<InstalldService> installdService = std::make_shared<InstalldService>();
    ASSERT_NE(installdService, nullptr);
    bool ready = installdService->IsServiceReady();
    ASSERT_EQ(false, ready);
    InstalldClient::GetInstance()->ResetInstalldProxy();
    int result = InstalldClient::GetInstance()->CreateBundleDir(BUNDLE_CODE_DIR);
    EXPECT_EQ(result, ERR_APPEXECFWK_INSTALLD_GET_PROXY_ERROR);
}

/**
* @tc.number: Communication_0300
* @tc.name: test the communication of the installd service and installd client
* @tc.desc: 1. the service is already initialized
*           2. the installd client can send msg to the service and receive the right result
*/
HWTEST_F(BmsInstallDaemonTest, Communication_0300, Function | SmallTest | Level0)
{
    int result = CreateBundleDataDir(BUNDLE_DATA_DIR, UID, GID);
    EXPECT_EQ(result, 0);
}

/**
* @tc.number: Communication_0400
* @tc.name: test the communication of the installd service and installd client
* @tc.desc: 1. the service is not initialized
*           2. the installd client can't send msg to the service and receive the error result
*/
HWTEST_F(BmsInstallDaemonTest, Communication_0400, Function | SmallTest | Level0)
{
    std::shared_ptr<InstalldService> installdService = std::make_shared<InstalldService>();
    ASSERT_NE(installdService, nullptr);
    bool ready = installdService->IsServiceReady();
    ASSERT_EQ(false, ready);
    InstalldClient::GetInstance()->ResetInstalldProxy();
    int result = InstalldClient::GetInstance()->CreateBundleDataDir(BUNDLE_DATA_DIR, UID, GID);
    EXPECT_EQ(result, ERR_APPEXECFWK_INSTALLD_GET_PROXY_ERROR);
}

/**
* @tc.number:BundleDir_0100
* @tc.name: test the create and remove bundle dir function of installd service
* @tc.desc: 1. the service is already initialized
*           2. the bundle dir of the right code dir can be created and removed
*/
HWTEST_F(BmsInstallDaemonTest, BundleDir_0100, Function | SmallTest | Level0)
{
    int result = CreateBundleDir(BUNDLE_CODE_DIR);
    ASSERT_EQ(result, 0);
    bool dirExist = CheckBundleDirExist();
    ASSERT_TRUE(dirExist);
    int result1 = RemoveBundleDir(BUNDLE_CODE_DIR);
    ASSERT_EQ(result1, 0);
    dirExist = CheckBundleDirExist();
    EXPECT_FALSE(dirExist);
}

/**
* @tc.number: BundleDir_0200
* @tc.name: test the create and remove bundle dir function of installd service
* @tc.desc: 1. the service is already initialized and the code dir is illegal
*           2. the bundle dir of the illegal code dir can't be created
*/
HWTEST_F(BmsInstallDaemonTest, BundleDir_0200, Function | SmallTest | Level0)
{
    int result = CreateBundleDir("");
    ASSERT_EQ(result, ERR_APPEXECFWK_INSTALLD_PARAM_ERROR);
    bool dirExist = CheckBundleDirExist();
    EXPECT_FALSE(dirExist);
}

/**
* @tc.number: BundleDir_0300
* @tc.name: test the create and remove bundle dir function of installd service
* @tc.desc: 1. the service is already initialized and the code dir is system dir
*           2. the bundle dir of the system dir can't be created
*/
HWTEST_F(BmsInstallDaemonTest, BundleDir_0300, Function | SmallTest | Level0)
{
    int result1 = CreateBundleDir(SYSTEM_DIR);
    ASSERT_EQ(result1, ERR_APPEXECFWK_INSTALLD_CREATE_DIR_FAILED);
    bool dirExist = CheckBundleDirExist();
    EXPECT_FALSE(dirExist);
}

/**
* @tc.number: BundleDataDir_0100
* @tc.name: test the create and remove bundle data dir function of installd service
* @tc.desc: 1. the service is already initialized and the code dir is system dir
*           2. the bundle data dir of the right code dir can be created and removed
*/
HWTEST_F(BmsInstallDaemonTest, BundleDataDir_0100, Function | SmallTest | Level0)
{
    int result = CreateBundleDataDir(BUNDLE_DATA_DIR, UID, GID);
    ASSERT_EQ(result, 0);
    bool dirExist = CheckBundleDataDirExist();
    ASSERT_TRUE(dirExist);
    int result1 = RemoveBundleDataDir(BUNDLE_DATA_DIR);
    ASSERT_EQ(result1, 0);
    dirExist = CheckBundleDataDirExist();
    EXPECT_FALSE(dirExist);
}

/**
* @tc.number: BundleDataDir_0200
* @tc.name: test the create and clean bundle data dir function of installd service
* @tc.desc: 1. the service is already initialized 
*           2. the bundle data dir of the right code dir can be created and clean
*/
HWTEST_F(BmsInstallDaemonTest, BundleDataDir_0200, Function | SmallTest | Level0)
{
    int result = CreateBundleDataDir(BUNDLE_DATA_DIR, UID, GID);
    ASSERT_EQ(result, 0);
    bool dirExist = CheckBundleDataDirExist();
    ASSERT_TRUE(dirExist);
    int result1 = CleanBundleDataDir(BUNDLE_DATA_DIR);
    ASSERT_EQ(result1, 0);
    dirExist = CheckBundleDataDirExist();
    EXPECT_TRUE(dirExist);
}

/**
* @tc.number: BundleDataDir_0300
* @tc.name: test the create and remove bundle data dir function of installd service
* @tc.desc: 1. the service is already initialized and the code dir is illegal
*           2. the bundle data dir of the illegal code dir can't be created
*/
HWTEST_F(BmsInstallDaemonTest, BundleDataDir_0300, Function | SmallTest | Level0)
{
    int result = CreateBundleDataDir("", UID, GID);
    ASSERT_EQ(result, ERR_APPEXECFWK_INSTALLD_PARAM_ERROR);
    bool dirExist = CheckBundleDataDirExist();
    EXPECT_FALSE(dirExist);
}

/**
* @tc.number: BundleDataDir_0400
* @tc.name: test the create and remove bundle data dir function of installd service
* @tc.desc: 1. the service is already initialized and the uid is illegal
*           2. the bundle data dir of the illegal uid can't be created
*/
HWTEST_F(BmsInstallDaemonTest, BundleDataDir_0400, Function | SmallTest | Level0)
{
    int result = CreateBundleDataDir(BUNDLE_DATA_DIR, -1, GID);
    ASSERT_EQ(result, ERR_APPEXECFWK_INSTALLD_PARAM_ERROR);
    bool dirExist = CheckBundleDataDirExist();
    EXPECT_FALSE(dirExist);
}

/**
* @tc.number: BundleDataDir_0500
* @tc.name: test the create and remove bundle data dir function of installd service
* @tc.desc: 1. the service is already initialized and the gid is illegal
*           2. the bundle data dir of the illegal gid can't be created
*/
HWTEST_F(BmsInstallDaemonTest, BundleDataDir_0500, Function | SmallTest | Level0)
{
    int result = CreateBundleDataDir(BUNDLE_DATA_DIR, UID, -1);
    ASSERT_EQ(result, ERR_APPEXECFWK_INSTALLD_PARAM_ERROR);
    bool dirExist = CheckBundleDataDirExist();
    EXPECT_FALSE(dirExist);
}

/**
* @tc.number: BundleDataDir_0500
* @tc.name: test the create and remove bundle data dir function of installd service
* @tc.desc: 1. the service is already initialized and the code dir is system dir
*           2. the bundle data dir of the system dir can't be created
*/
HWTEST_F(BmsInstallDaemonTest, BundleDataDir_0600, Function | SmallTest | Level0)
{
    int result = CreateBundleDataDir(SYSTEM_DIR, UID, GID);
    ASSERT_EQ(result, ERR_APPEXECFWK_INSTALLD_CREATE_DIR_FAILED);
    bool dirExist = CheckBundleDataDirExist();
    EXPECT_FALSE(dirExist);
}

/**
* @tc.number: ExtractBundleFile_0100
* @tc.name: test the ExtractBundleFile function of installd service with flag system bundle
* @tc.desc: 1. the bundle file is available and the target dir exists
*           2. the origin file exists and the extracted file exists
*/
HWTEST_F(BmsInstallDaemonTest, ExtractBundleFile_0100, Function | SmallTest | Level0)
{
    CreateBundleDir(BUNDLE_CODE_DIR);
    bool dirExist = CheckBundleDirExist();
    ASSERT_TRUE(dirExist);
    auto bundleFile = BUNDLE_FILE;
    int result = ExtractModuleFiles(bundleFile, TEMP_DIR);
    ASSERT_EQ(result, 0);
    int result1 = RenameModuleDir(TEMP_DIR, MODULE_DIR);
    EXPECT_EQ(result1, 0);
}

/**
* @tc.number: ExtractBundleFile_0200
* @tc.name: test the ExtractBundleFile function of installd service 
* @tc.desc: 1. the bundle file is illegal
*           2. the bundle file can't be extracted and the extracted file does not exists
*/
HWTEST_F(BmsInstallDaemonTest, ExtractBundleFile_0200, Function | SmallTest | Level0)
{
    CreateBundleDir(BUNDLE_CODE_DIR);
    bool dirExist = CheckBundleDirExist();
    ASSERT_TRUE(dirExist);
    int result = ExtractModuleFiles("", TEMP_DIR);
    EXPECT_EQ(result, ERR_APPEXECFWK_INSTALLD_PARAM_ERROR);
}

/**
* @tc.number: ExtractBundleFile_0300
* @tc.name: test the ExtractBundleFile function of installd service 
* @tc.desc: 1. the temp dir does not exist
*           2. the bundle file can't be extracted and the extracted file does not exists
*/
HWTEST_F(BmsInstallDaemonTest, ExtractBundleFile_0300, Function | SmallTest | Level0)
{
    CreateBundleDir(BUNDLE_CODE_DIR);
    bool dirExist = CheckBundleDirExist();
    ASSERT_TRUE(dirExist);
    auto bundleFile = BUNDLE_FILE;
    int result = ExtractModuleFiles(bundleFile, "");
    EXPECT_EQ(result, ERR_APPEXECFWK_INSTALLD_PARAM_ERROR);
}

/**
* @tc.number: ExtractBundleFile_0400
* @tc.name: test the ExtractBundleFile function of installd service 
* @tc.desc: 1. the old path does not exist
*           2. the bundle file can't be extracted and the extracted file does not exists
*/
HWTEST_F(BmsInstallDaemonTest, ExtractBundleFile_0400, Function | SmallTest | Level0)
{
    CreateBundleDir(BUNDLE_CODE_DIR);
    bool dirExist = CheckBundleDirExist();
    ASSERT_TRUE(dirExist);
    auto bundleFile = BUNDLE_FILE;
    int result = ExtractModuleFiles(bundleFile, TEMP_DIR);
    ASSERT_EQ(result, 0);
    int result1 = RenameModuleDir("", MODULE_DIR);
    EXPECT_EQ(result1, ERR_APPEXECFWK_INSTALLD_PARAM_ERROR);
}

/**
* @tc.number: ExtractBundleFile_0500
* @tc.name: test the ExtractBundleFile function of installd service 
* @tc.desc: 1. the new path does not exist
*           2. the bundle file can't be extracted and the extracted file does not exists
*/
HWTEST_F(BmsInstallDaemonTest, ExtractBundleFile_0500, Function | SmallTest | Level0)
{
    CreateBundleDir(BUNDLE_CODE_DIR);
    bool dirExist = CheckBundleDirExist();
    ASSERT_TRUE(dirExist);
    auto bundleFile = BUNDLE_FILE;
    int result = ExtractModuleFiles(bundleFile, TEMP_DIR);
    ASSERT_EQ(result, 0);
    int result1 = RenameModuleDir(TEMP_DIR, "");
    EXPECT_EQ(result1, ERR_APPEXECFWK_INSTALLD_PARAM_ERROR);
}
