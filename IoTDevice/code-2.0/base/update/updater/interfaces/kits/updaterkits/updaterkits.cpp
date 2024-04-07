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
#include "updaterkits/updaterkits.h"
#include <string>
#include <sys/reboot.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include "misc_info/misc_info.h"
#include "parameters.h"
#include "securec.h"
#include "utils.h"

using namespace updater;

static bool WriteToMiscAndRebootToUpdater(const std::string &miscFile,
    const struct UpdateMessage &updateMsg)
{
    // Write package name to misc, then trigger reboot.
    const char *bootCmd = "boot_updater";
    int ret = strncpy_s(const_cast<char*>(updateMsg.command), sizeof(updateMsg.command), bootCmd,
        sizeof(updateMsg.command) - 1);
    if (ret != 0) {
        return false;
    }
#ifndef UPDATER_UT
    int32_t propertyMaxSize = 92;
    char updateCmd[propertyMaxSize];
    void(snprintf_s(updateCmd, propertyMaxSize, propertyMaxSize - 1, "reboot,updater:%s", updateMsg.update));
    bool bRet = OHOS::system::SetParameter("sys.powerctl", updateCmd);
    if (!bRet) {
        std::cout << "WriteToMiscAndRebootToUpdater SetParameter failed, errno: " << errno << std::endl;
        return false;
    }
    while (true) {
        pause();
    }
#else
    return true;
#endif
}

bool RebootAndInstallUpgradePackage(const std::string &miscFile, const std::string &packageName)
{
    if (packageName.empty() || miscFile.empty()) {
        std::cout << "updaterkits: invalid argument. one of arugments is empty\n";
        return false;
    }

    // Check if package readalbe.
    if (access(packageName.c_str(), R_OK) < 0) {
        std::cout << "updaterkits: " << packageName << " is not readable\n";
        return false;
    }

    struct UpdateMessage updateMsg {};
    if (snprintf_s(updateMsg.update, sizeof(updateMsg.update), sizeof(updateMsg.update) - 1, "--update_package=%s",
        packageName.c_str()) < 0) {
        std::cout << "updaterkits: copy updater message failed\n";
        return false;
    }

    WriteToMiscAndRebootToUpdater(miscFile, updateMsg);

    // Never get here.
    return true;
}

bool RebootAndCleanUserData(const std::string &miscFile, const std::string &cmd)
{
    if (miscFile.empty() || cmd.empty()) {
        std::cout << "updaterkits: invalid argument. one of arugments is empty\n";
        return false;
    }

    // Write package name to misc, then trigger reboot.
    struct UpdateMessage updateMsg {};
    if (strncpy_s(updateMsg.update, sizeof(updateMsg.update), cmd.c_str(), cmd.size()) != EOK) {
        std::cout << "updaterkits: copy updater message failed\n";
        return false;
    }

    WriteToMiscAndRebootToUpdater(miscFile, updateMsg);

    // Never get here.
    return true;
}

