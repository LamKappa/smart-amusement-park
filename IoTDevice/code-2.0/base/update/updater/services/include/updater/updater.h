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

#ifndef UPDATER_UPDATER_H
#define UPDATER_UPDATER_H
#include <string>
#include "package/packages_info.h"
#include "package/pkg_manager.h"

namespace updater {
enum UpdaterStatus {
    UPDATE_ERROR = -1,
    UPDATE_SUCCESS,
    UPDATE_CORRUPT, /* package or verify failed, something is broken. */
    UPDATE_SKIP, /* skip update because of condition is not satisfied, e.g, battery is low */
    UPDATE_RETRY,
    UPDATE_NORMAL,
    UPDATE_FINISH
};

UpdaterStatus DoInstallUpdaterPackage(hpackage::PkgManager::PkgManagerPtr pkgManager,
    const std::string &packagePath, int retryCount);

UpdaterStatus StartUpdaterProc(hpackage::PkgManager::PkgManagerPtr pkgManager, const std::string &packagePath,
    int retryCount, int &maxTemperature);

int GetUpdatePackageInfo(hpackage::PkgManager::PkgManagerPtr pkgManager, const std::string& path);

int UpdatePreProcess(hpackage::PkgManager::PkgManagerPtr pkgManager, const std::string& path);
} // updater
#endif /* UPDATER_UPDATER_H */
