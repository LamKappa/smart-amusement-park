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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_APPMGR_APP_TASK_INFO_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_APPMGR_APP_TASK_INFO_H

#include <sys/types.h>
#include <string>

#include "parcel.h"

namespace OHOS {
namespace AppExecFwk {

class AppTaskInfo : public Parcelable {
public:
    /**
     * @brief Obtains the app name.
     *
     * @return Returns the app name.
     */
    const std::string &GetName() const;

    /**
     * @brief Obtains the process name.
     *
     * @return Returns the process name.
     */
    const std::string &GetProcessName() const;

    /**
     * @brief Obtains the app pid.
     *
     * @return Returns the app pid.
     */
    pid_t GetPid() const;

    /**
     * @brief Obtains the app record id.
     *
     * @return Returns app record id.
     */
    int32_t GetRecordId() const;

    /**
     * @brief Setting name for app.
     *
     * @param appName, the app name.
     */
    void SetName(const std::string &appName);

    /**
     * @brief Setting name for process.
     *
     * @param int32_t, the process name.
     */
    void SetProcessName(const std::string &processName);

    /**
     * @brief Setting pid for app.
     *
     * @param int32_t, the app pid.
     */
    void SetPid(const pid_t pid);

    /**
     * @brief Setting id for app record.
     *
     * @param int32_t, the app record id.
     */
    void SetRecordId(const int32_t appRecordId);

    /**
     * @brief read this Sequenceable object from a Parcel.
     *
     * @param inParcel Indicates the Parcel object into which the Sequenceable object has been marshaled.
     * @return Returns true if read succeeded; returns false otherwise.
     */
    bool ReadFromParcel(Parcel &parcel);

    /**
     * @brief Marshals this Sequenceable object into a Parcel.
     *
     * @param outParcel Indicates the Parcel object to which the Sequenceable object will be marshaled.
     */
    virtual bool Marshalling(Parcel &parcel) const override;

    /**
     * @brief Unmarshals this Sequenceable object from a Parcel.
     *
     * @param inParcel Indicates the Parcel object into which the Sequenceable object has been marshaled.
     */
    static AppTaskInfo *Unmarshalling(Parcel &parcel);

private:
    std::string appName_;
    std::string processName_;
    pid_t pid_ = 0;
    int32_t appRecordId_ = 0;
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_APPMGR_APP_TASK_INFO_H
