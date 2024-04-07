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

#include "ohos_application.h"
#include "application_impl.h"
#include "ability_record_mgr.h"
#include "app_loader.h"
#include "app_log_wrapper.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace AppExecFwk {

REGISTER_APPLICATION(OHOSApplication, OHOSApplication)

OHOSApplication::OHOSApplication()
{
    abilityLifecycleCallbacks_.clear();
    elementsCallbacks_.clear();
}

/**
 *
 * @brief Called when Ability#onSaveAbilityState(PacMap) was called on an ability.
 *
 * @param outState Indicates the PacMap object passed to Ability#onSaveAbilityState(PacMap)
 * for storing user data and states. This parameter cannot be null.
 */

void OHOSApplication::DispatchAbilitySavedState(const PacMap &outState)
{
    APP_LOGI("OHOSApplication::dispatchAbilitySavedState: called");
    for (auto callback : abilityLifecycleCallbacks_) {
        if (callback != nullptr) {
            callback->OnAbilitySaveState(outState);
        }
    }
}

/**
 *
 * @brief Will be called the application foregrounds
 *
 */
void OHOSApplication::OnForeground()
{}

/**
 *
 * @brief Will be called the application backgrounds
 *
 */
void OHOSApplication::OnBackground()
{}

void OHOSApplication::DumpApplication()
{
    APP_LOGD("OHOSApplication::Dump called");
    // create and initialize abilityInfos
    std::shared_ptr<AbilityInfo> abilityInfo = nullptr;
    std::shared_ptr<AbilityLocalRecord> record = nullptr;

    if (abilityRecordMgr_) {
        record = abilityRecordMgr_->GetAbilityItem(abilityRecordMgr_->GetToken());
    }

    if (record) {
        abilityInfo = record->GetAbilityInfo();
    }

    if (abilityInfo) {
        APP_LOGD("==============AbilityInfo==============");
        APP_LOGD("abilityInfo: package: %{public}s", abilityInfo->package.c_str());
        APP_LOGD("abilityInfo: name: %{public}s", abilityInfo->name.c_str());
        APP_LOGD("abilityInfo: label: %{public}s", abilityInfo->label.c_str());
        APP_LOGD("abilityInfo: description: %{public}s", abilityInfo->description.c_str());
        APP_LOGD("abilityInfo: iconPath: %{public}s", abilityInfo->iconPath.c_str());
        APP_LOGD("abilityInfo: visible: %{public}d", abilityInfo->visible);
        APP_LOGD("abilityInfo: kind: %{public}s", abilityInfo->kind.c_str());
        APP_LOGD("abilityInfo: type: %{public}d", abilityInfo->type);
        APP_LOGD("abilityInfo: orientation: %{public}d", abilityInfo->orientation);
        APP_LOGD("abilityInfo: launchMode: %{public}d", abilityInfo->launchMode);
        for (auto permission : abilityInfo->permissions) {
            APP_LOGD("abilityInfo: permission: %{public}s", permission.c_str());
        }
        APP_LOGD("abilityInfo: bundleName: %{public}s", abilityInfo->bundleName.c_str());
        APP_LOGD("abilityInfo: applicationName: %{public}s", abilityInfo->applicationName.c_str());
    }

    // create and initialize applicationInfo
    APP_LOGD("==============applicationInfo==============");
    std::shared_ptr<ApplicationInfo> applicationInfoPtr = GetApplicationInfo();
    if (applicationInfoPtr != nullptr) {
        APP_LOGD("applicationInfo: name: %{public}s", applicationInfoPtr->name.c_str());
        APP_LOGD("applicationInfo: bundleName: %{public}s", applicationInfoPtr->bundleName.c_str());
        APP_LOGD("applicationInfo: signatureKey: %{public}s", applicationInfoPtr->signatureKey.c_str());
    }
}

/**
 *
 * @brief Set the abilityRecordMgr to the OHOSApplication.
 *
 * @param abilityRecordMgr
 */
