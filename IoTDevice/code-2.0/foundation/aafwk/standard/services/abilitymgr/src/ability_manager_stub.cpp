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

#include "ability_manager_stub.h"

#include "errors.h"
#include "string_ex.h"

#include "ability_connect_callback_proxy.h"
#include "ability_connect_callback_stub.h"
#include "ability_manager_errors.h"
#include "ability_manager_proxy.h"
#include "ability_scheduler_proxy.h"
#include "ability_scheduler_stub.h"

namespace OHOS {
namespace AAFwk {

bool AbilityManagerProxy::WriteInterfaceToken(MessageParcel &data)
{
    if (!data.WriteInterfaceToken(AbilityManagerProxy::GetDescriptor())) {
        HILOG_ERROR("write interface token failed");
        return false;
    }
    return true;
}

int AbilityManagerProxy::StartAbility(const Want &want, int requestCode)
{
    int error;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return INNER_ERR;
    }
    if (!data.WriteParcelable(&want)) {
        HILOG_ERROR("%{public}s fail, want write parcelable error", __func__);
        return INNER_ERR;
    }
    if (!data.WriteInt32(requestCode)) {
        HILOG_ERROR("%{public}s fail, requestCode write int32 error", __func__);
        return INNER_ERR;
    }

    error = Remote()->SendRequest(IAbilityManager::START_ABILITY, data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("start ability fail, error: %d", error);
        return error;
    }
    return reply.ReadInt32();
}

int AbilityManagerProxy::StartAbility(const Want &want, const sptr<IRemoteObject> &callerToken, int requestCode)
{
    int error;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return INNER_ERR;
    }
    if (!data.WriteParcelable(&want)) {
        HILOG_ERROR("%{public}s fail, want write parcelable error", __func__);
        return INNER_ERR;
    }
    if (!data.WriteParcelable(callerToken)) {
        HILOG_ERROR("%{public}s fail, callerToken write parcelable error", __func__);
        return INNER_ERR;
    }
    if (!data.WriteInt32(requestCode)) {
        HILOG_ERROR("%{public}s fail, requestCode write int32 error", __func__);
        return INNER_ERR;
    }

    error = Remote()->SendRequest(IAbilityManager::START_ABILITY_ADD_CALLER, data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("start ability fail, error: %d", error);
        return error;
    }
    return reply.ReadInt32();
}

int AbilityManagerProxy::TerminateAbility(const sptr<IRemoteObject> &token, int resultCode, const Want *resultWant)
{
    int error;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return INNER_ERR;
    }
    if (!data.WriteParcelable(token) || !data.WriteInt32(resultCode) || !data.WriteParcelable(resultWant)) {
        HILOG_ERROR("terminate ability. fail to write to parcel.");
        return INNER_ERR;
    }
    error = Remote()->SendRequest(IAbilityManager::TERMINATE_ABILITY, data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("terminate ability fail, error: %d", error);
        return error;
    }
    return reply.ReadInt32();
}

int AbilityManagerProxy::TerminateAbilityByCaller(const sptr<IRemoteObject> &callerToken, int requestCode)
{
    int error;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return INNER_ERR;
    }
    if (!data.WriteParcelable(callerToken) || !data.WriteInt32(requestCode)) {
        HILOG_ERROR("%{public}s. fail to write to parcel.", __func__);
        return INNER_ERR;
    }
    error = Remote()->SendRequest(IAbilityManager::TERMINATE_ABILITY_BY_CALLER, data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("terminate ability fail, error: %d", error);
        return error;
    }
    return reply.ReadInt32();
}

int AbilityManagerProxy::ConnectAbility(
    const Want &want, const sptr<IAbilityConnection> &connect, const sptr<IRemoteObject> &callerToken)
{
    int error;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return INNER_ERR;
    }
    if (!data.WriteParcelable(&want)) {
        HILOG_ERROR("connect ability fail, want error");
        return ERR_INVALID_VALUE;
    }
    if (connect == nullptr) {
        HILOG_ERROR("connect ability fail, connect is nullptr");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteParcelable(connect->AsObject())) {
        HILOG_ERROR("connect ability fail, connect error");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteParcelable(callerToken)) {
        HILOG_ERROR("connect ability fail, callerToken error");
        return ERR_INVALID_VALUE;
    }

    error = Remote()->SendRequest(IAbilityManager::CONNECT_ABILITY, data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("connect ability fail, error: %d", error);
        return error;
    }
    return reply.ReadInt32();
}

