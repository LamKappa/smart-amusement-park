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

#ifndef OHOS_AAFWK_WANT_H
#define OHOS_AAFWK_WANT_H

#include <string>
#include <vector>
#include <algorithm>

#include "uri.h"
#include "want_params.h"
#include "element_name.h"
#include "operation.h"
#include "parcel.h"

using Operation = OHOS::AAFwk::Operation;

namespace OHOS {
namespace AAFwk {
class Want final : public Parcelable {
public:
    /**
     * Indicates the grant to perform read operations on the URI.
     */
    static constexpr unsigned int FLAG_AUTH_READ_URI_PERMISSION = 0x00000001;
    /**
     * Indicates the grant to perform write operations on the URI.
     */
    static constexpr unsigned int FLAG_AUTH_WRITE_URI_PERMISSION = 0x00000002;
    /**
     * Returns the result to the source ability.
     */
    static constexpr unsigned int FLAG_ABILITY_FORWARD_RESULT = 0x00000004;
    /**
     * Determines whether an ability on the local device can be migrated to a remote device.
     */
    static constexpr unsigned int FLAG_ABILITY_CONTINUATION = 0x00000008;
    /**
     * Specifies whether a component does not belong to OHOS.
     */
    static constexpr unsigned int FLAG_NOT_OHOS_COMPONENT = 0x00000010;
    /**
     * Specifies whether an ability is started.
     */
    static constexpr unsigned int FLAG_ABILITY_FORM_ENABLED = 0x00000020;
    /**
     * Indicates the grant for possible persisting on the URI.
     */
    static constexpr unsigned int FLAG_AUTH_PERSISTABLE_URI_PERMISSION = 0x00000040;
    /**
     * Returns the result to the source ability slice.
     */
    static constexpr unsigned int FLAG_AUTH_PREFIX_URI_PERMISSION = 0x00000080;
    /**
     * Supports multi-device startup in the distributed scheduling system.
     */
    static constexpr unsigned int FLAG_ABILITYSLICE_MULTI_DEVICE = 0x00000100;
    /**
     * Indicates that an ability using the Service template is started regardless of whether the host application has
     * been started.
     */
    static constexpr unsigned int FLAG_START_FOREGROUND_ABILITY = 0x00000200;
    /**
     * Install the specified ability if it's not installed.
     */
    static constexpr unsigned int FLAG_INSTALL_ON_DEMAND = 0x00000800;
    /**
     * Returns the result to the source ability slice.
     */
    static constexpr unsigned int FLAG_ABILITYSLICE_FORWARD_RESULT = 0x04000000;
    /**
     * Install the specifiedi ability with background mode if it's not installed.
     */
    static constexpr unsigned int FLAG_INSTALL_WITH_BACKGROUND_MODE = 0x80000000;

    /**
     * @description:  Default construcotr of Want class, which is used to initialzie flags and URI.
     * @param None
     * @return None
     */
    Want();

    /**
     * @description: Default deconstructor of Want class
     * @param None
     * @return None
     */
    ~Want();

    /**
     * @description: Copy construcotr of Want class, which is used to initialzie flags, URI, etc.
     * @param want the source instance of Want.
     * @return None
     */
    Want(const Want &want);
    Want &operator=(const Want &);

    /**
     * @description: Sets a flag in a Want.
     * @param flags Indicates the flag to set.
     * @return Returns this Want object containing the flag.
     */
    Want &SetFlags(unsigned int flags);

    /**
     * @description: Obtains the description of flags in a Want.
     * @return Returns the flag description in the Want.
     */
    unsigned int GetFlags() const;

    /**
     * @description: Adds a flag to a Want.
     * @param flags Indicates the flag to add.
     * @return Returns the Want object with the added flag.
     */
    Want &AddFlags(unsigned int flags);

    /**
     * @description: Removes the description of a flag from a Want.
     * @param flags Indicates the flag to remove.
     * @return Removes the description of a flag from a Want.
     */
    void RemoveFlags(unsigned int flag);

    /**
     * @description: Sets the bundleName and abilityName attributes for this Want object.
     * @param bundleName Indicates the bundleName to set for the operation attribute in the Want.
     * @param abilityName Indicates the abilityName to set for the operation attribute in the Want.
     * @return Returns this Want object that contains the specified bundleName and abilityName attributes.
     */
    Want &SetElementName(const std::string &bundleName, const std::string &abilityName);

