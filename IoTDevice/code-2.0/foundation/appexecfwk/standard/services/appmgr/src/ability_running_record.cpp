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

#include "ability_running_record.h"

#include "iremote_object.h"

namespace OHOS {
namespace AppExecFwk {

AbilityRunningRecord::AbilityRunningRecord(const std::shared_ptr<AbilityInfo> &info, const sptr<IRemoteObject> &token)
    : info_(info), token_(token)
{}

AbilityRunningRecord::~AbilityRunningRecord()
{}

const std::string &AbilityRunningRecord::GetName() const
{
    return info_->name;
}

const std::shared_ptr<AbilityInfo> &AbilityRunningRecord::GetAbilityInfo() const
{
    return info_;
}

const sptr<IRemoteObject> &AbilityRunningRecord::GetToken() const
{
    return token_;
}

void AbilityRunningRecord::SetState(const AbilityState state)
{
    state_ = state;
}

AbilityState AbilityRunningRecord::GetState() const
{
    return state_;
}

bool AbilityRunningRecord::IsSameState(const AbilityState state) const
{
    return state_ == state;
}

int32_t AbilityRunningRecord::GetLastLaunchTime() const
{
    return lastLaunchTime_;
}

const sptr<IRemoteObject> AbilityRunningRecord::GetPreToken() const
{
    return preToken_;
}

void AbilityRunningRecord::SetPreToken(const sptr<IRemoteObject> &preToken)
{
    preToken_ = preToken;
}

void AbilityRunningRecord::SetVisibility(const int32_t visibility)
{
    visibility_ = visibility;
}

int32_t AbilityRunningRecord::GetVisibility() const
{
    return visibility_;
}

void AbilityRunningRecord::SetPerceptibility(const int32_t perceptibility)
{
    perceptibility_ = perceptibility;
}

int32_t AbilityRunningRecord::GetPerceptibility() const
{
    return perceptibility_;
}

void AbilityRunningRecord::SetConnectionState(const int32_t connectionState)
{
    connectionState_ = connectionState;
}

int32_t AbilityRunningRecord::GetConnectionState() const
{
    return connectionState_;
}

}  // namespace AppExecFwk
}  // namespace OHOS
