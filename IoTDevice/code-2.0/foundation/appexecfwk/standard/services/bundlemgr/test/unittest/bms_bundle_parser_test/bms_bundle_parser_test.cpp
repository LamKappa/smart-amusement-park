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

#include <sstream>
#include <string>
#include <gtest/gtest.h>

#include "app_log_wrapper.h"
#include "common_profile.h"
#include "json_constants.h"
#include "bundle_extractor.h"
#include "bundle_constants.h"
#include "bundle_parser.h"
#include "bundle_profile.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;
using namespace OHOS::AppExecFwk::Constants;
using namespace OHOS::AppExecFwk::ProfileReader;

namespace {

const std::string RESOURCE_ROOT_PATH = "/data/test/resource/bms/parse_bundle/";
const std::string NEW_APP = "new";
const std::string BREAK_ZIP = "break_zip";
const std::string NO_PROFILE = "no_profile";
const std::string EMPTY_CONFIG = "empty_config";
const std::string NOTHING_CONFIG = "nothing_config";
const std::string FORMAT_ERROR_PROFILE = "format_error_profile";
const std::string FORMAT_MISSING_PROFILE = "format_missing_profile";
const std::string UNKOWN_PATH = "unknown_path";
const nlohmann::json CONFIG_JSON = R"(
    {
        "app": {
            "bundleName": "com.example.hiworld.himusic",
            "vendor": "example",
            "version": {
                "code": 2,
                "name": "2.0"
            },
            "apiVersion": {
                "compatible": 3,
                "target": 3,
                "releaseType": "Beta1"
            }
        },
        "deviceConfig": {
            "default": {
            }
        },
        "module": {
            "package": "com.example.hiworld.himusic.entry",
            "name": ".MainApplication",
            "supportedModes": [
                "drive"
            ],
            "distro": {
                "moduleType": "entry",
                "deliveryWithInstall": true,
                "moduleName": "hap-car"
            },
            "deviceType": [
                "car"
            ],
            "abilities": [
                {
                    "name": ".MainAbility",
                    "description": "himusic main ability",
                    "icon": "$media:ic_launcher",
                    "label": "HiMusic",
                    "launchType": "standard",
                    "orientation": "unspecified",
                    "visible": true,
                    "skills": [
                        {
                            "actions": [
                                "action.system.home"
                            ],
                            "entities": [
                                "entity.system.home"
                            ]
                        }
                    ],
                    "type": "page",
                    "formEnabled": false
                },
                {
                    "name": ".PlayService",
                    "description": "himusic play ability",
                    "icon": "$media:ic_launcher",
                    "label": "HiMusic",
                    "launchType": "standard",
                    "orientation": "unspecified",
                    "visible": false,
                    "skills": [
                        {
                            "actions": [
                                "action.play.music",
                                "action.stop.music"
                            ],
                            "entities": [
                                "entity.audio"
                            ]
                        }
                    ],
                    "type": "service",
                    "backgroundModes": [
                        "audioPlayback"
                    ]
                },
                {
                    "name": ".UserADataAbility",
                    "type": "data",
                    "uri": "dataability://com.example.hiworld.himusic.UserADataAbility",
                    "visible": true
                }
            ],
            "reqPermissions": [
                {
                    "name": "ohos.permission.DISTRIBUTED_DATASYNC",
                    "reason": "",
                    "usedScene": {
                        "ability": [
                            "com.example.hiworld.himusic.entry.MainAbility",
                            "com.example.hiworld.himusic.entry.PlayService"
                        ],
                        "when": "inuse"
                    }
                }
            ]
        }
    }
)"_json;

}  // namespace

class BmsBundleParserTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    void GetProfileTypeErrorProps(nlohmann::json &typeErrorProps) const;
    void CheckNoPropProfileParseApp(const std::string &propKey, const ErrCode expectCode) const;
    void CheckNoPropProfileParseDeviceConfig(const std::string &propKey, const ErrCode expectCode) const;
    void CheckNoPropProfileParseModule(const std::string &propKey, const ErrCode expectCode) const;
    void CheckProfilePermission(const nlohmann::json &checkedProfileJson) const;

protected:
    std::ostringstream pathStream_;
};

void BmsBundleParserTest::SetUpTestCase()
{}

void BmsBundleParserTest::TearDownTestCase()
{}

void BmsBundleParserTest::SetUp()
{}

void BmsBundleParserTest::TearDown()
{
    pathStream_.clear();
}