    /**
     * @description: Sets the bundleName and abilityName attributes for this Want object.
     * @param deviceId Indicates the deviceId to set for the operation attribute in the Want.
     * @param bundleName Indicates the bundleName to set for the operation attribute in the Want.
     * @param abilityName Indicates the abilityName to set for the operation attribute in the Want.
     * @return Returns this Want object that contains the specified bundleName and abilityName attributes.
     */
    Want &SetElementName(const std::string &deviceId, const std::string &bundleName, const std::string &abilityName);

    /**
     * @description: Sets an ElementName object in a Want.
     * @param element Indicates the ElementName description.
     * @return Returns this Want object containing the ElementName
     */
    Want &SetElement(const OHOS::AppExecFwk::ElementName &element);

    /**
     * @description: Obtains the description of the ElementName object in a Want.
     * @return Returns the ElementName description in the Want.
     */
    OHOS::AppExecFwk::ElementName GetElement() const;

    /**
     * @description: Creates a want with its corresponding attributes specified for starting the main ability of an
     * application.
     * @param ElementName  Indicates the ElementName object defining the deviceId, bundleName,
     * and abilityName sub-attributes of the operation attribute in a want.
     * @return Returns the want object used to start the main ability of an application.
     */
    static Want *MakeMainAbility(const OHOS::AppExecFwk::ElementName &elementName);

    /**
     * @description: Creates a Want instance by using a given Uniform Resource Identifier (URI).
     * This method parses the input URI and saves it in a Want object.
     * @param uri Indicates the URI to parse.
     * @return Returns a Want object containing the URI.
     */
    static Want *WantParseUri(const char *uri);

    /**
     * @description: Creates a Want instance by using a given Uniform Resource Identifier (URI).
     * This method parses the input URI and saves it in a Want object.
     * @param uri Indicates the URI to parse.
     * @return Returns a Want object containing the URI.
     */
    static Want *ParseUri(const std::string &uri);

    /**
     * @description: Obtains the description of a URI in a Want.
     * @return Returns the URI description in the Want.
     */
    Uri GetUri() const;

    /**
     * @description: Obtains the string representation of the URI in this Want.
     * @return Returns the string of the URI.
     */
    std::string GetUriString() const;

    /**
     * @description: Sets the description of a URI in a Want.
     * @param uri Indicates the URI description.
     * @return Returns this Want object containing the URI.
     */
    Want &SetUri(const std::string &uri);

    /**
     * @description: Sets the description of a URI in a Want.
     * @param uri Indicates the URI description.
     * @return Returns this Want object containing the URI.
     */
    Want &SetUri(const Uri &uri);

    /**
     * @description: Sets the description of a URI and a type in this Want.
     * @param uri Indicates the URI description.
     * @param type Indicates the type description.
     * @return Returns the Want object containing the URI and the type by setting.
     */
    Want &SetUriAndType(const Uri &uri, const std::string &type);

    /**
     * @description: Converts a Want into a URI string containing a representation of it.
     * @param want Indicates the want description.--Want.
     * @return   Returns a encoding URI string describing the Want object.
     */
    std::string WantToUri(Want &want);

    /**
     * @description: Converts parameter information in a Want into a URI string.
     * @return Returns the URI string.
     */
    std::string ToUri() const;

    /**
     * @description: Formats a specified URI.
     * This method uses the Uri.getLowerCaseScheme() method to format a URI and then saves
     * the formatted URI to this Want object.
     * @param uri Indicates the string of URI to format.
     * @return Returns this Want object that contains the formatted uri attribute.
     */
    Want &FormatUri(const std::string &uri);

    /**
     * @description: Formats a specified URI.
     * This method uses the Uri.getLowerCaseScheme() method to format a URI and then saves
     * the formatted URI to this Want object.
     * @param uri Indicates the URI to format.
     * @return Returns this Want object that contains the formatted URI attribute.
     */
    Want &FormatUri(const Uri &uri);

    /**
     * @description: Obtains the description of an action in a want.
     * @return Returns a Want object that contains the action description.
     */
    std::string GetAction() const;

    /**
     * @description: Sets the description of an action in a want.
     * @param action Indicates the action description to set.
     * @return Returns a Want object that contains the action description.
     */
    Want &SetAction(const std::string &action);

    /**
     * @description: Obtains the name of the specified bundle in a Want.
     * @return Returns the specified bundle name in the Want.
     */
    std::string GetBundle() const;

