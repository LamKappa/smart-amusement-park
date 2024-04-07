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

#include "app_scheduler_proxy.h"

#include "iremote_object.h"
#include "ipc_types.h"

#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
AppSchedulerProxy::AppSchedulerProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IAppScheduler>(impl)
{}

bool AppSchedulerProxy::WriteInterfaceToken(MessageParcel &data)
{
    if (!data.WriteInterfaceToken(AppSchedulerProxy::GetDescriptor())) {
        APP_LOGE("write interface token failed");
        return false;
    }
    return true;
}

void AppSchedulerProxy::ScheduleForegroundApplication()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!WriteInterfaceToken(data)) {
        return;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        APP_LOGE("Remote() is NULL");
        return;
    }
    int32_t ret =
        remote->SendRequest(static_cast<uint32_t>(IAppScheduler::Message::SCHEDULE_FOREGROUND_APPLICATION_TRANSACTION),
            data,
            reply,
            option);
    if (ret != NO_ERROR) {
        APP_LOGW("SendRequest is failed, error code: %{public}d", ret);
    }
}

void AppSchedulerProxy::ScheduleBackgroundApplication()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!WriteInterfaceToken(data)) {
        return;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        APP_LOGE("Remote() is NULL");
        return;
    }
    int32_t ret =
        remote->SendRequest(static_cast<uint32_t>(IAppScheduler::Message::SCHEDULE_BACKGROUND_APPLICATION_TRANSACTION),
            data,
            reply,
            option);
    if (ret != NO_ERROR) {
        APP_LOGW("SendRequest is failed, error code: %{public}d", ret);
    }
}

void AppSchedulerProxy::ScheduleTerminateApplication()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!WriteInterfaceToken(data)) {
        return;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        APP_LOGE("Remote() is NULL");
        return;
    }
    int32_t ret = remote->SendRequest(
        static_cast<uint32_t>(IAppScheduler::Message::SCHEDULE_TERMINATE_APPLICATION_TRANSACTION), data, reply, option);
    if (ret != NO_ERROR) {
        APP_LOGW("SendRequest is failed, error code: %{public}d", ret);
    }
}

void AppSchedulerProxy::ScheduleLowMemory()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!WriteInterfaceToken(data)) {
        return;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        APP_LOGE("Remote() is NULL");
        return;
    }
    int32_t ret = remote->SendRequest(
        static_cast<uint32_t>(IAppScheduler::Message::SCHEDULE_LOWMEMORY_APPLICATION_TRANSACTION), data, reply, option);
    if (ret != NO_ERROR) {
        APP_LOGW("SendRequest is failed, error code: %{public}d", ret);
    }
}

void AppSchedulerProxy::ScheduleShrinkMemory(const int32_t level)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!WriteInterfaceToken(data)) {
        return;
    }
    data.WriteInt32(level);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        APP_LOGE("Remote() is NULL");
        return;
    }
    int32_t ret = remote->SendRequest(
        static_cast<uint32_t>(IAppScheduler::Message::SCHEDULE_SHRINK_MEMORY_APPLICATION_TRANSACTION),
        data,
        reply,
        option);
    if (ret != NO_ERROR) {
        APP_LOGW("SendRequest is failed, error code: %{public}d", ret);
    }
}

void AppSchedulerProxy::ScheduleLaunchAbility(const AbilityInfo &info, const sptr<IRemoteObject> &token)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!WriteInterfaceToken(data)) {
        return;
    }
    data.WriteParcelable(&info);
    data.WriteParcelable(token.GetRefPtr());
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        APP_LOGE("Remote() is NULL");
        return;
    }
    int32_t ret = remote->SendRequest(
        static_cast<uint32_t>(IAppScheduler::Message::SCHEDULE_LAUNCH_ABILITY_TRANSACTION), data, reply, option);
    if (ret != NO_ERROR) {
        APP_LOGW("SendRequest is failed, error code: %{public}d", ret);
    }
}

void AppSchedulerProxy::ScheduleCleanAbility(const sptr<IRemoteObject> &token)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!WriteInterfaceToken(data)) {
        return;
    }
    data.WriteParcelable(token.GetRefPtr());
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        APP_LOGE("Remote() is NULL");
        return;
    }
    int32_t ret = remote->SendRequest(
        static_cast<uint32_t>(IAppScheduler::Message::SCHEDULE_CLEAN_ABILITY_TRANSACTION), data, reply, option);
    if (ret != NO_ERROR) {
        APP_LOGW("SendRequest is failed, error code: %{public}d", ret);
    }
}

void AppSchedulerProxy::ScheduleLaunchApplication(const AppLaunchData &launchData)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!WriteInterfaceToken(data)) {
        return;
    }
    data.WriteParcelable(&launchData);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        APP_LOGE("Remote() is NULL");
        return;
    }
    int32_t ret = remote->SendRequest(
        static_cast<uint32_t>(IAppScheduler::Message::SCHEDULE_LAUNCH_APPLICATION_TRANSACTION), data, reply, option);
    if (ret != NO_ERROR) {
        APP_LOGW("SendRequest is failed, error code: %{public}d", ret);
    }
}

void AppSchedulerProxy::ScheduleProfileChanged(const Profile &profile)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!WriteInterfaceToken(data)) {
        return;
    }
    data.WriteParcelable(&profile);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        APP_LOGE("Remote() is NULL");
        return;
    }
    int32_t ret = remote->SendRequest(
        static_cast<uint32_t>(IAppScheduler::Message::SCHEDULE_PROFILE_CHANGED_TRANSACTION), data, reply, option);
    if (ret != NO_ERROR) {
        APP_LOGW("SendRequest is failed, error code: %{public}d", ret);
    }
}

void AppSchedulerProxy::ScheduleConfigurationUpdated(const Configuration &config)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!WriteInterfaceToken(data)) {
        return;
    }
    data.WriteParcelable(&config);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        APP_LOGE("Remote() is NULL");
        return;
    }
    int32_t ret = remote->SendRequest(
        static_cast<uint32_t>(IAppScheduler::Message::SCHEDULE_CONFIGURATION_UPDATED), data, reply, option);
    if (ret != NO_ERROR) {
        APP_LOGW("SendRequest is failed, error code: %{public}d", ret);
    }
}

void AppSchedulerProxy::ScheduleProcessSecurityExit()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!WriteInterfaceToken(data)) {
        return;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        APP_LOGE("Remote() is NULL");
        return;
    }
    int32_t ret = remote->SendRequest(
        static_cast<uint32_t>(IAppScheduler::Message::SCHEDULE_PROCESS_SECURITY_EXIT_TRANSACTION), data, reply, option);
    if (ret != NO_ERROR) {
        APP_LOGW("SendRequest is failed, error code: %{public}d", ret);
    }
}

}  // namespace AppExecFwk
}  // namespace OHOS
