/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "include/updater/updater.h"
#include <cerrno>
#include <chrono>
#include <cstdio>
#include <iomanip>
#include <string>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include <vector>
#include "fs_manager/mount.h"
#include "log/log.h"
#include "package/pkg_manager.h"
#include "package/packages_info.h"
#include "progress_bar.h"
#include "text_label.h"
#include "updater/updater_const.h"
#include "updater_main.h"
#include "updater_ui.h"
#include "utils.h"

namespace updater {
using updater::utils::SplitString;
using updater::utils::Trim;
using namespace hpackage;

extern TextLable *g_updateInfoLabel;
extern ProgressBar *g_progressBar;
extern TextLable *g_updateStateLable;
extern int g_updateErrFlag;
int g_percentage;

static int GetTemprature()
{
    int temprature = FAKE_TEMPRATURE;
    return temprature;
}

static int32_t ExtractUpdaterBinary(PkgManager::PkgManagerPtr manager, const std::string &updaterBinary)
{
    PkgManager::StreamPtr outStream = nullptr;
    int32_t ret = manager->CreatePkgStream(outStream, G_WORK_PATH + updaterBinary, 0, PkgStream::PkgStreamType_Write);
    UPDATER_ERROR_CHECK(ret == PKG_SUCCESS, "ExtractUpdaterBinary create stream fail", return UPDATE_CORRUPT);
    ret = manager->ExtractFile(updaterBinary, outStream);
    manager->ClosePkgStream(outStream);
    return ret;
}

int GetUpdatePackageInfo(PkgManager::PkgManagerPtr pkgManager, const std::string &path)
{
    std::vector<std::string> components;
    if (pkgManager == nullptr) {
        LOG(ERROR) << "Fail to GetPackageInstance";
        return UPDATE_CORRUPT;
    }
    int32_t ret = pkgManager->LoadPackage(path, utils::GetCertName(), components);
    if (ret != PKG_SUCCESS) {
        LOG(INFO) << "LoadPackage fail ret :"<< ret;
        return ret;
    }
    return PKG_SUCCESS;
}

int UpdatePreProcess(PkgManager::PkgManagerPtr pkgManager, const std::string &path)
{
    int ret = -1;
    if (pkgManager == nullptr) {
        return PKG_INVALID_VERSION;
    }
    PackagesInfoPtr pkginfomanager = PackagesInfo::GetPackagesInfoInstance();
    UpgradePkgInfo* outPkgInfo = (UpgradePkgInfo*)pkgManager->GetPackageInfo(path);
    if (pkginfomanager == nullptr || outPkgInfo == nullptr) {
        LOG(ERROR) << "Fail to GetPackageInstance";
        return PKG_INVALID_VERSION;
    }
    std::vector<std::string> targetVersions = pkginfomanager->GetOTAVersion(pkgManager, "/version_list", G_WORK_PATH);
    for (size_t i = 0; i < targetVersions.size(); i++) {
        ret = outPkgInfo->softwareVersion.compare(targetVersions[i]);
        if (ret == 0) {
            break;
        }
    }
    // check broad info
    if (ret != 0) {
        PackagesInfo::ReleasePackagesInfoInstance(pkginfomanager);
        return ret;
    }
    LOG(WARNING) << "Check version success ";
    std::string localBoardId = utils::GetLocalBoardId();
    if (localBoardId.empty()) {
        PackagesInfo::ReleasePackagesInfoInstance(pkginfomanager);
        return 0;
    }
    std::vector<std::string> boardIdList = pkginfomanager->GetBoardID(pkgManager, "/board_list", "");
    for (size_t i = 0; i < boardIdList.size(); i++) {
        ret = localBoardId.compare(boardIdList[i]);
        if (ret == 0) {
            LOG(WARNING) << "Check board list success ";
            break;
        }
    }
    PackagesInfo::ReleasePackagesInfoInstance(pkginfomanager);
    return ret;
}

static UpdaterStatus IsSpaceCapacitySufficient(PkgManager::PkgManagerPtr pkgManager,
    const std::string &packagePath)
{
    UPDATER_ERROR_CHECK(pkgManager != nullptr, "pkgManager is nullptr", return UPDATE_CORRUPT);
    std::vector<std::string> fileIds;
    int ret = pkgManager->LoadPackageWithoutUnPack(packagePath, fileIds);
    UPDATER_ERROR_CHECK(ret == PKG_SUCCESS, "LoadPackageWithoutUnPack failed", return UPDATE_CORRUPT);

    const FileInfo *info = pkgManager->GetFileInfo("update.bin");
    UPDATER_ERROR_CHECK(info != nullptr, "update.bin is not exist", return UPDATE_CORRUPT);

    PkgManager::ReleasePackageInstance(pkgManager);

    struct statvfs64 updaterVfs;
    if (access("/sdcard/updater", 0) == 0) {
        struct statvfs64 dataVfs {};
        UPDATER_ERROR_CHECK(statvfs64("/data", &dataVfs) >= 0, "Statvfs read /data error!", return UPDATE_ERROR);
        UPDATER_ERROR_CHECK(dataVfs.f_bfree * dataVfs.f_bsize > MAX_LOG_SPACE, "/data free space is not enough",
            return UPDATE_ERROR);

        UPDATER_ERROR_CHECK(statvfs64("/sdcard", &updaterVfs) >= 0, "Statvfs read /sdcard error!", return UPDATE_ERROR);
        UPDATER_ERROR_CHECK(updaterVfs.f_bfree * updaterVfs.f_bsize > info->unpackedSize,
            "Can not update, free space is not enough",
            g_updateInfoLabel->SetText("Can't update, free space is not enough"); return UPDATE_ERROR);
    } else {
        UPDATER_ERROR_CHECK(statvfs64("/data", &updaterVfs) >= 0, "Statvfs read /data error!", return UPDATE_ERROR);
        UPDATER_ERROR_CHECK(updaterVfs.f_bfree * updaterVfs.f_bsize > (info->unpackedSize + MAX_LOG_SPACE),
            "Can not update, free space is not enough",
            g_updateInfoLabel->SetText("Can't update, free space is not enough"); return UPDATE_ERROR);
    }
    return UPDATE_SUCCESS;
}

UpdaterStatus DoInstallUpdaterPackage(PkgManager::PkgManagerPtr pkgManager, const std::string &packagePath,
    int retryCount)
{
    ShowUpdateFrame(true);

    UPDATER_ERROR_CHECK(pkgManager != nullptr, "Fail to GetPackageInstance", return UPDATE_CORRUPT);
    UPDATER_CHECK_ONLY_RETURN(SetupPartitions() == 0, g_updateInfoLabel->SetText("update failed\n");
        return UPDATE_ERROR);
    int beforeTemperature = GetTemprature();

    LOG(INFO) << "Verify package...";
    g_updateInfoLabel->SetText("Verify package...");

    UPDATER_ERROR_CHECK(access(packagePath.c_str(), 0) == 0, "package is not exist",
        g_updateInfoLabel->SetText("package is not exist"); return UPDATE_CORRUPT);
    if (retryCount > 0) {
        LOG(INFO) << "Retry for " << retryCount << " time(s)";
    } else {
        UpdaterStatus ret = IsSpaceCapacitySufficient(pkgManager, packagePath);
        // Only handle UPATE_ERROR and UPDATE_SUCCESS here.
        // If it returns UPDATE_CORRUPT, which means something wrong with package manager.
        // Let package verify handle this.
        if (ret == UPDATE_ERROR) {
            g_updateErrFlag = 1;
            return ret;
        } else if (ret == UPDATE_SUCCESS) {
            pkgManager = PkgManager::GetPackageInstance();
        }
    }

    int32_t verifyret = GetUpdatePackageInfo(pkgManager, packagePath);
    UPDATER_ERROR_CHECK(verifyret == PKG_SUCCESS, "Verify package Fail...",
        g_updateInfoLabel->SetText("Verify package Fail...\n"); return UPDATE_CORRUPT);
    LOG(INFO) << "Package verified. start to install package...";
    g_updateInfoLabel->SetText("Start to install package...");
    int32_t versionRet = UpdatePreProcess(pkgManager, packagePath);
    UPDATER_ERROR_CHECK(versionRet == PKG_SUCCESS, "Version Check Fail...", return UPDATE_CORRUPT);

    int maxTemperature;
    UpdaterStatus updateRet = StartUpdaterProc(pkgManager, packagePath, retryCount, maxTemperature);
    if (updateRet == UPDATE_SUCCESS) {
        g_progressBar->SetProgressValue(FULL_PERCENT_PROGRESS);
        g_updateStateLable->SetText("100%");
        g_updateInfoLabel->SetText("Update success, reboot now");
        std::this_thread::sleep_for(std::chrono::milliseconds(SHOW_FULL_PROGRESS_TIME));
        LOG(INFO)<< "update success , do reboot now";
    } else {
        g_updateErrFlag = 1;
        g_updateInfoLabel->SetText("Install failed.");
        LOG(ERROR) << "Install package failed.";
    }
    int afterTemprature = GetTemprature();
    LOG(INFO) << "Before install temprature: " << beforeTemperature << ",After install temprature: " << afterTemprature;
    return updateRet;
}

static void HandleChildOutput(const std::string &buffer, int32_t bufferLen,
    bool &retryUpdate)
{
    UPDATER_CHECK_ONLY_RETURN(bufferLen != 0, return);
    std::string str = buffer;
    std::vector<std::string> output = SplitString(str, ":");
    UPDATER_ERROR_CHECK(output.size() >= 1, "check output fail", return);
    auto outputHeader = Trim(output[0]);
    if (outputHeader == "ui_log") {
        UPDATER_ERROR_CHECK(output.size() >= DEFAULT_PROCESS_NUM, "check output fail", return);
        auto outputInfo = Trim(output[1]);
    } else if (outputHeader == "write_log") {
        UPDATER_ERROR_CHECK(output.size() >= DEFAULT_PROCESS_NUM, "check output fail", return);
        auto outputInfo = Trim(output[1]);
        LOG(INFO) << outputInfo;
    } else if (outputHeader == "retry_update") {
        retryUpdate = true;
    } else if (outputHeader == "show_progress") {
        UPDATER_ERROR_CHECK(output.size() >= DEFAULT_PROCESS_NUM, "check output fail", return);
        auto outputInfo = Trim(output[1]);
        float frac;
        std::vector<std::string> progress = SplitString(outputInfo, ",");
        if (progress.size() != DEFAULT_PROCESS_NUM) {
            LOG(ERROR) << "show progress with wrong arguments";
        } else {
            frac = std::stof(progress[0]);
            g_percentage = g_percentage + static_cast<int>(frac * FULL_PERCENT_PROGRESS);
            g_percentage = g_percentage / PROGRESS_VALUE_CONST;
            g_updateStateLable->SetText(std::to_string(g_percentage).append("%").c_str());
            g_progressBar->SetProgressValue(g_percentage);
        }
    } else if (outputHeader == "set_progress") {
        UPDATER_ERROR_CHECK(output.size() >= DEFAULT_PROCESS_NUM, "check output fail", return);
        auto outputInfo = Trim(output[1]);
        float frac = 0.0;
        frac = std::stof(output[1]);
        g_progressBar->SetProgressValue(FULL_PERCENT_PROGRESS);
    } else {
        LOG(WARNING) << "Child process returns unexpected message.";
    }
}

UpdaterStatus StartUpdaterProc(PkgManager::PkgManagerPtr pkgManager, const std::string &packagePath,
    int retryCount, int &maxTemperature)
{
    ShowUpdateFrame(true);

    int pfd[DEFAULT_PIPE_NUM]; /* communication between parent and child */
    UPDATER_FILE_CHECK(pipe(pfd) >= 0, "Create pipe failed: ", return UPDATE_ERROR);
    UPDATER_ERROR_CHECK(pkgManager != nullptr, "Fail to GetPackageInstance", return UPDATE_CORRUPT);
    int pipeRead = pfd[0];
    int pipeWrite = pfd[1];

    UPDATER_ERROR_CHECK(ExtractUpdaterBinary(pkgManager, UPDATER_BINARY) == 0,
        "Updater: cannot extract updater binary from update package.", return UPDATE_CORRUPT);
    pid_t pid = fork();
    UPDATER_CHECK_ONLY_RETURN(pid >= 0, ERROR_CODE(CODE_FORK_FAIL); return UPDATE_ERROR);
    if (pid == 0) { // child
        close(pipeRead);   // close read endpoint
        std::string fullPath = G_WORK_PATH + UPDATER_BINARY;
#ifdef UPDATER_UT
        if (packagePath.find("updater_binary_abnormal") != std::string::npos) {
            fullPath = "/system/bin/updater_binary_abnormal";
        } else {
            fullPath = "/system/bin/test_update_binary";
        }
#endif
        UPDATER_ERROR_CHECK_NOT_RETURN(chmod(fullPath.c_str(), utils::S_READ_WRITE_PERMISSION) == 0,
            "Failed to change mode");

        if (retryCount > 0) {
            execl(fullPath.c_str(), packagePath.c_str(), std::to_string(pipeWrite).c_str(), "retry", nullptr);
        } else {
            execl(fullPath.c_str(), packagePath.c_str(), std::to_string(pipeWrite).c_str(), nullptr);
        }
        LOG(INFO) << "Execute updater binary failed: " << errno;
        exit(-1);
    }

    close(pipeWrite); // close write endpoint
    char buffer[MAX_BUFFER_SIZE];
    bool retryUpdate = false;
    FILE* fromChild = fdopen(pipeRead, "r");
    g_progressBar->SetProgressValue(0);
    while (fgets(buffer, MAX_BUFFER_SIZE, fromChild) != nullptr) {
        size_t n = strlen(buffer);
        if (buffer[n - 1] == '\n') {
            buffer[n - 1] = '\0';
        }
        HandleChildOutput(buffer, MAX_BUFFER_SIZE,  retryUpdate);
    }
    fclose(fromChild);

    int status;
    waitpid(pid, &status, 0);
    UPDATER_CHECK_ONLY_RETURN(!retryUpdate, return UPDATE_RETRY);
    UPDATER_ERROR_CHECK(!(!WIFEXITED(status) || WEXITSTATUS(status) != 0),
        "Updater process exit with status: " << WEXITSTATUS(status), return UPDATE_ERROR);
    LOG(DEBUG) << "Updater process finished.";
    return UPDATE_SUCCESS;
}
} // namespace updater
