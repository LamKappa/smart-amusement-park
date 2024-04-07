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

#include "inner_bundle_info.h"

#include "app_log_wrapper.h"
#include "common_profile.h"

namespace OHOS {
namespace AppExecFwk {
namespace {

const std::string IS_SUPPORT_BACKUP = "isSupportBackup";
const std::string APP_TYPE = "appType";
const std::string UID = "uid";
const std::string GID = "gid";
const std::string BASE_DATA_DIR = "baseDataDir";
const std::string BUNDLE_STATUS = "bundleStatus";
const std::string BASE_APPLICATION_INFO = "baseApplicationInfo";
const std::string BASE_BUNDLE_INFO = "baseBundleInfo";
const std::string BASE_ABILITY_INFO = "baseAbilityInfos";
const std::string INNER_MODULE_INFO = "innerModuleInfos";
const std::string MAIN_ABILITY = "mainAbility";
const std::string SKILL_INFOS = "skillInfos";
const std::string USER_ID = "userId_";
const std::string IS_KEEP_DATA = "isKeepData";
const std::string PROVISION_ID = "provisionId";
const std::string APP_FEATURE = "appFeature";
const std::string HAS_ENTRY = "hasEntry";
const std::string MODULE_PACKAGE = "modulePackage";
const std::string MODULE_PATH = "modulePath";
const std::string MODULE_NAME = "moduleName";
const std::string MODULE_DESCRIPTION = "description";
const std::string MODULE_IS_ENTRY = "isEntry";
const std::string MODULE_METADATA = "metaData";
const std::string MODULE_DISTRO = "distro";
const std::string MODULE_REQ_CAPABILITIES = "reqCapabilities";
const std::string MODULE_REQ_PERMS = "reqPermissions";
const std::string MODULE_DEF_PERMS = "defPermissions";
const std::string MODULE_DATA_DIR = "moduleDataDir";
const std::string MODULE_RES_PATH = "moduleResPath";
const std::string MODULE_ABILITY_KEYS = "abilityKeys";
const std::string MODULE_SKILL_KEYS = "skillKeys";

}  // namespace

InnerBundleInfo::InnerBundleInfo()
{
    APP_LOGD("inner bundle info instance is created");
}

InnerBundleInfo::~InnerBundleInfo()
{
    APP_LOGD("inner bundle info instance is destroyed");
}

void to_json(nlohmann::json &jsonObject, const CustomizeData &customizeData)
{
    jsonObject = nlohmann::json{
        {ProfileReader::PROFILE_KEY_NAME, customizeData.name},
        {ProfileReader::BUNDLE_MODULE_META_KEY_EXTRA, customizeData.extra},
        {ProfileReader::BUNDLE_MODULE_META_KEY_VALUE, customizeData.value}
    };
}

void to_json(nlohmann::json &jsonObject, const Parameters &parameters)
{
    jsonObject = nlohmann::json{
        {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_TYPE, parameters.type},
        {ProfileReader::PROFILE_KEY_DESCRIPTION, parameters.description},
        {ProfileReader::PROFILE_KEY_NAME, parameters.name}
    };
}

void to_json(nlohmann::json &jsonObject, const Results &results)
{
    jsonObject = nlohmann::json{
        {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_TYPE, results.type},
        {ProfileReader::PROFILE_KEY_DESCRIPTION, results.description},
        {ProfileReader::PROFILE_KEY_NAME, results.name}
    };
}

void to_json(nlohmann::json &jsonObject, const MetaData &metaData)
{
    jsonObject = nlohmann::json{
        {ProfileReader::BUNDLE_MODULE_META_KEY_CUSTOMIZE_DATA, metaData.customizeData},
        {ProfileReader::BUNDLE_MODULE_META_KEY_PARAMETERS, metaData.parameters},
        {ProfileReader::BUNDLE_MODULE_META_KEY_RESULTS, metaData.results}
    };
}

void to_json(nlohmann::json &jsonObject, const Distro &distro)
{
    jsonObject = nlohmann::json{
            {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_DELIVERY_WITH_INSTALL, distro.deliveryWithInstall},
            {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_MODULE_NAME, distro.moduleName},
            {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_MODULE_TYPE, distro.moduleType}
            };
}

void to_json(nlohmann::json &jsonObject, const UsedScene &usedScene)
{
    jsonObject = nlohmann::json{
        {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS_ABILITY, usedScene.ability},
        {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS_WHEN, usedScene.when}
    };
}

void to_json(nlohmann::json &jsonObject, const ReqPermission &reqPermission)
{
    jsonObject = nlohmann::json{
        {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS_NAME, reqPermission.name},
        {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS_REASON, reqPermission.reason},
        {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS_USEDSCENE, reqPermission.usedScene}
    };
}

void to_json(nlohmann::json &jsonObject, const DefPermission &defPermission)
{
    jsonObject = nlohmann::json{
        {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_NAME, defPermission.name},
        {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_GRANTMODE, defPermission.grantMode},
        {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_AVAILABLESCOPE, defPermission.availableScope},
        {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_LABEL, defPermission.label},
        {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_LABEL_ID, defPermission.labelId},
        {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_DESCRIPTION, defPermission.description},
        {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_DESCRIPTION_ID, defPermission.descriptionId}
    };
}

void to_json(nlohmann::json &jsonObject, const InnerModuleInfo &info)
{
    jsonObject = nlohmann::json{
        {MODULE_PACKAGE, info.modulePackage},
        {MODULE_NAME, info.moduleName},
        {MODULE_PATH, info.modulePath},
        {MODULE_DATA_DIR, info.moduleDataDir},
        {MODULE_RES_PATH, info.moduleResPath},
        {MODULE_IS_ENTRY, info.isEntry},
        {MODULE_METADATA, info.metaData},
        {MODULE_DISTRO, info.distro},
        {MODULE_DESCRIPTION, info.description},
        {MODULE_REQ_CAPABILITIES, info.reqCapabilities},
        {MODULE_REQ_PERMS, info.reqPermissions},
        {MODULE_DEF_PERMS, info.defPermissions},
        {MODULE_ABILITY_KEYS, info.abilityKeys},
        {MODULE_SKILL_KEYS, info.skillKeys}
    };
}

void to_json(nlohmann::json &jsonObject, const SkillUri &uri)
{
    jsonObject = nlohmann::json{
        {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_SCHEME, uri.scheme},
        {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_HOST, uri.host},
        {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_PORT, uri.port},
        {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_PATH, uri.path},
        {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_TYPE, uri.type}
    };
}

void to_json(nlohmann::json &jsonObject, const Skill &skill)
{
    jsonObject = nlohmann::json{
        {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_ACTIONS, skill.actions},
        {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_ENTITIES, skill.entities},
        {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_URIS, skill.uris}
    };
}

void InnerBundleInfo::ToJson(nlohmann::json &jsonObject) const
{
    jsonObject[IS_SUPPORT_BACKUP] = isSupportBackup_;
    jsonObject[APP_TYPE] = appType_;
    jsonObject[UID] = uid_;
    jsonObject[GID] = gid_;
    jsonObject[BASE_DATA_DIR] = baseDataDir_;
    jsonObject[BUNDLE_STATUS] = bundleStatus_;
    jsonObject[BASE_APPLICATION_INFO] = baseApplicationInfo_;
    jsonObject[BASE_BUNDLE_INFO] = baseBundleInfo_;
    jsonObject[BASE_ABILITY_INFO] = baseAbilityInfos_;
    jsonObject[INNER_MODULE_INFO] = innerModuleInfos_;
    jsonObject[SKILL_INFOS] = skillInfos_;
    jsonObject[IS_KEEP_DATA] = isKeepData_;
    jsonObject[USER_ID] = userId_;
    jsonObject[MAIN_ABILITY] = mainAbility_;
    jsonObject[PROVISION_ID] = provisionId_;
    jsonObject[APP_FEATURE] = appFeature_;
    jsonObject[HAS_ENTRY] = hasEntry_;
}

void from_json(const nlohmann::json &jsonObject, InnerModuleInfo &info)
{
    // these are not required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(MODULE_PACKAGE) != jsonObjectEnd) {
        info.modulePackage = jsonObject.at(MODULE_PACKAGE).get<std::string>();
    }
    if (jsonObject.find(MODULE_NAME) != jsonObjectEnd) {
        info.moduleName = jsonObject.at(MODULE_NAME).get<std::string>();
    }
    if (jsonObject.find(MODULE_PATH) != jsonObjectEnd) {
        info.modulePath = jsonObject.at(MODULE_PATH).get<std::string>();
    }
    if (jsonObject.find(MODULE_DATA_DIR) != jsonObjectEnd) {
        info.moduleDataDir = jsonObject.at(MODULE_DATA_DIR).get<std::string>();
    }
    if (jsonObject.find(MODULE_RES_PATH) != jsonObjectEnd) {
        info.moduleResPath = jsonObject.at(MODULE_RES_PATH).get<std::string>();
    }
    if (jsonObject.find(MODULE_IS_ENTRY) != jsonObjectEnd) {
        info.isEntry = jsonObject.at(MODULE_IS_ENTRY).get<bool>();
    }
    if (jsonObject.find(MODULE_METADATA) != jsonObjectEnd) {
        info.metaData = jsonObject.at(MODULE_METADATA).get<MetaData>();
    }
    if (jsonObject.find(MODULE_DISTRO) != jsonObjectEnd) {
        info.distro = jsonObject.at(MODULE_DISTRO).get<Distro>();
    }
    if (jsonObject.find(MODULE_DESCRIPTION) != jsonObjectEnd) {
        info.description = jsonObject.at(MODULE_DESCRIPTION).get<std::string>();
    }
    if (jsonObject.find(MODULE_REQ_CAPABILITIES) != jsonObjectEnd) {
        info.reqCapabilities = jsonObject.at(MODULE_REQ_CAPABILITIES).get<std::vector<std::string>>();
    }
    if (jsonObject.find(MODULE_REQ_PERMS) != jsonObjectEnd) {
        info.reqPermissions = jsonObject.at(MODULE_REQ_PERMS).get<std::vector<ReqPermission>>();
    }
    if (jsonObject.find(MODULE_DEF_PERMS) != jsonObjectEnd) {
        info.defPermissions = jsonObject.at(MODULE_DEF_PERMS).get<std::vector<DefPermission>>();
    }
    if (jsonObject.find(MODULE_ABILITY_KEYS) != jsonObjectEnd) {
        info.abilityKeys = jsonObject.at(MODULE_ABILITY_KEYS).get<std::vector<std::string>>();
    }
    if (jsonObject.find(MODULE_SKILL_KEYS) != jsonObjectEnd) {
        info.skillKeys = jsonObject.at(MODULE_SKILL_KEYS).get<std::vector<std::string>>();
    }
}

void from_json(const nlohmann::json &jsonObject, SkillUri &uri)
{
    // these are required fields.
    uri.scheme = jsonObject.at(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_SCHEME).get<std::string>();
    // these are not required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_HOST) != jsonObjectEnd) {
        uri.host = jsonObject.at(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_HOST).get<std::string>();
    }

