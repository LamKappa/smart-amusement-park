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

#include "pac_map.h"
#include "parcel_macro.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
#define PAC_MAP_ADD_BASE(id, key, value, mapList)                                       \
    RemoveDataNode(key);                                                                \
    std::shared_ptr<PacMapNodeTypeBase> pnode = std::make_shared<PacMapNodeTypeBase>(); \
    pnode->Put##id##Value(value);                                                       \
    mapList.emplace(key, pnode);

#define PAC_MAP_ADD_ARRAY(id, key, value, mapList)                                        \
    RemoveDataNode(key);                                                                  \
    std::shared_ptr<PacMapNodeTypeArray> pnode = std::make_shared<PacMapNodeTypeArray>(); \
    pnode->Put##id##ValueArray(value);                                                    \
    mapList.emplace(key, pnode);

#define GET_PAC_MAP_BASE(id, mapList, key, defaultValue)                                 \
    auto it = mapList.find(key);                                                         \
    if (it != mapList.end()) {                                                           \
        PacMapNodeTypeBase *pBase = static_cast<PacMapNodeTypeBase *>(it->second.get()); \
        if (pBase != nullptr) {                                                          \
            return pBase->Get##id##Value(defaultValue);                                  \
        }                                                                                \
    }                                                                                    \
    return defaultValue;

#define GET_PAC_MAP_ARRAY(id, mapList, key, value)                                          \
    auto it = mapList.find(key);                                                            \
    if (it != mapList.end()) {                                                              \
        PacMapNodeTypeArray *pArray = static_cast<PacMapNodeTypeArray *>(it->second.get()); \
        if (pArray != nullptr) {                                                            \
            pArray->Get##id##ValueArray(value);                                             \
        }                                                                                   \
    }

/**
 * @brief A replication structure with deep copy.
 */
PacMap::PacMap(const PacMap &other)
{
    data_list_.clear();
    DeepCopyData(data_list_, other.data_list_);
}

PacMap::~PacMap()
{
    Clear();
}

/**
 * @brief A overload operation with shallow copy.
 */
PacMap &PacMap::operator=(const PacMap &other)
{
    if (&other != this) {
        ShallowCopyData(data_list_, other.data_list_);
    }
    return *this;
}

/**
 * @brief Clear all key-value pairs and free resources.
 */
void PacMap::Clear(void)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    data_list_.clear();
}

/**
 * @brief Creates and returns a copy of this object with shallow copy.
 *
 * @return A clone of this instance.
 */
PacMap PacMap::Clone(void)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    PacMap pac_map;
    ShallowCopyData(pac_map.data_list_, data_list_);
    return pac_map;
}

/**
 * @brief Creates a deep copy of this PacMap object with deep copy.
 *
 * @return
 */
PacMap PacMap::DeepCopy(void)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    PacMap pac_map;
    DeepCopyData(pac_map.data_list_, data_list_);
    return pac_map;
}

/**
 * @brief Adds a short value matching a specified key.
 * @param key A specified key.
 * @param value The value that matches the specified key.
 */
void PacMap::PutShortValue(const std::string &key, short value)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    PAC_MAP_ADD_BASE(Short, key, value, data_list_);
}

/**
 * @brief Adds a integer value matching a specified key.
 * @param key A specified key.
 * @param value The value that matches the specified key.
 */
void PacMap::PutIntValue(const std::string &key, int value)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    PAC_MAP_ADD_BASE(Int, key, value, data_list_);
}

/**
 * @brief Adds a long value matching a specified key.
 * @param key A specified key.
 * @param value The value that matches the specified key.
 */
void PacMap::PutLongValue(const std::string &key, long value)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    PAC_MAP_ADD_BASE(Long, key, value, data_list_);
}

/**
 * @brief Adds a boolean value matching a specified key.
 * @param key A specified key.
 * @param value The value that matches the specified key.
 */
void PacMap::PutBooleanValue(const std::string &key, bool value)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    PAC_MAP_ADD_BASE(Boolean, key, value, data_list_);
}

/**
 * @brief Adds a char value matching a specified key.
 * @param key A specified key.
 * @param value The value that matches the specified key.
 */
void PacMap::PutCharValue(const std::string &key, char value)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    PAC_MAP_ADD_BASE(Char, key, value, data_list_);
}

