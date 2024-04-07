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

#ifndef OHOS_SYSTEM_TEST_ABILITY_UTIL_H
#define OHOS_SYSTEM_TEST_ABILITY_UTIL_H
#include "event.h"
#include "stoperator.h"
#include "ability_manager_service.h"
#include "ability_manager_errors.h"
#include "app_mgr_service.h"
#include "hilog_wrapper.h"
#include "module_test_dump_util.h"
#include "sa_mgr_client.h"
#include "system_ability_definition.h"
#include "common_event.h"
#include "common_event_manager.h"
#include <memory>
#include <mutex>
#include <cstdio>
#include <thread>
#include <chrono>

namespace OHOS {
namespace STABUtil {

namespace {
using vector_str = std::vector<std::string>;
using MAP_STR_STR = std::map<std::string, std::string>;
}  // namespace
class STAbilityUtil {

public:
    static std::shared_ptr<STAbilityUtil> GetInstance();
    static void DestroyInstance();
    ~STAbilityUtil() = default;

    /**
     *
     * @param  {string} eventName                  : Sent Event Name
     * @param  {int} code                  : Sent Code
     * @param  {string} data                  : Sent Data
     * @Introduction: Sent Event
     */
    static bool PublishEvent(const std::string &eventName, const int &code, const std::string &data);

    /**
     *
     * @param  {string} hapName                  : Hap File Vame
     * @Introduction: use bm install xxx.hap
     */
    static void Install(const std::string &hapName);

    /**
     *
     * @param  {vector<string>} hapNames                  : container containing multiple hap names
     * @Introduction: use bm install multiple hap
     */
    static void InstallHaps(vector_str &hapNames);

    /**
     *
     * @param  {string} bundleName                  : bundleName
     * @Introduction: use bm uninstall bundleName
     */
    static void Uninstall(const std::string &bundleName);

    /**
     *
     * @param  {vector<string>} bundleNames                  : container containing multiple bundle names
     * @Introduction: use bm install multiple bundle
     */
    static void UninstallBundle(vector_str &bundleNames);

    /**
     *
     * @param  {string} serviceName                  : process name
     * @Introduction: kill process
     */
    static void KillService(const std::string &serviceName);

    /**
     *
     * @param  {string} serviceName                  : executable file name
     * @param  {time_t} delay                  : Waiting time for executable to start(milliseconds)
     * @Introduction: start executable file
     */
    static void StartService(const std::string &serviceName, const time_t &delay = 0);

    /**
     *
     * @param  {vector<string>} bundleNames                  : Container Containing Multiple Bundle Names
     * @Introduction: start executable file
     */
    static void KillBundleProcess(vector_str &bundleNames);

    /**
     *
     * @Introduction: Get Ability manager Service.
     */
    static sptr<AAFwk::IAbilityManager> GetAbilityManagerService();

    /**
     *
     * @Introduction: Get App manager Service.
     */
    static sptr<AppExecFwk::IAppMgr> GetAppMgrService();

    /**
     *
     * @param  {Want} want                  : The want of the ability to start.
     * @param  {sptr<AAFwk::IAbilityManager>} abilityMs                  : Ability Manager Service ptr
     * @param  {time_t} delay                  : Waiting time for ability to start (milliseconds)
     * @Introduction: start ability
     */
    static ErrCode StartAbility(
        const AAFwk::Want &want, sptr<AAFwk::IAbilityManager> &abilityMs, const time_t &delay = 0);

    /**
     *
     * @param  {string} eventName                  : Sent Event Name
     * @param  {int} code                  : Sent Code
     * @param  {string} data                  : Sent Data
     * @Introduction: Sent Event to terminate app
     */
    static bool StopAbility(const std::string &eventName, const int &code, const std::string &data);
    /**
     *
     * @param  {string} deviceId                  : Device ID
     * @param  {string} abilityName                  : Ability Name
     * @param  {string} bundleName                  : Bundle Name
     * @param  {MAP_STR_STR} params                  : Params (SetParam)
     * @Introduction: Great Want
     */
    static AAFwk::Want MakeWant(
        std::string deviceId, std::string abilityName, std::string bundleName, MAP_STR_STR params = {});

    /**
     *
     * @param  {string} deviceId                  : Device ID
     * @param  {string} abilityName                  : Ability Name
     * @param  {string} bundleName                  : Bundle Name
     * @param  {vector_str} params                  : Params (SetParam)
     * @Introduction: Great Want
     */
    static AAFwk::Want MakeWant(
        std::string deviceId, std::string abilityName, std::string bundleName, vector_str params = {});

    /**
     *
     * @param  {int64_t} id                  : Ability Record ID
     * @param  {sptr<AAFwk::IAbilityManager>} abilityMs                  : Ability Manager Service ptr
     * @Introduction: Get Top AbilityRecord ID
     */
    static ErrCode GetTopAbilityRecordId(int64_t &id, sptr<AAFwk::IAbilityManager> &abilityMs);

    /**
     *
     * @param  {shared_ptr<RunningProcessInfo>} runningProcessInfo                  : Process Info
     * @param  {sptr<AppExecFwk::IAppMgr>} appMs                  : App Manager ptr
     * @param  {time_t} delay                  : Waiting time for ability to Get Process Info (milliseconds)
     * @Introduction: Get Top AbilityRecord ID
     */
    static ErrCode GetRunningProcessInfo(std::shared_ptr<AppExecFwk::RunningProcessInfo> &runningProcessInfo,
        sptr<AppExecFwk::IAppMgr> &appMs, const time_t &delay = 0);

