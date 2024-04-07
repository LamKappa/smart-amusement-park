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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_DATA_STORAGE_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_DATA_STORAGE_H

#include <map>

#include "inner_bundle_info.h"
#include "bundle_data_storage_interface.h"

namespace OHOS {
namespace AppExecFwk {

class BundleDataStorage : public IBundleDataStorage {
public:
    /**
     * @brief Load all installed bundles data from bmsdb.json to innerBundleInfos.
     * @param infos Indicates the map to save all installed bundles.
     * @return Returns true if the data is successfully loaded; returns false otherwise.
     */
    virtual bool LoadAllData(std::map<std::string, std::map<std::string, InnerBundleInfo>> &infos) const;
    /**
     * @brief Save the bundle data corresponding to the device Id of the bundle name to bmsdb.json.
     * @param deviceId Indicates this device Id corresponding to the bundle name.
     * @param innerBundleInfo Indicates the InnerBundleInfo object to be save.
     * @return Returns true if the data is successfully saved; returns false otherwise.
     */
    virtual bool SaveStorageBundleInfo(const std::string &deviceId, const InnerBundleInfo &innerBundleInfo) const;
    /**
     * @brief Modify the bundle data corresponding to the device Id of the bundle name to bmsdb.json.
     * @param deviceId Indicates this device Id corresponding to the bundle name.
     * @param innerBundleInfo Indicates the InnerBundleInfo object to be Modify.
     * @return Returns true if the data is successfully modified; returns false otherwise.
     */
    virtual bool ModifyStorageBundleInfo(const std::string &deviceId, const InnerBundleInfo &innerBundleInfo) const;
    /**
     * @brief Delete the bundle data corresponding to the device Id of the bundle name to bmsdb.json.
     * @param deviceId Indicates this device Id corresponding to the bundle name.
     * @param innerBundleInfo Indicates the InnerBundleInfo object to be Delete.
     * @return Returns true if the data is successfully deleted; returns false otherwise.
     */
    virtual bool DeleteStorageBundleInfo(const std::string &deviceId, const InnerBundleInfo &innerBundleInfo) const;
    /**
     * @brief Delete the module data corresponding to the device Id of the bundle name to bmsdb.json.
     * @param deviceId Indicates this device Id corresponding to the bundle name.
     * @param innerBundleInfo Indicates the InnerBundleInfo object to be Delete.
     * @return Returns true if the data is successfully deleted; returns false otherwise.
     */
    virtual bool DeleteStorageModuleInfo(
        const std::string &deviceId, const InnerBundleInfo &innerBundleInfo, const std::string &moduleName) const;

private:
    bool KeyToDeviceAndName(const std::string &key, std::string &deviceId, std::string &bundleName) const;
    void DeviceAndNameToKey(const std::string &deviceId, const std::string &bundleName, std::string &key) const;
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_DATA_STORAGE_H