    /**
     * @description: Sets a bundle name in this Want.
     * If a bundle name is specified in a Want, the Want will match only
     * the abilities in the specified bundle. You cannot use this method and
     * setPicker(ohos.aafwk.content.Want) on the same Want.
     * @param bundleName Indicates the bundle name to set.
     * @return Returns a Want object containing the specified bundle name.
     */
    Want &SetBundle(const std::string &bundleName);

    /**
     * @description: Obtains the description of all entities in a Want
     * @return Returns a set of entities
     */
    const std::vector<std::string> &GetEntities() const;

    /**
     * @description: Adds the description of an entity to a Want
     * @param entity Indicates the entity description to add
     * @return {Want} Returns this Want object containing the entity.
     */
    Want &AddEntity(const std::string &entity);

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
    bool HasEntity(const std::string &key) const;

    /**
     * @description: Obtains the number of entities in a Want
     * @return Returns the entity quantity
     */
    int CountEntities();

    /**
     * @description: Obtains the description of the URI scheme in this want.
     * @return Returns the URI scheme description in this want.
     */
    const std::string GetScheme() const;

    /**
     * @description: Obtains the description of the type in this Want
     * @return Returns the type description in this Want
     */
    std::string GetType() const;

    /**
     * @description: Sets the description of a type in this Want
     * @param type Indicates the type description
     * @return Returns this Want object containing the type
     */
    Want &SetType(const std::string &type);

    /**
     * @description: Formats a specified MIME type. This method uses
     * the formatMimeType(java.lang.String) method to format a MIME type
     * and then saves the formatted type to this Want object.
     * @param type Indicates the MIME type to format
     * @return Returns this Want object that contains the formatted type attribute
     */
    Want &FormatType(const std::string &type);

    /**
     * @description: Formats a specified URI and MIME type.
     * This method works in the same way as formatUri(ohos.utils.net.URI)
     * and formatType(java.lang.String).
     * @param uri Indicates the URI to format.
     * @param type Indicates the MIME type to format.
     * @return Returns this Want object that contains the formatted URI and type attributes.
     */
    Want &FormatUriAndType(const Uri &uri, const std::string &type);

    /**
     * @description: This method formats data of a specified MIME type
     * by removing spaces from the data and converting the data into
     * lowercase letters. You can use this method to normalize
     * the external data used to create Want information.
     * @param type Indicates the MIME type to format
     * @return Returns this Want object that contains the formatted type attribute
     */
    static std::string FormatMimeType(const std::string &mimeType);

    /**
     * @description: clear the specific want object.
     * @param want Indicates the want to clear
     */
    static void ClearWant(Want *want);

    /**
     * @description: Obtains the description of the WantParams object in a Want
     * @return Returns the WantParams description in the Want
     */
    const WantParams &GetParams() const;

    /**
     * @description: Sets a wantParams object in a want.
     * @param wantParams  Indicates the wantParams description.
     * @return Returns this want object containing the wantParams.
     */
    Want &SetParams(const WantParams &wantParams);

    /**
     * @description: Obtains a bool-type value matching the given key.
     * @param key   Indicates the key of WantParams.
     * @param defaultValue  Indicates the default bool-type value.
     * @return Returns the bool-type value of the parameter matching the given key;
     * returns the default value if the key does not exist.
     */
    bool GetBoolParam(const std::string &key, bool defaultValue) const;

    /**
     * @description: Obtains a bool-type array matching the given key.
     * @param key   Indicates the key of WantParams.
     * @return Returns the bool-type array of the parameter matching the given key;
     * returns null if the key does not exist.
     */
    std::vector<bool> GetBoolArrayParam(const std::string &key) const;

    /**
     * @description: Sets a parameter value of the boolean type.
     * @param key   Indicates the key matching the parameter.
     * @param value Indicates the boolean value of the parameter.
     * @return Returns this want object containing the parameter value.
     */
    Want &SetParam(const std::string &key, bool value);

    /**
     * @description: Sets a parameter value of the boolean array type.
     * @param key   Indicates the key matching the parameter.
     * @param value Indicates the boolean array of the parameter.
     * @return Returns this want object containing the parameter value.
     */
    Want &SetParam(const std::string &key, const std::vector<bool> &value);

