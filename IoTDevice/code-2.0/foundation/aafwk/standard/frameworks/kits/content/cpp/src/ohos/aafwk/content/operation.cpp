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

#include "operation.h"
#include "operation_builder.h"
#include "parcel_macro.h"
using namespace OHOS::AppExecFwk;
namespace OHOS {
namespace AAFwk {
Operation::Operation() : flags_(0), uri_("")
{
    entities_.clear();
}

Operation::Operation(const Operation &other) : flags_(0), uri_(other.uri_.ToString())
{
    flags_ = other.flags_;
    action_ = other.action_;
    deviceId_ = other.deviceId_;
    entities_ = other.entities_;
    bundleName_ = other.bundleName_;
    abilityName_ = other.abilityName_;
    entities_.clear();
}

Operation::~Operation()
{}

/**
 * @description: Obtains the value of the abilityName attribute included in this Operation.
 * @return Returns the ability name included in this Operation.
 */
std::string Operation::GetAbilityName() const
{
    return abilityName_;
}

/**
 * @description: Obtains the value of the action attribute included in this Operation.
 * @return Returns the action included in this Operation.
 */
std::string Operation::GetAction() const
{
    return action_;
}
/**
 * @description: Sets a bundle name in this Want.
 * If a bundle name is specified in a Want, the Want will match only
 * the abilities in the specified bundle. You cannot use this method and
 * setPicker(ohos.aafwk.content.Want) on the same Want.
 * @param bundleName Indicates the bundle name to set.
 * @return Returns a Want object containing the specified bundle name.
 */
void Operation::SetBundleName(const std::string &bundleName)
{
    bundleName_ = bundleName;
}
/**
 * @description: Obtains the value of the bundleName attribute included in this Operation.
 * @return Returns the bundle name included in this Operation.
 */
std::string Operation::GetBundleName() const
{
    return bundleName_;
}

/**
 * @description: Obtains the value of the deviceId attribute included in this Operation.
 * @return Returns the device ID included in this Operation.
 */
std::string Operation::GetDeviceId() const
{
    return deviceId_;
}

/**
 * @description: Obtains the value of the entities attribute included in this Operation.
 * @return Returns the entities included in this Operation.
 */
const std::vector<std::string> &Operation::GetEntities() const
{
    return entities_;
}
/**
 * @description: Adds the description of an entity to a Want
 * @param entity Indicates the entity description to add
 * @return Returns this Want object containing the entity.
 */
void Operation::AddEntity(const std::string &entity)
{
    if (!HasEntity(entity)) {
        entities_.emplace_back(entity);
    }
}

/**
 * @description: Removes the description of an entity from a Want
 * @param entity Indicates the entity description to remove.
 * @return void
 */
void Operation::RemoveEntity(const std::string &entity)
{
    if (!entities_.empty()) {
        auto it = std::find(entities_.begin(), entities_.end(), entity);
        if (it != entities_.end()) {
            entities_.erase(it);
        }
    }
}

/**
 * @description: Checks whether a Want contains the given entity
 * @param entity Indicates the entity to check
 * @return Returns true if the given entity is contained; returns false otherwise
 */
bool Operation::HasEntity(const std::string &entity) const
{
    return std::find(entities_.begin(), entities_.end(), entity) != entities_.end();
}

/**
 * @description: Obtains the number of entities in a Want
 * @return Returns the entity quantity
 */
int Operation::CountEntities() const
{
    return entities_.size();
}
/**
 * @description: Obtains the value of the flags attribute included in this Operation.
 * @return Returns the flags included in this Operation.
 */
unsigned int Operation::GetFlags() const
{
    return flags_;
}

/**
 * @description: Sets a flag in a Want.
 * @param flags Indicates the flag to set.
 * @return Returns this Want object containing the flag.
 */
void Operation::SetFlags(unsigned int flags)
{
    flags_ = flags;
}
/**
 * @description: Adds a flag to a Want.
 * @param flags Indicates the flag to add.
 * @return Returns the Want object with the added flag.
 */
void Operation::AddFlags(unsigned int flags)
{
    flags_ |= flags;
}

/**
 * @description: Removes the description of a flag from a Want.
 * @param flags Indicates the flag to remove.
 * @return Removes the description of a flag from a Want.
 */
void Operation::RemoveFlags(unsigned int flags)
{
    flags_ &= ~flags;
}

/**
 * @description: Obtains the value of the uri attribute included in this Operation.
 * @return Returns the URI included in this Operation.
 */
Uri Operation::GetUri() const
{
    return uri_;
}

bool Operation::operator==(const Operation &other) const
{
    if (abilityName_ != other.abilityName_) {
        return false;
    }
    if (action_ != other.action_) {
        return false;
    }
    if (bundleName_ != other.bundleName_) {
        return false;
    }
    if (deviceId_ != other.deviceId_) {
        return false;
    }
    long entitiesCount = entities_.size();
    long otherEntitiesCount = other.entities_.size();
    if (entitiesCount != otherEntitiesCount) {
        return false;
    } else {
        for (long i = 0; i < entitiesCount; i++) {
            if (entities_[i] != other.entities_[i]) {
                return false;
            }
        }
    }
    if (flags_ != other.flags_) {
        return false;
    }
    if (uri_.ToString() != other.uri_.ToString()) {
        return false;
    }

    return true;
}

Operation &Operation::operator=(const Operation &other)
{
    uri_ = other.uri_;
    flags_ = other.flags_;
    action_ = other.action_;
    deviceId_ = other.deviceId_;
    entities_ = other.entities_;
    bundleName_ = other.bundleName_;
    abilityName_ = other.abilityName_;
    return *this;
}

bool Operation::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(abilityName_));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(action_));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(bundleName_));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(deviceId_));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(StringVector, parcel, entities_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, flags_);

    Uri uri("");
    if (uri_ == uri) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, VALUE_NULL);
    } else {
        if (!parcel.WriteInt32(VALUE_OBJECT)) {
            return false;
        }
        if (!parcel.WriteParcelable(&uri_)) {
            return false;
        }
    }

    return true;
}

