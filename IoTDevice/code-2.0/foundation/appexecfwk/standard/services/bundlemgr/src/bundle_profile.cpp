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

#include "bundle_profile.h"

#include <sstream>
#include <fstream>

#include "permission/permission_kit.h"
#include "app_log_wrapper.h"
#include "bundle_constants.h"
#include "common_profile.h"

namespace OHOS {
namespace AppExecFwk {
namespace ProfileReader {

const std::map<std::string, AbilityType> ABILITY_TYPE_MAP = {
    {"page", AbilityType::PAGE}, {"service", AbilityType::SERVICE}, {"data", AbilityType::DATA}};
const std::map<std::string, DisplayOrientation> DISPLAY_ORIENTATION_MAP = {
    {"unspecified", DisplayOrientation::UNSPECIFIED},
    {"landscape", DisplayOrientation::LANDSCAPE},
    {"portrait", DisplayOrientation::PORTRAIT},
    {"followrecent", DisplayOrientation::FOLLOWRECENT}};
const std::map<std::string, LaunchMode> LAUNCH_MODE_MAP = {
    {"singleton", LaunchMode::SINGLETON}, {"singletop", LaunchMode::SINGLETOP}, {"standard", LaunchMode::STANDARD}};

struct Version {
    int32_t code = 0;
    std::string name;
};

struct ApiVersion {
    uint32_t compatible = 0;
    uint32_t target = 0;
    std::string releaseType = "Release";
};
// config.json app
struct App {
    std::string bundleName;
    std::string vendor;
    Version version;
    ApiVersion apiVersion;
};

struct ReqVersion {
    uint32_t compatible = 0;
    uint32_t target = 0;
};

struct Ark {
    ReqVersion reqVersion;
    std::string flag;
};

struct Domain {
    bool subDomains = false;
    std::string name;
};

struct DomainSetting {
    bool cleartextPermitted = false;
    std::vector<Domain> domains;
};

struct SecurityConfig {
    DomainSetting domainSetting;
};

struct Network {
    bool usesCleartext = false;
    SecurityConfig securityConfig;
};

struct Device {
    std::string jointUserId;
    std::string process;
    bool keepAlive = false;
    Ark ark;
    bool directLaunch = false;
    bool supportBackup = false;
    bool compressNativeLibs = true;
    Network network;
};
// config.json  deviceConfig
struct DeviceConfig {
    Device defaultDevice;
    Device phone;
    Device tablet;
    Device tv;
    Device car;
    Device wearable;
    Device liteWearable;
    Device smartVision;
};

struct Form {
    std::vector<std::string> formEntity;
    int32_t minHeight = 0;
    int32_t defaultHeight = 0;
    int32_t minWidth = 0;
    int32_t defaultWidth = 0;
};

struct FormsCustomizeData {
    std::string name;
    std::string value;
};

struct FormsMetaData {
    std::vector<FormsCustomizeData> customizeData;
};

struct Forms {
    std::string name;
    std::string description;
    bool isDefault = false;
    std::string type;
    std::string colorMode = "auto";
    std::vector<std::string> supportDimensions;
    std::string defaultDimension;
    std::vector<std::string> landscapeLayouts;
    std::vector<std::string> portraitLayouts;
    bool updateEnabled = false;
    std::string scheduledUpateTime = "0:0";
    int32_t updateDuration = 0;
    std::string deepLink;
    std::string jsComponentName;
    FormsMetaData metaData;
};

struct UriPermission {
    std::string mode;
    std::string path;
};

struct Ability {
    std::string name;
    std::string description;
    int32_t descriptionId = 0;
    std::string icon;
    int32_t iconId = 0;
    std::string label;
    int32_t labelId = 0;
    std::string uri;
    std::string process;
    std::string launchType = "standard";
    std::string theme;
    bool visible = false;
    std::vector<std::string> permissions;
    std::vector<Skill> skills;
    std::vector<std::string> deviceCapability;
    MetaData metaData;
    std::string type;
    bool formEnabled = false;
    Form form;
    std::string orientation = "unspecified";
    std::vector<std::string> backgroundModes;
    bool grantPermission;
    UriPermission uriPermission;
    std::string readPermission;
    std::string writePermission;
    bool directLaunch = false;
    std::vector<std::string> configChanges;
    std::string mission;
    std::string targetAbility;
    bool multiUserShared = false;
    bool supportPipMode = false;
    bool formsEnabled = false;
    std::vector<Forms> formses;
};

struct Window {
    int32_t designWidth = 750;
    bool autoDesignWidth = false;
};

struct Js {
    std::string name = "default";
    std::vector<std::string> pages;
    Window window;
    std::string type = "normal";
};

struct Intent {
    std::string targetClass;
    std::string targetBundle;
};

struct CommonEvent {
    std::string name;
    std::string permission;
    std::vector<std::string> data;
    std::vector<std::string> type;
    std::vector<std::string> events;
};

struct Shortcut {
    std::string shortcutId;
    std::string label;
    std::vector<Intent> intents;
};

// config.json module
struct Module {
    std::string package;
    std::string name;
    std::string description;
    std::vector<std::string> supportedModes;
    std::vector<std::string> reqCapabilities;
    std::vector<std::string> deviceType;
    Distro distro;
    MetaData metaData;
    std::vector<Ability> abilities;
    std::vector<Js> jses;
    std::vector<CommonEvent> commonEvents;
    std::vector<Shortcut> shortcuts;
    std::vector<DefPermission> defPermissions;
    std::vector<ReqPermission> reqPermissions;
};

// config.json
struct ConfigJson {
    App app;
    DeviceConfig deveicConfig;
    Module module;
};

/*
 * form_json is global static overload method in self namespace ProfileReader,
 * which need callback by json library, and can not rename this function,
 * so don't named according UpperCamelCase style
 */
void from_json(const nlohmann::json &jsonObject, Version &version)
{
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(BUNDLE_APP_PROFILE_KEY_CODE) != jsonObjectEnd) {
        version.code = jsonObject.at(BUNDLE_APP_PROFILE_KEY_CODE).get<int32_t>();
    }

