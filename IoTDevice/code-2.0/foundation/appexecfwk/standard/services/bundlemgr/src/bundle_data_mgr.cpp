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

#include "bundle_data_mgr.h"

#include <chrono>

#include "nlohmann/json.hpp"
#include "app_log_wrapper.h"
#include "permission/permission_kit.h"
#include "bundle_constants.h"
#include "bundle_data_storage.h"
#include "json_serializer.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "bundle_status_callback_death_recipient.h"

namespace OHOS {
namespace AppExecFwk {

BundleDataMgr::BundleDataMgr()
{
    InitStateTransferMap();
    if (!dataStorage_) {
        dataStorage_ = std::make_shared<BundleDataStorage>();
    }
    APP_LOGI("BundleDataMgr instance is created");
}

BundleDataMgr::~BundleDataMgr()
{
    APP_LOGI("BundleDataMgr instance is destroyed");
    installStates_.clear();
    transferStates_.clear();
    bundleInfos_.clear();
}

bool BundleDataMgr::LoadDataFromPersistentStorage()
{
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    bool ret = dataStorage_->LoadAllData(bundleInfos_);
    if (ret) {
        if (bundleInfos_.empty()) {
            APP_LOGW("persistent data is empty");
            return false;
        }
        for (const auto &item : bundleInfos_) {
            std::lock_guard<std::mutex> lock(stateMutex_);
            installStates_.emplace(item.first, InstallState::INSTALL_SUCCESS);
        }
        RestoreUidAndGid();
        allInstallFlag_ = true;
    }
    return ret;
}

bool BundleDataMgr::UpdateBundleInstallState(const std::string &bundleName, const InstallState state)
{
    if (bundleName.empty()) {
        APP_LOGW("update result:fail, reason:bundle name is empty");
        return false;
    }

    std::lock_guard<std::mutex> lock(stateMutex_);
    auto item = installStates_.find(bundleName);
    if (item == installStates_.end()) {
        if (state == InstallState::INSTALL_START) {
            installStates_.emplace(bundleName, state);
            APP_LOGD("update result:success, state:INSTALL_START");
            return true;
        }
        APP_LOGW("update result:fail, reason:incorrect state");
        return false;
    }

    auto stateRange = transferStates_.equal_range(state);
    for (auto previousState = stateRange.first; previousState != stateRange.second; ++previousState) {
        if (item->second == previousState->second) {
            APP_LOGI("update result:success, current:%{public}d, state:%{public}d", previousState->second, state);
            if (IsDeleteDataState(state)) {
                installStates_.erase(item);
                DeleteBundleInfo(bundleName, state);
                return true;
            }
            item->second = state;
            return true;
        }
    }
    APP_LOGW("update result:fail, reason:incorrect current:%{public}d, state:%{public}d", item->second, state);
    return false;
}

bool BundleDataMgr::AddInnerBundleInfo(const std::string &bundleName, InnerBundleInfo &info)
{
    APP_LOGI("to save info:%{public}s", info.GetBundleName().c_str());
    if (bundleName.empty()) {
        APP_LOGW("save info fail, empty bundle name");
        return false;
    }

    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem != bundleInfos_.end()) {
        APP_LOGE("bundle info already exist");
        return false;
    }
    std::lock_guard<std::mutex> stateLock(stateMutex_);
    auto statusItem = installStates_.find(bundleName);
    if (statusItem == installStates_.end()) {
        APP_LOGE("save info fail, app:%{public}s is not installed", bundleName.c_str());
        return false;
    }
    if (statusItem->second == InstallState::INSTALL_SUCCESS) {
        APP_LOGI("save bundle:%{public}s info", bundleName.c_str());
        int64_t time =
            std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch())
                .count();
        #if __WORDSIZE == 64
            APP_LOGI("the bundle install time is %{public}ld", time);
        #else
            APP_LOGI("the bundle install time is %{public}lld", time);
        #endif
        info.SetBundleInstallTime(time);
        if (dataStorage_->SaveStorageBundleInfo(Constants::CURRENT_DEVICE_ID, info)) {
            APP_LOGI("write storage success bundle:%{public}s", bundleName.c_str());
            std::map<std::string, InnerBundleInfo> infoWithId;
            infoWithId.emplace(Constants::CURRENT_DEVICE_ID, info);
            bundleInfos_.emplace(bundleName, infoWithId);
            return true;
        }
    }
    return false;
}

