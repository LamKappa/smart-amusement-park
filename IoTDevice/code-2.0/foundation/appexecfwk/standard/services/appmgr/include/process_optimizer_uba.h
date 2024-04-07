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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_APPMGR_INCLUDE_PROCESS_OPTIMIZER_UBA_H
#define FOUNDATION_APPEXECFWK_SERVICES_APPMGR_INCLUDE_PROCESS_OPTIMIZER_UBA_H

#include <chrono>
#include <functional>
#include <string>
#include <vector>
#include <variant>

#include "process_optimizer.h"

namespace OHOS {
namespace AppExecFwk {

// UBA short for User Behavior Analysis
using UbaService = int;
using UbaServicePtr = std::shared_ptr<UbaService>;

class ProcessOptimizerUBA : public ProcessOptimizer {
public:
    using AbilityToken = sptr<IRemoteObject>;

public:
    ProcessOptimizerUBA(const UbaServicePtr &ubaService, const LmkdClientPtr &lmkdClient = nullptr,
        int suspendTimeout = APP_SUSPEND_TIMEOUT_DEFAULT);

    virtual ~ProcessOptimizerUBA();

public:
    // callbacks
    std::function<AbilityPtr(AbilityToken)> GetAbilityByToken;

public:
    void OnAppAdded(const AppPtr &app) override;
    void OnAppRemoved(const AppPtr &app) override;
    void OnAppStateChanged(const AppPtr &app, const ApplicationState oldState) override;
    void OnAbilityStarted(const AbilityPtr &ability) override;
    void OnAbilityConnected(const AbilityPtr &ability, const AbilityPtr &targetAbility) override;
    void OnAbilityDisconnected(const AbilityPtr &ability, const AbilityPtr &targetAbility) override;
    void OnAbilityStateChanged(const AbilityPtr &ability, const AbilityState oldState) override;
    void OnAbilityVisibleChanged(const AbilityPtr &ability) override;
    void OnAbilityPerceptibleChanged(const AbilityPtr &ability) override;
    void OnAbilityRemoved(const AbilityPtr &ability) override;

protected:
    void OnLowMemoryAlert(const CgroupManager::LowMemoryLevel level) override;

private:
    using Clock = std::chrono::system_clock;
    using TimePoint = Clock::time_point;

    class BaseAbilityAction {
    public:
        BaseAbilityAction(const AbilityPtr &ability);
        ~BaseAbilityAction() = default;

        TimePoint GetTime() const;
        std::string GetTimeString() const;
        const std::string &GetName() const;

    private:
        TimePoint time_;
        std::string name_;
    };

    class StartAbilityAction : public BaseAbilityAction {
    public:
        StartAbilityAction(const AbilityPtr &ability, const AbilityPtr &preAbility);
        ~StartAbilityAction() = default;

        const std::string &GetPreName() const;

    private:
        std::string preName_;
    };

    class ConnectAbilityAction : public BaseAbilityAction {
    public:
        ConnectAbilityAction(const AbilityPtr &ability, const AbilityPtr &targetAbility);
        ~ConnectAbilityAction() = default;

        const std::string &GetTargetName() const;

    private:
        std::string targetName_;
    };

    class DisconnectAbilityAction : public BaseAbilityAction {
    public:
        DisconnectAbilityAction(const AbilityPtr &ability, const AbilityPtr &targetAbility);
        ~DisconnectAbilityAction() = default;

        const std::string &GetTargetName() const;

    private:
        std::string targetName_;
    };

    class ChangeAbilityStateAction : public BaseAbilityAction {
    public:
        ChangeAbilityStateAction(const AbilityPtr &ability, const AbilityState oldState);
        ~ChangeAbilityStateAction() = default;

        AbilityState GetOldState() const;
        AbilityState GetNewState() const;

    private:
        AbilityState oldState_;
        AbilityState newState_;
    };

    class ChangeAbilityVisible : public BaseAbilityAction {
    public:
        ChangeAbilityVisible(const AbilityPtr &ability);
        ~ChangeAbilityVisible() = default;
    };

    class ChangeAbilityPerceptible : public BaseAbilityAction {
    public:
        ChangeAbilityPerceptible(const AbilityPtr &ability);
        ~ChangeAbilityPerceptible() = default;
    };

    class RemoveAbilityAction : public BaseAbilityAction {
    public:
        RemoveAbilityAction(const AbilityPtr &ability);
        ~RemoveAbilityAction() = default;
    };

    using AbilityAction =
        std::variant<std::monostate, StartAbilityAction, ConnectAbilityAction, DisconnectAbilityAction,
            ChangeAbilityStateAction, ChangeAbilityVisible, ChangeAbilityPerceptible, RemoveAbilityAction>;

    template <typename T, typename... ARGS>
    void RecordAbilityAction(ARGS... args)
    {
        abilityActionCache_[abilityActionCount_++].emplace<T>(args...);
        if (abilityActionCount_ >= ABILITY_ACTION_CACHE_SIZE) {
            CommitAbilityActions();
        }
    }

    void CommitAbilityActions();
    UbaServicePtr GetUbaService();

private:
    UbaServicePtr ubaService_;
    static constexpr size_t ABILITY_ACTION_CACHE_SIZE = 100;
    AbilityAction abilityActionCache_[ABILITY_ACTION_CACHE_SIZE];
    size_t abilityActionCount_;
};
}  // namespace AppExecFwk
}  // namespace OHOS

#endif  // FOUNDATION_APPEXECFWK_SERVICES_APPMGR_INCLUDE_PROCESS_OPTIMIZER_UBA_H