    if (jsonObject.find(PROFILE_KEY_NAME) != jsonObjectEnd) {
        version.name = jsonObject.at(PROFILE_KEY_NAME).get<std::string>();
    }
}

void from_json(const nlohmann::json &jsonObject, ApiVersion &apiVersion)
{
    // these are required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(BUNDLE_APP_PROFILE_KEY_COMPATIBLE) != jsonObjectEnd) {
        apiVersion.compatible = jsonObject.at(BUNDLE_APP_PROFILE_KEY_COMPATIBLE).get<uint32_t>();
    }
    // these are not required fields.
    if (jsonObject.find(BUNDLE_APP_PROFILE_KEY_TARGET) != jsonObjectEnd) {
        apiVersion.target = jsonObject.at(BUNDLE_APP_PROFILE_KEY_TARGET).get<uint32_t>();
    }

    if (jsonObject.find(BUNDLE_APP_PROFILE_KEY_RELEASE_TYPE) != jsonObjectEnd) {
        apiVersion.releaseType = jsonObject.at(BUNDLE_APP_PROFILE_KEY_RELEASE_TYPE).get<std::string>();
    }
}

void from_json(const nlohmann::json &jsonObject, App &app)
{
    // these are required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(BUNDLE_APP_PROFILE_KEY_BUNDLE_NAME) != jsonObjectEnd) {
        app.bundleName = jsonObject.at(BUNDLE_APP_PROFILE_KEY_BUNDLE_NAME).get<std::string>();
    }

    if (jsonObject.find(BUNDLE_APP_PROFILE_KEY_VERSION) != jsonObjectEnd) {
        app.version = jsonObject.at(BUNDLE_APP_PROFILE_KEY_VERSION).get<Version>();
    }

    if (jsonObject.find(BUNDLE_APP_PROFILE_KEY_API_VERSION) != jsonObjectEnd) {
        app.apiVersion = jsonObject.at(BUNDLE_APP_PROFILE_KEY_API_VERSION).get<ApiVersion>();
    }
    // these are not required fields.

    if (jsonObject.find(BUNDLE_APP_PROFILE_KEY_VENDOR) != jsonObjectEnd) {
        app.vendor = jsonObject.at(BUNDLE_APP_PROFILE_KEY_VENDOR).get<std::string>();
    }
}

void from_json(const nlohmann::json &jsonObject, ReqVersion &reqVersion)
{
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_COMPATIBLE) != jsonObjectEnd) {
        reqVersion.compatible = jsonObject.at(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_COMPATIBLE).get<uint32_t>();
    }

    if (jsonObject.find(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_TARGET) != jsonObjectEnd) {
        reqVersion.target = jsonObject.at(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_TARGET).get<uint32_t>();
    }
}

void from_json(const nlohmann::json &jsonObject, Ark &ark)
{
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_REQ_VERSION) != jsonObjectEnd) {
        ark.reqVersion = jsonObject.at(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_REQ_VERSION).get<ReqVersion>();
    }

    if (jsonObject.find(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_FLAG) != jsonObjectEnd) {
        ark.flag = jsonObject.at(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_FLAG).get<std::string>();
    }
}

void from_json(const nlohmann::json &jsonObject, Domain &domain)
{
    // these are not required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SUB_DOMAINS) != jsonObjectEnd) {
        domain.subDomains = jsonObject.at(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SUB_DOMAINS).get<bool>();
    }
    if (jsonObject.find(PROFILE_KEY_NAME) != jsonObjectEnd) {
        domain.name = jsonObject.at(PROFILE_KEY_NAME).get<std::string>();
    }
}

void from_json(const nlohmann::json &jsonObject, DomainSetting &domainSetting)
{
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_CLEAR_TEXT_PERMITTED) != jsonObjectEnd) {
        domainSetting.cleartextPermitted =
            jsonObject.at(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_CLEAR_TEXT_PERMITTED).get<bool>();
    }

    if (jsonObject.find(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DOMAINS) != jsonObjectEnd) {
        domainSetting.domains = jsonObject.at(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DOMAINS).get<std::vector<Domain>>();
    }
}

void from_json(const nlohmann::json &jsonObject, SecurityConfig &securityConfig)
{
    // these are not required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DOMAIN_SETTINGS) != jsonObjectEnd) {
        securityConfig.domainSetting =
            jsonObject.at(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DOMAIN_SETTINGS).get<DomainSetting>();
    }
}

void from_json(const nlohmann::json &jsonObject, Network &network)
{
    // these are not required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_USES_CLEAR_TEXT) != jsonObjectEnd) {
        network.usesCleartext = jsonObject.at(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_USES_CLEAR_TEXT).get<bool>();
    }

    if (jsonObject.find(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SECURITY_CONFIG) != jsonObjectEnd) {
        network.securityConfig = jsonObject.at(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SECURITY_CONFIG).get<SecurityConfig>();
    }
}

