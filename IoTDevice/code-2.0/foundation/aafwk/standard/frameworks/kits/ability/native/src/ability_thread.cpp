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

#include "ability_thread.h"
#include <functional>
#include "ohos_application.h"
#include "ability_loader.h"
#include "ability_state.h"
#include "ability_impl_factory.h"
#include "page_ability_impl.h"
#include "application_impl.h"
#include "app_log_wrapper.h"
#include "context_deal.h"

namespace OHOS {
namespace AppExecFwk {
using AbilityManagerClient = OHOS::AAFwk::AbilityManagerClient;
constexpr static char ACE_ABILITY_NAME[] = "AceAbility";

/**
 * @brief Default constructor used to create a AbilityThread instance.
 */
AbilityThread::AbilityThread()
    : abilityImpl_(nullptr), token_(nullptr), currentAbility_(nullptr), abilityHandler_(nullptr), runner_(nullptr)
{}

AbilityThread::~AbilityThread()
{
    DelayedSingleton<AbilityImplFactory>::DestroyInstance();
}

/**
 * @description: Attach The ability thread to the main process.
 *
 * @param abilityRecord Indicates the abilityRecord.
 *
 * @return Returns the abilityName.
 *
 */
std::string AbilityThread::CreateAbilityName(const std::shared_ptr<AbilityLocalRecord> &abilityRecord)
{
    std::string abilityName;
    APP_LOGI("AbilityThread::CreateAbilityName called");
    if (abilityRecord == nullptr) {
        APP_LOGE("AbilityThread::CreateAbilityName failed,abilityRecord is nullptr");
        return abilityName;
    }

    std::shared_ptr<AbilityInfo> abilityInfo = abilityRecord->GetAbilityInfo();
    if (abilityInfo == nullptr) {
        APP_LOGE("AbilityThread::ability attach failed,abilityInfo is nullptr");
        return abilityName;
    }

    bool isAceAbility = false;
    APP_LOGI("AbilityThread::ability attach the ability type is %{public}d", abilityInfo->type);
    APP_LOGI("AbilityThread::ability attach the ability is Native %{public}d", abilityInfo->isNativeAbility);
    if ((abilityInfo->type == AbilityType::PAGE) && (abilityInfo->isNativeAbility == false)) {
        APP_LOGI("AbilityThread::ability attach the ability is Ace");
        isAceAbility = true;
    }
    abilityName = isAceAbility ? ACE_ABILITY_NAME : abilityInfo->name;
    return abilityName;
}

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
std::shared_ptr<ContextDeal> AbilityThread::CreateAndInitContextDeal(std::shared_ptr<OHOSApplication> &application,
    const std::shared_ptr<AbilityLocalRecord> &abilityRecord, const std::shared_ptr<Context> &abilityObject)
{
    std::shared_ptr<ContextDeal> contextDeal = nullptr;
    APP_LOGI("AbilityThread::CreateAndInitContextDeal called");
    if ((application == nullptr) || (abilityRecord == nullptr) || (abilityObject == nullptr)) {
        APP_LOGE("AbilityThread::ability attach failed,context or record or abilityObject is nullptr");
        return contextDeal;
    }

    contextDeal = std::make_shared<ContextDeal>();
    if (contextDeal == nullptr) {
        APP_LOGE("AbilityThread::ability attach failed,contextDeal  is nullptr");
        return contextDeal;
    }

    contextDeal->SetAbilityInfo(abilityRecord->GetAbilityInfo());
    contextDeal->SetApplicationInfo(application->GetApplicationInfo());
    contextDeal->SetProcessInfo(application->GetProcessInfo());

    std::shared_ptr<Context> tmpContext = application->GetApplicationContext();
    contextDeal->SetApplicationContext(tmpContext);

    contextDeal->SetBundleCodePath(abilityRecord->GetAbilityInfo()->codePath);
    contextDeal->SetContext(abilityObject);

    return contextDeal;
}

/**
 * @description: Attach The ability thread to the main process.
 * @param application Indicates the main process.
 * @param abilityRecord Indicates the abilityRecord.
 * @param mainRunner The runner which main_thread holds.
 */
void AbilityThread::Attach(std::shared_ptr<OHOSApplication> &application,
    const std::shared_ptr<AbilityLocalRecord> &abilityRecord, const std::shared_ptr<EventRunner> &mainRunner)
{
    APP_LOGI("AbilityThread:: attach begin");
    if ((application == nullptr) || (abilityRecord == nullptr) || (mainRunner == nullptr)) {
        APP_LOGE("AbilityThread::ability attach failed,context or record is nullptr");
        return;
    }

    // 1.new AbilityHandler
    std::string abilityName = CreateAbilityName(abilityRecord);
    abilityHandler_ = std::make_shared<AbilityHandler>(mainRunner, this);
    if (abilityHandler_ == nullptr) {
        APP_LOGE("AbilityThread::ability attach failed,abilityHandler_ is nullptr");
        return;
    }

    // 2.new ability
    auto ability = AbilityLoader::GetInstance().GetAbilityByName(abilityName);
    if (ability == nullptr) {
        APP_LOGE("AbilityThread::ability attach failed,load ability failed");
        return;
    }

    APP_LOGI("AbilityThread::new ability success.");
    std::shared_ptr<Context> abilityObject(ability);
    currentAbility_.reset(ability);
    token_ = abilityRecord->GetToken();
    abilityRecord->SetEventHandler(abilityHandler_);
    abilityRecord->SetEventRunner(mainRunner);
    abilityRecord->SetAbilityThread(this);
    std::shared_ptr<ContextDeal> contextDeal = CreateAndInitContextDeal(application, abilityRecord, abilityObject);
    ability->AttachBaseContext(contextDeal);

    // 3.new abilityImpl
    abilityImpl_ =
        DelayedSingleton<AbilityImplFactory>::GetInstance()->MakeAbilityImplObject(abilityRecord->GetAbilityInfo());
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::ability abilityImpl_ == nullptr");
        return;
    }
    abilityImpl_->Init(application, abilityRecord, currentAbility_, abilityHandler_, token_, contextDeal);