void BmsBundleParserTest::GetProfileTypeErrorProps(nlohmann::json &typeErrorProps) const
{
    typeErrorProps[PROFILE_KEY_NAME] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[PROFILE_KEY_LABEL] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[PROFILE_KEY_DESCRIPTION] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[PROFILE_KEY_TYPE] = JsonConstants::NOT_STRING_TYPE;
    // bundle profile tag
    typeErrorProps[BUNDLE_PROFILE_KEY_APP] = JsonConstants::NOT_OBJECT_TYPE;
    typeErrorProps[BUNDLE_PROFILE_KEY_DEVICE_CONFIG] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_PROFILE_KEY_MODULE] = JsonConstants::NOT_OBJECT_TYPE;
    // sub BUNDLE_PROFILE_KEY_APP
    typeErrorProps[BUNDLE_APP_PROFILE_KEY_BUNDLE_NAME] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_APP_PROFILE_KEY_VENDOR] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_APP_PROFILE_KEY_VERSION] = JsonConstants::NOT_OBJECT_TYPE;
    typeErrorProps[BUNDLE_APP_PROFILE_KEY_API_VERSION] = JsonConstants::NOT_OBJECT_TYPE;
    // BUNDLE_APP_PROFILE_KEY_VERSION
    typeErrorProps[BUNDLE_APP_PROFILE_KEY_CODE] = JsonConstants::NOT_NUMBER_TYPE;
    // BUNDLE_APP_PROFILE_KEY_API_VERSION
    typeErrorProps[BUNDLE_APP_PROFILE_KEY_COMPATIBLE] = JsonConstants::NOT_NUMBER_TYPE;
    typeErrorProps[BUNDLE_APP_PROFILE_KEY_TARGET] = JsonConstants::NOT_NUMBER_TYPE;
    typeErrorProps[BUNDLE_APP_PROFILE_KEY_RELEASE_TYPE] = JsonConstants::NOT_STRING_TYPE;
    // sub BUNDLE_PROFILE_KEY_DEVICE_CONFIG
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DEFAULT] = JsonConstants::NOT_OBJECT_TYPE;
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_PHONE] = JsonConstants::NOT_OBJECT_TYPE;
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_TABLET] = JsonConstants::NOT_OBJECT_TYPE;
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_TV] = JsonConstants::NOT_OBJECT_TYPE;
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_CAR] = JsonConstants::NOT_OBJECT_TYPE;
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_WEARABLE] = JsonConstants::NOT_OBJECT_TYPE;
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_LITE_WEARABLE] = JsonConstants::NOT_OBJECT_TYPE;
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SMART_VISION] = JsonConstants::NOT_OBJECT_TYPE;
    // BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DEFAULT
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_PROCESS] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DIRECT_LAUNCH] = JsonConstants::NOT_BOOL_TYPE;
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SUPPORT_BACKUP] = JsonConstants::NOT_BOOL_TYPE;
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_COMPRESS_NATIVE_LIBS] = JsonConstants::NOT_BOOL_TYPE;
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_NETWORK] = JsonConstants::NOT_OBJECT_TYPE;
    // BUNDLE_DEVICE_CONFIG_PROFILE_KEY_NETWORK
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_USES_CLEAR_TEXT] = JsonConstants::NOT_BOOL_TYPE;
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SECURITY_CONFIG] = JsonConstants::NOT_OBJECT_TYPE;
    // BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SECURITY_CONFIG
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DOMAIN_SETTINGS] = JsonConstants::NOT_OBJECT_TYPE;
    // BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DOMAIN_SETTINGS
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_CLEAR_TEXT_PERMITTED] = JsonConstants::NOT_BOOL_TYPE;
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DOMAINS] = JsonConstants::NOT_ARRAY_TYPE;
    // BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DOMAINS
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SUB_DOMAINS] = JsonConstants::NOT_ARRAY_TYPE;
    // sub BUNDLE_PROFILE_KEY_MODULE
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_PACKAGE] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_SUPPORTED_MODES] = JsonConstants::NOT_ARRAY_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_DEVICE_TYPE] = JsonConstants::NOT_ARRAY_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_DISTRO] = JsonConstants::NOT_OBJECT_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_ABILITIES] = JsonConstants::NOT_ARRAY_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_JS] = JsonConstants::NOT_OBJECT_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_SHORTCUTS] = JsonConstants::NOT_ARRAY_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS] = JsonConstants::NOT_ARRAY_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS] = JsonConstants::NOT_ARRAY_TYPE;
    // BUNDLE_MODULE_PROFILE_KEY_DISTRO
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_DELIVERY_WITH_INSTALL] = JsonConstants::NOT_BOOL_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_MODULE_NAME] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_MODULE_TYPE] = JsonConstants::NOT_STRING_TYPE;
    // BUNDLE_MODULE_PROFILE_KEY_ABILITIES
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_ICON] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_URI] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_LAUNCH_TYPE] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_VISIBLE] = JsonConstants::NOT_BOOL_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_PERMISSIONS] = JsonConstants::NOT_ARRAY_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_SKILLS] = JsonConstants::NOT_ARRAY_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_DEVICE_CAP_ABILITY] = JsonConstants::NOT_ARRAY_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_ORIENTATION] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_BACKGROUND_MODES] = JsonConstants::NOT_ARRAY_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_READ_PERMISSION] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_WRITE_PERMISSION] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_DIRECT_LAUNCH] = JsonConstants::NOT_BOOL_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_CONFIG_CHANGES] = JsonConstants::NOT_ARRAY_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_MISSION] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_TARGET_ABILITY] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_MULTIUSER_SHARED] = JsonConstants::NOT_BOOL_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_SUPPORT_PIP_MODE] = JsonConstants::NOT_BOOL_TYPE;
    // BUNDLE_MODULE_PROFILE_KEY_SKILLS
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_ACTIONS] = JsonConstants::NOT_ARRAY_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_ENTITIES] = JsonConstants::NOT_ARRAY_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_URIS] = JsonConstants::NOT_ARRAY_TYPE;
    // BUNDLE_MODULE_PROFILE_KEY_URIS
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_SCHEME] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_HOST] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_PORT] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_PATH] = JsonConstants::NOT_STRING_TYPE;
    // BUNDLE_MODULE_PROFILE_KEY_JS
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_PAGES] = JsonConstants::NOT_ARRAY_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_WINDOW] = JsonConstants::NOT_OBJECT_TYPE;
    // BUNDLE_MODULE_PROFILE_KEY_WINDOW
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_DESIGN_WIDTH] = JsonConstants::NOT_NUMBER_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_AUTO_DESIGN_WIDTH] = JsonConstants::NOT_BOOL_TYPE;
    // BUNDLE_MODULE_PROFILE_KEY_SHORTCUTS
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_SHORTCUT_ID] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_SHORTCUT_INTENTS] = JsonConstants::NOT_STRING_TYPE;
    // BUNDLE_MODULE_PROFILE_KEY_SHORTCUT_INTENTS
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_TARGET_CLASS] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_TARGET_BUNDLE] = JsonConstants::NOT_STRING_TYPE;
}