int AbilityManagerProxy::DisconnectAbility(const sptr<IAbilityConnection> &connect)
{
    int error;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (connect == nullptr) {
        HILOG_ERROR("disconnect ability fail, connect is nullptr");
        return ERR_INVALID_VALUE;
    }
    if (!WriteInterfaceToken(data)) {
        return INNER_ERR;
    }
    if (!data.WriteParcelable(connect->AsObject())) {
        HILOG_ERROR("disconnect ability fail, connect error");
        return ERR_INVALID_VALUE;
    }

    error = Remote()->SendRequest(IAbilityManager::DISCONNECT_ABILITY, data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("disconnect ability fail, error: %d", error);
        return error;
    }
    return reply.ReadInt32();
}

sptr<IAbilityScheduler> AbilityManagerProxy::AcquireDataAbility(
    const Uri &uri, bool tryBind, const sptr<IRemoteObject> &callerToken)
{
    int error;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!callerToken) {
        HILOG_ERROR("invalid parameters for acquire data ability.");
        return nullptr;
    }
    if (!WriteInterfaceToken(data)) {
        return nullptr;
    }
    if (!data.WriteString(uri.ToString()) || !data.WriteBool(tryBind) || !data.WriteParcelable(callerToken)) {
        HILOG_ERROR("failed to mashalling the acquires data ability.");
        return nullptr;
    }
    error = Remote()->SendRequest(IAbilityManager::ACQUIRE_DATA_ABILITY, data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("failed to send acquire data ability request, error: %{public}d", error);
        return nullptr;
    }

    return iface_cast<IAbilityScheduler>(reply.ReadParcelable<IRemoteObject>());
}

int AbilityManagerProxy::ReleaseDataAbility(
    sptr<IAbilityScheduler> dataAbilityScheduler, const sptr<IRemoteObject> &callerToken)
{
    int error;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!dataAbilityScheduler || !callerToken) {
        return ERR_INVALID_VALUE;
    }
    if (!WriteInterfaceToken(data)) {
        return INNER_ERR;
    }
    if (!data.WriteParcelable(dataAbilityScheduler->AsObject()) || !data.WriteParcelable(callerToken)) {
        HILOG_ERROR("failed to mashalling the release data ability.");
        return INNER_ERR;
    }
    error = Remote()->SendRequest(IAbilityManager::RELEASE_DATA_ABILITY, data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("failed to send release data ability request, error: %d", error);
        return error;
    }
    return reply.ReadInt32();
}

int AbilityManagerProxy::AttachAbilityThread(const sptr<IAbilityScheduler> &scheduler, const sptr<IRemoteObject> &token)
{
    int error;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (scheduler == nullptr) {
        return ERR_INVALID_VALUE;
    }
    if (!WriteInterfaceToken(data)) {
        return INNER_ERR;
    }
    if (!data.WriteParcelable(scheduler->AsObject()) || !data.WriteParcelable(token)) {
        HILOG_ERROR("attach ability. fail to write to parcel.");
        return INNER_ERR;
    }
    error = Remote()->SendRequest(IAbilityManager::ATTACH_ABILITY_THREAD, data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("attach ability fail, error: %d", error);
        return error;
    }
    return reply.ReadInt32();
}

int AbilityManagerProxy::AbilityTransitionDone(const sptr<IRemoteObject> &token, int state)
{
    int error;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return INNER_ERR;
    }
    if (!data.WriteParcelable(token) || !data.WriteInt32(state)) {
        HILOG_ERROR("ability transaction done. fail to write to parcel.");
        return INNER_ERR;
    }
    error = Remote()->SendRequest(IAbilityManager::ABILITY_TRANSITION_DONE, data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("ability transaction done fail, error: %d", error);
        return error;
    }
    return reply.ReadInt32();
}

