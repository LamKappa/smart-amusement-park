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

#include "ohos/aafwk/base/pac_map_node_user_object.h"
#include <iostream>
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
PacMapUserObjectLoader &PacMapUserObjectLoader::GetInstance(void)
{
    static PacMapUserObjectLoader gPacMapUserObjectLoader;
    return gPacMapUserObjectLoader;
}

/**
 * @brief Registered user-defined serialization class.
 * @param objectName The name of the custom class.
 * @param createFun Function object that creates an instance of a custom class.
 */
void PacMapUserObjectLoader::RegisterUserObject(const std::string &objectName, const CreateUserMapObject &createFun)
{
    register_class_list_.emplace(objectName, createFun);
}

/**
 * @brief Represents obtaining an instance of an object of a registered serialization class.
 * @param className The name of the custom class.
 *
 * @return Returns an instance of the object, or nullptr on failure.
 */
TUserMapObject *PacMapUserObjectLoader::GetUserObjectByName(const std::string &className)
{
    auto iter = register_class_list_.find(className);
    if (iter != register_class_list_.end()) {
        return iter->second();
    } else {
        return nullptr;
    }
}

PacMapNodeTypeObject::PacMapNodeTypeObject(const PacMapNodeTypeObject &other) : PacMapNode(other)
{
    InnerDeepCopy(&other);
}

PacMapNodeTypeObject &PacMapNodeTypeObject::operator=(const PacMapNodeTypeObject &other)
{
    if (this != &other) {
        object_value_ = other.object_value_;
    }
    return *this;
}

/**
 * @brief Adds a object to current object. The object must be a subclass of TUserMapObject
 * @param value Added object. A smart pointer to a subclass of TUserMapObject.
 */
void PacMapNodeTypeObject::PutObject(const std::shared_ptr<TUserMapObject> &object)
{
    object_value_ = object;
}

/**
 * @brief Obtains the object value.
 * @return Returns the smart pointer to subclass of TUserMapObject.
 */
std::shared_ptr<TUserMapObject> PacMapNodeTypeObject::GetObject(void)
{
    return object_value_;
}

/**
 * @brief Indicates whether some other object is "equal to" this one.
 * @param other The object with which to compare.
 * @return true if this object is the same as the obj argument; false otherwise.
 */
bool PacMapNodeTypeObject::Equals(const PacMapNode *other)
{
    if (other == nullptr) {
        return false;
    }

    PacMapNode *pnode = const_cast<PacMapNode *>(other);
    PacMapNodeTypeObject *other_ = static_cast<PacMapNodeTypeObject *>(pnode);
    if (other_ == nullptr) {
        return false;
    }

    if (object_value_.get() == other_->object_value_.get()) {
        return true;
    }

    if (object_value_.get() == nullptr || other_->object_value_.get() == nullptr) {
        return false;
    }

    return object_value_->Equals(other_->object_value_.get());
}

/**
 * @brief Copy the data of the specified object to the current object with deepcopy
 * @param other The original object that stores the data.
 */
void PacMapNodeTypeObject::DeepCopy(const PacMapNode *other)
{
    PacMapNodeTypeObject *object_object = (PacMapNodeTypeObject *)other;
    if (object_object != nullptr) {
        InnerDeepCopy(object_object);
    }
}

void PacMapNodeTypeObject::InnerDeepCopy(const PacMapNodeTypeObject *other)
{
    if (other == nullptr || other->object_value_ == nullptr) {
        return;
    }

    TUserMapObject *userObject =
        PacMapUserObjectLoader::GetInstance().GetUserObjectByName(other->object_value_.get()->GetClassName());
    if (userObject == nullptr) {
        return;
    }

    std::shared_ptr<TUserMapObject> user_object(userObject);
    user_object->DeepCopy(other->object_value_.get());
    object_value_ = user_object;
}

/**
 * @brief Marshals this Sequenceable object to a Parcel.
 * @param key Indicates the key in String format.
 * @param parcel Indicates the Parcel object into which the Sequenceable object has been marshaled.
 * @return Marshals success returns true, otherwise returns false.
 */
bool PacMapNodeTypeObject::Marshalling(const std::string &key, Parcel &parcel) const
{
    if (object_value_ == nullptr) {
        return false;
    }

    if (!parcel.WriteString16(Str8ToStr16(key))) {
        return false;
    }
    if (!parcel.WriteInt32(PACMAP_DATA_OBJECT)) {
        return false;
    }
    if (!parcel.WriteString16(Str8ToStr16(object_value_->GetClassName()))) {
        return false;
    }

    return object_value_->Marshalling(parcel);
}

/**
 * @brief Unmarshals this Sequenceable object from a Parcel.
 * @param dataType Indicates the type of data stored.
 * @param parcel Indicates the Parcel object into which the Sequenceable object has been marshaled.
 * @return Unmarshals success returns true, otherwise returns false.
 */
bool PacMapNodeTypeObject::Unmarshalling(int32_t dataType, Parcel &parcel)
{
    std::u16string u16ClassName;
    std::string className;
    // obtains name of class
    if (!parcel.ReadString16(u16ClassName)) {
        return false;
    }
    className = Str16ToStr8(u16ClassName);

    std::shared_ptr<TUserMapObject> user_object(PacMapUserObjectLoader::GetInstance().GetUserObjectByName(className));
    if (user_object == nullptr) {
        return false;
    }

    if (user_object->Unmarshalling(parcel)) {
        object_value_ = user_object;
        return true;
    }
    return false;
}
}  // namespace AppExecFwk
}  // namespace OHOS