bool BundleDataMgr::AddNewModuleInfo(
    const std::string &bundleName, const InnerBundleInfo &newInfo, InnerBundleInfo &oldInfo)
{
    APP_LOGI("add new module info:%{public}s", bundleName.c_str());
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem == bundleInfos_.end()) {
        APP_LOGE("bundle info not exist");
        return false;
    }
    std::lock_guard<std::mutex> stateLock(stateMutex_);
    auto statusItem = installStates_.find(bundleName);
    if (statusItem == installStates_.end()) {
        APP_LOGE("save info fail, app:%{public}s is not updated", bundleName.c_str());
        return false;
    }
    if (statusItem->second == InstallState::UPDATING_SUCCESS) {
        APP_LOGI("save bundle:%{public}s info", bundleName.c_str());
        int64_t time =
            std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch())
                .count();
        #if __WORDSIZE == 64
            APP_LOGI("the bundle update time is %{public}ld", time);
        #else
            APP_LOGI("the bundle update time is %{public}lld", time);
        #endif
        oldInfo.SetBundleUpdateTime(time);
        oldInfo.UpdateVersionInfo(newInfo);
        oldInfo.AddModuleInfo(newInfo);
        oldInfo.SetBundleStatus(InnerBundleInfo::BundleStatus::ENABLED);
        if (dataStorage_->DeleteStorageBundleInfo(Constants::CURRENT_DEVICE_ID, oldInfo)) {
            if (dataStorage_->SaveStorageBundleInfo(Constants::CURRENT_DEVICE_ID, oldInfo)) {
                APP_LOGI("update storage success bundle:%{public}s", bundleName.c_str());
                bundleInfos_.at(bundleName).at(Constants::CURRENT_DEVICE_ID) = oldInfo;
                return true;
            }
        }
    }
    return false;
}

bool BundleDataMgr::RemoveModuleInfo(
    const std::string &bundleName, const std::string &modulePackage, InnerBundleInfo &oldInfo)
{
    APP_LOGI("remove module info:%{public}s/%{public}s", bundleName.c_str(), modulePackage.c_str());
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem == bundleInfos_.end()) {
        APP_LOGE("bundle info not exist");
        return false;
    }
    std::lock_guard<std::mutex> stateLock(stateMutex_);
    auto statusItem = installStates_.find(bundleName);
    if (statusItem == installStates_.end()) {
        APP_LOGE("save info fail, app:%{public}s is not updated", bundleName.c_str());
        return false;
    }
    if (statusItem->second == InstallState::UNINSTALL_START) {
        APP_LOGI("save bundle:%{public}s info", bundleName.c_str());
        oldInfo.RemoveModuleInfo(modulePackage);
        oldInfo.SetBundleStatus(InnerBundleInfo::BundleStatus::ENABLED);
        if (dataStorage_->DeleteStorageBundleInfo(Constants::CURRENT_DEVICE_ID, oldInfo)) {
            if (dataStorage_->SaveStorageBundleInfo(Constants::CURRENT_DEVICE_ID, oldInfo)) {
                APP_LOGI("update storage success bundle:%{public}s", bundleName.c_str());
                bundleInfos_.at(bundleName).at(Constants::CURRENT_DEVICE_ID) = oldInfo;
                return true;
            }
        }
        APP_LOGI("after delete modulePackage:%{public}s info", modulePackage.c_str());
    }
    return true;
}

bool BundleDataMgr::UpdateInnerBundleInfo(
    const std::string &bundleName, const InnerBundleInfo &newInfo, InnerBundleInfo &oldInfo)
{
    APP_LOGI("update module info:%{public}s", bundleName.c_str());
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem == bundleInfos_.end()) {
        APP_LOGE("bundle info not exist");
        return false;
    }
    std::lock_guard<std::mutex> stateLock(stateMutex_);
    auto statusItem = installStates_.find(bundleName);
    if (statusItem == installStates_.end()) {
        APP_LOGE("save info fail, app:%{public}s is not updated", bundleName.c_str());
        return false;
    }
    if (statusItem->second == InstallState::UPDATING_SUCCESS) {
        APP_LOGI("save bundle:%{public}s info", bundleName.c_str());
        int64_t time =
            std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch())
                .count();
        #if __WORDSIZE == 64
            APP_LOGI("the bundle update time is %{public}ld", time);
        #else
            APP_LOGI("the bundle update time is %{public}lld", time);
        #endif
        oldInfo.SetBundleUpdateTime(time);
        oldInfo.UpdateVersionInfo(newInfo);
        oldInfo.UpdateModuleInfo(newInfo);
        oldInfo.SetBundleStatus(InnerBundleInfo::BundleStatus::ENABLED);
        if (dataStorage_->DeleteStorageBundleInfo(Constants::CURRENT_DEVICE_ID, oldInfo)) {
            if (dataStorage_->SaveStorageBundleInfo(Constants::CURRENT_DEVICE_ID, oldInfo)) {
                APP_LOGI("update storage success bundle:%{public}s", bundleName.c_str());
                bundleInfos_.at(bundleName).at(Constants::CURRENT_DEVICE_ID) = oldInfo;
                return true;
            }
        }
    }
    return false;
}