    if (jsonObject.find(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_PORT) != jsonObjectEnd) {
        uri.port = jsonObject.at(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_PORT).get<std::string>();
    }

    if (jsonObject.find(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_PATH) != jsonObjectEnd) {
        uri.path = jsonObject.at(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_PATH).get<std::string>();
    }

    if (jsonObject.find(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_TYPE) != jsonObjectEnd) {
        uri.type = jsonObject.at(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_TYPE).get<std::string>();
    }
}

void from_json(const nlohmann::json &jsonObject, Skill &skill)
{
    // these are not required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_ACTIONS) != jsonObjectEnd) {
        skill.actions = jsonObject.at(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_ACTIONS).get<std::vector<std::string>>();
    }

    if (jsonObject.find(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_ENTITIES) != jsonObjectEnd) {
        skill.entities =
            jsonObject.at(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_ENTITIES).get<std::vector<std::string>>();
    }

    if (jsonObject.find(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_URIS) != jsonObjectEnd) {
        skill.uris = jsonObject.at(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_URIS).get<std::vector<SkillUri>>();
    }
}

void from_json(const nlohmann::json &jsonObject, CustomizeData &customizeData)
{
    // these are not required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(ProfileReader::PROFILE_KEY_NAME) != jsonObjectEnd) {
        customizeData.name = jsonObject.at(ProfileReader::PROFILE_KEY_NAME).get<std::string>();
    }

    if (jsonObject.find(ProfileReader::BUNDLE_MODULE_META_KEY_EXTRA) != jsonObjectEnd) {
        customizeData.extra = jsonObject.at(ProfileReader::BUNDLE_MODULE_META_KEY_EXTRA).get<std::string>();
    }

    if (jsonObject.find(ProfileReader::BUNDLE_MODULE_META_KEY_VALUE) != jsonObjectEnd) {
        customizeData.value = jsonObject.at(ProfileReader::BUNDLE_MODULE_META_KEY_VALUE).get<std::string>();
    }
}