void BmsBundleParserTest::CheckNoPropProfileParseApp(const std::string &propKey, const ErrCode expectCode) const
{
    BundleProfile bundleProfile;
    InnerBundleInfo innerBundleInfo;
    std::ostringstream profileFileBuffer;

    nlohmann::json errorProfileJson = CONFIG_JSON;
    errorProfileJson[BUNDLE_PROFILE_KEY_APP].erase(propKey);
    profileFileBuffer << errorProfileJson.dump();

    ErrCode result = bundleProfile.TransformTo(profileFileBuffer, innerBundleInfo);
    EXPECT_EQ(result, expectCode);
}

void BmsBundleParserTest::CheckNoPropProfileParseDeviceConfig(
    const std::string &propKey, const ErrCode expectCode) const
{
    BundleProfile bundleProfile;
    InnerBundleInfo innerBundleInfo;
    std::ostringstream profileFileBuffer;

    nlohmann::json errorProfileJson = CONFIG_JSON;
    errorProfileJson[BUNDLE_PROFILE_KEY_DEVICE_CONFIG].erase(propKey);
    profileFileBuffer << errorProfileJson.dump();

    ErrCode result = bundleProfile.TransformTo(profileFileBuffer, innerBundleInfo);
    EXPECT_EQ(result, expectCode);
}

void BmsBundleParserTest::CheckNoPropProfileParseModule(const std::string &propKey, const ErrCode expectCode) const
{
    BundleProfile bundleProfile;
    InnerBundleInfo innerBundleInfo;
    std::ostringstream profileFileBuffer;

    nlohmann::json errorProfileJson = CONFIG_JSON;
    errorProfileJson[BUNDLE_PROFILE_KEY_MODULE].erase(propKey);
    profileFileBuffer << errorProfileJson.dump();

    ErrCode result = bundleProfile.TransformTo(profileFileBuffer, innerBundleInfo);
    EXPECT_EQ(result, expectCode);
}

void BmsBundleParserTest::CheckProfilePermission(const nlohmann::json &checkedProfileJson) const
{
    BundleProfile bundleProfile;
    InnerBundleInfo innerBundleInfo;
    std::ostringstream profileFileBuffer;

    profileFileBuffer << checkedProfileJson.dump();

    ErrCode result = bundleProfile.TransformTo(profileFileBuffer, innerBundleInfo);
    EXPECT_EQ(result, 0) << profileFileBuffer.str();
}

/**
 * @tc.number: BmsBundleParser
 * Function: BundleParser
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test bundle package can be parse to InnerBundleInfo successfully
 */
