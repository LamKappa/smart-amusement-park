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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_COMMON_PROFILE_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_COMMON_PROFILE_H

#include <string>

#include "nlohmann/json.hpp"

namespace OHOS {
namespace AppExecFwk {
namespace ProfileReader {

// commen tag
const std::string PROFILE_KEY_NAME = "name";
const std::string PROFILE_KEY_LABEL = "label";
const std::string PROFILE_KEY_LABEL_ID = "labelId";
const std::string PROFILE_KEY_DESCRIPTION = "description";
const std::string PROFILE_KEY_DESCRIPTION_ID = "descriptionId";
const std::string PROFILE_KEY_TYPE = "type";

// bundle profile tag
const std::string BUNDLE_PROFILE_KEY_APP = "app";
const std::string BUNDLE_PROFILE_KEY_DEVICE_CONFIG = "deviceConfig";
const std::string BUNDLE_PROFILE_KEY_MODULE = "module";
// sub  BUNDLE_PROFILE_KEY_APP
const std::string BUNDLE_APP_PROFILE_KEY_BUNDLE_NAME = "bundleName";
const std::string BUNDLE_APP_PROFILE_KEY_VENDOR = "vendor";
const std::string BUNDLE_APP_PROFILE_KEY_VERSION = "version";
const std::string BUNDLE_APP_PROFILE_KEY_API_VERSION = "apiVersion";
// sub BUNDLE_APP_PROFILE_KEY_VERSION
const std::string BUNDLE_APP_PROFILE_KEY_CODE = "code";
// sub BUNDLE_APP_PROFILE_KEY_API_VERSION
const std::string BUNDLE_APP_PROFILE_KEY_COMPATIBLE = "compatible";
const std::string BUNDLE_APP_PROFILE_KEY_TARGET = "target";
const std::string BUNDLE_APP_PROFILE_KEY_RELEASE_TYPE = "releaseType";
const std::string APP_RELEASE_TYPE_VALUE_RELEASE = "Release";
// sub  BUNDLE_PROFILE_KEY_DEVICE_CONFIG
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DEFAULT = "default";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_PHONE = "phone";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_TABLET = "tablet";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_TV = "tv";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_CAR = "car";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_WEARABLE = "wearable";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_LITE_WEARABLE = "liteWearable";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SMART_VISION = "smartVision";
// sub BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DEFAULT
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_JOINT_USER_ID = "jointUserId";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_PROCESS = "process";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_KEEP_ALIVE = "keepAlive";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_ARK = "ark";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DIRECT_LAUNCH = "directLaunch";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SUPPORT_BACKUP = "supportBackup";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_COMPRESS_NATIVE_LIBS = "compressNativeLibs";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_NETWORK = "network";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_REQ_VERSION = "reqVersion";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_FLAG = "flag";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_COMPATIBLE = "compatible";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_TARGET = "target";
// sub BUNDLE_DEVICE_CONFIG_PROFILE_KEY_NETWORK
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_USES_CLEAR_TEXT = "usesCleartext";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SECURITY_CONFIG = "securityConfig";
// sub BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SECURITY_CONFIG
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DOMAIN_SETTINGS = "domainSettings";
// sub BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DOMAIN_SETTINGS
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_CLEAR_TEXT_PERMITTED = "cleartextPermitted";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DOMAINS = "domains";
// sub BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DOMAINS
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SUB_DOMAINS = "subDomains";
// sub BUNDLE_PROFILE_KEY_MODULE
const std::string BUNDLE_MODULE_PROFILE_KEY_PACKAGE = "package";
const std::string BUNDLE_MODULE_PROFILE_KEY_SUPPORTED_MODES = "supportedModes";
const std::string BUNDLE_MODULE_PROFILE_KEY_REQ_CAPABILITIES = "reqCapabilities";
const std::string BUNDLE_MODULE_PROFILE_KEY_SUPPORTED_REQ_CAPABILITIES = "reqCapabilities";
const std::string MODULE_SUPPORTED_MODES_VALUE_DRIVE = "drive";
const std::string BUNDLE_MODULE_PROFILE_KEY_DEVICE_TYPE = "deviceType";
const std::string BUNDLE_MODULE_PROFILE_KEY_DISTRO = "distro";
const std::string BUNDLE_MODULE_PROFILE_KEY_META_DATA = "metaData";
const std::string BUNDLE_MODULE_PROFILE_KEY_ABILITIES = "abilities";
const std::string BUNDLE_MODULE_PROFILE_KEY_JS = "js";
const std::string BUNDLE_MODULE_PROFILE_KEY_COMMON_EVENTS = "commonEvents";
const std::string BUNDLE_MODULE_PROFILE_KEY_SHORTCUTS = "shortcuts";
const std::string BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS = "defPermissions";
const std::string BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_NAME = "name";
const std::string BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_GRANTMODE = "grantMode";
const std::string BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_GRANTMODE_USER_GRANT = "user_grant";
const std::string BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_GRANTMODE_SYSTEM_GRANT = "system_grant";
const std::string BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_AVAILABLESCOPE = "availableScope";
const std::string BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_AVAILABLESCOPE_SIGNATURE = "signature";
const std::string BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_AVAILABLESCOPE_PRIVILEGED = "privileged";
const std::string BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_AVAILABLESCOPE_RESTRICTED = "restricted";
const std::string BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_LABEL = "label";
const std::string BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_LABEL_ID = "labelId";
const std::string BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_DESCRIPTION = "description";
const std::string BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_DESCRIPTION_ID = "descriptionId";
const std::string BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS = "reqPermissions";
const std::string BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS_NAME = "name";
const std::string BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS_REASON = "reason";
const std::string BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS_USEDSCENE = "usedScene";
const std::string BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS_ABILITY = "ability";
const std::string BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS_WHEN = "when";
const std::string BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS_WHEN_INUSE = "inuse";
const std::string BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS_WHEN_ALWAYS = "always";
const std::string BUNDLE_MODULE_PROFILE_KEY_CUSTOMIZE_DATA = "customizeData";
// sub BUNDLE_MODULE_PROFILE_KEY_DISTRO
const std::string BUNDLE_MODULE_PROFILE_KEY_DELIVERY_WITH_INSTALL = "deliveryWithInstall";
const std::string BUNDLE_MODULE_PROFILE_KEY_MODULE_NAME = "moduleName";
const std::string BUNDLE_MODULE_PROFILE_KEY_MODULE_TYPE = "moduleType";
// sub BUNDLE_MODULE_PROFILE_KEY_SKILLS
const std::string BUNDLE_MODULE_PROFILE_KEY_ACTIONS = "actions";
const std::string BUNDLE_MODULE_PROFILE_KEY_ENTITIES = "entities";
const std::string BUNDLE_MODULE_PROFILE_KEY_URIS = "uris";
// sub BUNDLE_MODULE_PROFILE_KEY_URIS
const std::string BUNDLE_MODULE_PROFILE_KEY_SCHEME = "scheme";
const std::string BUNDLE_MODULE_PROFILE_KEY_HOST = "host";
const std::string BUNDLE_MODULE_PROFILE_KEY_PORT = "port";
const std::string BUNDLE_MODULE_PROFILE_KEY_PATH = "path";
const std::string BUNDLE_MODULE_PROFILE_KEY_TYPE = "type";
// sub BUNDLE_MODULE_PROFILE_KEY_META_DATA
const std::string BUNDLE_MODULE_META_KEY_NAME = "name";
const std::string BUNDLE_MODULE_META_KEY_DESCRIPTION = "description";
const std::string BUNDLE_MODULE_META_KEY_PARAMETERS = "parameters";
const std::string BUNDLE_MODULE_META_KEY_RESULTS = "results";
const std::string BUNDLE_MODULE_META_KEY_CUSTOMIZE_DATA = "customizeData";
const std::string BUNDLE_MODULE_META_KEY_VALUE = "value";
const std::string BUNDLE_MODULE_META_KEY_EXTRA = "extra";
// sub BUNDLE_MODULE_PROFILE_KEY_DISTRO_TYPE
const std::string MODULE_DISTRO_MODULE_TYPE_VALUE_ENTRY = "entry";
const std::string MODULE_DISTRO_MODULE_TYPE_VALUE_FEATURE = "feature";
// sub BUNDLE_MODULE_PROFILE_KEY_ABILITIES
const std::string BUNDLE_MODULE_PROFILE_KEY_ICON = "icon";
const std::string BUNDLE_MODULE_PROFILE_KEY_ICON_ID = "iconId";
const std::string BUNDLE_MODULE_PROFILE_KEY_URI = "uri";
const std::string BUNDLE_MODULE_PROFILE_KEY_LAUNCH_TYPE = "launchType";
const std::string BUNDLE_MODULE_PROFILE_KEY_LAUNCH_THEME = "theme";
const std::string BUNDLE_MODULE_PROFILE_KEY_VISIBLE = "visible";
const std::string BUNDLE_MODULE_PROFILE_KEY_PERMISSIONS = "permissions";
const std::string BUNDLE_MODULE_PROFILE_KEY_SKILLS = "skills";
const std::string BUNDLE_MODULE_PROFILE_KEY_PROCESS = "process";
const std::string BUNDLE_MODULE_PROFILE_KEY_DEVICE_CAP_ABILITY = "deviceCapability";
const std::string BUNDLE_MODULE_PROFILE_KEY_FORM_ENABLED = "formEnabled";
const std::string BUNDLE_MODULE_PROFILE_KEY_FORM = "form";
const std::string BUNDLE_MODULE_PROFILE_KEY_ORIENTATION = "orientation";
const std::string BUNDLE_MODULE_PROFILE_KEY_BACKGROUND_MODES = "backgroundModes";
const std::string BUNDLE_MODULE_PROFILE_KEY_GRANT_PERMISSION = "grantPermission";
const std::string BUNDLE_MODULE_PROFILE_KEY_URI_PERMISSION = "uriPermission";
const std::string BUNDLE_MODULE_PROFILE_KEY_READ_PERMISSION = "readPermission";
const std::string BUNDLE_MODULE_PROFILE_KEY_WRITE_PERMISSION = "writePermission";
const std::string BUNDLE_MODULE_PROFILE_KEY_DIRECT_LAUNCH = "directLaunch";
const std::string BUNDLE_MODULE_PROFILE_KEY_CONFIG_CHANGES = "configChanges";
const std::string BUNDLE_MODULE_PROFILE_KEY_MISSION = "mission";
const std::string BUNDLE_MODULE_PROFILE_KEY_TARGET_ABILITY = "targetAbility";
const std::string BUNDLE_MODULE_PROFILE_KEY_MULTIUSER_SHARED = "multiUserShared";
const std::string BUNDLE_MODULE_PROFILE_KEY_SUPPORT_PIP_MODE = "supportPipMode";
const std::string BUNDLE_MODULE_PROFILE_KEY_FORMS_ENABLED = "formsEnabled";
const std::string BUNDLE_MODULE_PROFILE_KEY_FORMS = "forms";
// sub BUNDLE_MODULE_PROFILE_KEY_FORM
const std::string BUNDLE_MODULE_PROFILE_KEY_MODE = "mode";
// sub BUNDLE_MODULE_PROFILE_KEY_FORM
const std::string BUNDLE_MODULE_PROFILE_FORM_ENTITY = "formEntity";
const std::string BUNDLE_MODULE_PROFILE_FORM_MIN_HEIGHT = "minHeight";
const std::string BUNDLE_MODULE_PROFILE_FORM_DEFAULT_HEIGHT = "defaultHeight";
const std::string BUNDLE_MODULE_PROFILE_FORM_MIN_WIDTH = "minWidth";
const std::string BUNDLE_MODULE_PROFILE_FORM_DEFAULT_WIDTH = "defaultWidth";
// sub BUNDLE_MODULE_PROFILE_KEY_FORMS
const std::string BUNDLE_MODULE_PROFILE_FORMS_IS_DEFAULT = "isDefault";
const std::string BUNDLE_MODULE_PROFILE_FORMS_COLOR_MODE = "colorMode";
const std::string BUNDLE_MODULE_PROFILE_FORMS_SUPPORT_DIMENSIONS = "supportDimensions";
const std::string BUNDLE_MODULE_PROFILE_FORMS_DEFAULT_DIMENSION = "defaultDimension";
const std::string BUNDLE_MODULE_PROFILE_FORMS_LANDSCAPE_LAYOUTS = "landscapeLayouts";
const std::string BUNDLE_MODULE_PROFILE_FORMS_PORTRAIT_LAYOUTS = "portraitLayouts";
const std::string BUNDLE_MODULE_PROFILE_FORMS_UPDATEENABLED = "updateEnabled";
const std::string BUNDLE_MODULE_PROFILE_FORMS_SCHEDULED_UPDATE_TIME = "scheduledUpateTime";
const std::string BUNDLE_MODULE_PROFILE_FORMS_UPDATE_DURATION = "updateDuration";
const std::string BUNDLE_MODULE_PROFILE_FORMS_DEEP_LINK = "deepLink";
const std::string BUNDLE_MODULE_PROFILE_FORMS_JS_COMPONENT_NAME = "jsComponentName";
const std::string BUNDLE_MODULE_PROFILE_FORMS_VALUE = "value";
// sub BUNDLE_MODULE_PROFILE_KEY_JS
const std::string BUNDLE_MODULE_PROFILE_KEY_PAGES = "pages";
const std::string BUNDLE_MODULE_PROFILE_KEY_WINDOW = "window";
// sub BUNDLE_MODULE_PROFILE_KEY_COMMON_EVENTS
const std::string BUNDLE_MODULE_PROFILE_KEY_PERMISSION = "permission";
const std::string BUNDLE_MODULE_PROFILE_KEY_DATA = "data";
const std::string BUNDLE_MODULE_PROFILE_KEY_EVENTS = "events";
const std::string MODULE_ABILITY_JS_TYPE_VALUE_NORMAL = "normal";
const std::string MODULE_ABILITY_JS_TYPE_VALUE_FORM = "form";
// sub BUNDLE_MODULE_PROFILE_KEY_WINDOW
const std::string BUNDLE_MODULE_PROFILE_KEY_DESIGN_WIDTH = "designWidth";
const std::string BUNDLE_MODULE_PROFILE_KEY_AUTO_DESIGN_WIDTH = "autoDesignWidth";
// sub BUNDLE_MODULE_PROFILE_KEY_SHORTCUTS
const std::string BUNDLE_MODULE_PROFILE_KEY_SHORTCUT_ID = "shortcutId";
const std::string BUNDLE_MODULE_PROFILE_KEY_SHORTCUT_INTENTS = "intents";
// sub BUNDLE_MODULE_PROFILE_KEY_SHORTCUT_INTENTS
const std::string BUNDLE_MODULE_PROFILE_KEY_TARGET_CLASS = "targetClass";
const std::string BUNDLE_MODULE_PROFILE_KEY_TARGET_BUNDLE = "targetBundle";

}  // namespace ProfileReader
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_COMMON_PROFILE_H
