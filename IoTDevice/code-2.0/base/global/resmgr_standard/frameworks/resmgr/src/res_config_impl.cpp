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
#include "res_config_impl.h"
#include "locale_info_impl.h"
#include "locale_matcher.h"
#include "utils/utils.h"
namespace OHOS {
namespace Global {
namespace Resource {
ResConfigImpl::ResConfigImpl()
    : localeInfo_(nullptr),
      direction_(DIRECTION_NOT_SET),
      screenDensity_(SCREEN_DENSITY_NOT_SET),
      deviceType_(DEVICE_NOT_SET),
      isCompletedScript_(false)
{}

RState ResConfigImpl::SetLocaleInfo(const char *language,
    const char *script,
    const char *region)
{
    RState state = SUCCESS;
    if (Utils::IsStrEmpty(language)) {
        delete this->localeInfo_;
        this->localeInfo_ = nullptr;
        return state;
    }
    LocaleInfoImpl *localeInfo =
        LocaleInfoImpl::BuildFromParts(language, script, region, state);
    if (state == SUCCESS) {
        delete localeInfo_;
        localeInfo_ = localeInfo;
        this->isCompletedScript_ = false;
        if (script == nullptr || script[0] == '\0') {
            if (LocaleMatcher::Normalize(localeInfo_)) {
                this->isCompletedScript_ = true;
            } else {
                delete localeInfo_;
                localeInfo_ = nullptr;
                return NOT_ENOUGH_MEM;
            }
        }
    }

    return state;
}

void ResConfigImpl::SetDeviceType(DeviceType deviceType)
{
    this->deviceType_ = deviceType;
}

void ResConfigImpl::SetDirection(Direction direction)
{
    this->direction_ = direction;
}

void ResConfigImpl::SetScreenDensity(ScreenDensity screenDensity)
{
    this->screenDensity_ = screenDensity;
}

const LocaleInfo *ResConfigImpl::GetLocaleInfo() const
{
    return this->localeInfo_;
}

const LocaleInfoImpl *ResConfigImpl::GetLocaleInfoImpl() const
{
    return this->localeInfo_;
}

Direction ResConfigImpl::GetDirection() const
{
    return this->direction_;
}

ScreenDensity ResConfigImpl::GetScreenDensity() const
{
    return this->screenDensity_;
}

DeviceType ResConfigImpl::GetDeviceType() const
{
    return this->deviceType_;
}

bool ResConfigImpl::Copy(ResConfig &other)
{
    if (this->GetLocaleInfo() == nullptr && other.GetLocaleInfo() != nullptr) {
        LocaleInfoImpl* temp = new(std::nothrow) LocaleInfoImpl;
        if (temp == nullptr) {
            return false;
        }
        RState rs = temp->Copy(other.GetLocaleInfo());
        if (rs != SUCCESS) {
            delete temp;
            return false;
        }
        this->localeInfo_ = temp;
    }
    if (this->GetLocaleInfo() != nullptr && other.GetLocaleInfo() == nullptr) {
        delete this->localeInfo_;
        this->localeInfo_ = nullptr;
    }
    if (this->GetLocaleInfo() != nullptr && other.GetLocaleInfo() != nullptr) {
        uint64_t encodedLocale = Utils::EncodeLocale(
            this->GetLocaleInfo()->GetLanguage(),
            this->GetLocaleInfo()->GetScript(), this->GetLocaleInfo()->GetRegion());
        uint64_t otherEncodedLocale = Utils::EncodeLocale(
            other.GetLocaleInfo()->GetLanguage(),
            other.GetLocaleInfo()->GetScript(), other.GetLocaleInfo()->GetRegion());
        if (encodedLocale != otherEncodedLocale) {
            this->localeInfo_->Copy(other.GetLocaleInfo());
        }
    }
    if (this->GetDeviceType() != other.GetDeviceType()) {
        this->SetDeviceType(other.GetDeviceType());
    }
    if (this->GetDirection() != other.GetDirection()) {
        this->SetDirection(other.GetDirection());
    }
    if (this->GetScreenDensity() != other.GetScreenDensity()) {
        this->SetScreenDensity(other.GetScreenDensity());
    }
    return true;
}

bool ResConfigImpl::Match(const ResConfigImpl *other) const
{
    if (LocaleMatcher::Match(this->localeInfo_, other->GetLocaleInfoImpl()) ==
        false) {
        return false;
    }
    if (this->direction_ != DIRECTION_NOT_SET &&
        other->direction_ != DIRECTION_NOT_SET) {
        if (this->direction_ != other->direction_) {
            return false;
        }
    }
    if (this->deviceType_ != DEVICE_NOT_SET &&
        other->deviceType_ != DEVICE_NOT_SET) {
        if (this->deviceType_ != other->deviceType_) {
            return false;
        }
    }
    return true;
}

/**
 * compare this  and target
 * if this more match request,then return true
 * else
 * return false
 *
 */
bool ResConfigImpl::IsMoreSuitable(const ResConfigImpl *other,
    const ResConfigImpl *request) const
{
    if (request != nullptr) {
        int8_t result =
            LocaleMatcher::IsMoreSuitable(this->GetLocaleInfoImpl(), other->GetLocaleInfoImpl(),
                                          request->GetLocaleInfoImpl());
        if (result > 0) {
            return true;
        }
        if (result < 0) {
            return false;
        }
        /**
         * direction must full match.
         * when request is set direction and this is not equal other.
         * this or other oriention is not set.
         */
        if (this->direction_ != other->direction_ &&
            request->direction_ != Direction::DIRECTION_NOT_SET) {
            return this->direction_ != Direction::DIRECTION_NOT_SET;
        }
        if (this->deviceType_ != other->deviceType_ &&
            request->deviceType_ != DeviceType::DEVICE_NOT_SET) {
            return this->deviceType_ != DeviceType::DEVICE_NOT_SET;
        }
        if (request->screenDensity_ != ScreenDensity::SCREEN_DENSITY_NOT_SET &&
            this->screenDensity_ != other->screenDensity_) {
            int thisDistance = this->screenDensity_ - request->screenDensity_;
            int otherDistance = other->screenDensity_ - request->screenDensity_;
            if (thisDistance >= 0 && otherDistance >= 0) {
                return (thisDistance <= otherDistance);
            }
            if (thisDistance > 0) {
                return true;
            }
            if (otherDistance > 0) {
                return false;
            }
            return (thisDistance >= otherDistance);
        }
        return true;
    }
    return this->IsMoreSpecificThan(other);
}

ResConfigImpl::~ResConfigImpl()
{
    delete localeInfo_;
}

void ResConfigImpl::CompleteScript()
{
    if (isCompletedScript_) {
        return;
    }
    if (LocaleMatcher::Normalize(this->localeInfo_)) {
        isCompletedScript_ = true;
    }
}

bool ResConfigImpl::IsCompletedScript() const
{
    return isCompletedScript_;
}

bool ResConfigImpl::IsMoreSpecificThan(const ResConfigImpl *other) const
{
    if (other == nullptr) {
        return true;
    }
    int8_t result = LocaleMatcher::IsMoreSpecificThan(
        this->GetLocaleInfoImpl(),
        (other == nullptr) ? nullptr : other->GetLocaleInfoImpl());
    if (result > 0) {
        return true;
    }
    if (result < 0) {
        return false;
    }
    if (this->direction_ != other->direction_) {
        return (this->direction_ != Direction::DIRECTION_NOT_SET);
    }
    if (this->deviceType_ != other->deviceType_) {
        return (this->deviceType_ != DeviceType::DEVICE_NOT_SET);
    }
    if (this->screenDensity_ != other->screenDensity_) {
        return  (this->screenDensity_ != ScreenDensity::SCREEN_DENSITY_NOT_SET);
    }
    return true;
}

ResConfig *CreateResConfig()
{
    ResConfigImpl* temp = new(std::nothrow) ResConfigImpl;
    return temp;
}
} // namespace Resource
} // namespace Global
} // namespace OHOS