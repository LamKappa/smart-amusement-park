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

#ifndef FOUNDATION_APPEXECFWK_OHOS_DATA_ABILITY_HELPER_H
#define FOUNDATION_APPEXECFWK_OHOS_DATA_ABILITY_HELPER_H

#include "context.h"
#include "dummy_values_bucket.h"
#include "dummy_data_ability_predicates.h"
#include "dummy_result_set.h"
#include "uri.h"

using Uri = OHOS::Uri;

namespace OHOS {
namespace AppExecFwk {
using string = std::string;
class PacMap;
class DataAbilityHelper final : public std::enable_shared_from_this<DataAbilityHelper> {
public:
    ~DataAbilityHelper() = default;

    /**
     * @brief Creates a DataAbilityHelper instance without specifying the Uri based on the given Context.
     *
     * @param context Indicates the Context object on OHOS.
     *
     * @return Returns the created DataAbilityHelper instance where Uri is not specified.
     */
    static std::shared_ptr<DataAbilityHelper> Creator(const std::shared_ptr<Context> &context);

    /**
     * @brief Creates a DataAbilityHelper instance with the Uri specified based on the given Context.
     *
     * @param context Indicates the Context object on OHOS.
     * @param uri Indicates the database table or disk file to operate.
     *
     * @return Returns the created DataAbilityHelper instance with a specified Uri.
     */
    static std::shared_ptr<DataAbilityHelper> Creator(
        const std::shared_ptr<Context> &context, const std::shared_ptr<Uri> &uri);

    /**
     * @brief You can use this method to specify the Uri of the data to operate and set the binding relationship
     * between the ability using the Data template (Data ability for short) and the associated client process in
     * a DataAbilityHelper instance.
     *
     * @param context Indicates the Context object on OHOS.
     * @param uri Indicates the database table or disk file to operate.
     * @param tryBind Specifies whether the exit of the corresponding Data ability process causes the exit of the
     * client process.
     *
     * @return Returns the created DataAbilityHelper instance.
     */
    static std::shared_ptr<DataAbilityHelper> Creator(
        const std::shared_ptr<Context> &context, const std::shared_ptr<Uri> &uri, const bool tryBind);

    /**
     * @brief Releases the client resource of the Data ability.
     * You should call this method to releases client resource after the data operations are complete.
     *
     * @return Returns true if the resource is successfully released; returns false otherwise.
     */
    bool Release();
    /**
     * @brief Obtains the MIME types of files supported.
     *
     * @param uri Indicates the path of the files to obtain.
     * @param mimeTypeFilter Indicates the MIME types of the files to obtain. This parameter cannot be null.
     *
     * @return Returns the matched MIME types. If there is no match, null is returned.
     */
    std::vector<std::string> GetFileTypes(Uri &uri, const std::string &mimeTypeFilter);

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
    int OpenFile(Uri &uri, const std::string &mode);

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
    int OpenRawFile(Uri &uri, const std::string &mode);

    /**
     * @brief Inserts a single data record into the database.
     *
     * @param uri Indicates the path of the data to operate.
     * @param value  Indicates the data record to insert. If this parameter is null, a blank row will be inserted.
     *
     * @return Returns the index of the inserted data record.
     */
    int Insert(Uri &uri, const ValuesBucket &value);

    /**
     * @brief Updates data records in the database.
     *
     * @param uri Indicates the path of data to update.
     * @param value Indicates the data to update. This parameter can be null.
     * @param predicates Indicates filter criteria. You should define the processing logic when this parameter is null.
     *
     * @return Returns the number of data records updated.
     */
    int Update(Uri &uri, const ValuesBucket &value, const DataAbilityPredicates &predicates);

    /**
     * @brief Deletes one or more data records from the database.
     *
     * @param uri Indicates the path of the data to operate.
     * @param predicates Indicates filter criteria. You should define the processing logic when this parameter is null.
     *
     * @return Returns the number of data records deleted.
     */
    int Delete(Uri &uri, const DataAbilityPredicates &predicates);

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
        Uri &uri, std::vector<std::string> &columns, const DataAbilityPredicates &predicates);

    /**
     * @brief Obtains the MIME type matching the data specified by the URI of the Data ability. This method should be
     * implemented by a Data ability. Data abilities supports general data types, including text, HTML, and JPEG.
     *
     * @param uri Indicates the URI of the data.
     *
     * @return Returns the MIME type that matches the data specified by uri.
     */
    std::string GetType(Uri &uri);

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
    bool Reload(Uri &uri, const PacMap &extras);

    /**
     * @brief Inserts multiple data records into the database.
     *
     * @param uri Indicates the path of the data to operate.
     * @param values Indicates the data records to insert.
     *
     * @return Returns the number of data records inserted.
     */
    int BatchInsert(Uri &uri, const std::vector<ValuesBucket> &values);

private:
    DataAbilityHelper(const std::shared_ptr<Context> &context, const std::shared_ptr<Uri> &uri,
        const sptr<AAFwk::IAbilityScheduler> &dataAbilityProxy, bool tryBind = false);
    DataAbilityHelper(const std::shared_ptr<Context> &context);

    sptr<IRemoteObject> token_;
    std::weak_ptr<Context> context_;
    std::shared_ptr<Uri> uri_ = nullptr;
    bool tryBind_ = false;
    sptr<AAFwk::IAbilityScheduler> dataAbilityProxy_ = nullptr;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_OHOS_DATA_ABILITY_HELPER_H