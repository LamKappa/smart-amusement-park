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
#ifndef PACKAGESINFO_H
#define PACKAGESINFO_H

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>
#include "pkg_manager.h"

/**
 * Constructor used to create a singleton PackagesInfo instance.
 */
namespace updater {
class PackagesInfo;
using PackagesInfoPtr = PackagesInfo *;
class PackagesInfo {
public:
    ~PackagesInfo() {};

    static PackagesInfoPtr GetPackagesInfoInstance();

    static void ReleasePackagesInfoInstance(PackagesInfoPtr pkginfomanager);

    /**
     * Obtains the version information of the update package.
     *
     * @param packagePath       path of the update package
     * @return                  version information of the update package
     */
    std::vector<std::string> GetOTAVersion(hpackage::PkgManager::PkgManagerPtr manager,
        const std::string &versionList, const std::string &versionPath);

   /**
     * Get the Boardid information.
     *
     * @param packagePath       path of the update package
     * @return                  Boardid information of the update package
     */
    std::vector<std::string> GetBoardID(hpackage::PkgManager::PkgManagerPtr manager, const std::string &boardList,
        const std::string &boardListPath);

    /**
     * Gets information about files in the package. For an update package, 
     * you can convert the information into updater information for viewing.
     *
     * @param fileId            file ID
     * @return                  information about files in the package
     */
    bool IsAllowRollback();
private:
    PackagesInfo() {};
};
}
#endif // PACKAGESINFO_H