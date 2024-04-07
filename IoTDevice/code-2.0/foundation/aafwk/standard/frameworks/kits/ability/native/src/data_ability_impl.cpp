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

#include "data_ability_impl.h"
#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
using AbilityManagerClient = OHOS::AAFwk::AbilityManagerClient;
/**
 * @brief Handling the life cycle switching of PageAbility.
 *
 * @param want Indicates the structure containing information about the ability.
 * @param targetState The life cycle state to switch to.
 *
 */
void DataAbilityImpl::HandleAbilityTransaction(const Want &want, const AAFwk::LifeCycleStateInfo &targetState)
{
    APP_LOGI("DataAbilityImpl::sourceState:%{public}d; targetState: %{public}d; isNewWant: %{public}d",
        lifecycleState_,
        targetState.state,
        targetState.isNewWant);
    if ((lifecycleState_ == targetState.state) && !targetState.isNewWant) {
        APP_LOGE("Org lifeCycleState equals to Dst lifeCycleState.");
        return;
    }

    switch (targetState.state) {
        case AAFwk::ABILITY_STATE_ACTIVE: {
            if (lifecycleState_ == AAFwk::ABILITY_STATE_INITIAL) {
                SerUriString(targetState.caller.deviceId + "/" + targetState.caller.bundleName + "/" +
                             targetState.caller.abilityName);
                Start(want);
            } else {
                return;
            }
            break;
        }
        default: {
            APP_LOGE("DataAbilityImpl::HandleAbilityTransaction state is error");
            return;
            break;
        }
    }

    AbilityManagerClient::GetInstance()->AbilityTransitionDone(token_, targetState.state);
}

/**
 * @brief Obtains the MIME types of files supported.
 *
 * @param uri Indicates the path of the files to obtain.
 * @param mimeTypeFilter Indicates the MIME types of the files to obtain. This parameter cannot be null.
 *
 * @return Returns the matched MIME types. If there is no match, null is returned.
 */
std::vector<std::string> DataAbilityImpl::GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter)
{
    std::vector<std::string> types;
    if (ability_ == nullptr) {
        APP_LOGE("DataAbilityImpl::GetFileTypes ability_ is nullptr");
        return types;
    }

    types = ability_->GetFileTypes(uri, mimeTypeFilter);
    return types;
}

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
int DataAbilityImpl::OpenFile(const Uri &uri, const std::string &mode)
{
    int fd = -1;
    if (ability_ == nullptr) {
        APP_LOGE("DataAbilityImpl::OpenFile ability_ is nullptr");
        return fd;
    }

    fd = ability_->OpenFile(uri, mode);
    return fd;
}

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
int DataAbilityImpl::OpenRawFile(const Uri &uri, const std::string &mode)
{
    int fd = -1;
    if (ability_ == nullptr) {
        APP_LOGE("DataAbilityImpl::OpenRawFile ability_ is nullptr");
        return fd;
    }

    fd = ability_->OpenRawFile(uri, mode);
    return fd;
}

/**
 * @brief Inserts a single data record into the database.
 *
 * @param uri Indicates the path of the data to operate.
 * @param value  Indicates the data record to insert. If this parameter is null, a blank row will be inserted.
 *
 * @return Returns the index of the inserted data record.
 */
int DataAbilityImpl::Insert(const Uri &uri, const ValuesBucket &value)
{
    int index = -1;
    if (ability_ == nullptr) {
        APP_LOGE("DataAbilityImpl::Insert ability_ is nullptr");
        return index;
    }

    index = ability_->Insert(uri, value);
    return index;
}

/**
 * @brief Updates data records in the database.
 *
 * @param uri Indicates the path of data to update.
 * @param value Indicates the data to update. This parameter can be null.
 * @param predicates Indicates filter criteria. You should define the processing logic when this parameter is null.
 *
 * @return Returns the number of data records updated.
 */
int DataAbilityImpl::Update(const Uri &uri, const ValuesBucket &value, const DataAbilityPredicates &predicates)
{
    int index = -1;
    if (ability_ == nullptr) {
        APP_LOGE("DataAbilityImpl::Update ability_ is nullptr");
        return index;
    }

    index = ability_->Update(uri, value, predicates);
    return index;
}

/**
 * @brief Deletes one or more data records from the database.
 *
 * @param uri Indicates the path of the data to operate.
 * @param predicates Indicates filter criteria. You should define the processing logic when this parameter is null.
 *
 * @return Returns the number of data records deleted.
 */
int DataAbilityImpl::Delete(const Uri &uri, const DataAbilityPredicates &predicates)
{
    int index = -1;
    if (ability_ == nullptr) {
        APP_LOGE("DataAbilityImpl::Delete ability_ is nullptr");
        return index;
    }

    index = ability_->Delete(uri, predicates);
    return index;
}

/**
 * @brief Deletes one or more data records from the database.
 *
 * @param uri Indicates the path of data to query.
 * @param columns Indicates the columns to query. If this parameter is null, all columns are queried.
 * @param predicates Indicates filter criteria. You should define the processing logic when this parameter is null.
 *
 * @return Returns the query result.
 */
std::shared_ptr<ResultSet> DataAbilityImpl::Query(
    const Uri &uri, std::vector<std::string> &columns, const DataAbilityPredicates &predicates)
{
    std::shared_ptr<ResultSet> resultSet = nullptr;
    if (ability_ == nullptr) {
        APP_LOGE("DataAbilityImpl::Query ability_ is nullptr");
        return resultSet;
    }

    resultSet = ability_->Query(uri, columns, predicates);
    return resultSet;
}

/**
 * @brief Obtains the MIME type matching the data specified by the URI of the Data ability. This method should be
 * implemented by a Data ability. Data abilities supports general data types, including text, HTML, and JPEG.
 *
 * @param uri Indicates the URI of the data.
 *
 * @return Returns the MIME type that matches the data specified by uri.
 */
std::string DataAbilityImpl::GetType(const Uri &uri)
{
    std::string type;
    if (ability_ == nullptr) {
        APP_LOGE("DataAbilityImpl::GetType ability_ is nullptr");
        return type;
    }
    type = ability_->GetType(uri);
    return type;
}

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
bool DataAbilityImpl::Reload(const Uri &uri, const PacMap &extras)
{
    bool ret = false;
    if (ability_ == nullptr) {
        APP_LOGE("DataAbilityImpl::Reload ability_ is nullptr");
        return ret;
    }
    ret = ability_->Reload(uri, extras);
    return ret;
}

/**
 * @brief Inserts multiple data records into the database.
 *
 * @param uri Indicates the path of the data to operate.
 * @param values Indicates the data records to insert.
 *
 * @return Returns the number of data records inserted.
 */
int DataAbilityImpl::BatchInsert(const Uri &uri, const std::vector<ValuesBucket> &values)
{
    int ret = -1;
    if (ability_ == nullptr) {
        APP_LOGE("DataAbilityImpl::BatchInsert​ ability_ is nullptr");
        return ret;
    }
    ret = ability_->BatchInsert(uri, values);
    return ret;
}
}  // namespace AppExecFwk
}  // namespace OHOS
