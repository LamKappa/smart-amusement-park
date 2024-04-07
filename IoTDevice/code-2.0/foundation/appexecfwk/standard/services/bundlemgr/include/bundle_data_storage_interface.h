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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_IBUNDLE_DATA_STORAGE_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_IBUNDLE_DATA_STORAGE_H

#include <map>

#include "inner_bundle_info.h"

namespace OHOS {
namespace AppExecFwk {

class IBundleDataStorage {
public:
    IBundleDataStorage() = default;
    virtual ~IBundleDataStorage() = default;
    /**
     * @brief Load all installed bundles data from bmsdb.json to innerBundleInfos.
     * @param infos Indicates the map to save all installed bundles.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    virtual bool LoadAllData(std::map<std::string, std::map<std::string, InnerBundleInfo>> &infos) const = 0;
    /**
     * @brief Save the bundle data corresponding to the device Id of the bundle name to bmsdb.json.
     * @param deviceId Indicates this device Id corresponding to the bundle name.
     * @param innerBundleInfo Indicates the InnerBundleInfo object to be save.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    virtual bool SaveStorageBundleInfo(const std::string &deviceId, const InnerBundleInfo &innerBundleInfo) const = 0;
    /**
     * @brief Modify the bundle data corresponding to the device Id of the bundle name to bmsdb.json.
     * @param deviceId Indicates this device Id corresponding to the bundle name.
     * @param innerBundleInfo Indicates the InnerBundleInfo object to be Modify.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    virtual bool ModifyStorageBundleInfo(const std::string &deviceId, const InnerBundleInfo &innerBundleInfo) const = 0;
    /**
     * @brief Delete the bundle data corresponding to the device Id of the bundle name to bmsdb.json.
     * @param deviceId Indicates this device Id corresponding to the bundle name.
     * @param innerBundleInfo Indicates the InnerBundleInfo object to be Delete.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    virtual bool DeleteStorageBundleInfo(const std::string &deviceId, const InnerBundleInfo &innerBundleInfo) const = 0;
    /**
     * @brief Delete the module data corresponding to the device Id of the bundle name to bmsdb.json.
     * @param deviceId Indicates this device Id corresponding to the bundle name.
     * @param innerBundleInfo Indicates the InnerBundleInfo object to be Delete.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    virtual bool DeleteStorageModuleInfo(
        const std::string &deviceId, const InnerBundleInfo &innerBundleInfo, const std::string &moduleName) const = 0;
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_IBUNDLE_DATA_STORAGE_H