    /**
     * @description: Obtains a byte-type value matching the given key.
     * @param key   Indicates the key of WantParams.
     * @param defaultValue  Indicates the default byte-type value.
     * @return Returns the byte-type value of the parameter matching the given key;
     * returns the default value if the key does not exist.
     */
    byte GetByteParam(const std::string &key, byte defaultValue) const;

    /**
     * @description: Obtains a byte-type array matching the given key.
     * @param key   Indicates the key of WantParams.
     * @return Returns the byte-type array of the parameter matching the given key;
     * returns null if the key does not exist.
     */
    std::vector<byte> GetByteArrayParam(const std::string &key) const;

    /**
     * @description: Sets a parameter value of the byte type.
     * @param key   Indicates the key matching the parameter.
     * @param value Indicates the byte-type value of the parameter.
     * @return Returns this Want object containing the parameter value.
     */
    Want &SetParam(const std::string &key, byte value);

    /**
     * @description: Sets a parameter value of the byte array type.
     * @param key   Indicates the key matching the parameter.
     * @param value Indicates the byte array of the parameter.
     * @return Returns this Want object containing the parameter value.
     */
    Want &SetParam(const std::string &key, const std::vector<byte> &value);

    /**
     * @description: Obtains a char value matching the given key.
     * @param key   Indicates the key of wnatParams.
     * @param value Indicates the default char value.
     * @return Returns the char value of the parameter matching the given key;
     * returns the default value if the key does not exist.
     */
    zchar GetCharParam(const std::string &key, zchar defaultValue) const;

    /**
     * @description: Obtains a char array matching the given key.
     * @param key   Indicates the key of wantParams.
     * @return Returns the char array of the parameter matching the given key;
     * returns null if the key does not exist.
     */
    std::vector<zchar> GetCharArrayParam(const std::string &key) const;

    /**
     * @description: Sets a parameter value of the char type.
     * @param key   Indicates the key of wantParams.
     * @param value Indicates the char value of the parameter.
     * @return Returns this want object containing the parameter value.
     */
    Want &SetParam(const std::string &key, zchar value);

    /**
     * @description: Sets a parameter value of the char array type.
     * @param key   Indicates the key of wantParams.
     * @param value Indicates the char array of the parameter.
     * @return Returns this want object containing the parameter value.
     */
    Want &SetParam(const std::string &key, const std::vector<zchar> &value);

    /**
     * @description: Obtains an int value matching the given key.
     * @param key   Indicates the key of wantParams.
     * @param value Indicates the default int value.
     * @return Returns the int value of the parameter matching the given key;
     * returns the default value if the key does not exist.
     */
    int GetIntParam(const std::string &key, int defaultValue) const;

    /**
     * @description: Obtains an int array matching the given key.
     * @param key   Indicates the key of wantParams.
     * @return Returns the int array of the parameter matching the given key;
     * returns null if the key does not exist.
     */
    std::vector<int> GetIntArrayParam(const std::string &key) const;

    /**
     * @description: Sets a parameter value of the int type.
     * @param key   Indicates the key matching the parameter.
     * @param value Indicates the int value of the parameter.
     * @return Returns this Want object containing the parameter value.
     */
    Want &SetParam(const std::string &key, int value);

    /**
     * @description: Sets a parameter value of the int array type.
     * @param key   Indicates the key matching the parameter.
     * @param value Indicates the int array of the parameter.
     * @return Returns this Want object containing the parameter value.
     */
    Want &SetParam(const std::string &key, const std::vector<int> &value);

    /**
     * @description: Obtains a double value matching the given key.
     * @param key   Indicates the key of wantParams.
     * @param defaultValue  Indicates the default double value.
     * @return Returns the double value of the parameter matching the given key;
     * returns the default value if the key does not exist.
     */
    double GetDoubleParam(const std::string &key, double defaultValue) const;

    /**
     * @description: Obtains a double array matching the given key.
     * @param key   Indicates the key of WantParams.
     * @return Returns the double array of the parameter matching the given key;
     * returns null if the key does not exist.
     */
    std::vector<double> GetDoubleArrayParam(const std::string &key) const;

    /**
     * @description: Sets a parameter value of the double type.
     * @param key   Indicates the key matching the parameter.
     * @param value Indicates the int value of the parameter.
     * @return Returns this Want object containing the parameter value.
     */
    Want &SetParam(const std::string &key, double value);