/**
 * @brief Adds a byte value matching a specified key.
 * @param key A specified key.
 * @param value The value that matches the specified key.
 */
void PacMap::PutByteValue(const std::string &key, AAFwk::byte value)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    PAC_MAP_ADD_BASE(Byte, key, value, data_list_);
}

/**
 * @brief Adds a float value matching a specified key.
 * @param key A specified key.
 * @param value The value that matches the specified key.
 */
void PacMap::PutFloatValue(const std::string &key, float value)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    PAC_MAP_ADD_BASE(Float, key, value, data_list_);
}

/**
 * @brief Adds a double value matching a specified key.
 * @param key A specified key.
 * @param value The value that matches the specified key.
 */
void PacMap::PutDoubleValue(const std::string &key, double value)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    PAC_MAP_ADD_BASE(Double, key, value, data_list_);
}

/**
 * @brief Adds a string {std::string} value matching a specified key.
 * @param key A specified key.
 * @param value The value that matches the specified key.
 */
void PacMap::PutStringValue(const std::string &key, const std::string &value)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    PAC_MAP_ADD_BASE(String, key, value, data_list_);
}

/**
 * @brief Adds an object value matching a specified key. The object must be a subclass of TUserMapObject.
 * @param key A specified key.
 * @param value A smart pointer to the object that matches the specified key.
 */
void PacMap::PutObject(const std::string &key, const std::shared_ptr<TUserMapObject> &value)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    RemoveDataNode(key);
    std::shared_ptr<PacMapNodeTypeObject> pnode = std::make_shared<PacMapNodeTypeObject>();
    pnode->PutObject(value);
    data_list_.emplace(key, pnode);
}

/**
 * @brief Adds some short values matching a specified key.
 * @param key A specified key.
 * @param value Store a list of short values.
 */
void PacMap::PutShortValueArray(const std::string &key, const std::vector<short> &value)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    PAC_MAP_ADD_ARRAY(Short, key, value, data_list_);
}

/**
 * @brief Adds some integer values matching a specified key.
 * @param key A specified key.
 * @param value Store a list of integer values.
 */
void PacMap::PutIntValueArray(const std::string &key, const std::vector<int> &value)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    PAC_MAP_ADD_ARRAY(Integer, key, value, data_list_);
}

/**
 * @brief Adds some long values matching a specified key.
 * @param key A specified key.
 * @param value Store a list of long values.
 */
void PacMap::PutLongValueArray(const std::string &key, const std::vector<long> &value)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    PAC_MAP_ADD_ARRAY(Long, key, value, data_list_);
}

/**
 * @brief Adds some boolean values matching a specified key.
 * @param key A specified key.
 * @param value Store a list of boolean values.
 */
void PacMap::PutBooleanValueArray(const std::string &key, const std::vector<bool> &value)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    PAC_MAP_ADD_ARRAY(Boolean, key, value, data_list_);
}

/**
 * @brief Adds some char values matching a specified key.
 * @param key A specified key.
 * @param value Store a list of char values.
 */
void PacMap::PutCharValueArray(const std::string &key, const std::vector<char> &value)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    PAC_MAP_ADD_ARRAY(Char, key, value, data_list_);
}

/**
 * @brief Adds some byte values matching a specified key.
 * @param key A specified key.
 * @param value Store a list of byte values.
 */
void PacMap::PutByteValueArray(const std::string &key, const std::vector<AAFwk::byte> &value)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    PAC_MAP_ADD_ARRAY(Byte, key, value, data_list_);
}

/**
 * @brief Adds some float values matching a specified key.
 * @param key A specified key.
 * @param value Store a list of float values.
 */
void PacMap::PutFloatValueArray(const std::string &key, const std::vector<float> &value)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    PAC_MAP_ADD_ARRAY(Float, key, value, data_list_);
}

/**
 * @brief Adds some double values matching a specified key.
 * @param key A specified key.
 * @param value Store a list of double values.
 */
void PacMap::PutDoubleValueArray(const std::string &key, const std::vector<double> &value)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    PAC_MAP_ADD_ARRAY(Double, key, value, data_list_);
}

/**
 * @brief Adds some string {std::string} values matching a specified key.
 * @param key A specified key.
 * @param value Store a list of string values.
 */