void from_json(const nlohmann::json &jsonObject, Device &device)
{
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_JOINT_USER_ID) != jsonObjectEnd) {
        device.jointUserId = jsonObject.at(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_JOINT_USER_ID).get<std::string>();
    }

    if (jsonObject.find(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_PROCESS) != jsonObjectEnd) {
        device.process = jsonObject.at(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_PROCESS).get<std::string>();
    }

    if (jsonObject.find(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_KEEP_ALIVE) != jsonObjectEnd) {
        device.keepAlive = jsonObject.at(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_KEEP_ALIVE).get<bool>();
    }

    if (jsonObject.find(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_ARK) != jsonObjectEnd) {
        device.ark = jsonObject.at(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_ARK).get<Ark>();
    }

    if (jsonObject.find(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DIRECT_LAUNCH) != jsonObjectEnd) {
        device.directLaunch = jsonObject.at(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DIRECT_LAUNCH).get<bool>();
    }

    if (jsonObject.find(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SUPPORT_BACKUP) != jsonObjectEnd) {
        device.supportBackup = jsonObject.at(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SUPPORT_BACKUP).get<bool>();
    }

    if (jsonObject.find(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_COMPRESS_NATIVE_LIBS) != jsonObjectEnd) {
        device.compressNativeLibs = jsonObject.at(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_COMPRESS_NATIVE_LIBS).get<bool>();
    }

    if (jsonObject.find(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_NETWORK) != jsonObjectEnd) {
        device.network = jsonObject.at(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_NETWORK).get<Network>();
    }
}

void from_json(const nlohmann::json &jsonObject, DeviceConfig &deviceConfig)
{
    // these are required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DEFAULT) != jsonObjectEnd) {
        deviceConfig.defaultDevice = jsonObject.at(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DEFAULT).get<Device>();
    }
    // these are not required fields.
    if (jsonObject.find(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_PHONE) != jsonObjectEnd) {
        deviceConfig.phone = jsonObject.at(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_PHONE).get<Device>();
    }

    if (jsonObject.find(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_TABLET) != jsonObjectEnd) {
        deviceConfig.tablet = jsonObject.at(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_TABLET).get<Device>();
    }

    if (jsonObject.find(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_TV) != jsonObjectEnd) {
        deviceConfig.tv = jsonObject.at(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_TV).get<Device>();
    }

    if (jsonObject.find(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_CAR) != jsonObjectEnd) {
        deviceConfig.car = jsonObject.at(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_CAR).get<Device>();
    }

    if (jsonObject.find(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_WEARABLE) != jsonObjectEnd) {
        deviceConfig.wearable = jsonObject.at(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_WEARABLE).get<Device>();
    }

    if (jsonObject.find(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_LITE_WEARABLE) != jsonObjectEnd) {
        deviceConfig.liteWearable = jsonObject.at(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_LITE_WEARABLE).get<Device>();
    }

    if (jsonObject.find(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SMART_VISION) != jsonObjectEnd) {
        deviceConfig.smartVision = jsonObject.at(BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SMART_VISION).get<Device>();
    }
}

void from_json(const nlohmann::json &jsonObject, Form &form)
{
    // these are not required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(BUNDLE_MODULE_PROFILE_FORM_ENTITY) != jsonObjectEnd) {
        form.formEntity = jsonObject.at(BUNDLE_MODULE_PROFILE_FORM_ENTITY).get<std::vector<std::string>>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_FORM_MIN_HEIGHT) != jsonObjectEnd) {
        form.minHeight = jsonObject.at(BUNDLE_MODULE_PROFILE_FORM_MIN_HEIGHT).get<int32_t>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_FORM_DEFAULT_HEIGHT) != jsonObjectEnd) {
        form.defaultHeight = jsonObject.at(BUNDLE_MODULE_PROFILE_FORM_DEFAULT_HEIGHT).get<int32_t>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_FORM_MIN_WIDTH) != jsonObjectEnd) {
        form.minWidth = jsonObject.at(BUNDLE_MODULE_PROFILE_FORM_MIN_WIDTH).get<int32_t>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_FORM_DEFAULT_WIDTH) != jsonObjectEnd) {
        form.defaultWidth = jsonObject.at(BUNDLE_MODULE_PROFILE_FORM_DEFAULT_WIDTH).get<int32_t>();
    }
}

void from_json(const nlohmann::json &jsonObject, FormsCustomizeData &customizeDataForms)
{
    // these are not required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(PROFILE_KEY_NAME) != jsonObjectEnd) {
        customizeDataForms.name = jsonObject.at(PROFILE_KEY_NAME).get<std::string>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_FORMS_VALUE) != jsonObjectEnd) {
        customizeDataForms.value = jsonObject.at(BUNDLE_MODULE_PROFILE_FORMS_VALUE).get<std::string>();
    }
}

void from_json(const nlohmann::json &jsonObject, FormsMetaData &formsMetaData)
{
    // these are not required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(PROFILE_KEY_NAME) != jsonObjectEnd) {
        formsMetaData.customizeData =
            jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_CUSTOMIZE_DATA).get<std::vector<FormsCustomizeData>>();
    }
}