    // 4. ability attach : ipc
    ErrCode err = AbilityManagerClient::GetInstance()->AttachAbilityThread(this, token_);
    if (err != ERR_OK) {
        APP_LOGE("AbilityThread:: attach success faile err = %{public}d", err);
        return;
    }

    APP_LOGI("AbilityThread:: attach success");
}

/**
 * @description: Attach The ability thread to the main process.
 * @param application Indicates the main process.
 * @param abilityRecord Indicates the abilityRecord.
 */
void AbilityThread::Attach(
    std::shared_ptr<OHOSApplication> &application, const std::shared_ptr<AbilityLocalRecord> &abilityRecord)
{
    APP_LOGI("AbilityThread:: attach begin");
    if ((application == nullptr) || (abilityRecord == nullptr)) {
        APP_LOGE("AbilityThread::ability attach failed,context or record is nullptr");
        return;
    }
    // 1.new AbilityHandler
    std::string abilityName = CreateAbilityName(abilityRecord);
    runner_ = EventRunner::Create(abilityName);
    if (runner_ == nullptr) {
        APP_LOGE("AbilityThread::ability attach failed,create runner failed");
        return;
    }
    abilityHandler_ = std::make_shared<AbilityHandler>(runner_, this);
    if (abilityHandler_ == nullptr) {
        APP_LOGE("AbilityThread::ability attach failed,abilityHandler_ is nullptr");
        return;
    }

    // 2.new ability
    auto ability = AbilityLoader::GetInstance().GetAbilityByName(abilityName);
    if (ability == nullptr) {
        APP_LOGE("AbilityThread::ability attach failed,load ability failed");
        return;
    }

    APP_LOGI("AbilityThread::new ability success.");
    std::shared_ptr<Context> abilityObject(ability);
    currentAbility_.reset(ability);
    token_ = abilityRecord->GetToken();
    abilityRecord->SetEventHandler(abilityHandler_);
    abilityRecord->SetEventRunner(runner_);
    abilityRecord->SetAbilityThread(this);
    std::shared_ptr<ContextDeal> contextDeal = CreateAndInitContextDeal(application, abilityRecord, abilityObject);
    ability->AttachBaseContext(contextDeal);

    // 3.new abilityImpl
    abilityImpl_ =
        DelayedSingleton<AbilityImplFactory>::GetInstance()->MakeAbilityImplObject(abilityRecord->GetAbilityInfo());
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::ability abilityImpl_ == nullptr");
        return;
    }
    abilityImpl_->Init(application, abilityRecord, currentAbility_, abilityHandler_, token_, contextDeal);

