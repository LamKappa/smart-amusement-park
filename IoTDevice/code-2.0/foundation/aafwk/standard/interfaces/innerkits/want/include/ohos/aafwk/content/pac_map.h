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

#ifndef OHOS_APPEXECFWK_PAC_MAP_H
#define OHOS_APPEXECFWK_PAC_MAP_H

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <memory>
#include <mutex>
#include <set>

#include "parcel.h"
#include "ohos/aafwk/base/pac_map_node.h"
#include "ohos/aafwk/base/pac_map_node_base.h"
#include "ohos/aafwk/base/pac_map_node_array.h"
#include "ohos/aafwk/base/pac_map_node_user_object.h"

namespace OHOS {
namespace AppExecFwk {
namespace PacMapObject {
using Object = std::shared_ptr<PacMapNode>;
}  // namespace PacMapObject

using PacMapList = std::map<std::string, PacMapObject::Object>;

class PacMap : public Parcelable {
public:
    /**
     * @brief Default constructor used to create a PacMap instance, in which the Map object has no key-value pair.
     */
    PacMap() = default;

    /**
     * @brief A replication structure with deep copy.
     */
    PacMap(const PacMap &other);

    ~PacMap();

    /**
     * @brief A overload operation with shallow copy.
     */
    PacMap &operator=(const PacMap &other);

    /**
     * @brief Clear all key-value pairs and free resources.
     */
    void Clear(void);

    /**
     * @brief Creates and returns a copy of this object with shallow copy.
     *
     * @return A clone of this instance.
     */
    PacMap Clone(void);

    /**
     * @brief Creates a deep copy of this PacMap object with deep copy.
     * @param pacMap Returns the constructed object.
     */
    PacMap DeepCopy(void);

    /**
     * @brief Adds a short value matching a specified key.
     * @param key A specified key.
     * @param value The value that matches the specified key.
     */
    void PutShortValue(const std::string &key, short value);

    /**
     * @brief Adds a integer value matching a specified key.
     * @param key A specified key.
     * @param value The value that matches the specified key.
     */
    void PutIntValue(const std::string &key, int value);

    /**
     * @brief Adds a long value matching a specified key.
     * @param key A specified key.
     * @param value The value that matches the specified key.
     */
    void PutLongValue(const std::string &key, long value);

    /**
     * @brief Adds a boolean value matching a specified key.
     * @param key A specified key.
     * @param value The value that matches the specified key.
     */
    void PutBooleanValue(const std::string &key, bool value);

    /**
     * @brief Adds a char value matching a specified key.
     * @param key A specified key.
     * @param value The value that matches the specified key.
     */
    void PutCharValue(const std::string &key, char value);

    /**
     * @brief Adds a byte value matching a specified key.
     * @param key A specified key.
     * @param value The value that matches the specified key.
     */
    void PutByteValue(const std::string &key, AAFwk::byte value);

    /**
     * @brief Adds a float value matching a specified key.
     * @param key A specified key.
     * @param value The value that matches the specified key.
     */
    void PutFloatValue(const std::string &key, float value);

    /**
     * @brief Adds a double value matching a specified key.
     * @param key A specified key.
     * @param value The value that matches the specified key.
     */
    void PutDoubleValue(const std::string &key, double value);

    /**
     * @brief Adds a string {std::string} value matching a specified key.
     * @param key A specified key.
     * @param value The value that matches the specified key.
     */
    void PutStringValue(const std::string &key, const std::string &value);

    /**
     * @brief Adds an object value matching a specified key. The object must be a subclass of TUserMapObject.
     * @param key A specified key.
     * @param value A smart pointer to the object that matches the specified key.
     */
    void PutObject(const std::string &key, const std::shared_ptr<TUserMapObject> &value);

    /**
     * @brief Adds some short values matching a specified key.
     * @param key A specified key.
     * @param value Store a list of short values.
     */
    void PutShortValueArray(const std::string &key, const std::vector<short> &value);

    /**
     * @brief Adds some integer values matching a specified key.
     * @param key A specified key.
     * @param value Store a list of integer values.
     */
    void PutIntValueArray(const std::string &key, const std::vector<int> &value);

    /**
     * @brief Adds some long values matching a specified key.
     * @param key A specified key.
     * @param value Store a list of long values.
     */
    void PutLongValueArray(const std::string &key, const std::vector<long> &value);

    /**
     * @brief Adds some boolean values matching a specified key.
     * @param key A specified key.
     * @param value Store a list of boolean values.
     */
    void PutBooleanValueArray(const std::string &key, const std::vector<bool> &value);

    /**
     * @brief Adds some char values matching a specified key.
     * @param key A specified key.
     * @param value Store a list of char values.
     */
    void PutCharValueArray(const std::string &key, const std::vector<char> &value);

