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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_PRIORITY_OBJECT_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_PRIORITY_OBJECT_H

#include <string>
#include <unistd.h>

#include "nocopyable.h"
#include "parcel.h"

namespace OHOS {

namespace AppExecFwk {

class PriorityObject : public Parcelable {
public:
    PriorityObject() = default;
    virtual ~PriorityObject() = default;

    /**
     * @brief Obtains the app pid.
     *
     * @return Returns the app pid.
     */
    pid_t GetPid() const;

    /**
     * @brief Obtains the app max adj.
     *
     * @return Returns the app max adj.
     */
    int32_t GetMaxAdj() const;

    /**
     * @brief Obtains the app cur adj.
     *
     * @return Returns the app cur adj.
     */
    int32_t GetCurAdj() const;

    /**
     * @brief Obtains the app cur cgroup.
     *
     * @return Returns the app cur cgroup.
     */
    int32_t GetCurCgroup() const;

    /**
     * @brief Obtains the app time level.
     *
     * @return Returns the app time level.
     */
    int32_t GetTimeLevel() const;

    /**
     * @brief Obtains the ability visible status.
     *
     * @return Returns the ability visible status.
     */
    bool GetVisibleStatus() const;

    /**
     * @brief Obtains the ability preceptible status.
     *
     * @return Returns the ability preceptible status.
     */
    bool GetPerceptibleStatus() const;

    /**
     * @brief Setting pid for app.
     *
     * @param appName, the app pid.
     */
    void SetPid(const pid_t pid);

    /**
     * @brief Setting max adj for app.
     *
     * @param appName, the app max adj.
     */
    void SetMaxAdj(const int32_t maxAdj);

    /**
     * @brief Setting cur adj for app.
     *
     * @param appName, the app cur adj.
     */
    void SetCurAdj(const int32_t curAdj);

    /**
     * @brief Setting cur cgroup for app.
     *
     * @param appName, the app cur cgroup.
     */
    void SetCurCgroup(const int32_t curCgroup);

    /**
     * @brief Setting time level for app.
     *
     * @param appName, the app time level.
     */
    void SetTimeLevel(const int32_t timeLevel);

    /**
     * @brief Setting visible status for ability.
     *
     * @param status, the visible status.
     */
    void SetVisibleStatus(bool status);

    /**
     * @brief Setting perceptible status for ability.
     *
     * @param status, the perceptible status.
     */
    void SetPerceptibleStatus(bool status);

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
    static PriorityObject *Unmarshalling(Parcel &parcel);

private:
    pid_t pid_ = 0;
    int32_t maxAdj_ = 0;
    int32_t curAdj_ = 0;
    int32_t curCgroup_ = 0;
    int32_t timeLevel_ = 0;
    bool visibleStatus_ = false;
    bool perceptibleStatus_ = false;
};

}  // namespace AppExecFwk
}  // namespace OHOS

#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_PRIORITY_OBJECT_H