bool BundleDataMgr::QueryAbilityInfo(const Want &want, AbilityInfo &abilityInfo) const
{
    // for launcher
    if (want.HasEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM)) {
        std::lock_guard<std::mutex> lock(bundleInfoMutex_);
        for (const auto &item : bundleInfos_) {
            for (const auto &info : item.second) {
                if (allInstallFlag_ && info.second.GetIsLauncherApp()) {
                    APP_LOGI("find launcher app %{public}s", info.second.GetBundleName().c_str());
                    info.second.GetMainAbilityInfo(abilityInfo);
                    info.second.GetApplicationInfo(
                        ApplicationFlag::GET_APPLICATION_INFO_WITH_PERMS, 0, abilityInfo.applicationInfo);
                    return true;
                }
            }
        }
        return false;
    }

    ElementName element = want.GetElement();
    std::string abilityName = element.GetAbilityName();
    std::string bundleName = element.GetBundleName();
    APP_LOGI("bundle name:%{public}s, ability name:%{public}s", bundleName.c_str(), abilityName.c_str());
    if (bundleName.empty() || abilityName.empty()) {
        APP_LOGW("Want error, bundleName or abilityName empty");
        return false;
    }

    std::string keyName = bundleName + abilityName;
    APP_LOGI("ability, name:%{public}s", keyName.c_str());
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.empty()) {
        APP_LOGI("bundleInfos_ is empty");
        return false;
    }
    auto item = bundleInfos_.find(bundleName);
    if (item == bundleInfos_.end()) {
        APP_LOGI("bundle:%{public}s not find", bundleName.c_str());
        return false;
    }

    auto infoWithIdItem = item->second.find(Constants::CURRENT_DEVICE_ID);
    if (infoWithIdItem == item->second.end()) {
        APP_LOGI("bundle:%{public}s device id not find", bundleName.c_str());
        return false;
    }
    if (infoWithIdItem->second.IsDisabled()) {
        APP_LOGI("app %{public}s is disabled", infoWithIdItem->second.GetBundleName().c_str());
        return false;
    }
    auto ability = infoWithIdItem->second.FindAbilityInfo(bundleName, abilityName);
    if (!ability) {
        APP_LOGE("ability:%{public}s not find", keyName.c_str());
        return false;
    }
    abilityInfo = (*ability);
    infoWithIdItem->second.GetApplicationInfo(
        ApplicationFlag::GET_APPLICATION_INFO_WITH_PERMS, 0, abilityInfo.applicationInfo);
    return true;
}

bool BundleDataMgr::QueryAbilityInfoByUri(const std::string &abilityUri, AbilityInfo &abilityInfo) const
{
    APP_LOGI("QueryAbilityInfoByUri");
    if (abilityUri.empty()) {
        return false;
    }
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.empty()) {
        APP_LOGI("bundleInfos_ data is empty");
        return false;
    }
    for (const auto &item : bundleInfos_) {
        for (const auto &info : item.second) {
            if (info.second.IsDisabled()) {
                APP_LOGI("app %{public}s is disabled", info.second.GetBundleName().c_str());
                continue;
            }
            auto ability = info.second.FindAbilityInfoByUri(abilityUri);
            if (!ability) {
                continue;
            }
            abilityInfo = (*ability);
            info.second.GetApplicationInfo(
                ApplicationFlag::GET_APPLICATION_INFO_WITH_PERMS, 0, abilityInfo.applicationInfo);
            return true;
        }
    }
    return false;
}

bool BundleDataMgr::GetApplicationInfo(
    const std::string &appName, const ApplicationFlag flag, const int userId, ApplicationInfo &appInfo) const
{
    APP_LOGI("GetApplicationInfo %{public}s", appName.c_str());
    if (appName.empty()) {
        return false;
    }
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.empty()) {
        APP_LOGI("bundleInfos_ data is empty");
        return false;
    }
    APP_LOGI("GetApplicationInfo %{public}s", appName.c_str());
    auto infoItem = bundleInfos_.find(appName);
    if (infoItem == bundleInfos_.end()) {
        return false;
    }
    APP_LOGI("GetApplicationInfo %{public}s", infoItem->first.c_str());
    auto bundleInfo = infoItem->second.find(Constants::CURRENT_DEVICE_ID);
    if (bundleInfo == infoItem->second.end()) {
        return false;
    }
    if (bundleInfo->second.IsDisabled()) {
        APP_LOGI("app %{public}s is disabled", bundleInfo->second.GetBundleName().c_str());
        return false;
    }
    bundleInfo->second.GetApplicationInfo(flag, userId, appInfo);
    return true;
}

bool BundleDataMgr::GetApplicationInfos(
    const ApplicationFlag flag, const int userId, std::vector<ApplicationInfo> &appInfos) const
{
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.empty()) {
        APP_LOGI("bundleInfos_ data is empty");
        return false;
    }
    bool find = false;
    for (const auto &item : bundleInfos_) {
        for (const auto &info : item.second) {
            if (info.second.IsDisabled()) {
                APP_LOGI("app %{public}s is disabled", info.second.GetBundleName().c_str());
                continue;
            }
            ApplicationInfo appInfo;
            info.second.GetApplicationInfo(flag, userId, appInfo);
            appInfos.emplace_back(appInfo);
            find = true;
        }
    }
    APP_LOGI("get installed bundles success");
    return find;
}

