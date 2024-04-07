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

#include "app_mgr_client.h"

#include <cstdio>
#include <string>
#include <unistd.h>

#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"

#include "app_mgr_interface.h"
#include "app_service_manager.h"

namespace OHOS {
namespace AppExecFwk {

AppMgrClient::AppMgrClient()
{
    SetServiceManager(std::make_unique<AppServiceManager>());
}

AppMgrClient::~AppMgrClient()
{}

AppMgrResultCode AppMgrClient::LoadAbility(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &preToken,
    const AbilityInfo &abilityInfo, const ApplicationInfo &appInfo)
{
    sptr<IAppMgr> service = iface_cast<IAppMgr>(remote_);
    if (service != nullptr) {
        sptr<IAmsMgr> amsService = service->GetAmsMgr();
        if (amsService != nullptr) {
            // From here, separate AbilityInfo and ApplicationInfo from AA.
            std::shared_ptr<AbilityInfo> abilityInfoPtr = std::make_shared<AbilityInfo>(abilityInfo);
            std::shared_ptr<ApplicationInfo> appInfoPtr = std::make_shared<ApplicationInfo>(appInfo);
            amsService->LoadAbility(token, preToken, abilityInfoPtr, appInfoPtr);
            return AppMgrResultCode::RESULT_OK;
        }
    }
    return AppMgrResultCode::ERROR_SERVICE_NOT_CONNECTED;
}

AppMgrResultCode AppMgrClient::TerminateAbility(const sptr<IRemoteObject> &token)
{
    sptr<IAppMgr> service = iface_cast<IAppMgr>(remote_);
    if (service != nullptr) {
        sptr<IAmsMgr> amsService = service->GetAmsMgr();
        if (amsService != nullptr) {
            amsService->TerminateAbility(token);
            return AppMgrResultCode::RESULT_OK;
        }
    }
    return AppMgrResultCode::ERROR_SERVICE_NOT_CONNECTED;
}

AppMgrResultCode AppMgrClient::UpdateAbilityState(const sptr<IRemoteObject> &token, const AbilityState state)
{
    sptr<IAppMgr> service = iface_cast<IAppMgr>(remote_);
    if (service != nullptr) {
        sptr<IAmsMgr> amsService = service->GetAmsMgr();
        if (amsService != nullptr) {
            amsService->UpdateAbilityState(token, state);
            return AppMgrResultCode::RESULT_OK;
        }
    }
    return AppMgrResultCode::ERROR_SERVICE_NOT_CONNECTED;
}

AppMgrResultCode AppMgrClient::RegisterAppStateCallback(const sptr<IAppStateCallback> &callback)
{
    sptr<IAppMgr> service = iface_cast<IAppMgr>(remote_);
    if (service != nullptr) {
        sptr<IAmsMgr> amsService = service->GetAmsMgr();
        if (amsService != nullptr) {
            amsService->RegisterAppStateCallback(callback);
            return AppMgrResultCode::RESULT_OK;
        }
    }
    return AppMgrResultCode::ERROR_SERVICE_NOT_CONNECTED;
}

AppMgrResultCode AppMgrClient::Reset()
{
    sptr<IAppMgr> service = iface_cast<IAppMgr>(remote_);
    if (service != nullptr) {
        sptr<IAmsMgr> amsService = service->GetAmsMgr();
        if (amsService != nullptr) {
            amsService->Reset();
            return AppMgrResultCode::RESULT_OK;
        }
    }
    return AppMgrResultCode::ERROR_SERVICE_NOT_CONNECTED;
}

AppMgrResultCode AppMgrClient::AbilityBehaviorAnalysis(const sptr<IRemoteObject> &token,
    const sptr<IRemoteObject> &preToken, const int32_t visibility, const int32_t perceptibility,
    const int32_t connectionState)
{
    sptr<IAppMgr> service = iface_cast<IAppMgr>(remote_);
    if (service != nullptr) {
        sptr<IAmsMgr> amsService = service->GetAmsMgr();
        if (amsService != nullptr) {
            amsService->AbilityBehaviorAnalysis(token, preToken, visibility, perceptibility, connectionState);
            return AppMgrResultCode::RESULT_OK;
        }
    }
    return AppMgrResultCode::ERROR_SERVICE_NOT_CONNECTED;
}

AppMgrResultCode AppMgrClient::KillProcessByAbilityToken(const sptr<IRemoteObject> &token)
{
    sptr<IAppMgr> service = iface_cast<IAppMgr>(remote_);
    if (service != nullptr) {
        sptr<IAmsMgr> amsService = service->GetAmsMgr();
        if (amsService != nullptr) {
            amsService->KillProcessByAbilityToken(token);
            return AppMgrResultCode::RESULT_OK;
        }
    }
    return AppMgrResultCode::ERROR_SERVICE_NOT_CONNECTED;
}

AppMgrResultCode AppMgrClient::KillApplication(const std::string &bundleName)
{
    sptr<IAppMgr> service = iface_cast<IAppMgr>(remote_);
    if (service != nullptr) {
        sptr<IAmsMgr> amsService = service->GetAmsMgr();
        if (amsService != nullptr) {
            int32_t result = amsService->KillApplication(bundleName);
            if (result == ERR_OK) {
                return AppMgrResultCode::RESULT_OK;
            }
            return AppMgrResultCode::ERROR_SERVICE_NOT_READY;
        }
    }
    return AppMgrResultCode::ERROR_SERVICE_NOT_CONNECTED;
}

AppMgrResultCode AppMgrClient::ClearUpApplicationData(const std::string &bundleName)
{
    sptr<IAppMgr> service = iface_cast<IAppMgr>(remote_);
    if (service != nullptr) {
        service->ClearUpApplicationData(bundleName);
        return AppMgrResultCode::RESULT_OK;
    }
    return AppMgrResultCode::ERROR_SERVICE_NOT_CONNECTED;
}

AppMgrResultCode AppMgrClient::GetAllRunningProcesses(std::shared_ptr<RunningProcessInfo> &runningProcessInfo)
{
    sptr<IAppMgr> service = iface_cast<IAppMgr>(remote_);
    if (service != nullptr) {
        service->GetAllRunningProcesses(runningProcessInfo);
        return AppMgrResultCode::RESULT_OK;
    }
    return AppMgrResultCode::ERROR_SERVICE_NOT_CONNECTED;
}

AppMgrResultCode AppMgrClient::ConnectAppMgrService()
{
    if (!serviceManager_) {
        return AppMgrResultCode::ERROR_SERVICE_NOT_READY;
    }
    remote_ = serviceManager_->GetAppMgrService();
    if (!remote_) {
        return AppMgrResultCode::ERROR_SERVICE_NOT_READY;
    }
    return AppMgrResultCode::RESULT_OK;
}

void AppMgrClient::SetServiceManager(std::unique_ptr<AppServiceManager> serviceMgr)
{
    serviceManager_ = std::move(serviceMgr);
}

}  // namespace AppExecFwk
}  // namespace OHOS
