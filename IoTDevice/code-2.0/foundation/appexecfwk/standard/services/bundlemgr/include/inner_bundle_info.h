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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_INNER_BUNDLE_INFO_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_INNER_BUNDLE_INFO_H

#include "nocopyable.h"
#include "permission/permission_kit.h"

#include "ability_info.h"
#include "bundle_info.h"
#include "hap_module_info.h"
#include "bundle_constants.h"
#include "json_serializer.h"
#include "common_profile.h"

namespace OHOS {
namespace AppExecFwk {

struct Parameters {
    std::string description;
    std::string name;
    std::string type;
};

struct Results {
    std::string description;
    std::string name;
    std::string type;
};

struct CustomizeData {
    std::string name;
    std::string value;
    std::string extra;
};

struct MetaData {
    std::vector<Parameters> parameters;
    std::vector<Results> results;
    std::vector<CustomizeData> customizeData;
};

struct Distro {
    bool deliveryWithInstall;
    std::string moduleName;
    std::string moduleType;
};

struct DefPermission {
    std::string name;
    std::string grantMode;
    std::vector<std::string> availableScope;
    std::string label;
    int32_t labelId;
    std::string description;
    int32_t descriptionId;
};

struct UsedScene {
    std::vector<std::string> ability;
    std::string when;
};

struct ReqPermission {
    std::string name;
    std::string reason;
    UsedScene usedScene;
};

struct InnerModuleInfo {
    std::string modulePackage;
    std::string moduleName;
    std::string modulePath;
    std::string moduleDataDir;
    std::string moduleResPath;
    std::string description;
    bool isEntry;
    MetaData metaData;
    Distro distro;
    std::vector<std::string> reqCapabilities;
    std::vector<ReqPermission> reqPermissions;
    std::vector<DefPermission> defPermissions;
    std::vector<std::string> abilityKeys;
    std::vector<std::string> skillKeys;
};

struct SkillUri {
    std::string scheme;
    std::string host;
    std::string port;
    std::string path;
    std::string type;
};

struct Skill {
    std::vector<std::string> actions;
    std::vector<std::string> entities;
    std::vector<SkillUri> uris;
};

class InnerBundleInfo {
public:
    enum class BundleStatus {
        ENABLED = 1,
        DISABLED,
    };

