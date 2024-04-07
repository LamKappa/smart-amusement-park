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

#ifndef OHOS_AAFWK_OPERATION_H
#define OHOS_AAFWK_OPERATION_H

#include <string>
#include "uri.h"
#include "parcel.h"
#include "string_ex.h"

namespace OHOS {
namespace AAFwk {

class OperationBuilder;

class Operation : public Parcelable {
    friend class OperationBuilder;
    friend class Want;

public:
    Operation();
    ~Operation();
    Operation(const Operation &other);
    /**
     * @description: Obtains the value of the abilityName attribute included in this Operation.
     * @return Returns the ability name included in this Operation.
     */
    std::string GetAbilityName() const;

    /**
     * @description: Obtains the value of the action attribute included in this Operation.
     * @return Returns the action included in this Operation.
     */
    std::string GetAction() const;

    /**
     * @description: Obtains the value of the bundleName attribute included in this Operation.
     * @return Returns the bundle name included in this Operation.
     */
    std::string GetBundleName() const;

    /**
     * @description: Obtains the value of the deviceId attribute included in this Operation.
     * @return Returns the device ID included in this Operation.
     */
    std::string GetDeviceId() const;

    /**
     * @description: Obtains the value of the entities attribute included in this Operation.
     * @return Returns the entities included in this Operation.
     */
    const std::vector<std::string> &GetEntities() const;

    /**
     * @description: Obtains the value of the flags attribute included in this Operation.
     * @return Returns the flags included in this Operation.
     */
    unsigned int GetFlags() const;

    /**
     * @description: Obtains the value of the uri attribute included in this Operation.
     * @return Returns the URI included in this Operation.
     */
    OHOS::Uri GetUri() const;

    bool operator==(const Operation &other) const;
    Operation &operator=(const Operation &other);

    bool Marshalling(Parcel &parcel) const;
    static Operation *Unmarshalling(Parcel &parcel);

private:
    /**
     * @description: Sets a flag in a Want.
     * @param flags Indicates the flag to set.
     * @return Returns this Want object containing the flag.
     */
    void SetFlags(unsigned int flags);
    /**
     * @description: Adds a flag to a Want.
     * @param flags Indicates the flag to add.
     * @return Returns the Want object with the added flag.
     */
    void AddFlags(unsigned int flags);
    /**
     * @description: Removes the description of a flag from a Want.
     * @param flags Indicates the flag to remove.
     * @return Removes the description of a flag from a Want.
     */
    void RemoveFlags(unsigned int flags);

    /**
     * @description: Adds the description of an entity to a Want
     * @param entity Indicates the entity description to add
     * @return Returns this Want object containing the entity.
     */
    void AddEntity(const std::string &entity);

    /**
     * @description: Removes the description of an entity from a Want
     * @param entity Indicates the entity description to remove.
     * @return void
     */
    void RemoveEntity(const std::string &entity);

    /**
     * @description: Checks whether a Want contains the given entity
     * @param entity Indicates the entity to check
     * @return Returns true if the given entity is contained; returns false otherwise
     */
    bool HasEntity(const std::string &entity) const;

    /**
     * @description: Obtains the number of entities in a Want
     * @return Returns the entity quantity
     */
    int CountEntities() const;

    /**
     * @description: Sets a bundle name in this Want.
     * If a bundle name is specified in a Want, the Want will match only
     * the abilities in the specified bundle. You cannot use this method and
     * setPicker(ohos.aafwk.content.Want) on the same Want.
     * @param bundleName Indicates the bundle name to set.
     * @return Returns a Want object containing the specified bundle name.
     */
    void SetBundleName(const std::string &bundleName);

    /**
     * @description: Sets a uri in this operation.
     * @param uri Indicates uri object to set.
     * @return -
     */
    void SetUri(const Uri &uri);

    /**
     * @description: Gets a uri in this operation.
     * @param uri Indicates uri object to set.
     * @return Returns a uri in this operation.
     */
    Uri &GetUri(const Uri &uri);

    /**
     * @description: Sets the value of the abilityName attribute included in this Operation.
     * @return Returns the ability name included in this Operation.
     */
    void SetAbilityName(const std::string &abilityname);

    /**
     * @description: Sets the value of the deviceId attribute included in this Operation.
     * @param deviceid Indicates deviceid object to set.
     * @return -
     */
    void SetDeviceId(const std::string &deviceid);

    /**
     * @description: Sets the value of the action attribute included in this Operation.
     * @param deviceid Indicates deviceid object to set.
     * @return -
     */
    void SetAction(const std::string &action);

    /**
     * @description: Sets the entities of this Operation.
     * @param entities Indicates entities to set.
     * @return -
     */
    void SetEntities(const std::vector<std::string> &entities);

private:
    std::string abilityName_;
    std::string action_;
    std::string bundleName_;
    std::string deviceId_;
    std::vector<std::string> entities_;
    unsigned int flags_;
    Uri uri_;
    friend class OperationBuilder;

    // no object in parcel
    static constexpr int VALUE_NULL = -1;
    // object exist in parcel
    static constexpr int VALUE_OBJECT = 1;

    bool ReadFromParcel(Parcel &parcel);
};
}  // namespace AAFwk
}  // namespace OHOS

#endif  // OHOS_AAFWK_WANT_PARAMS_H