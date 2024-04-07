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

#include "bundle_parser.h"

#include <sstream>

#include "app_log_wrapper.h"
#include "bundle_constants.h"
#include "bundle_extractor.h"
#include "bundle_profile.h"

namespace OHOS {
namespace AppExecFwk {

ErrCode BundleParser::Parse(const std::string &pathName, InnerBundleInfo &innerBundleInfo) const
{
    APP_LOGI("parse from %{public}s", pathName.c_str());
    BundleExtractor bundleExtractor(pathName);
    if (!bundleExtractor.Init()) {
        APP_LOGE("bundle extractor init failed");
        return ERR_APPEXECFWK_PARSE_UNEXPECTED;
    }

    std::ostringstream outStream;
    if (!bundleExtractor.ExtractProfile(outStream)) {
        APP_LOGE("extract profile file failed");
        return ERR_APPEXECFWK_PARSE_NO_PROFILE;
    }

    BundleProfile bundleProfile;
    return bundleProfile.TransformTo(outStream, innerBundleInfo);
}

}  // namespace AppExecFwk
}  // namespace OHOS