    InnerBundleInfo();
    ~InnerBundleInfo();
    /**
     * @brief Transform the InnerBundleInfo object to json.
     * @param jsonObject Indicates the obtained json object.
     * @return
     */
    void ToJson(nlohmann::json &jsonObject) const;
    /**
     * @brief Transform the json object to InnerBundleInfo object.
     * @param jsonObject Indicates the obtained json object.
     * @return
     */
    bool FromJson(const nlohmann::json &jsonObject);
    /**
     * @brief Add module info to old InnerBundleInfo object.
     * @param newInfo Indicates the new InnerBundleInfo object.
     * @return  Returns true if the module successfully added; returns false otherwise.
     */
    bool AddModuleInfo(const InnerBundleInfo &newInfo);
    /**
     * @brief Update module info to old InnerBundleInfo object.
     * @param newInfo Indicates the new InnerBundleInfo object.
     * @return
     */
    void UpdateModuleInfo(const InnerBundleInfo &newInfo);
    /**
     * @brief Update version info to old InnerBundleInfo object.
     * @param newInfo Indicates the new InnerBundleInfo object.
     * @return
     */
    void UpdateVersionInfo(const InnerBundleInfo &newInfo);
    /**
     * @brief Remove module info from InnerBundleInfo object.
     * @param modulePackage Indicates the module package to be remove.
     * @return
     */
    void RemoveModuleInfo(const std::string &modulePackage);
    /**
     * @brief Find hap module info by module package.
     * @param modulePackage Indicates the module package.
     * @return Returns the HapModuleInfo object if find it; returns null otherwise.
     */
    std::optional<HapModuleInfo> FindHapModuleInfo(const std::string &modulePackage) const;
    /**
     * @brief Find skills by keyName.
     * @param keyName Indicates the keyName.
     * @return Returns the skills object if find it; returns null otherwise.
     */
    std::optional<std::vector<Skill>> FindSkills(const std::string &keyName) const;
    /**
     * @brief Find abilityInfo by bundle name and ability name.
     * @param bundleName Indicates the bundle name.
     * @param abilityName Indicates the ability name
     * @return Returns the AbilityInfo object if find it; returns null otherwise.
     */
    std::optional<AbilityInfo> FindAbilityInfo(const std::string &bundleName, const std::string &abilityName) const;
    /**
     * @brief Transform the InnerBundleInfo object to string.
     * @return Returns the string object
     */
    std::string ToString() const;
    /**
     * @brief Add ability infos to old InnerBundleInfo object.
     * @param abilityInfos Indicates the AbilityInfo object to be add.
     * @return
     */
    void AddModuleAbilityInfo(const std::map<std::string, AbilityInfo> &abilityInfos)
    {
        for (const auto &ability : abilityInfos) {
            baseAbilityInfos_.try_emplace(ability.first, ability.second);
        }
    }
    /**
     * @brief Add skill infos to old InnerBundleInfo object.
     * @param skillInfos Indicates the Skill object to be add.
     * @return
     */
    void AddModuleSkillInfo(const std::map<std::string, std::vector<Skill>> &skillInfos)
    {
        for (const auto &skills : skillInfos) {
            skillInfos_.try_emplace(skills.first, skills.second);
        }
    }
    /**
     * @brief Add innerModuleInfos to old InnerBundleInfo object.
     * @param innerModuleInfos Indicates the InnerModuleInfo object to be add.
     * @return
     */
    void AddInnerModuleInfo(const std::map<std::string, InnerModuleInfo> &innerModuleInfos)
    {
        for (const auto &info : innerModuleInfos) {
            innerModuleInfos_.try_emplace(info.first, info.second);
        }
    }
    /**
     * @brief Get application name.
     * @return Return application name
     */
    std::string GetApplicationName() const
    {
        return baseApplicationInfo_.name;
    }
    /**
     * @brief Set bundle status.
     * @param status Indicates the BundleStatus object to set.
     * @return
     */
    void SetBundleStatus(const BundleStatus &status)
    {
        bundleStatus_ = status;
    }
    /**
     * @brief Get bundle status.
     * @return Return the BundleStatus object
     */
    BundleStatus GetBundleStatus() const
    {
        return bundleStatus_;
    }
    /**
     * @brief Set bundle install time.
     * @param time Indicates the install time to set.
     * @return
     */
    void SetBundleInstallTime(const int64_t time)
    {
        baseBundleInfo_.installTime = time;
        baseBundleInfo_.updateTime = time;
    }
    /**
     * @brief Get bundle install time.
     * @return Return the bundle install time.
     */
    int64_t GetBundleInstallTime() const
    {
        return baseBundleInfo_.installTime;
    }
    /**
     * @brief Set bundle update time.
     * @param time Indicates the update time to set.
     * @return
     */
    void SetBundleUpdateTime(const int64_t time)
    {
        baseBundleInfo_.updateTime = time;
    }
    /**
     * @brief Get bundle update time.
     * @return Return the bundle update time.
     */
    int64_t GetBundleUpdateTime() const
    {
        return baseBundleInfo_.updateTime;
    }
    /**
     * @brief Set whether the application supports backup.
     * @param isSupportBackup Indicates the supports status to set.
     */
    void SetIsSupportBackup(bool isSupportBackup)
    {
        isSupportBackup_ = isSupportBackup;
    }
    /**
     * @brief Get whether the application supports backup.
     * @return Return the supports status.
     */
    bool GetIsSupportBackup() const
    {
        return isSupportBackup_;
    }
    /**
     * @brief Get bundle name.
     * @return Return bundle name
     */
    std::string GetBundleName() const
    {
        return baseApplicationInfo_.bundleName;
    }
    /**
     * @brief Set baseBundleInfo.
     * @param bundleInfo Indicates the BundleInfo object.
     */
    void SetBaseBundleInfo(const BundleInfo &bundleInfo)
    {
        baseBundleInfo_ = bundleInfo;
    }
    /**
     * @brief Get baseBundleInfo.
     * @return Return the BundleInfo object.
     */
    BundleInfo GetBaseBundleInfo() const
    {
        return baseBundleInfo_;
    }
    /**
     * @brief Set baseApplicationInfo.
     * @param applicationInfo Indicates the ApplicationInfo object.
     */
    void SetBaseApplicationInfo(const ApplicationInfo &applicationInfo)
    {
        baseApplicationInfo_ = applicationInfo;
    }
    /**
     * @brief Update baseApplicationInfo.
     * @param applicationInfo Indicates the ApplicationInfo object.
     */
    void UpdateBaseApplicationInfo(const ApplicationInfo &applicationInfo)
    {
        baseApplicationInfo_.label = applicationInfo.label;
        baseApplicationInfo_.labelId = applicationInfo.labelId;
        baseApplicationInfo_.iconPath = applicationInfo.iconPath;
        baseApplicationInfo_.iconId = applicationInfo.iconId;
        baseApplicationInfo_.description = applicationInfo.description;
        baseApplicationInfo_.descriptionId = applicationInfo.descriptionId;
        if (!baseApplicationInfo_.isLauncherApp) {
            baseApplicationInfo_.isLauncherApp = applicationInfo.isLauncherApp;
        }
    }
    /**
     * @brief Get baseApplicationInfo.
     * @return Return the ApplicationInfo object.
     */
    ApplicationInfo GetBaseApplicationInfo() const
    {
        return baseApplicationInfo_;
    }
    /**
     * @brief Get application code path.
     * @return Return the string object.
     */
    std::string GetAppCodePath() const
    {
        return baseApplicationInfo_.codePath;
    }
    /**
     * @brief Set application code path.
     * @param codePath Indicates the code path to be set.
     */
    void SetAppCodePath(std::string codePath)
    {
        baseApplicationInfo_.codePath = codePath;
    }
    /**
     * @brief Insert innerModuleInfos.
     * @param modulePackage Indicates the modulePackage object as key.
     * @param innerModuleInfo Indicates the InnerModuleInfo object as value.
     */
    void InsertInnerModuleInfo(const std::string &modulePackage, const InnerModuleInfo &innerModuleInfo)
    {
        innerModuleInfos_.try_emplace(modulePackage, innerModuleInfo);
    }
    /**
     * @brief Insert baseAbilityInfos.
     * @param keyName Indicates the key.
     * @param abilityInfo Indicates the AbilityInfo object as value.
     */
    void InsertAbilitiesInfo(const std::string &keyName, const AbilityInfo &abilityInfo)
    {
        baseAbilityInfos_.emplace(keyName, abilityInfo);
    }
    /**
     * @brief Insert skillInfos.
     * @param keyName Indicates the abilityName as key.
     * @param abilityInfo Indicates the Skills object as value.
     */
    void InsertSkillInfo(const std::string &abilityName, const std::vector<Skill> &skills)
    {
        skillInfos_.emplace(abilityName, skills);
    }
    /**
     * @brief Find AbilityInfo object by Uri.
     * @param abilityUri Indicates the ability uri.
     * @return Returns the AbilityInfo object if find it; returns null otherwise.
     */
    std::optional<AbilityInfo> FindAbilityInfoByUri(const std::string &abilityUri) const
    {
        for (const auto &ability : baseAbilityInfos_) {
            if (ability.second.uri == abilityUri) {
                return ability.second;
            }
        }
        return std::nullopt;
    }
    /**
     * @brief Get all ability names in application.
     * @return Returns ability names.
     */
    auto GetAbilityNames() const
    {
        std::vector<std::string> abilityNames;
        for (auto &ability : baseAbilityInfos_) {
            abilityNames.emplace_back(ability.second.name);
        }
        return abilityNames;
    }
    /**
     * @brief Get all skill keys in application.
     * @return Returns skill keys.
     */
    auto GetSkillKeys() const
    {
        std::vector<std::string> skillKeys;
        for (auto &skill : skillInfos_) {
            skillKeys.emplace_back(skill.first);
        }
        return skillKeys;
    }
    /**
     * @brief Get version code in application.
     * @return Returns version code.
     */
    uint32_t GetVersionCode() const
    {
        return baseBundleInfo_.versionCode;
    }
    /**
     * @brief Get signature key in application.
     * @return Returns signature key.
     */
    std::string GetSignatureKey() const
    {
        return baseApplicationInfo_.signatureKey;
    }
    /**
     * @brief Set application base data dir.
     * @param baseDataDir Indicates the dir to be set.
     */
    void SetBaseDataDir(std::string baseDataDir)
    {
        baseDataDir_ = baseDataDir;
    }
    /**
     * @brief Get application base data dir.
     * @return Return the string object.
     */
    std::string GetBaseDataDir() const
    {
        return baseDataDir_;
    }
    /**
     * @brief Get application data dir.
     * @return Return the string object.
     */
    std::string GetAppDataDir() const
    {
        return baseApplicationInfo_.dataDir;
    }
    /**
     * @brief Set application data dir.
     * @param dataDir Indicates the data Dir to be set.
     */
    void SetAppDataDir(std::string dataDir)
    {
        baseApplicationInfo_.dataDir = dataDir;
    }
    /**
     * @brief Set application data base dir.
     * @param dataBaseDir Indicates the data base Dir to be set.
     */
    void SetAppDataBaseDir(std::string dataBaseDir)
    {
        baseApplicationInfo_.dataBaseDir = dataBaseDir;
    }
    /**
     * @brief Set application cache dir.
     * @param cacheDir Indicates the cache Dir to be set.
     */
    void SetAppCacheDir(std::string cacheDir)
    {
        baseApplicationInfo_.cacheDir = cacheDir;
    }
    /**
     * @brief Set application uid.
     * @param uid Indicates the uid to be set.
     */
    void SetUid(int uid)
    {
        uid_ = uid;
        baseBundleInfo_.uid = uid;
    }
    /**
     * @brief Get application uid.
     * @return Returns the uid.
     */
    int GetUid() const
    {
        return uid_;
    }
    /**
     * @brief Get application gid.
     * @return Returns the gid.
     */
    int GetGid() const
    {
        return gid_;
    }
    /**
     * @brief Set application gid.
     * @param gid Indicates the gid to be set.
     */
    void SetGid(int gid)
    {
        gid_ = gid;
        baseBundleInfo_.gid = gid;
    }
    /**
     * @brief Get application AppType.
     * @return Returns the AppType.
     */
    Constants::AppType GetAppType() const
    {
        return appType_;
    }
    /**
     * @brief Set application AppType.
     * @param gid Indicates the AppType to be set.
     */
    void SetAppType(Constants::AppType appType)
    {
        appType_ = appType;
    }
    /**
     * @brief Get application user id.
     * @return Returns the user id.
     */
    int GetUserId() const
    {
        return userId_;
    }
    /**
     * @brief Set application user id.
     * @param gid Indicates the user id to be set.
     */
    void SetUserId(int userId)
    {
        userId_ = userId;
    }

