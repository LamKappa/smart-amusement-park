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
#include "system_test_ability_util.h"

namespace OHOS {
namespace STABUtil {
using namespace OHOS;
using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;
using namespace OHOS::MTUtil;
using namespace OHOS::STtools;
using namespace OHOS::EventFwk;

std::mutex STAbilityUtil::mutex_;
std::shared_ptr<STAbilityUtil> STAbilityUtil::instance_ = nullptr;

bool STAbilityUtil::PublishEvent(const std::string &eventName, const int &code, const std::string &data)
{
    Want want;
    want.SetAction(eventName);
    CommonEventData commonData;
    commonData.SetWant(want);
    commonData.SetCode(code);
    commonData.SetData(data);
    return CommonEventManager::PublishCommonEvent(commonData);
}

void STAbilityUtil::Install(const std::string &hapName)
{
    system(("bm install -p /system/vendor/" + hapName + ".hap > /dev/null 2>&1").c_str());
}

void STAbilityUtil::InstallHaps(vector_str &hapNames)
{
    for (auto hapName : hapNames) {
        Install(hapName);
    }
}

void STAbilityUtil::Uninstall(const std::string &bundleName)
{
    system(("bm uninstall -n " + bundleName + " > /dev/null 2>&1").c_str());
}

void STAbilityUtil::UninstallBundle(vector_str &bundleNames)
{
    for (auto bundleName : bundleNames) {
        Uninstall(bundleName);
    }
}

void STAbilityUtil::KillService(const std::string &serviceName)
{
    system(("kill -9 $(pidof " + serviceName + ") > /dev/null 2>&1").c_str());
}

void STAbilityUtil::StartService(const std::string &serviceName, const time_t &delay)
{
    system(("/system/bin/" + serviceName + "& > /dev/null 2>&1").c_str());
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}

void STAbilityUtil::KillBundleProcess(vector_str &bundleNames)
{
    for (std::string bundleName : bundleNames) {
        KillService(bundleName);
    }
}

sptr<IAbilityManager> STAbilityUtil::GetAbilityManagerService()
{
    sptr<IRemoteObject> abilityMsObj =
        OHOS::DelayedSingleton<SaMgrClient>::GetInstance()->GetSystemAbility(ABILITY_MGR_SERVICE_ID);
    if (abilityMsObj == nullptr) {
        HILOG_ERROR("failed to get ability manager service");
        return nullptr;
    }
    return iface_cast<IAbilityManager>(abilityMsObj);
}

sptr<IAppMgr> STAbilityUtil::GetAppMgrService()
{
    sptr<IRemoteObject> appMsObj =
        OHOS::DelayedSingleton<SaMgrClient>::GetInstance()->GetSystemAbility(APP_MGR_SERVICE_ID);
    if (appMsObj == nullptr) {
        HILOG_ERROR("failed to get app manager service");
        return nullptr;
    }
    return iface_cast<IAppMgr>(appMsObj);
}

ErrCode STAbilityUtil::StartAbility(const Want &want, sptr<IAbilityManager> &abilityMs, const time_t &delay)
{
    ErrCode result = OHOS::ERR_OK;
    abilityMs = GetAbilityManagerService();
    if (abilityMs == nullptr) {
        result = OHOS::ERR_INVALID_VALUE;
        HILOG_ERROR("failed to get ability manager service");
        return result;
    }

    result = abilityMs->StartAbility(want);
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    if (result == OHOS::ERR_OK) {
        HILOG_INFO("start ability successfully.");
    } else {
        HILOG_INFO("failed to start ability.");
    }

    return result;
}

bool STAbilityUtil::StopAbility(const std::string &eventName, const int &code, const std::string &data)
{
    return PublishEvent(eventName, code, data);
}

ErrCode STAbilityUtil::StopServiceAbility(const Want &want, unsigned int usec)
{
    ErrCode result = OHOS::ERR_OK;

    sptr<IAbilityManager> abilityMs = GetAbilityManagerService();
    if (abilityMs == nullptr) {
        result = OHOS::ERR_INVALID_VALUE;
        HILOG_ERROR("failed to get ability manager service");
        return result;
    }

    result = abilityMs->StopServiceAbility(want);
    if (result == OHOS::ERR_OK) {
        HILOG_INFO("stop service ability successfully.");
    } else {
        HILOG_INFO("failed to stop service ability.");
    }

    return result;
}

ErrCode STAbilityUtil::ConnectAbility(const Want &want, const sptr<IAbilityConnection> &connect,
    const sptr<IRemoteObject> &callerToken, unsigned int usec)
{
    ErrCode result = OHOS::ERR_OK;

    sptr<IAbilityManager> abilityMs = GetAbilityManagerService();
    if (abilityMs == nullptr) {
        result = OHOS::ERR_INVALID_VALUE;
        HILOG_ERROR("failed to get ability manager service");
        return result;
    }

    result = abilityMs->ConnectAbility(want, connect, callerToken);
    if (result == OHOS::ERR_OK) {
        HILOG_INFO("connect ability successfully.");
    } else {
        HILOG_INFO("failed to connect ability.");
    }

    return result;
}

ErrCode STAbilityUtil::DisconnectAbility(const sptr<IAbilityConnection> &connect, unsigned int usec)
{
    ErrCode result = OHOS::ERR_OK;

    sptr<IAbilityManager> abilityMs = GetAbilityManagerService();
    if (abilityMs == nullptr) {
        result = OHOS::ERR_INVALID_VALUE;
        HILOG_ERROR("failed to get ability manager service");
        return result;
    }

    result = abilityMs->DisconnectAbility(connect);
    if (result == OHOS::ERR_OK) {
        HILOG_INFO("StopServiceAbility successfully.");
    } else {
        HILOG_INFO("failed to StopServiceAbility.");
    }

    return result;
}

Want STAbilityUtil::MakeWant(std::string deviceId, std::string abilityName, std::string bundleName, MAP_STR_STR params)
{
    ElementName element(deviceId, bundleName, abilityName);
    Want want;
    want.SetElement(element);
    for (const auto &param : params) {
        want.SetParam(param.first, param.second);
    }
    return want;
}

Want STAbilityUtil::MakeWant(std::string deviceId, std::string abilityName, std::string bundleName, vector_str params)
{
    ElementName element(deviceId, bundleName, abilityName);
    Want want;
    want.SetElement(element);
    want.SetParam("operator", params);
    return want;
}

ErrCode STAbilityUtil::GetTopAbilityRecordId(int64_t &id, sptr<IAbilityManager> &abilityMs)
{
    ErrCode result = OHOS::ERR_OK;
    id = -1;
    abilityMs = GetAbilityManagerService();
    if (abilityMs == nullptr) {
        result = OHOS::ERR_INVALID_VALUE;
        HILOG_ERROR("failed to get ability manager service");
        return result;
    }
    StackInfo stackInfo;
    abilityMs->GetAllStackInfo(stackInfo);
    MissionStackInfo defaultMissionStack;
    for (const auto &stackInfo : stackInfo.missionStackInfos) {
        if (stackInfo.id == 1) {  // DEFAULT_MISSION_STACK_ID = 1
            defaultMissionStack = stackInfo;
            break;
        }
    }
    if (!defaultMissionStack.missionRecords.empty() &&
        !defaultMissionStack.missionRecords.begin()->abilityRecordInfos.empty()) {
        id = defaultMissionStack.missionRecords.begin()->abilityRecordInfos.begin()->id;
    }
    return result;
}

ErrCode STAbilityUtil::GetRunningProcessInfo(
    std::shared_ptr<RunningProcessInfo> &runningProcessInfo, sptr<IAppMgr> &appMs, const time_t &delay)
{
    ErrCode result = ERR_OK;
    appMs = GetAppMgrService();
    if (appMs == nullptr) {
        result = OHOS::ERR_INVALID_VALUE;
        HILOG_ERROR("failed to get app manager service");
        return result;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    result = appMs->GetAllRunningProcesses(runningProcessInfo);
    if (result == ERR_OK) {
        HILOG_INFO("get running process info successfully.");
    } else {
        HILOG_INFO("failed to get running process info.");
    }
    return result;
}

ErrCode STAbilityUtil::KillApplication(const std::string &appName, sptr<IAppMgr> &appMs, const time_t &delay)
{
    ErrCode result = ERR_OK;
    appMs = GetAppMgrService();
    if (appMs == nullptr) {
        result = OHOS::ERR_INVALID_VALUE;
        HILOG_ERROR("failed to get app manager service");
        return result;
    }
    result = appMs->GetAmsMgr()->KillApplication(appName);
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    if (result == ERR_OK) {
        HILOG_INFO("kill application:%{public}s successfully.", appName.c_str());
    } else {
        HILOG_INFO("failed to kill application:%{public}s.", appName.c_str());
    }
    return result;
}

AppProcessInfo STAbilityUtil::GetAppProcessInfoByName(
    const std::string &processName, sptr<IAppMgr> &appMs, const time_t &delay)
{
    AppProcessInfo appProcessInfo;
    appProcessInfo.pid_ = 0;
    std::shared_ptr<RunningProcessInfo> runningProcessInfo = std::make_shared<RunningProcessInfo>();
    if (ERR_OK == GetRunningProcessInfo(runningProcessInfo, appMs, delay)) {
        for (const auto &info : runningProcessInfo->appProcessInfos) {
            if (processName == info.processName_) {
                appProcessInfo = info;
            }
        }
    }
    runningProcessInfo.reset();
    return appProcessInfo;
}

int STAbilityUtil::WaitCompleted(Event &event, const std::string &eventName, const int code, const int timeout)
{
    HILOG_INFO("WaitCompleted");
    return event.WaitingMessage(std::to_string(code) + eventName, timeout, false);
}

void STAbilityUtil::Completed(Event &event, const std::string &eventName, const int code)
{
    HILOG_INFO("Completed");
    return event.CompleteMessage(std::to_string(code) + eventName);
}

void STAbilityUtil::Completed(Event &event, const std::string &eventName, const int code, const std::string &data)
{
    HILOG_INFO("STAbilityUtil::Completed");
    return event.CompleteMessage(std::to_string(code) + eventName, data);
}

std::string STAbilityUtil::GetData(Event &event, const std::string &eventName, const int code)
{
    HILOG_INFO("STAbilityUtil::GetData");
    return event.GetData(std::to_string(code) + eventName);
}

void STAbilityUtil::CleanMsg(Event &event)
{
    HILOG_INFO("CleanMsg");
    return event.Clean();
}

std::vector<std::string> STAbilityUtil::SerializationStOperatorToVector(StOperator &ParentOperator)
{
    std::vector<std::string> vectorOperator;
    PushOperatorInVector(vectorOperator, ParentOperator);
    return vectorOperator;
}

void STAbilityUtil::DeserializationStOperatorFromVector(
    StOperator &ParentOperator, std::vector<std::string> &vectorOperator)
{
    PullOperatorFromVector(ParentOperator, vectorOperator);
}

void STAbilityUtil::PushOperatorInVector(std::vector<std::string> &vectorOperator, StOperator &ParentOperator)
{
    vectorOperator.emplace_back(std::to_string(ParentOperator.GetChildOperator().size()));
    vectorOperator.emplace_back(ParentOperator.GetAbilityType());
    vectorOperator.emplace_back(ParentOperator.GetBundleName());
    vectorOperator.emplace_back(ParentOperator.GetAbilityName());
    vectorOperator.emplace_back(ParentOperator.GetOperatorName());
    vectorOperator.emplace_back(ParentOperator.GetMessage());
    for (auto child : ParentOperator.GetChildOperator()) {
        PushOperatorInVector(vectorOperator, *child);
    }
}

void STAbilityUtil::PullOperatorFromVector(StOperator &ParentOperator, std::vector<std::string> &vectorOperator)
{
    int childnum = std::stoi(vectorOperator.front());
    vectorOperator.erase(vectorOperator.begin());
    std::string abilityType = vectorOperator.front();
    vectorOperator.erase(vectorOperator.begin());
    std::string bundleName = vectorOperator.front();
    vectorOperator.erase(vectorOperator.begin());
    std::string abilityName = vectorOperator.front();
    vectorOperator.erase(vectorOperator.begin());
    std::string operatorName = vectorOperator.front();
    vectorOperator.erase(vectorOperator.begin());
    std::string message = vectorOperator.front();
    vectorOperator.erase(vectorOperator.begin());
    ParentOperator.SetAbilityType(abilityType)
        .SetBundleName(bundleName)
        .SetAbilityName(abilityName)
        .SetOperatorName(operatorName)
        .SetMessage(message);
    for (int i = 0; i < childnum; i++) {
        auto child = std::make_shared<StOperator>();
        if (child == nullptr) {
            return;
        }
        ParentOperator.AddChildOperator(child);
        PullOperatorFromVector(*(child.get()), vectorOperator);
    }
}

}  // namespace STABUtil
}  // namespace OHOS