void OHOSApplication::SetAbilityRecordMgr(const std::shared_ptr<AbilityRecordMgr> &abilityRecordMgr)
{
    if (abilityRecordMgr == nullptr) {
        APP_LOGE("ContextDeal::SetAbilityRecordMgr failed, abilityRecordMgr is nullptr");
        return;
    }
    abilityRecordMgr_ = abilityRecordMgr;
}

/**
 *
 * Register AbilityLifecycleCallbacks with OHOSApplication
 *
 * @param callBack callBack When the life cycle of the ability in the application changes,
 */
void OHOSApplication::RegisterAbilityLifecycleCallbacks(const std::shared_ptr<AbilityLifecycleCallbacks> &callBack)
{
    APP_LOGI("OHOSApplication::RegisterAbilityLifecycleCallbacks: called");

    if (callBack == nullptr) {
        APP_LOGI("OHOSApplication::RegisterAbilityLifecycleCallbacks: observer is null");
        return;
    }

    abilityLifecycleCallbacks_.emplace_back(callBack);
}

/**
 *
 * Unregister AbilityLifecycleCallbacks with OHOSApplication
 *
 * @param callBack RegisterAbilityLifecycleCallbacks`s callBack
 */
void OHOSApplication::UnregisterAbilityLifecycleCallbacks(const std::shared_ptr<AbilityLifecycleCallbacks> &callBack)
{
    APP_LOGI("OHOSApplication::UnregisterAbilityLifecycleCallbacks: called");

    if (callBack == nullptr) {
        APP_LOGI("OHOSApplication::UnregisterAbilityLifecycleCallbacks: observer is null");
        return;
    }

    abilityLifecycleCallbacks_.remove(callBack);
}

/**
 *
 * Will be called when the given ability calls Ability->onStart
 *
 * @param Ability Indicates the ability object that calls the onStart() method.
 */
void OHOSApplication::OnAbilityStart(const std::shared_ptr<Ability> &ability)
{
    if (ability == nullptr) {
        APP_LOGE("ContextDeal::OnAbilityStart failed, ability is nullptr");
        return;
    }

    APP_LOGI("OHOSApplication::OnAbilityStart: called");
    for (auto callback : abilityLifecycleCallbacks_) {
        if (callback != nullptr) {
            callback->OnAbilityStart(ability);
        }
    }
}

/**
 *
 * Will be called when the given ability calls Ability->onInactive
 *
 * @param Ability Indicates the Ability object that calls the onInactive() method.
 */
void OHOSApplication::OnAbilityInactive(const std::shared_ptr<Ability> &ability)
{
    if (ability == nullptr) {
        APP_LOGE("ContextDeal::OnAbilityInactive failed, ability is nullptr");
        return;
    }

    APP_LOGI("OHOSApplication::OnAbilityInactive: called");
    for (auto callback : abilityLifecycleCallbacks_) {
        if (callback != nullptr) {
            callback->OnAbilityInactive(ability);
        }
    }
}

/**
 *
 * Will be called when the given ability calls Ability->onBackground
 *
 * @param Ability Indicates the Ability object that calls the onBackground() method.
 */
void OHOSApplication::OnAbilityBackground(const std::shared_ptr<Ability> &ability)
{
    if (ability == nullptr) {
        APP_LOGE("ContextDeal::OnAbilityBackground failed, ability is nullptr");
        return;
    }

    APP_LOGI("OHOSApplication::OnAbilityBackground: called");
    for (auto callback : abilityLifecycleCallbacks_) {
        if (callback != nullptr) {
            callback->OnAbilityBackground(ability);
        }
    }
}

/**
 *
 * Will be called when the given ability calls Ability->onForeground
 *
 * @param Ability Indicates the Ability object that calls the onForeground() method.
 */
void OHOSApplication::OnAbilityForeground(const std::shared_ptr<Ability> &ability)
{
    if (ability == nullptr) {
        APP_LOGE("ContextDeal::OnAbilityForeground failed, ability is nullptr");
        return;
    }

    APP_LOGI("OHOSApplication::OnAbilityForeground: called");
    for (auto callback : abilityLifecycleCallbacks_) {
        if (callback != nullptr) {
            callback->OnAbilityForeground(ability);
        }
    }
}

