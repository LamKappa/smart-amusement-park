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

#ifndef PREFERENCES_VALUE_H
#define PREFERENCES_VALUE_H

#include <string>
#include <variant>

namespace OHOS {
namespace NativePreferences {
class PreferencesValue {
public:
    ~PreferencesValue(){};

    explicit PreferencesValue(int value);
    explicit PreferencesValue(int64_t value);
    explicit PreferencesValue(float value);
    explicit PreferencesValue(bool value);
    explicit PreferencesValue(std::string value);

    bool IsInt() const;
    bool IsLong() const;
    bool IsFloat() const;
    bool IsBool() const;
    bool IsString() const;

    operator int();
    operator float();
    operator bool();
    operator int64_t();
    operator std::string();

    bool operator==(const PreferencesValue &value);

private:
    std::variant<int, int64_t, float, bool, std::string> data_;
};
} // End of namespace NativePreferences
} // End of namespace OHOS
#endif // End of #ifndef PREFERENCES_VALUE_H