void PacMap::PutStringValueArray(const std::string &key, const std::vector<std::string> &value)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    PAC_MAP_ADD_ARRAY(String, key, value, data_list_);
}

/**
 * @brief Inserts all key-value pairs of a map object into the built-in data object.
 * Duplicate key values will be replaced.
 * @param mapData Store a list of key-value pairs.
 */
void PacMap::PutAll(const std::map<std::string, PacMapObject::Object> &mapData)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    data_list_.clear();
    DeepCopyData(data_list_, mapData);
}

/**
 * @brief Saves the data in a PacMap object to the current object. Duplicate key values will be replaced.
 * @param pacMap Store the date of PacMap.
 */
void PacMap::PutAll(PacMap &pacMap)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    data_list_.clear();
    DeepCopyData(data_list_, pacMap.data_list_);
}

/**
 * @brief Obtains the int value matching a specified key.
 * @param key A specified key.
 * @param defaultValue The return value when the function fails.
 * @return If the match is successful, return the value matching the key, otherwise return the @a defaultValue.
 */
int PacMap::GetIntValue(const std::string &key, int defaultValue)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    GET_PAC_MAP_BASE(Integer, data_list_, key, defaultValue);
}

/**
 * @brief Obtains the short value matching a specified key.
 * @param key A specified key.
 * @param defaultValue The return value when the function fails.
 * @return If the match is successful, return the value matching the key, otherwise return the @a defaultValue.
 */
short PacMap::GetShortValue(const std::string &key, short defaultValue)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    GET_PAC_MAP_BASE(Short, data_list_, key, defaultValue);
}

/**
 * @brief Obtains the boolean value matching a specified key.
 * @param key A specified key.
 * @param defaultValue The return value when the function fails.
 * @return If the match is successful, return the value matching the key, otherwise return the @a defaultValue.
 */
bool PacMap::GetBooleanValue(const std::string &key, bool defaultValue)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    GET_PAC_MAP_BASE(Boolean, data_list_, key, defaultValue);
}

/**
 * @brief Obtains the long value matching a specified key.
 * @param key A specified key.
 * @param defaultValue The return value when the function fails.
 * @return If the match is successful, return the value matching the key, otherwise return the @a defaultValue.
 */
long PacMap::GetLongValue(const std::string &key, long defaultValue)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    GET_PAC_MAP_BASE(Long, data_list_, key, defaultValue);
}

/**
 * @brief Obtains the char value matching a specified key.
 * @param key A specified key.
 * @param defaultValue The return value when the function fails.
 * @return If the match is successful, return the value matching the key, otherwise return the @a defaultValue.
 */
char PacMap::GetCharValue(const std::string &key, char defaultValue)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    GET_PAC_MAP_BASE(Char, data_list_, key, defaultValue);
}

/**
 * @brief Obtains the byte value matching a specified key.
 * @param key A specified key.
 * @param defaultValue The return value when the function fails.
 * @return If the match is successful, return the value matching the key, otherwise return the @a defaultValue.
 */
AAFwk::byte PacMap::GetByteValue(const std::string &key, AAFwk::byte defaultValue)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    GET_PAC_MAP_BASE(Byte, data_list_, key, defaultValue);
}

/**
 * @brief Obtains the float value matching a specified key.
 * @param key A specified key.
 * @param defaultValue The return value when the function fails.
 * @return If the match is successful, return the value matching the key, otherwise return the @a defaultValue.
 */
float PacMap::GetFloatValue(const std::string &key, float defaultValue)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    GET_PAC_MAP_BASE(Float, data_list_, key, defaultValue);
}

/**
 * @brief Obtains the double value matching a specified key.
 * @param key A specified key.
 * @param defaultValue The return value when the function fails.
 * @return If the match is successful, return the value matching the key, otherwise return the @a defaultValue.
 */
double PacMap::GetDoubleValue(const std::string &key, double defaultValue)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    GET_PAC_MAP_BASE(Double, data_list_, key, defaultValue);
}

/**
 * @brief Obtains the string {std::string} value matching a specified key.
 * @param key A specified key.
 * @param defaultValue The return value when the function fails.
 * @return If the match is successful, return the value matching the key, otherwise return the @a defaultValue.
 */