bool BundleDataMgr::GetBundleInfo(const std::string &bundleName, const BundleFlag flag, BundleInfo &bundleInfo) const
{
    if (bundleName.empty()) {
        APP_LOGW("bundle name is empty");
        return false;
    }

    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.empty()) {
        APP_LOGI("bundleInfos_ data is empty");
        return false;
    }
    APP_LOGI("GetBundleInfo %{public}s", bundleName.c_str());
    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem == bundleInfos_.end()) {
        return false;
    }
    APP_LOGI("GetBundleInfo %{public}s", infoItem->first.c_str());
    auto innerBundleInfo = infoItem->second.find(Constants::CURRENT_DEVICE_ID);
    if (innerBundleInfo == infoItem->second.end()) {
        return false;
    }
    if (innerBundleInfo->second.IsDisabled()) {
        APP_LOGI("app %{public}s is disabled", innerBundleInfo->second.GetBundleName().c_str());
        return false;
    }
    innerBundleInfo->second.GetBundleInfo(flag, bundleInfo);
    APP_LOGI("bundle:%{public}s device id find success", bundleName.c_str());
    return true;
}

bool BundleDataMgr::GetBundleInfosByMetaData(const std::string &metaData, std::vector<BundleInfo> &bundleInfos) const
{
    if (metaData.empty()) {
        APP_LOGW("bundle name is empty");
        return false;
    }

    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.empty()) {
        APP_LOGI("bundleInfos_ data is empty");
        return false;
    }
    bool find = false;
    for (const auto &item : bundleInfos_) {
        for (const auto &info : item.second) {
            if (info.second.IsDisabled()) {
                APP_LOGI("app %{public}s is disabled", info.second.GetBundleName().c_str());
                continue;
            }
            if (info.second.CheckSpecialMetaData(metaData)) {
                BundleInfo bundleInfo;
                info.second.GetBundleInfo(BundleFlag::GET_BUNDLE_WITH_ABILITIES, bundleInfo);
                bundleInfos.emplace_back(bundleInfo);
                find = true;
            }
        }
    }
    return find;
}

bool BundleDataMgr::GetBundleList(std::vector<std::string> &bundleNames) const
{
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.empty()) {
        APP_LOGI("bundleInfos_ data is empty");
        return false;
    }
    bool find = false;
    for (const auto &item : bundleInfos_) {
        bundleNames.emplace_back(item.first);
        find = true;
    }
    APP_LOGI("get installed bundles success");
    return find;
}

bool BundleDataMgr::GetBundleInfos(const BundleFlag flag, std::vector<BundleInfo> &bundleInfos) const
{
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.empty()) {
        APP_LOGI("bundleInfos_ data is empty");
        return false;
    }
    bool find = false;
    for (const auto &item : bundleInfos_) {
        std::map<std::string, InnerBundleInfo> infoWithId = item.second;
        auto infoWithIdItem = infoWithId.find(Constants::CURRENT_DEVICE_ID);
        if (infoWithIdItem == infoWithId.end()) {
            APP_LOGI("current device id bundle not find");
            continue;
        }
        if (infoWithIdItem->second.IsDisabled()) {
            APP_LOGI("app %{public}s is disabled", infoWithIdItem->second.GetBundleName().c_str());
            continue;
        }
        BundleInfo bundleInfo;
        infoWithIdItem->second.GetBundleInfo(flag, bundleInfo);
        bundleInfos.emplace_back(bundleInfo);
        find = true;
    }
    APP_LOGI("get installed bundle infos success");
    return find;
}

bool BundleDataMgr::GetBundleNameForUid(const int uid, std::string &bundleName) const
{
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.empty()) {
        APP_LOGI("bundleInfos_ data is empty");
        return false;
    }
    for (const auto &item : bundleInfos_) {
        for (const auto &info : item.second) {
            if (info.second.IsDisabled()) {
                APP_LOGI("app %{public}s is disabled", info.second.GetBundleName().c_str());
                continue;
            }
            if (info.second.GetUid() == uid) {
                bundleName = info.second.GetBundleName();
                return true;
            }
        }
    }
    return false;
}

bool BundleDataMgr::GetBundleGids(const std::string &bundleName, std::vector<int> &gids) const
{
    return true;
}

bool BundleDataMgr::QueryKeepAliveBundleInfos(std::vector<BundleInfo> &bundleInfos) const
{
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.empty()) {
        APP_LOGI("bundleInfos_ data is empty");
        return false;
    }
    for (const auto &item : bundleInfos_) {
        for (const auto &info : item.second) {
            if (info.second.IsDisabled()) {
                APP_LOGI("app %{public}s is disabled", info.second.GetBundleName().c_str());
                continue;
            }
            if (info.second.GetIsKeepAlive()) {
                BundleInfo bundleInfo;
                info.second.GetBundleInfo(BundleFlag::GET_BUNDLE_WITH_ABILITIES, bundleInfo);
                bundleInfos.emplace_back(bundleInfo);
            }
        }
    }
    return !(bundleInfos.empty());
}