    // 4. ability attach : ipc
    ErrCode err = AbilityManagerClient::GetInstance()->AttachAbilityThread(this, token_);
    if (err != ERR_OK) {
        APP_LOGE("AbilityThread:: attach success faile err = %{public}d", err);
        return;
    }

    APP_LOGI("AbilityThread:: attach success");
}

/**
 * @description:  Handle the life cycle of Ability.
 * @param want  Indicates the structure containing lifecycle information about the ability.
 * @param lifeCycleStateInfo  Indicates the lifeCycleStateInfo.
 */
void AbilityThread::HandleAbilityTransaction(const Want &want, const LifeCycleStateInfo &lifeCycleStateInfo)
{
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::HandleAbilityTransaction abilityImpl_ == nullptr");
        return;
    }

    abilityImpl_->SetCallingContext(lifeCycleStateInfo.caller.deviceId,
        lifeCycleStateInfo.caller.bundleName,
        lifeCycleStateInfo.caller.abilityName);
    abilityImpl_->HandleAbilityTransaction(want, lifeCycleStateInfo);
}

/**
 * @description:  Handle the current connection of Ability.
 * @param want  Indicates the structure containing connection information about the ability.
 */
void AbilityThread::HandleConnectAbility(const Want &want)
{
    APP_LOGI("AbilityThread::HandleConnectAbility called");
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::HandleConnectAbility abilityImpl_ == nullptr");
        return;
    }

    sptr<IRemoteObject> service = abilityImpl_->ConnectAbility(want);
    ErrCode err = AbilityManagerClient::GetInstance()->ScheduleConnectAbilityDone(token_, service);
    if (err != ERR_OK) {
        APP_LOGE("AbilityThread:: HandleConnectAbility faile err = %{public}d", err);
    }
}

/**
 * @description:  Handle the current disconnection of Ability.
 */
void AbilityThread::HandleDisconnectAbility(const Want &want)
{
    APP_LOGI("AbilityThread::HandleDisconnectAbility called");
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::HandleDisconnectAbility abilityImpl_ == nullptr");
        return;
    }

    abilityImpl_->DisconnectAbility(want);
    ErrCode err = AbilityManagerClient::GetInstance()->ScheduleDisconnectAbilityDone(token_);
    if (err != ERR_OK) {
        APP_LOGE("AbilityThread:: HandleDisconnectAbility faile err = %{public}d", err);
    }
}

/**
 * @brief Handle the current commadn of Ability.
 *
 * @param want The Want object to command to.
 *
 * * @param restart Indicates the startup mode. The value true indicates that Service is restarted after being
 * destroyed, and the value false indicates a normal startup.
 *
 * @param startId Indicates the number of times the Service ability has been started. The startId is incremented by 1
 * every time the ability is started. For example, if the ability has been started for six times, the value of startId
 * is 6.
 */