int AbilityManagerProxy::ScheduleConnectAbilityDone(
    const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &remoteObject)
{
    int error;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return INNER_ERR;
    }
    if (!data.WriteParcelable(token)) {
        HILOG_ERROR("schedule connect done fail, token error");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteParcelable(remoteObject)) {
        HILOG_ERROR("schedule connect done fail, remoteObject error");
        return ERR_INVALID_VALUE;
    }

    error = Remote()->SendRequest(IAbilityManager::CONNECT_ABILITY_DONE, data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("schedule connect done fail, error: %d", error);
        return error;
    }
    return reply.ReadInt32();
}

int AbilityManagerProxy::ScheduleDisconnectAbilityDone(const sptr<IRemoteObject> &token)
{
    int error;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return INNER_ERR;
    }
    if (!data.WriteParcelable(token)) {
        HILOG_ERROR("schedule disconnect done fail, token error");
        return ERR_INVALID_VALUE;
    }

    error = Remote()->SendRequest(IAbilityManager::DISCONNECT_ABILITY_DONE, data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("schedule disconnect done fail, error: %d", error);
        return error;
    }
    return reply.ReadInt32();
}

int AbilityManagerProxy::ScheduleCommandAbilityDone(const sptr<IRemoteObject> &token)
{
    int error;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return INNER_ERR;
    }
    if (!data.WriteParcelable(token)) {
        HILOG_ERROR("schedule command done fail, token error");
        return ERR_INVALID_VALUE;
    }

    error = Remote()->SendRequest(IAbilityManager::COMMAND_ABILITY_DONE, data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("schedule command done fail, error: %d", error);
        return error;
    }
    return reply.ReadInt32();
}

void AbilityManagerProxy::AddWindowInfo(const sptr<IRemoteObject> &token, int32_t windowToken)
{
    int error;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return;
    }
    if (!data.WriteParcelable(token) || !data.WriteInt32(windowToken)) {
        HILOG_ERROR("add window info write to parce fail.");
        return;
    }
    error = Remote()->SendRequest(ADD_WINDOW_INFO, data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("add window info fail, error: %d", error);
    }
}

void AbilityManagerProxy::DumpState(const std::string &args, std::vector<std::string> &state)
{
    int error;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return;
    }
    data.WriteString16(Str8ToStr16(args));
    error = Remote()->SendRequest(IAbilityManager::DUMP_STATE, data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("AbilityManagerProxy: SendRequest err %d", error);
        return;
    }
    int32_t stackNum = reply.ReadInt32();
    for (int i = 0; i < stackNum; i++) {
        std::string stac = Str16ToStr8(reply.ReadString16());
        state.emplace_back(stac);
    }
}

int AbilityManagerProxy::TerminateAbilityResult(const sptr<IRemoteObject> &token, int startId)
{
    int error;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return INNER_ERR;
    }
    if (!data.WriteParcelable(token) || !data.WriteInt32(startId)) {
        HILOG_ERROR("terminate ability for result fail");
        return ERR_INVALID_VALUE;
    }
    error = Remote()->SendRequest(IAbilityManager::TERMINATE_ABILITY_RESULT, data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("terminate ability for result fail, error: %d", error);
        return error;
    }
    return reply.ReadInt32();
}

int AbilityManagerProxy::StopServiceAbility(const Want &want)
{
    int error;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return INNER_ERR;
    }
    if (!data.WriteParcelable(&want)) {
        HILOG_ERROR("stop service ability. fail to write to parcel.");
        return INNER_ERR;
    }
    error = Remote()->SendRequest(IAbilityManager::STOP_SERVICE_ABILITY, data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("stop service ability, error: %d", error);
        return error;
    }
    return reply.ReadInt32();
}

