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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_APPMGR_PROCESS_INFO_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_APPMGR_PROCESS_INFO_H

#include <string>
#include <unistd.h>

#include "nocopyable.h"
#include "parcel.h"

namespace OHOS {
namespace AppExecFwk {

class ProcessInfo : public Parcelable {
public:
    ProcessInfo() = default;
    explicit ProcessInfo(const std::string &name, const pid_t &pid);
    virtual ~ProcessInfo() = default;

    /**
     * @brief Obtains the name of the current process.
     *
     * @return Returns the current process name.
     */
    inline const std::string &GetProcessName() const
    {
        return processName_;
    }

    /**
     * @brief Obtains the id of the current process.
     *
     * @return Returns the current process id.
     */
    inline pid_t GetPid() const
    {
        return pid_;
    }

    /**
     * @brief read this Sequenceable object from a Parcel.
     *
     * @param inParcel Indicates the Parcel object into which the Sequenceable object has been marshaled.
     * @return Returns true if read successed; returns false otherwise.
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
    static ProcessInfo *Unmarshalling(Parcel &parcel);

private:
    std::string processName_;
    pid_t pid_ = 0;
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_APPMGR_PROCESS_INFO_H