    // only used in install progress with newInfo
    std::string GetCurrentModulePackage() const
    {
        return currentPackage_;
    }
    void SetCurrentModulePackage(const std::string &modulePackage)
    {
        currentPackage_ = modulePackage;
    }
    void AddModuleSrcDir(const std::string &moduleSrcDir)
    {
        if (innerModuleInfos_.count(currentPackage_) == 1) {
            innerModuleInfos_.at(currentPackage_).modulePath = moduleSrcDir;
        }
    }
    void AddModuleDataDir(const std::string &moduleDataDir)
    {
        if (innerModuleInfos_.count(currentPackage_) == 1) {
            innerModuleInfos_.at(currentPackage_).moduleDataDir = moduleDataDir;
        }
    }
    void AddModuleResPath(const std::string &moduleSrcDir)
    {
        if (innerModuleInfos_.count(currentPackage_) == 1) {
            std::string moduleResPath = moduleSrcDir + Constants::PATH_SEPARATOR + Constants::ASSETS_DIR +
                                        Constants::PATH_SEPARATOR +
                                        innerModuleInfos_.at(currentPackage_).distro.moduleName +
                                        Constants::PATH_SEPARATOR + Constants::RESOURCES_INDEX;
            innerModuleInfos_.at(currentPackage_).moduleResPath = moduleResPath;
            for (auto &abilityInfo : baseAbilityInfos_) {
                abilityInfo.second.resourcePath = moduleResPath;
            }
        }
    }
    std::vector<DefPermission> GetDefPermissions() const
    {
        std::vector<DefPermission> defPermissions;
        if (innerModuleInfos_.count(currentPackage_) == 1) {
            defPermissions = innerModuleInfos_.at(currentPackage_).defPermissions;
        }
        return defPermissions;
    }

