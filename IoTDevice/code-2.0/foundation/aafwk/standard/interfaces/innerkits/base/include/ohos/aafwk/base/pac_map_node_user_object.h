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

#ifndef OHOS_AppExecFwk_PAC_MAP_NODE_USER_OBJECT_H
#define OHOS_AppExecFwk_PAC_MAP_NODE_USER_OBJECT_H

#include <memory>
#include <functional>
#include <unordered_map>
#include "parcel.h"
#include "pac_map_node.h"

namespace OHOS {
namespace AppExecFwk {
/**
 * Tusermapobject is used as the base class for object deserialization
 */
class TUserMapObject : public Parcelable {
public:
    TUserMapObject(const std::string &className) : class_name_(className)
    {}
    virtual ~TUserMapObject() = default;

    inline void SetClassName(const std::string &className)
    {
        class_name_ = className;
    }
    inline std::string GetClassName(void)
    {
        return class_name_;
    }

    /**
     * @brief Indicates whether some other object is "equal to" this one.
     * @param other The object with which to compare.
     * @return true if this object is the same as the obj argument; false otherwise.
     */
    virtual bool Equals(const TUserMapObject *other) = 0;

    /**
     * @brief Copy the data of the specified object to the current object with deepcopy
     */
    virtual void DeepCopy(const TUserMapObject *other) = 0;

    /**
     * @brief Unmarshals this Sequenceable object from a Parcel.
     *
     * @return Unmarshals success returns true, otherwise returns false.
     */
    virtual bool Unmarshalling(Parcel &parcel) = 0;

protected:
    std::string class_name_;
};

using CreateUserMapObject = std::function<TUserMapObject *(void)>;

class PacMapUserObjectLoader {
public:
    static PacMapUserObjectLoader &GetInstance(void);
    ~PacMapUserObjectLoader() = default;
    /**
     * @brief Registered user-defined serialization class.
     * @param objectName The name of the custom class.
     * @param createFun Function object that creates an instance of a custom class.
     */
    void RegisterUserObject(const std::string &objectName, const CreateUserMapObject &createFun);

    /**
     * @brief Represents obtaining an instance of an object of a registered serialization class.
     * @param className The name of the custom class.
     *
     * @return Returns an instance of the object, or nullptr on failure.
     */
    TUserMapObject *GetUserObjectByName(const std::string &className);

private:
    std::unordered_map<std::string, CreateUserMapObject> register_class_list_;
    PacMapUserObjectLoader() = default;
    PacMapUserObjectLoader(const PacMapUserObjectLoader &) = delete;
    PacMapUserObjectLoader &operator=(const PacMapUserObjectLoader &) = delete;
    PacMapUserObjectLoader(PacMapUserObjectLoader &&) = delete;
    PacMapUserObjectLoader &operator=(PacMapUserObjectLoader &&) = delete;
};

/**
 * @brief Register the object's deserialization class, which must be a child TUserMapObject.
 *
 * @param className The name of class.
 */
#define REGISTER_USER_MAP_OBJECT(className)                                                 \
    static __attribute__((constructor)) void RegisterUserMapObject_##className()            \
    {                                                                                       \
        PacMapUserObjectLoader::GetInstance().RegisterUserObject(                           \
            #className, []() -> TUserMapObject * { return new (std::nothrow) className; }); \
    }

class PacMapNodeTypeObject : public PacMapNode {
public:
    PacMapNodeTypeObject() : PacMapNode(DT_PACMAP_OBJECT), object_value_(nullptr)
    {}
    virtual ~PacMapNodeTypeObject() = default;

    PacMapNodeTypeObject(const PacMapNodeTypeObject &other);
    PacMapNodeTypeObject &operator=(const PacMapNodeTypeObject &other);

    /**
     * @brief Adds a object to current object. The object must be a subclass of TUserMapObject
     * @param object Added object. A smart pointer to a subclass of TUserMapObject.
     */
    void PutObject(const std::shared_ptr<TUserMapObject> &object);

    /**
     * @brief Obtains the object value.
     * @return Returns the smart pointer to subclass of TUserMapObject.
     */
    std::shared_ptr<TUserMapObject> GetObject(void);

    /**
     * @brief Indicates whether some other object is "equal to" this one.
     * @param other The object with which to compare.
     * @return true if this object is the same as the obj argument; false otherwise.
     */
    virtual bool Equals(const PacMapNode *other) override;

    /**
     * @brief Copy the data of the specified object to the current object with deepcopy
     * @param other The original object that stores the data.
     */
    virtual void DeepCopy(const PacMapNode *other) override;

    /**
     * @brief Marshals this Sequenceable object to a Parcel.
     * @param key Indicates the key in String format.
     * @param parcel Indicates the Parcel object into which the Sequenceable object has been marshaled.
     * @return Marshals success returns true, otherwise returns false.
     */
    virtual bool Marshalling(const std::string &key, Parcel &parcel) const override;

    /**
     * @brief Unmarshals this Sequenceable object from a Parcel.
     * @param dataType Indicates the type of data stored.
     * @param parcel Indicates the Parcel object into which the Sequenceable object has been marshaled.
     * @return Unmarshals success returns true, otherwise returns false.
     */
    virtual bool Unmarshalling(int32_t dataType, Parcel &parcel) override;

private:
    std::shared_ptr<TUserMapObject> object_value_;
    void InnerDeepCopy(const PacMapNodeTypeObject *other);
};
}  // namespace AppExecFwk
}  // namespace OHOS

#endif