HWTEST_F(BmsBundleParserTest, TestParse_0100, Function | SmallTest | Level0)
{
    BundleParser bundleParser;
    InnerBundleInfo innerBundleInfo;
    pathStream_ << RESOURCE_ROOT_PATH << NEW_APP << INSTALL_FILE_SUFFIX;
    ErrCode result = bundleParser.Parse(pathStream_.str(), innerBundleInfo);
    BundleInfo bundleInfo = innerBundleInfo.GetBaseBundleInfo();
    EXPECT_EQ(result, ERR_OK);
    EXPECT_EQ(bundleInfo.name, "com.example.hiworld.himusic");
    EXPECT_EQ(bundleInfo.label, "HiMusic");
    EXPECT_EQ(bundleInfo.description, "");
    EXPECT_EQ(bundleInfo.vendor, "example");
    uint32_t versionCode = 2;
    EXPECT_EQ(bundleInfo.versionCode, versionCode);
    EXPECT_EQ(bundleInfo.versionName, "2.0");
    EXPECT_EQ(bundleInfo.minSdkVersion, 0);
    EXPECT_EQ(bundleInfo.maxSdkVersion, 0);
    EXPECT_EQ(bundleInfo.mainEntry, "");
}

/**
 * @tc.number: TestParse_0200
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parse bundle failed when file is not exist by the input pathName
 */
HWTEST_F(BmsBundleParserTest, TestParse_0200, Function | SmallTest | Level0)
{
    BundleParser bundleParser;
    InnerBundleInfo innerBundleInfo;
    pathStream_ << RESOURCE_ROOT_PATH << UNKOWN_PATH << INSTALL_FILE_SUFFIX;
    ErrCode result = bundleParser.Parse(pathStream_.str(), innerBundleInfo);
    EXPECT_EQ(result, ERR_APPEXECFWK_PARSE_UNEXPECTED);
}

/**
 * @tc.number: TestParse_0300
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parse bundle failed when the file is a break zip
 */
HWTEST_F(BmsBundleParserTest, TestParse_0300, Function | SmallTest | Level0)
{
    BundleParser bundleParser;
    InnerBundleInfo innerBundleInfo;
    pathStream_ << RESOURCE_ROOT_PATH << BREAK_ZIP << INSTALL_FILE_SUFFIX;
    ErrCode result = bundleParser.Parse(pathStream_.str(), innerBundleInfo);
    EXPECT_EQ(result, ERR_APPEXECFWK_PARSE_NO_PROFILE);
}

/**
 * @tc.number: TestParse_0400
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. test parse bundle failed when the config.json is not exist in the zip
 */
HWTEST_F(BmsBundleParserTest, TestParse_0400, Function | SmallTest | Level0)
{
    BundleParser bundleParser;
    InnerBundleInfo innerBundleInfo;
    pathStream_ << RESOURCE_ROOT_PATH << NO_PROFILE << INSTALL_FILE_SUFFIX;
    ErrCode result = bundleParser.Parse(pathStream_.str(), innerBundleInfo);
    EXPECT_EQ(result, ERR_APPEXECFWK_PARSE_NO_PROFILE);
}

/**
 * @tc.number: TestParse_0500
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parse bundle failed when the config.json has format error
 */
HWTEST_F(BmsBundleParserTest, TestParse_0500, Function | SmallTest | Level0)
{
    BundleParser bundleParser;
    InnerBundleInfo innerBundleInfo;
    pathStream_ << RESOURCE_ROOT_PATH << FORMAT_ERROR_PROFILE << INSTALL_FILE_SUFFIX;
    ErrCode result = bundleParser.Parse(pathStream_.str(), innerBundleInfo);
    EXPECT_EQ(result, ERR_APPEXECFWK_PARSE_BAD_PROFILE);
}

/**
 * @tc.number: TestParse_0600
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parse bundle failed when prop(APP_notMustPropKeys) is not exist in the config.json
 */
HWTEST_F(BmsBundleParserTest, TestParse_0600, Function | SmallTest | Level0)
{
    std::vector<std::string> notMustPropKeys = {
        PROFILE_KEY_DESCRIPTION,
        PROFILE_KEY_LABEL,
        // sub BUNDLE_APP_PROFILE_KEY_API_VERSION
        BUNDLE_APP_PROFILE_KEY_VENDOR,
        BUNDLE_APP_PROFILE_KEY_TARGET,
        BUNDLE_APP_PROFILE_KEY_RELEASE_TYPE,
    };

    for (const auto &propKey : notMustPropKeys) {
        APP_LOGD("test not must prop %{public}s not exist", propKey.c_str());
        CheckNoPropProfileParseApp(propKey, ERR_OK);
    }
}

/**
 * @tc.number: TestParse_0700
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parse bundle failed when prop(deviceConfig_notMustPropKeys) is not exist in the config.json
 */