std::string PacMap::GetStringValue(const std::string &key, const std::string &defaultValue)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    GET_PAC_MAP_BASE(String, data_list_, key, defaultValue);
}

/**
 * @brief Obtains some int values matching a specified key.
 * @param key A specified key.
 * @param value Save the returned int values.
 */
void PacMap::GetIntValueArray(const std::string &key, std::vector<int> &value)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    GET_PAC_MAP_ARRAY(Integer, data_list_, key, value);
}

/**
 * @brief Obtains some short values matching a specified key.
 * @param key A specified key.
 * @param value Save the returned short values.
 */
void PacMap::GetShortValueArray(const std::string &key, std::vector<short> &value)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    GET_PAC_MAP_ARRAY(Short, data_list_, key, value);
}

/**
 * @brief Obtains some boolean values matching a specified key.
 * @param key A specified key.
 * @param value Save the returned boolean values.
 */
void PacMap::GetBooleanValueArray(const std::string &key, std::vector<bool> &value)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    GET_PAC_MAP_ARRAY(Boolean, data_list_, key, value);
}

/**
 * @brief Obtains some long values matching a specified key.
 * @param key A specified key.
 * @param value Save the returned long values.
 */
void PacMap::GetLongValueArray(const std::string &key, std::vector<long> &value)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    GET_PAC_MAP_ARRAY(Long, data_list_, key, value);
}

/**
 * @brief Obtains some char values matching a specified key.
 * @param key A specified key.
 * @param value Save the returned char values.
 */
void PacMap::GetCharValueArray(const std::string &key, std::vector<char> &value)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    GET_PAC_MAP_ARRAY(Char, data_list_, key, value);
}

/**
 * @brief Obtains some byte values matching a specified key.
 * @param key A specified key.
 * @param value Save the returned byte values.
 */
void PacMap::GetByteValueArray(const std::string &key, std::vector<AAFwk::byte> &value)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    GET_PAC_MAP_ARRAY(Byte, data_list_, key, value);
}

/**
 * @brief Obtains some float values matching a specified key.
 * @param key A specified key.
 * @param value Save the returned float values.
 */
void PacMap::GetFloatValueArray(const std::string &key, std::vector<float> &value)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    GET_PAC_MAP_ARRAY(Float, data_list_, key, value);
}

/**
 * @brief Obtains some double values matching a specified key.
 * @param key A specified key.
 * @param value Save the returned double values.
 */
void PacMap::GetDoubleValueArray(const std::string &key, std::vector<double> &value)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    GET_PAC_MAP_ARRAY(Double, data_list_, key, value);
}

/**
 * @brief Obtains some string {std::string} values matching a specified key.
 * @param key A specified key.
 * @param value Save the returned string {std::string} values.
 */
void PacMap::GetStringValueArray(const std::string &key, std::vector<std::string> &value)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    GET_PAC_MAP_ARRAY(String, data_list_, key, value);
}

/**
 * @brief Obtains the object matching a specified key.
 * @param key A specified key.
 * @return Returns the smart pointer to object that matches the key.
 */
std::shared_ptr<TUserMapObject> PacMap::GetObject(const std::string &key)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    auto it = data_list_.find(key);
    if (it == data_list_.end()) {
        return nullptr;
    }

    PacMapNodeTypeObject *pObject = static_cast<PacMapNodeTypeObject *>(it->second.get());
    if (pObject == nullptr) {
        return nullptr;
    }

    return pObject->GetObject();
}

/**
 * @brief Obtains all the data that has been stored with shallow copy.
 * @return Returns all data in current PacMap. There is no dependency between the returned data and
 * the original data.
 */
std::map<std::string, PacMapObject::Object> PacMap::GetAll(void)
{
    std::lock_guard<std::mutex> mLock(mapLock_);

    PacMapList tmpMapList;
    ShallowCopyData(tmpMapList, this->data_list_);
    return tmpMapList;
}