    std::vector<ReqPermission> GetReqPermissions() const
    {
        std::vector<ReqPermission> reqPermissions;
        if (innerModuleInfos_.count(currentPackage_) == 1) {
            reqPermissions = innerModuleInfos_.at(currentPackage_).reqPermissions;
        }
        return reqPermissions;
    }

    bool FindModule(std::string modulePackage) const
    {
        return (innerModuleInfos_.find(modulePackage) != innerModuleInfos_.end());
    }

    void SetIsKeepData(bool isKeepData)
    {
        isKeepData_ = isKeepData;
    }

    bool GetIsKeepData() const
    {
        return isKeepData_;
    }

    void SetIsKeepAlive(bool isKeepAlive)
    {
        baseBundleInfo_.isKeepAlive = isKeepAlive;
    }

    bool GetIsKeepAlive() const
    {
        return baseBundleInfo_.isKeepAlive;
    }

    void SetIsNativeApp(bool isNativeApp)
    {
        baseBundleInfo_.isNativeApp = isNativeApp;
    }

    bool GetIsNativeApp() const
    {
        return baseBundleInfo_.isNativeApp;
    }

    void SetIsLauncherApp(bool isLauncher)
    {
        baseApplicationInfo_.isLauncherApp = isLauncher;
    }

    bool GetIsLauncherApp() const
    {
        return baseApplicationInfo_.isLauncherApp;
    }