void from_json(const nlohmann::json &jsonObject, Forms &forms)
{
    // these are required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(PROFILE_KEY_NAME) != jsonObjectEnd) {
        forms.name = jsonObject.at(PROFILE_KEY_NAME).get<std::string>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_FORMS_IS_DEFAULT) != jsonObjectEnd) {
        forms.isDefault = jsonObject.at(BUNDLE_MODULE_PROFILE_FORMS_IS_DEFAULT).get<bool>();
    }

    if (jsonObject.find(PROFILE_KEY_TYPE) != jsonObjectEnd) {
        forms.type = jsonObject.at(PROFILE_KEY_TYPE).get<std::string>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_FORMS_SUPPORT_DIMENSIONS) != jsonObjectEnd) {
        forms.supportDimensions =
            jsonObject.at(BUNDLE_MODULE_PROFILE_FORMS_SUPPORT_DIMENSIONS).get<std::vector<std::string>>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_FORMS_DEFAULT_DIMENSION) != jsonObjectEnd) {
        forms.defaultDimension = jsonObject.at(BUNDLE_MODULE_PROFILE_FORMS_DEFAULT_DIMENSION).get<std::string>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_FORMS_LANDSCAPE_LAYOUTS) != jsonObjectEnd) {
        forms.landscapeLayouts =
            jsonObject.at(BUNDLE_MODULE_PROFILE_FORMS_LANDSCAPE_LAYOUTS).get<std::vector<std::string>>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_FORMS_PORTRAIT_LAYOUTS) != jsonObjectEnd) {
        forms.portraitLayouts =
            jsonObject.at(BUNDLE_MODULE_PROFILE_FORMS_PORTRAIT_LAYOUTS).get<std::vector<std::string>>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_FORMS_UPDATEENABLED) != jsonObjectEnd) {
        forms.updateEnabled = jsonObject.at(BUNDLE_MODULE_PROFILE_FORMS_UPDATEENABLED).get<bool>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_FORMS_JS_COMPONENT_NAME) != jsonObjectEnd) {
        forms.jsComponentName = jsonObject.at(BUNDLE_MODULE_PROFILE_FORMS_JS_COMPONENT_NAME).get<std::string>();
    }

    // these are not required fields.
    if (jsonObject.find(PROFILE_KEY_DESCRIPTION) != jsonObjectEnd) {
        forms.description = jsonObject.at(PROFILE_KEY_DESCRIPTION).get<std::string>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_FORMS_COLOR_MODE) != jsonObjectEnd) {
        forms.colorMode = jsonObject.at(BUNDLE_MODULE_PROFILE_FORMS_COLOR_MODE).get<std::string>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_FORMS_SCHEDULED_UPDATE_TIME) != jsonObjectEnd) {
        forms.scheduledUpateTime = jsonObject.at(BUNDLE_MODULE_PROFILE_FORMS_SCHEDULED_UPDATE_TIME).get<int32_t>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_FORMS_UPDATE_DURATION) != jsonObjectEnd) {
        forms.updateDuration = jsonObject.at(BUNDLE_MODULE_PROFILE_FORMS_UPDATE_DURATION).get<int32_t>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_FORMS_DEEP_LINK) != jsonObjectEnd) {
        forms.deepLink = jsonObject.at(BUNDLE_MODULE_PROFILE_FORMS_DEEP_LINK).get<std::string>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_META_DATA) != jsonObjectEnd) {
        forms.metaData = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_META_DATA).get<FormsMetaData>();
    }
}

void from_json(const nlohmann::json &jsonObject, UriPermission &uriPermission)
{
    // these are not required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_MODE) != jsonObjectEnd) {
        uriPermission.mode = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_MODE).get<std::string>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_PATH) != jsonObjectEnd) {
        uriPermission.path = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_PATH).get<std::string>();
    }
}