    /**
     *
     * @param  {string} appName                  : app Name
     * @param  {sptr<AppExecFwk::IAppMgr>} appMs                  : App Manager ptr
     * @param  {time_t} delay                  : Waiting time for ability to KillApplication (milliseconds)
     * @Introduction: Get Top AbilityRecord ID
     */
    static ErrCode KillApplication(
        const std::string &appName, sptr<AppExecFwk::IAppMgr> &appMs, const time_t &delay = 0);

    /**
     *
     * @param  {string} processName                  : processName
     * @param  {sptr<AppExecFwk::IAppMgr>} appMs                  : App Manager ptr
     * @param  {time_t} delay                  : Waiting time for ability to GetAppProcessInfo (milliseconds)
     * @Introduction: Get Top AbilityRecord ID
     */
    static AppExecFwk::AppProcessInfo GetAppProcessInfoByName(
        const std::string &processName, sptr<AppExecFwk::IAppMgr> &appMs, const time_t &delay = 0);

    /**
     *
     * @param  {Event} event                   : Event Class Object
     * @param  {string} eventName                  : The name of the event to wait for.
     * @param  {int} code                  : The code of the event to wait for.
     * @param  {int} timeout                  : Time of wait (seconds)
     * @Introduction: Judge whether the event is received in the event queue, if not, wait
     */
    static int WaitCompleted(
        STtools::Event &event, const std::string &eventName, const int code, const int timeout = 10);

    /**
     *
     * @param  {Event} event                   : Event Class Object
     * @param  {string} eventName                  : The name of the event to wait for.
     * @param  {int} code                  : The code of the event to wait for.
     * @Introduction: Compare the received events, if not, join the event queue.
     */
    static void Completed(STtools::Event &event, const std::string &eventName, const int code);

    /**
     *
     * @param  {Event} event                   : Event Class Object
     * @param  {string} eventName                  : The name of the event to wait for.
     * @param  {int} code                  : The code of the event to wait for.
     * @param  {string} data                  : The data of the event to wait for.
     * @Introduction: Compare the received events, if not, join the event queue.
     */
    static void Completed(STtools::Event &event, const std::string &eventName, const int code, const std::string &data);

    /**
     *
     * @param  {Event} event                   : Event Class Object
     * @Introduction: Clean wait event.
     */
    static void CleanMsg(STtools::Event &event);

    /**
     *
     * @param  {Event} event                   : Event Class Object
     * @param  {string} eventName                  : The name of the event to wait for.
     * @param  {int} code                  : The code of the event to wait for.
     * @Introduction: get the event data.
     */
    static std::string GetData(STtools::Event &event, const std::string &eventName, const int code);

    /**
     *
     * @param  {StOperator} ParentOperator                   : StOperator Class Object
     * @Introduction:  Serialization StOperator Class Object To Vector
     */
    static std::vector<std::string> SerializationStOperatorToVector(STtools::StOperator &ParentOperator);

    /**
     *
     * @param  {StOperator} ParentOperator                   : StOperator Class Object
     * @param  {vector<string>} vectorOperator                   : Data Source Resolved Into StOperator Class Object
     * @Introduction: Deserialization StOperator Class Object From Vector
     */
    static void DeserializationStOperatorFromVector(
        STtools::StOperator &ParentOperator, std::vector<std::string> &vectorOperator);

    /**
     *
     * @param  {Want} want                  : The want of the ability to start.
     * @param  {sptr<IAbilityConnection>} connect                  : Callback
     * @param  {time_t} delay                  : Waiting time for ability to start (milliseconds)
     * @Introduction: connect ability
     */
    static ErrCode ConnectAbility(const AAFwk::Want &want, const sptr<AAFwk::IAbilityConnection> &connect,
        const sptr<IRemoteObject> &callerToken, unsigned int usec = 0);

    /**
     *
     * @param  {Want} want                  : The want of the ability to start.
     * @param  {sptr<IAbilityConnection>} connect                  : Callback
     * @param  {time_t} delay                  : Waiting time for ability to start (milliseconds)
     * @Introduction: disconnect ability
     */
    static ErrCode DisconnectAbility(const sptr<AAFwk::IAbilityConnection> &connect, unsigned int usec = 0);

    /**
     *
     * @param  {Want} want                  : The want of the ability to start.
     * @param  {time_t} delay                  : Waiting time for ability to start (milliseconds)
     * @Introduction: stop service ability
     */
    static ErrCode StopServiceAbility(const AAFwk::Want &want, unsigned int usec = 0);

private:
    /**
     *
     * @param  {vector<string>} vectorOperator                   : StOperator Class Object Info Save in vectorOperator
     * @param  {StOperator} ParentOperator                   : StOperator Class Object
     * @Introduction: push StOperator Class Object In Vector
     */
    static void PushOperatorInVector(std::vector<std::string> &vectorOperator, STtools::StOperator &ParentOperator);

    /**
     *
     * @param  {StOperator} ParentOperator                   : StOperator Class Object
     * @param  {vector<string>} vectorOperator                   : Data Source Resolved Into StOperator Class Object
     * @Introduction: Pull StOperator Class Object From Vector
     */
    STAbilityUtil() = default;
    static void PullOperatorFromVector(STtools::StOperator &ParentOperator, std::vector<std::string> &vectorOperator);
    static std::shared_ptr<STAbilityUtil> instance_;
    static std::mutex mutex_;
};

}  // namespace STABUtil
}  // namespace OHOS
#endif  // OHOS_SYSTEM_TEST_ABILITY_UTIL_H