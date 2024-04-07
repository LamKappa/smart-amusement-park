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

#include "ability_info.h"

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "nlohmann/json.hpp"
#include "string_ex.h"
#include "app_log_wrapper.h"
#include "parcel_macro.h"
#include "json_serializer.h"
#include "bundle_constants.h"

namespace OHOS {
namespace AppExecFwk {

namespace {

const std::string JSON_KEY_PACKAGE = "package";
const std::string JSON_KEY_NAME = "name";
const std::string JSON_KEY_BUNDLE_NAME = "bundleName";
const std::string JSON_KEY_APPLICATION_NAME = "applicationName";
const std::string JSON_KEY_LABEL = "label";
const std::string JSON_KEY_DESCRIPTION = "description";
const std::string JSON_KEY_ICON_PATH = "iconPath";
const std::string JSON_KEY_VISIBLE = "visible";
const std::string JSON_KEY_KIND = "kind";
const std::string JSON_KEY_TYPE = "type";
const std::string JSON_KEY_ORIENTATION = "orientation";
const std::string JSON_KEY_LAUNCH_MODE = "launchMode";
const std::string JSON_KEY_CODE_PATH = "codePath";
const std::string JSON_KEY_RESOURCE_PATH = "resourcePath";
const std::string JSON_KEY_LIB_PATH = "libPath";
const std::string JSON_KEY_PERMISSIONS = "permissions";
const std::string JSON_KEY_PROCESS = "process";
const std::string JSON_KEY_DEVICE_TYPES = "deviceTypes";
const std::string JSON_KEY_DEVICE_CAPABILITIES = "deviceCapabilities";
const std::string JSON_KEY_URI = "uri";
const std::string JSON_KEY_MODULE_NAME = "moduleName";
const std::string JSON_KEY_DEVICE_ID = "deviceId";
const std::string JSON_KEY_IS_LAUNCHER_ABILITY = "isLauncherAbility";
const std::string JSON_KEY_IS_NATIVE_ABILITY = "isNativeAbility";

}  // namespace

bool AbilityInfo::ReadFromParcel(Parcel &parcel)
{
    name = Str16ToStr8(parcel.ReadString16());
    label = Str16ToStr8(parcel.ReadString16());
    description = Str16ToStr8(parcel.ReadString16());
    iconPath = Str16ToStr8(parcel.ReadString16());
    kind = Str16ToStr8(parcel.ReadString16());
    uri = Str16ToStr8(parcel.ReadString16());
    package = Str16ToStr8(parcel.ReadString16());
    bundleName = Str16ToStr8(parcel.ReadString16());
    moduleName = Str16ToStr8(parcel.ReadString16());
    applicationName = Str16ToStr8(parcel.ReadString16());
    process = Str16ToStr8(parcel.ReadString16());
    deviceId = Str16ToStr8(parcel.ReadString16());
    codePath = Str16ToStr8(parcel.ReadString16());
    resourcePath = Str16ToStr8(parcel.ReadString16());
    libPath = Str16ToStr8(parcel.ReadString16());
    visible = parcel.ReadBool();
    isLauncherAbility = parcel.ReadBool();
    isNativeAbility = parcel.ReadBool();

    int32_t typeData;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, typeData);
    type = static_cast<AbilityType>(typeData);

    int32_t orientationData;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, orientationData);
    orientation = static_cast<DisplayOrientation>(orientationData);

    int32_t launchModeData;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, launchModeData);
    launchMode = static_cast<LaunchMode>(launchModeData);

    int32_t permissionsSize;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, permissionsSize);
    for (int32_t i = 0; i < permissionsSize; i++) {
        permissions.emplace_back(Str16ToStr8(parcel.ReadString16()));
    }

    int32_t deviceTypesSize;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, deviceTypesSize);
    for (int32_t i = 0; i < deviceTypesSize; i++) {
        deviceTypes.emplace_back(Str16ToStr8(parcel.ReadString16()));
    }

    int32_t deviceCapabilitiesSize;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, deviceCapabilitiesSize);
    for (int32_t i = 0; i < deviceCapabilitiesSize; i++) {
        deviceCapabilities.emplace_back(Str16ToStr8(parcel.ReadString16()));
    }

    std::unique_ptr<ApplicationInfo> appInfo(parcel.ReadParcelable<ApplicationInfo>());
    if (!appInfo) {
        APP_LOGE("ReadParcelable<ApplicationInfo> failed");
        return false;
    }
    applicationInfo = *appInfo;
    return true;
}

AbilityInfo *AbilityInfo::Unmarshalling(Parcel &parcel)
{
    AbilityInfo *info = new (std::nothrow) AbilityInfo();
    if (info && !info->ReadFromParcel(parcel)) {
        APP_LOGW("read from parcel failed");
        delete info;
        info = nullptr;
    }
    return info;
}

bool AbilityInfo::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(name));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(label));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(description));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(iconPath));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(kind));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(uri));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(package));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(bundleName));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(moduleName));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(applicationName));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(process));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(deviceId));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(codePath));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(resourcePath));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(libPath));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, visible);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, isLauncherAbility);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, isNativeAbility);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, static_cast<int32_t>(type));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, static_cast<int32_t>(orientation));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, static_cast<int32_t>(launchMode));

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, permissions.size());
    for (auto &permission : permissions) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(permission));
    }

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, deviceTypes.size());
    for (auto &deviceType : deviceTypes) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(deviceType));
    }

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, deviceCapabilities.size());
    for (auto &deviceCapability : deviceCapabilities) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(deviceCapability));
    }

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, parcel, &applicationInfo);
    return true;
}