std::string BundleDataMgr::GetAbilityLabel(const std::string &bundleName, const std::string &className) const
{
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.empty()) {
        APP_LOGI("bundleInfos_ data is empty");
        return Constants::EMPTY_STRING;
    }
    APP_LOGI("GetAbilityLabel %{public}s", bundleName.c_str());
    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem == bundleInfos_.end()) {
        return Constants::EMPTY_STRING;
    }
    auto innerBundleInfo = infoItem->second.find(Constants::CURRENT_DEVICE_ID);
    if (innerBundleInfo == infoItem->second.end()) {
        return Constants::EMPTY_STRING;
    }
    if (innerBundleInfo->second.IsDisabled()) {
        APP_LOGI("app %{public}s is disabled", innerBundleInfo->second.GetBundleName().c_str());
        return Constants::EMPTY_STRING;
    }
    auto ability = innerBundleInfo->second.FindAbilityInfo(bundleName, className);
    if (!ability) {
        return Constants::EMPTY_STRING;
    }
    return (*ability).label;
}

bool BundleDataMgr::GetHapModuleInfo(const AbilityInfo &abilityInfo, HapModuleInfo &hapModuleInfo) const
{
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.empty()) {
        APP_LOGI("bundleInfos_ data is empty");
        return false;
    }
    APP_LOGI("GetHapModuleInfo %{public}s", abilityInfo.bundleName.c_str());
    auto infoItem = bundleInfos_.find(abilityInfo.bundleName);
    if (infoItem == bundleInfos_.end()) {
        return false;
    }
    auto innerBundleInfo = infoItem->second.find(Constants::CURRENT_DEVICE_ID);
    if (innerBundleInfo == infoItem->second.end()) {
        return false;
    }
    if (innerBundleInfo->second.IsDisabled()) {
        APP_LOGI("app %{public}s is disabled", innerBundleInfo->second.GetBundleName().c_str());
        return false;
    }
    auto module = innerBundleInfo->second.FindHapModuleInfo(abilityInfo.package);
    if (!module) {
        APP_LOGE("can not find module %{public}s", abilityInfo.package.c_str());
        return false;
    }
    hapModuleInfo = *module;
    return true;
}

bool BundleDataMgr::GetLaunchWantForBundle(const std::string &bundleName, Want &want) const
{
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.empty()) {
        APP_LOGI("bundleInfos_ data is empty");
        return false;
    }
    APP_LOGI("GetLaunchWantForBundle %{public}s", bundleName.c_str());
    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem == bundleInfos_.end()) {
        return false;
    }
    auto innerBundleInfo = infoItem->second.find(Constants::CURRENT_DEVICE_ID);
    if (innerBundleInfo == infoItem->second.end()) {
        return false;
    }
    if (innerBundleInfo->second.IsDisabled()) {
        APP_LOGI("app %{public}s is disabled", innerBundleInfo->second.GetBundleName().c_str());
        return false;
    }
    std::string mainAbility = innerBundleInfo->second.GetMainAbility();
    if (mainAbility.empty()) {
        APP_LOGE("no main ability in the bundle %{public}s", bundleName.c_str());
        return false;
    }
    auto skills = innerBundleInfo->second.FindSkills(mainAbility);
    if (!skills || (*skills).empty()) {
        APP_LOGE("no skills of %{public}s", mainAbility.c_str());
        return false;
    }

    want.SetElementName(Constants::CURRENT_DEVICE_ID, bundleName, mainAbility);
    want.SetAction((*skills)[0].actions[0]);
    for (auto &skill : (*skills)) {
        for (auto &entity : skill.entities) {
            want.AddEntity(entity);
        }
    }
    return true;
}

bool BundleDataMgr::CheckIsSystemAppByUid(const int uid) const
{
    int maxSysUid{Constants::MAX_SYS_UID};
    int baseSysUid{Constants::ROOT_UID};
    if (uid >= baseSysUid && uid <= maxSysUid) {
        return true;
    }
    return false;
}

void BundleDataMgr::InitStateTransferMap()
{
    transferStates_.emplace(InstallState::INSTALL_SUCCESS, InstallState::INSTALL_START);
    transferStates_.emplace(InstallState::INSTALL_FAIL, InstallState::INSTALL_START);
    transferStates_.emplace(InstallState::UNINSTALL_START, InstallState::INSTALL_SUCCESS);
    transferStates_.emplace(InstallState::UNINSTALL_FAIL, InstallState::UNINSTALL_START);
    transferStates_.emplace(InstallState::UNINSTALL_SUCCESS, InstallState::UNINSTALL_START);
    transferStates_.emplace(InstallState::UPDATING_START, InstallState::INSTALL_SUCCESS);
    transferStates_.emplace(InstallState::UPDATING_SUCCESS, InstallState::UPDATING_START);
    transferStates_.emplace(InstallState::UPDATING_FAIL, InstallState::UPDATING_START);
    transferStates_.emplace(InstallState::INSTALL_SUCCESS, InstallState::UPDATING_START);
    transferStates_.emplace(InstallState::INSTALL_SUCCESS, InstallState::UPDATING_SUCCESS);
    transferStates_.emplace(InstallState::INSTALL_SUCCESS, InstallState::UNINSTALL_START);
}