void from_json(const nlohmann::json &jsonObject, Parameters &parameters)
{
    // these are required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_TYPE) != jsonObjectEnd) {
        parameters.type = jsonObject.at(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_TYPE).get<std::string>();
    }
    // these are not required fields.

    if (jsonObject.find(ProfileReader::PROFILE_KEY_DESCRIPTION) != jsonObjectEnd) {
        parameters.description = jsonObject.at(ProfileReader::PROFILE_KEY_DESCRIPTION).get<std::string>();
    }

    if (jsonObject.find(ProfileReader::PROFILE_KEY_NAME) != jsonObjectEnd) {
        parameters.name = jsonObject.at(ProfileReader::PROFILE_KEY_NAME).get<std::string>();
    }
}

void from_json(const nlohmann::json &jsonObject, Results &results)
{
    // these are required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_TYPE) != jsonObjectEnd) {
        results.type = jsonObject.at(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_TYPE).get<std::string>();
    }
    // these are not required fields.
    if (jsonObject.find(ProfileReader::PROFILE_KEY_DESCRIPTION) != jsonObjectEnd) {
        results.description = jsonObject.at(ProfileReader::PROFILE_KEY_DESCRIPTION).get<std::string>();
    }

    if (jsonObject.find(ProfileReader::PROFILE_KEY_NAME) != jsonObjectEnd) {
        results.name = jsonObject.at(ProfileReader::PROFILE_KEY_NAME).get<std::string>();
    }
}

