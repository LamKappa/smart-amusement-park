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

#include "mock_bundle_mgr_host.h"

using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;

bool MockBundleMgrHost::DumpInfos(const DumpFlag flag, const std::string &bundleName, std::string &result)
{
    APP_LOGI("enter");

    APP_LOGI("flag: %{public}d", flag);
    APP_LOGI("bundleName: %{public}s", bundleName.c_str());

    if (bundleName.size() > 0) {
        result = bundleName + "\n";
    }

    return true;
}