Operation *Operation::Unmarshalling(Parcel &parcel)
{
    Operation *operation = new (std::nothrow) Operation();
    if (operation != nullptr && !operation->ReadFromParcel(parcel)) {
        delete operation;
        operation = nullptr;
    }

    return operation;
}

bool Operation::ReadFromParcel(Parcel &parcel)
{
    std::u16string readString16;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, readString16);
    abilityName_ = Str16ToStr8(readString16);
    readString16.clear();

    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, readString16);
    action_ = Str16ToStr8(readString16);
    readString16.clear();

    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, readString16);
    bundleName_ = Str16ToStr8(readString16);
    readString16.clear();

    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, readString16);
    deviceId_ = Str16ToStr8(readString16);
    readString16.clear();

    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(StringVector, parcel, &entities_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, flags_);

    // uri_
    int32_t empty = VALUE_NULL;
    if (!parcel.ReadInt32(empty)) {
        return false;
    }

    if (empty == VALUE_OBJECT) {
        auto uri = parcel.ReadParcelable<Uri>();
        if (uri != nullptr) {
            uri_ = *uri;
            delete uri;
            uri = nullptr;
        } else {
            return false;
        }
    }

    return true;
}

/**
 * @description: Sets a uri in this operation.
 * @param uri Indicates uri object to set.
 * @return -
 */
void Operation::SetUri(const Uri &uri)
{
    uri_ = uri;
}

/**
 * @description: Returns a uri in this operation.
 * @param uri Indicates uri object to set.
 * @return Returns a uri in this operation.
 */
Uri &Operation::GetUri(const Uri &uri)
{
    return uri_;
}

/**
 * @description: Sets the value of the abilityName attribute included in this Operation.
 * @param abilityname
 * @return -
 */
void Operation::SetAbilityName(const std::string &abilityname)
{
    abilityName_ = abilityname;
}
/**
 * @description: Sets the value of the deviceId attribute included in this Operation.
 * @param  deviceid
 * @return -
 */
void Operation::SetDeviceId(const std::string &deviceid)
{
    deviceId_ = deviceid;
}

/**
 * @description: Sets the value of the action attribute included in this Operation.
 * @param deviceid Indicates deviceid object to set.
 * @return -
 */
void Operation::SetAction(const std::string &action)
{
    action_ = action;
}

/**
 * @description: Sets the entities of this Operation.
 * @param entities Indicates entities to set.
 * @return -
 */
void Operation::SetEntities(const std::vector<std::string> &entities)
{
    entities_.clear();
    entities_ = entities;
}
}  // namespace AAFwk
}  // namespace OHOS