    /**
     * @brief Adds some byte values matching a specified key.
     * @param key A specified key.
     * @param value Store a list of byte values.
     */
    void PutByteValueArray(const std::string &key, const std::vector<AAFwk::byte> &value);

    /**
     * @brief Adds some float values matching a specified key.
     * @param key A specified key.
     * @param value Store a list of float values.
     */
    void PutFloatValueArray(const std::string &key, const std::vector<float> &value);

    /**
     * @brief Adds some double values matching a specified key.
     * @param key A specified key.
     * @param value Store a list of double values.
     */
    void PutDoubleValueArray(const std::string &key, const std::vector<double> &value);

    /**
     * @brief Adds some string {std::string} values matching a specified key.
     * @param key A specified key.
     * @param value Store a list of string values.
     */
    void PutStringValueArray(const std::string &key, const std::vector<std::string> &value);

    /**
     * @brief Inserts all key-value pairs of a map object into the built-in data object.
     * Duplicate key values will be replaced.
     * @param mapData Store a list of key-value pairs.
     */
    void PutAll(const std::map<std::string, PacMapObject::Object> &mapData);

    /**
     * @brief Saves the data in a PacMap object to the current object. Duplicate key values will be replaced.
     * @param pacMap Store the date of PacMap.
     */
    void PutAll(PacMap &pacMap);

    /**
     * @brief Obtains the int value matching a specified key.
     * @param key A specified key.
     * @param defaultValue The return value when the function fails.
     * @return If the match is successful, return the value matching the key, otherwise return the @a defaultValue.
     */
    int GetIntValue(const std::string &key, int defaultValue = 0);

    /**
     * @brief Obtains the short value matching a specified key.
     * @param key A specified key.
     * @param defaultValue The return value when the function fails.
     * @return If the match is successful, return the value matching the key, otherwise return the @a defaultValue.
     */
    short GetShortValue(const std::string &key, short defaultValue = 0);

    /**
     * @brief Obtains the boolean value matching a specified key.
     * @param key A specified key.
     * @param defaultValue The return value when the function fails.
     * @return If the match is successful, return the value matching the key, otherwise return the @a defaultValue.
     */
    bool GetBooleanValue(const std::string &key, bool defaultValue = false);

    /**
     * @brief Obtains the long value matching a specified key.
     * @param key A specified key.
     * @param defaultValue The return value when the function fails.
     * @return If the match is successful, return the value matching the key, otherwise return the @a defaultValue.
     */
    long GetLongValue(const std::string &key, long defaultValue = 0);

    /**
     * @brief Obtains the char value matching a specified key.
     * @param key A specified key.
     * @param defaultValue The return value when the function fails.
     * @return If the match is successful, return the value matching the key, otherwise return the @a defaultValue.
     */
    char GetCharValue(const std::string &key, char defaultValue = 0x00);

    /**
     * @brief Obtains the byte value matching a specified key.
     * @param key A specified key.
     * @param defaultValue The return value when the function fails.
     * @return If the match is successful, return the value matching the key, otherwise return the @a defaultValue.
     */
    AAFwk::byte GetByteValue(const std::string &key, AAFwk::byte defaultValue = 0x00);

    /**
     * @brief Obtains the float value matching a specified key.
     * @param key A specified key.
     * @param defaultValue The return value when the function fails.
     * @return If the match is successful, return the value matching the key, otherwise return the @a defaultValue.
     */
    float GetFloatValue(const std::string &key, float defaultValue = 0.0f);

    /**
     * @brief Obtains the double value matching a specified key.
     * @param key A specified key.
     * @param defaultValue The return value when the function fails.
     * @return If the match is successful, return the value matching the key, otherwise return the @a defaultValue.
     */
    double GetDoubleValue(const std::string &key, double defaultValue = 0.0);

    /**
     * @brief Obtains the string {std::string} value matching a specified key.
     * @param key A specified key.
     * @param defaultValue The return value when the function fails.
     * @return If the match is successful, return the value matching the key, otherwise return the @a defaultValue.
     */
    std::string GetStringValue(const std::string &key, const std::string &defaultValue = "");

    /**
     * @brief Obtains some int values matching a specified key.
     * @param key A specified key.
     * @param value Save the returned int values.
     */
    void GetIntValueArray(const std::string &key, std::vector<int> &value);

    /**
     * @brief Obtains some short values matching a specified key.
     * @param key A specified key.
     * @param value Save the returned short values.
     */
    void GetShortValueArray(const std::string &key, std::vector<short> &value);

    /**
     * @brief Obtains some boolean values matching a specified key.
     * @param key A specified key.
     * @param value Save the returned boolean values.
     */
    void GetBooleanValueArray(const std::string &key, std::vector<bool> &value);