void from_json(const nlohmann::json &jsonObject, MetaData &metaData)
{
    // these are not required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(ProfileReader::BUNDLE_MODULE_META_KEY_CUSTOMIZE_DATA) != jsonObjectEnd) {
        metaData.customizeData =
            jsonObject.at(ProfileReader::BUNDLE_MODULE_META_KEY_CUSTOMIZE_DATA).get<std::vector<CustomizeData>>();
    }

    if (jsonObject.find(ProfileReader::BUNDLE_MODULE_META_KEY_PARAMETERS) != jsonObjectEnd) {
        metaData.parameters =
            jsonObject.at(ProfileReader::BUNDLE_MODULE_META_KEY_PARAMETERS).get<std::vector<Parameters>>();
    }

    if (jsonObject.find(ProfileReader::BUNDLE_MODULE_META_KEY_RESULTS) != jsonObjectEnd) {
        metaData.results = jsonObject.at(ProfileReader::BUNDLE_MODULE_META_KEY_RESULTS).get<std::vector<Results>>();
    }
}

void from_json(const nlohmann::json &jsonObject, Distro &distro)
{
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_DELIVERY_WITH_INSTALL) != jsonObjectEnd) {
        distro.deliveryWithInstall =
            jsonObject.at(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_DELIVERY_WITH_INSTALL).get<bool>();
    }

    if (jsonObject.find(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_MODULE_NAME) != jsonObjectEnd) {
        distro.moduleName = jsonObject.at(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_MODULE_NAME).get<std::string>();
    }

    if (jsonObject.find(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_MODULE_TYPE) != jsonObjectEnd) {
        distro.moduleType = jsonObject.at(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_MODULE_TYPE).get<std::string>();
    }
}

