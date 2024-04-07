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

#include "element_name.h"

#include "string_ex.h"

#include "app_log_wrapper.h"
#include "parcel_macro.h"

namespace OHOS {
namespace AppExecFwk {

void ElementName::SetElementDeviceID(ElementName *element, const char *deviceId)
{
    if (element == nullptr) {
        return;
    }
    element->SetDeviceID(deviceId);
}

void ElementName::SetElementBundleName(ElementName *element, const char *bundleName)
{
    if (element == nullptr) {
        return;
    }
    element->SetBundleName(bundleName);
}

void ElementName::SetElementAbilityName(ElementName *element, const char *abilityName)
{
    if (element == nullptr) {
        return;
    }
    element->SetAbilityName(abilityName);
}

void ElementName::ClearElement(ElementName *element)
{
    if (element == nullptr) {
        return;
    }
    element->SetDeviceID("");
    element->SetBundleName("");
    element->SetAbilityName("");
}

ElementName::ElementName(const std::string &deviceId, const std::string &bundleName, const std::string &abilityName)
    : deviceId_(deviceId), bundleName_(bundleName), abilityName_(abilityName)
{
    APP_LOGD("instance is created with parameters");
}

ElementName::ElementName()
{
    APP_LOGD("instance is created without parameter");
}

ElementName::~ElementName()
{
    APP_LOGD("instance is destroyed");
}

std::string ElementName::GetURI() const
{
    return deviceId_ + "/" + bundleName_ + "/" + abilityName_;
}

bool ElementName::operator==(const ElementName &element) const
{
    return (deviceId_ == element.GetDeviceID() && bundleName_ == element.GetBundleName() &&
            abilityName_ == element.GetAbilityName());
}

bool ElementName::ReadFromParcel(Parcel &parcel)
{
    bundleName_ = Str16ToStr8(parcel.ReadString16());
    abilityName_ = Str16ToStr8(parcel.ReadString16());
    deviceId_ = Str16ToStr8(parcel.ReadString16());
    return true;
}

ElementName *ElementName::Unmarshalling(Parcel &parcel)
{
    ElementName *elementName = new (std::nothrow) ElementName();
    if (elementName && !elementName->ReadFromParcel(parcel)) {
        APP_LOGW("read from parcel failed");
        delete elementName;
        elementName = nullptr;
    }
    return elementName;
}

bool ElementName::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(bundleName_));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(abilityName_));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(deviceId_));
    return true;
}

}  // namespace AppExecFwk
}  // namespace OHOS
