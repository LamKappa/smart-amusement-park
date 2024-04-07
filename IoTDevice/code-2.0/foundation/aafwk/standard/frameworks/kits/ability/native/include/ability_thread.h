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

#ifndef FOUNDATION_APPEXECFWK_ABILITY_THREAD_H
#define FOUNDATION_APPEXECFWK_ABILITY_THREAD_H

#include "want.h"
#include "ability_manager_client.h"
#include "ability_manager_interface.h"
#include "ability_impl.h"
#include "ability.h"
#include "ability_local_record.h"
#include "context.h"
#include "ohos_application.h"
#include "ability_scheduler_stub.h"
#include "pac_map.h"
#include "ohos/aafwk/base/ipc_singleton.h"
#include "dummy_values_bucket.h"
#include "dummy_data_ability_predicates.h"
#include "dummy_result_set.h"

namespace OHOS {
namespace AppExecFwk {
using AbilitySchedulerStub = OHOS::AAFwk::AbilitySchedulerStub;
using LifeCycleStateInfo = OHOS::AAFwk::LifeCycleStateInfo;
class AbilityImpl;
class Ability;
class AbilityHandler;
class AbilityLocalRecord;
class ApplicationImpl;
class OHOSApplication;
class AbilityHandler;
class AbilityThread : public AbilitySchedulerStub {
public:
    /**
     * @brief Default constructor used to create a AbilityThread instance.
     */
    AbilityThread();
    ~AbilityThread();

    /**
     * @description: Attach The ability thread to the main process.
     * @param application Indicates the main process.
     * @param abilityRecord Indicates the abilityRecord.
     * @param mainRunner The runner which main_thread holds.
     */
    static void AbilityThreadMain(std::shared_ptr<OHOSApplication> &application,
        const std::shared_ptr<AbilityLocalRecord> &abilityRecord, const std::shared_ptr<EventRunner> &mainRunner);

    /**
     * @description: Attach The ability thread to the main process.
     * @param application Indicates the main process.
     * @param abilityRecord Indicates the abilityRecord.
     */
    static void AbilityThreadMain(
        std::shared_ptr<OHOSApplication> &application, const std::shared_ptr<AbilityLocalRecord> &abilityRecord);

    /**
     * @description: Attach The ability thread to the main process.
     * @param application Indicates the main process.
     * @param abilityRecord Indicates the abilityRecord.
     * @param mainRunner The runner which main_thread holds.
     */
    void Attach(std::shared_ptr<OHOSApplication> &application, const std::shared_ptr<AbilityLocalRecord> &abilityRecord,
        const std::shared_ptr<EventRunner> &mainRunner);

    /**
     * @description: Attach The ability thread to the main process.
     * @param application Indicates the main process.
     * @param abilityRecord Indicates the abilityRecord.
     */
    void Attach(
        std::shared_ptr<OHOSApplication> &application, const std::shared_ptr<AbilityLocalRecord> &abilityRecord);

    /**
     * @description:  Provide operating system AbilityTransaction information to the observer
     * @param want Indicates the structure containing Transaction information about the ability.
     * @param lifeCycleStateInfo Indicates the lifecycle state.
     */
    void ScheduleAbilityTransaction(const Want &want, const LifeCycleStateInfo &targetState);

    /**
     * @description:  Provide operating system ConnectAbility information to the observer
     * @param  want Indicates the structure containing connect information about the ability.
     */
    void ScheduleConnectAbility(const Want &want);

    /**
     * @description: Provide operating system ConnectAbility information to the observer
     * @return  None
     */
    void ScheduleDisconnectAbility(const Want &want);

    /**
     * @description: Provide operating system CommandAbility information to the observer
     *
     * @param want The Want object to command to.
     *
     * * @param restart Indicates the startup mode. The value true indicates that Service is restarted after being
     * destroyed, and the value false indicates a normal startup.
     *
     * @param startId Indicates the number of times the Service ability has been started. The startId is incremented by
     * 1 every time the ability is started. For example, if the ability has been started for six times, the value of
     * startId is 6.
     */
    void ScheduleCommandAbility(const Want &want, bool restart, int startId);