void from_json(const nlohmann::json &jsonObject, Ability &ability)
{
    // these are required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(PROFILE_KEY_NAME) != jsonObjectEnd) {
        ability.name = jsonObject.at(PROFILE_KEY_NAME).get<std::string>();
    }

    if (jsonObject.find(PROFILE_KEY_TYPE) != jsonObjectEnd) {
        ability.type = jsonObject.at(PROFILE_KEY_TYPE).get<std::string>();
    }
    // these are not required fields.
    if (jsonObject.find(PROFILE_KEY_DESCRIPTION) != jsonObjectEnd) {
        ability.description = jsonObject.at(PROFILE_KEY_DESCRIPTION).get<std::string>();
    }

    if (jsonObject.find(PROFILE_KEY_DESCRIPTION_ID) != jsonObjectEnd) {
        ability.descriptionId = jsonObject.at(PROFILE_KEY_DESCRIPTION_ID).get<int32_t>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_ICON) != jsonObjectEnd) {
        ability.icon = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_ICON).get<std::string>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_ICON_ID) != jsonObjectEnd) {
        ability.iconId = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_ICON_ID).get<int32_t>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_PROCESS) != jsonObjectEnd) {
        ability.process = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_PROCESS).get<std::string>();
    }

    if (jsonObject.find(PROFILE_KEY_LABEL) != jsonObjectEnd) {
        ability.label = jsonObject.at(PROFILE_KEY_LABEL).get<std::string>();
    }

    if (jsonObject.find(PROFILE_KEY_LABEL_ID) != jsonObjectEnd) {
        ability.labelId = jsonObject.at(PROFILE_KEY_LABEL_ID).get<int32_t>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_URI) != jsonObjectEnd) {
        ability.uri = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_URI).get<std::string>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_LAUNCH_TYPE) != jsonObjectEnd) {
        ability.launchType = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_LAUNCH_TYPE).get<std::string>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_LAUNCH_THEME) != jsonObjectEnd) {
        ability.theme = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_LAUNCH_THEME).get<std::string>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_VISIBLE) != jsonObjectEnd) {
        ability.visible = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_VISIBLE).get<bool>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_PERMISSIONS) != jsonObjectEnd) {
        ability.permissions = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_PERMISSIONS).get<std::vector<std::string>>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_SKILLS) != jsonObjectEnd) {
        ability.skills = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_SKILLS).get<std::vector<Skill>>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_DEVICE_CAP_ABILITY) != jsonObjectEnd) {
        ability.deviceCapability =
            jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_DEVICE_CAP_ABILITY).get<std::vector<std::string>>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_META_DATA) != jsonObjectEnd) {
        ability.metaData = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_META_DATA).get<MetaData>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_FORM_ENABLED) != jsonObjectEnd) {
        ability.formEnabled = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_FORM_ENABLED).get<bool>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_FORM) != jsonObjectEnd) {
        ability.form = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_FORM).get<Form>();
    }
    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_ORIENTATION) != jsonObjectEnd) {
        ability.orientation = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_ORIENTATION).get<std::string>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_BACKGROUND_MODES) != jsonObjectEnd) {
        ability.backgroundModes =
            jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_BACKGROUND_MODES).get<std::vector<std::string>>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_GRANT_PERMISSION) != jsonObjectEnd) {
        ability.grantPermission = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_GRANT_PERMISSION).get<bool>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_URI_PERMISSION) != jsonObjectEnd) {
        ability.uriPermission = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_URI_PERMISSION).get<UriPermission>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_READ_PERMISSION) != jsonObjectEnd) {
        ability.readPermission = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_READ_PERMISSION).get<std::string>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_WRITE_PERMISSION) != jsonObjectEnd) {
        ability.writePermission = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_WRITE_PERMISSION).get<std::string>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_DIRECT_LAUNCH) != jsonObjectEnd) {
        ability.directLaunch = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_DIRECT_LAUNCH).get<bool>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_CONFIG_CHANGES) != jsonObjectEnd) {
        ability.configChanges = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_CONFIG_CHANGES).get<std::vector<std::string>>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_MISSION) != jsonObjectEnd) {
        ability.mission = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_MISSION).get<std::string>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_TARGET_ABILITY) != jsonObjectEnd) {
        ability.targetAbility = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_TARGET_ABILITY).get<std::string>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_MULTIUSER_SHARED) != jsonObjectEnd) {
        ability.multiUserShared = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_MULTIUSER_SHARED).get<bool>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_SUPPORT_PIP_MODE) != jsonObjectEnd) {
        ability.supportPipMode = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_SUPPORT_PIP_MODE).get<bool>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_FORMS_ENABLED) != jsonObjectEnd) {
        ability.formsEnabled = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_FORMS_ENABLED).get<bool>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_FORMS) != jsonObjectEnd) {
        ability.formses = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_FORMS).get<std::vector<Forms>>();
    }
}

void from_json(const nlohmann::json &jsonObject, Window &window)
{
    // these are not required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_DESIGN_WIDTH) != jsonObjectEnd) {
        window.designWidth = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_DESIGN_WIDTH).get<int32_t>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_AUTO_DESIGN_WIDTH) != jsonObjectEnd) {
        window.autoDesignWidth = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_AUTO_DESIGN_WIDTH).get<bool>();
    }
}

void from_json(const nlohmann::json &jsonObject, Js &js)
{
    // these are required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(PROFILE_KEY_NAME) != jsonObjectEnd) {
        js.name = jsonObject.at(PROFILE_KEY_NAME).get<std::string>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_PAGES) != jsonObjectEnd) {
        js.pages = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_PAGES).get<std::vector<std::string>>();
    }
    // these are not required fields.

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_WINDOW) != jsonObjectEnd) {
        js.window = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_WINDOW).get<Window>();
    }

    if (jsonObject.find(PROFILE_KEY_TYPE) != jsonObjectEnd) {
        js.type = jsonObject.at(PROFILE_KEY_TYPE).get<std::string>();
    }
}

void from_json(const nlohmann::json &jsonObject, Intent &intent)
{
    // these are not required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_TARGET_CLASS) != jsonObjectEnd) {
        intent.targetClass = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_TARGET_CLASS).get<std::string>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_TARGET_BUNDLE) != jsonObjectEnd) {
        intent.targetBundle = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_TARGET_BUNDLE).get<std::string>();
    }
}

void from_json(const nlohmann::json &jsonObject, CommonEvent &commonEvent)
{
    // these are required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(PROFILE_KEY_NAME) != jsonObjectEnd) {
        commonEvent.name = jsonObject.at(PROFILE_KEY_NAME).get<std::string>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_EVENTS) != jsonObjectEnd) {
        commonEvent.events = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_EVENTS).get<std::vector<std::string>>();
    }
    // these are not required fields.

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_PERMISSION) != jsonObjectEnd) {
        commonEvent.permission = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_PERMISSION).get<std::string>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_DATA) != jsonObjectEnd) {
        commonEvent.data = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_DATA).get<std::vector<std::string>>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_TYPE) != jsonObjectEnd) {
        commonEvent.type = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_TYPE).get<std::vector<std::string>>();
    }
}

