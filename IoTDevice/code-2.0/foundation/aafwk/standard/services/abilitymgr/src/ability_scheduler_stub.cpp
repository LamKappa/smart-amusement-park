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

#include "ability_scheduler_stub.h"

#include "ability_scheduler_proxy.h"
#include "hilog_wrapper.h"
#include "ipc_types.h"
#include "pac_map.h"
#include "want.h"

namespace OHOS {
namespace AAFwk {

bool AbilitySchedulerProxy::WriteInterfaceToken(MessageParcel &data)
{
    if (!data.WriteInterfaceToken(AbilitySchedulerProxy::GetDescriptor())) {
        HILOG_ERROR("write interface token failed");
        return false;
    }
    return true;
}

void AbilitySchedulerProxy::ScheduleAbilityTransaction(const Want &want, const LifeCycleStateInfo &stateInfo)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        return;
    }
    data.WriteParcelable(&want);
    data.WriteParcelable(&stateInfo);
    int32_t err = Remote()->SendRequest(IAbilityScheduler::SCHEDULE_ABILITY_TRANSACTION, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("ScheduleAbilityTransaction fail to SendRequest. err: %d", err);
    }
}

void AbilitySchedulerProxy::SendResult(int requestCode, int resultCode, const Want &resultWant)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        return;
    }
    data.WriteInt32(requestCode);
    data.WriteInt32(resultCode);
    if (!data.WriteParcelable(&resultWant)) {
        HILOG_ERROR("fail to WriteParcelable");
        return;
    }
    int32_t err = Remote()->SendRequest(IAbilityScheduler::SEND_RESULT, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("SendResult fail to SendRequest. err: %d", err);
    }
}

void AbilitySchedulerProxy::ScheduleConnectAbility(const Want &want)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        return;
    }
    if (!data.WriteParcelable(&want)) {
        HILOG_ERROR("fail to WriteParcelable");
        return;
    }
    int32_t err = Remote()->SendRequest(IAbilityScheduler::SCHEDULE_ABILITY_CONNECT, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("ScheduleConnectAbility fail to SendRequest. err: %d", err);
    }
}

void AbilitySchedulerProxy::ScheduleDisconnectAbility(const Want &want)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        return;
    }
    if (!data.WriteParcelable(&want)) {
        HILOG_ERROR("fail to WriteParcelable");
        return;
    }

    int32_t err = Remote()->SendRequest(IAbilityScheduler::SCHEDULE_ABILITY_DISCONNECT, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("ScheduleDisconnectAbility fail to SendRequest. err: %d", err);
    }
}

void AbilitySchedulerProxy::ScheduleCommandAbility(const Want &want, bool restart, int startId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        return;
    }
    if (!data.WriteParcelable(&want) && !data.WriteBool(restart) && !data.WriteInt32(startId)) {
        HILOG_ERROR("fail to WriteParcelable");
        return;
    }

    int32_t err = Remote()->SendRequest(IAbilityScheduler::SCHEDULE_ABILITY_COMMAND, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("ScheduleCommandAbility fail to SendRequest. err: %d", err);
    }
}

void AbilitySchedulerProxy::ScheduleSaveAbilityState(PacMap &outState)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        return;
    }
    int32_t err = Remote()->SendRequest(IAbilityScheduler::SCHEDULE_SAVE_ABILITY_STATE, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("ScheduleSaveAbilityState fail to SendRequest. err: %d", err);
    }
}

void AbilitySchedulerProxy::ScheduleRestoreAbilityState(const PacMap &inState)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        return;
    }
    int32_t err = Remote()->SendRequest(IAbilityScheduler::SCHEDULE_RESTORE_ABILITY_STATE, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("ScheduleRestoreAbilityState fail to SendRequest. err: %d", err);
    }
}