    /**
     * @description: Provide operating system SaveabilityState information to the observer
     * @param state Indicates save ability state used to dispatch.
     */
    void ScheduleSaveAbilityState(PacMap &state);

    /**
     * @description:  Provide operating system RestoreAbilityState information to the observer
     * @param state Indicates resotre ability state used to dispatchRestoreAbilityState.
     */
    void ScheduleRestoreAbilityState(const PacMap &state);

    /**
     * @brief Send the result code and data to be returned by this Page ability to the caller.
     * When a Page ability is destroyed, the caller overrides the AbilitySlice#onAbilityResult(int, int, Want) method to
     * receive the result set in the current method. This method can be called only after the ability has been
     * initialized.
     *
     * @param requestCode Indicates the request code for send.
     * @param resultCode Indicates the result code returned after the ability is destroyed. You can define the result
     * code to identify an error.
     * @param want Indicates the data returned after the ability is destroyed. You can define the data returned. This
     * parameter can be null.
     */
    void SendResult(int requestCode, int resultCode, const Want &resultData);

    /**
     * @brief Obtains the MIME types of files supported.
     *
     * @param uri Indicates the path of the files to obtain.
     * @param mimeTypeFilter Indicates the MIME types of the files to obtain. This parameter cannot be null.
     *
     * @return Returns the matched MIME types. If there is no match, null is returned.
     */
    std::vector<std::string> GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter);

    /**
     * @brief Opens a file in a specified remote path.
     *
     * @param uri Indicates the path of the file to open.
     * @param mode Indicates the file open mode, which can be "r" for read-only access, "w" for write-only access
     * (erasing whatever data is currently in the file), "wt" for write access that truncates any existing file,
     * "wa" for write-only access to append to any existing data, "rw" for read and write access on any existing data,
     *  or "rwt" for read and write access that truncates any existing file.
     *
     * @return Returns the file descriptor.
     */
    int OpenFile(const Uri &uri, const std::string &mode);

    /**
     * @brief This is like openFile, open a file that need to be able to return sub-sections of files，often assets
     * inside of their .hap.
     *
     * @param uri Indicates the path of the file to open.
     * @param mode Indicates the file open mode, which can be "r" for read-only access, "w" for write-only access
     * (erasing whatever data is currently in the file), "wt" for write access that truncates any existing file,
     * "wa" for write-only access to append to any existing data, "rw" for read and write access on any existing
     * data, or "rwt" for read and write access that truncates any existing file.
     *
     * @return Returns the RawFileDescriptor object containing file descriptor.
     */
    int OpenRawFile(const Uri &uri, const std::string &mode);

    /**
     * @brief Inserts a single data record into the database.
     *
     * @param uri Indicates the path of the data to operate.
     * @param value  Indicates the data record to insert. If this parameter is null, a blank row will be inserted.
     *
     * @return Returns the index of the inserted data record.
     */
    int Insert(const Uri &uri, const ValuesBucket &value);

    /**
     * @brief Updates data records in the database.
     *
     * @param uri Indicates the path of data to update.
     * @param value Indicates the data to update. This parameter can be null.
     * @param predicates Indicates filter criteria. You should define the processing logic when this parameter is null.
     *
     * @return Returns the number of data records updated.
     */
    int Update(const Uri &uri, const ValuesBucket &value, const DataAbilityPredicates &predicates);

    /**
     * @brief Deletes one or more data records from the database.
     *
     * @param uri Indicates the path of the data to operate.
     * @param predicates Indicates filter criteria. You should define the processing logic when this parameter is null.
     *
     * @return Returns the number of data records deleted.
     */
    int Delete(const Uri &uri, const DataAbilityPredicates &predicates);

    /**
     * @brief Deletes one or more data records from the database.
     *
     * @param uri Indicates the path of data to query.
     * @param columns Indicates the columns to query. If this parameter is null, all columns are queried.
     * @param predicates Indicates filter criteria. You should define the processing logic when this parameter is null.
     *
     * @return Returns the query result.
     */
    std::shared_ptr<ResultSet> Query(
        const Uri &uri, std::vector<std::string> &columns, const DataAbilityPredicates &predicates);