void PacMap::DeepCopyData(PacMapList &desPacMapList, const PacMapList &srcPacMapList)
{
    for (auto it = srcPacMapList.begin(); it != srcPacMapList.end(); it++) {
        if (it->second->IsBase()) {
            std::shared_ptr<PacMapNodeTypeBase> pac_node = std::make_shared<PacMapNodeTypeBase>();
            pac_node->DeepCopy(it->second.get());
            desPacMapList.emplace(it->first, pac_node);
        } else if (it->second->IsArray()) {
            std::shared_ptr<PacMapNodeTypeArray> pac_node = std::make_shared<PacMapNodeTypeArray>();
            pac_node->DeepCopy(it->second.get());
            desPacMapList.emplace(it->first, pac_node);
        } else if (it->second->IsObject()) {
            std::shared_ptr<PacMapNodeTypeObject> pac_node = std::make_shared<PacMapNodeTypeObject>();
            pac_node->DeepCopy(it->second.get());
            desPacMapList.emplace(it->first, pac_node);
        }
    }
}

void PacMap::ShallowCopyData(PacMapList &desPacMapList, const PacMapList &srcPacMapList)
{
    desPacMapList.clear();
    for (auto it = srcPacMapList.begin(); it != srcPacMapList.end(); it++) {
        desPacMapList.emplace(it->first, it->second);
    }
}

void PacMap::RemoveDataNode(const std::string &key)
{
    auto it = data_list_.find(key);
    if (it != data_list_.end()) {
        data_list_.erase(it);
    }
}

bool PacMap::EqualPacMapData(const PacMapList &leftPacMapList, const PacMapList &rightPacMapList)
{
    for (auto right = rightPacMapList.begin(); right != rightPacMapList.end(); right++) {
        auto left = leftPacMapList.find(right->first);
        if (left == leftPacMapList.end()) {
            return false;
        }

        if (left->second.get() == right->second.get()) {
            continue;
        }

        if (left->second->GetMapNodeTypType() != right->second->GetMapNodeTypType()) {
            return false;
        }

        if (left->second->Equals(right->second.get())) {
            continue;
        } else {
            return false;
        }
    }
    return true;
}

/**
 * @brief Indicates whether some other object is "equal to" this one.
 * @param pacMap The object with which to compare.
 * @return Returns true if this object is the same as the pacMap argument; false otherwise.
 */
bool PacMap::Equals(const PacMap *pacMap)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    if (pacMap == nullptr) {
        return false;
    }

    if (this == pacMap) {
        return true;
    }

    if (data_list_.size() != pacMap->data_list_.size()) {
        return false;
    }

    if (!EqualPacMapData(data_list_, pacMap->data_list_)) {
        return false;
    }

    return true;
}

bool PacMap::Equals(const PacMap &pacMap)
{
    return Equals(&pacMap);
}

/**
 * @brief Checks whether the current object is empty.
 * @return If there is no data, return true, otherwise return false.
 */
bool PacMap::IsEmpty(void) const
{
    return data_list_.empty();
}

/**
 * @brief Obtains the number of key-value pairs stored in the current object.
 * @return Returns the number of key-value pairs.
 */
int PacMap::GetSize(void) const
{
    return data_list_.size();
}

/**
 * @brief Obtains all the keys of the current object.
 */
const std::set<std::string> PacMap::GetKeys(void)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    std::set<std::string> keys;

    for (auto it = data_list_.begin(); it != data_list_.end(); it++) {
        keys.emplace(it->first);
    }
    return keys;
}

/**
 * @brief Checks whether a specified key is contained.
 * @param key Indicates the key in String
 * @return Returns true if the key is contained; returns false otherwise.
 */
bool PacMap::HasKey(const std::string &key)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    return (data_list_.find(key) != data_list_.end());
}

/**
 * @brief Deletes a key-value pair with a specified key.
 * @param key Specifies the key of the deleted data.
 */
void PacMap::Remove(const std::string &key)
{
    std::lock_guard<std::mutex> mLock(mapLock_);
    auto it = data_list_.find(key);
    if (it != data_list_.end()) {
        data_list_.erase(it);
    }
}

bool PacMap::WriteBaseToParcel(
    const std::string &key, const std::shared_ptr<PacMapNode> &nodeData, Parcel &parcel) const
{
    PacMapNodeTypeBase *node_base = static_cast<PacMapNodeTypeBase *>(nodeData.get());
    if (node_base == nullptr) {
        return false;
    }
    return node_base->Marshalling(key, parcel);
}

