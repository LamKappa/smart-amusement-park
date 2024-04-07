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

#ifndef KIT_TEST_COMMON_INFO_H
#define KIT_TEST_COMMON_INFO_H

namespace OHOS {
namespace AppExecFwk {
constexpr int pressureTimes = 3;
const std::string g_EVENT_REQU_FIRST = "requ_com_ohos_amsst_appkit_first";
const std::string g_EVENT_RESP_FIRST = "resp_com_ohos_amsst_appkit_first";
const std::string g_EVENT_RESP_FIRST_LIFECYCLE = "resp_com_ohos_amsst_appkit_first_lifecycle";
const std::string g_EVENT_RESP_FIRST_LIFECYCLE_CALLBACK = "resp_com_ohos_amsst_appkit_first_lifecycle_callbacks";
const std::string g_EVENT_RESP_FIRST_LIFECYCLE_OBSERVER = "resp_com_ohos_amsst_appkit_first_lifecycle_observer";
const std::string g_EVENT_RESP_FIRST_LIFECYCLE_OBSERVER2 = "resp_com_ohos_amsst_appkit_first_lifecycle_observer2";
const std::string g_EVENT_RESP_FIRST_ON_ABILITY_RESULT = "resp_com_ohos_amsst_appkit_first_onabilityresult";
const std::string g_EVENT_RESP_FIRST_ON_BACK_PRESSED = "resp_com_ohos_amsst_appkit_first_onbackpressed";
const std::string g_EVENT_RESP_FIRST_ON_NEW_WANT = "resp_com_ohos_amsst_appkit_first_onnewwant";
const std::string g_EVENT_REQU_FIRSTB = "requ_com_ohos_amsst_appkit_firstb";
const std::string g_EVENT_RESP_FIRSTB = "resp_com_ohos_amsst_appkit_firstb";
const std::string g_EVENT_RESP_FIRSTB_LIFECYCLE = "resp_com_ohos_amsst_appkit_firstb_lifecycle";
const std::string g_EVENT_REQU_SECOND = "requ_com_ohos_amsst_appkit_second";
const std::string g_EVENT_RESP_SECOND = "resp_com_ohos_amsst_appkit_second";
const std::string g_EVENT_RESP_SECOND_LIFECYCLE = "resp_com_ohos_amsst_appkit_second_lifecycle";
const std::string g_EVENT_RESP_SECOND_ABILITY_CONTEXT = "resp_com_ohos_amsst_appkit_secondability_ability_context";
const std::string g_EVENT_RESP_SECOND_ABILITY_API = "resp_com_ohos_amsst_appkit_secondability_ability_api";
// PageAbility: ThirdAbility Constants to use
const std::string g_respPageThirdAbilityST = "resp_com_ohos_amsst_appkit_third_ability";
const std::string g_requPageThirdAbilityST = "requ_com_ohos_amsst_appkit_third_ability";
const std::vector<const std::string> g_requPageThirdAbilitySTVector = {g_requPageThirdAbilityST};
// PageAbility: FourthAbility Constants to use
const std::string g_respPageFourthAbilityST = "resp_com_ohos_amsst_appkit_fourth_ability";
const std::string g_requPageFourthAbilityST = "requ_com_ohos_amsst_appkit_fourth_ability";
const std::vector<const std::string> g_requPageFourthAbilitySTVector = {g_requPageFourthAbilityST};
// PageAbility: SixthAbility Constants to use
const std::string g_respPageSixthAbilityST = "resp_com_ohos_amsst_appkit_sixth_ability";
const std::string g_respPageSixthAbilityLifecycleCallbacks = "resp_sixth_ability_lifecycle_callback";
const std::string g_requPageSixthAbilityST = "requ_com_ohos_amsst_appkit_sixth_ability";
const std::vector<const std::string> g_requPageSixthAbilitySTVector = {g_requPageSixthAbilityST};
// PageAbility: KitTestAbilityManager Constants to use
const std::string g_respPageManagerAbilityST = "resp_com_ohos_amsst_appkit_manager_ability";
const std::string g_requPageManagerAbilityST = "requ_com_ohos_amsst_appkit_manager_ability";
const std::vector<const std::string> g_requPageManagerAbilitySTVector = {g_requPageManagerAbilityST};
// PageAbility: KitTestAbilityManagerSecond Constants to use
const std::string g_respPageManagerSecondAbilityST = "resp_com_ohos_amsst_appkit_manager_second_ability";
const std::string g_requPageManagerSecondAbilityST = "requ_com_ohos_amsst_appkit_manager_second_ability";
const std::vector<const std::string> g_requPageManagerSecondAbilitySTVector = {g_requPageManagerSecondAbilityST};
const std::string g_onAbilityStart = ":OnAbilityStart";
const std::string g_onAbilityInactive = ":OnAbilityInactive";
const std::string g_onAbilityBackground = ":OnAbilityBackground";
const std::string g_onAbilityForeground = ":OnAbilityForeground";
const std::string g_onAbilityActive = ":OnAbilityActive";
const std::string g_onAbilityStop = ":OnAbilityStop";
const std::string g_onAbilitySaveState = ":OnAbilitySaveState";
const std::string g_memoryLevel = ":OnMemoryLevel";
const std::string g_configuration = ":OnConfigurationUpdated";
const std::string g_abilityStateOnStart = ":OnStart";
const std::string g_abilityStateOnStop = ":OnStop";
const std::string g_abilityStateOnActive = ":OnActive";
const std::string g_abilityStateOnInactive = ":OnInactive";
const std::string g_abilityStateOnBackground = ":OnBackground";
const std::string g_abilityStateOnForeground = ":OnForeground";
const std::string g_abilityStateOnNewWant = ":OnNewWant";
const std::string g_EVENT_REQU_FIFTH = "requ_com_ohos_amsst_appkit_fifth";
const std::string g_EVENT_RESP_FIFTH = "resp_com_ohos_amsst_appkit_fifth";

enum class WantApi {
    WantCopy,
    WantAssign,
    AddEntity,
    AddFlags,
    ClearWant,
    CountEntities,
    FormatMimeType,
    FormatType,
    FormatUri,
    FormatUriAndType,
    GetAction,
    GetBundle,
    GetEntities,
    GetElement,
    GetUri,
    GetUriString,
    GetFlags,
    GetScheme,
    GetType,
    HasEntity,
    MakeMainAbility,
    Marshalling,
    ParseUri,
    RemoveEntity,
    RemoveFlags,
    SetAction,
    SetBundle,
    SetElement,
    SetElementName_String_String,
    SetElementName_String_String_String,
    SetFlags,
    SetType,
    SetUri,
    SetUriAndType,
    ToUri,
    Unmarshalling,
    WantParseUri,
    WantToUri,
    GetParams,
    GetByteParam,
    GetByteArrayParam,
    GetBoolParam,
    GetBoolArrayParam,
    GetCharParam,
    GetCharArrayParam,
    GetIntParam,
    GetIntArrayParam,
    GetDoubleParam,
    GetDoubleArrayParam,
    GetFloatParam,
    GetFloatArrayParam,
    GetLongParam,
    GetLongArrayParam,
    GetShortParam,
    GetShortArrayParam,
    GetStringParam,
    GetStringArrayParam,
    SetParam_WantParams,
    SetParam_byte,
    SetParam_byte_array,
    SetParam_bool,
    SetParam_bool_array,
    SetParam_char,
    SetParam_char_array,
    SetParam_int,
    SetParam_int_array,
    SetParam_double,
    SetParam_double_array,
    SetParam_float,
    SetParam_float_array,
    SetParam_long,
    SetParam_long_array,
    SetParam_short,
    SetParam_short_array,
    SetParam_string,
    SetParam_string_array,
    HasParameter,
    ReplaceParams_WantParams,
    ReplaceParams_Want,
    RemoveParam,
    GetOperation,
    SetOperation,
    OperationEquals,
    CloneOperation,
    End
};

enum class SkillsApi {
    AddAction = (int)WantApi::End,
    CountActions,
    GetAction,
    HasAction,
    RemoveAction,
    AddEntity,
    CountEntities,
    GetEntity,
    HasEntity,
    RemoveEntity,
    AddAuthority,
    CountAuthorities,
    GetAuthority,
    HasAuthority,
    RemoveAuthority,
    AddScheme,
    CountSchemes,
    GetScheme,
    HasScheme,
    RemoveScheme,
    AddSchemeSpecificPart,
    CountSchemeSpecificParts,
    GetSchemeSpecificPart,
    HasSchemeSpecificPart,
    RemoveSchemeSpecificPart,
    AddPath_String,
    AddPath_String_MatchType,
    AddPath_PatternMatcher,
    AddPath_String_CountPaths,
    AddPath_String_MatchType_CountPaths,
    AddPath_PatternMatcher_CountPaths,
    CountPaths,
    AddPath_String_GetPath,
    AddPath_String_MatchType_GetPath,
    AddPath_PatternMatcher_GetPath,
    GetPath,
    AddPath_String_HasPath,
    AddPath_String_MatchType_HasPath,
    AddPath_PatternMatcher_HasPath,
    HasPath,
    RemovePath_String,
    RemovePath_String_MatchType,
    RemovePath_PatternMatcher,
    RemovePath_Other,
    AddType_String,
    AddType_String_MatchType,
    AddType_PatternMatcher,
    AddType_String_CountTypes,
    AddType_String_MatchType_CountTypes,
    AddType_PatternMatcher_CountTypes,
    CountTypes,
    AddType_String_GetType,
    AddType_String_MatchType_GetType,
    AddType_PatternMatcher_GetType,
    GetType,
    AddType_String_HasType,
    AddType_String_MatchType_HasType,
    AddType_PatternMatcher_HasType,
    HasType,
    RemoveType_String,
    RemoveType_String_MatchType,
    RemoveType_PatternMatcher,
    RemoveType_Other,
    GetEntities,
    GetWantParams,
    Match,
    Unmarshalling,
    Marshalling,
    Skills,
    Skills_Skills,
    End
};

enum class AbilityApi {
    GetAbilityName = (int)SkillsApi::End,
    GetAbilityPackage,
    GetLifecycle,
    GetWant,
    GetWindow,
    Dump,
    OnStart,
    OnActive,
    OnBackground,
    OnInactive,
    OnStop,
    OnForeground,
    OnAbilityResult,
    OnBackPressed,
    OnNewWant,
    Reload,
    SetResult,
    SetWant,
    StartAbilityForResult_Want_int,
    StartAbility_Want,
    StartAbility_Want_AbilityStartSetting,
    TerminateAbility,
    BatchInsert,
    GetType,
    OnKeyDown,
    OnKeyUp,
    OnTouchEvent,
    End
};

enum class AbilityContextApi {
    GetApplicationInfo = (int)AbilityApi::End,
    GetCacheDir,
    GetCodeCacheDir,
    GetDatabaseDir,
    GetDataDir,
    GetDir,
    GetNoBackupFilesDir,
    GetBundleManager,
    VerifyCallingPermission,
    VerifyPermission,
    VerifySelfPermission,
    GetBundleCodePath,
    GetBundleName,
    GetBundleResourcePath,
    CanRequestPermission,
    GetApplicationContext,
    GetCallingAbility,
    GetContext,
    GetAbilityManager,
    GetProcessInfo,
    GetAppType,
    GetCallingBundle,
    StartAbility_Want_int,
    TerminateAbility,
    GetElementName,
    GetHapModuleInfo,
    End
};

enum class LifeCycleApi {
    GetLifecycleState = (int)AbilityContextApi::End,
    AddObserver,
    DispatchLifecycle_Event_Want,
    DispatchLifecycle_Event,
    RemoveObserver,
    End
};

enum class AbilityLifecycleCallbacksApi {
    OnAbilityStart = (int)LifeCycleApi::End,
    OnAbilityInactive,
    OnAbilityBackground,
    OnAbilityForeground,
    OnAbilityActive,
    OnAbilityStop,
    OnAbilitySaveState,
    End
};

enum class AbilityManagerApi {
    ClearUpApplicationData = (int)AbilityLifecycleCallbacksApi::End,
    GetAllRunningProcesses,
    GetAllStackInfo,
    QueryRecentAbilityMissionInfo,
    QueryRunningAbilityMissionInfo,
    MoveMissionToTop,
    End
};

enum class ContextApi {
    CanRequestPermission = (int)AbilityManagerApi::End,
    DeleteFile,
    GetAbilityInfo,
    GetApplicationInfo,
    GetApplicationContext,
    GetBundleCodePath,
    GetBundleManager,
    GetBundleName,
    GetBundleResourcePath,
    GetCallingBundle,
    GetCacheDir,
    GetCodeCacheDir,
    GetDatabaseDir,
    GetDataDir,
    GetDir,
    GetFilesDir,
    GetHapModuleInfo,
    GetNoBackupFilesDir,
    GetProcessName,
    GetResourceManager,
    VerifyCallingPermission,
    VerifySelfPermission,
    StartAbility_Want_int,
    TerminateAbility_int,
    CreateBundleContext,
    VerifyCallingOrSelfPermission,
    VerifyPermission,
    End
};

enum class ElementNameApi {
    SetElementDeviceID = (int)ContextApi::End,
    SetElementBundleName,
    SetElementAbilityName,
    ClearElement,
    End
};

enum class OHOSApplicationApi {
    RegisterAbilityLifecycleCallbacks = (int)ElementNameApi::End,
    UnregisterAbilityLifecycleCallbacks,
    DispatchAbilitySavedState,
    OnConfigurationUpdated,
    OnMemoryLevel,
    OnStart,
    OnTerminate,
    RegisterElementsCallbacks,
    UnregisterElementsCallbacks,
    End
};

enum class LifecycleObserverApi {
    OnActive = (int)OHOSApplicationApi::End,
    OnBackground,
    OnForeground,
    OnInactive,
    OnStart,
    OnStop,
    OnStateChanged_Event_Want,
    OnStateChanged_Event,
    End
};

enum class ProcessInfoApi {
    GetPid = (int)LifecycleObserverApi::End,
    GetProcessName,
    Marshalling,
    Unmarshalling,
    ProcessInfo,
    ProcessInfo_String_int,
    End
};

enum class RunningProcessInfoApi {
    GetProcessName = (int)ProcessInfoApi::End,
    GetPid,
    GetUid,
    End,
};

enum class WantParamsApi {
    HasParam = (int)RunningProcessInfoApi::End,
    IsEmpty,
    Marshalling,
    Unmarshalling,
    Size,
    WantParamsCopy,
    GetParam,
    KeySet,
    Remove,
    SetParam,
    End,
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // KIT_TEST_COMMON_INFO_H