/**
 *
 * Will be called when the given ability calls Ability->onActive
 *
 * @param Ability Indicates the Ability object that calls the onActive() method.
 */
void OHOSApplication::OnAbilityActive(const std::shared_ptr<Ability> &ability)
{
    if (ability == nullptr) {
        APP_LOGE("ContextDeal::OnAbilityActive failed, ability is nullptr");
        return;
    }

    APP_LOGI("OHOSApplication::OnAbilityActive: called");
    for (auto callback : abilityLifecycleCallbacks_) {
        if (callback != nullptr) {
            callback->OnAbilityActive(ability);
        }
    }
}

/**
 *
 * Will be called when the given ability calls Ability->onStop
 *
 * @param Ability Indicates the Ability object that calls the onStop() method.
 */
void OHOSApplication::OnAbilityStop(const std::shared_ptr<Ability> &ability)
{
    if (ability == nullptr) {
        APP_LOGE("ContextDeal::OnAbilityStop failed, ability is nullptr");
        return;
    }

    APP_LOGI("OHOSApplication::OnAbilityStop: called");
    for (auto callback : abilityLifecycleCallbacks_) {
        if (callback != nullptr) {
            callback->OnAbilityStop(ability);
        }
    }
}

/**
 *
 * @brief Register ElementsCallback with OHOSApplication
 *
 * @param callBack callBack when the system configuration of the device changes.
 */
void OHOSApplication::RegisterElementsCallbacks(const std::shared_ptr<ElementsCallback> &callback)
{
    APP_LOGI("OHOSApplication::RegisterElementsCallbacks: called");

    if (callback == nullptr) {
        APP_LOGI("OHOSApplication::RegisterElementsCallbacks: observer is null");
        return;
    }

    elementsCallbacks_.emplace_back(callback);
}

/**
 *
 * @brief Unregister ElementsCallback with OHOSApplication
 *
 * @param callback RegisterElementsCallbacks`s callback
 */
void OHOSApplication::UnregisterElementsCallbacks(const std::shared_ptr<ElementsCallback> &callback)
{
    APP_LOGI("OHOSApplication::UnregisterElementsCallbacks: called");

    if (callback == nullptr) {
        APP_LOGI("OHOSApplication::UnregisterElementsCallbacks: observer is null");
        return;
    }

    elementsCallbacks_.remove(callback);
}

/**
 *
 * @brief Will be Called when the system configuration of the device changes.
 *
 * @param config Indicates the new Configuration object.
 */
void OHOSApplication::OnConfigurationUpdated(const Configuration &config)
{
    APP_LOGI("OHOSApplication::OnConfigurationUpdated: called");
    for (auto callback : elementsCallbacks_) {
        if (callback != nullptr) {
            callback->OnConfigurationUpdated(nullptr, config);
        }
    }
}

/**
 *
 * @brief Called when the system has determined to trim the memory, for example,
 * when the ability is running in the background and there is no enough memory for
 * running as many background processes as possible.
 *
 * @param level Indicates the memory trim level, which shows the current memory usage status.
 */
void OHOSApplication::OnMemoryLevel(int level)
{
    APP_LOGI("OHOSApplication::OnMemoryLevel: called");
    for (auto callback : elementsCallbacks_) {
        if (callback != nullptr) {
            callback->OnMemoryLevel(level);
        }
    }
}

/**
 *
 * @brief Will be called the application starts
 *
 */
void OHOSApplication::OnStart()
{
    APP_LOGI("OHOSApplication::OnStart: called");
}

/**
 *
 * @brief Will be called the application ends
 *
 */
void OHOSApplication::OnTerminate()
{
    APP_LOGI("OHOSApplication::OnTerminate: called");
}

/**
 *
 * @brief Called when an ability calls Ability#onSaveAbilityState(PacMap).
 * You can implement your own logic in this method.
 * @param outState IIndicates the {@link PacMap} object passed to the onSaveAbilityState() callback.
 *
 */
void OHOSApplication::OnAbilitySaveState(const PacMap &outState)
{
    DispatchAbilitySavedState(outState);
}

}  // namespace AppExecFwk
}  // namespace OHOS
