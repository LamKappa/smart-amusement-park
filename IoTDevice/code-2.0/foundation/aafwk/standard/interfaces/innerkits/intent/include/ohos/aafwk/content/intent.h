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
#ifndef OHOS_AAFWK_INTENT_H
#define OHOS_AAFWK_INTENT_H

#include <string>
#include <vector>

#include "intent_params.h"

#include "element_name.h"
#include "parcel.h"

namespace OHOS {
namespace AAFwk {

class Intent final : public Parcelable {
public:
    Intent();
    inline ~Intent()
    {}

    static Intent *ParseUri(const std::string &uri);
    std::string ToUri();

    std::string GetAction() const;
    Intent &SetAction(const std::string &action);

    unsigned int GetFlags() const;
    Intent &SetFlag(const unsigned int flag);
    Intent &AddFlag(const unsigned int flag);
    void RemoveFlags(const unsigned int flag);

    std::string GetEntity() const;
    Intent &SetEntity(const std::string &entity);

    OHOS::AppExecFwk::ElementName GetElement() const;
    Intent &SetElement(const OHOS::AppExecFwk::ElementName &element);

    bool GetBoolParam(const std::string &key, const bool defaultValue);
    Intent &SetBoolParam(const std::string &key, const bool value);
    std::vector<bool> GetBoolArrayParam(const std::string &key);
    Intent &SetBoolArrayParam(const std::string &key, const std::vector<bool> &value);

    zchar GetCharParam(const std::string &key, const zchar defaultValue);
    Intent &SetCharParam(const std::string &key, const zchar value);
    std::vector<zchar> GetCharArrayParam(const std::string &key);
    Intent &SetCharArrayParam(const std::string &key, const std::vector<zchar> &value);

    byte GetByteParam(const std::string &key, const byte defaultValue);
    Intent &SetByteParam(const std::string &key, const byte value);
    std::vector<byte> GetByteArrayParam(const std::string &key);
    Intent &SetByteArrayParam(const std::string &key, const std::vector<byte> &value);

    short GetShortParam(const std::string &key, const short defaultValue);
    Intent &SetShortParam(const std::string &key, const short value);
    std::vector<short> GetShortArrayParam(const std::string &key);
    Intent &SetShortArrayParam(const std::string &key, const std::vector<short> &value);

    int GetIntParam(const std::string &key, const int defaultValue);
    Intent &SetIntParam(const std::string &key, const int value);
    std::vector<int> GetIntArrayParam(const std::string &key);
    Intent &SetIntArrayParam(const std::string &key, const std::vector<int> &value);

    long GetLongParam(const std::string &key, const long defaultValue);
    Intent &SetLongParam(const std::string &key, const long value);
    std::vector<long> GetLongArrayParam(const std::string &key);
    Intent &SetLongArrayParam(const std::string &key, const std::vector<long> &value);

    float GetFloatParam(const std::string &key, const float defaultValue);
    Intent &SetFloatParam(const std::string &key, const float value);
    std::vector<float> GetFloatArrayParam(const std::string &key);
    Intent &SetFloatArrayParam(const std::string &key, const std::vector<float> &value);

    double GetDoubleParam(const std::string &key, const double defaultValue);
    Intent &SetDoubleParam(const std::string &key, const double value);
    std::vector<double> GetDoubleArrayParam(const std::string &key);
    Intent &SetDoubleArrayParam(const std::string &key, const std::vector<double> &value);

    std::string GetStringParam(const std::string &key);
    Intent &SetStringParam(const std::string &key, const std::string &value);
    std::vector<std::string> GetStringArrayParam(const std::string &key);
    Intent &SetStringArrayParam(const std::string &key, const std::vector<std::string> &value);

    bool HasParameter(const std::string &key) const;

    virtual bool Marshalling(Parcel &parcel) const;

    static Intent *Unmarshalling(Parcel &parcel);

public:
    // action definition
    static const std::string ACTION_PLAY;

    // entity definition
    static const std::string ENTITY_HOME;
    static const std::string ENTITY_VIDEO;

    // flag definition
    static constexpr unsigned int FLAG_ABILITY_NEW_MISSION = 0x00000001;

private:
    std::string action_;
    unsigned int flags_;
    std::string entity_;
    IntentParams parameters_;

    OHOS::AppExecFwk::ElementName element_;

    static const std::string OCT_EQUALSTO;
    static const std::string OCT_SEMICOLON;

    // no object in parcel
    static constexpr int VALUE_NULL = -1;
    // object exist in parcel
    static constexpr int VALUE_OBJECT = 1;

private:
    static bool ParseFlag(const std::string &content, Intent &intent);
    static std::string Decode(const std::string &str);
    static std::string Encode(const std::string &str);
    static bool ParseContent(const std::string &content, std::string &prop, std::string &value);
    static bool ParseUriInternal(const std::string &content, OHOS::AppExecFwk::ElementName &element, Intent &intent);
    bool ReadFromParcel(Parcel &parcel);
};

}  // namespace AAFwk
}  // namespace OHOS

#endif  // OHOS_AAFWK_INTENT_H