bool BundleDataMgr::IsDeleteDataState(const InstallState state) const
{
    return (state == InstallState::INSTALL_FAIL || state == InstallState::UNINSTALL_FAIL ||
            state == InstallState::UNINSTALL_SUCCESS || state == InstallState::UPDATING_FAIL);
}

bool BundleDataMgr::IsDisableState(const InstallState state) const
{
    if (state == InstallState::UPDATING_START || state == InstallState::UNINSTALL_START) {
        return true;
    }
    return false;
}

void BundleDataMgr::DeleteBundleInfo(const std::string &bundleName, const InstallState state)
{
    if (InstallState::INSTALL_FAIL == state) {
        APP_LOGW("del fail, bundle:%{public}s has no installed info", bundleName.c_str());
        return;
    }

    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem != bundleInfos_.end()) {
        APP_LOGI("del bundle name:%{public}s", bundleName.c_str());
        const InnerBundleInfo &innerBundleInfo = infoItem->second[Constants::CURRENT_DEVICE_ID];
        RecycleUidAndGid(innerBundleInfo);
        bool ret = dataStorage_->DeleteStorageBundleInfo(Constants::CURRENT_DEVICE_ID, innerBundleInfo);
        if (!ret) {
            APP_LOGW("delete storage error name:%{public}s", bundleName.c_str());
        }
        // only delete self-device bundle
        infoItem->second.erase(Constants::CURRENT_DEVICE_ID);
        if (infoItem->second.empty()) {
            APP_LOGD("now only store current device installed info, delete all");
            bundleInfos_.erase(bundleName);
        }
    }
}

bool BundleDataMgr::IsAppOrAbilityInstalled(const std::string &bundleName) const
{
    if (bundleName.empty()) {
        APP_LOGW("name:%{public}s empty", bundleName.c_str());
        return false;
    }

    std::lock_guard<std::mutex> lock(stateMutex_);
    auto statusItem = installStates_.find(bundleName);
    if (statusItem == installStates_.end()) {
        APP_LOGW("name:%{public}s not find", bundleName.c_str());
        return false;
    }

    if (statusItem->second == InstallState::INSTALL_SUCCESS) {
        return true;
    }

    APP_LOGW("name:%{public}s not install success", bundleName.c_str());
    return false;
}

bool BundleDataMgr::GetInnerBundleInfo(
    const std::string &bundleName, const std::string &deviceId, InnerBundleInfo &info)
{
    APP_LOGI("GetInnerBundleInfo %{public}s", bundleName.c_str());
    if (bundleName.empty() || deviceId.empty()) {
        APP_LOGE("bundleName or deviceId empty");
        return false;
    }

    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem == bundleInfos_.end()) {
        APP_LOGE("can not find bundle %{public}s", bundleName.c_str());
        return false;
    }
    auto infoWithIdItem = infoItem->second.find(deviceId);
    if (infoWithIdItem == infoItem->second.end()) {
        APP_LOGE("bundle:%{public}s device id not find", bundleName.c_str());
        return false;
    }
    infoWithIdItem->second.SetBundleStatus(InnerBundleInfo::BundleStatus::DISABLED);
    info = infoWithIdItem->second;
    return true;
}

bool BundleDataMgr::DisableBundle(const std::string &bundleName)
{
    APP_LOGI("DisableBundle %{public}s", bundleName.c_str());
    if (bundleName.empty()) {
        APP_LOGE("bundleName empty");
        return false;
    }

    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem == bundleInfos_.end()) {
        APP_LOGE("can not find bundle %{public}s", bundleName.c_str());
        return false;
    }
    auto infoWithIdItem = infoItem->second.find(Constants::CURRENT_DEVICE_ID);
    if (infoWithIdItem == infoItem->second.end()) {
        APP_LOGE("bundle:%{public}s device id not find", bundleName.c_str());
        return false;
    }
    infoWithIdItem->second.SetBundleStatus(InnerBundleInfo::BundleStatus::DISABLED);
    return true;
}

bool BundleDataMgr::EnableBundle(const std::string &bundleName)
{
    APP_LOGI("EnableBundle %{public}s", bundleName.c_str());
    if (bundleName.empty()) {
        APP_LOGE("bundleName empty");
        return false;
    }

    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem == bundleInfos_.end()) {
        APP_LOGE("can not find bundle %{public}s", bundleName.c_str());
        return false;
    }
    auto infoWithIdItem = infoItem->second.find(Constants::CURRENT_DEVICE_ID);
    if (infoWithIdItem == infoItem->second.end()) {
        APP_LOGE("bundle:%{public}s device id not find", bundleName.c_str());
        return false;
    }
    infoWithIdItem->second.SetBundleStatus(InnerBundleInfo::BundleStatus::ENABLED);
    return true;
}

