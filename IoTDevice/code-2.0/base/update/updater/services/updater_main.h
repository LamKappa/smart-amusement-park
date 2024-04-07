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
#ifndef UPDATER_MAIN_H
#define UPDATER_MAIN_H

#include <iostream>
#include <string>
#include <vector>
#include "updater/updater.h"

namespace updater {
enum FactoryResetMode {
    USER_WIPE_DATA = 0,
    FACTORY_WIPE_DATA,
};

struct UpdaterParams {
    bool factoryWipeData;
    bool userWipeData;
    int retryCount;
    std::string updatePackage;
};

int UpdaterMain(int argc, char **argv);

std::vector<std::string> ParseParams(int argc, char **argv);

void PostUpdater();

int FactoryReset(FactoryResetMode mode, const std::string &path);

UpdaterStatus UpdaterFromSdcard();

bool CopyUpdaterLogs(const std::string &sLog, const std::string &dLog);

bool IsBatteryCapacitySufficient();

void CompressLogs(const std::string &name);
} // namespace updater
#endif // UPDATER_MAIN_H
