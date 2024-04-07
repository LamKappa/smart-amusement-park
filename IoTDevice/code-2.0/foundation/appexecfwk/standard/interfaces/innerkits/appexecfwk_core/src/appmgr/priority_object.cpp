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

#include "priority_object.h"

#include "app_log_wrapper.h"

namespace OHOS {

namespace AppExecFwk {

pid_t PriorityObject::GetPid() const
{
    return pid_;
}

int32_t PriorityObject::GetMaxAdj() const
{
    return maxAdj_;
}

int32_t PriorityObject::GetCurAdj() const
{
    return curAdj_;
}

int32_t PriorityObject::GetCurCgroup() const
{
    return curCgroup_;
}

int32_t PriorityObject::GetTimeLevel() const
{
    return timeLevel_;
}

bool PriorityObject::GetVisibleStatus() const
{
    return visibleStatus_;
}

bool PriorityObject::GetPerceptibleStatus() const
{
    return perceptibleStatus_;
}

void PriorityObject::SetPid(const pid_t pid)
{
    pid_ = pid;
}

void PriorityObject::SetMaxAdj(const int32_t maxAdj)
{
    maxAdj_ = maxAdj;
}

void PriorityObject::SetCurAdj(const int32_t curAdj)
{
    curAdj_ = curAdj;
}

void PriorityObject::SetCurCgroup(const int32_t curCgroup)
{
    curCgroup_ = curCgroup;
}

void PriorityObject::SetTimeLevel(const int32_t timeLevel)
{
    timeLevel_ = timeLevel;
}

void PriorityObject::SetVisibleStatus(bool status)
{
    visibleStatus_ = status;
}

void PriorityObject::SetPerceptibleStatus(bool status)
{
    perceptibleStatus_ = status;
}

bool PriorityObject::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteInt32(pid_)) {
        return false;
    }
    if (!parcel.WriteInt32(maxAdj_)) {
        return false;
    }
    if (!parcel.WriteInt32(curAdj_)) {
        return false;
    }
    if (!parcel.WriteInt32(curCgroup_)) {
        return false;
    }
    if (!parcel.WriteInt32(timeLevel_)) {
        return false;
    }
    return true;
}

bool PriorityObject::ReadFromParcel(Parcel &parcel)
{
    if (!parcel.ReadInt32(pid_)) {
        return false;
    }
    if (!parcel.ReadInt32(maxAdj_)) {
        return false;
    }
    if (!parcel.ReadInt32(curAdj_)) {
        return false;
    }
    if (!parcel.ReadInt32(curCgroup_)) {
        return false;
    }
    if (!parcel.ReadInt32(timeLevel_)) {
        return false;
    }
    return true;
}

PriorityObject *PriorityObject::Unmarshalling(Parcel &parcel)
{
    PriorityObject *priorityObject = new (std::nothrow) PriorityObject();
    if (priorityObject && !priorityObject->ReadFromParcel(parcel)) {
        APP_LOGW("failed, because ReadFromParcel failed");
        delete priorityObject;
        priorityObject = nullptr;
    }
    return priorityObject;
}

}  // namespace AppExecFwk
}  // namespace OHOS