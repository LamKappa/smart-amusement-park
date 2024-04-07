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

#include "ability_local_record.h"
#include "ability_impl.h"

namespace OHOS {
namespace AppExecFwk {
/**
 *
 * default constructor
 *
 */
AbilityLocalRecord::AbilityLocalRecord(const std::shared_ptr<AbilityInfo> &info, const sptr<IRemoteObject> &token)
    : abilityInfo_(info), token_(token)
{}

/**
 *
 * @default Destructor
 *
 */
AbilityLocalRecord::~AbilityLocalRecord()
{}

/**
 * @description: Get an AbilityInfo in an ability.
 *
 * @return Returns a pointer to abilityinfo
 */
const std::shared_ptr<AbilityInfo> &AbilityLocalRecord::GetAbilityInfo()
{
    return abilityInfo_;
}

/**
 * @description: Get an EventHandler in an ability.
 *
 * @return Returns a pointer to EventHandler
 */
const std::shared_ptr<EventHandler> &AbilityLocalRecord::GetEventHandler()
{
    return handler_;
}

/**
 * @description: Set an EventHandler in an ability.
 * @param handler EventHandler object
 * @return None.
 */
void AbilityLocalRecord::SetEventHandler(const std::shared_ptr<EventHandler> &handler)
{
    handler_ = handler;
}

/**
 * @description: Get an EventRunner in an ability.
 *
 * @return Returns a pointer to EventRunner
 */
const std::shared_ptr<EventRunner> &AbilityLocalRecord::GetEventRunner()
{
    return runner_;
}

/**
 * @description: Set an EventRunner in an ability.
 * @param runner EventHandler object
 * @return None.
 */
void AbilityLocalRecord::SetEventRunner(const std::shared_ptr<EventRunner> &runner)
{
    runner_ = runner;
}

/**
 * @description: Gets the identity of the ability
 * @return return the identity of the ability.
 */
const sptr<IRemoteObject> &AbilityLocalRecord::GetToken()
{
    return token_;
}

/**
 * @description: Get an AbilityImpl in an ability.
 *
 * @return Returns AbilityImpl pointer
 */
const std::shared_ptr<AbilityImpl> &AbilityLocalRecord::GetAbilityImpl()
{
    return abilityImpl_;
}

/**
 * @description: Set an AbilityImpl in an ability.
 * @param abilityImpl AbilityImpl object
 * @return None.
 */
void AbilityLocalRecord::SetAbilityImpl(const std::shared_ptr<AbilityImpl> &abilityImpl)
{
    abilityImpl_ = abilityImpl;
}

/**
 * @description: Obtains the information based on ability thread.
 * @return return AbilityThread Pointer
 */
const sptr<AbilityThread> &AbilityLocalRecord::GetAbilityThread()
{
    return abilityThread_;
}

/**
 * @description: Set an AbilityThread in an ability.
 * @param abilityThread AbilityThread object
 * @return None.
 */
void AbilityLocalRecord::SetAbilityThread(const sptr<AbilityThread> &abilityThread)
{
    abilityThread_ = abilityThread;
}
}  // namespace AppExecFwk
}  // namespace OHOS