void AbilityThread::HandleCommandAbility(const Want &want, bool restart, int startId)
{
    APP_LOGI("AbilityThread::HandleCommandAbility called");
    abilityImpl_->CommandAbility(want, restart, startId);
    ErrCode err = AbilityManagerClient::GetInstance()->ScheduleCommandAbilityDone(token_);
    if (err != ERR_OK) {
        APP_LOGE("AbilityThread:: HandleCommandAbility  faile err = %{public}d", err);
    }
}

/**
 * @description: Handle the SaveAbility state.
 * @param state Indicates save ability state used to dispatchSaveAbilityState.
 */
void AbilityThread::HandleSaveAbilityState(PacMap &state)
{
    APP_LOGI("AbilityThread::HandleSaveAbilityState called");
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::HandleSaveAbilityState abilityImpl_ == nullptr");
        return;
    }

    abilityImpl_->DispatchSaveAbilityState(state);
}

/**
 * @description: Handle the restoreAbility state.
 * @param state  Indicates save ability state used to dispatchRestoreAbilityState.
 */
void AbilityThread::HandleRestoreAbilityState(const PacMap &state)
{
    APP_LOGI("AbilityThread::HandleRestoreAbilityState called");
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::HandleRestoreAbilityState abilityImpl_ == nullptr");
        return;
    }

    abilityImpl_->DispatchRestoreAbilityState(state);
}

/**
 * @description: Provide operating system SaveabilityState information to the observer
 * @param state Indicates save ability state used to dispatch.
 */
void AbilityThread::ScheduleSaveAbilityState(PacMap &state)
{
    APP_LOGI("AbilityThread::ScheduleSaveAbilityState called");

    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::ScheduleSaveAbilityState abilityImpl_ == nullptr");
        return;
    }

    auto task = [abilityThread = this, &state]() { abilityThread->HandleSaveAbilityState(state); };

    if (abilityHandler_ == nullptr) {
        APP_LOGE("AbilityThread::ScheduleSaveAbilityState abilityHandler_ == nullptr");
        return;
    }

    bool ret = abilityHandler_->PostTask(task);
    if (!ret) {
        APP_LOGE("AbilityThread::ScheduleSaveAbilityState PostTask error");
    }
}

/**
 * @description:  Provide operating system RestoreAbilityState information to the observer
 * @param state Indicates resotre ability state used to dispatchRestoreAbilityState.
 */
void AbilityThread::ScheduleRestoreAbilityState(const PacMap &state)
{
    APP_LOGI("AbilityThread::ScheduleRestoreAbilityState called");
    if (abilityImpl_ == nullptr) {
        APP_LOGE("ScheduleRestoreAbilityState::failed");
        return;
    }
    auto task = [abilityThread = this, state]() { abilityThread->HandleRestoreAbilityState(state); };

    if (abilityHandler_ == nullptr) {
        APP_LOGE("AbilityThread::ScheduleRestoreAbilityState abilityHandler_ == nullptr");
        return;
    }

    bool ret = abilityHandler_->PostTask(task);
    if (!ret) {
        APP_LOGE("AbilityThread::ScheduleRestoreAbilityState PostTask error");
    }
}

/**
 * @description:  Provide operating system AbilityTransaction information to the observer
 * @param want Indicates the structure containing Transaction information about the ability.
 * @param lifeCycleStateInfo Indicates the lifecycle state.
 */
void AbilityThread::ScheduleAbilityTransaction(const Want &want, const LifeCycleStateInfo &lifeCycleStateInfo)
{
    APP_LOGI("ScheduleAbilityTransaction::enter, targeState = %{public}d, isNewWant = %{public}d",
        lifeCycleStateInfo.state,
        lifeCycleStateInfo.isNewWant);
    if ((token_ == nullptr) || abilityImpl_ == nullptr) {
        APP_LOGE("ScheduleAbilityTransaction::failed");
        return;
    }
    auto task = [abilityThread = this, want, lifeCycleStateInfo]() {
        abilityThread->HandleAbilityTransaction(want, lifeCycleStateInfo);
    };

    if (abilityHandler_ == nullptr) {
        APP_LOGE("AbilityThread::ScheduleAbilityTransaction abilityHandler_ == nullptr");
        return;
    }

    bool ret = abilityHandler_->PostTask(task);
    if (!ret) {
        APP_LOGE("AbilityThread::ScheduleAbilityTransaction PostTask error");
    }
}

