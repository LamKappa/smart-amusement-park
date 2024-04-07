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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_ELEMENT_NAME_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_ELEMENT_NAME_H

#include <string>

#include "parcel.h"

namespace OHOS {
namespace AppExecFwk {

class ElementName : public Parcelable {
    /*
     * How to locate unique Ability: deviceId/bundleName/abilityName
     */
public:
    ElementName(const std::string &deviceId, const std::string &bundleName, const std::string &abilityName);
    ElementName();
    ~ElementName();

    std::string GetURI() const;
    bool operator==(const ElementName &element) const;

    inline void SetDeviceID(const std::string &id)
    {
        deviceId_ = id;
    }

    inline std::string GetDeviceID() const
    {
        return deviceId_;
    }

    inline void SetBundleName(const std::string &name)
    {
        bundleName_ = name;
    }

    inline std::string GetBundleName() const
    {
        return bundleName_;
    }

    inline void SetAbilityName(const std::string &name)
    {
        abilityName_ = name;
    }

    inline std::string GetAbilityName() const
    {
        return abilityName_;
    }

    bool ReadFromParcel(Parcel &parcel);
    virtual bool Marshalling(Parcel &parcel) const override;
    static ElementName *Unmarshalling(Parcel &parcel);

    void SetElementDeviceID(ElementName *element, const char *deviceId);
    void SetElementBundleName(ElementName *element, const char *bundleName);
    void SetElementAbilityName(ElementName *element, const char *abilityName);
    void ClearElement(ElementName *element);

private:
    std::string deviceId_;
    std::string bundleName_;
    std::string abilityName_;
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_ELEMENT_NAME_H
