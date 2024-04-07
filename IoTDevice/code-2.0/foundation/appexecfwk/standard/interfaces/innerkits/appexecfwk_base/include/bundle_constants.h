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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_BUNDLE_CONSTANTS_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_BUNDLE_CONSTANTS_H

#include <string>

namespace OHOS {
namespace AppExecFwk {
namespace Constants {

const std::string EMPTY_STRING = "";
const std::string BUNDLE_PROFILE_NAME = "config.json";
const std::string INSTALL_FILE_SUFFIX = ".hap";
const std::string PATH_SEPARATOR = "/";
const std::string FILE_UNDERLINE = "_";
const std::string ILLEGAL_PATH_FIELD = "../";
const char DOT_SUFFIX = '.';
const std::string CURRENT_DEVICE_ID = "PHONE-001";
const std::string BUNDLE_DATA_BASE_DIR = "/data/bundlemgr";
const std::string BUNDLE_DATA_BASE_FILE = BUNDLE_DATA_BASE_DIR + "/bmsdb.json";
const std::string SYSTEM_APP_SCAN_PATH = "/system/app";
const std::string SYSTEM_APP_INSTALL_PATH = "/data/accounts";
const std::string THIRD_SYSTEM_APP_SCAN_PATH = "/system/vendor";
const std::string THIRD_SYSTEM_APP_INSTALL_PATH = "/data/accounts";
const std::string THIRD_PARTY_APP_INSTALL_PATH = "/data/accounts";
const std::string EXTRACT_TMP_PATH = "/data/sadata/install_tmp/bundle_haps";
const std::string HAP_COPY_PATH = "/data/sadata/install_tmp/Tmp_";
const std::string USER_ACCOUNT_DIR = "account";
const std::string APP_CODE_DIR = "applications";
const std::string APP_DATA_DIR = "appdata";
const std::string DATA_BASE_DIR = "database";
const std::string DATA_DIR = "files";
const std::string CACHE_DIR = "cache";
const std::string SHARED_DIR = "shared";
const std::string SHARED_PREFERENCE_DIR = "sharedPreference";
const std::string TMP_SUFFIX = "_tmp";
const std::string ASSETS_DIR = "assets";
const std::string RESOURCES_INDEX = "resources.index";

const std::string BMS_SERVICE_NAME = "BundleMgrService";
const std::string INSTALLD_SERVICE_NAME = "installd";
const std::string SYSTEM_APP = "system";
const std::string THIRD_PARTY_APP = "third-party";
constexpr int DEFAULT_USERID = 0;
constexpr int INVALID_USERID = -1;
constexpr int PATH_MAX_SIZE = 256;
constexpr int SIGNATURE_MATCHED = 0;
constexpr int SIGNATURE_NOT_MATCHED = 1;
constexpr int SIGNATURE_UNKNOWN_BUNDLE = 2;
constexpr int PERMISSION_GRANTED = 0;
constexpr int PERMISSION_NOT_GRANTED = -1;
constexpr int DUMP_INDENT = 4;
constexpr unsigned int INSTALLD_UMASK = 0000;

// uid and gid
constexpr int32_t INVALID_UID = -1;
constexpr int32_t INVALID_GID = -1;
constexpr int32_t ROOT_UID = 0;
constexpr int32_t BMS_UID = 1000;
constexpr int32_t BMS_GID = 1000;
constexpr int32_t BASE_SYS_UID = 2100;
constexpr int32_t MAX_SYS_UID = 2899;
constexpr int32_t BASE_SYS_VEN_UID = 5000;
constexpr int32_t MAX_SYS_VEN_UID = 5999;
constexpr int32_t BASE_APP_UID = 10000;
constexpr int32_t MAX_APP_UID = 65535;
const std::string PROFILE_KEY_UID_SIZE = "size";
const std::string PROFILE_KEY_UID_AND_GID = "uid_and_gid";

// permissions
const std::string PERMISSION_INSTALL_BUNDLE = "ohos.permission.INSTALL_BUNDLE";

enum class AppType {
    SYSTEM_APP = 0,
    THIRD_SYSTEM_APP,
    THIRD_PARTY_APP,
};

const std::string INTENT_ACTION_HOME = "action.system.home";
const std::string INTENT_ENTITY_HOME = "entity.system.home";
const std::string FLAG_HW_HOME_INTENT_FROM_SYSTEM = "flag.home.intent.from.system";

// the ability file folder name.
const std::string LIB_FOLDER_NAME = "libs";
const std::string RES_FOLDER_NAME = "resources";

constexpr uint8_t MAX_LABLE_LEN = 30;
constexpr uint8_t MAX_BUNDLE_NAME = 255;
constexpr uint8_t MIN_BUNDLE_NAME = 7;
constexpr uint8_t MAX_VENDOR = 255;
constexpr uint8_t EQUAL_ZERO = 0;
constexpr uint8_t MAX_MODULE_PACKAGE = 127;
constexpr uint8_t MAX_MODULE_NAME = 255;
constexpr uint8_t MAX_MODULE_ABILITIES_READPERMISSION = 255;
constexpr uint8_t MAX_MODULE_ABILITIES_WRITEPERMISSION = 255;
constexpr uint8_t MAX_MODULE_SHORTCUTID = 63;
constexpr uint8_t MAX_MODULE_LABEL = 63;

}  // namespace Constants
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_BUNDLE_CONSTANTS_H