    /**
     * @description: Sets a parameter value of the double array type.
     * @param key   Indicates the key matching the parameter.
     * @param value Indicates the double array of the parameter.
     * @return Returns this want object containing the parameter value.
     */
    Want &SetParam(const std::string &key, const std::vector<double> &value);

    /**
     * @description: Obtains a float value matching the given key.
     * @param key   Indicates the key of wnatParams.
     * @param value Indicates the default float value.
     * @return Returns the float value of the parameter matching the given key;
     * returns the default value if the key does not exist.
     */
    float GetFloatParam(const std::string &key, float defaultValue) const;

    /**
     * @description: Obtains a float array matching the given key.
     * @param key Indicates the key of WantParams.
     * @return Obtains a float array matching the given key.
     */
    std::vector<float> GetFloatArrayParam(const std::string &key) const;

    /**
     * @description: Sets a parameter value of the float type.
     * @param key Indicates the key matching the parameter.
     * @param value Indicates the byte-type value of the parameter.
     * @return Returns this Want object containing the parameter value.
     */
    Want &SetParam(const std::string &key, float value);

    /**
     * @description: Sets a parameter value of the float array type.
     * @param key Indicates the key matching the parameter.
     * @param value Indicates the byte-type value of the parameter.
     * @return Returns this Want object containing the parameter value.
     */
    Want &SetParam(const std::string &key, const std::vector<float> &value);

    /**
     * @description: Obtains a long value matching the given key.
     * @param key Indicates the key of wantParams.
     * @param value Indicates the default long value.
     * @return Returns the long value of the parameter matching the given key;
     * returns the default value if the key does not exist.
     */
    long GetLongParam(const std::string &key, long defaultValue) const;

    /**
     * @description: Obtains a long array matching the given key.
     * @param key Indicates the key of wantParams.
     * @return Returns the long array of the parameter matching the given key;
     * returns null if the key does not exist.
     */
    std::vector<long> GetLongArrayParam(const std::string &key) const;

    /**
     * @description: Sets a parameter value of the long type.
     * @param key Indicates the key matching the parameter.
     * @param value Indicates the byte-type value of the parameter.
     * @return Returns this Want object containing the parameter value.
     */
    Want &SetParam(const std::string &key, long value);

    /**
     * @description: Sets a parameter value of the long array type.
     * @param key Indicates the key matching the parameter.
     * @param value Indicates the byte-type value of the parameter.
     * @return Returns this Want object containing the parameter value.
     */
    Want &SetParam(const std::string &key, const std::vector<long> &value);

    /**
     * @description: a short value matching the given key.
     * @param key Indicates the key of wantParams.
     * @param defaultValue Indicates the default short value.
     * @return Returns the short value of the parameter matching the given key;
     * returns the default value if the key does not exist.
     */
    short GetShortParam(const std::string &key, short defaultValue) const;

    /**
     * @description: Obtains a short array matching the given key.
     * @param key Indicates the key of wantParams.
     * @return Returns the short array of the parameter matching the given key;
     * returns null if the key does not exist.
     */
    std::vector<short> GetShortArrayParam(const std::string &key) const;

    /**
     * @description: Sets a parameter value of the short type.
     * @param key Indicates the key matching the parameter.
     * @param value Indicates the byte-type value of the parameter.
     * @return Returns this Want object containing the parameter value.
     */
    Want &SetParam(const std::string &key, short value);

    /**
     * @description: Sets a parameter value of the short array type.
     * @param key Indicates the key matching the parameter.
     * @param value Indicates the byte-type value of the parameter.
     * @return Returns this Want object containing the parameter value.
     */
    Want &SetParam(const std::string &key, const std::vector<short> &value);

    /**
     * @description: Obtains a string value matching the given key.
     * @param key Indicates the key of wantParams.
     * @return Returns the string value of the parameter matching the given key;
     * returns null if the key does not exist.
     */
    std::string GetStringParam(const std::string &key) const;

    /**
     * @description: Obtains a string array matching the given key.
     * @param key Indicates the key of wantParams.
     * @return Returns the string array of the parameter matching the given key;
     * returns null if the key does not exist.
     */
    std::vector<std::string> GetStringArrayParam(const std::string &key) const;

    /**
     * @description: Sets a parameter value of the string type.
     * @param key Indicates the key matching the parameter.
     * @param value Indicates the byte-type value of the parameter.
     * @return Returns this Want object containing the parameter value.
     */
    Want &SetParam(const std::string &key, const std::string &value);