int AbilityManagerProxy::GetAllStackInfo(StackInfo &stackInfo)
{
    int error;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!WriteInterfaceToken(data)) {
        return INNER_ERR;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        HILOG_ERROR("GetAllStackInfo: remote is nullptr");
        return ERR_UNKNOWN_OBJECT;
    }
    error = remote->SendRequest(IAbilityManager::LIST_STACK_INFO, data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("GetAllStackInfo: SendRequest err %{public}d", error);
        return error;
    }
    int32_t result = reply.ReadInt32();
    if (result != ERR_OK) {
        HILOG_ERROR("GetAllStackInfo: ReadInt32 err %{public}d", result);
        return result;
    }
    std::unique_ptr<StackInfo> info(reply.ReadParcelable<StackInfo>());
    if (!info) {
        HILOG_ERROR("readParcelableInfo<StackInfo> failed");
        return ERR_UNKNOWN_OBJECT;
    }
    stackInfo = *info;
    return result;
}

template <typename T>
int AbilityManagerProxy::GetParcelableInfos(MessageParcel &reply, std::vector<T> &parcelableInfos)
{
    int32_t infoSize = reply.ReadInt32();
    for (int32_t i = 0; i < infoSize; i++) {
        std::unique_ptr<T> info(reply.ReadParcelable<T>());
        if (!info) {
            HILOG_ERROR("Read Parcelable infos failed");
            return ERR_INVALID_VALUE;
        }
        parcelableInfos.emplace_back(*info);
    }
    HILOG_INFO("get parcelable infos success");
    return NO_ERROR;
}

int AbilityManagerProxy::GetRecentMissions(
    const int32_t numMax, const int32_t flags, std::vector<RecentMissionInfo> &recentList)
{
    int error;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        return INNER_ERR;
    }
    if (!data.WriteInt32(numMax)) {
        HILOG_ERROR("get recent missions by numMax , WriteInt32 fail.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteInt32(flags)) {
        HILOG_ERROR("get recent missions by flags , WriteInt32 fail.");
        return ERR_INVALID_VALUE;
    }
    error = Remote()->SendRequest(IAbilityManager::GET_RECENT_MISSION, data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("get recent mission fail, error: %{public}d", error);
        return error;
    }
    error = GetParcelableInfos<RecentMissionInfo>(reply, recentList);
    if (error != NO_ERROR) {
        HILOG_ERROR("GetParcelableInfos fail, error: %{public}d", error);
        return error;
    }
    return reply.ReadInt32();
}

int AbilityManagerProxy::GetMissionSnapshot(const int32_t missionId, MissionSnapshotInfo &snapshot)
{
    int error;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return INNER_ERR;
    }
    if (!data.WriteInt32(missionId)) {
        HILOG_ERROR("get recent missions by missionId , WriteInt32 fail.");
        return ERR_INVALID_VALUE;
    }
    error = Remote()->SendRequest(IAbilityManager::GET_MISSION_SNAPSHOT, data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("get mission snapshot fail, error: %{public}d", error);
        return error;
    }
    std::unique_ptr<MissionSnapshotInfo> info(reply.ReadParcelable<MissionSnapshotInfo>());
    if (!info) {
        HILOG_ERROR("readParcelableInfo<MissionSnapshotInfo> failed");
        return ERR_UNKNOWN_OBJECT;
    }
    snapshot = *info;
    return reply.ReadInt32();
}

int AbilityManagerProxy::MoveMissionToTop(int32_t missionId)
{
    int error;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return INNER_ERR;
    }
    if (!data.WriteInt32(missionId)) {
        HILOG_ERROR("move mission to top by missionId , WriteInt32 fail.");
        return ERR_INVALID_VALUE;
    }
    error = Remote()->SendRequest(IAbilityManager::MOVE_MISSION_TO_TOP, data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("move mission to top fail, error: %{public}d", error);
        return error;
    }
    return reply.ReadInt32();
}

int AbilityManagerProxy::RemoveMission(int id)
{
    int error;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return INNER_ERR;
    }
    if (!data.WriteInt32(id)) {
        HILOG_ERROR("remove mission by id , WriteInt32 fail.");
        return ERR_INVALID_VALUE;
    }
    error = Remote()->SendRequest(IAbilityManager::REMOVE_MISSION, data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("remove mission by id , error: %d", error);
        return error;
    }
    return reply.ReadInt32();
}