void from_json(const nlohmann::json &jsonObject, UsedScene &usedScene)
{
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS_ABILITY) != jsonObjectEnd) {
        usedScene.ability = jsonObject.at(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS_ABILITY)
                                .get<std::vector<std::string>>();
    }
    if (jsonObject.find(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS_WHEN) != jsonObjectEnd) {
        usedScene.when =
            jsonObject.at(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS_WHEN).get<std::string>();
    }
}

void from_json(const nlohmann::json &jsonObject, ReqPermission &reqPermission)
{
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS_NAME) != jsonObjectEnd) {
        reqPermission.name =
            jsonObject.at(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS_NAME).get<std::string>();
    }
    if (jsonObject.find(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS_REASON) != jsonObjectEnd) {
        reqPermission.reason =
            jsonObject.at(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS_REASON).get<std::string>();
    }
    if (jsonObject.find(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS_USEDSCENE) != jsonObjectEnd) {
        reqPermission.usedScene =
            jsonObject.at(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS_USEDSCENE).get<UsedScene>();
    }
}

void from_json(const nlohmann::json &jsonObject, DefPermission &defPermission)
{
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_NAME) != jsonObjectEnd) {
        defPermission.name =
            jsonObject.at(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_NAME).get<std::string>();
    }
    if (jsonObject.find(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_GRANTMODE) != jsonObjectEnd) {
        defPermission.grantMode =
            jsonObject.at(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_GRANTMODE).get<std::string>();
    }
    if (jsonObject.find(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_AVAILABLESCOPE) != jsonObjectEnd) {
        defPermission.availableScope =
            jsonObject.at(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_AVAILABLESCOPE)
                .get<std::vector<std::string>>();
    }
    if (jsonObject.find(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_LABEL) != jsonObjectEnd) {
        defPermission.label =
            jsonObject.at(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_LABEL).get<std::string>();
    }
    if (jsonObject.find(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_LABEL_ID) != jsonObjectEnd) {
        defPermission.labelId =
            jsonObject.at(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_LABEL_ID).get<int32_t>();
    }
    if (jsonObject.find(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_DESCRIPTION) != jsonObjectEnd) {
        defPermission.description =
            jsonObject.at(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_DESCRIPTION).get<std::string>();
    }
    if (jsonObject.find(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_DESCRIPTION_ID) != jsonObjectEnd) {
        defPermission.descriptionId =
            jsonObject.at(ProfileReader::BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_DESCRIPTION_ID).get<int32_t>();
    }
}

