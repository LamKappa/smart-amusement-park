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

#include "bundle_extractor.h"

#include "app_log_wrapper.h"
#include "bundle_constants.h"

namespace OHOS {
namespace AppExecFwk {

BundleExtractor::BundleExtractor(const std::string &source) : BaseExtractor(source)
{
    APP_LOGI("BundleExtractor is created");
}

BundleExtractor::~BundleExtractor()
{
    APP_LOGI("BundleExtractoris destroyed");
}

bool BundleExtractor::ExtractProfile(std::ostream &dest) const
{
    return ExtractByName(Constants::BUNDLE_PROFILE_NAME, dest);
}

}  // namespace AppExecFwk
}  // namespace OHOS