void from_json(const nlohmann::json &jsonObject, Shortcut &shortcut)
{
    // these are required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_SHORTCUT_ID) != jsonObjectEnd) {
        shortcut.shortcutId = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_SHORTCUT_ID).get<std::string>();
    }
    // these are not required fields.

    if (jsonObject.find(PROFILE_KEY_LABEL) != jsonObjectEnd) {
        shortcut.label = jsonObject.at(PROFILE_KEY_LABEL).get<std::string>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_SHORTCUT_INTENTS) != jsonObjectEnd) {
        shortcut.intents = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_SHORTCUT_INTENTS).get<std::vector<Intent>>();
    }
}

void from_json(const nlohmann::json &jsonObject, Module &module)
{
    // these are required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_PACKAGE) != jsonObjectEnd) {
        module.package = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_PACKAGE).get<std::string>();
    }

    if (jsonObject.find(PROFILE_KEY_NAME) != jsonObjectEnd) {
        module.name = jsonObject.at(PROFILE_KEY_NAME).get<std::string>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_DEVICE_TYPE) != jsonObjectEnd) {
        module.deviceType = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_DEVICE_TYPE).get<std::vector<std::string>>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_DISTRO) != jsonObjectEnd) {
        module.distro = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_DISTRO).get<Distro>();
    }

    // these are not required fields.
    if (jsonObject.find(PROFILE_KEY_DESCRIPTION) != jsonObjectEnd) {
        module.description = jsonObject.at(PROFILE_KEY_DESCRIPTION).get<std::string>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_SUPPORTED_MODES) != jsonObjectEnd) {
        module.supportedModes =
            jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_SUPPORTED_MODES).get<std::vector<std::string>>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_REQ_CAPABILITIES) != jsonObjectEnd) {
        module.reqCapabilities =
            jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_REQ_CAPABILITIES).get<std::vector<std::string>>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_META_DATA) != jsonObjectEnd) {
        module.metaData = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_META_DATA).get<MetaData>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_ABILITIES) != jsonObjectEnd) {
        module.abilities = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_ABILITIES).get<std::vector<Ability>>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_JS) != jsonObjectEnd) {
        module.jses = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_JS).get<std::vector<Js>>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_COMMON_EVENTS) != jsonObjectEnd) {
        module.commonEvents = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_COMMON_EVENTS).get<std::vector<CommonEvent>>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_SHORTCUTS) != jsonObjectEnd) {
        module.shortcuts = jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_SHORTCUTS).get<std::vector<Shortcut>>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS) != jsonObjectEnd) {
        module.defPermissions =
            jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS).get<std::vector<DefPermission>>();
    }

    if (jsonObject.find(BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS) != jsonObjectEnd) {
        module.reqPermissions =
            jsonObject.at(BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS).get<std::vector<ReqPermission>>();
    }
}

void from_json(const nlohmann::json &jsonObject, ConfigJson &configJson)
{
    // Because it does not support exceptions, every element needs to be searched first
    APP_LOGI("read 'app' tag from config.json");
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(BUNDLE_PROFILE_KEY_APP) != jsonObjectEnd) {
        configJson.app = jsonObject.at(BUNDLE_PROFILE_KEY_APP).get<App>();
    }
    APP_LOGI("read 'device' tag from config.json");
    if (jsonObject.find(BUNDLE_PROFILE_KEY_DEVICE_CONFIG) != jsonObjectEnd) {
        configJson.deveicConfig = jsonObject.at(BUNDLE_PROFILE_KEY_DEVICE_CONFIG).get<DeviceConfig>();
    }
    APP_LOGI("read 'module' tag from config.json");
    if (jsonObject.find(BUNDLE_PROFILE_KEY_MODULE) != jsonObjectEnd) {
        configJson.module = jsonObject.at(BUNDLE_PROFILE_KEY_MODULE).get<Module>();
    }
}

}  // namespace ProfileReader