    /**
     * @brief Obtains the MIME type matching the data specified by the URI of the Data ability. This method should be
     * implemented by a Data ability. Data abilities supports general data types, including text, HTML, and JPEG.
     *
     * @param uri Indicates the URI of the data.
     *
     * @return Returns the MIME type that matches the data specified by uri.
     */
    std::string GetType(const Uri &uri);

    /**
     * @brief Reloads data in the database.
     *
     * @param uri Indicates the position where the data is to reload. This parameter is mandatory.
     * @param extras Indicates the PacMap object containing the additional parameters to be passed in this call. This
     * parameter can be null. If a custom Sequenceable object is put in the PacMap object and will be transferred across
     * processes, you must call BasePacMap.setClassLoader(ClassLoader) to set a class loader for the custom object.
     *
     * @return Returns true if the data is successfully reloaded; returns false otherwise.
     */
    bool Reload(const Uri &uri, const PacMap &extras);

    /**
     * @brief Inserts multiple data records into the database.
     *
     * @param uri Indicates the path of the data to operate.
     * @param values Indicates the data records to insert.
     *
     * @return Returns the number of data records inserted.
     */
    int BatchInsert(const Uri &uri, const std::vector<ValuesBucket> &values);

private:
    /**
     * @description: Create the abilityname.
     *
     * @param abilityRecord Indicates the abilityRecord.
     *
     * @return Returns the abilityname.
     *
     */
    std::string CreateAbilityName(const std::shared_ptr<AbilityLocalRecord> &abilityRecord);

    /**
     * @description: Create and init contextDeal.
     *
     * @param application Indicates the main process.
     * @param abilityRecord Indicates the abilityRecord.
     * @param abilityObject Indicates the abilityObject.
     *
     * @return Returns the contextDeal.
     *
     */
    std::shared_ptr<ContextDeal> CreateAndInitContextDeal(std::shared_ptr<OHOSApplication> &application,
        const std::shared_ptr<AbilityLocalRecord> &abilityRecord, const std::shared_ptr<Context> &abilityObject);

    /**
     * @description:  Handle the life cycle of Ability.
     * @param want  Indicates the structure containing lifecycle information about the ability.
     * @param lifeCycleStateInfo  Indicates the lifeCycleStateInfo.
     */
    void HandleAbilityTransaction(const Want &want, const LifeCycleStateInfo &lifeCycleStateInfo);

    /**
     * @description:  Handle the current connection of Ability.
     * @param want  Indicates the structure containing connection information about the ability.
     */
    void HandleConnectAbility(const Want &want);

    /**
     * @description:  Handle the current disconnection of Ability.
     */
    void HandleDisconnectAbility(const Want &want);

    /**
     * @brief Handle the current commadn of Ability.
     *
     * @param want The Want object to command to.
     *
     * * @param restart Indicates the startup mode. The value true indicates that Service is restarted after being
     * destroyed, and the value false indicates a normal startup.
     *
     * @param startId Indicates the number of times the Service ability has been started. The startId is incremented by
     * 1 every time the ability is started. For example, if the ability has been started for six times, the value of
     * startId is 6.
     */
    void HandleCommandAbility(const Want &want, bool restart, int startId);

    /**
     * @description: Handle the SaveAbility state.
     * @param state Indicates save ability state used to dispatchSaveAbilityState.
     */
    void HandleSaveAbilityState(PacMap &state);

    /**
     * @description: Handle the restoreAbility state.
     * @param state  Indicates save ability state used to dispatchRestoreAbilityState.
     */
    void HandleRestoreAbilityState(const PacMap &state);

    std::shared_ptr<AbilityImpl> abilityImpl_ = nullptr;
    sptr<IRemoteObject> token_;
    std::shared_ptr<Ability> currentAbility_ = nullptr;
    std::shared_ptr<AbilityHandler> abilityHandler_ = nullptr;
    std::shared_ptr<EventRunner> runner_ = nullptr;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_ABILITY_THREAD_H