bool InnerBundleInfo::FromJson(const nlohmann::json &jsonObject)
{
    try {
        isSupportBackup_ = jsonObject.at(IS_SUPPORT_BACKUP).get<bool>();
        appType_ = jsonObject.at(APP_TYPE).get<Constants::AppType>();
        uid_ = jsonObject.at(UID).get<int>();
        gid_ = jsonObject.at(GID).get<int>();
        baseDataDir_ = jsonObject.at(BASE_DATA_DIR).get<std::string>();
        bundleStatus_ = jsonObject.at(BUNDLE_STATUS).get<BundleStatus>();
        baseBundleInfo_ = jsonObject.at(BASE_BUNDLE_INFO).get<BundleInfo>();
        baseApplicationInfo_ = jsonObject.at(BASE_APPLICATION_INFO).get<ApplicationInfo>();
        baseAbilityInfos_ = jsonObject.at(BASE_ABILITY_INFO).get<std::map<std::string, AbilityInfo>>();
        innerModuleInfos_ = jsonObject.at(INNER_MODULE_INFO).get<std::map<std::string, InnerModuleInfo>>();
        skillInfos_ = jsonObject.at(SKILL_INFOS).get<std::map<std::string, std::vector<Skill>>>();
        isKeepData_ = jsonObject.at(IS_KEEP_DATA).get<bool>();
        userId_ = jsonObject.at(USER_ID).get<int>();
        mainAbility_ = jsonObject.at(MAIN_ABILITY).get<std::string>();
        provisionId_ = jsonObject.at(PROVISION_ID).get<std::string>();
        appFeature_ = jsonObject.at(APP_FEATURE).get<std::string>();
        hasEntry_ = jsonObject.at(HAS_ENTRY).get<bool>();
    } catch (nlohmann::detail::parse_error &exception) {
        APP_LOGE("has a parse_error:%{public}s", exception.what());
        return false;
    } catch (nlohmann::detail::type_error &exception) {
        APP_LOGE("has a type_error:%{public}s.", exception.what());
        return false;
    } catch (nlohmann::detail::out_of_range &exception) {
        APP_LOGE("has an out_of_range exception:%{public}s.", exception.what());
        return false;
    }
    return true;
}

std::optional<std::vector<Skill>> InnerBundleInfo::FindSkills(const std::string &keyName) const
{
    auto skillsInfo = skillInfos_.find(keyName);
    if (skillsInfo == skillInfos_.end()) {
        return std::nullopt;
    }
    auto &skills = skillsInfo->second;
    if (skills.empty()) {
        return std::nullopt;
    }
    return std::optional<std::vector<Skill>> {skills};
}

std::optional<HapModuleInfo> InnerBundleInfo::FindHapModuleInfo(const std::string &modulePackage) const
{
    auto it = innerModuleInfos_.find(modulePackage);
    if (it == innerModuleInfos_.end()) {
        APP_LOGE("can not find module %{public}s", modulePackage.c_str());
        return std::nullopt;
    }
    HapModuleInfo hapInfo;
    hapInfo.name = it->second.modulePackage;
    hapInfo.moduleName = it->second.moduleName;
    hapInfo.description = it->second.description;
    hapInfo.supportedModes = baseApplicationInfo_.supportedModes;
    hapInfo.reqCapabilities = it->second.reqCapabilities;
    bool first = false;
    for (auto &ability : baseAbilityInfos_) {
        if (ability.first.find(modulePackage) != std::string::npos) {
            if (!first) {
                hapInfo.label = ability.second.label;
                hapInfo.iconPath = ability.second.iconPath;
                hapInfo.deviceTypes = ability.second.deviceTypes;
                first = true;
            }
            auto &abilityInfo = hapInfo.abilityInfos.emplace_back(ability.second);
            GetApplicationInfo(ApplicationFlag::GET_APPLICATION_INFO_WITH_PERMS, 0, abilityInfo.applicationInfo);
        }
    }
    return hapInfo;
}

std::optional<AbilityInfo> InnerBundleInfo::FindAbilityInfo(
    const std::string &bundleName, const std::string &abilityName) const
{
    for (const auto &ability : baseAbilityInfos_) {
        if ((ability.second.bundleName == bundleName) && (ability.second.name == abilityName)) {
            return ability.second;
        }
    }
    return std::nullopt;
}