/**
 * @description:  Provide operating system ConnectAbility information to the observer
 * @param  want Indicates the structure containing connect information about the ability.
 */
void AbilityThread::ScheduleConnectAbility(const Want &want)
{
    APP_LOGI("AbilityThread::ScheduleConnectAbility called");
    auto task = [abilityThread = this, want]() { abilityThread->HandleConnectAbility(want); };

    if (abilityHandler_ == nullptr) {
        APP_LOGE("AbilityThread::ScheduleConnectAbility abilityHandler_ == nullptr");
        return;
    }

    bool ret = abilityHandler_->PostTask(task);
    if (!ret) {
        APP_LOGE("AbilityThread::ScheduleConnectAbility PostTask error");
    }
}

/**
 * @description: Provide operating system ConnectAbility information to the observer
 * @return  None
 */
void AbilityThread::ScheduleDisconnectAbility(const Want &want)
{
    APP_LOGI("AbilityThread::ScheduleDisconnectAbility called");
    auto task = [abilityThread = this, want]() { abilityThread->HandleDisconnectAbility(want); };

    if (abilityHandler_ == nullptr) {
        APP_LOGE("AbilityThread::ScheduleDisconnectAbility abilityHandler_ == nullptr");
        return;
    }

    bool ret = abilityHandler_->PostTask(task);
    if (!ret) {
        APP_LOGE("AbilityThread::ScheduleDisconnectAbility PostTask error");
    }
}

/**
 * @description: Provide operating system CommandAbility information to the observer
 *
 * @param want The Want object to command to.
 *
 * * @param restart Indicates the startup mode. The value true indicates that Service is restarted after being
 * destroyed, and the value false indicates a normal startup.
 *
 * @param startId Indicates the number of times the Service ability has been started. The startId is incremented by 1
 * every time the ability is started. For example, if the ability has been started for six times, the value of startId
 * is 6.
 */
void AbilityThread::ScheduleCommandAbility(const Want &want, bool restart, int startId)
{
    APP_LOGI("AbilityThread::ScheduleCommandAbility called");
    auto task = [abilityThread = this, want, restart, startId]() {
        abilityThread->HandleCommandAbility(want, restart, startId);
    };

    if (abilityHandler_ == nullptr) {
        APP_LOGE("AbilityThread::ScheduleCommandAbility abilityHandler_ == nullptr");
        return;
    }

    bool ret = abilityHandler_->PostTask(task);
    if (!ret) {
        APP_LOGE("AbilityThread::ScheduleCommandAbility PostTask error");
    }
}

/**
 * @brief Send the result code and data to be returned by this Page ability to the caller.
 * When a Page ability is destroyed, the caller overrides the AbilitySlice#onAbilityResult(int, int, Want) method to
 * receive the result set in the current method. This method can be called only after the ability has been initialized.
 *
 * @param requestCode Indicates the request code for send.
 * @param resultCode Indicates the result code returned after the ability is destroyed. You can define the result code
 * to identify an error.
 * @param want Indicates the data returned after the ability is destroyed. You can define the data returned. This
 * parameter can be null.
 */
