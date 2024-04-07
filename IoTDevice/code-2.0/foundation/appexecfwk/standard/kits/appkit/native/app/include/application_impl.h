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

#ifndef FOUNDATION_APPEXECFWK_OHOS_APPLICATION_IMPL_H
#define FOUNDATION_APPEXECFWK_OHOS_APPLICATION_IMPL_H

#include <map>
#include <string>
#include "application_info.h"
#include "profile.h"
#include "iremote_object.h"
#include "ability_local_record.h"

namespace OHOS {
namespace AppExecFwk {
class OHOSApplication;
class AbilityLocalRecord;
class Configuration;
class ApplicationImpl {
public:
    ApplicationImpl();
    virtual ~ApplicationImpl() = default;

    /**
     * @brief Set the application to the ApplicationImpl.
     *
     * @param application The application which the mainthread launched.
     *
     */
    void SetApplication(const std::shared_ptr<OHOSApplication> &application);

    /**
     * @brief Schedule the application to the APP_STATE_READY state.
     *
     * @return Returns true if performAppReady is scheduled successfully;
     *         Returns false otherwise.
     */
    bool PerformAppReady();

    /**
     * @brief Schedule the application to the APP_STATE_FOREGROUND state.
     *
     * @return Returns true if PerformForeground is scheduled successfully;
     *         Returns false otherwise.
     */
    bool PerformForeground();

    /**
     * @brief Schedule the application to the APP_STATE_BACKGROUND state.
     *
     * @return Returns true if PerformBackground is scheduled successfully;
     *         Returns false otherwise.
     */
    bool PerformBackground();

    /**
     * @brief Schedule the application to the APP_STATE_TERMINATED state.
     *
     * @return Returns true if PerformTerminate is scheduled successfully;
     *         Returns false otherwise.
     */
    bool PerformTerminate();

    /**
     * @brief Schedule the application to the APP_STATE_TERMINATED state.
     *
     * @return Returns true if PerformTerminate is scheduled successfully;
     *         Returns false otherwise.
     */
    void PerformTerminateStrong();

    /**
     * @brief Set the target state to application.
     *
     * @param state The target state of application.
     *
     */
    int SetState(int state);

    /**
     * @brief Get the current state of application.
     *
     * @return Returns the current state of application.
     *
     */
    int GetState() const;

    /**
     * @brief Set the RecordId to application.
     *
     * @param id recordId.
     *
     */
    void SetRecordId(int id);

    /**
     * @brief Get the recordId of application.
     *
     * @return Returns the recordId of application.
     *
     */
    int GetRecordId() const;

    /**
     * @brief System determines to trim the memory.
     *
     * @param level Indicates the memory trim level, which shows the current memory usage status.
     *
     */
    void PerformMemoryLevel(int level);

    /**
     * @brief System determines to send the new config to application.
     *
     * @param config Indicates the updated configuration information.
     *
     */
    void PerformConfigurationUpdated(const Configuration &config);

private:
    enum {
        APP_STATE_CREATE = 0,
        APP_STATE_READY = 1,
        APP_STATE_FOREGROUND = 2,
        APP_STATE_BACKGROUND = 3,
        APP_STATE_TERMINATED = 4
    };
    int curState_;
    int recordId_;
    std::shared_ptr<OHOSApplication> application_ = nullptr;

    DISALLOW_COPY_AND_MOVE(ApplicationImpl);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_OHOS_APPLICATION_IMPL_H