bool InnerBundleInfo::AddModuleInfo(const InnerBundleInfo &newInfo)
{
    if (newInfo.currentPackage_.empty()) {
        APP_LOGE("current package is empty");
        return false;
    }
    if (FindModule(newInfo.currentPackage_)) {
        APP_LOGE("current package %{public}s is exist", currentPackage_.c_str());
        return false;
    }
    if (!hasEntry_ && newInfo.HasEntry()) {
        hasEntry_ = true;
    }
    if (mainAbility_.empty() && !newInfo.mainAbility_.empty()) {
        UpdateBaseApplicationInfo(newInfo.baseApplicationInfo_);
        SetMainAbility(newInfo.mainAbility_);
        SetMainAbilityName(newInfo.mainAbilityName_);
    }
    AddInnerModuleInfo(newInfo.innerModuleInfos_);
    AddModuleAbilityInfo(newInfo.baseAbilityInfos_);
    AddModuleSkillInfo(newInfo.skillInfos_);
    return true;
}

void InnerBundleInfo::UpdateVersionInfo(const InnerBundleInfo &newInfo)
{
    if (baseBundleInfo_.versionCode == newInfo.GetVersionCode()) {
        APP_LOGE("old version equals to new version");
        return;
    }
    baseBundleInfo_.versionCode = newInfo.GetVersionCode();
    baseBundleInfo_.vendor = newInfo.GetBaseBundleInfo().vendor;
    baseBundleInfo_.versionName = newInfo.GetBaseBundleInfo().versionName;
    baseBundleInfo_.minSdkVersion = newInfo.GetBaseBundleInfo().minSdkVersion;
    baseBundleInfo_.maxSdkVersion = newInfo.GetBaseBundleInfo().maxSdkVersion;
    baseBundleInfo_.compatibleVersion = newInfo.GetBaseBundleInfo().compatibleVersion;
    baseBundleInfo_.targetVersion = newInfo.GetBaseBundleInfo().targetVersion;
    baseBundleInfo_.releaseType = newInfo.GetBaseBundleInfo().releaseType;
}

void InnerBundleInfo::UpdateModuleInfo(const InnerBundleInfo &newInfo)
{
    if (newInfo.currentPackage_.empty()) {
        APP_LOGE("no package in new info");
        return;
    }
    innerModuleInfos_.erase(newInfo.currentPackage_);
    for (auto it = baseAbilityInfos_.begin(); it != baseAbilityInfos_.end();) {
        if (it->first.find(newInfo.currentPackage_) != std::string::npos) {
            skillInfos_.erase(it->first);
            it = baseAbilityInfos_.erase(it);
        } else {
            ++it;
        }
    }
    if (!hasEntry_ && newInfo.HasEntry()) {
        hasEntry_ = true;
    }
    if (mainAbility_ == newInfo.mainAbility_) {
        UpdateBaseApplicationInfo(newInfo.baseApplicationInfo_);
    }
    AddInnerModuleInfo(newInfo.innerModuleInfos_);
    AddModuleAbilityInfo(newInfo.baseAbilityInfos_);
    AddModuleSkillInfo(newInfo.skillInfos_);
}

void InnerBundleInfo::RemoveModuleInfo(const std::string &modulePackage)
{
    if (innerModuleInfos_.find(modulePackage) == innerModuleInfos_.end()) {
        APP_LOGE("can not find module %{public}s", modulePackage.c_str());
        return;
    }
    if (mainAbility_.find(modulePackage) != std::string::npos) {
        mainAbility_.clear();
    }
    for (auto it = innerModuleInfos_.begin(); it != innerModuleInfos_.end();) {
        (it->first == modulePackage) ? innerModuleInfos_.erase(it++) : (++it);
    }
    for (auto it = baseAbilityInfos_.begin(); it != baseAbilityInfos_.end();) {
        (it->first.find(modulePackage) != std::string::npos) ? baseAbilityInfos_.erase(it++) : (++it);
    }
    for (auto it = skillInfos_.begin(); it != skillInfos_.end();) {
        (it->first.find(modulePackage) != std::string::npos) ? skillInfos_.erase(it++) : (++it);
    }
}

