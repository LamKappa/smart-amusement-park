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

#include "data_ability_helper.h"
#include "ability_thread.h"
#include "ability_scheduler_interface.h"
#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
std::string SchemeOhos = "dataability";
using IAbilityScheduler = OHOS::AAFwk::IAbilityScheduler;
using AbilityManagerClient = OHOS::AAFwk::AbilityManagerClient;
DataAbilityHelper::DataAbilityHelper(const std::shared_ptr<Context> &context, const std::shared_ptr<Uri> &uri,
    const sptr<IAbilityScheduler> &dataAbilityProxy, bool tryBind)
{
    token_ = context->GetToken();
    context_ = std::weak_ptr<Context>(context);
    uri_ = uri;
    tryBind_ = tryBind;
    dataAbilityProxy_ = dataAbilityProxy;
}

DataAbilityHelper::DataAbilityHelper(const std::shared_ptr<Context> &context)
{
    token_ = context->GetToken();
    context_ = std::weak_ptr<Context>(context);
}

/**
 * @brief Creates a DataAbilityHelper instance without specifying the Uri based on the given Context.
 *
 * @param context Indicates the Context object on OHOS.
 *
 * @return Returns the created DataAbilityHelper instance where Uri is not specified.
 */
std::shared_ptr<DataAbilityHelper> DataAbilityHelper::Creator(const std::shared_ptr<Context> &context)
{
    if (context == nullptr) {
        APP_LOGE("DataAbilityHelper::Creator (context, uri, tryBind) failed, context == nullptr");
        return nullptr;
    }

    DataAbilityHelper *ptrDataAbilityHelper = new (std::nothrow) DataAbilityHelper(context);
    if (ptrDataAbilityHelper == nullptr) {
        APP_LOGE("DataAbilityHelper::Creator (context) failed, create DataAbilityHelper failed");
        return nullptr;
    }

    return std::shared_ptr<DataAbilityHelper>(ptrDataAbilityHelper);
}

/**
 * @brief Creates a DataAbilityHelper instance with the Uri specified based on the given Context.
 *
 * @param context Indicates the Context object on OHOS.
 * @param uri Indicates the database table or disk file to operate.
 *
 * @return Returns the created DataAbilityHelper instance with a specified Uri.
 */
std::shared_ptr<DataAbilityHelper> DataAbilityHelper::Creator(
    const std::shared_ptr<Context> &context, const std::shared_ptr<Uri> &uri)
{
    return DataAbilityHelper::Creator(context, uri, false);
}

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
std::shared_ptr<DataAbilityHelper> DataAbilityHelper::Creator(
    const std::shared_ptr<Context> &context, const std::shared_ptr<Uri> &uri, const bool tryBind)
{
    if (context == nullptr) {
        APP_LOGE("DataAbilityHelper::Creator (context, uri, tryBind) failed, context == nullptr");
        return nullptr;
    }

    if (uri == nullptr) {
        APP_LOGE("DataAbilityHelper::Creator (context, uri, tryBind) failed, uri == nullptr");
        return nullptr;
    }

    if (uri->GetScheme() != SchemeOhos) {
        APP_LOGE("DataAbilityHelper::Creator (context, uri, tryBind) failed, the Scheme is not dataability, Scheme: "
                 "%{public}s",
            uri->GetScheme().c_str());
        return nullptr;
    }

    sptr<IAbilityScheduler> dataAbilityProxy =
        AbilityManagerClient::GetInstance()->AcquireDataAbility(*uri.get(), tryBind, context->GetToken());
    if (dataAbilityProxy == nullptr) {
        APP_LOGE("DataAbilityHelper::Creator failed get dataAbilityProxy");
        return nullptr;
    }

    DataAbilityHelper *ptrDataAbilityHelper =
        new (std::nothrow) DataAbilityHelper(context, uri, dataAbilityProxy, tryBind);
    if (ptrDataAbilityHelper == nullptr) {
        APP_LOGE("DataAbilityHelper::Creator (context, uri, tryBind) failed, create DataAbilityHelper failed");
        return nullptr;
    }

    return std::shared_ptr<DataAbilityHelper>(ptrDataAbilityHelper);
}

/**
 * @brief Releases the client resource of the Data ability.
 * You should call this method to releases client resource after the data operations are complete.
 *
 * @return Returns true if the resource is successfully released; returns false otherwise.
 */