HWTEST_F(BmsBundleParserTest, TestParse_0700, Function | SmallTest | Level0)
{
    std::vector<std::string> notMustPropKeys = {
        PROFILE_KEY_DESCRIPTION,
        PROFILE_KEY_LABEL,
        // sub BUNDLE_PROFILE_KEY_DEVICE_CONFIG
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_PHONE,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_TABLET,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_TV,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_CAR,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_WEARABLE,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_LITE_WEARABLE,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SMART_VISION,
        // sub BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DEFAULT
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_PROCESS,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DIRECT_LAUNCH,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SUPPORT_BACKUP,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_COMPRESS_NATIVE_LIBS,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_NETWORK,
        // sub BUNDLE_DEVICE_CONFIG_PROFILE_KEY_NETWORK
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_USES_CLEAR_TEXT,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SECURITY_CONFIG,
        // sub BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SECURITY_CONFIG
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DOMAIN_SETTINGS,
    };

    for (const auto &propKey : notMustPropKeys) {
        APP_LOGD("test not must prop %{public}s not exist", propKey.c_str());
        CheckNoPropProfileParseDeviceConfig(propKey, ERR_OK);
    }
}

/**
 * @tc.number: TestParse_0800
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parse bundle failed when prop(module_notMustPropKeys) is not exist in the config.json
 */
HWTEST_F(BmsBundleParserTest, TestParse_0800, Function | SmallTest | Level0)
{
    std::vector<std::string> notMustPropKeys = {
        PROFILE_KEY_DESCRIPTION,
        PROFILE_KEY_LABEL,
        // sub BUNDLE_PROFILE_KEY_MODULE
        BUNDLE_MODULE_PROFILE_KEY_SUPPORTED_MODES,
        BUNDLE_MODULE_PROFILE_KEY_ABILITIES,
        BUNDLE_MODULE_PROFILE_KEY_JS,
        BUNDLE_MODULE_PROFILE_KEY_SHORTCUTS,
        BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS,
        BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS,
        // sub BUNDLE_MODULE_PROFILE_KEY_ABILITIES
        BUNDLE_MODULE_PROFILE_KEY_PROCESS,
        BUNDLE_MODULE_PROFILE_KEY_ICON,
        BUNDLE_MODULE_PROFILE_KEY_URI,
        BUNDLE_MODULE_PROFILE_KEY_LAUNCH_TYPE,
        BUNDLE_MODULE_PROFILE_KEY_VISIBLE,
        BUNDLE_MODULE_PROFILE_KEY_PERMISSIONS,
        BUNDLE_MODULE_PROFILE_KEY_SKILLS,
        BUNDLE_MODULE_PROFILE_KEY_DEVICE_CAP_ABILITY,
        BUNDLE_MODULE_PROFILE_KEY_ORIENTATION,
        BUNDLE_MODULE_PROFILE_KEY_BACKGROUND_MODES,
        BUNDLE_MODULE_PROFILE_KEY_READ_PERMISSION,
        BUNDLE_MODULE_PROFILE_KEY_WRITE_PERMISSION,
        BUNDLE_MODULE_PROFILE_KEY_DIRECT_LAUNCH,
        BUNDLE_MODULE_PROFILE_KEY_CONFIG_CHANGES,
        BUNDLE_MODULE_PROFILE_KEY_MISSION,
        BUNDLE_MODULE_PROFILE_KEY_TARGET_ABILITY,
        BUNDLE_MODULE_PROFILE_KEY_MULTIUSER_SHARED,
        BUNDLE_MODULE_PROFILE_KEY_SUPPORT_PIP_MODE,
        // sub BUNDLE_MODULE_PROFILE_KEY_JS
        BUNDLE_MODULE_PROFILE_KEY_WINDOW,
        // sub BUNDLE_MODULE_PROFILE_KEY_WINDOW
        BUNDLE_MODULE_PROFILE_KEY_DESIGN_WIDTH,
        BUNDLE_MODULE_PROFILE_KEY_AUTO_DESIGN_WIDTH,
        // sub BUNDLE_MODULE_PROFILE_KEY_SHORTCUTS
        BUNDLE_MODULE_PROFILE_KEY_SHORTCUT_INTENTS,
        // sub BUNDLE_MODULE_PROFILE_KEY_SHORTCUT_INTENTS
        BUNDLE_MODULE_PROFILE_KEY_TARGET_CLASS,
        BUNDLE_MODULE_PROFILE_KEY_TARGET_BUNDLE,
    };

    for (const auto &propKey : notMustPropKeys) {
        APP_LOGD("test not must prop %{public}s not exist", propKey.c_str());
        CheckNoPropProfileParseModule(propKey, ERR_OK);
    }
}

/**
 * @tc.number: TestParse_0900
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parse bundle failed when prop(configJson.app.bundleName) is not exist in the config.json
 */