    /**
     * @brief Obtains some long values matching a specified key.
     * @param key A specified key.
     * @param value Save the returned long values.
     */
    void GetLongValueArray(const std::string &key, std::vector<long> &value);

    /**
     * @brief Obtains some char values matching a specified key.
     * @param key A specified key.
     * @param value Save the returned char values.
     */
    void GetCharValueArray(const std::string &key, std::vector<char> &value);

    /**
     * @brief Obtains some byte values matching a specified key.
     * @param key A specified key.
     * @param value Save the returned byte values.
     */
    void GetByteValueArray(const std::string &key, std::vector<AAFwk::byte> &value);

    /**
     * @brief Obtains some float values matching a specified key.
     * @param key A specified key.
     * @param value Save the returned float values.
     */
    void GetFloatValueArray(const std::string &key, std::vector<float> &value);

    /**
     * @brief Obtains some double values matching a specified key.
     * @param key A specified key.
     * @param value Save the returned double values.
     */
    void GetDoubleValueArray(const std::string &key, std::vector<double> &value);

    /**
     * @brief Obtains some string {std::string} values matching a specified key.
     * @param key A specified key.
     * @param value Save the returned string {std::string} values.
     */
    void GetStringValueArray(const std::string &key, std::vector<std::string> &value);

    /**
     * @brief Obtains the object matching a specified key.
     * @param key A specified key.
     * @return Returns the smart pointer to object that matches the key.
     */
    std::shared_ptr<TUserMapObject> GetObject(const std::string &key);

    /**
     * @brief Obtains all the data that has been stored with shallow copy.
     * @return Returns all data in current PacMap. There is no dependency between the returned data and
     * the original data.
     */
    std::map<std::string, PacMapObject::Object> GetAll(void);

    /**
     * @brief Indicates whether some other object is "equal to" this one.
     * @param pacMap The object with which to compare.
     * @return Returns true if this object is the same as the pacMap argument; false otherwise.
     */
    bool Equals(const PacMap *pacMap);
    bool Equals(const PacMap &pacMap);

    /**
     * @brief Checks whether the current object is empty.
     * @return If there is no data, return true, otherwise return false.
     */
    bool IsEmpty(void) const;

    /**
     * @brief Obtains the number of key-value pairs stored in the current object.
     * @return Returns the number of key-value pairs.
     */
    int GetSize(void) const;

    /**
     * @brief Obtains all the keys of the current object.
     */
    const std::set<std::string> GetKeys(void);

    /**
     * @brief Checks whether a specified key is contained.
     * @param key Indicates the key in String
     * @return Returns true if the key is contained; returns false otherwise.
     */
    bool HasKey(const std::string &key);

    /**
     * @brief Deletes a key-value pair with a specified key.
     * @param key Specifies the key of the deleted data.
     */
    void Remove(const std::string &key);

    /**
     * @brief Marshals this Sequenceable object to a Parcel.
     * @param parcel Indicates the Parcel object into which the Sequenceable object has been marshaled.
     * @return Marshals success returns true, otherwise returns false.
     */
    virtual bool Marshalling(Parcel &parcel) const override;

    /**
     * @brief Unmarshals this Sequenceable object from a Parcel.
     * @param parcel Indicates the Parcel object into which the Sequenceable object has been marshaled.
     * @return Unmarshals success returns a smart pointer to PacMap, otherwise returns nullptr.
     */
    static PacMap *Unmarshalling(Parcel &parcel);

private:
    PacMapList data_list_;
    std::mutex mapLock_;

    void DeepCopyData(PacMapList &desPacMapList, const PacMapList &srcPacMapList);
    void ShallowCopyData(PacMapList &desPacMapList, const PacMapList &srcPacMapList);
    void RemoveDataNode(const std::string &key);
    bool EqualPacMapData(const PacMapList &leftPacMapList, const PacMapList &rightPacMapList);
    bool WriteBaseToParcel(const std::string &key, const std::shared_ptr<PacMapNode> &nodeData, Parcel &parcel) const;
    bool WriteArrayToParcel(const std::string &key, const std::shared_ptr<PacMapNode> &nodeData, Parcel &parcel) const;
    bool WriteObjectToParcel(const std::string &key, const std::shared_ptr<PacMapNode> &nodeData, Parcel &parcel) const;
    bool ReadFromParcel(Parcel &parcel);
    bool ReadBaseFromParcel(const std::string &key, int32_t dataType, Parcel &parcel);
    bool ReadArrayFromParcel(const std::string &key, int32_t dataType, Parcel &parcel);
    bool ReadObjectFromParcel(const std::string &key, int32_t dataType, Parcel &parcel);
};
}  // namespace AppExecFwk
}  // namespace OHOS

#endif