std::string InnerBundleInfo::ToString() const
{
    nlohmann::json j;
    j[IS_SUPPORT_BACKUP] = isSupportBackup_;
    j[APP_TYPE] = appType_;
    j[UID] = uid_;
    j[GID] = gid_;
    j[BASE_DATA_DIR] = baseDataDir_;
    j[BUNDLE_STATUS] = bundleStatus_;
    j[BASE_APPLICATION_INFO] = baseApplicationInfo_;
    j[BASE_BUNDLE_INFO] = baseBundleInfo_;
    j[BASE_ABILITY_INFO] = baseAbilityInfos_;
    j[INNER_MODULE_INFO] = innerModuleInfos_;
    j[SKILL_INFOS] = skillInfos_;
    j[IS_KEEP_DATA] = isKeepData_;
    j[USER_ID] = userId_;
    j[MAIN_ABILITY] = mainAbility_;
    j[APP_FEATURE] = appFeature_;
    j[PROVISION_ID] = provisionId_;
    j[HAS_ENTRY] = hasEntry_;
    return j.dump();
}

void InnerBundleInfo::GetApplicationInfo(const ApplicationFlag flag, const int userId, ApplicationInfo &appInfo) const
{
    appInfo = baseApplicationInfo_;
    for (const auto &info : innerModuleInfos_) {
        ModuleInfo moduleInfo;
        moduleInfo.moduleName = info.second.moduleName;
        moduleInfo.moduleSourceDir = info.second.modulePath;
        appInfo.moduleInfos.emplace_back(moduleInfo);
        appInfo.moduleSourceDirs.emplace_back(info.second.modulePath);
        if (info.second.isEntry) {
            appInfo.entryDir = info.second.modulePath;
        }
        if (flag == ApplicationFlag::GET_APPLICATION_INFO_WITH_PERMS) {
            std::transform(info.second.reqPermissions.begin(),
                info.second.reqPermissions.end(),
                std::back_inserter(appInfo.permissions),
                [](const auto &p) { return p.name; });
        }
    }
}

void InnerBundleInfo::GetBundleInfo(const BundleFlag flag, BundleInfo &bundleInfo) const
{
    bundleInfo = baseBundleInfo_;
    GetApplicationInfo(ApplicationFlag::GET_APPLICATION_INFO_WITH_PERMS, 0, bundleInfo.applicationInfo);
    for (const auto &info : innerModuleInfos_) {
        std::transform(info.second.reqPermissions.begin(),
            info.second.reqPermissions.end(),
            std::back_inserter(bundleInfo.reqPermissions),
            [](const auto &p) { return p.name; });
        std::transform(info.second.defPermissions.begin(),
            info.second.defPermissions.end(),
            std::back_inserter(bundleInfo.defPermissions),
            [](const auto &p) { return p.name; });
        bundleInfo.hapModuleNames.emplace_back(info.second.modulePackage);
        bundleInfo.moduleNames.emplace_back(info.second.moduleName);
        bundleInfo.moduleDirs.emplace_back(info.second.modulePath);
        bundleInfo.modulePublicDirs.emplace_back(info.second.moduleDataDir);
        bundleInfo.moduleResPaths.emplace_back(info.second.moduleResPath);
        if (info.second.isEntry) {
            bundleInfo.mainEntry = info.second.modulePackage;
            bundleInfo.entryModuleName = info.second.moduleName;
        }
    }
    if (flag == BundleFlag::GET_BUNDLE_WITH_ABILITIES) {
        std::transform(baseAbilityInfos_.begin(),
            baseAbilityInfos_.end(),
            std::back_inserter(bundleInfo.abilityInfos),
            [this](auto m) {
                GetApplicationInfo(ApplicationFlag::GET_APPLICATION_INFO_WITH_PERMS, 0, m.second.applicationInfo);
                return m.second;
            });
    }
}

bool InnerBundleInfo::CheckSpecialMetaData(const std::string &metaData) const
{
    for (const auto &moduleInfo : innerModuleInfos_) {
        for (const auto &data : moduleInfo.second.metaData.customizeData) {
            if (metaData == data.name) {
                return true;
            }
        }
    }
    return false;
}

}  // namespace AppExecFwk
}  // namespace OHOS