HWTEST_F(BmsBundleParserTest, TestParse_0900, Function | SmallTest | Level0)
{
    std::vector<std::string> mustPropKeys = {
        BUNDLE_APP_PROFILE_KEY_BUNDLE_NAME,
    };

    for (const auto &propKey : mustPropKeys) {
        APP_LOGD("test must prop %{public}s not exist", propKey.c_str());
        CheckNoPropProfileParseApp(propKey, ERR_APPEXECFWK_PARSE_BAD_PROFILE);
    }
}

/**
 * @tc.number: TestParse_1000
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parse bundle failed when prop(configJson.module.package,deviceType) is not exist in the
 *              config.json
 */
HWTEST_F(BmsBundleParserTest, TestParse_1000, Function | SmallTest | Level0)
{
    std::vector<std::string> mustPropKeys = {
        BUNDLE_MODULE_PROFILE_KEY_PACKAGE,
        BUNDLE_MODULE_PROFILE_KEY_DEVICE_TYPE,
    };

    for (const auto &propKey : mustPropKeys) {
        APP_LOGD("test must prop %{public}s not exist", propKey.c_str());
        CheckNoPropProfileParseModule(propKey, ERR_APPEXECFWK_PARSE_BAD_PROFILE);
    }
}

/**
 * @tc.number: TestParse_1100
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parse bundle failed when prop(configJson.module.distro.moduleName) is not exist in the
 *           config.json
 */
HWTEST_F(BmsBundleParserTest, TestParse_1100, Function | SmallTest | Level0)
{
    std::vector<std::string> mustPropKeys = {
        BUNDLE_MODULE_PROFILE_KEY_DISTRO,
    };

    for (const auto &propKey : mustPropKeys) {
        APP_LOGD("test must prop %{public}s not exist", propKey.c_str());
        CheckNoPropProfileParseModule(propKey, ERR_APPEXECFWK_PARSE_BAD_PROFILE);
    }
}

/**
 * @tc.number: TestParse_1200
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parse bundle failed when prop(module.abilities.name) is not exist in the config.json
 */
HWTEST_F(BmsBundleParserTest, TestParse_1200, Function | SmallTest | Level0)
{
    std::vector<std::string> mustPropKeys = {
        PROFILE_KEY_NAME,
    };

    for (const auto &propKey : mustPropKeys) {
        APP_LOGD("test must prop %{public}s not exist", propKey.c_str());
        CheckNoPropProfileParseModule(propKey, ERR_APPEXECFWK_PARSE_BAD_PROFILE);
    }
}

/**
 * @tc.number: TestParse_1300
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parse bundle failed when the config.json is empty json
 */
HWTEST_F(BmsBundleParserTest, TestParse_1300, Function | SmallTest | Level0)
{
    BundleParser bundleParser;
    InnerBundleInfo innerBundleInfo;
    pathStream_ << RESOURCE_ROOT_PATH << EMPTY_CONFIG << INSTALL_FILE_SUFFIX;
    ErrCode result = bundleParser.Parse(pathStream_.str(), innerBundleInfo);
    EXPECT_EQ(result, ERR_APPEXECFWK_PARSE_UNEXPECTED);
}

/**
 * @tc.number: TestParse_1400
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parse bundle failed when the config.json has format MISSING
 */
HWTEST_F(BmsBundleParserTest, TestParse_1400, Function | SmallTest | Level0)
{
    BundleParser bundleParser;
    InnerBundleInfo innerBundleInfo;
    pathStream_ << RESOURCE_ROOT_PATH << FORMAT_MISSING_PROFILE << INSTALL_FILE_SUFFIX;
    ErrCode result = bundleParser.Parse(pathStream_.str(), innerBundleInfo);
    EXPECT_EQ(result, ERR_APPEXECFWK_PARSE_BAD_PROFILE);
}

/**
 * @tc.number: TestParse_1500
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parse bundle failed when the config.json is nothing json
 */
HWTEST_F(BmsBundleParserTest, TestParse_1500, Function | SmallTest | Level0)
{
    BundleParser bundleParser;
    InnerBundleInfo innerBundleInfo;
    pathStream_ << RESOURCE_ROOT_PATH << NOTHING_CONFIG << INSTALL_FILE_SUFFIX;
    ErrCode result = bundleParser.Parse(pathStream_.str(), innerBundleInfo);
    EXPECT_EQ(result, ERR_APPEXECFWK_PARSE_BAD_PROFILE);
}

/**
 * @tc.number: TestParse_1600
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parsing failed when an ability packet with an incorrect type in the file path
 */
