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

#ifndef OHOS_AAFWK_LIFECYCLE_DEAL_H
#define OHOS_AAFWK_LIFECYCLE_DEAL_H

#include <memory>

#include "ability_scheduler_interface.h"
#include "want.h"
#include "nocopyable.h"

namespace OHOS {
namespace AAFwk {
/**
 * @class LifecycleDeal
 * LifecycleDeal schedule ability life.
 */
class LifecycleDeal {
public:
    LifecycleDeal();
    virtual ~LifecycleDeal();

    /**
     * set scheduler for accessing ability
     *
     * @param scheduler, ability thread ipc proxy.
     */
    void SetScheduler(const sptr<IAbilityScheduler> &scheduler);

    /**
     * schedule ability life
     *
     */
    void Activate(const Want &want, LifeCycleStateInfo &stateInfo);
    void Inactivate(const Want &want, LifeCycleStateInfo &stateInfo);
    void MoveToBackground(const Want &want, LifeCycleStateInfo &stateInfo);
    void ConnectAbility(const Want &want);
    void DisconnectAbility(const Want &want);
    void Terminate(const Want &want, LifeCycleStateInfo &stateInfo);
    void CommandAbility(const Want &want, bool reStart, int startId);

private:
    sptr<IAbilityScheduler> abilityScheduler_;  // kit interface used to schedule ability life

    DISALLOW_COPY_AND_MOVE(LifecycleDeal);
};
}  // namespace AAFwk
}  // namespace OHOS
#endif  // OHOS_AAFWK_LIFECYCLE_DEAL_H