    void SetMainAbility(const std::string &mainAbility)
    {
        mainAbility_ = mainAbility;
    }

    std::string GetMainAbility() const
    {
        return mainAbility_;
    }

    void SetMainAbilityName(const std::string &mainAbilityName)
    {
        mainAbilityName_ = mainAbilityName;
    }

    std::string GetMainAbilityName() const
    {
        return mainAbilityName_;
    }

    void GetMainAbilityInfo(AbilityInfo &abilityInfo) const
    {
        if (!mainAbility_.empty()) {
            abilityInfo = baseAbilityInfos_.at(mainAbility_);
        }
    }

    std::string GetModuleDir(std::string modulePackage) const
    {
        if (innerModuleInfos_.find(modulePackage) != innerModuleInfos_.end()) {
            return innerModuleInfos_.at(modulePackage).modulePath;
        }
        return Constants::EMPTY_STRING;
    }

    std::string GetModuleDataDir(std::string modulePackage) const
    {
        if (innerModuleInfos_.find(modulePackage) != innerModuleInfos_.end()) {
            return innerModuleInfos_.at(modulePackage).moduleDataDir;
        }
        return Constants::EMPTY_STRING;
    }

    bool IsDisabled() const
    {
        return (bundleStatus_ == BundleStatus::DISABLED);
    }

    void SetSeInfo(const std::string &seInfo)
    {
        baseBundleInfo_.seInfo = seInfo;
    }

    std::string GetSeInfo() const
    {
        return baseBundleInfo_.seInfo;
    }

    bool IsOnlyModule(const std::string &modulePackage)
    {
        if ((innerModuleInfos_.size() == 1) && (innerModuleInfos_.count(modulePackage) == 1)) {
            return true;
        }
        return false;
    }

