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

#include "bundle_data_storage.h"

#include <fstream>
#include <iomanip>

#include "string_ex.h"
#include "app_log_wrapper.h"
#include "bundle_constants.h"
#include "bundle_profile.h"

namespace OHOS {
namespace AppExecFwk {

bool BundleDataStorage::KeyToDeviceAndName(const std::string &key, std::string &deviceId, std::string &bundleName) const
{
    bool ret = false;
    std::vector<std::string> splitStrs;
    const std::string::size_type EXPECT_SPLIT_SIZE = 2;
    OHOS::SplitStr(key, Constants::FILE_UNDERLINE, splitStrs);
    // the expect split size should be 2.
    // key rule is <deviceId>_<bundleName>
    if (splitStrs.size() == EXPECT_SPLIT_SIZE) {
        deviceId = splitStrs[0];
        bundleName = splitStrs[1];
        ret = true;
    }
    APP_LOGD("bundleName = %{public}s", bundleName.c_str());
    return ret;
}

void BundleDataStorage::DeviceAndNameToKey(
    const std::string &deviceId, const std::string &bundleName, std::string &key) const
{
    key.append(deviceId);
    key.append(Constants::FILE_UNDERLINE);
    key.append(bundleName);
    APP_LOGD("bundleName = %{public}s", bundleName.c_str());
}

bool BundleDataStorage::LoadAllData(std::map<std::string, std::map<std::string, InnerBundleInfo>> &infos) const
{
    bool ret = false;
    APP_LOGI("load all installed bundle data to map");

    std::fstream i(Constants::BUNDLE_DATA_BASE_FILE);
    nlohmann::json jParse;
    if (!i.is_open()) {
        APP_LOGE("failed to open bundle database file");
        // if file not exist, should create file here
        std::ofstream o(Constants::BUNDLE_DATA_BASE_FILE);
        o.close();
        return false;
    }
    APP_LOGI("open bundle database file success");
    i.seekg(0, std::ios::end);
    int len = static_cast<int>(i.tellg());
    if (len > 0) {
        i.seekg(0, std::ios::beg);
        i >> jParse;
        for (auto &app : jParse.items()) {
            std::map<std::string, InnerBundleInfo> deviceMap;
            for (auto &device : app.value().items()) {
                InnerBundleInfo innerBundleInfo;
                ret = innerBundleInfo.FromJson(device.value());
                deviceMap.emplace(device.key(), innerBundleInfo);
            }
            auto pair = infos.emplace(app.key(), deviceMap);
            ret = pair.second;
        }
    }
    i.close();
    return ret;
}

bool BundleDataStorage::SaveStorageBundleInfo(const std::string &deviceId, const InnerBundleInfo &innerBundleInfo) const
{
    APP_LOGI("save bundle data");
    bool ret = true;
    std::string appName = innerBundleInfo.GetApplicationName();
    std::fstream f(Constants::BUNDLE_DATA_BASE_FILE);
    bool isExist = f.good();

    if (isExist) {
        nlohmann::json innerInfo;
        innerBundleInfo.ToJson(innerInfo);
        f.seekg(0, std::ios::end);
        int len = static_cast<int>(f.tellg());
        if (len == 0) {
            nlohmann::json appRoot;
            nlohmann::json app;
            app[deviceId] = innerInfo;
            appRoot[appName] = app;
            f << std::setw(Constants::DUMP_INDENT) << appRoot << std::endl;
        } else {
            f.seekg(0, std::ios::beg);
            nlohmann::json jsonFile;
            f >> jsonFile;
            if (jsonFile.find(appName) != jsonFile.end()) {
                if (jsonFile[appName].find(deviceId) != jsonFile[appName].end()) {
                    // appName and device id is exist
                    APP_LOGE("appName = %{public}s is exist", appName.c_str());
                    ret = false;
                } else {
                    jsonFile[appName][deviceId] = innerInfo;
                    f.seekp(0, std::ios::beg);
                    f << std::setw(Constants::DUMP_INDENT) << jsonFile << std::endl;
                }
            } else {
                nlohmann::json app;
                app[deviceId] = innerInfo;
                jsonFile[appName] = app;
                f.seekp(0, std::ios::beg);
                f << std::setw(Constants::DUMP_INDENT) << jsonFile << std::endl;
            }
        }
    } else {
        APP_LOGI("bundle database file not exist");
        ret = false;
    }
    f.close();
    return ret;
}

bool BundleDataStorage::ModifyStorageBundleInfo(
    const std::string &deviceId, const InnerBundleInfo &innerBundleInfo) const
{
    return true;
}

bool BundleDataStorage::DeleteStorageBundleInfo(
    const std::string &deviceId, const InnerBundleInfo &innerBundleInfo) const
{
    APP_LOGI("delete bundle data");
    bool ret = false;
    bool isEmpty = false;
    std::string appName = innerBundleInfo.GetApplicationName();
    std::ifstream i(Constants::BUNDLE_DATA_BASE_FILE);
    nlohmann::json jParse;
    if (!i.is_open()) {
        APP_LOGE("failed to open bundle database file");
        return false;
    } else {
        i.seekg(0, std::ios::end);
        int len = static_cast<int>(i.tellg());
        if (len != 0) {
            i.seekg(0, std::ios::beg);
            i >> jParse;
            if (jParse.find(appName) != jParse.end()) {
                auto it = jParse[appName].find(deviceId);
                if (it != jParse[appName].end()) {
                    jParse[appName].erase(it);
                    if (jParse[appName].size() == 0) {
                        jParse.erase(appName);
                        if (jParse.size() == 0) {
                            isEmpty = true;
                        }
                    }
                    ret = true;
                }
            } else {
                APP_LOGE("not find appName = %{public}s", appName.c_str());
            }
        } else {
            APP_LOGE("file is empty appName = %{private}s", appName.c_str());
        }
    }
    i.close();

    std::ofstream o(Constants::BUNDLE_DATA_BASE_FILE);
    if (!o.is_open()) {
        APP_LOGE("failed to open bundle database file");
        ret = false;
    } else {
        if (!isEmpty) {
            o << std::setw(Constants::DUMP_INDENT) << jParse;
        }
    }
    o.close();
    return ret;
}

bool BundleDataStorage::DeleteStorageModuleInfo(
    const std::string &deviceId, const InnerBundleInfo &innerBundleInfo, const std::string &moduleName) const
{
    return true;
}

}  // namespace AppExecFwk
}  // namespace OHOS