namespace {

bool CheckBundleNameIsValid(const std::string &bundleName)
{
    if (bundleName.empty()) {
        return false;
    }
    if (bundleName.size() < Constants::MIN_BUNDLE_NAME || bundleName.size() > Constants::MAX_BUNDLE_NAME) {
        return false;
    }
    char head = bundleName.at(0);
    if (head < 'A' || ('Z' < head && head < 'a') || head > 'z') {
        return false;
    }
    for (const auto &c : bundleName) {
        if (c < '.' || c == '/' || ('9' < c && c < 'A') || ('Z' < c && c < '_') || c == '`' || c > 'z') {
            return false;
        }
    }
    return true;
}

bool CheckModuleInfosIsValid(ProfileReader::ConfigJson &configJson)
{
    if (configJson.module.deviceType.empty()) {
        APP_LOGE("module deviceType invalid");
        return false;
    }
    if (!configJson.module.abilities.empty()) {
        for (const auto &ability : configJson.module.abilities) {
            if (ability.name.empty() || ability.type.empty()) {
                APP_LOGE("ability name or type invalid");
                return false;
            }
        }
    }
    if (configJson.app.version.code <= 0) {
        APP_LOGE("version code invalid");
        return false;
    }
    auto iter =
        std::find_if(configJson.module.deviceType.begin(), configJson.module.deviceType.end(), [](const auto &d) {
            return ((d.compare(ProfileReader::BUNDLE_DEVICE_CONFIG_PROFILE_KEY_LITE_WEARABLE) == 0 ||
                     d.compare(ProfileReader::BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SMART_VISION) == 0));
        });
    if (iter != configJson.module.deviceType.end()) {
        APP_LOGE("this is a lite device app, ignores other check");
        // if lite device hap doesn't have a module package name, assign it as bundle name.
        if (configJson.module.package.empty()) {
            configJson.module.package = configJson.app.bundleName;
        }
        return true;
    }
    if (configJson.module.package.empty()) {
        APP_LOGE("module package invalid");
        return false;
    }
    if (configJson.module.name.empty()) {
        APP_LOGE("module name invalid");
        return false;
    }
    if (configJson.module.distro.moduleName.empty()) {
        APP_LOGE("module distro invalid");
        return false;
    }
    return true;
}

bool TransformToInfo(const ProfileReader::ConfigJson &configJson, ApplicationInfo &applicationInfo)
{
    applicationInfo.name = configJson.app.bundleName;
    applicationInfo.bundleName = configJson.app.bundleName;
    applicationInfo.deviceId = Constants::CURRENT_DEVICE_ID;
    applicationInfo.isLauncherApp = false;
    auto it = find(configJson.module.supportedModes.begin(), configJson.module.supportedModes.end(), "drive");
    if (it != configJson.module.supportedModes.end()) {
        applicationInfo.supportedModes = 1;
    } else {
        applicationInfo.supportedModes = 0;
    }
    applicationInfo.process = configJson.deveicConfig.defaultDevice.process;
    return true;
}

bool TransformToInfo(const ProfileReader::ConfigJson &configJson, BundleInfo &bundleInfo)
{
    bundleInfo.name = configJson.app.bundleName;
    bundleInfo.vendor = configJson.app.vendor;
    bundleInfo.versionCode = static_cast<uint32_t>(configJson.app.version.code);
    bundleInfo.versionName = configJson.app.version.name;
    bundleInfo.jointUserId = configJson.deveicConfig.defaultDevice.jointUserId;
    bundleInfo.minSdkVersion = configJson.deveicConfig.defaultDevice.ark.reqVersion.compatible;
    if (configJson.deveicConfig.defaultDevice.ark.reqVersion.target == Constants::EQUAL_ZERO) {
        bundleInfo.maxSdkVersion = bundleInfo.minSdkVersion;
    } else {
        bundleInfo.minSdkVersion = configJson.deveicConfig.defaultDevice.ark.reqVersion.target;
    }
    bundleInfo.compatibleVersion = configJson.app.apiVersion.compatible;
    bundleInfo.targetVersion = configJson.app.apiVersion.target;
    bundleInfo.releaseType = configJson.app.apiVersion.releaseType;
    bundleInfo.isKeepAlive = configJson.deveicConfig.defaultDevice.keepAlive;
    if (configJson.module.abilities.size() > 0) {
        bundleInfo.label = configJson.module.abilities[0].label;
    }
    if (configJson.module.distro.moduleType == ProfileReader::MODULE_DISTRO_MODULE_TYPE_VALUE_ENTRY) {
        bundleInfo.description = configJson.module.description;
    }
    return true;
}

bool TransformToInfo(const ProfileReader::ConfigJson &configJson, InnerModuleInfo &innerModuleInfo)
{
    innerModuleInfo.modulePackage = configJson.module.package;
    innerModuleInfo.moduleName = configJson.module.name;
    innerModuleInfo.description = configJson.module.description;
    innerModuleInfo.metaData = configJson.module.metaData;
    innerModuleInfo.distro = configJson.module.distro;
    innerModuleInfo.reqCapabilities = configJson.module.reqCapabilities;
    innerModuleInfo.defPermissions = configJson.module.defPermissions;
    innerModuleInfo.reqPermissions = configJson.module.reqPermissions;
    return true;
}

bool TransformToInfo(
    const ProfileReader::ConfigJson &configJson, const ProfileReader::Ability &ability, AbilityInfo &abilityInfo)
{
    abilityInfo.name = ability.name;
    abilityInfo.label = ability.label;
    abilityInfo.description = ability.description;
    abilityInfo.iconPath = ability.icon;
    abilityInfo.visible = ability.visible;
    abilityInfo.kind = ability.type;
    auto iterType = std::find_if(std::begin(ProfileReader::ABILITY_TYPE_MAP),
        std::end(ProfileReader::ABILITY_TYPE_MAP),
        [&ability](const auto &item) { return item.first == ability.type; });
    if (iterType != ProfileReader::ABILITY_TYPE_MAP.end()) {
        abilityInfo.type = iterType->second;
    } else {
        return false;
    }

    auto iterOrientation = std::find_if(std::begin(ProfileReader::DISPLAY_ORIENTATION_MAP),
        std::end(ProfileReader::DISPLAY_ORIENTATION_MAP),
        [&ability](const auto &item) { return item.first == ability.orientation; });
    if (iterOrientation != ProfileReader::DISPLAY_ORIENTATION_MAP.end()) {
        abilityInfo.orientation = iterOrientation->second;
    }

    auto iterLaunch = std::find_if(std::begin(ProfileReader::LAUNCH_MODE_MAP),
        std::end(ProfileReader::LAUNCH_MODE_MAP),
        [&ability](const auto &item) { return item.first == ability.launchType; });
    if (iterLaunch != ProfileReader::LAUNCH_MODE_MAP.end()) {
        abilityInfo.launchMode = iterLaunch->second;
    }

    for (const auto &permission : ability.permissions) {
        abilityInfo.permissions.emplace_back(permission);
    }
    abilityInfo.process = (ability.process.empty()) ? configJson.deveicConfig.defaultDevice.process : ability.process;
    abilityInfo.deviceTypes = configJson.module.deviceType;
    abilityInfo.deviceCapabilities = ability.deviceCapability;
    abilityInfo.uri = ability.uri;
    abilityInfo.package = configJson.module.package;
    abilityInfo.bundleName = configJson.app.bundleName;
    abilityInfo.moduleName = configJson.module.name;
    abilityInfo.applicationName = configJson.app.bundleName;
    return true;
}

bool TransformToInfo(ProfileReader::ConfigJson &configJson, InnerBundleInfo &innerBundleInfo)
{
    APP_LOGD("transform profile configJson to innerBundleInfo");
    if (!CheckBundleNameIsValid(configJson.app.bundleName)) {
        APP_LOGE("bundle name is valid");
        return false;
    }
    if (!CheckModuleInfosIsValid(configJson)) {
        APP_LOGE("module infos is valid");
        return false;
    }
    ApplicationInfo applicationInfo;
    TransformToInfo(configJson, applicationInfo);
    applicationInfo.isSystemApp = (innerBundleInfo.GetAppType() == Constants::AppType::SYSTEM_APP) ? true : false;

    BundleInfo bundleInfo;
    TransformToInfo(configJson, bundleInfo);

    InnerModuleInfo innerModuleInfo;
    TransformToInfo(configJson, innerModuleInfo);

    bool find = false;
    for (const auto &ability : configJson.module.abilities) {
        AbilityInfo abilityInfo;
        if (!TransformToInfo(configJson, ability, abilityInfo)) {
            APP_LOGE("ability type is valid");
            return false;
        }
        std::string keyName = configJson.app.bundleName + configJson.module.package + abilityInfo.name;
        innerModuleInfo.abilityKeys.emplace_back(keyName);
        innerModuleInfo.skillKeys.emplace_back(keyName);
        innerBundleInfo.InsertSkillInfo(keyName, ability.skills);
        if (!find) {
            for (const auto &skill : ability.skills) {
                if (std::find(skill.actions.begin(), skill.actions.end(), Constants::INTENT_ACTION_HOME) !=
                        skill.actions.end() &&
                    std::find(skill.entities.begin(), skill.entities.end(), Constants::INTENT_ENTITY_HOME) !=
                        skill.entities.end() &&
                    (find == false)) {
                    innerBundleInfo.SetMainAbility(keyName);
                    innerBundleInfo.SetMainAbilityName(ability.name);
                    // if there is main ability, it's label will be the application's label
                    applicationInfo.label = ability.label;
                    applicationInfo.labelId = ability.labelId;
                    applicationInfo.iconPath = ability.icon;
                    applicationInfo.iconId = ability.iconId;
                    applicationInfo.description = ability.description;
                    applicationInfo.descriptionId = ability.descriptionId;
                    find = true;
                }
                if (std::find(skill.entities.begin(),
                              skill.entities.end(),
                              Constants::FLAG_HW_HOME_INTENT_FROM_SYSTEM) != skill.entities.end()) {
                    applicationInfo.isLauncherApp = true;
                    abilityInfo.isLauncherAbility = true;
                }
            }
        }
        if (configJson.module.jses.empty()) {
            bundleInfo.isNativeApp = true;
            abilityInfo.isNativeAbility = true;
        }
        innerBundleInfo.InsertAbilitiesInfo(keyName, abilityInfo);
    }

    if (configJson.module.distro.moduleType == ProfileReader::MODULE_DISTRO_MODULE_TYPE_VALUE_ENTRY) {
        innerBundleInfo.SetHasEntry(true);
        innerModuleInfo.isEntry = true;
    }
    innerBundleInfo.SetIsSupportBackup(configJson.deveicConfig.defaultDevice.supportBackup);
    innerBundleInfo.SetCurrentModulePackage(configJson.module.package);
    innerBundleInfo.SetBaseApplicationInfo(applicationInfo);
    innerBundleInfo.SetBaseBundleInfo(bundleInfo);
    innerBundleInfo.InsertInnerModuleInfo(configJson.module.package, innerModuleInfo);
    return true;
}

}  // namespace