void AbilityThread::SendResult(int requestCode, int resultCode, const Want &want)
{
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::SendResult abilityImpl_ == nullptr");
        return;
    }

    if (requestCode != -1) {
        abilityImpl_->SendResult(requestCode, resultCode, want);
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
std::vector<std::string> AbilityThread::GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter)
{
    std::vector<std::string> types;
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::GetFileTypes abilityImpl_ is nullptr");
        return types;
    }

    types = abilityImpl_->GetFileTypes(uri, mimeTypeFilter);
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
int AbilityThread::OpenFile(const Uri &uri, const std::string &mode)
{
    int fd = -1;
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::OpenFile abilityImpl_ is nullptr");
        return fd;
    }

    fd = abilityImpl_->OpenFile(uri, mode);
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
int AbilityThread::OpenRawFile(const Uri &uri, const std::string &mode)
{
    int fd = -1;
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::OpenRawFile abilityImpl_ is nullptr");
        return fd;
    }

    fd = abilityImpl_->OpenRawFile(uri, mode);
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
int AbilityThread::Insert(const Uri &uri, const ValuesBucket &value)
{
    int index = -1;
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::Insert abilityImpl_ is nullptr");
        return index;
    }

    index = abilityImpl_->Insert(uri, value);
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
int AbilityThread::Update(const Uri &uri, const ValuesBucket &value, const DataAbilityPredicates &predicates)
{
    int index = -1;
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::Update abilityImpl_ is nullptr");
        return index;
    }

    index = abilityImpl_->Update(uri, value, predicates);
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
int AbilityThread::Delete(const Uri &uri, const DataAbilityPredicates &predicates)
{
    int index = -1;
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::Delete abilityImpl_ is nullptr");
        return index;
    }

    index = abilityImpl_->Delete(uri, predicates);
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
std::shared_ptr<ResultSet> AbilityThread::Query(
    const Uri &uri, std::vector<std::string> &columns, const DataAbilityPredicates &predicates)
{
    std::shared_ptr<ResultSet> resultSet = nullptr;
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::Query abilityImpl_ is nullptr");
        return resultSet;
    }

    resultSet = abilityImpl_->Query(uri, columns, predicates);
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
std::string AbilityThread::GetType(const Uri &uri)
{
    std::string type;
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::GetType abilityImpl_ is nullptr");
        return type;
    }

    type = abilityImpl_->GetType(uri);
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
bool AbilityThread::Reload(const Uri &uri, const PacMap &extras)
{
    bool ret = false;
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::Reload abilityImpl_ is nullptr");
        return ret;
    }

    ret = abilityImpl_->Reload(uri, extras);
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
int AbilityThread::BatchInsert(const Uri &uri, const std::vector<ValuesBucket> &values)
{
    int ret = -1;
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::BatchInsert​ abilityImpl_ is nullptr");
        return ret;
    }

    ret = abilityImpl_->BatchInsert(uri, values);
    return ret;
}

/**
 * @description: Attach The ability thread to the main process.
 * @param application Indicates the main process.
 * @param abilityRecord Indicates the abilityRecord.
 * @param mainRunner The runner which main_thread holds.
 */
void AbilityThread::AbilityThreadMain(std::shared_ptr<OHOSApplication> &application,
    const std::shared_ptr<AbilityLocalRecord> &abilityRecord, const std::shared_ptr<EventRunner> &mainRunner)
{
    APP_LOGI("AbilityThread::AbilityThreadMain::begin");
    sptr<AbilityThread> thread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
    if (thread == nullptr) {
        APP_LOGE("AbilityThread::AbilityThreadMain failed,thread  is nullptr");
        return;
    }
    thread->Attach(application, abilityRecord, mainRunner);
}

/**
 * @description: Attach The ability thread to the main process.
 * @param application Indicates the main process.
 * @param abilityRecord Indicates the abilityRecord.
 */
void AbilityThread::AbilityThreadMain(
    std::shared_ptr<OHOSApplication> &application, const std::shared_ptr<AbilityLocalRecord> &abilityRecord)
{
    APP_LOGI("AbilityThread::AbilityThreadMain::begin");
    sptr<AbilityThread> thread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
    if (thread == nullptr) {
        APP_LOGE("AbilityThread::AbilityThreadMain failed,thread  is nullptr");
        return;
    }
    thread->Attach(application, abilityRecord);
}
}  // namespace AppExecFwk
}  // namespace OHOS