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
#include "locale_info.h"
#include "ohos/init_data.h"

namespace OHOS {
namespace Global {
namespace I18n {
using namespace icu;

LocaleInfo::LocaleInfo(std::string localeTag)
{
    UErrorCode status = U_ZERO_ERROR;
    auto builder = std::make_unique<LocaleBuilder>();
    Locale locale = builder->setLanguageTag(StringPiece(localeTag)).build(status);
    if (status != U_ZERO_ERROR) {
        locale = Locale::getDefault();
    }
    language = locale.getLanguage();
    script = locale.getScript();
    region = locale.getCountry();
    baseName = language;
    if (script.length() == SCRIPT_LEN) {
        baseName += "-" + script;
    }
    if (region.length() == REGION_LEN) {
        baseName += "-" + region;
    }
}

LocaleInfo::~LocaleInfo() {}

bool LocaleInfo::icuInitialized = LocaleInfo::Init();

std::string LocaleInfo::GetLanguage() const
{
    return language;
}

std::string LocaleInfo::GetScript() const
{
    return script;
}

std::string LocaleInfo::GetRegion() const
{
    return region;
}

std::string LocaleInfo::GetBaseName() const
{
    return baseName;
}

bool LocaleInfo::Init()
{
    SetHwIcuDirectory();
    return true;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS