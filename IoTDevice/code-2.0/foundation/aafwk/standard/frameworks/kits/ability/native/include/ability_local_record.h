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

#ifndef FOUNDATION_APPEXECFWK_ABILITY_ITEM_H
#define FOUNDATION_APPEXECFWK_ABILITY_ITEM_H

#include <string>

#include "iremote_object.h"
#include "event_runner.h"
#include "ability_info.h"
#include "application_info.h"
#include "refbase.h"
namespace OHOS {
namespace AppExecFwk {
class AbilityThread;
class AbilityImpl;
class AbilityLocalRecord {
public:
    /**
     *
     * default constructor
     *
     */
    AbilityLocalRecord(const std::shared_ptr<AbilityInfo> &info, const sptr<IRemoteObject> &token);

    /**
     *
     * @default Destructor
     *
     */
    virtual ~AbilityLocalRecord();

    /**
     * @description: Get an AbilityInfo in an ability.
     *
     * @return Returns a pointer to abilityinfo.
     */
    const std::shared_ptr<AbilityInfo> &GetAbilityInfo();

    /**
     * @description: Get an EventHandler in an ability.
     *
     * @return Returns a pointer to EventHandler
     */
    const std::shared_ptr<EventHandler> &GetEventHandler();

    /**
     * @description: Set an EventHandler in an ability.
     * @param handler EventHandler object
     * @return None.
     */
    void SetEventHandler(const std::shared_ptr<EventHandler> &handler);

    /**
     * @description: Get an EventRunner in an ability.
     *
     * @return Returns a pointer to EventRunner
     */
    const std::shared_ptr<EventRunner> &GetEventRunner();

    /**
     * @description: Set an EventRunner in an ability.
     * @param runner EventHandler object
     * @return None.
     */
    void SetEventRunner(const std::shared_ptr<EventRunner> &runner);

    /**
     * @description: Gets the identity of the ability
     * @return return the identity of the ability.
     */
    const sptr<IRemoteObject> &GetToken();

    /**
     * @description: Get an AbilityImpl in an ability.
     *
     * @return Returns AbilityImpl pointer
     */
    const std::shared_ptr<AbilityImpl> &GetAbilityImpl();

    /**
     * @description: Set an AbilityImpl in an ability.
     * @param abilityImpl AbilityImpl object
     * @return None.
     */
    void SetAbilityImpl(const std::shared_ptr<AbilityImpl> &abilityImpl);

    /**
     * @description: Obtains the information based on ability thread.
     * @return return AbilityThread Pointer
     */
    const sptr<AbilityThread> &GetAbilityThread();

    /**
     * @description: Set an AbilityThread in an ability.
     * @param abilityThread AbilityThread object
     * @return None.
     */
    void SetAbilityThread(const sptr<AbilityThread> &abilityThread);

private:
    std::shared_ptr<AbilityInfo> abilityInfo_;
    sptr<IRemoteObject> token_;
    std::shared_ptr<EventRunner> runner_;
    std::shared_ptr<EventHandler> handler_;
    std::shared_ptr<AbilityImpl> abilityImpl_;  // store abilityImpl
    sptr<AbilityThread> abilityThread_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_ABILITY_ITEM_H
