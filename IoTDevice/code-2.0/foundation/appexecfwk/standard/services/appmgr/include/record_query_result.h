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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_APPMGR_INCLUDE_APP_MGR_RECORF_QUERY_RESULT_H
#define FOUNDATION_APPEXECFWK_SERVICES_APPMGR_INCLUDE_APP_MGR_RECORF_QUERY_RESULT_H

namespace OHOS {
namespace AppExecFwk {

struct RecordQueryResult {
    void Reset()
    {
        appExists = false;
        abilityExists = false;
        error = ERR_OK;
        appRecordId = 0;
    }
    // if app not exists, create and set |appRecordId|
    int32_t appRecordId = 0;
    // if ability not exists, create and set |abilityRecordId|
    bool appExists = false;
    bool abilityExists = false;
    ErrCode error = ERR_OK;
};

}  // namespace AppExecFwk
}  // namespace OHOS

#endif  // FOUNDATION_APPEXECFWK_SERVICES_APPMGR_INCLUDE_APP_MGR_RECORF_QUERY_RESULT_H