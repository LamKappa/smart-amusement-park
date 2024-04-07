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
#include "updater_main.h"
#include <chrono>
#include <dirent.h>
#include <fcntl.h>
#include <getopt.h>
#include <libgen.h>
#include <string>
#include <sys/reboot.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/syscall.h>
#include <thread>
#include <unistd.h>
#include <vector>
#include "applypatch/partition_record.h"
#include "fs_manager/mount.h"
#include "include/updater/updater.h"
#include "log/log.h"
#include "misc_info/misc_info.h"
#include "package/pkg_manager.h"
#include "pkg_manager.h"
#include "pkg_utils.h"
#include "securec.h"
#include "ui/frame.h"
#include "ui/text_label.h"
#include "ui/updater_ui.h"
#include "updater/updater_const.h"
#include "utils.h"

namespace updater {
using utils::String2Int;
using namespace hpackage;
using namespace updater::utils;

extern TextLable *g_logLabel;
extern TextLable* g_logResultLabel;
extern TextLable* g_updateInfoLabel;
extern int g_updateErrFlag;

constexpr struct option OPTIONS[] = {
    { "update_package", required_argument, nullptr, 0 },
    { "retry_count", required_argument, nullptr, 0 },
    { "factory_wipe_data", no_argument, nullptr, 0 },
    { "user_wipe_data", no_argument, nullptr, 0 },
    { nullptr, 0, nullptr, 0 },
};

static void SetRetryCountToMisc(int retryCount, const std::vector<std::string> args)
{
    struct UpdateMessage msg {};
    char buffer[20];
    UPDATER_ERROR_CHECK(!strncpy_s(msg.command, sizeof(msg.command), "boot_updater", sizeof(msg.command) - 1),
        "SetRetryCountToMisc strncpy_s failed", return);
    for (const auto& arg : args) {
        if (arg.find("--retry_count") == std::string::npos) {
            UPDATER_ERROR_CHECK(!strncat_s(msg.update, sizeof(msg.update), arg.c_str(), sizeof(msg.update) - 1),
                "SetRetryCountToMisc strncat_s failed", return);
            UPDATER_ERROR_CHECK(!strncat_s(msg.update, sizeof(msg.update), "\n", sizeof(msg.update)),
                "SetRetryCountToMisc strncat_s failed", return);
        }
    }
    UPDATER_ERROR_CHECK(snprintf_s(buffer, sizeof(buffer), sizeof(buffer), "--retry_count=%d", retryCount) != -1,
        "SetRetryCountToMisc snprintf_s failed", return);
    UPDATER_ERROR_CHECK(!strncat_s(msg.update, sizeof(msg.update), buffer, sizeof(msg.update)),
        "SetRetryCountToMisc strncat_s failed", return);
    UPDATER_ERROR_CHECK_NOT_RETURN(WriteUpdaterMessage(MISC_FILE, msg) == true, "Write command to misc failed.");
}

static int DoFactoryReset(FactoryResetMode mode, const std::string &path)
{
    if (mode == USER_WIPE_DATA) {
        STAGE(UPDATE_STAGE_BEGIN) << "User FactoryReset";
        LOG(INFO) << "Begin erasing /data";
        if (FormatPartition(path) != 0) {
            LOG(ERROR) << "User level FactoryReset failed";
            STAGE(UPDATE_STAGE_FAIL) << "User FactoryReset";
            ERROR_CODE(CODE_FACTORY_RESET_FAIL);
            return 1;
        }
        LOG(INFO) << "User level FactoryReset success";
        STAGE(UPDATE_STAGE_SUCCESS) << "User FactoryReset";
    }
    return 0;
}

int FactoryReset(FactoryResetMode mode, const std::string &path)
{
    return DoFactoryReset(mode, path);
}

UpdaterStatus UpdaterFromSdcard()
{
#ifndef UPDATER_UT
    UPDATER_WARING_CHECK(MountForPath("/sdcard") == 0, "MountForPath /sdcard failed!", return UPDATE_ERROR);
#endif
    UPDATER_ERROR_CHECK(access(SDCARD_CARD_PKG_PATH.c_str(), 0) == 0, "package is not exist",
        g_logLabel->SetText("package is not exist!");
        return UPDATE_CORRUPT);
    PkgManager::PkgManagerPtr pkgManager = PkgManager::GetPackageInstance();
    UPDATER_ERROR_CHECK(pkgManager != nullptr, "pkgManager is nullptr", return UPDATE_CORRUPT);

    STAGE(UPDATE_STAGE_BEGIN) << "UpdaterFromSdcard";
    g_logLabel->SetText("Update start, please do not remove SD card!");
    LOG(INFO) << "UpdaterFromSdcard start, sdcard updaterPath : " << SDCARD_CARD_PKG_PATH;

    UpdaterStatus updateRet = DoInstallUpdaterPackage(pkgManager, SDCARD_CARD_PKG_PATH.c_str(), 0);
    return updateRet;
}

bool IsBatteryCapacitySufficient()
{
    return true;
}

static UpdaterStatus InstallUpdaterPackage(UpdaterParams &upParams, const std::vector<std::string> &args,
    PkgManager::PkgManagerPtr manager)
{
    UpdaterStatus status = UPDATE_NORMAL;
    if (IsBatteryCapacitySufficient() == false) {
        g_logLabel->SetText("Battery is low.\n");
        LOG(ERROR) << "Battery is not sufficient for install package.";
        status = UPDATE_SKIP;
    } else {
        STAGE(UPDATE_STAGE_BEGIN) << "Install package";
        if (upParams.retryCount == 0) {
            // First time enter updater, record retryCount in case of abnormal reset.
            UPDATER_ERROR_CHECK(PartitionRecord::GetInstance().ClearRecordPartitionOffset() == true,
                "ClearRecordPartitionOffset failed", return UPDATE_ERROR);
            SetRetryCountToMisc(upParams.retryCount + 1, args);
        }

        ShowUpdateFrame(true);
        status = DoInstallUpdaterPackage(manager, upParams.updatePackage, upParams.retryCount);
        if (status != UPDATE_SUCCESS) {
            ShowUpdateFrame(false);
            g_logLabel->SetText("update failed!");
            g_updateInfoLabel->SetText("update failed!");
            STAGE(UPDATE_STAGE_FAIL) << "Install failed";
            if (status == UPDATE_RETRY && upParams.retryCount < MAX_RETRY_COUNT) {
                upParams.retryCount += 1;
                g_logLabel->SetText("Retry installation");
                SetRetryCountToMisc(upParams.retryCount, args);
                utils::DoReboot("updater");
            }
        } else {
            LOG(INFO) << "Install package success.";
            STAGE(UPDATE_STAGE_SUCCESS) << "Install package";
            PostUpdater();
            utils::DoReboot("");
        }
    }
    ShowUpdateFrame(false);
    return status;
}

static UpdaterStatus StartUpdaterEntry(PkgManager::PkgManagerPtr manager,
    const std::vector<std::string> &args, UpdaterParams &upParams)
{
    UpdaterStatus status = UPDATE_SUCCESS;
    if (upParams.updatePackage != "") {
        ShowUpdateFrame(true);
        status = InstallUpdaterPackage(upParams, args, manager);
        UPDATER_CHECK_ONLY_RETURN(status == UPDATE_SUCCESS, return status);
    } else if (upParams.factoryWipeData) {
        LOG(INFO) << "Factory level FactoryReset begin";
        status = UPDATE_SUCCESS;
        ShowUpdateFrame(true);
        DoProgress();
        UPDATER_ERROR_CHECK(FactoryReset(FACTORY_WIPE_DATA, "/data") == 0, "FactoryReset factory level failed",
            status = UPDATE_ERROR);

        ShowUpdateFrame(false);
        if (status != UPDATE_SUCCESS) {
            g_logResultLabel->SetText("Factory reset failed");
        } else {
            g_logResultLabel->SetText("Factory reset done");
        }
    } else if (upParams.userWipeData) {
        LOG(INFO) << "User level FactoryReset begin";
        ShowUpdateFrame(true);
        DoProgress();
        UPDATER_ERROR_CHECK(FactoryReset(USER_WIPE_DATA, "/data") == 0, "FactoryReset user level failed",
            status = UPDATE_ERROR);
        ShowUpdateFrame(false);
        if (status != UPDATE_SUCCESS) {
            g_logResultLabel->SetText("Wipe data failed");
        } else {
            g_logResultLabel->SetText("Wipe data finished");
            PostUpdater();
            std::this_thread::sleep_for(std::chrono::milliseconds(UI_SHOW_DURATION));
            utils::DoReboot("");
        }
    }
    return status;
}

static UpdaterStatus StartUpdater(PkgManager::PkgManagerPtr manager, const std::vector<std::string> &args,
    char **argv)
{
    UpdaterParams upParams {
        false, false, 0, ""
    };
    std::vector<char *> extractedArgs;
    int rc;
    int optionIndex;

    for (const auto &arg : args) {
        extractedArgs.push_back(const_cast<char *>(arg.c_str()));
    }
    extractedArgs.push_back(nullptr);
    extractedArgs.insert(extractedArgs.begin(), argv[0]);
    while ((rc = getopt_long(extractedArgs.size() - 1, extractedArgs.data(), "", OPTIONS, &optionIndex)) != -1) {
        switch (rc) {
            case 0: {
                std::string option = OPTIONS[optionIndex].name;
                if (option == "update_package") {
                    upParams.updatePackage = optarg;
                } else if (option == "retry_count") {
                    upParams.retryCount = atoi(optarg);
                } else if (option == "factory_wipe_data") {
                    upParams.factoryWipeData = true;
                } else if (option == "user_wipe_data") {
                    upParams.userWipeData = true;
                }
                break;
            }
            case '?':
                LOG(ERROR) << "Invalid argument.";
                break;
            default:
                LOG(ERROR) << "Invalid argument.";
                break;
        }
    }
    optind = 1;
    // Sanity checks
    UPDATER_WARING_CHECK((upParams.factoryWipeData && upParams.userWipeData) == false,
        "Factory level reset and user level reset both set. use user level reset.", upParams.factoryWipeData = false);

    return StartUpdaterEntry(manager, args, upParams);
}

void CompressLogs(const std::string &name)
{
    PkgManager::PkgManagerPtr pkgManager = PkgManager::GetPackageInstance();
    UPDATER_ERROR_CHECK(pkgManager != nullptr, "pkgManager is nullptr", return);
    std::vector<std::pair<std::string, ZipFileInfo>> files;
    // Build the zip file to be packaged
    std::vector<std::string> testFileNames;
    std::string realName = GetName(name.c_str());
    testFileNames.push_back(realName);
    for (auto name : testFileNames) {
        ZipFileInfo file;
        file.fileInfo.identity = name;
        file.fileInfo.packMethod = PKG_COMPRESS_METHOD_ZIP;
        file.fileInfo.digestMethod = PKG_DIGEST_TYPE_CRC;
        std::string fileName = "/data/updater/log/" + name;
        files.push_back(std::pair<std::string, ZipFileInfo>(fileName, file));
    }

    PkgInfo pkgInfo;
    pkgInfo.signMethod = PKG_SIGN_METHOD_RSA;
    pkgInfo.digestMethod = PKG_DIGEST_TYPE_SHA256;
    pkgInfo.pkgType = PKG_PACK_TYPE_ZIP;

    char realTime[MAX_TIME_SIZE];
    auto sysTime = std::chrono::system_clock::now();
    auto currentTime = std::chrono::system_clock::to_time_t(sysTime);
    std::strftime(realTime, sizeof(realTime), "%H_%M_%S", std::localtime(&currentTime));
    char pkgName[MAX_LOG_NAME_SIZE];
    UPDATER_CHECK_ONLY_RETURN(snprintf_s(pkgName, MAX_LOG_NAME_SIZE, MAX_LOG_NAME_SIZE - 1,
        "/data/updater/log/%s_%s.zip", realName.c_str(), realTime) != -1, return);
    int32_t ret = pkgManager->CreatePackage(pkgName, utils::GetCertName(), &pkgInfo, files);
    UPDATER_CHECK_ONLY_RETURN(ret != 0, return);
    UPDATER_CHECK_ONLY_RETURN(utils::DeleteFile(name) == 0, return);
}

bool CopyUpdaterLogs(const std::string &sLog, const std::string &dLog)
{
    UPDATER_WARING_CHECK(MountForPath(UPDATER_LOG_DIR) == 0, "MountForPath /data/log failed!", return false);
    if (access(UPDATER_LOG_DIR.c_str(), 0) != 0) {
        UPDATER_ERROR_CHECK(!utils::MkdirRecursive(UPDATER_LOG_DIR, S_READ_WRITE_PERMISSION),
                            "MkdirRecursive error!", return false);
    }

    FILE* dFp = fopen(dLog.c_str(), "ab+");
    UPDATER_ERROR_CHECK(dFp != nullptr, "open log failed", return false);

    FILE* sFp = fopen(sLog.c_str(), "r");
    UPDATER_ERROR_CHECK(sFp != nullptr, "open log failed", fclose(dFp); return false);

    char buf[MAX_LOG_BUF_SIZE];
    size_t bytes;
    while ((bytes = fread(buf, 1, sizeof(buf), sFp)) != 0) {
        fwrite(buf, 1, bytes, dFp);
    }
    fseek(dFp, 0, SEEK_END);
    UPDATER_INFO_CHECK(ftell(dFp) < MAX_LOG_SIZE, "log size greater than 5M!", CompressLogs(dLog));
    fclose(sFp);
    fclose(dFp);
    sync();
    return true;
}

static bool DeleteUpdaterPath(const std::string &path)
{
    auto pDir = std::unique_ptr<DIR, decltype(&closedir)>(opendir(path.c_str()), closedir);
    UPDATER_INFO_CHECK_NOT_RETURN(pDir != nullptr, "Can not open dir");

    struct dirent *dp = nullptr;
    if (pDir != nullptr) {
        while ((dp = readdir(pDir.get())) != nullptr) {
            std::string currentName(dp->d_name);
            if (currentName[0] != '.' && (currentName.compare("log") != 0)) {
                std::string tmpName(path);
                tmpName.append("/" + currentName);
                if (tmpName.find(".") == std::string::npos) {
                    DeleteUpdaterPath(tmpName);
                }
#ifndef UPDATER_UT
                remove(tmpName.c_str());
#endif
            }
        }
    }
    return true;
}

static bool ClearMisc()
{
    struct UpdateMessage cleanBoot {};
    UPDATER_ERROR_CHECK(WriteUpdaterMessage(MISC_FILE, cleanBoot) == true,
        "ClearMisc clear boot message to misc failed", return false);
    auto fp = std::unique_ptr<FILE, decltype(&fclose)>(fopen(MISC_FILE.c_str(), "rb+"), fclose);
    UPDATER_FILE_CHECK(fp != nullptr, "WriteVersionCode fopen failed", return false);
    fseek(fp.get(), MISC_PARTITION_RECORD_OFFSET, SEEK_SET);
    off_t clearOffset = 0;
    UPDATER_FILE_CHECK(fwrite(&clearOffset, sizeof(off_t), 1, fp.get()) == 1,
        "ClearMisc write misc initOffset 0 failed", return false);

    struct PartitionRecordInfo cleanPartition {};
    for (size_t tmpOffset = 0; tmpOffset < PARTITION_UPDATER_RECORD_MSG_SIZE; tmpOffset +=
        sizeof(PartitionRecordInfo)) {
        fseek(fp.get(), PARTITION_RECORD_START + tmpOffset, SEEK_SET);
        UPDATER_FILE_CHECK(fwrite(&cleanPartition, sizeof(PartitionRecordInfo), 1, fp.get()) == 1,
            "ClearMisc write misc cleanPartition failed", return false);
    }
    return true;
}

void PostUpdater()
{
    STAGE(UPDATE_STAGE_BEGIN) << "PostUpdater";
    // clear update misc partition.
    UPDATER_ERROR_CHECK_NOT_RETURN(ClearMisc() == true, "PostUpdater clear misc failed");
    if (!access(COMMAND_FILE.c_str(), 0)) {
        UPDATER_ERROR_CHECK_NOT_RETURN(unlink(COMMAND_FILE.c_str()) == 0, "Delete command failed");
    }

    // delete updater tmp files
    if (access(UPDATER_PATH.c_str(), 0) == 0 && access(SDCARD_CARD_PATH.c_str(), 0) != 0) {
        UPDATER_ERROR_CHECK_NOT_RETURN(DeleteUpdaterPath(UPDATER_PATH), "DeleteUpdaterPath failed");
    }
    if (!access(SDCARD_CARD_PATH.c_str(), 0)) {
        UPDATER_ERROR_CHECK_NOT_RETURN(DeleteUpdaterPath(SDCARD_CARD_PATH), "Delete sdcard path failed");
    }
    // save logs
    UPDATER_ERROR_CHECK_NOT_RETURN(CopyUpdaterLogs(TMP_LOG, UPDATER_LOG) == true, "Copy updater log failed!");
    UPDATER_ERROR_CHECK_NOT_RETURN(CopyUpdaterLogs(TMP_ERROR_CODE_PATH, ERROR_CODE_PATH) == true,
        "Copy error code log failed!");
    chmod(UPDATER_LOG.c_str(), S_ONLY_READ_PERMISSION);
    chmod(UPDATER_STAGE_LOG.c_str(), S_ONLY_READ_PERMISSION);
    chmod(ERROR_CODE_PATH.c_str(), S_ONLY_READ_PERMISSION);
    STAGE(UPDATE_STAGE_SUCCESS) << "PostUpdater";
    UPDATER_ERROR_CHECK_NOT_RETURN(CopyUpdaterLogs(TMP_STAGE_LOG, UPDATER_STAGE_LOG) == true, "Copy stage log failed!");
}

std::vector<std::string> ParseParams(int argc, char **argv)
{
    struct UpdateMessage boot {};
    // read from misc
    UPDATER_ERROR_CHECK_NOT_RETURN(ReadUpdaterMessage(MISC_FILE, boot) == true,
        "ReadUpdaterMessage MISC_FILE failed!");
    // if boot.update is empty, read from command.The Misc partition may have dirty data,
    // so strlen(boot.update) is not used, which can cause system exceptions.
    if (boot.update[0] == '\0' && !access(COMMAND_FILE.c_str(), 0)) {
        UPDATER_ERROR_CHECK_NOT_RETURN(ReadUpdaterMessage(COMMAND_FILE, boot) == true,
                                       "ReadUpdaterMessage COMMAND_FILE failed!");
    }
    STAGE(UPDATE_STAGE_OUT) << "Init Params: " << boot.update;
    std::vector<std::string> parseParams(argv, argv + argc);
    boot.update[sizeof(boot.update) - 1] = '\0';
    parseParams = utils::SplitString(boot.update, "\n");
    return parseParams;
}

int UpdaterMain(int argc, char **argv)
{
    UpdaterStatus status = UPDATE_SUCCESS;
    PkgManager::PkgManagerPtr manager = PkgManager::GetPackageInstance();
    InitUpdaterLogger("UPDATER", TMP_LOG, TMP_STAGE_LOG, TMP_ERROR_CODE_PATH);
    SetLogLevel(INFO);
    LoadFstab();
    std::vector<std::string> args = ParseParams(argc, argv);

    LOG(INFO) << "Ready to start";
    HosInit();
    status = StartUpdater(manager, args, argv);
#ifndef UPDATER_UT
    if (status != UPDATE_SUCCESS && status != UPDATE_SKIP) {
       ShowUpdateFrame(false);
    }
#endif
    PostUpdater();
    PkgManager::ReleasePackageInstance(manager);
    // Wait for user input
    while (true) {
        pause();
    }
    return 0;
}
} // updater
