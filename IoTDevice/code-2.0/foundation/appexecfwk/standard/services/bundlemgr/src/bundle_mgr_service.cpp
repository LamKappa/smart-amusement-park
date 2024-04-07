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

#include "bundle_mgr_service.h"

#include "datetime_ex.h"
#include "app_log_wrapper.h"
#include "perf_profile.h"
#include "system_ability_definition.h"
#include "bundle_constants.h"
#include "system_ability_helper.h"

namespace OHOS {
namespace AppExecFwk {

const bool REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(DelayedSingleton<BundleMgrService>::GetInstance().get());

BundleMgrService::BundleMgrService() : SystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, true)
{
    APP_LOGI("instance is created");
    PerfProfile::GetInstance().SetBmsLoadStartTime(GetTickCount());
}

BundleMgrService::~BundleMgrService()
{
    if (host_ != nullptr) {
        host_ = nullptr;
    }
    if (installer_ != nullptr) {
        installer_ = nullptr;
    }
    if (handler_) {
        handler_.reset();
    }
    if (runner_) {
        runner_.reset();
    }
    if (dataMgr_) {
        dataMgr_.reset();
    }
    APP_LOGI("instance is destroyed");
}

void BundleMgrService::OnStart()
{
    APP_LOGD("start is triggered");
    if (!Init()) {
        APP_LOGE("init fail");
        return;
    }

    if (!registerToService_) {
        if (!SystemAbilityHelper::AddSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, host_)) {
            APP_LOGE("fail to register to system ability manager");
            return;
        }
        APP_LOGI("register to sam success");
        registerToService_ = true;
    }

    PerfProfile::GetInstance().SetBmsLoadEndTime(GetTickCount());
    if (!needToScan_) {
        PerfProfile::GetInstance().Dump();
    }
}

void BundleMgrService::OnStop()
{
    APP_LOGI("OnStop is called");
    SelfClean();
}

bool BundleMgrService::IsServiceReady() const
{
    return ready_;
}

bool BundleMgrService::Init()
{
    if (ready_) {
        APP_LOGW("init more than one time");
        return false;
    }

    if (host_ == nullptr) {
        host_ = new (std::nothrow) BundleMgrHostImpl();
        if (!host_) {
            APP_LOGE("create host instance fail");
            return false;
        }
    }

    APP_LOGI("init begin");

    if (!runner_) {
        runner_ = EventRunner::Create(Constants::BMS_SERVICE_NAME);
        if (!runner_) {
            APP_LOGE("create runner fail");
            return false;
        }
    }
    APP_LOGD("create runner success");

    if (!handler_) {
        handler_ = std::make_shared<BMSEventHandler>(runner_);
        if (!handler_) {
            APP_LOGE("create bms event handler fail");
            return false;
        }
    }
    APP_LOGD("create handler success");

    if (!installer_) {
        installer_ = new (std::nothrow) BundleInstallerHost();
        if (!installer_ || !installer_->Init()) {
            APP_LOGE("init installer fail");
            return false;
        }
    }
    APP_LOGD("create installer host success");

    if (!dataMgr_) {
        APP_LOGI("Create BundledataMgr");
        dataMgr_ = std::make_shared<BundleDataMgr>();
    }
    APP_LOGD("create dataManager success");

    if (!(dataMgr_->LoadDataFromPersistentStorage())) {
        APP_LOGW("load data from persistent storage fail");
        handler_->SendEvent(BMSEventHandler::BUNDLE_SCAN_START);
        needToScan_ = true;
    }

    ready_ = true;
    APP_LOGI("init end success");
    return true;
}

sptr<IBundleInstaller> BundleMgrService::GetBundleInstaller() const
{
    return installer_;
}

const std::shared_ptr<BundleDataMgr> BundleMgrService::GetDataMgr() const
{
    return dataMgr_;
}

void BundleMgrService::SelfClean()
{
    if (ready_) {
        ready_ = false;
        if (registerToService_) {
            registerToService_ = false;
        }
        if (needToScan_) {
            needToScan_ = false;
        }
    }
}

}  // namespace AppExecFwk
}  // namespace OHOS