bool BundleDataMgr::IsApplicationEnabled(const std::string &bundleName) const
{
    APP_LOGI("IsApplicationEnabled %{public}s", bundleName.c_str());
    if (bundleName.empty()) {
        APP_LOGE("bundleName empty");
        return false;
    }

    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem == bundleInfos_.end()) {
        APP_LOGE("can not find bundle %{public}s", bundleName.c_str());
        return false;
    }
    auto infoWithIdItem = infoItem->second.find(Constants::CURRENT_DEVICE_ID);
    if (infoWithIdItem == infoItem->second.end()) {
        APP_LOGE("bundle:%{public}s device id not find", bundleName.c_str());
        return false;
    }
    return (infoWithIdItem->second.GetBundleStatus() == InnerBundleInfo::BundleStatus::ENABLED) ? true : false;
}

bool BundleDataMgr::SetApplicationEnabled(const std::string &bundleName, bool isEnable)
{
    APP_LOGI("SetApplicationEnabled %{public}s", bundleName.c_str());
    if (bundleName.empty()) {
        APP_LOGE("bundleName empty");
        return false;
    }
    return isEnable ? EnableBundle(bundleName) : DisableBundle(bundleName);
}

bool BundleDataMgr::RegisterBundleStatusCallback(const sptr<IBundleStatusCallback> &bundleStatusCallback)
{
    APP_LOGI("RegisterBundleStatusCallback %{public}s", bundleStatusCallback->GetBundleName().c_str());
    std::lock_guard<std::mutex> lock(callbackMutex_);
    callbackList_.emplace_back(bundleStatusCallback);
    if (bundleStatusCallback->AsObject() != nullptr) {
        sptr<BundleStatusCallbackDeathRecipient> deathRecipient = new BundleStatusCallbackDeathRecipient();
        bundleStatusCallback->AsObject()->AddDeathRecipient(deathRecipient);
    }
    return true;
}

bool BundleDataMgr::ClearBundleStatusCallback(const sptr<IBundleStatusCallback> &bundleStatusCallback)
{
    APP_LOGI("ClearBundleStatusCallback %{public}s", bundleStatusCallback->GetBundleName().c_str());
    std::lock_guard<std::mutex> lock(callbackMutex_);
    callbackList_.erase(std::remove_if(callbackList_.begin(),
                        callbackList_.end(),
                        [&](const sptr<IBundleStatusCallback> &callback) {
                                return callback->AsObject() == bundleStatusCallback->AsObject();
                            }),
                        callbackList_.end());
    return true;
}

bool BundleDataMgr::UnregisterBundleStatusCallback()
{
    std::lock_guard<std::mutex> lock(callbackMutex_);
    callbackList_.clear();
    return true;
}

bool BundleDataMgr::GenerateUidAndGid(InnerBundleInfo &info)
{
    int baseUid;
    std::map<int, std::string> &innerMap = [&]() -> decltype(auto) {
        switch (info.GetAppType()) {
            case Constants::AppType::SYSTEM_APP:
                baseUid = Constants::BASE_SYS_UID;
                return (sysUidMap_);
            case Constants::AppType::THIRD_SYSTEM_APP:
                baseUid = Constants::BASE_SYS_VEN_UID;
                return (sysVendorUidMap_);
            case Constants::AppType::THIRD_PARTY_APP:
                baseUid = Constants::BASE_APP_UID;
                return (appUidMap_);
            default:
                APP_LOGE("app type error");
                baseUid = Constants::BASE_APP_UID;
                return (appUidMap_);
        }
    }();
    std::lock_guard<std::mutex> lock(uidMapMutex_);
    if (innerMap.empty()) {
        APP_LOGI("first app install");
        innerMap.emplace(0, info.GetBundleName());
        info.SetUid(baseUid);
        info.SetGid(baseUid);
        return true;
    }
    int uid = 0;
    for (int i = 0; i < innerMap.rbegin()->first; ++i) {
        if (innerMap.find(i) == innerMap.end()) {
            APP_LOGI("the %{public}d app install", i);
            innerMap.emplace(i, info.GetBundleName());
            uid = i + baseUid;
            info.SetUid(baseUid);
            info.SetGid(baseUid);
            return true;
        }
    }
    if ((info.GetAppType() == Constants::AppType::SYSTEM_APP) && (innerMap.rbegin()->first == Constants::MAX_SYS_UID)) {
        return false;
    }
    if ((info.GetAppType() == Constants::AppType::THIRD_SYSTEM_APP) &&
        (innerMap.rbegin()->first == Constants::MAX_SYS_VEN_UID)) {
        return false;
    }

    innerMap.emplace((innerMap.rbegin()->first + 1), info.GetBundleName());
    uid = innerMap.rbegin()->first + baseUid;
    APP_LOGI("the uid is %{public}d", uid);
    info.SetUid(uid);
    info.SetGid(uid);
    return true;
}

bool BundleDataMgr::RecycleUidAndGid(const InnerBundleInfo &info)
{
    std::map<int, std::string> &innerMap = [&]() -> decltype(auto) {
        switch (info.GetAppType()) {
            case Constants::AppType::SYSTEM_APP:
                return (sysUidMap_);
            case Constants::AppType::THIRD_SYSTEM_APP:
                return (sysVendorUidMap_);
            case Constants::AppType::THIRD_PARTY_APP:
                return (appUidMap_);
            default:
                APP_LOGE("app type error");
                return (appUidMap_);
        }
    }();
    for (auto &kv : innerMap) {
        if (kv.second == info.GetBundleName()) {
            innerMap.erase(kv.first);
            return true;
        }
    }
    return true;
}