HWTEST_F(BmsBundleParserTest, TestParse_1600, Function | SmallTest | Level0)
{
    BundleParser bundleParser;
    InnerBundleInfo innerBundleInfo;
    pathStream_ << RESOURCE_ROOT_PATH << "demo.error_type";
    ErrCode result = bundleParser.Parse(pathStream_.str(), innerBundleInfo);
    EXPECT_EQ(result, ERR_APPEXECFWK_PARSE_UNEXPECTED);

    pathStream_.str("");
    pathStream_ << RESOURCE_ROOT_PATH << "demo.";
    result = bundleParser.Parse(pathStream_.str(), innerBundleInfo);
    EXPECT_EQ(result, ERR_APPEXECFWK_PARSE_UNEXPECTED);

    pathStream_.str("");
    pathStream_ << RESOURCE_ROOT_PATH << "bundle_suffix_test.BUNDLE";
    result = bundleParser.Parse(pathStream_.str(), innerBundleInfo);
    EXPECT_EQ(result, ERR_APPEXECFWK_PARSE_UNEXPECTED);
}

/**
 * @tc.number: TestParse_1700
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parsing failed when an bundle packet with a deep file path depth
 */
HWTEST_F(BmsBundleParserTest, TestParse_1700, Function | SmallTest | Level1)
{
    BundleParser bundleParser;
    InnerBundleInfo innerBundleInfo;
    pathStream_ << RESOURCE_ROOT_PATH;
    int maxDeep = 100;
    for (int i = 0; i < maxDeep; i++) {
        pathStream_ << "test/";
    }
    pathStream_ << NEW_APP << INSTALL_FILE_SUFFIX;
    ErrCode result = bundleParser.Parse(pathStream_.str(), innerBundleInfo);
    EXPECT_EQ(result, ERR_APPEXECFWK_PARSE_UNEXPECTED) << pathStream_.str();
}

/**
 * @tc.number: TestParse_1800
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parsing failed when an bundle packet with a long path
 */
HWTEST_F(BmsBundleParserTest, TestParse_1800, Function | SmallTest | Level1)
{
    BundleParser bundleParser;
    InnerBundleInfo innerBundleInfo;
    pathStream_ << RESOURCE_ROOT_PATH;
    int maxLength = 256;
    for (int i = 0; i < maxLength; i++) {
        pathStream_ << "test/";
    }
    pathStream_ << NEW_APP << INSTALL_FILE_SUFFIX;
    ErrCode result = bundleParser.Parse(pathStream_.str(), innerBundleInfo);
    EXPECT_EQ(result, ERR_APPEXECFWK_PARSE_UNEXPECTED);
}

/**
 * @tc.number: TestParse_1900
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parsing failed when an bundle packet with special character in the file path
 */
HWTEST_F(BmsBundleParserTest, TestParse_1900, Function | SmallTest | Level1)
{
    BundleParser bundleParser;
    InnerBundleInfo innerBundleInfo;
    pathStream_ << RESOURCE_ROOT_PATH;
    std::string specialChars = "~!@#$%^&*(){}[]:;'?<>,.|`/./+_-";
    pathStream_ << specialChars << "new" << INSTALL_FILE_SUFFIX;

    ErrCode result = bundleParser.Parse(pathStream_.str(), innerBundleInfo);
    EXPECT_EQ(result, ERR_APPEXECFWK_PARSE_UNEXPECTED);
}

/**
 * @tc.number: TestParse_2000
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parsing failed when def-permission prop has error in the config.json
 */
HWTEST_F(BmsBundleParserTest, TestParse_2000, Function | SmallTest | Level1)
{
    nlohmann::json errorDefPermJson = CONFIG_JSON;
    errorDefPermJson[BUNDLE_PROFILE_KEY_MODULE][BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS] = R"(
        [{
            "name": "~!@#$%^&*(){}[]:;'?<>,.|`/./+_-",
            "reason": "~!@#$%^&*(){}[]:;'?<>,.|`/./+_-",
            "when": "~!@#$%^&*(){}[]:;'?<>,.|`/./+_-"
        }]
    )"_json;
    CheckProfilePermission(errorDefPermJson);
}

/**
 * @tc.number: TestParse_2100
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parsing failed when req-permission prop has error in the config.json
 */
HWTEST_F(BmsBundleParserTest, TestParse_2100, Function | SmallTest | Level1)
{
    nlohmann::json errorReqPermJson = CONFIG_JSON;
    errorReqPermJson[BUNDLE_PROFILE_KEY_MODULE][BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS] = R"(
        [{
            "name": "~!@#$%^&*(){}[]:;'?<>,.|`/./+_-",
            "reason": "~!@#$%^&*(){}[]:;'?<>,.|`/./+_-",
            "when": "~!@#$%^&*(){}[]:;'?<>,.|`/./+_-"
        }]
    )"_json;
    CheckProfilePermission(errorReqPermJson);
}

/**
 * @tc.number: TestParse_2200
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parsing failed when def-permission prop has empty in the config.json
 */
