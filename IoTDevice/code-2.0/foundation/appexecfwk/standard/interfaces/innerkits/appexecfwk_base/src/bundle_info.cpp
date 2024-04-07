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

#include "bundle_info.h"

#include "string_ex.h"

#include "app_log_wrapper.h"
#include "json_serializer.h"
#include "parcel_macro.h"

namespace OHOS {
namespace AppExecFwk {

bool BundleInfo::ReadFromParcel(Parcel &parcel)
{
    name = Str16ToStr8(parcel.ReadString16());
    label = Str16ToStr8(parcel.ReadString16());
    description = Str16ToStr8(parcel.ReadString16());
    vendor = Str16ToStr8(parcel.ReadString16());
    versionName = Str16ToStr8(parcel.ReadString16());
    mainEntry = Str16ToStr8(parcel.ReadString16());
    cpuAbi = Str16ToStr8(parcel.ReadString16());
    appId = Str16ToStr8(parcel.ReadString16());
    entryModuleName = Str16ToStr8(parcel.ReadString16());
    releaseType = Str16ToStr8(parcel.ReadString16());
    jointUserId = Str16ToStr8(parcel.ReadString16());
    seInfo = Str16ToStr8(parcel.ReadString16());
    versionCode = parcel.ReadUint32();
    minSdkVersion = parcel.ReadInt32();
    maxSdkVersion = parcel.ReadInt32();
    compatibleVersion = parcel.ReadInt32();
    targetVersion = parcel.ReadInt32();
    uid = parcel.ReadInt32();
    gid = parcel.ReadInt32();
    isKeepAlive = parcel.ReadBool();
    isNativeApp = parcel.ReadBool();
    installTime = parcel.ReadInt64();
    updateTime = parcel.ReadInt64();

    int32_t reqPermissionsSize;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, reqPermissionsSize);
    for (int32_t i = 0; i < reqPermissionsSize; i++) {
        reqPermissions.emplace_back(Str16ToStr8(parcel.ReadString16()));
    }

    int32_t defPermissionsSize;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, defPermissionsSize);
    for (int32_t i = 0; i < defPermissionsSize; i++) {
        defPermissions.emplace_back(Str16ToStr8(parcel.ReadString16()));
    }

    int32_t hapModuleNamesSize;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, hapModuleNamesSize);
    for (int32_t i = 0; i < hapModuleNamesSize; i++) {
        hapModuleNames.emplace_back(Str16ToStr8(parcel.ReadString16()));
    }

    int32_t moduleNamesSize;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, moduleNamesSize);
    for (int32_t i = 0; i < moduleNamesSize; i++) {
        moduleNames.emplace_back(Str16ToStr8(parcel.ReadString16()));
    }

    int32_t modulePublicDirsSize;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, modulePublicDirsSize);
    for (int32_t i = 0; i < modulePublicDirsSize; i++) {
        modulePublicDirs.emplace_back(Str16ToStr8(parcel.ReadString16()));
    }

    int32_t moduleDirsSize;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, moduleDirsSize);
    for (int32_t i = 0; i < moduleDirsSize; i++) {
        moduleDirs.emplace_back(Str16ToStr8(parcel.ReadString16()));
    }

    int32_t moduleResPathsSize;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, moduleResPathsSize);
    for (int32_t i = 0; i < moduleResPathsSize; i++) {
        moduleResPaths.emplace_back(Str16ToStr8(parcel.ReadString16()));
    }

    std::unique_ptr<ApplicationInfo> appInfo(parcel.ReadParcelable<ApplicationInfo>());
    if (!appInfo) {
        APP_LOGE("ReadParcelable<ApplicationInfo> failed");
        return false;
    }
    applicationInfo = *appInfo;

    int32_t abilityInfosSize = parcel.ReadInt32();
    for (int32_t i = 0; i < abilityInfosSize; i++) {
        std::unique_ptr<AbilityInfo> abilityInfo(parcel.ReadParcelable<AbilityInfo>());
        if (!abilityInfo) {
            APP_LOGE("ReadParcelable<AbilityInfo> failed");
            return false;
        }
        abilityInfos.emplace_back(*abilityInfo);
    }
    return true;
}

bool BundleInfo::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(name));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(label));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(description));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(vendor));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(versionName));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(mainEntry));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(cpuAbi));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(appId));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(entryModuleName));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(releaseType));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(jointUserId));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(seInfo));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, versionCode);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, minSdkVersion);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, maxSdkVersion);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, compatibleVersion);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, targetVersion);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, uid);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, gid);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, isKeepAlive);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, isNativeApp);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int64, parcel, installTime);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int64, parcel, updateTime);

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, reqPermissions.size());
    for (auto &reqPermission : reqPermissions) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(reqPermission));
    }

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, defPermissions.size());
    for (auto &defPermission : defPermissions) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(defPermission));
    }

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, hapModuleNames.size());
    for (auto &hapModuleName : hapModuleNames) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(hapModuleName));
    }

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, moduleNames.size());
    for (auto &moduleName : moduleNames) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(moduleName));
    }

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, modulePublicDirs.size());
    for (auto &modulePublicDir : modulePublicDirs) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(modulePublicDir));
    }

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, moduleDirs.size());
    for (auto &moduleDir : moduleDirs) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(moduleDir));
    }

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, moduleResPaths.size());
    for (auto &moduleResPath : moduleResPaths) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(moduleResPath));
    }

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, parcel, &applicationInfo);

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, abilityInfos.size());
    for (auto &abilityInfo : abilityInfos) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, parcel, &abilityInfo);
    }
    return true;
}