void AbilityInfo::Dump(std::string prefix, int fd)
{
    APP_LOGI("called dump Abilityinfo");
    if (fd < 0) {
        APP_LOGE("dump Abilityinfo fd error");
        return;
    }
    int flags = fcntl(fd, F_GETFL);
    if (flags < 0) {
        APP_LOGE("dump Abilityinfo fcntl error %{public}s", strerror(errno));
        return;
    }
    flags &= O_ACCMODE;
    if ((flags == O_WRONLY) || (flags == O_RDWR)) {
        nlohmann::json jsonObject = *this;
        std::string result;
        result.append(prefix);
        result.append(jsonObject.dump(Constants::DUMP_INDENT));
        int ret = TEMP_FAILURE_RETRY(write(fd, result.c_str(), result.size()));
        if (ret < 0) {
            APP_LOGE("dump Abilityinfo write error %{public}s", strerror(errno));
        }
    }
    return;
}

void to_json(nlohmann::json &jsonObject, const AbilityInfo &abilityInfo)
{
    jsonObject = nlohmann::json{
        {"name", abilityInfo.name},
        {"label", abilityInfo.label},
        {"description", abilityInfo.description},
        {"iconPath", abilityInfo.iconPath},
        {"visible", abilityInfo.visible},
        {"isLauncherAbility", abilityInfo.isLauncherAbility},
        {"isNativeAbility", abilityInfo.isNativeAbility},
        {"kind", abilityInfo.kind},
        {"type", abilityInfo.type},
        {"orientation", abilityInfo.orientation},
        {"launchMode", abilityInfo.launchMode},
        {"permissions", abilityInfo.permissions},
        {"process", abilityInfo.process},
        {"deviceTypes", abilityInfo.deviceTypes},
        {"deviceCapabilities", abilityInfo.deviceCapabilities},
        {"uri", abilityInfo.uri},
        {"package", abilityInfo.package},
        {"bundleName", abilityInfo.bundleName},
        {"moduleName", abilityInfo.moduleName},
        {"applicationName", abilityInfo.applicationName},
        {"deviceId", abilityInfo.deviceId},
        {"codePath", abilityInfo.codePath},
        {"resourcePath", abilityInfo.resourcePath},
        {"libPath", abilityInfo.libPath}
    };
}

void from_json(const nlohmann::json &jsonObject, AbilityInfo &abilityInfo)
{
    abilityInfo.name = jsonObject.at(JSON_KEY_NAME).get<std::string>();
    abilityInfo.label = jsonObject.at(JSON_KEY_LABEL).get<std::string>();
    abilityInfo.description = jsonObject.at(JSON_KEY_DESCRIPTION).get<std::string>();
    abilityInfo.iconPath = jsonObject.at(JSON_KEY_ICON_PATH).get<std::string>();
    abilityInfo.visible = jsonObject.at(JSON_KEY_VISIBLE).get<bool>();
    abilityInfo.isLauncherAbility = jsonObject.at(JSON_KEY_IS_LAUNCHER_ABILITY).get<bool>();
    abilityInfo.isNativeAbility = jsonObject.at(JSON_KEY_IS_NATIVE_ABILITY).get<bool>();
    abilityInfo.kind = jsonObject.at(JSON_KEY_KIND).get<std::string>();
    abilityInfo.type = jsonObject.at(JSON_KEY_TYPE).get<AbilityType>();
    abilityInfo.orientation = jsonObject.at(JSON_KEY_ORIENTATION).get<DisplayOrientation>();
    abilityInfo.launchMode = jsonObject.at(JSON_KEY_LAUNCH_MODE).get<LaunchMode>();
    abilityInfo.permissions = jsonObject.at(JSON_KEY_PERMISSIONS).get<std::vector<std::string>>();
    abilityInfo.process = jsonObject.at(JSON_KEY_PROCESS).get<std::string>();
    abilityInfo.deviceTypes = jsonObject.at(JSON_KEY_DEVICE_TYPES).get<std::vector<std::string>>();
    abilityInfo.deviceCapabilities = jsonObject.at(JSON_KEY_DEVICE_CAPABILITIES).get<std::vector<std::string>>();
    abilityInfo.uri = jsonObject.at(JSON_KEY_URI).get<std::string>();
    abilityInfo.package = jsonObject.at(JSON_KEY_PACKAGE).get<std::string>();
    abilityInfo.bundleName = jsonObject.at(JSON_KEY_BUNDLE_NAME).get<std::string>();
    abilityInfo.moduleName = jsonObject.at(JSON_KEY_MODULE_NAME).get<std::string>();
    abilityInfo.applicationName = jsonObject.at(JSON_KEY_APPLICATION_NAME).get<std::string>();
    abilityInfo.deviceId = jsonObject.at(JSON_KEY_DEVICE_ID).get<std::string>();
    abilityInfo.codePath = jsonObject.at(JSON_KEY_CODE_PATH).get<std::string>();
    abilityInfo.resourcePath = jsonObject.at(JSON_KEY_RESOURCE_PATH).get<std::string>();
    abilityInfo.libPath = jsonObject.at(JSON_KEY_LIB_PATH).get<std::string>();
}

}  // namespace AppExecFwk
}  // namespace OHOS