ErrCode BundleProfile::TransformTo(const std::ostringstream &source, InnerBundleInfo &innerBundleInfo) const
{
    APP_LOGI("transform profile stream to bundle info");
    if (source.str().size() == 0) {
        return ERR_APPEXECFWK_PARSE_BAD_PROFILE;
    }

    ProfileReader::ConfigJson configJson;
    try {
        nlohmann::json jsonObject = nlohmann::json::parse(source.str());
        configJson = jsonObject.get<ProfileReader::ConfigJson>();
    } catch (nlohmann::detail::parse_error &exception) {
        APP_LOGE("has a parse_error:%{public}s", exception.what());
        return ERR_APPEXECFWK_PARSE_BAD_PROFILE;
    } catch (nlohmann::detail::type_error &exception) {
        APP_LOGE("has a type_error:%{public}s", exception.what());
        return ERR_APPEXECFWK_PARSE_PROFILE_PROP_TYPE_ERROR;
    } catch (nlohmann::detail::out_of_range &exception) {
        APP_LOGE("has an out_of_range exception:%{public}s", exception.what());
        return ERR_APPEXECFWK_PARSE_PROFILE_MISSING_PROP;
    } catch (...) {
        APP_LOGE("has an other exception");
        return ERR_APPEXECFWK_PARSE_PROFILE_MISSING_PROP;
    }

    if (!TransformToInfo(configJson, innerBundleInfo)) {
        return ERR_APPEXECFWK_PARSE_BAD_PROFILE;
    }
    return ERR_OK;
}

}  // namespace AppExecFwk
}  // namespace OHOS
