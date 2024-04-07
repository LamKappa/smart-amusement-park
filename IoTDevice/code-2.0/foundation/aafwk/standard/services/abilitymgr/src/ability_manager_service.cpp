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

#include "ability_manager_service.h"

#include <functional>
#include <memory>
#include <string>
#include <unistd.h>
#include "string_ex.h"

#include "ability_info.h"
#include "ability_manager_errors.h"
#include "hilog_wrapper.h"
#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "sa_mgr_client.h"
#include "system_ability_definition.h"

using OHOS::AppExecFwk::ElementName;

namespace OHOS {
namespace AAFwk {
using namespace std::chrono;
constexpr auto DATA_ABILITY_START_TIMEOUT = 5s;
const std::map<std::string, AbilityManagerService::DumpKey> AbilityManagerService::dumpMap = {
    std::map<std::string, AbilityManagerService::DumpKey>::value_type("--all", KEY_DUMP_ALL),
    std::map<std::string, AbilityManagerService::DumpKey>::value_type("-a", KEY_DUMP_ALL),
    std::map<std::string, AbilityManagerService::DumpKey>::value_type("--stack-list", KEY_DUMP_STACK_LIST),
    std::map<std::string, AbilityManagerService::DumpKey>::value_type("-l", KEY_DUMP_STACK_LIST),
    std::map<std::string, AbilityManagerService::DumpKey>::value_type("--stack", KEY_DUMP_STACK),
    std::map<std::string, AbilityManagerService::DumpKey>::value_type("-s", KEY_DUMP_STACK),
    std::map<std::string, AbilityManagerService::DumpKey>::value_type("--mission", KEY_DUMP_MISSION),
    std::map<std::string, AbilityManagerService::DumpKey>::value_type("-m", KEY_DUMP_MISSION),
    std::map<std::string, AbilityManagerService::DumpKey>::value_type("--top", KEY_DUMP_TOP_ABILITY),
    std::map<std::string, AbilityManagerService::DumpKey>::value_type("-t", KEY_DUMP_TOP_ABILITY),
    std::map<std::string, AbilityManagerService::DumpKey>::value_type("--waitting-queue", KEY_DUMP_WAIT_QUEUE),
    std::map<std::string, AbilityManagerService::DumpKey>::value_type("-w", KEY_DUMP_WAIT_QUEUE),
    std::map<std::string, AbilityManagerService::DumpKey>::value_type("--serv", KEY_DUMP_SERVICE),
    std::map<std::string, AbilityManagerService::DumpKey>::value_type("-e", KEY_DUMP_SERVICE),
    std::map<std::string, AbilityManagerService::DumpKey>::value_type("--data", KEY_DUMP_DATA),
    std::map<std::string, AbilityManagerService::DumpKey>::value_type("-d", KEY_DUMP_DATA),
    std::map<std::string, AbilityManagerService::DumpKey>::value_type("--ui", KEY_DUMP_SYSTEM_UI),
    std::map<std::string, AbilityManagerService::DumpKey>::value_type("-u", KEY_DUMP_SYSTEM_UI),
};
const bool REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(DelayedSingleton<AbilityManagerService>::GetInstance().get());

AbilityManagerService::AbilityManagerService()
    : SystemAbility(ABILITY_MGR_SERVICE_ID, true),
      eventLoop_(nullptr),
      handler_(nullptr),
      state_(ServiceRunningState::STATE_NOT_START),
      connectManager_(std::make_shared<AbilityConnectManager>()),
      iBundleManager_(nullptr)
{
    std::shared_ptr<AppScheduler> appScheduler(
        DelayedSingleton<AppScheduler>::GetInstance().get(), [](AppScheduler *x) { x->DecStrongRef(x); });
    appScheduler_ = appScheduler;
    DumpFuncInit();
}

AbilityManagerService::~AbilityManagerService()
{}

void AbilityManagerService::OnStart()
{
    if (state_ == ServiceRunningState::STATE_RUNNING) {
        HILOG_INFO("Ability Manager Service has already started.");
        return;
    }
    HILOG_INFO("Ability Manager Service started.");
    if (!Init()) {
        HILOG_ERROR("failed to init service.");
        return;
    }
    state_ = ServiceRunningState::STATE_RUNNING;
    eventLoop_->Run();
    /* Publish service maybe failed, so we need call this function at the last,
     * so it can't affect the TDD test program */
    bool ret = Publish(DelayedSingleton<AbilityManagerService>::GetInstance().get());
    if (!ret) {
        HILOG_ERROR("AbilityManagerService::Init Publish failed!");
        return;
    }

    HILOG_INFO("Ability Manager Service start success.");
}

bool AbilityManagerService::Init()
{
    eventLoop_ = AppExecFwk::EventRunner::Create(AbilityConfig::NAME_ABILITY_MGR_SERVICE);
    if (eventLoop_.get() == nullptr) {
        HILOG_ERROR("failed to create EventRunner");
        return false;
    }

    handler_ = std::make_shared<AbilityEventHandler>(eventLoop_, weak_from_this());
    if (!handler_ || !connectManager_) {
        HILOG_ERROR("handler_ or connectManager_ is nullptr");
        return false;
    }
    connectManager_->SetEventHandler(handler_);

    auto dataAbilityManager = std::make_shared<DataAbilityManager>();
    if (!dataAbilityManager) {
        HILOG_ERROR("Failed to init data ability manager.");
        return false;
    }

    int userId = GetUserId();
    SetStackManager(userId);
    systemAppManager_ = std::make_shared<KernalSystemAppManager>(userId);
    if (!systemAppManager_) {
        HILOG_ERROR("Failed to init kernal system app manager.");
        return false;
    }

    auto startLauncherAbilityTask = [aams = shared_from_this()]() {
        aams->WaitForStartingLauncherAbility();
        aams->StartSystemUi(AbilityConfig::SYSTEM_UI_STATUS_BAR);
        aams->StartSystemUi(AbilityConfig::SYSTEM_UI_NAVIGATION_BAR);
    };
    handler_->PostTask(startLauncherAbilityTask, "startLauncherAbility");
    dataAbilityManager_ = dataAbilityManager;
    HILOG_INFO("init success");
    return true;
}

void AbilityManagerService::OnStop()
{
    HILOG_INFO("stop service");
    eventLoop_.reset();
    handler_.reset();
    state_ = ServiceRunningState::STATE_NOT_START;
}

ServiceRunningState AbilityManagerService::QueryServiceState() const
{
    return state_;
}

int AbilityManagerService::StartAbility(const Want &want, int requestCode)
{
    HILOG_INFO("%{public}s", __func__);
    return StartAbility(want, nullptr, requestCode);
}

int AbilityManagerService::StartAbility(const Want &want, const sptr<IRemoteObject> &callerToken, int requestCode)
{
    HILOG_INFO("%{public}s", __func__);

    if (callerToken != nullptr && !VerificationToken(callerToken)) {
        return ERR_INVALID_VALUE;
    }

    AbilityRequest abilityRequest;
    int result = GenerateAbilityRequest(want, requestCode, abilityRequest, callerToken);
    if (result != ERR_OK) {
        HILOG_ERROR("%{public}s generate ability request error.", __func__);
        return result;
    }
    auto abilityInfo = abilityRequest.abilityInfo;
    auto type = abilityInfo.type;
    auto curPageAbility = currentStackManager_->GetCurrentTopAbility();

    if (abilityInfo.applicationInfo.isLauncherApp && type == AppExecFwk::AbilityType::PAGE && curPageAbility &&
        curPageAbility->GetAbilityInfo().name == AbilityConfig::SYSTEM_DIALOG_NAME &&
        curPageAbility->GetAbilityInfo().bundleName == AbilityConfig::SYSTEM_UI_BUNDLE_NAME) {
        HILOG_ERROR("page ability is dialog type, cannot return to luncher");
        return ERR_INVALID_VALUE;
    }

    if (type == AppExecFwk::AbilityType::DATA) {
        HILOG_ERROR("Cannot start data ability, use 'AcquireDataAbility()' instead.");
        return ERR_INVALID_VALUE;
    }

    if (abilityInfo.name != AbilityConfig::SYSTEM_DIALOG_NAME &&
        abilityInfo.bundleName != AbilityConfig::SYSTEM_UI_BUNDLE_NAME) {
        result = PreLoadAppDataAbilities(abilityInfo.bundleName);
        if (result != ERR_OK) {
            HILOG_ERROR("StartAbility: App data ability preloading failed, '%{public}s', %{public}d",
                abilityInfo.bundleName.c_str(),
                result);
            return result;
        }
    }

    if (type == AppExecFwk::AbilityType::SERVICE) {
        return connectManager_->StartAbility(abilityRequest);
    }

    if (IsSystemUiApp(abilityRequest.abilityInfo)) {
        return systemAppManager_->StartAbility(abilityRequest);
    }

    return currentStackManager_->StartAbility(abilityRequest);
}

int AbilityManagerService::TerminateAbility(const sptr<IRemoteObject> &token, int resultCode, const Want *resultWant)
{
    HILOG_INFO("%{public}s. for result: %{public}d", __func__, (resultWant != nullptr));
    if (!VerificationToken(token)) {
        return ERR_INVALID_VALUE;
    }

    auto abilityRecord = Token::GetAbilityRecordByToken(token);
    if (!abilityRecord) {
        HILOG_ERROR("token is invalid");
        return ERR_INVALID_VALUE;
    }

    if (IsSystemUiApp(abilityRecord->GetAbilityInfo())) {
        HILOG_ERROR("system ui not allow terminate.");
        return ERR_INVALID_VALUE;
    }

    auto type = abilityRecord->GetAbilityInfo().type;
    if (type == AppExecFwk::AbilityType::SERVICE) {
        return connectManager_->TerminateAbility(token);
    }

    if (type == AppExecFwk::AbilityType::DATA) {
        HILOG_ERROR("Cannot terminate data ability, use 'ReleaseDataAbility()' instead.");
        return ERR_INVALID_VALUE;
    }

    return currentStackManager_->TerminateAbility(token, resultCode, resultWant);
}

int AbilityManagerService::TerminateAbilityByCaller(const sptr<IRemoteObject> &callerToken, int requestCode)
{
    HILOG_INFO("%{public}s %{public}d", __func__, __LINE__);
    if (!VerificationToken(callerToken)) {
        return ERR_INVALID_VALUE;
    }

    auto abilityRecord = Token::GetAbilityRecordByToken(callerToken);
    if (!abilityRecord) {
        HILOG_ERROR("%{public}s failed, caller ability record is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }

    if (IsSystemUiApp(abilityRecord->GetAbilityInfo())) {
        HILOG_ERROR("system ui not allow terminate.");
        return ERR_INVALID_VALUE;
    }

    auto type = abilityRecord->GetAbilityInfo().type;
    switch (type) {
        case AppExecFwk::AbilityType::SERVICE:
            return connectManager_->TerminateAbility(abilityRecord, requestCode);
            break;
        case AppExecFwk::AbilityType::PAGE:
            return currentStackManager_->TerminateAbility(abilityRecord, requestCode);
            break;
        default:
            return ERR_INVALID_VALUE;
            break;
    }
}

int AbilityManagerService::GetRecentMissions(
    const int32_t numMax, const int32_t flags, std::vector<RecentMissionInfo> &recentList)
{
    HILOG_INFO("%{public}s numMax %{public}d, flags: %{public}d", __func__, numMax, flags);
    if (numMax < 0 || flags < 0) {
        HILOG_ERROR("get recent missions, numMax or flags is invalid");
        return ERR_INVALID_VALUE;
    }

    return currentStackManager_->GetRecentMissions(numMax, flags, recentList);
}

int AbilityManagerService::GetMissionSnapshot(const int32_t missionId, MissionSnapshotInfo &snapshot)
{
    return 0;
}

int AbilityManagerService::MoveMissionToTop(int32_t missionId)
{
    HILOG_INFO("%{public}s mission id: %d", __func__, missionId);
    if (missionId < 0) {
        HILOG_ERROR("%{public}s, mission id is invalid", __func__);
        return ERR_INVALID_VALUE;
    }
    return currentStackManager_->MoveMissionToTop(missionId);
}

int AbilityManagerService::RemoveMission(int id)
{
    HILOG_DEBUG("remove mission called");
    if (id < 0) {
        HILOG_ERROR("remove mission, id is invalid");
        return ERR_INVALID_VALUE;
    }
    return currentStackManager_->RemoveMissionById(id);
}

int AbilityManagerService::RemoveStack(int id)
{
    HILOG_DEBUG("remove stack called");
    if (id < 0) {
        HILOG_ERROR("remove stack, id is invalid");
        return ERR_INVALID_VALUE;
    }
    return currentStackManager_->RemoveStack(id);
}

int AbilityManagerService::ConnectAbility(
    const Want &want, const sptr<IAbilityConnection> &connect, const sptr<IRemoteObject> &callerToken)
{
    HILOG_INFO("%{public}s", __func__);
    if (connect == nullptr || connect->AsObject() == nullptr) {
        HILOG_ERROR("%{public}s, connect is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    AbilityRequest abilityRequest;
    int result = GenerateAbilityRequest(want, -1, abilityRequest, callerToken);
    if (result != ERR_OK) {
        HILOG_ERROR("%{public}s generate ability request error.", __func__);
        return result;
    }
    auto abilityInfo = abilityRequest.abilityInfo;
    auto type = abilityInfo.type;
    if (type != AppExecFwk::AbilityType::SERVICE) {
        HILOG_ERROR("Connect Ability failed, target Ability is not Service.");
        return TARGET_ABILITY_NOT_SERVICE;
    }
    result = PreLoadAppDataAbilities(abilityInfo.bundleName);
    if (result != ERR_OK) {
        HILOG_ERROR("ConnectAbility: App data ability preloading failed, '%{public}s', %{public}d",
            abilityInfo.bundleName.c_str(),
            result);
        return result;
    }
    return connectManager_->ConnectAbilityLocked(abilityRequest, connect, callerToken);
}

int AbilityManagerService::DisconnectAbility(const sptr<IAbilityConnection> &connect)
{
    HILOG_DEBUG("%{public}s", __func__);
    if (connect == nullptr || connect->AsObject() == nullptr) {
        HILOG_ERROR("%s, connect is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }

    return connectManager_->DisconnectAbilityLocked(connect);
}

void AbilityManagerService::RemoveAllServiceRecord()
{
    connectManager_->RemoveAll();
}

std::shared_ptr<AbilityRecord> AbilityManagerService::GetServiceRecordByElementName(const std::string &element)
{
    return connectManager_->GetServiceRecordByElementName(element);
}

std::list<std::shared_ptr<ConnectionRecord>> AbilityManagerService::GetConnectRecordListByCallback(
    sptr<IAbilityConnection> callback)
{
    return connectManager_->GetConnectRecordListByCallback(callback);
}

sptr<IAbilityScheduler> AbilityManagerService::AcquireDataAbility(
    const Uri &uri, bool tryBind, const sptr<IRemoteObject> &callerToken)
{
    HILOG_INFO("%{public}s, called.", __func__);
    if (!VerificationToken(callerToken)) {
        return nullptr;
    }

    auto bms = GetBundleManager();
    if (bms == nullptr) {
        HILOG_ERROR("Failed to get bundle manager service");
        return nullptr;
    }

    auto localUri(uri);
    if (localUri.GetScheme() != AbilityConfig::SCHEME_DATA_ABILITY) {
        HILOG_ERROR("Acquire data ability with invalid uri scheme.");
        return nullptr;
    }
    std::vector<std::string> pathSegments;
    localUri.GetPathSegments(pathSegments);
    if (pathSegments.empty()) {
        HILOG_ERROR("Acquire data ability with invalid uri path.");
        return nullptr;
    }

    AbilityRequest abilityRequest;
    std::string dataAbilityUri = AbilityConfig::SCHEME_DATA_ABILITY + "://" + pathSegments[0];
    bool queryResult = iBundleManager_->QueryAbilityInfoByUri(dataAbilityUri, abilityRequest.abilityInfo);
    if (!queryResult || abilityRequest.abilityInfo.name.empty() || abilityRequest.abilityInfo.bundleName.empty()) {
        HILOG_ERROR("Invalid ability info for data ability acquiring.");
        return nullptr;
    }
    abilityRequest.appInfo = abilityRequest.abilityInfo.applicationInfo;
    if (abilityRequest.appInfo.name.empty() || abilityRequest.appInfo.bundleName.empty()) {
        HILOG_ERROR("Invalid app info for data ability acquiring.");
        return nullptr;
    }
    if (abilityRequest.abilityInfo.type != AppExecFwk::AbilityType::DATA) {
        HILOG_ERROR("BMS query result is not a data ability.");
        return nullptr;
    }
    HILOG_DEBUG("Query data ability info: %{public}s|%{public}s|%{public}s",
        abilityRequest.appInfo.name.c_str(),
        abilityRequest.appInfo.bundleName.c_str(),
        abilityRequest.abilityInfo.name.c_str());

    return dataAbilityManager_->Acquire(abilityRequest, tryBind, callerToken);
}

int AbilityManagerService::ReleaseDataAbility(
    sptr<IAbilityScheduler> dataAbilityScheduler, const sptr<IRemoteObject> &callerToken)
{
    HILOG_INFO("%{public}s, called.", __func__);
    if (!VerificationToken(callerToken)) {
        return ERR_INVALID_STATE;
    }

    return dataAbilityManager_->Release(dataAbilityScheduler, callerToken);
}

int AbilityManagerService::AttachAbilityThread(
    const sptr<IAbilityScheduler> &scheduler, const sptr<IRemoteObject> &token)
{
    HILOG_INFO("%{public}s, called.", __func__);
    if (scheduler == nullptr) {
        HILOG_ERROR("pram is null.");
        return ERR_INVALID_VALUE;
    }

    if (!VerificationToken(token)) {
        return ERR_INVALID_VALUE;
    }

    auto abilityRecord = Token::GetAbilityRecordByToken(token);
    if (abilityRecord == nullptr) {
        HILOG_ERROR("%s, ability record is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    auto abilityInfo = abilityRecord->GetAbilityInfo();
    auto type = abilityInfo.type;
    if (type == AppExecFwk::AbilityType::SERVICE) {
        return connectManager_->AttachAbilityThreadLocked(scheduler, token);
    }
    if (type == AppExecFwk::AbilityType::DATA) {
        return dataAbilityManager_->AttachAbilityThread(scheduler, token);
    }
    if (IsSystemUiApp(abilityInfo)) {
        return systemAppManager_->AttachAbilityThread(scheduler, token);
    }
    return currentStackManager_->AttachAbilityThread(scheduler, token);
}

void AbilityManagerService::DumpFuncInit()
{
    dumpFuncMap_[KEY_DUMP_ALL] = &AbilityManagerService::DumpInner;
    dumpFuncMap_[KEY_DUMP_STACK_LIST] = &AbilityManagerService::DumpStackListInner;
    dumpFuncMap_[KEY_DUMP_STACK] = &AbilityManagerService::DumpStackInner;
    dumpFuncMap_[KEY_DUMP_MISSION] = &AbilityManagerService::DumpMissionInner;
    dumpFuncMap_[KEY_DUMP_TOP_ABILITY] = &AbilityManagerService::DumpTopAbilityInner;
    dumpFuncMap_[KEY_DUMP_WAIT_QUEUE] = &AbilityManagerService::DumpWaittingAbilityQueueInner;
    dumpFuncMap_[KEY_DUMP_SERVICE] = &AbilityManagerService::DumpStateInner;
    dumpFuncMap_[KEY_DUMP_DATA] = &AbilityManagerService::DataDumpStateInner;
    dumpFuncMap_[KEY_DUMP_SYSTEM_UI] = &AbilityManagerService::SystemDumpStateInner;
}

void AbilityManagerService::DumpInner(const std::string &args, std::vector<std::string> &info)
{
    currentStackManager_->Dump(info);
}

void AbilityManagerService::DumpStackListInner(const std::string &args, std::vector<std::string> &info)
{
    currentStackManager_->DumpStackList(info);
}

void AbilityManagerService::DumpStackInner(const std::string &args, std::vector<std::string> &info)
{
    std::vector<std::string> argList;
    SplitStr(args, " ", argList);
    if (argList.empty()) {
        return;
    }
    if (argList.size() < MIN_DUMP_ARGUMENT_NUM) {
        info.push_back("error: invalid argument, please see 'ability dump -h'.");
        return;
    }
    int stackId = -1;
    (void)StrToInt(argList[1], stackId);
    currentStackManager_->DumpStack(stackId, info);
}

void AbilityManagerService::DumpMissionInner(const std::string &args, std::vector<std::string> &info)
{
    std::vector<std::string> argList;
    SplitStr(args, " ", argList);
    if (argList.empty()) {
        return;
    }
    if (argList.size() < MIN_DUMP_ARGUMENT_NUM) {
        info.push_back("error: invalid argument, please see 'ability dump -h'.");
        return;
    }
    int missionId = -1;
    (void)StrToInt(argList[1], missionId);
    currentStackManager_->DumpMission(missionId, info);
}

void AbilityManagerService::DumpTopAbilityInner(const std::string &args, std::vector<std::string> &info)
{
    currentStackManager_->DumpTopAbility(info);
}

void AbilityManagerService::DumpWaittingAbilityQueueInner(const std::string &args, std::vector<std::string> &info)
{
    std::string result;
    DumpWaittingAbilityQueue(result);
    info.push_back(result);
}

void AbilityManagerService::DumpStateInner(const std::string &args, std::vector<std::string> &info)
{
    std::vector<std::string> argList;
    SplitStr(args, " ", argList);
    if (argList.empty()) {
        return;
    }
    if (argList.size() == MIN_DUMP_ARGUMENT_NUM) {
        connectManager_->DumpState(info, argList[1]);
    } else if (argList.size() < MIN_DUMP_ARGUMENT_NUM) {
        connectManager_->DumpState(info);
    } else {
        info.emplace_back("error: invalid argument, please see 'ability dump -h'.");
    }
}

void AbilityManagerService::DataDumpStateInner(const std::string &args, std::vector<std::string> &info)
{
    std::vector<std::string> argList;
    SplitStr(args, " ", argList);
    if (argList.empty()) {
        return;
    }
    if (argList.size() == MIN_DUMP_ARGUMENT_NUM) {
        dataAbilityManager_->DumpState(info, argList[1]);
    } else if (argList.size() < MIN_DUMP_ARGUMENT_NUM) {
        dataAbilityManager_->DumpState(info);
    } else {
        info.emplace_back("error: invalid argument, please see 'ability dump -h'.");
    }
}

void AbilityManagerService::SystemDumpStateInner(const std::string &args, std::vector<std::string> &info)
{
    systemAppManager_->DumpState(info);
}

void AbilityManagerService::DumpState(const std::string &args, std::vector<std::string> &info)
{
    std::vector<std::string> argList;
    SplitStr(args, " ", argList);
    if (argList.empty()) {
        return;
    }
    auto it = dumpMap.find(argList[0]);
    if (it == dumpMap.end()) {
        return;
    }
    DumpKey key = it->second;
    auto itFunc = dumpFuncMap_.find(key);
    if (itFunc != dumpFuncMap_.end()) {
        auto dumpFunc = itFunc->second;
        if (dumpFunc != nullptr) {
            (this->*dumpFunc)(args, info);
            return;
        }
    }
    info.push_back("error: invalid argument, please see 'ability dump -h'.");
}

int AbilityManagerService::AbilityTransitionDone(const sptr<IRemoteObject> &token, int state)
{
    HILOG_INFO("%{public}s, state:%{public}d", __func__, state);
    if (!VerificationToken(token)) {
        return ERR_INVALID_VALUE;
    }

    auto abilityRecord = Token::GetAbilityRecordByToken(token);
    if (abilityRecord == nullptr) {
        HILOG_ERROR("%s, ability record is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    auto abilityInfo = abilityRecord->GetAbilityInfo();
    HILOG_DEBUG("%{public}s, state:%{public}d  name:%{public}s", __func__, state, abilityInfo.name.c_str());
    auto type = abilityInfo.type;
    if (type == AppExecFwk::AbilityType::SERVICE) {
        return connectManager_->AbilityTransitionDone(token, state);
    }
    if (type == AppExecFwk::AbilityType::DATA) {
        return dataAbilityManager_->AbilityTransitionDone(token, state);
    }
    if (IsSystemUiApp(abilityInfo)) {
        return systemAppManager_->AbilityTransitionDone(token, state);
    }

    return currentStackManager_->AbilityTransitionDone(token, state);
}

int AbilityManagerService::ScheduleConnectAbilityDone(
    const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &remoteObject)
{
    HILOG_INFO("%{public}s, called.", __func__);
    if (!VerificationToken(token)) {
        return ERR_INVALID_VALUE;
    }

    auto abilityRecord = Token::GetAbilityRecordByToken(token);
    if (!abilityRecord) {
        HILOG_ERROR("%s, ability record is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }

    auto type = abilityRecord->GetAbilityInfo().type;
    if (type != AppExecFwk::AbilityType::SERVICE) {
        HILOG_ERROR("Connect Ability failed, target Ability is not Service.");
        return TARGET_ABILITY_NOT_SERVICE;
    }

    return connectManager_->ScheduleConnectAbilityDoneLocked(token, remoteObject);
}

int AbilityManagerService::ScheduleDisconnectAbilityDone(const sptr<IRemoteObject> &token)
{
    HILOG_INFO("%{public}s, called.", __func__);
    if (!VerificationToken(token)) {
        return ERR_INVALID_VALUE;
    }

    auto abilityRecord = Token::GetAbilityRecordByToken(token);
    if (!abilityRecord) {
        HILOG_ERROR("%s, ability record is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }

    auto type = abilityRecord->GetAbilityInfo().type;
    if (type != AppExecFwk::AbilityType::SERVICE) {
        HILOG_ERROR("Connect Ability failed, target Ability is not Service.");
        return TARGET_ABILITY_NOT_SERVICE;
    }

    return connectManager_->ScheduleDisconnectAbilityDoneLocked(token);
}

int AbilityManagerService::ScheduleCommandAbilityDone(const sptr<IRemoteObject> &token)
{
    HILOG_INFO("%{public}s, called.", __func__);
    if (!VerificationToken(token)) {
        return ERR_INVALID_VALUE;
    }

    auto abilityRecord = Token::GetAbilityRecordByToken(token);
    if (!abilityRecord) {
        HILOG_ERROR("%s, ability record is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }

    auto type = abilityRecord->GetAbilityInfo().type;
    if (type != AppExecFwk::AbilityType::SERVICE) {
        HILOG_ERROR("Connect Ability failed, target Ability is not Service.");
        return TARGET_ABILITY_NOT_SERVICE;
    }

    return connectManager_->ScheduleCommandAbilityDoneLocked(token);
}

void AbilityManagerService::AddWindowInfo(const sptr<IRemoteObject> &token, int32_t windowToken)
{
    HILOG_DEBUG("add window id.");
    if (!VerificationToken(token)) {
        return;
    }
    currentStackManager_->AddWindowInfo(token, windowToken);
}

void AbilityManagerService::OnAbilityRequestDone(const sptr<IRemoteObject> &token, const int32_t state)
{
    HILOG_INFO("%{public}s, called.", __func__);
    if (!VerificationToken(token)) {
        return;
    }

    auto abilityRecord = Token::GetAbilityRecordByToken(token);
    if (abilityRecord == nullptr) {
        HILOG_ERROR("%s, ability record is nullptr", __func__);
        return;
    }

    auto type = abilityRecord->GetAbilityInfo().type;
    switch (type) {
        case AppExecFwk::AbilityType::SERVICE:
            connectManager_->OnAbilityRequestDone(token, state);
            break;
        case AppExecFwk::AbilityType::DATA:
            dataAbilityManager_->OnAbilityRequestDone(token, state);
            break;
        default: {
            if (IsSystemUiApp(abilityRecord->GetAbilityInfo())) {
                systemAppManager_->OnAbilityRequestDone(token, state);
                break;
            }
            currentStackManager_->OnAbilityRequestDone(token, state);
            break;
        }
    }
}

std::shared_ptr<AbilityEventHandler> AbilityManagerService::GetEventHandler()
{
    return handler_;
}

void AbilityManagerService::SetStackManager(int userId)
{
    auto iterator = stackManagers_.find(userId);
    if (iterator != stackManagers_.end()) {
        currentStackManager_ = iterator->second;
    } else {
        currentStackManager_ = std::make_shared<AbilityStackManager>(userId);
        currentStackManager_->Init();
        stackManagers_.emplace(userId, currentStackManager_);
    }
}

std::shared_ptr<AbilityStackManager> AbilityManagerService::GetStackManager()
{
    return currentStackManager_;
}

void AbilityManagerService::DumpWaittingAbilityQueue(std::string &result)
{
    currentStackManager_->DumpWaittingAbilityQueue(result);
    return;
}

// multi user scene
int AbilityManagerService::GetUserId()
{
    return 0;
}

void AbilityManagerService::WaitForStartingLauncherAbility()
{
    HILOG_INFO("Waiting AppMgr Service run completed.");
    while (!appScheduler_->Init(shared_from_this())) {
        HILOG_ERROR("failed to init appScheduler_");
        usleep(REPOLL_TIME_MICRO_SECONDS);
    }

    HILOG_INFO("Waiting BundleMgr Service run completed.");
    /* wait until connected to bundle manager service */
    while (iBundleManager_ == nullptr) {
        sptr<IRemoteObject> bundle_obj =
            OHOS::DelayedSingleton<SaMgrClient>::GetInstance()->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
        if (bundle_obj == nullptr) {
            HILOG_ERROR("failed to get bundle manager service");
            usleep(REPOLL_TIME_MICRO_SECONDS);
            continue;
        }
        iBundleManager_ = iface_cast<AppExecFwk::IBundleMgr>(bundle_obj);
    }

    HILOG_INFO("Waiting Home Launcher Ability install completed.");

    /* query if launcher ability has installed */
    AppExecFwk::AbilityInfo abilityInfo;
    /* First stage, hardcoding for the first launcher App */
    Want want;
    want.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
    while (!(iBundleManager_->QueryAbilityInfo(want, abilityInfo))) {
        HILOG_INFO("Waiting query launcher ability info completed.");
        usleep(REPOLL_TIME_MICRO_SECONDS);
    }

    AppExecFwk::AbilityInfo statusBarInfo;
    AppExecFwk::AbilityInfo navigationBarInfo;
    Want statusBarWant;
    Want navigationBarWant;
    statusBarWant.SetElementName(AbilityConfig::SYSTEM_UI_BUNDLE_NAME, AbilityConfig::SYSTEM_UI_STATUS_BAR);
    navigationBarWant.SetElementName(AbilityConfig::SYSTEM_UI_BUNDLE_NAME, AbilityConfig::SYSTEM_UI_NAVIGATION_BAR);
    uint32_t waitCnt = 0;
    // Wait 10 minutes for the installation to complete.
    while ((!(iBundleManager_->QueryAbilityInfo(statusBarWant, statusBarInfo)) ||
               !(iBundleManager_->QueryAbilityInfo(navigationBarWant, navigationBarInfo))) &&
           waitCnt < MAX_WAIT_SYSTEM_UI_NUM) {
        HILOG_INFO("Waiting query system ui info completed.");
        usleep(REPOLL_TIME_MICRO_SECONDS);
        waitCnt++;
    }
    HILOG_INFO("waiting boot animation for 4 seconds.");
    usleep(REPOLL_TIME_MICRO_SECONDS * 4);
    HILOG_INFO("start Home Launcher Ability.");
    /* start launch ability */
    (void)StartAbility(want, -1);
    return;
}

void AbilityManagerService::StartSystemUi(const std::string abilityName)
{
    HILOG_INFO("starting system ui app.");
    Want want;
    want.SetElementName(AbilityConfig::SYSTEM_UI_BUNDLE_NAME, abilityName);
    HILOG_INFO("starting system ui: %{public}s.", abilityName.c_str());
    (void)StartAbility(want, -1);
    return;
}

int AbilityManagerService::GenerateAbilityRequest(
    const Want &want, int requestCode, AbilityRequest &request, const sptr<IRemoteObject> &callerToken)
{
    request.want = want;
    request.requestCode = requestCode;
    request.callerToken = callerToken;

    auto bms = GetBundleManager();
    if (bms == nullptr) {
        HILOG_ERROR("failed to get bundle manager service");
        return GET_ABILITY_SERVICE_FAILED;
    }

    bms->QueryAbilityInfo(want, request.abilityInfo);
    if (request.abilityInfo.name.empty() || request.abilityInfo.bundleName.empty()) {
        HILOG_ERROR("failed to get ability info");
        return RESOLVE_ABILITY_ERR;
    }
    HILOG_DEBUG("query ability name:%s,", request.abilityInfo.name.c_str());

    request.appInfo = request.abilityInfo.applicationInfo;
    if (request.appInfo.name.empty() || request.appInfo.bundleName.empty()) {
        HILOG_ERROR("failed to get app info");
        return RESOLVE_APP_ERR;
    }
    HILOG_DEBUG("query app name:%s,", request.appInfo.name.c_str());
    return ERR_OK;
}

int AbilityManagerService::GetAllStackInfo(StackInfo &stackInfo)
{
    HILOG_DEBUG("get all stack info start");
    if (!currentStackManager_) {
        HILOG_ERROR("currentStackManager_ is nullptr");
        return ERR_NO_INIT;
    }
    currentStackManager_->GetAllStackInfo(stackInfo);
    HILOG_DEBUG("get all stack info end");

    return ERR_OK;
}

int AbilityManagerService::TerminateAbilityResult(const sptr<IRemoteObject> &token, int startId)
{
    HILOG_INFO("%{public}s, startId:%{public}d", __func__, startId);
    if (!VerificationToken(token)) {
        return ERR_INVALID_VALUE;
    }

    auto abilityRecord = Token::GetAbilityRecordByToken(token);
    if (!abilityRecord) {
        HILOG_ERROR("%{public}s, ability record is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    auto type = abilityRecord->GetAbilityInfo().type;
    if (type != AppExecFwk::AbilityType::SERVICE) {
        HILOG_ERROR("%{public}s failed, target Ability is not Service.", __func__);
        return TARGET_ABILITY_NOT_SERVICE;
    }

    return connectManager_->TerminateAbilityResult(token, startId);
}

int AbilityManagerService::StopServiceAbility(const Want &want)
{
    HILOG_DEBUG("%{public}s", __func__);
    AbilityRequest abilityRequest;
    int result = GenerateAbilityRequest(want, -1, abilityRequest, nullptr);
    if (result != ERR_OK) {
        HILOG_ERROR("%{public}s generate ability request error.", __func__);
        return result;
    }
    auto abilityInfo = abilityRequest.abilityInfo;
    auto type = abilityInfo.type;
    if (type != AppExecFwk::AbilityType::SERVICE) {
        HILOG_ERROR("%{public}s target ability is not service type", __func__);
        return TARGET_ABILITY_NOT_SERVICE;
    }
    return connectManager_->StopServiceAbility(abilityRequest);
}

void AbilityManagerService::OnAbilityDied(std::shared_ptr<AbilityRecord> abilityRecord)
{
    if (!abilityRecord) {
        HILOG_ERROR("ams on ability died: invalid ability record.");
        return;
    }

    if (systemAppManager_ && abilityRecord->IsKernalSystemAbility()) {
        systemAppManager_->OnAbilityDied(abilityRecord);
        return;
    }

    if (currentStackManager_) {
        currentStackManager_->OnAbilityDied(abilityRecord);
    }

    if (connectManager_) {
        connectManager_->OnAbilityDied(abilityRecord);
    }

    if (dataAbilityManager_) {
        dataAbilityManager_->OnAbilityDied(abilityRecord);
    }
}

int AbilityManagerService::KillProcess(const std::string &bundleName)
{
    HILOG_INFO("%{public}s, bundleName: %{public}s %{public}d", __func__, bundleName.c_str(), __LINE__);

    if (!currentStackManager_) {
        HILOG_ERROR("currentStackManager_ is nullptr");
        return ERR_NO_INIT;
    }

    return currentStackManager_->KillProcess(bundleName);
}

int AbilityManagerService::UninstallApp(const std::string &bundleName)
{
    HILOG_INFO("%{public}s, bundleName: %{public}s %{public}d", __func__, bundleName.c_str(), __LINE__);

    if (!currentStackManager_) {
        HILOG_ERROR("currentStackManager_ is nullptr");
        return ERR_NO_INIT;
    }

    return currentStackManager_->UninstallApp(bundleName);
}

sptr<AppExecFwk::IBundleMgr> AbilityManagerService::GetBundleManager()
{
    if (iBundleManager_ == nullptr) {
        auto bundleObj =
            OHOS::DelayedSingleton<SaMgrClient>::GetInstance()->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
        if (bundleObj == nullptr) {
            HILOG_ERROR("failed to get bundle manager service");
            return nullptr;
        }
        iBundleManager_ = iface_cast<AppExecFwk::IBundleMgr>(bundleObj);
    }

    return iBundleManager_;
}

int AbilityManagerService::PreLoadAppDataAbilities(const std::string &bundleName)
{
    if (bundleName.empty()) {
        HILOG_ERROR("Invalid bundle name when app data abilities preloading.");
        return ERR_INVALID_VALUE;
    }

    if (dataAbilityManager_ == nullptr) {
        HILOG_ERROR("Invalid data ability manager when app data abilities preloading.");
        return ERR_INVALID_STATE;
    }

    auto bms = GetBundleManager();
    if (bms == nullptr) {
        HILOG_ERROR("Failed to get bundle manager service when app data abilities preloading.");
        return GET_ABILITY_SERVICE_FAILED;
    }

    AppExecFwk::BundleInfo bundleInfo;
    bool ret = bms->GetBundleInfo(bundleName, AppExecFwk::BundleFlag::GET_BUNDLE_WITH_ABILITIES, bundleInfo);
    if (!ret) {
        HILOG_ERROR("Failed to get bundle info when app data abilities preloading.");
        return RESOLVE_APP_ERR;
    }

    HILOG_INFO("App data abilities preloading for bundle '%{public}s'...", bundleName.data());

    auto begin = system_clock::now();
    AbilityRequest dataAbilityRequest;
    dataAbilityRequest.appInfo = bundleInfo.applicationInfo;
    for (auto it = bundleInfo.abilityInfos.begin(); it != bundleInfo.abilityInfos.end(); ++it) {
        if (it->type != AppExecFwk::AbilityType::DATA) {
            continue;
        }
        if ((system_clock::now() - begin) >= DATA_ABILITY_START_TIMEOUT) {
            HILOG_ERROR("App data ability preloading for '%{public}s' timeout.", bundleName.c_str());
            return ERR_TIMED_OUT;
        }
        dataAbilityRequest.abilityInfo = *it;
        HILOG_INFO("App data ability preloading: '%{public}s.%{public}s'...", it->bundleName.c_str(), it->name.c_str());

        auto dataAbility = dataAbilityManager_->Acquire(dataAbilityRequest, false, nullptr);
        if (dataAbility == nullptr) {
            HILOG_ERROR(
                "Failed to preload data ability '%{public}s.%{public}s'.", it->bundleName.c_str(), it->name.c_str());
            return ERR_NULL_OBJECT;
        }
    }

    HILOG_INFO("App data abilities preloading done.");

    return ERR_OK;
}

bool AbilityManagerService::IsSystemUiApp(const AppExecFwk::AbilityInfo &info) const
{
    if (info.bundleName != AbilityConfig::SYSTEM_UI_BUNDLE_NAME) {
        return false;
    }
    return (info.name == AbilityConfig::SYSTEM_UI_NAVIGATION_BAR || info.name == AbilityConfig::SYSTEM_UI_STATUS_BAR);
}

void AbilityManagerService::HandleLoadTimeOut(int64_t eventId)
{
    HILOG_DEBUG("%{public}s", __func__);
    if (systemAppManager_) {
        systemAppManager_->OnTimeOut(AbilityManagerService::LOAD_TIMEOUT_MSG, eventId);
    }
    if (currentStackManager_) {
        currentStackManager_->OnTimeOut(AbilityManagerService::LOAD_TIMEOUT_MSG, eventId);
    }
}

void AbilityManagerService::HandleActiveTimeOut(int64_t eventId)
{
    HILOG_DEBUG("%{public}s", __func__);
    if (systemAppManager_) {
        systemAppManager_->OnTimeOut(AbilityManagerService::ACTIVE_TIMEOUT_MSG, eventId);
    }
    if (currentStackManager_) {
        currentStackManager_->OnTimeOut(AbilityManagerService::ACTIVE_TIMEOUT_MSG, eventId);
    }
}

void AbilityManagerService::HandleInactiveTimeOut(int64_t eventId)
{
    HILOG_DEBUG("%{public}s", __func__);
    if (currentStackManager_) {
        currentStackManager_->OnTimeOut(AbilityManagerService::INACTIVE_TIMEOUT_MSG, eventId);
    }
}

bool AbilityManagerService::VerificationToken(const sptr<IRemoteObject> &token)
{
    HILOG_INFO("%{public}s, called.", __func__);
    if (!currentStackManager_ || !dataAbilityManager_ || !connectManager_) {
        HILOG_ERROR("%{public}s, nullptr pointer", __func__);
        return false;
    }

    if (currentStackManager_->GetAbilityRecordByToken(token)) {
        return true;
    }

    if (currentStackManager_->GetAbilityFromTerminateList(token)) {
        return true;
    }

    if (dataAbilityManager_->GetAbilityRecordByToken(token)) {
        return true;
    }

    if (connectManager_->GetServiceRecordByToken(token)) {
        return true;
    }

    if (systemAppManager_->GetAbilityRecordByToken(token)) {
        return true;
    }

    HILOG_ERROR("%{public}s, Failed to verify token", __func__);
    return false;
}

}  // namespace AAFwk
}  // namespace OHOS