int AbilityManagerProxy::RemoveStack(int id)
{
    int error;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return INNER_ERR;
    }
    if (!data.WriteInt32(id)) {
        HILOG_ERROR("remove stack by id , WriteInt32 fail.");
        return ERR_INVALID_VALUE;
    }
    error = Remote()->SendRequest(IAbilityManager::REMOVE_STACK, data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("remove stack by id , error: %d", error);
        return error;
    }
    return reply.ReadInt32();
}

int AbilityManagerProxy::KillProcess(const std::string &bundleName)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return INNER_ERR;
    }
    if (!data.WriteString16(Str8ToStr16(bundleName))) {
        HILOG_ERROR("%{public}s , WriteString16 fail.", __func__);
        return ERR_INVALID_VALUE;
    }
    int error = Remote()->SendRequest(IAbilityManager::KILL_PROCESS, data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("%s, error: %d", __func__, error);
        return error;
    }
    return reply.ReadInt32();
}

int AbilityManagerProxy::UninstallApp(const std::string &bundleName)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return INNER_ERR;
    }
    if (!data.WriteString16(Str8ToStr16(bundleName))) {
        HILOG_ERROR("%{public}s , WriteString16 fail.", __func__);
        return ERR_INVALID_VALUE;
    }
    int error = Remote()->SendRequest(IAbilityManager::UNINSTALL_APP, data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("%s, error: %d", __func__, error);
        return error;
    }
    return reply.ReadInt32();
}

AbilityManagerStub::AbilityManagerStub()
{
    requestFuncMap_[TERMINATE_ABILITY] = &AbilityManagerStub::TerminateAbilityInner;
    requestFuncMap_[TERMINATE_ABILITY_BY_CALLER] = &AbilityManagerStub::TerminateAbilityByCallerInner;
    requestFuncMap_[ATTACH_ABILITY_THREAD] = &AbilityManagerStub::AttachAbilityThreadInner;
    requestFuncMap_[ABILITY_TRANSITION_DONE] = &AbilityManagerStub::AbilityTransitionDoneInner;
    requestFuncMap_[CONNECT_ABILITY_DONE] = &AbilityManagerStub::ScheduleConnectAbilityDoneInner;
    requestFuncMap_[DISCONNECT_ABILITY_DONE] = &AbilityManagerStub::ScheduleDisconnectAbilityDoneInner;
    requestFuncMap_[ADD_WINDOW_INFO] = &AbilityManagerStub::AddWindowInfoInner;
    requestFuncMap_[TERMINATE_ABILITY_RESULT] = &AbilityManagerStub::TerminateAbilityResultInner;
    requestFuncMap_[LIST_STACK_INFO] = &AbilityManagerStub::GetAllStackInfoInner;
    requestFuncMap_[GET_RECENT_MISSION] = &AbilityManagerStub::GetRecentMissionsInner;
    requestFuncMap_[REMOVE_MISSION] = &AbilityManagerStub::RemoveMissionInner;
    requestFuncMap_[REMOVE_STACK] = &AbilityManagerStub::RemoveStackInner;
    requestFuncMap_[COMMAND_ABILITY_DONE] = &AbilityManagerStub::ScheduleCommandAbilityDoneInner;
    requestFuncMap_[GET_MISSION_SNAPSHOT] = &AbilityManagerStub::GetMissionSnapshotInner;
    requestFuncMap_[ACQUIRE_DATA_ABILITY] = &AbilityManagerStub::AcquireDataAbilityInner;
    requestFuncMap_[RELEASE_DATA_ABILITY] = &AbilityManagerStub::ReleaseDataAbilityInner;
    requestFuncMap_[MOVE_MISSION_TO_TOP] = &AbilityManagerStub::MoveMissionToTopInner;
    requestFuncMap_[KILL_PROCESS] = &AbilityManagerStub::KillProcessInner;
    requestFuncMap_[UNINSTALL_APP] = &AbilityManagerStub::UninstallAppInner;
    requestFuncMap_[START_ABILITY] = &AbilityManagerStub::StartAbilityInner;
    requestFuncMap_[START_ABILITY_ADD_CALLER] = &AbilityManagerStub::StartAbilityAddCallerInner;
    requestFuncMap_[CONNECT_ABILITY] = &AbilityManagerStub::ConnectAbilityInner;
    requestFuncMap_[DISCONNECT_ABILITY] = &AbilityManagerStub::DisconnectAbilityInner;
    requestFuncMap_[STOP_SERVICE_ABILITY] = &AbilityManagerStub::StopServiceAbilityInner;
    requestFuncMap_[DUMP_STATE] = &AbilityManagerStub::DumpStateInner;
}