bool BundleDataMgr::RestoreUidAndGid()
{
    // this function should be called with bundleInfoMutex_ locked
    for (const auto &item : bundleInfos_) {
        for (const auto &info : item.second) {
            uint32_t uid = info.second.GetUid();
            if ((uid < Constants::BASE_SYS_VEN_UID) && (uid >= Constants::BASE_SYS_UID)) {
                sysUidMap_[uid - Constants::BASE_SYS_UID] = info.second.GetBundleName();
            } else if ((uid >= Constants::BASE_SYS_VEN_UID) && (uid <= Constants::MAX_SYS_VEN_UID)) {
                sysVendorUidMap_[uid - Constants::BASE_SYS_VEN_UID] = info.second.GetBundleName();
            } else if (uid > Constants::MAX_SYS_VEN_UID) {
                appUidMap_[uid - Constants::BASE_APP_UID] = info.second.GetBundleName();
            }
        }
    }
    return true;
}

bool BundleDataMgr::NotifyBundleStatus(const std::string &bundleName, const std::string &modulePackage,
    const std::string &mainAbility, const ErrCode resultCode, const NotifyType type)
{
    APP_LOGI("notify type %{public}d with %{public}d for %{public}s-%{public}s in %{public}s",
        type,
        resultCode,
        modulePackage.c_str(),
        mainAbility.c_str(),
        bundleName.c_str());
    uint8_t installType = [&]() -> uint8_t {
        if ((type == NotifyType::UNINSTALL_BUNDLE) || (type == NotifyType::UNINSTALL_MODULE)) {
            return static_cast<uint8_t>(InstallType::UNINSTALL_CALLBACK);
        }
        return static_cast<uint8_t>(InstallType::INSTALL_CALLBACK);
    }();
    {
        std::lock_guard<std::mutex> lock(callbackMutex_);
        for (const auto &callback : callbackList_) {
            if (callback->GetBundleName() == bundleName) {
                // if the msg needed, it could convert in the proxy node
                callback->OnBundleStateChanged(installType, resultCode, Constants::EMPTY_STRING, bundleName);
            }
        }
    }

    if (resultCode != ERR_OK) {
        return true;
    }
    std::string eventData = [type]() -> std::string {
        switch (type) {
            case NotifyType::INSTALL:
                return EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED;
            case NotifyType::UNINSTALL_BUNDLE:
                return EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED;
            case NotifyType::UNINSTALL_MODULE:
                return EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED;
            case NotifyType::UPDATE:
                return EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED;
            default:
                APP_LOGE("event type error");
                return EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED;
        }
    }();
    APP_LOGI("will send event data %{public}s", eventData.c_str());
    Want want;
    want.SetAction(eventData);
    ElementName element;
    element.SetBundleName(bundleName);
    element.SetAbilityName(mainAbility);
    want.SetElement(element);
    EventFwk::CommonEventData commonData{want};
    EventFwk::CommonEventManager::PublishCommonEvent(commonData);
    return true;
}

bool BundleDataMgr::GetProvisionId(const std::string &bundleName, std::string &provisionId) const
{
    APP_LOGI("GetProvisionId %{public}s", bundleName.c_str());
    if (bundleName.empty()) {
        APP_LOGE("bundleName empty");
        return false;
    }
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem == bundleInfos_.end()) {
        APP_LOGE("can not find bundle %{public}s", bundleName.c_str());
        return false;
    }
    auto infoWithIdItem = infoItem->second.find(Constants::CURRENT_DEVICE_ID);
    if (infoWithIdItem == infoItem->second.end()) {
        APP_LOGE("bundle:%{public}s device id not find", bundleName.c_str());
        return false;
    }
    provisionId = infoWithIdItem->second.GetProvisionId();
    return true;
}

bool BundleDataMgr::GetAppFeature(const std::string &bundleName, std::string &appFeature) const
{
    APP_LOGI("GetAppFeature %{public}s", bundleName.c_str());
    if (bundleName.empty()) {
        APP_LOGE("bundleName empty");
        return false;
    }
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem == bundleInfos_.end()) {
        APP_LOGE("can not find bundle %{public}s", bundleName.c_str());
        return false;
    }
    auto infoWithIdItem = infoItem->second.find(Constants::CURRENT_DEVICE_ID);
    if (infoWithIdItem == infoItem->second.end()) {
        APP_LOGE("bundle:%{public}s device id not find", bundleName.c_str());
        return false;
    }
    appFeature = infoWithIdItem->second.GetAppFeature();
    return true;
}

void BundleDataMgr::SetAllInstallFlag(bool flag)
{
    APP_LOGI("SetAllInstallFlag %{public}d", flag);
    allInstallFlag_ = flag;
}

}  // namespace AppExecFwk
}  // namespace OHOS
