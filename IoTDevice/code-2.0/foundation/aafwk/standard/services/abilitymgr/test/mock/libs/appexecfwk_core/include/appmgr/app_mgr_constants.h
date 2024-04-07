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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_INCLUDE_APP_MGR_DEF_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_INCLUDE_APP_MGR_DEF_H

namespace OHOS {
namespace AppExecFwk {

enum class ApplicationState {
    APP_STATE_BEGIN = 0,
    APP_STATE_CREATE = APP_STATE_BEGIN,
    APP_STATE_READY,
    APP_STATE_FOREGROUND,
    APP_STATE_BACKGROUND,
    APP_STATE_SUSPENDED,
    APP_STATE_TERMINATED,
    APP_STATE_END,
};

enum class AbilityState {
    ABILITY_STATE_BEGIN = 0,
    ABILITY_STATE_CREATE = ABILITY_STATE_BEGIN,
    ABILITY_STATE_READY,
    ABILITY_STATE_FOREGROUND,
    ABILITY_STATE_BACKGROUND,
    ABILITY_STATE_TERMINATED,
    ABILITY_STATE_END,
};

enum class AppMgrResultCode { RESULT_OK = 0, ERROR_SERVICE_NOT_READY, ERROR_SERVICE_NOT_CONNECTED };

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_INCLUDE_APP_MGR_DEF_H
