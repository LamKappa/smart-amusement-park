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

#include "ohos/aafwk/content/intent_filter.h"

#include "string_ex.h"
#include "ohos/aafwk/content/intent.h"

namespace OHOS {
namespace AAFwk {

IntentFilter::IntentFilter()
{}

std::string IntentFilter::GetEntity() const
{
    return entity_;
}

void IntentFilter::SetEntity(const std::string &entity)
{
    entity_ = entity;
}

void IntentFilter::AddAction(const std::string &action)
{
    auto it = std::find(actions_.cbegin(), actions_.cend(), action);
    if (it == actions_.cend()) {
        actions_.push_back(action);
    }
}

int IntentFilter::CountAction() const
{
    return actions_.size();
}

std::string IntentFilter::GetAction(int index) const
{
    std::string action;
    if (index < static_cast<int>(actions_.size())) {
        action = actions_[index];
    }
    return action;
}

void IntentFilter::RemoveAction(const std::string &action)
{
    auto it = std::find(actions_.cbegin(), actions_.cend(), action);
    if (it != actions_.cend()) {
        actions_.erase(it);
    }
}

bool IntentFilter::HasAction(const std::string &action) const
{
    return std::find(actions_.cbegin(), actions_.cend(), action) != actions_.cend();
}

bool IntentFilter::Marshalling(Parcel &parcel) const
{
    // write entity
    if (!parcel.WriteString16(Str8ToStr16(entity_))) {
        return false;
    }

    // write actions
    std::vector<std::u16string> actionU16;
    for (std::vector<std::string>::size_type i = 0; i < actions_.size(); i++) {
        actionU16.push_back(Str8ToStr16(actions_[i]));
    }

    if (!parcel.WriteString16Vector(actionU16)) {
        return false;
    }

    return true;
}

bool IntentFilter::ReadFromParcel(Parcel &parcel)
{
    // read entity
    entity_ = Str16ToStr8(parcel.ReadString16());

    // read actions
    std::vector<std::u16string> actionU16;
    if (!parcel.ReadString16Vector(&actionU16)) {
        return false;
    }

    actions_.clear();
    for (std::vector<std::u16string>::size_type i = 0; i < actionU16.size(); i++) {
        actions_.push_back(Str16ToStr8(actionU16[i]));
    }

    return true;
}

IntentFilter *IntentFilter::Unmarshalling(Parcel &parcel)
{
    IntentFilter *filter = new (std::nothrow) IntentFilter();
    if (filter && !filter->ReadFromParcel(parcel)) {
        delete filter;
        filter = nullptr;
    }
    return filter;
}

bool IntentFilter::MatchAction(const std::string &action) const
{
    return HasAction(action);
}

bool IntentFilter::MatchEntity(const std::string &entity) const
{
    return entity_ == entity;
}

bool IntentFilter::Match(const Intent &intent) const
{
    return MatchAction(intent.GetAction()) && MatchEntity(intent.GetEntity());
}

}  // namespace AAFwk
}  // namespace OHOS