    void SetProvisionId(const std::string &provisionId)
    {
        provisionId_ = provisionId;
    }

    std::string GetProvisionId() const
    {
        return provisionId_;
    }

    void SetAppFeature(const std::string &appFeature)
    {
        appFeature_ = appFeature;
    }

    std::string GetAppFeature() const
    {
        return appFeature_;
    }

    void SetHasEntry(bool hasEntry)
    {
        hasEntry_ = hasEntry;
    }

    bool HasEntry() const
    {
        return hasEntry_;
    }

    // use for new Info in updating progress
    void RestoreFromOldInfo(const InnerBundleInfo &oldInfo)
    {
        SetAppCodePath(oldInfo.GetAppCodePath());
        SetBaseDataDir(oldInfo.GetBaseDataDir());
        SetUid(oldInfo.GetUid());
        SetGid(oldInfo.GetGid());
    }
    void RestoreModuleInfo(const InnerBundleInfo &oldInfo)
    {
        if (oldInfo.FindModule(currentPackage_)) {
            innerModuleInfos_.at(currentPackage_).moduleDataDir = oldInfo.GetModuleDataDir(currentPackage_);
        }
    }
    /**
     * @brief Obtains configuration information about an application.
     * @param flag Indicates the flag used to specify information contained
     *             in the ApplicationInfo object that will be returned.
     * @param userId Indicates the user ID.
     * @param appInfo Indicates the obtained ApplicationInfo object.
     */
    void GetApplicationInfo(const ApplicationFlag flag, const int userId, ApplicationInfo &appInfo) const;
    /**
     * @brief Obtains configuration information about an bundle.
     * @param flag Indicates the flag used to specify information contained in the BundleInfo that will be returned.
     * @param bundleInfos Indicates all of the obtained BundleInfo objects.
     */
    void GetBundleInfo(const BundleFlag flag, BundleInfo &bundleInfo) const;
    /**
     * @brief Check if special metadata is in the application.
     * @param metaData Indicates the special metaData.
     * @param bundleInfos Returns true if the metadata in application; returns false otherwise.
     */
    bool CheckSpecialMetaData(const std::string &metaData) const;

private:
    // using for get
    bool isSupportBackup_ = false;
    bool isKeepData_ = false;
    Constants::AppType appType_ = Constants::AppType::THIRD_PARTY_APP;
    int uid_ = Constants::INVALID_UID;
    int gid_ = Constants::INVALID_GID;
    int userId_ = Constants::DEFAULT_USERID;
    std::string baseDataDir_;
    BundleStatus bundleStatus_ = BundleStatus::ENABLED;
    ApplicationInfo baseApplicationInfo_;
    BundleInfo baseBundleInfo_;  // applicationInfo and abilityInfo empty
    std::string mainAbility_;
    std::string provisionId_;
    std::string appFeature_;
    bool hasEntry_ = false;
    // only using for install or update progress, doesn't need to save to database
    std::string currentPackage_;
    std::string mainAbilityName_;

    std::map<std::string, AbilityInfo> baseAbilityInfos_;
    std::map<std::string, InnerModuleInfo> innerModuleInfos_;
    std::map<std::string, std::vector<Skill>> skillInfos_;
};

void from_json(const nlohmann::json &jsonObject, InnerModuleInfo &info);
void from_json(const nlohmann::json &jsonObject, SkillUri &uri);
void from_json(const nlohmann::json &jsonObject, Skill &skill);
void from_json(const nlohmann::json &jsonObject, CustomizeData &customizeData);
void from_json(const nlohmann::json &jsonObject, Parameters &parameters);
void from_json(const nlohmann::json &jsonObject, Results &results);
void from_json(const nlohmann::json &jsonObject, MetaData &metaData);
void from_json(const nlohmann::json &jsonObject, Distro &distro);
void from_json(const nlohmann::json &jsonObject, ReqPermission &ReqPermission);
void from_json(const nlohmann::json &jsonObject, DefPermission &DefPermission);
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_INNER_BUNDLE_INFO_H
