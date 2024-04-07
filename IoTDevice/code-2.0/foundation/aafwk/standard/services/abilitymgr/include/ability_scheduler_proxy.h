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

#ifndef OHOS_AAFWK_ABILITY_SCHEDULE_PROXY_H
#define OHOS_AAFWK_ABILITY_SCHEDULE_PROXY_H

#include "ability_scheduler_interface.h"

#include <iremote_proxy.h>

namespace OHOS {
namespace AAFwk {
/**
 * @class AbilitySchedulerProxy
 * AbilityScheduler proxy.
 */
class AbilitySchedulerProxy : public IRemoteProxy<IAbilityScheduler> {
public:
    explicit AbilitySchedulerProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IAbilityScheduler>(impl)
    {}

    virtual ~AbilitySchedulerProxy()
    {}

    /*
     * ScheduleAbilityTransaction,  schedule ability to transform life state.
     *
     * @param Want, Special Want for service type's ability.
     * @param targetState, The lifecycle state to be transformed
     */
    void ScheduleAbilityTransaction(const Want &want, const LifeCycleStateInfo &targetState) override;

    /*
     * SendResult, Send result to app when ability is terminated with result want.
     *
     * @param requestCode, the requestCode of the ability to start.
     * @param resultCode, the resultCode of the ability to terminate.
     * @param resultWant, the want of the ability to terminate.
     */
    void SendResult(int requestCode, int resultCode, const Want &resultWant) override;

    /*
     * ScheduleConnectAbility,  schedule service ability to connect.
     *
     * @param Want, Special Want for service type's ability.
     */
    void ScheduleConnectAbility(const Want &want) override;

    /*
     * ScheduleDisconnectAbility, schedule service ability to disconnect.
     */
    void ScheduleDisconnectAbility(const Want &want) override;

    /*
     * ScheduleCommandAbility, schedule service ability to command.
     */
    void ScheduleCommandAbility(const Want &want, bool restart, int startId) override;

    /*
     * ScheduleSaveAbilityState, scheduling save ability state.
     */
    void ScheduleSaveAbilityState(PacMap &outState) override;

    /*
     * ScheduleRestoreAbilityState, scheduling restore ability state.
     */
    void ScheduleRestoreAbilityState(const PacMap &inState) override;

    /**
     * @brief Obtains the MIME types of files supported.
     *
     * @param uri Indicates the path of the files to obtain.
     * @param mimeTypeFilter Indicates the MIME types of the files to obtain. This parameter cannot be null.
     *
     * @return Returns the matched MIME types. If there is no match, null is returned.
     */
    virtual std::vector<std::string> GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter) override;

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
    virtual int OpenFile(const Uri &uri, const std::string &mode) override;

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
    virtual int OpenRawFile(const Uri &uri, const std::string &mode) override;

    /**
     * @brief Inserts a single data record into the database.
     *
     * @param uri Indicates the path of the data to operate.
     * @param value  Indicates the data record to insert. If this parameter is null, a blank row will be inserted.
     *
     * @return Returns the index of the inserted data record.
     */
    virtual int Insert(const Uri &uri, const ValuesBucket &value) override;

    /**
     * @brief Updates data records in the database.
     *
     * @param uri Indicates the path of data to update.
     * @param value Indicates the data to update. This parameter can be null.
     * @param predicates Indicates filter criteria. You should define the processing logic when this parameter is null.
     *
     * @return Returns the number of data records updated.
     */
    virtual int Update(const Uri &uri, const ValuesBucket &value, const DataAbilityPredicates &predicates) override;

    /**
     * @brief Deletes one or more data records from the database.
     *
     * @param uri Indicates the path of the data to operate.
     * @param predicates Indicates filter criteria. You should define the processing logic when this parameter is null.
     *
     * @return Returns the number of data records deleted.
     */
    virtual int Delete(const Uri &uri, const DataAbilityPredicates &predicates) override;

    /**
     * @brief Deletes one or more data records from the database.
     *
     * @param uri Indicates the path of data to query.
     * @param columns Indicates the columns to query. If this parameter is null, all columns are queried.
     * @param predicates Indicates filter criteria. You should define the processing logic when this parameter is null.
     *
     * @return Returns the query result.
     */
    virtual std::shared_ptr<ResultSet> Query(
        const Uri &uri, std::vector<std::string> &columns, const DataAbilityPredicates &predicates) override;

    /**
     * @brief Obtains the MIME type matching the data specified by the URI of the Data ability. This method should be
     * implemented by a Data ability. Data abilities supports general data types, including text, HTML, and JPEG.
     *
     * @param uri Indicates the URI of the data.
     *
     * @return Returns the MIME type that matches the data specified by uri.
     */
    std::string GetType(const Uri &uri) override;

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
    bool Reload(const Uri &uri, const PacMap &extras) override;

    /**
     * @brief Inserts multiple data records into the database.
     *
     * @param uri Indicates the path of the data to operate.
     * @param values Indicates the data records to insert.
     *
     * @return Returns the number of data records inserted.
     */
    int BatchInsert(const Uri &uri, const std::vector<ValuesBucket> &values) override;

private:
    bool WriteInterfaceToken(MessageParcel &data);

private:
    static inline BrokerDelegator<AbilitySchedulerProxy> delegator_;
};
}  // namespace AAFwk
}  // namespace OHOS
#endif  // OHOS_AAFWK_ABILITY_SCHEDULE_PROXY_H