bool PacMap::WriteArrayToParcel(
    const std::string &key, const std::shared_ptr<PacMapNode> &nodeData, Parcel &parcel) const
{
    PacMapNodeTypeArray *node_array = static_cast<PacMapNodeTypeArray *>(nodeData.get());
    if (node_array == nullptr) {
        return false;
    }
    return node_array->Marshalling(key, parcel);
}

bool PacMap::WriteObjectToParcel(
    const std::string &key, const std::shared_ptr<PacMapNode> &nodeData, Parcel &parcel) const
{
    PacMapNodeTypeObject *node_object = static_cast<PacMapNodeTypeObject *>(nodeData.get());
    if (node_object == nullptr) {
        return false;
    }

    return node_object->Marshalling(key, parcel);
}

bool PacMap::ReadBaseFromParcel(const std::string &key, int32_t dataType, Parcel &parcel)
{
    std::shared_ptr<PacMapNodeTypeBase> pnode = std::make_shared<PacMapNodeTypeBase>();
    if (pnode->Unmarshalling(dataType, parcel)) {
        data_list_.emplace(key, pnode);
        return true;
    }
    return false;
}

bool PacMap::ReadArrayFromParcel(const std::string &key, int32_t dataType, Parcel &parcel)
{
    std::shared_ptr<PacMapNodeTypeArray> pnode = std::make_shared<PacMapNodeTypeArray>();
    if (pnode->Unmarshalling(dataType, parcel)) {
        data_list_.emplace(key, pnode);
        return true;
    }
    return false;
}

bool PacMap::ReadObjectFromParcel(const std::string &key, int32_t dataType, Parcel &parcel)
{
    std::shared_ptr<PacMapNodeTypeObject> pnode = std::make_shared<PacMapNodeTypeObject>();
    if (pnode->Unmarshalling(dataType, parcel)) {
        data_list_.emplace(key, pnode);
        return true;
    }
    return false;
}

bool PacMap::ReadFromParcel(Parcel &parcel)
{
    int32_t size;
    int32_t data_type;
    __uint32_t udata_type;

    // read element count
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, size);

    for (int32_t i = 0; i < size; i++) {
        std::string key;
        data_type = 0;

        // read key
        key = Str16ToStr8(parcel.ReadString16());
        if (key == "") {
            continue;
        }

        // read type of data
        READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, data_type);

        // read data
        udata_type = data_type;
        if ((udata_type & 0x000000FF) == udata_type) {
            if (!ReadBaseFromParcel(key, data_type, parcel)) {
                return false;
            }
        } else if ((udata_type & 0x0000FF00) == udata_type) {
            if (!ReadArrayFromParcel(key, data_type, parcel)) {
                return false;
            }
        } else if ((udata_type & 0x00FF0000) == udata_type) {
            if (!ReadObjectFromParcel(key, data_type, parcel)) {
                return false;
            }
        }
    }
    return true;
}

/**
 * @brief Marshals this Sequenceable object to a Parcel.
 * @param parcel Indicates the Parcel object into which the Sequenceable object has been marshaled.
 * @return Marshals success returns true, otherwise returns false.
 */
bool PacMap::Marshalling(Parcel &parcel) const
{
    if (IsEmpty()) {
        return false;
    }

    // write element count
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, data_list_.size());

    for (auto it = data_list_.begin(); it != data_list_.end(); it++) {
        if (it->second->IsBase()) {
            if (!WriteBaseToParcel(it->first, it->second, parcel)) {
                return false;
            }
        } else if (it->second->IsArray()) {
            if (!WriteArrayToParcel(it->first, it->second, parcel)) {
                return false;
            }
        } else if (it->second->IsObject()) {
            if (!WriteObjectToParcel(it->first, it->second, parcel)) {
                return false;
            }
        }
    }

    return true;
}

/**
 * @brief Unmarshals this Sequenceable object from a Parcel.
 * @param parcel Indicates the Parcel object into which the Sequenceable object has been marshaled.
 * @return Unmarshals success returns a smart pointer to PacMap, otherwise returns nullptr.
 */
PacMap *PacMap::Unmarshalling(Parcel &parcel)
{
    PacMap *pPacMap = new (std::nothrow) PacMap();
    if (pPacMap != nullptr && !pPacMap->ReadFromParcel(parcel)) {
        delete pPacMap;
        return nullptr;
    }
    return pPacMap;
}
}  // namespace AppExecFwk
}  // namespace OHOS