AbilityManagerStub::~AbilityManagerStub()
{
    requestFuncMap_.clear();
}

int AbilityManagerStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOG_DEBUG("AbilityManagerStub::OnRemoteRequest, cmd = %d, flags= %d", code, option.GetFlags());
    std::u16string descriptor = AbilityManagerStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HILOG_INFO("local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }

    auto itFunc = requestFuncMap_.find(code);
    if (itFunc != requestFuncMap_.end()) {
        auto requestFunc = itFunc->second;
        if (requestFunc != nullptr) {
            return (this->*requestFunc)(data, reply);
        }
    }
    HILOG_WARN("AbilityManagerStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int AbilityManagerStub::TerminateAbilityInner(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> token = data.ReadParcelable<IRemoteObject>();
    int resultCode = data.ReadInt32();
    Want *resultWant = data.ReadParcelable<Want>();
    int32_t result = TerminateAbility(token, resultCode, resultWant);
    reply.WriteInt32(result);
    if (resultWant != nullptr) {
        delete resultWant;
    }
    return NO_ERROR;
}

int AbilityManagerStub::TerminateAbilityByCallerInner(MessageParcel &data, MessageParcel &reply)
{
    auto callerToken = data.ReadParcelable<IRemoteObject>();
    int requestCode = data.ReadInt32();
    int32_t result = TerminateAbility(callerToken, requestCode);
    reply.WriteInt32(result);
    return NO_ERROR;
}

int AbilityManagerStub::AttachAbilityThreadInner(MessageParcel &data, MessageParcel &reply)
{
    auto scheduler = iface_cast<IAbilityScheduler>(data.ReadParcelable<IRemoteObject>());
    auto token = data.ReadParcelable<IRemoteObject>();
    int32_t result = AttachAbilityThread(scheduler, token);
    reply.WriteInt32(result);
    return NO_ERROR;
}

int AbilityManagerStub::AbilityTransitionDoneInner(MessageParcel &data, MessageParcel &reply)
{
    auto token = data.ReadParcelable<IRemoteObject>();
    int targetState = data.ReadInt32();
    int32_t result = AbilityTransitionDone(token, targetState);
    reply.WriteInt32(result);
    return NO_ERROR;
}

int AbilityManagerStub::ScheduleConnectAbilityDoneInner(MessageParcel &data, MessageParcel &reply)
{
    auto token = data.ReadParcelable<IRemoteObject>();
    auto remoteObject = data.ReadParcelable<IRemoteObject>();
    int32_t result = ScheduleConnectAbilityDone(token, remoteObject);
    reply.WriteInt32(result);
    return NO_ERROR;
}

int AbilityManagerStub::ScheduleDisconnectAbilityDoneInner(MessageParcel &data, MessageParcel &reply)
{
    auto token = data.ReadParcelable<IRemoteObject>();
    int32_t result = ScheduleDisconnectAbilityDone(token);
    reply.WriteInt32(result);
    return NO_ERROR;
}

int AbilityManagerStub::AddWindowInfoInner(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> token = data.ReadParcelable<IRemoteObject>();
    int windowToken = data.ReadInt32();
    AddWindowInfo(token, windowToken);
    return NO_ERROR;
}

int AbilityManagerStub::TerminateAbilityResultInner(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> token = data.ReadParcelable<IRemoteObject>();
    int startId = data.ReadInt32();
    int32_t result = TerminateAbilityResult(token, startId);
    reply.WriteInt32(result);
    return NO_ERROR;
}

int AbilityManagerStub::GetAllStackInfoInner(MessageParcel &data, MessageParcel &reply)
{
    StackInfo stackInfo;
    int32_t result = GetAllStackInfo(stackInfo);
    if (!reply.WriteInt32(result)) {
        HILOG_ERROR("AbilityManagerStub: GetAllStackInfo result error");
        return ERR_INVALID_VALUE;
    }
    if (!reply.WriteParcelable(&stackInfo)) {
        HILOG_ERROR("AbilityManagerStub: GetAllStackInfo error");
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

int AbilityManagerStub::GetRecentMissionsInner(MessageParcel &data, MessageParcel &reply)
{
    int numMax = data.ReadInt32();
    int flags = data.ReadInt32();
    std::vector<RecentMissionInfo> missionInfos;
    int32_t result = GetRecentMissions(numMax, flags, missionInfos);
    reply.WriteInt32(missionInfos.size());
    for (auto &it : missionInfos) {
        if (!reply.WriteParcelable(&it)) {
            return ERR_INVALID_VALUE;
        }
    }
    if (!reply.WriteInt32(result)) {
        return ERR_INVALID_VALUE;
    }
    return result;
}

int AbilityManagerStub::RemoveMissionInner(MessageParcel &data, MessageParcel &reply)
{
    int id = data.ReadInt32();
    int32_t result = RemoveMission(id);
    if (!reply.WriteInt32(result)) {
        HILOG_ERROR("AbilityManagerStub: remove mission error");
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

int AbilityManagerStub::RemoveStackInner(MessageParcel &data, MessageParcel &reply)
{
    int id = data.ReadInt32();
    int32_t result = RemoveStack(id);
    if (!reply.WriteInt32(result)) {
        HILOG_ERROR("AbilityManagerStub: remove stack error");
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

int AbilityManagerStub::ScheduleCommandAbilityDoneInner(MessageParcel &data, MessageParcel &reply)
{
    auto token = data.ReadParcelable<IRemoteObject>();
    int32_t result = ScheduleCommandAbilityDone(token);
    reply.WriteInt32(result);
    return NO_ERROR;
}

int AbilityManagerStub::GetMissionSnapshotInner(MessageParcel &data, MessageParcel &reply)
{
    MissionSnapshotInfo snapshot;
    int32_t missionId = data.ReadInt32();
    int32_t result = GetMissionSnapshot(missionId, snapshot);
    if (!reply.WriteParcelable(&snapshot)) {
        HILOG_ERROR("AbilityManagerStub: GetMissionSnapshot error");
        return ERR_INVALID_VALUE;
    }

    if (!reply.WriteInt32(result)) {
        HILOG_ERROR("AbilityManagerStub: GetMissionSnapshot result error");
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

int AbilityManagerStub::AcquireDataAbilityInner(MessageParcel &data, MessageParcel &reply)
{
    std::unique_ptr<Uri> uri(new Uri(data.ReadString()));
    bool tryBind = data.ReadBool();
    sptr<IRemoteObject> callerToken = data.ReadParcelable<IRemoteObject>();
    sptr<IAbilityScheduler> result = AcquireDataAbility(*uri, tryBind, callerToken);
    HILOG_DEBUG("acquire data ability %{public}s", result ? "ok" : "failed");
    if (result) {
        reply.WriteParcelable(result->AsObject());
    } else {
        reply.WriteParcelable(nullptr);
    }
    return NO_ERROR;
}

int AbilityManagerStub::ReleaseDataAbilityInner(MessageParcel &data, MessageParcel &reply)
{
    auto scheduler = iface_cast<IAbilityScheduler>(data.ReadParcelable<IRemoteObject>());
    auto callerToken = data.ReadParcelable<IRemoteObject>();
    int32_t result = ReleaseDataAbility(scheduler, callerToken);
    HILOG_DEBUG("release data ability ret = %d", result);
    reply.WriteInt32(result);
    return NO_ERROR;
}

int AbilityManagerStub::MoveMissionToTopInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t id = data.ReadInt32();
    int result = MoveMissionToTop(id);
    if (!reply.WriteInt32(result)) {
        HILOG_ERROR("AbilityManagerStub: move mission to top error");
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

int AbilityManagerStub::KillProcessInner(MessageParcel &data, MessageParcel &reply)
{
    std::string bundleName = Str16ToStr8(data.ReadString16());
    int result = KillProcess(bundleName);
    if (!reply.WriteInt32(result)) {
        HILOG_ERROR("AbilityManagerStub: remove stack error");
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

int AbilityManagerStub::UninstallAppInner(MessageParcel &data, MessageParcel &reply)
{
    std::string bundleName = Str16ToStr8(data.ReadString16());
    int result = UninstallApp(bundleName);
    if (!reply.WriteInt32(result)) {
        HILOG_ERROR("AbilityManagerStub: remove stack error");
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

int AbilityManagerStub::StartAbilityInner(MessageParcel &data, MessageParcel &reply)
{
    Want *want = data.ReadParcelable<Want>();
    if (want == nullptr) {
        HILOG_ERROR("AbilityManagerStub: want is nullptr");
        return ERR_INVALID_VALUE;
    }
    int requestCode = data.ReadInt32();
    int32_t result = StartAbility(*want, requestCode);
    reply.WriteInt32(result);
    delete want;
    return NO_ERROR;
}

int AbilityManagerStub::StartAbilityAddCallerInner(MessageParcel &data, MessageParcel &reply)
{
    Want *want = data.ReadParcelable<Want>();
    if (want == nullptr) {
        HILOG_ERROR("AbilityManagerStub: want is nullptr");
        return ERR_INVALID_VALUE;
    }
    auto callerToken = data.ReadParcelable<IRemoteObject>();
    int requestCode = data.ReadInt32();
    int32_t result = StartAbility(*want, callerToken, requestCode);
    reply.WriteInt32(result);
    delete want;
    return NO_ERROR;
}

int AbilityManagerStub::ConnectAbilityInner(MessageParcel &data, MessageParcel &reply)
{
    Want *want = data.ReadParcelable<Want>();
    if (want == nullptr) {
        HILOG_ERROR("AbilityManagerStub: want is nullptr");
        return ERR_INVALID_VALUE;
    }
    auto callback = iface_cast<IAbilityConnection>(data.ReadParcelable<IRemoteObject>());
    auto token = data.ReadParcelable<IRemoteObject>();
    int32_t result = ConnectAbility(*want, callback, token);
    reply.WriteInt32(result);
    delete want;
    return NO_ERROR;
}

int AbilityManagerStub::DisconnectAbilityInner(MessageParcel &data, MessageParcel &reply)
{
    auto callback = iface_cast<IAbilityConnection>(data.ReadParcelable<IRemoteObject>());
    int32_t result = DisconnectAbility(callback);
    HILOG_DEBUG("disconnect ability ret = %d", result);
    reply.WriteInt32(result);
    return NO_ERROR;
}

int AbilityManagerStub::StopServiceAbilityInner(MessageParcel &data, MessageParcel &reply)
{
    Want *want = data.ReadParcelable<Want>();
    if (want == nullptr) {
        HILOG_ERROR("AbilityManagerStub: want is nullptr");
        return ERR_INVALID_VALUE;
    }
    int32_t result = StopServiceAbility(*want);
    reply.WriteInt32(result);
    delete want;
    return NO_ERROR;
}

int AbilityManagerStub::DumpStateInner(MessageParcel &data, MessageParcel &reply)
{
    std::vector<std::string> result;
    std::string args = Str16ToStr8(data.ReadString16());
    std::vector<std::string> argList;
    SplitStr(args, " ", argList);
    if (argList.empty()) {
        return ERR_INVALID_VALUE;
    }
    DumpState(args, result);
    reply.WriteInt32(result.size());
    for (auto stack : result) {
        reply.WriteString16(Str8ToStr16(stack));
    }
    return NO_ERROR;
}
}  // namespace AAFwk
}  // namespace OHOS