BundleInfo *BundleInfo::Unmarshalling(Parcel &parcel)
{
    BundleInfo *info = new (std::nothrow) BundleInfo();
    if (info && !info->ReadFromParcel(parcel)) {
        APP_LOGW("read from parcel failed");
        delete info;
        info = nullptr;
    }
    return info;
}

void to_json(nlohmann::json &jsonObject, const BundleInfo &bundleInfo)
{
    jsonObject = nlohmann::json{
        {"name", bundleInfo.name},
        {"label", bundleInfo.label},
        {"description", bundleInfo.description},
        {"vendor", bundleInfo.vendor},
        {"isKeepAlive", bundleInfo.isKeepAlive},
        {"isNativeApp", bundleInfo.isNativeApp},
        {"applicationInfo", bundleInfo.applicationInfo},
        {"abilityInfos", bundleInfo.abilityInfos},
        {"jointUserId", bundleInfo.jointUserId},
        {"versionCode", bundleInfo.versionCode},
        {"versionName", bundleInfo.versionName},
        {"minSdkVersion", bundleInfo.minSdkVersion},
        {"maxSdkVersion", bundleInfo.maxSdkVersion},
        {"mainEntry", bundleInfo.mainEntry},
        {"cpuAbi", bundleInfo.cpuAbi},
        {"appId", bundleInfo.appId},
        {"compatibleVersion", bundleInfo.compatibleVersion},
        {"targetVersion", bundleInfo.targetVersion},
        {"releaseType", bundleInfo.releaseType},
        {"uid", bundleInfo.uid},
        {"gid", bundleInfo.gid},
        {"seInfo", bundleInfo.seInfo},
        {"installTime", bundleInfo.installTime},
        {"updateTime", bundleInfo.updateTime},
        {"entryModuleName", bundleInfo.entryModuleName},
        {"reqPermissions", bundleInfo.reqPermissions},
        {"defPermissions", bundleInfo.defPermissions},
        {"hapModuleNames", bundleInfo.hapModuleNames},
        {"moduleNames", bundleInfo.moduleNames},
        {"modulePublicDirs", bundleInfo.modulePublicDirs},
        {"moduleDirs", bundleInfo.moduleDirs},
        {"moduleResPaths", bundleInfo.moduleResPaths}
    };
}

void from_json(const nlohmann::json &jsonObject, BundleInfo &bundleInfo)
{
    bundleInfo.name = jsonObject.at("name").get<std::string>();
    bundleInfo.label = jsonObject.at("label").get<std::string>();
    bundleInfo.description = jsonObject.at("description").get<std::string>();
    bundleInfo.vendor = jsonObject.at("vendor").get<std::string>();
    bundleInfo.isKeepAlive = jsonObject.at("isKeepAlive").get<bool>();
    bundleInfo.isNativeApp = jsonObject.at("isNativeApp").get<bool>();
    bundleInfo.applicationInfo = jsonObject.at("applicationInfo").get<ApplicationInfo>();
    bundleInfo.abilityInfos = jsonObject.at("abilityInfos").get<std::vector<AbilityInfo>>();
    bundleInfo.versionCode = jsonObject.at("versionCode").get<uint32_t>();
    bundleInfo.versionName = jsonObject.at("versionName").get<std::string>();
    bundleInfo.jointUserId = jsonObject.at("jointUserId").get<std::string>();
    bundleInfo.minSdkVersion = jsonObject.at("minSdkVersion").get<int32_t>();
    bundleInfo.maxSdkVersion = jsonObject.at("maxSdkVersion").get<int32_t>();
    bundleInfo.mainEntry = jsonObject.at("mainEntry").get<std::string>();
    bundleInfo.cpuAbi = jsonObject.at("cpuAbi").get<std::string>();
    bundleInfo.appId = jsonObject.at("appId").get<std::string>();
    bundleInfo.compatibleVersion = jsonObject.at("compatibleVersion").get<int>();
    bundleInfo.targetVersion = jsonObject.at("targetVersion").get<int>();
    bundleInfo.releaseType = jsonObject.at("releaseType").get<std::string>();
    bundleInfo.uid = jsonObject.at("uid").get<int>();
    bundleInfo.gid = jsonObject.at("gid").get<int>();
    bundleInfo.seInfo = jsonObject.at("seInfo").get<std::string>();
    bundleInfo.installTime = jsonObject.at("installTime").get<int64_t>();
    bundleInfo.updateTime = jsonObject.at("updateTime").get<int64_t>();
    bundleInfo.entryModuleName = jsonObject.at("entryModuleName").get<std::string>();
    bundleInfo.reqPermissions = jsonObject.at("reqPermissions").get<std::vector<std::string>>();
    bundleInfo.defPermissions = jsonObject.at("defPermissions").get<std::vector<std::string>>();
    bundleInfo.hapModuleNames = jsonObject.at("hapModuleNames").get<std::vector<std::string>>();
    bundleInfo.moduleNames = jsonObject.at("moduleNames").get<std::vector<std::string>>();
    bundleInfo.modulePublicDirs = jsonObject.at("modulePublicDirs").get<std::vector<std::string>>();
    bundleInfo.moduleDirs = jsonObject.at("moduleDirs").get<std::vector<std::string>>();
    bundleInfo.moduleResPaths = jsonObject.at("moduleResPaths").get<std::vector<std::string>>();
}

}  // namespace AppExecFwk
}  // namespace OHOS