    /**
     * @description: Sets a parameter value of the string array type.
     * @param key Indicates the key matching the parameter.
     * @param value Indicates the byte-type value of the parameter.
     * @return Returns this Want object containing the parameter value.
     */
    Want &SetParam(const std::string &key, const std::vector<std::string> &value);

    /**
     * @description: Checks whether a Want contains the parameter matching a given key.
     * @param key Indicates the key.
     * @return Returns true if the Want contains the parameter; returns false otherwise.
     */
    bool HasParameter(const std::string &key) const;

    /**
     * @description: Replaces parameters in this Want object with those in the given WantParams object.
     * @param wantParams Indicates the WantParams object containing the new parameters.
     * @return Returns this Want object containing the new parameters.
     */
    Want *ReplaceParams(WantParams &wantParams);

    /**
     * @description: Replaces parameters in this Want object with those in the given Want object.
     * @param want Indicates the Want object containing the new parameters.
     * @return Returns this Want object containing the new parameters.
     */
    Want *ReplaceParams(Want &want);

    /**
     * @description: Removes the parameter matching the given key.
     * @param key Indicates the key matching the parameter to be removed.
     */
    void RemoveParam(const std::string &key);

    /**
     * @description: Gets the description of an operation in a Want.
     * @return Returns the operation included in this Want.
     */
    Operation GetOperation() const;

    /**
     * @description: Sets the description of an operation in a Want.
     * @param operation Indicates the operation description.
     */
    void SetOperation(const OHOS::AAFwk::Operation &operation);

    /**
     * @description: Sets the description of an operation in a Want.
     * @param want Indicates the Want object to compare.
     * @return Returns true if the operation components of the two objects are equal; returns false otherwise.
     */
    bool OperationEquals(const Want &want);

    /**
     * @description: Creates a Want object that contains only the operation component of this Want.
     * @return Returns the created Want object.
     */
    Want *CloneOperation();

    /**
     * @description: Marshals a Want into a Parcel.
     * Fields in the Want are marshalled separately. If any field fails to be marshalled, false is returned.
     * @param parcel Indicates the Parcel object for marshalling.
     * @return Returns true if the marshalling is successful; returns false otherwise.
     */
    virtual bool Marshalling(Parcel &parcel) const;

    /**
     * @description: Unmarshals a Want from a Parcel.
     * Fields in the Want are unmarshalled separately. If any field fails to be unmarshalled, false is returned.
     * @param parcel Indicates the Parcel object for unmarshalling.
     * @return Returns true if the unmarshalling is successful; returns false otherwise.
     */
    static Want *Unmarshalling(Parcel &parcel);

public:
    // action definition
    static const std::string ACTION_PLAY;
    static const std::string ACTION_HOME;

    // entity definition
    static const std::string ENTITY_HOME;
    static const std::string ENTITY_VIDEO;
    static const std::string FLAG_HW_HOME_INTENT_FROM_SYSTEM;

    // flag definition
    static unsigned int FLAG_ABILITY_NEW_MISSION;
    static unsigned int FLAG_ABILITY_CLEAR_MISSION;

    static constexpr int HEX_STRING_BUF_LEN = 36;
    static constexpr int HEX_STRING_LEN = 10;

private:
    WantParams parameters_;
    Operation operation_;
    Want *picker_;

    static const std::string OCT_EQUALSTO;
    static const std::string OCT_SEMICOLON;
    static const std::string MIME_TYPE;
    static const std::string WANT_HEADER;

    // no object in parcel
    static constexpr int VALUE_NULL = -1;
    // object exist in parcel
    static constexpr int VALUE_OBJECT = 1;

private:
    static bool ParseFlag(const std::string &content, Want &want);
    static std::string Decode(const std::string &str);
    static std::string Encode(const std::string &str);
    static bool ParseContent(const std::string &content, std::string &prop, std::string &value);
    static bool ParseUriInternal(const std::string &content, OHOS::AppExecFwk::ElementName &element, Want &want);
    bool ReadFromParcel(Parcel &parcel);
    static bool CheckAndSetParameters(Want &want, const std::string &key, std::string &prop, const std::string &value);
    Uri GetLowerCaseScheme(const Uri &uri);
};
}  // namespace AAFwk
}  // namespace OHOS

#endif  // OHOS_AAFWK_WANT_H