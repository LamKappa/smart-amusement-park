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

#include <cstdio>
#include <string>
#include <unistd.h>
#include <vector>
#include "fs_manager/mount.h"
#include "log.h"
#include "updater/updater_const.h"
#include "update_processor.h"
#include "utils.h"

using namespace updater;
int main(int argc, char **argv)
{
    InitUpdaterLogger("UPDATER_BINARY", TMP_LOG, TMP_STAGE_LOG, TMP_ERROR_CODE_PATH);
    if (argc < MINIMAL_ARGC_LIMIT) {
        LOG(ERROR) << "Invalid arguments.";
        return EXIT_INVALID_ARGS;
    }

    bool retry = false;
    int pipeFd = static_cast<int>(std::strtol(argv[1], nullptr, DECIMAL));
    if (argc >= BINARY_MAX_ARGS && strcmp(argv[BINARY_SECOND_ARG], "retry") == 0) {
        retry = true;
    }
    // Re-load fstab to child process.
    LoadFstab();
    std::string packagePath = argv[0];
    return ProcessUpdater(retry, pipeFd, packagePath, utils::GetCertName());
}