/**
 * @brief Obtains the MIME types of files supported.
 *
 * @param uri Indicates the path of the files to obtain.
 * @param mimeTypeFilter Indicates the MIME types of the files to obtain. This parameter cannot be null.
 *
 * @return Returns the matched MIME types. If there is no match, null is returned.
 */
std::vector<std::string> AbilitySchedulerProxy::GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter)
{
    std::vector<std::string> types;

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return types;
    }

    if (!data.WriteParcelable(&uri)) {
        HILOG_ERROR("fail to WriteParcelable uri");
        return types;
    }

    if (!data.WriteString(mimeTypeFilter)) {
        HILOG_ERROR("fail to WriteString mimeTypeFilter");
        return types;
    }

    int32_t err = Remote()->SendRequest(IAbilityScheduler::SCHEDULE_GETFILETYPES, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("GetFileTypes fail to SendRequest. err: %d", err);
    }

    if (!reply.ReadStringVector(&types)) {
        HILOG_ERROR("fail to ReadStringVector types");
    }

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
int AbilitySchedulerProxy::OpenFile(const Uri &uri, const std::string &mode)
{
    int fd = -1;

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return fd;
    }

    if (!data.WriteParcelable(&uri)) {
        HILOG_ERROR("fail to WriteParcelable uri");
        return fd;
    }

    if (!data.WriteString(mode)) {
        HILOG_ERROR("fail to WriteString mode");
        return fd;
    }

    int32_t err = Remote()->SendRequest(IAbilityScheduler::SCHEDULE_OPENFILE, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("OpenFile fail to SendRequest. err: %d", err);
        return err;
    }

    fd = reply.ReadFileDescriptor();
    if (fd == -1) {
        HILOG_ERROR("fail to ReadInt32 fd");
        return fd;
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
int AbilitySchedulerProxy::OpenRawFile(const Uri &uri, const std::string &mode)
{
    int fd = -1;

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return fd;
    }

    if (!data.WriteParcelable(&uri)) {
        HILOG_ERROR("fail to WriteParcelable uri");
        return fd;
    }

    if (!data.WriteString(mode)) {
        HILOG_ERROR("fail to WriteString mode");
        return fd;
    }

    int32_t err = Remote()->SendRequest(IAbilityScheduler::SCHEDULE_OPENRAWFILE, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("OpenFile fail to SendRequest. err: %d", err);
    }

    if (!reply.ReadInt32(fd)) {
        HILOG_ERROR("fail to ReadInt32 fd");
        return fd;
    }

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
int AbilitySchedulerProxy::Insert(const Uri &uri, const ValuesBucket &value)
{
    int index = -1;

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return index;
    }

    if (!data.WriteParcelable(&uri)) {
        HILOG_ERROR("fail to WriteParcelable uri");
        return index;
    }

    if (!data.WriteParcelable(&value)) {
        HILOG_ERROR("fail to WriteParcelable value");
        return index;
    }

    int32_t err = Remote()->SendRequest(IAbilityScheduler::SCHEDULE_INSERT, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("Insert fail to SendRequest. err: %d", err);
        return err;
    }

    if (!reply.ReadInt32(index)) {
        HILOG_ERROR("fail to ReadInt32 index");
        return index;
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
int AbilitySchedulerProxy::Update(const Uri &uri, const ValuesBucket &value, const DataAbilityPredicates &predicates)
{
    int index = -1;

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return index;
    }

    if (!data.WriteParcelable(&uri)) {
        HILOG_ERROR("fail to WriteParcelable uri");
        return index;
    }

    if (!data.WriteParcelable(&value)) {
        HILOG_ERROR("fail to WriteParcelable value");
        return index;
    }

    if (!data.WriteParcelable(&predicates)) {
        HILOG_ERROR("fail to WriteParcelable predicates");
        return index;
    }

    int32_t err = Remote()->SendRequest(IAbilityScheduler::SCHEDULE_UPDATE, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("Update fail to SendRequest. err: %d", err);
        return err;
    }

    if (!reply.ReadInt32(index)) {
        HILOG_ERROR("fail to ReadInt32 index");
        return index;
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
int AbilitySchedulerProxy::Delete(const Uri &uri, const DataAbilityPredicates &predicates)
{
    int index = -1;

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return index;
    }

    if (!data.WriteParcelable(&uri)) {
        HILOG_ERROR("fail to WriteParcelable uri");
        return index;
    }

    if (!data.WriteParcelable(&predicates)) {
        HILOG_ERROR("fail to WriteParcelable predicates");
        return index;
    }

    int32_t err = Remote()->SendRequest(IAbilityScheduler::SCHEDULE_DELETE, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("Delete fail to SendRequest. err: %d", err);
    }

    if (!reply.ReadInt32(index)) {
        HILOG_ERROR("fail to ReadInt32 index");
        return index;
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
std::shared_ptr<ResultSet> AbilitySchedulerProxy::Query(
    const Uri &uri, std::vector<std::string> &columns, const DataAbilityPredicates &predicates)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return nullptr;
    }

    if (!data.WriteParcelable(&uri)) {
        HILOG_ERROR("fail to WriteParcelable uri");
        return nullptr;
    }

    if (!data.WriteStringVector(columns)) {
        HILOG_ERROR("fail to WriteStringVector columns");
        return nullptr;
    }

    if (!data.WriteParcelable(&predicates)) {
        HILOG_ERROR("fail to WriteParcelable predicates");
        return nullptr;
    }

    int32_t err = Remote()->SendRequest(IAbilityScheduler::SCHEDULE_QUERY, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("Query fail to SendRequest. err: %d", err);
        return nullptr;
    }

    ResultSet *value = reply.ReadParcelable<ResultSet>();
    if (value == nullptr) {
        HILOG_ERROR("ReadParcelable value is nullptr");
        return nullptr;
    }

    std::shared_ptr<ResultSet> resultSet(value);

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
std::string AbilitySchedulerProxy::GetType(const Uri &uri)
{
    std::string type;

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return type;
    }

    if (!data.WriteParcelable(&uri)) {
        HILOG_ERROR("fail to WriteParcelable uri");
        return type;
    }

    int32_t err = Remote()->SendRequest(IAbilityScheduler::SCHEDULE_GETTYPE, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("GetFileTypes fail to SendRequest. err: %d", err);
    }

    type = reply.ReadString();
    if (type.empty()) {
        HILOG_ERROR("fail to ReadString type");
        return type;
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
bool AbilitySchedulerProxy::Reload(const Uri &uri, const PacMap &extras)
{
    bool ret = false;

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return ret;
    }

    if (!data.WriteParcelable(&uri)) {
        HILOG_ERROR("fail to WriteParcelable uri");
        return ret;
    }

    if (!data.WriteParcelable(&extras)) {
        HILOG_ERROR("fail to WriteParcelable extras");
        return ret;
    }

    int32_t err = Remote()->SendRequest(IAbilityScheduler::SCHEDULE_RELOAD, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("GetFileTypes fail to SendRequest. err: %d", err);
    }

    ret = reply.ReadBool();
    if (!ret) {
        HILOG_ERROR("fail to ReadBool ret");
        return ret;
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
int AbilitySchedulerProxy::BatchInsert(const Uri &uri, const std::vector<ValuesBucket> &values)
{
    int ret = -1;

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return ret;
    }

    if (!data.WriteParcelable(&uri)) {
        HILOG_ERROR("fail to WriteParcelable uri");
        return ret;
    }

    int count = values.size();
    if (!data.WriteInt32(count)) {
        HILOG_ERROR("fail to WriteInt32 ret");
        return ret;
    }

    for (int i = 0; i < count; i++) {
        if (!data.WriteParcelable(&values[i])) {
            HILOG_ERROR("fail to WriteParcelable ret, index = %{public}d", i);
            return ret;
        }
    }

    int32_t err = Remote()->SendRequest(IAbilityScheduler::SCHEDULE_BATCHINSERT, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("GetFileTypes fail to SendRequest. err: %d", err);
    }

    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 index");
        return ret;
    }

    return ret;
}

AbilitySchedulerStub::AbilitySchedulerStub()
{
    requestFuncMap_[SCHEDULE_ABILITY_TRANSACTION] = &AbilitySchedulerStub::AbilityTransactionInner;
    requestFuncMap_[SEND_RESULT] = &AbilitySchedulerStub::SendResultInner;
    requestFuncMap_[SCHEDULE_ABILITY_CONNECT] = &AbilitySchedulerStub::ConnectAbilityInner;
    requestFuncMap_[SCHEDULE_ABILITY_DISCONNECT] = &AbilitySchedulerStub::DisconnectAbilityInner;
    requestFuncMap_[SCHEDULE_ABILITY_COMMAND] = &AbilitySchedulerStub::CommandAbilityInner;
    requestFuncMap_[SCHEDULE_SAVE_ABILITY_STATE] = &AbilitySchedulerStub::SaveAbilityStateInner;
    requestFuncMap_[SCHEDULE_RESTORE_ABILITY_STATE] = &AbilitySchedulerStub::RestoreAbilityStateInner;
    requestFuncMap_[SCHEDULE_GETFILETYPES] = &AbilitySchedulerStub::GetFileTypesInner;
    requestFuncMap_[SCHEDULE_OPENFILE] = &AbilitySchedulerStub::OpenFileInner;
    requestFuncMap_[SCHEDULE_OPENRAWFILE] = &AbilitySchedulerStub::OpenRawFileInner;
    requestFuncMap_[SCHEDULE_INSERT] = &AbilitySchedulerStub::InsertInner;
    requestFuncMap_[SCHEDULE_UPDATE] = &AbilitySchedulerStub::UpdatetInner;
    requestFuncMap_[SCHEDULE_DELETE] = &AbilitySchedulerStub::DeleteInner;
    requestFuncMap_[SCHEDULE_QUERY] = &AbilitySchedulerStub::QueryInner;
    requestFuncMap_[SCHEDULE_GETTYPE] = &AbilitySchedulerStub::GetTypeInner;
    requestFuncMap_[SCHEDULE_RELOAD] = &AbilitySchedulerStub::ReloadInner;
    requestFuncMap_[SCHEDULE_BATCHINSERT] = &AbilitySchedulerStub::BatchInsertInner;
}

AbilitySchedulerStub::~AbilitySchedulerStub()
{
    requestFuncMap_.clear();
}

int AbilitySchedulerStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOG_DEBUG("AbilitySchedulerStub::OnRemoteRequest, cmd = %d, flags= %d", code, option.GetFlags());
    std::u16string descriptor = AbilitySchedulerStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HILOG_INFO("local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }

    auto itFunc = requestFuncMap_.find(code);
    if (itFunc != requestFuncMap_.end()) {
        auto requestFunc = itFunc->second;
        if (requestFunc != nullptr) {
            return (this->*requestFunc)(data, reply);
        }
    }
    HILOG_WARN("AbilitySchedulerStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int AbilitySchedulerStub::AbilityTransactionInner(MessageParcel &data, MessageParcel &reply)
{
    Want *want = data.ReadParcelable<Want>();
    if (want == nullptr) {
        HILOG_ERROR("AbilitySchedulerStub want is nullptr");
        return ERR_INVALID_VALUE;
    }
    std::unique_ptr<LifeCycleStateInfo> stateInfo(data.ReadParcelable<LifeCycleStateInfo>());
    if (!stateInfo) {
        HILOG_ERROR("ReadParcelable<LifeCycleStateInfo> failed");
        return ERR_INVALID_VALUE;
    }
    ScheduleAbilityTransaction(*want, *stateInfo);
    delete want;
    return NO_ERROR;
}

int AbilitySchedulerStub::SendResultInner(MessageParcel &data, MessageParcel &reply)
{
    int requestCode = data.ReadInt32();
    int resultCode = data.ReadInt32();
    Want *want = data.ReadParcelable<Want>();
    if (want == nullptr) {
        HILOG_ERROR("AbilitySchedulerStub want is nullptr");
        return ERR_INVALID_VALUE;
    }
    SendResult(requestCode, resultCode, *want);
    delete want;
    return NO_ERROR;
}

int AbilitySchedulerStub::ConnectAbilityInner(MessageParcel &data, MessageParcel &reply)
{
    Want *want = data.ReadParcelable<Want>();
    if (want == nullptr) {
        HILOG_ERROR("AbilitySchedulerStub want is nullptr");
        return ERR_INVALID_VALUE;
    }
    ScheduleConnectAbility(*want);
    delete want;
    return NO_ERROR;
}

int AbilitySchedulerStub::DisconnectAbilityInner(MessageParcel &data, MessageParcel &reply)
{
    Want *want = data.ReadParcelable<Want>();
    if (want == nullptr) {
        HILOG_ERROR("AbilitySchedulerStub want is nullptr");
        return ERR_INVALID_VALUE;
    }
    ScheduleDisconnectAbility(*want);
    delete want;
    return NO_ERROR;
}

int AbilitySchedulerStub::CommandAbilityInner(MessageParcel &data, MessageParcel &reply)
{
    Want *want = data.ReadParcelable<Want>();
    if (want == nullptr) {
        HILOG_ERROR("AbilitySchedulerStub want is nullptr");
        return ERR_INVALID_VALUE;
    }
    bool reStart = data.ReadBool();
    int startId = data.ReadInt32();
    ScheduleCommandAbility(*want, reStart, startId);
    delete want;
    return NO_ERROR;
}

int AbilitySchedulerStub::SaveAbilityStateInner(MessageParcel &data, MessageParcel &reply)
{
    return NO_ERROR;
}

int AbilitySchedulerStub::RestoreAbilityStateInner(MessageParcel &data, MessageParcel &reply)
{
    HILOG_INFO("RestoreAbilityStateInner");
    return NO_ERROR;
}

int AbilitySchedulerStub::GetFileTypesInner(MessageParcel &data, MessageParcel &reply)
{
    Uri *uri = data.ReadParcelable<Uri>();
    if (uri == nullptr) {
        HILOG_ERROR("AbilitySchedulerStub uri is nullptr");
        return ERR_INVALID_VALUE;
    }
    std::string mimeTypeFilter = data.ReadString();
    if (mimeTypeFilter.empty()) {
        HILOG_ERROR("AbilitySchedulerStub mimeTypeFilter is nullptr");
        return ERR_INVALID_VALUE;
    }
    std::vector<std::string> types = GetFileTypes(*uri, mimeTypeFilter);
    if (!reply.WriteStringVector(types)) {
        HILOG_ERROR("fail to WriteStringVector types");
        return ERR_INVALID_VALUE;
    }
    delete uri;
    return NO_ERROR;
}

int AbilitySchedulerStub::OpenFileInner(MessageParcel &data, MessageParcel &reply)
{
    Uri *uri = data.ReadParcelable<Uri>();
    if (uri == nullptr) {
        HILOG_ERROR("AbilitySchedulerStub uri is nullptr");
        return ERR_INVALID_VALUE;
    }
    std::string mode = data.ReadString();
    if (mode.empty()) {
        HILOG_ERROR("AbilitySchedulerStub mode is nullptr");
        return ERR_INVALID_VALUE;
    }
    int fd = OpenFile(*uri, mode);
    if (!reply.WriteFileDescriptor(fd)) {
        HILOG_ERROR("fail to WriteFileDescriptor fd");
        return ERR_INVALID_VALUE;
    }
    delete uri;
    return NO_ERROR;
}

int AbilitySchedulerStub::OpenRawFileInner(MessageParcel &data, MessageParcel &reply)
{
    Uri *uri = data.ReadParcelable<Uri>();
    if (uri == nullptr) {
        HILOG_ERROR("AbilitySchedulerStub uri is nullptr");
        return ERR_INVALID_VALUE;
    }
    std::string mode = data.ReadString();
    if (mode.empty()) {
        HILOG_ERROR("AbilitySchedulerStub mode is nullptr");
        return ERR_INVALID_VALUE;
    }
    int fd = OpenRawFile(*uri, mode);
    if (!reply.WriteInt32(fd)) {
        HILOG_ERROR("fail to WriteInt32 fd");
        return ERR_INVALID_VALUE;
    }
    delete uri;
    return NO_ERROR;
}

int AbilitySchedulerStub::InsertInner(MessageParcel &data, MessageParcel &reply)
{
    Uri *uri = data.ReadParcelable<Uri>();
    if (uri == nullptr) {
        HILOG_ERROR("AbilitySchedulerStub uri is nullptr");
        return ERR_INVALID_VALUE;
    }
    ValuesBucket *value = data.ReadParcelable<ValuesBucket>();
    if (value == nullptr) {
        HILOG_ERROR("ReadParcelable value is nullptr");
        return ERR_INVALID_VALUE;
    }
    int index = Insert(*uri, *value);
    if (!reply.WriteInt32(index)) {
        HILOG_ERROR("fail to WriteInt32 index");
        return ERR_INVALID_VALUE;
    }
    delete uri;
    delete value;
    return NO_ERROR;
}

int AbilitySchedulerStub::UpdatetInner(MessageParcel &data, MessageParcel &reply)
{
    Uri *uri = data.ReadParcelable<Uri>();
    if (uri == nullptr) {
        HILOG_ERROR("AbilitySchedulerStub uri is nullptr");
        return ERR_INVALID_VALUE;
    }
    ValuesBucket *value = data.ReadParcelable<ValuesBucket>();
    if (value == nullptr) {
        HILOG_ERROR("ReadParcelable value is nullptr");
        return ERR_INVALID_VALUE;
    }
    DataAbilityPredicates *predicates = data.ReadParcelable<DataAbilityPredicates>();
    if (predicates == nullptr) {
        HILOG_ERROR("ReadParcelable predicates is nullptr");
        return ERR_INVALID_VALUE;
    }
    int index = Update(*uri, *value, *predicates);
    if (!reply.WriteInt32(index)) {
        HILOG_ERROR("fail to WriteInt32 index");
        return ERR_INVALID_VALUE;
    }
    delete uri;
    delete value;
    delete predicates;
    return NO_ERROR;
}

int AbilitySchedulerStub::DeleteInner(MessageParcel &data, MessageParcel &reply)
{
    Uri *uri = data.ReadParcelable<Uri>();
    if (uri == nullptr) {
        HILOG_ERROR("AbilitySchedulerStub uri is nullptr");
        return ERR_INVALID_VALUE;
    }
    DataAbilityPredicates *predicates = data.ReadParcelable<DataAbilityPredicates>();
    if (predicates == nullptr) {
        HILOG_ERROR("ReadParcelable predicates is nullptr");
        return ERR_INVALID_VALUE;
    }
    int index = Delete(*uri, *predicates);
    if (!reply.WriteInt32(index)) {
        HILOG_ERROR("fail to WriteInt32 index");
        return ERR_INVALID_VALUE;
    }
    delete uri;
    delete predicates;
    return NO_ERROR;
}

int AbilitySchedulerStub::QueryInner(MessageParcel &data, MessageParcel &reply)
{
    Uri *uri = data.ReadParcelable<Uri>();
    if (uri == nullptr) {
        HILOG_ERROR("AbilitySchedulerStub uri is nullptr");
        return ERR_INVALID_VALUE;
    }
    std::vector<std::string> columns;
    if (!data.ReadStringVector(&columns)) {
        HILOG_ERROR("fail to ReadStringVector columns");
        return ERR_INVALID_VALUE;
    }
    DataAbilityPredicates *predicates = data.ReadParcelable<DataAbilityPredicates>();
    if (predicates == nullptr) {
        HILOG_ERROR("ReadParcelable predicates is nullptr");
        return ERR_INVALID_VALUE;
    }
    std::shared_ptr<ResultSet> resultSet = Query(*uri, columns, *predicates);
    ResultSet *resultSetPtr = resultSet.get();
    if (resultSetPtr == nullptr || !reply.WriteParcelable(resultSetPtr)) {
        HILOG_ERROR("fail to WriteParcelable resultSet");
        return ERR_INVALID_VALUE;
    }
    delete uri;
    delete predicates;
    return NO_ERROR;
}

int AbilitySchedulerStub::GetTypeInner(MessageParcel &data, MessageParcel &reply)
{
    Uri *uri = data.ReadParcelable<Uri>();
    if (uri == nullptr) {
        HILOG_ERROR("AbilitySchedulerStub uri is nullptr");
        return ERR_INVALID_VALUE;
    }
    std::string type = GetType(*uri);
    if (!reply.WriteString(type)) {
        HILOG_ERROR("fail to WriteString type");
        return ERR_INVALID_VALUE;
    }
    delete uri;
    return NO_ERROR;
}

int AbilitySchedulerStub::ReloadInner(MessageParcel &data, MessageParcel &reply)
{
    Uri *uri = data.ReadParcelable<Uri>();
    if (uri == nullptr) {
        HILOG_ERROR("AbilitySchedulerStub uri is nullptr");
        return ERR_INVALID_VALUE;
    }

    PacMap *extras = data.ReadParcelable<PacMap>();
    if (extras == nullptr) {
        HILOG_ERROR("AbilitySchedulerStub extras is nullptr");
        return ERR_INVALID_VALUE;
    }
    bool ret = Reload(*uri, *extras);
    if (!reply.WriteBool(ret)) {
        HILOG_ERROR("fail to writeBool ret");
        return ERR_INVALID_VALUE;
    }
    delete uri;
    delete extras;
    return NO_ERROR;
}

int AbilitySchedulerStub::BatchInsertInner(MessageParcel &data, MessageParcel &reply)
{
    Uri *uri = data.ReadParcelable<Uri>();
    if (uri == nullptr) {
        HILOG_ERROR("AbilitySchedulerStub uri is nullptr");
        return ERR_INVALID_VALUE;
    }

    int count = 0;
    if (!data.ReadInt32(count)) {
        HILOG_ERROR("fail to ReadInt32 index");
        return ERR_INVALID_VALUE;
    }

    std::vector<ValuesBucket> values;
    for (int i = 0; i < count; i++) {
        ValuesBucket *value = data.ReadParcelable<ValuesBucket>();
        if (value == nullptr) {
            HILOG_ERROR("AbilitySchedulerStub value is nullptr, index = %{public}d", i);
            return ERR_INVALID_VALUE;
        }
        values.emplace_back(*value);
    }

    int ret = BatchInsert(*uri, values);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("fail to WriteInt32 ret");
        return ERR_INVALID_VALUE;
    }
    delete uri;
    return NO_ERROR;
}

void AbilitySchedulerRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    HILOG_ERROR("recv AbilitySchedulerRecipient death notice");

    if (handler_) {
        handler_(remote);
    }
}

AbilitySchedulerRecipient::AbilitySchedulerRecipient(RemoteDiedHandler handler) : handler_(handler)
{}

AbilitySchedulerRecipient::~AbilitySchedulerRecipient()
{}

}  // namespace AAFwk
}  // namespace OHOS
