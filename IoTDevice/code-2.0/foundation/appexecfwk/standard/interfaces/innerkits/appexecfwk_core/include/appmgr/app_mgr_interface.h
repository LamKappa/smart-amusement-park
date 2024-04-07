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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_APPMGR_APP_MGR_INTERFACE_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_APPMGR_APP_MGR_INTERFACE_H

#include "iremote_broker.h"
#include "iremote_object.h"

#include "ability_info.h"
#include "application_info.h"
#include "app_record_id.h"
#include "iapp_state_callback.h"
#include "ams_mgr_interface.h"
#include "running_process_info.h"

namespace OHOS {
namespace AppExecFwk {

class IAppMgr : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.appexecfwk.AppMgr");

    /**
     * AttachApplication, call AttachApplication() through proxy object,
     * get all the information needed to start the Application (data related to the Application ).
     *
     * @param app, information needed to start the Application.
     * @return
     */
    virtual void AttachApplication(const sptr<IRemoteObject> &app) = 0;

    /**
     * ApplicationForegrounded, call ApplicationForegrounded() through proxy object,
     * set the application to Foreground State.
     *
     * @param recordId, a unique record that identifies this Application from others.
     * @return
     */
    virtual void ApplicationForegrounded(const int32_t recordId) = 0;

    /**
     * ApplicationBackgrounded, call ApplicationBackgrounded() through proxy object,
     * set the application to Backgrounded State.
     *
     * @param recordId, a unique record that identifies this Application from others.
     * @return
     */
    virtual void ApplicationBackgrounded(const int32_t recordId) = 0;

    /**
     * ApplicationTerminated, call ApplicationTerminated() through proxy object,
     * terminate the application.
     *
     * @param recordId, a unique record that identifies this Application from others.
     * @return
     */
    virtual void ApplicationTerminated(const int32_t recordId) = 0;

    /**
     * CheckPermission, call CheckPermission() through proxy object, check the permission.
     *
     * @param recordId, a unique record that identifies this Application from others.
     * @param permission, check the permissions.
     * @return ERR_OK, return back success, others fail.
     */
    virtual int CheckPermission(const int32_t recordId, const std::string &permission) = 0;

    /**
     * AbilityCleaned,call through AbilityCleaned() proxy project, clean Ability record.
     *
     * @param token, a unique record that identifies AbilityCleaned from others.
     * @return
     */
    virtual void AbilityCleaned(const sptr<IRemoteObject> &token) = 0;

    /**
     * GetAmsMgr, call GetAmsMgr() through proxy object, get AMS interface instance.
     *
     * @return sptr<IAmsMgr>, return to AMS interface instance.
     */
    virtual sptr<IAmsMgr> GetAmsMgr() = 0;

    /**
     * ClearUpApplicationData, call ClearUpApplicationData() through proxy project,
     * clear the application data.
     *
     * @param bundleName, bundle name in Application record.
     * @return
     */
    virtual void ClearUpApplicationData(const std::string &bundleName) = 0;

    /**
     * IsBackgroundRunningRestricted, call IsBackgroundRunningRestricted() through proxy project,
     * Checks whether the process of this application is forbidden to run in the background.
     *
     * @param bundleName, bundle name in Application record.
     * @return ERR_OK, return back success, others fail.
     */
    virtual int IsBackgroundRunningRestricted(const std::string &bundleName) = 0;

    /**
     * GetAllRunningProcesses, call GetAllRunningProcesses() through proxy project.
     * Obtains information about application processes that are running on the device.
     *
     * @param runningProcessInfo, app name in Application record.
     * @return ERR_OK ,return back success，others fail.
     */
    virtual int GetAllRunningProcesses(std::shared_ptr<RunningProcessInfo> &runningProcessInfo) = 0;

    enum class Message {
        AMS_APP_ATTACH_APPLICATION = 0,
        AMS_APP_APPLICATION_FOREGROUNDED,
        AMS_APP_APPLICATION_BACKGROUNDED,
        AMS_APP_APPLICATION_TERMINATED,
        AMS_APP_CHECK_PERMISSION,
        AMS_APP_ABILITY_CLEANED,
        AMS_APP_GET_MGR_INSTANCE,
        AMS_APP_CLEAR_UP_APPLICATION_DATA,
        AMS_APP_IS_BACKGROUND_RUNNING_RESTRICTED,
        AMS_APP_GET_ALL_RUNNING_PROCESSES,
    };
};

}  // namespace AppExecFwk
}  // namespace OHOS

#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_APPMGR_APP_MGR_INTERFACE_H