bool DataAbilityHelper::Release()
{
    if (uri_ == nullptr) {
        APP_LOGE("DataAbilityHelper::Release failed, uri_ is nullptr");
        return false;
    }

    int err = AbilityManagerClient::GetInstance()->ReleaseDataAbility(dataAbilityProxy_, token_);
    if (err != ERR_OK) {
        APP_LOGE("DataAbilityHelper::GetFileTypes failed to ReleaseDataAbility err = %{public}d", err);
        return false;
    }

    return true;
}

/**
 * @brief Obtains the MIME types of files supported.
 *
 * @param uri Indicates the path of the files to obtain.
 * @param mimeTypeFilter Indicates the MIME types of the files to obtain. This parameter cannot be null.
 *
 * @return Returns the matched MIME types. If there is no match, null is returned.
 */
std::vector<std::string> DataAbilityHelper::GetFileTypes(Uri &uri, const std::string &mimeTypeFilter)
{
    std::vector<std::string> matchedMIMEs;
    if ((uri_ == nullptr) || (!uri_->Equals(uri))) {
        if (uri.GetScheme() == SchemeOhos) {
            sptr<IAbilityScheduler> dataAbilityProxy =
                AbilityManagerClient::GetInstance()->AcquireDataAbility(uri, tryBind_, token_);
            if (dataAbilityProxy == nullptr) {
                APP_LOGE("DataAbilityHelper::GetFileTypes failed dataAbility == nullptr");
                return matchedMIMEs;
            }

            matchedMIMEs = dataAbilityProxy->GetFileTypes(uri, mimeTypeFilter);

            int err = AbilityManagerClient::GetInstance()->ReleaseDataAbility(dataAbilityProxy, token_);
            if (err != ERR_OK) {
                APP_LOGE("DataAbilityHelper::GetFileTypes failed to ReleaseDataAbility err = %{public}d", err);
            }
        }
    } else {
        if (dataAbilityProxy_ != nullptr) {
            matchedMIMEs = dataAbilityProxy_->GetFileTypes(uri, mimeTypeFilter);
        }
    }
    return matchedMIMEs;
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
int DataAbilityHelper::OpenFile(Uri &uri, const std::string &mode)
{
    int fd = -1;
    if ((uri_ == nullptr) || (!uri_->Equals(uri))) {
        if (uri.GetScheme() == SchemeOhos) {
            sptr<IAbilityScheduler> dataAbilityProxy =
                AbilityManagerClient::GetInstance()->AcquireDataAbility(uri, tryBind_, token_);
            if (dataAbilityProxy == nullptr) {
                APP_LOGE("DataAbilityHelper::OpenFile failed dataAbility == nullptr");
                return fd;
            }

            fd = dataAbilityProxy->OpenFile(uri, mode);

            int err = AbilityManagerClient::GetInstance()->ReleaseDataAbility(dataAbilityProxy, token_);
            if (err != ERR_OK) {
                APP_LOGE("AbilityThread::OpenFile failed to ReleaseDataAbility err = %{public}d", err);
            }
        }
    } else {
        if (dataAbilityProxy_ != nullptr) {
            fd = dataAbilityProxy_->OpenFile(uri, mode);
        }
    }
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
int DataAbilityHelper::OpenRawFile(Uri &uri, const std::string &mode)
{
    int fd = -1;
    if ((uri_ == nullptr) || (!uri_->Equals(uri))) {
        if (uri.GetScheme() == SchemeOhos) {
            sptr<AAFwk::IAbilityScheduler> dataAbilityProxy =
                AbilityManagerClient::GetInstance()->AcquireDataAbility(uri, tryBind_, token_);
            if (dataAbilityProxy == nullptr) {
                APP_LOGE("DataAbilityHelper::OpenRawFile failed dataAbility == nullptr");
                return fd;
            }

            fd = dataAbilityProxy->OpenRawFile(uri, mode);

            int err = AbilityManagerClient::GetInstance()->ReleaseDataAbility(dataAbilityProxy, token_);
            if (err != ERR_OK) {
                APP_LOGE("AbilityThread::OpenRawFile failed to ReleaseDataAbility err = %{public}d", err);
            }
        }
    } else {
        if (dataAbilityProxy_ != nullptr) {
            fd = dataAbilityProxy_->OpenRawFile(uri, mode);
        }
    }
    return fd;
}

/**
 * @brief Inserts a single data record into the database.
 *
 * @param uri Indicates the path of the data to operate.
 * @param value Indicates the data record to insert. If this parameter is null, a blank row will be inserted.
 *
 * @return Returns the index of the inserted data record.
 */
int DataAbilityHelper::Insert(Uri &uri, const ValuesBucket &value)
{
    int index = -1;
    if ((uri_ == nullptr) || (!uri_->Equals(uri))) {
        if (uri.GetScheme() == SchemeOhos) {
            sptr<IAbilityScheduler> dataAbilityProxy =
                AbilityManagerClient::GetInstance()->AcquireDataAbility(uri, tryBind_, token_);
            if (dataAbilityProxy == nullptr) {
                APP_LOGE("DataAbilityHelper::Insert failed dataAbility == nullptr");
                return index;
            }

            index = dataAbilityProxy->Insert(uri, value);

            int err = AbilityManagerClient::GetInstance()->ReleaseDataAbility(dataAbilityProxy, token_);
            if (err != ERR_OK) {
                APP_LOGE("AbilityThread::Insert failed to ReleaseDataAbility err = %{public}d", err);
            }
        }
    } else {
        if (dataAbilityProxy_ != nullptr) {
            index = dataAbilityProxy_->Insert(uri, value);
        }
    }
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
int DataAbilityHelper::Update(Uri &uri, const ValuesBucket &value, const DataAbilityPredicates &predicates)
{
    int index = -1;
    if ((uri_ == nullptr) || (!uri_->Equals(uri))) {
        if (uri.GetScheme() == SchemeOhos) {
            sptr<IAbilityScheduler> dataAbilityProxy =
                AbilityManagerClient::GetInstance()->AcquireDataAbility(uri, tryBind_, token_);
            if (dataAbilityProxy == nullptr) {
                APP_LOGE("DataAbilityHelper::Insert failed dataAbility == nullptr");
                return index;
            }

            index = dataAbilityProxy->Update(uri, value, predicates);

            int err = AbilityManagerClient::GetInstance()->ReleaseDataAbility(dataAbilityProxy, token_);
            if (err != ERR_OK) {
                APP_LOGE("AbilityThread::Insert failed to ReleaseDataAbility err = %{public}d", err);
            }
        }
    } else {
        if (dataAbilityProxy_ != nullptr) {
            index = dataAbilityProxy_->Update(uri, value, predicates);
        }
    }
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
int DataAbilityHelper::Delete(Uri &uri, const DataAbilityPredicates &predicates)
{
    int index = -1;
    if ((uri_ == nullptr) || (!uri_->Equals(uri))) {
        if (uri.GetScheme() == SchemeOhos) {
            sptr<IAbilityScheduler> dataAbilityProxy =
                AbilityManagerClient::GetInstance()->AcquireDataAbility(uri, tryBind_, token_);
            if (dataAbilityProxy == nullptr) {
                APP_LOGE("DataAbilityHelper::Delete failed dataAbility == nullptr");
                return index;
            }

            index = dataAbilityProxy->Delete(uri, predicates);

            int err = AbilityManagerClient::GetInstance()->ReleaseDataAbility(dataAbilityProxy, token_);
            if (err != ERR_OK) {
                APP_LOGE("AbilityThread::Delete failed to ReleaseDataAbility err = %{public}d", err);
            }
        }
    } else {
        if (dataAbilityProxy_ != nullptr) {
            index = dataAbilityProxy_->Delete(uri, predicates);
        }
    }
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
std::shared_ptr<ResultSet> DataAbilityHelper::Query(
    Uri &uri, std::vector<std::string> &columns, const DataAbilityPredicates &predicates)
{
    std::shared_ptr<ResultSet> resultset = nullptr;
    if ((uri_ == nullptr) || (!uri_->Equals(uri))) {
        if (uri.GetScheme() == SchemeOhos) {
            sptr<IAbilityScheduler> dataAbilityProxy =
                AbilityManagerClient::GetInstance()->AcquireDataAbility(uri, tryBind_, token_);
            if (dataAbilityProxy == nullptr) {
                APP_LOGE("DataAbilityHelper::Query failed dataAbility == nullptr");
                return resultset;
            }

            resultset = dataAbilityProxy->Query(uri, columns, predicates);

            int err = AbilityManagerClient::GetInstance()->ReleaseDataAbility(dataAbilityProxy, token_);
            if (err != ERR_OK) {
                APP_LOGE("AbilityThread::Query failed to ReleaseDataAbility err = %{public}d", err);
            }
        }
    } else {
        if (dataAbilityProxy_ != nullptr) {
            resultset = dataAbilityProxy_->Query(uri, columns, predicates);
        }
    }
    return resultset;
}

/**
 * @brief Obtains the MIME type matching the data specified by the URI of the Data ability. This method should be
 * implemented by a Data ability. Data abilities supports general data types, including text, HTML, and JPEG.
 *
 * @param uri Indicates the URI of the data.
 *
 * @return Returns the MIME type that matches the data specified by uri.
 */
std::string DataAbilityHelper::GetType(Uri &uri)
{
    std::string type;
    if ((uri_ == nullptr) || (!uri_->Equals(uri))) {
        if (uri.GetScheme() == SchemeOhos) {
            sptr<IAbilityScheduler> dataAbilityProxy =
                AbilityManagerClient::GetInstance()->AcquireDataAbility(uri, tryBind_, token_);
            if (dataAbilityProxy == nullptr) {
                APP_LOGE("DataAbilityHelper::GetType failed dataAbility == nullptr");
                return type;
            }

            type = dataAbilityProxy->GetType(uri);

            int err = AbilityManagerClient::GetInstance()->ReleaseDataAbility(dataAbilityProxy, token_);
            if (err != ERR_OK) {
                APP_LOGE("AbilityThread::GetType failed to ReleaseDataAbility err = %{public}d", err);
            }
        }
    } else {
        if (dataAbilityProxy_ != nullptr) {
            type = dataAbilityProxy_->GetType(uri);
        }
    }
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
bool DataAbilityHelper::Reload(Uri &uri, const PacMap &extras)
{
    bool ret = false;
    if ((uri_ == nullptr) || (!uri_->Equals(uri))) {
        if (uri.GetScheme() == SchemeOhos) {
            sptr<AAFwk::IAbilityScheduler> dataAbilityProxy =
                AbilityManagerClient::GetInstance()->AcquireDataAbility(uri, tryBind_, token_);
            if (dataAbilityProxy == nullptr) {
                APP_LOGE("DataAbilityHelper::Reload failed dataAbility == nullptr");
                return ret;
            }

            ret = dataAbilityProxy->Reload(uri, extras);

            int err = AbilityManagerClient::GetInstance()->ReleaseDataAbility(dataAbilityProxy, token_);
            if (err != ERR_OK) {
                APP_LOGE("AbilityThread::Reload failed to ReleaseDataAbility err = %{public}d", err);
            }
        }
    } else {
        if (dataAbilityProxy_ != nullptr) {
            ret = dataAbilityProxy_->Reload(uri, extras);
        }
    }
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
int DataAbilityHelper::BatchInsert(Uri &uri, const std::vector<ValuesBucket> &values)
{
    int ret = -1;
    if ((uri_ == nullptr) || (!uri_->Equals(uri))) {
        if (uri.GetScheme() == SchemeOhos) {
            sptr<AAFwk::IAbilityScheduler> dataAbilityProxy =
                AbilityManagerClient::GetInstance()->AcquireDataAbility(uri, tryBind_, token_);
            if (dataAbilityProxy == nullptr) {
                APP_LOGE("DataAbilityHelper::BatchInsert​ failed dataAbility == nullptr");
                return ret;
            }

            ret = dataAbilityProxy->BatchInsert(uri, values);

            int err = AbilityManagerClient::GetInstance()->ReleaseDataAbility(dataAbilityProxy, token_);
            if (err != ERR_OK) {
                APP_LOGE("AbilityThread::BatchInsert​ failed to ReleaseDataAbility err = %{public}d", err);
            }
        }
    } else {
        if (dataAbilityProxy_ != nullptr) {
            ret = dataAbilityProxy_->BatchInsert(uri, values);
        }
    }
    return ret;
}
}  // namespace AppExecFwk
}  // namespace OHOS