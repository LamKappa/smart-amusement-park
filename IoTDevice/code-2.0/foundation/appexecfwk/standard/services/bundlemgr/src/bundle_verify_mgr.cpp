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

#include "bundle_verify_mgr.h"

#include "ipc_skeleton.h"
#include "interfaces/hap_verify.h"
#include "app_log_wrapper.h"
#include "bundle_constants.h"

namespace OHOS {
namespace AppExecFwk {

using namespace OHOS::Security;

bool BundleVerifyMgr::HapVerify(const std::string &filePath, Verify::HapVerifyResult &hapVerifyResult)
{
    auto ret = Verify::HapVerify(filePath, hapVerifyResult);
    APP_LOGI("HapVerify result %{public}d", ret);
    return ret == Verify::HapVerifyResultCode::VERIFY_SUCCESS;
}

}  // namespace AppExecFwk
}  // namespace OHOS