HWTEST_F(BmsBundleParserTest, TestParse_2200, Function | SmallTest | Level1)
{
    nlohmann::json errorDefPermJson = CONFIG_JSON;
    errorDefPermJson[BUNDLE_PROFILE_KEY_MODULE][BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS] = R"(
        [{

        }]
    )"_json;
    CheckProfilePermission(errorDefPermJson);
}

/**
 * @tc.number: TestParse_2300
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parsing failed when req-permission prop has empty in the config.json
 */
HWTEST_F(BmsBundleParserTest, TestParse_2300, Function | SmallTest | Level1)
{
    nlohmann::json errorReqPermJson = CONFIG_JSON;
    errorReqPermJson[BUNDLE_PROFILE_KEY_MODULE][BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS] = R"(
        [{

        }]
    )"_json;
    CheckProfilePermission(errorReqPermJson);
}

/**
 * @tc.number: TestExtractByName_0100
 * @tc.name: extract file stream by file name from package
 * @tc.desc: 1. system running normally
 *           2. test extract file from is not exist bundle or ability package
 */
HWTEST_F(BmsBundleParserTest, TestExtractByName_0100, Function | SmallTest | Level0)
{
    pathStream_ << RESOURCE_ROOT_PATH << UNKOWN_PATH << INSTALL_FILE_SUFFIX;
    std::string fileInBundle = "";
    std::ostringstream fileBuffer;

    BundleExtractor bundleExtractor(pathStream_.str());
    bool result = bundleExtractor.ExtractByName(fileInBundle, fileBuffer);
    ASSERT_FALSE(result);
}

/**
 * @tc.number: TestExtractByName_0200
 * @tc.name: extract file stream by file name from package
 * @tc.desc: 1. system running normally
 *           2. test extract is not exist file from bundle or ability package
 */
HWTEST_F(BmsBundleParserTest, TestExtractByName_0200, Function | SmallTest | Level0)
{
    pathStream_ << RESOURCE_ROOT_PATH << NEW_APP << INSTALL_FILE_SUFFIX;
    std::string fileInBundle = "unknown";
    std::ostringstream fileBuffer;

    BundleExtractor bundleExtractor(pathStream_.str());
    bool result = bundleExtractor.ExtractByName(fileInBundle, fileBuffer);
    ASSERT_FALSE(result);
}

/**
 * @tc.number: TestExtractByName_0300
 * @tc.name: extract file stream by file name from package
 * @tc.desc: 1. system running normally
 *           2. test failed to extract files from a package with a deep file path depth
 */
HWTEST_F(BmsBundleParserTest, TestExtractByName_0300, Function | SmallTest | Level1)
{
    pathStream_ << RESOURCE_ROOT_PATH;
    int maxDeep = 100;
    for (int i = 0; i < maxDeep; i++) {
        pathStream_ << "test/";
    }
    pathStream_ << "app" << INSTALL_FILE_SUFFIX;

    std::string fileInBundle = "config.json";
    std::ostringstream fileBuffer;

    BundleExtractor bundleExtractor(pathStream_.str());
    bool result = bundleExtractor.ExtractByName(fileInBundle, fileBuffer);
    ASSERT_FALSE(result);
}

/**
 * @tc.number: TestExtractByName_0400
 * @tc.name: extract file stream by file name from package
 * @tc.desc: 1. system running normally
 *           2. test failed to extract files from a file with a long path
 */
HWTEST_F(BmsBundleParserTest, TestExtractByName_0400, Function | SmallTest | Level1)
{
    pathStream_ << RESOURCE_ROOT_PATH;
    int maxLength = 256;
    for (int i = 0; i < maxLength; i++) {
        pathStream_ << "test";
    }
    pathStream_ << "new" << INSTALL_FILE_SUFFIX;

    std::string fileInBundle = "config.json";
    std::ostringstream fileBuffer;

    BundleExtractor bundleExtractor(pathStream_.str());
    bool result = bundleExtractor.ExtractByName(fileInBundle, fileBuffer);
    ASSERT_FALSE(result);
}

/**
 * @tc.number: TestExtractByName_0500
 * @tc.name: extract file stream by file name from package
 * @tc.desc: 1. system running normally
 *           2. test failed to extract files from a package with special character in the file path
 */
HWTEST_F(BmsBundleParserTest, TestExtractByName_0500, Function | SmallTest | Level1)
{
    pathStream_ << RESOURCE_ROOT_PATH;
    std::string specialChars = "~!@#$%^&*(){}[]:;'?<>,.|`/./+_-";
    pathStream_ << specialChars << "new" << INSTALL_FILE_SUFFIX;

    std::string fileInBundle = "config.json";
    std::ostringstream fileBuffer;

    BundleExtractor bundleExtractor(pathStream_.str());
    bool result = bundleExtractor.ExtractByName(fileInBundle, fileBuffer);
    ASSERT_FALSE(result);
}