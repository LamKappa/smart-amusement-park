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

#include "ability_impl.h"
#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
void AbilityImpl::Init(std::shared_ptr<OHOSApplication> &application, const std::shared_ptr<AbilityLocalRecord> &record,
    std::shared_ptr<Ability> &ability, std::shared_ptr<AbilityHandler> &handler, const sptr<IRemoteObject> &token,
    std::shared_ptr<ContextDeal> &contextDeal)
{
    APP_LOGI("AbilityImpl::init begin");

    if ((token == nullptr) || (application == nullptr) || (handler == nullptr) || (record == nullptr) ||
        ability == nullptr || contextDeal == nullptr) {
        APP_LOGE("AbilityImpl::init failed, token is nullptr, application is nullptr, handler is nullptr, record is "
                 "nullptr, ability is nullptr, contextDeal is nullptr");
        return;
    }

    token_ = record->GetToken();
    record->SetAbilityImpl(shared_from_this());
    ability_ = ability;
    ability_->Init(record->GetAbilityInfo(), application, handler, token);
    lifecycleState_ = AAFwk::ABILITY_STATE_INITIAL;
    abilityLifecycleCallbacks_ = application;
    contextDeal_ = contextDeal;

    APP_LOGI("AbilityImpl::init end");
}

/**
 * @brief Toggles the lifecycle status of Ability to AAFwk::ABILITY_STATE_INACTIVE. And notifies the application
 * that it belongs to of the lifecycle status.
 *
 * @param want  The Want object to switch the life cycle.
 */
void AbilityImpl::Start(const Want &want)
{
    if (ability_ == nullptr || abilityLifecycleCallbacks_ == nullptr) {
        APP_LOGE("AbilityImpl::Start ability_ or abilityLifecycleCallbacks_ is nullptr");
        return;
    }

    APP_LOGI("AbilityImpl::Start");
    ability_->OnStart(want);
    if (ability_->GetAbilityInfo()->type == AbilityType::DATA) {
        lifecycleState_ = AAFwk::ABILITY_STATE_ACTIVE;
    } else {
        lifecycleState_ = AAFwk::ABILITY_STATE_INACTIVE;
    }

    abilityLifecycleCallbacks_->OnAbilityStart(ability_);
}

/**
 * @brief Toggles the lifecycle status of Ability to AAFwk::ABILITY_STATE_INITIAL. And notifies the application
 * that it belongs to of the lifecycle status.
 *
 */
void AbilityImpl::Stop()
{
    if (ability_ == nullptr || abilityLifecycleCallbacks_ == nullptr) {
        APP_LOGE("AbilityImpl::Stop ability_ or abilityLifecycleCallbacks_ is nullptr");
        return;
    }

    APP_LOGD("AbilityImpl::Stop");
    ability_->OnStop();
    lifecycleState_ = AAFwk::ABILITY_STATE_INITIAL;
    abilityLifecycleCallbacks_->OnAbilityStop(ability_);
}

/**
 * @brief Toggles the lifecycle status of Ability to AAFwk::ABILITY_STATE_ACTIVE. And notifies the application
 * that it belongs to of the lifecycle status.
 *
 */
void AbilityImpl::Active()
{
    if (ability_ == nullptr || abilityLifecycleCallbacks_ == nullptr) {
        APP_LOGE("AbilityImpl::Active ability_ or abilityLifecycleCallbacks_ is nullptr");
        return;
    }

    APP_LOGD("AbilityImpl::Active");
    ability_->OnActive();
    lifecycleState_ = AAFwk::ABILITY_STATE_ACTIVE;
    abilityLifecycleCallbacks_->OnAbilityActive(ability_);
}

/**
 * @brief Toggles the lifecycle status of Ability to AAFwk::ABILITY_STATE_INACTIVE. And notifies the application
 * that it belongs to of the lifecycle status.
 *
 */
void AbilityImpl::Inactive()
{
    if (ability_ == nullptr || abilityLifecycleCallbacks_ == nullptr) {
        APP_LOGE("AbilityImpl::Inactive ability_ or abilityLifecycleCallbacks_ is nullptr");
        return;
    }

    APP_LOGD("AbilityImpl::Inactive");
    ability_->OnInactive();
    lifecycleState_ = AAFwk::ABILITY_STATE_INACTIVE;
    abilityLifecycleCallbacks_->OnAbilityInactive(ability_);
}

/**
 * @brief Toggles the lifecycle status of Ability to AAFwk::ABILITY_STATE_INACTIVE. And notifies the application
 * that it belongs to of the lifecycle status.
 *
 * @param want The Want object to switch the life cycle.
 */
void AbilityImpl::Foreground(const Want &want)
{
    if (ability_ == nullptr || abilityLifecycleCallbacks_ == nullptr) {
        APP_LOGE("AbilityImpl::Foreground ability_ or abilityLifecycleCallbacks_ is nullptr");
        return;
    }

    APP_LOGD("AbilityImpl::Foreground");
    ability_->OnForeground(want);
    lifecycleState_ = AAFwk::ABILITY_STATE_INACTIVE;
    abilityLifecycleCallbacks_->OnAbilityForeground(ability_);
}

/**
 * @brief Toggles the lifecycle status of Ability to AAFwk::ABILITY_STATE_BACKGROUND. And notifies the application
 * that it belongs to of the lifecycle status.
 *
 */
void AbilityImpl::Background()
{
    if (ability_ == nullptr || abilityLifecycleCallbacks_ == nullptr) {
        APP_LOGE("AbilityImpl::Background ability_ or abilityLifecycleCallbacks_ is nullptr");
        return;
    }

    APP_LOGD("AbilityImpl::Background");
    ability_->OnLeaveForeground();
    ability_->OnBackground();
    lifecycleState_ = AAFwk::ABILITY_STATE_BACKGROUND;
    abilityLifecycleCallbacks_->OnAbilityBackground(ability_);
}

/**
 * @brief Save data and states of an ability when it is restored by the system. and Calling information back to Ability.
 *        This method should be implemented by a Page ability.
 * @param instate The Want object to connect to.
 *
 */
void AbilityImpl::DispatchSaveAbilityState(PacMap &outState)
{
    if (ability_ == nullptr || abilityLifecycleCallbacks_ == nullptr) {
        APP_LOGE("AbilityImpl::DispatchSaveAbilityState ability_ or abilityLifecycleCallbacks_ is nullptr");
        return;
    }

    APP_LOGD("AbilityImpl::DispatchSaveAbilityState");
    ability_->OnSaveAbilityState(outState);
    abilityLifecycleCallbacks_->OnAbilitySaveState(outState);
}

/**
 * @brief Restores data and states of an ability when it is restored by the system. and Calling information back to
 * Ability. This method should be implemented by a Page ability.
 * @param instate The Want object to connect to.
 *
 */
void AbilityImpl::DispatchRestoreAbilityState(const PacMap &inState)
{
    if (ability_ == nullptr) {
        APP_LOGE("AbilityImpl::DispatchRestoreAbilityState ability_ is nullptr");
        return;
    }

    APP_LOGD("AbilityImpl:: DispatchRestoreAbilityState");
    ability_->OnRestoreAbilityState(inState);
}

void AbilityImpl::HandleAbilityTransaction(const Want &want, const AAFwk::LifeCycleStateInfo &targetState)
{}

/**
 * @brief Connect the ability. and Calling information back to Ability.
 *
 * @param want The Want object to connect to.
 *
 */
sptr<IRemoteObject> AbilityImpl::ConnectAbility(const Want &want)
{
    if (ability_ == nullptr) {
        APP_LOGE("AbilityImpl::ConnectAbility ability_ is nullptr");
        return nullptr;
    }

    APP_LOGD("AbilityImpl:: ConnectAbility");
    sptr<IRemoteObject> object = ability_->OnConnect(want);
    lifecycleState_ = AAFwk::ABILITY_STATE_ACTIVE;
    abilityLifecycleCallbacks_->OnAbilityActive(ability_);

    return object;
}

/**
 * @brief Disconnects the connected object.
 *
 * @param want The Want object to disconnect to.
 */
void AbilityImpl::DisconnectAbility(const Want &want)
{
    if (ability_ == nullptr) {
        APP_LOGE("AbilityImpl::DisconnectAbility ability_ is nullptr");
        return;
    }

    ability_->OnDisconnect(want);
}

/**
 * @brief Command the ability. and Calling information back to Ability.
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
void AbilityImpl::CommandAbility(const Want &want, bool restart, int startId)
{
    if (ability_ == nullptr) {
        APP_LOGE("AbilityImpl::CommandAbility ability_ is nullptr");
        return;
    }

    APP_LOGD("AbilityImpl:: CommandAbility");
    ability_->OnCommand(want, restart, startId);
    lifecycleState_ = AAFwk::ABILITY_STATE_ACTIVE;
    abilityLifecycleCallbacks_->OnAbilityActive(ability_);
}

/**
 * @brief Gets the current Ability status.
 *
 */
int AbilityImpl::GetCurrentState()
{
    return lifecycleState_;
}

/**
 * @brief Execution the KeyDown callback of the ability
 * @param keyCode Indicates the code of the key pressed.
 * @param keyEvent Indicates the key-down event.
 *
 * @return Returns true if this event is handled and will not be passed further; returns false if this event is
 * not handled and should be passed to other handlers.
 *
 */
bool AbilityImpl::DoKeyDown(int keyCode, const KeyEvent &keyEvent)
{
    APP_LOGD("AbilityImpl::DoKeyDown called");
    return false;
}

/**
 * @brief Execution the KeyUp callback of the ability
 * @param keyCode Indicates the code of the key released.
 * @param keyEvent Indicates the key-up event.
 *
 * @return Returns true if this event is handled and will not be passed further; returns false if this event is
 * not handled and should be passed to other handlers.
 *
 */
bool AbilityImpl::DoKeyUp(int keyCode, const KeyEvent &keyEvent)
{
    APP_LOGD("AbilityImpl::DoKeyUp called");
    return false;
}

/**
 * @brief Called when a touch event is dispatched to this ability. The default implementation of this callback
 * does nothing and returns false.
 * @param touchEvent Indicates information about the touch event.
 *
 * @return Returns true if the event is handled; returns false otherwise.
 *
 */
bool AbilityImpl::DoTouchEvent(const TouchEvent &touchEvent)
{
    APP_LOGD("AbilityImpl::DoTouchEvent called");
    return false;
}

/**
 * @brief Send the result code and data to be returned by this Page ability to the caller.
 * When a Page ability is destroyed, the caller overrides the AbilitySlice#onAbilityResult(int, int, Want) method to
 * receive the result set in the current method. This method can be called only after the ability has been initialized.
 *
 * @param requestCode Indicates the request code.
 * @param resultCode Indicates the result code returned after the ability is destroyed. You can define the result code
 * to identify an error.
 * @param resultData Indicates the data returned after the ability is destroyed. You can define the data returned. This
 * parameter can be null.
 */
void AbilityImpl::SendResult(int requestCode, int resultCode, const Want &resultData)
{
    if (ability_ == nullptr) {
        APP_LOGE("AbilityImpl::SendResult ability_ is nullptr");
        return;
    }

    if (resultData.HasParameter(OHOS_PERMISSIONS_REQUEST_RESULT_KEY) &&
        resultData.HasParameter(OHOS_PERMISSIONS_REQUEST_KEY)) {

        std::vector<int> grant_result = resultData.GetIntArrayParam(OHOS_PERMISSIONS_REQUEST_RESULT_KEY);
        std::vector<std::string> permissions = resultData.GetStringArrayParam(OHOS_PERMISSIONS_REQUEST_KEY);
        if (permissions.size() > 0 && permissions.size() == grant_result.size()) {
            ability_->OnRequestPermissionsFromUserResult(requestCode, permissions, grant_result);
        } else {
            APP_LOGE("AbilityImpl::SendResult Grand failed, data error!");
        }
    } else {
        ability_->OnAbilityResult(requestCode, resultCode, resultData);
    }
}

/**
 * @brief Called when the launch mode of an ability is set to singleInstance. This happens when you re-launch
 * an ability that has been at the top of the ability stack.
 *
 * @param want  Indicates the new Want containing information about the ability.
 */
void AbilityImpl::NewWant(const Want &want)
{
    if (ability_ == nullptr) {
        APP_LOGE("AbilityImpl::NewWant ability_ is nullptr");
        return;
    }
    ability_->SetWant(want);
    ability_->OnNewWant(want);
}

/**
 * @brief Obtains the MIME types of files supported.
 *
 * @param uri Indicates the path of the files to obtain.
 * @param mimeTypeFilter Indicates the MIME types of the files to obtain. This parameter cannot be null.
 *
 * @return Returns the matched MIME types. If there is no match, null is returned.
 */
std::vector<std::string> AbilityImpl::GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter)
{
    std::vector<std::string> types;
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
int AbilityImpl::OpenFile(const Uri &uri, const std::string &mode)
{
    return -1;
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
int AbilityImpl::OpenRawFile(const Uri &uri, const std::string &mode)
{
    return -1;
}

/**
 * @brief Inserts a single data record into the database.
 *
 * @param uri Indicates the path of the data to operate.
 * @param value  Indicates the data record to insert. If this parameter is null, a blank row will be inserted.
 *
 * @return Returns the index of the inserted data record.
 */
int AbilityImpl::Insert(const Uri &uri, const ValuesBucket &value)
{
    return -1;
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
int AbilityImpl::Update(const Uri &uri, const ValuesBucket &value, const DataAbilityPredicates &predicates)
{
    return -1;
}

/**
 * @brief Deletes one or more data records from the database.
 *
 * @param uri Indicates the path of the data to operate.
 * @param predicates Indicates filter criteria. You should define the processing logic when this parameter is null.
 *
 * @return Returns the number of data records deleted.
 */
int AbilityImpl::Delete(const Uri &uri, const DataAbilityPredicates &predicates)
{
    return -1;
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
std::shared_ptr<ResultSet> AbilityImpl::Query(
    const Uri &uri, std::vector<std::string> &columns, const DataAbilityPredicates &predicates)
{
    return nullptr;
}

/**
 * @brief Obtains the MIME type matching the data specified by the URI of the Data ability. This method should be
 * implemented by a Data ability. Data abilities supports general data types, including text, HTML, and JPEG.
 *
 * @param uri Indicates the URI of the data.
 *
 * @return Returns the MIME type that matches the data specified by uri.
 */
std::string AbilityImpl::GetType(const Uri &uri)
{
    return "";
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
bool AbilityImpl::Reload(const Uri &uri, const PacMap &extras)
{
    return false;
}

/**
 * @brief Inserts multiple data records into the database.
 *
 * @param uri Indicates the path of the data to operate.
 * @param values Indicates the data records to insert.
 *
 * @return Returns the number of data records inserted.
 */
int AbilityImpl::BatchInsert(const Uri &uri, const std::vector<ValuesBucket> &values)
{
    return -1;
}

/**
 * @brief SerUriString
 */
void AbilityImpl::SerUriString(const std::string &uri)
{
    if (contextDeal_ == nullptr) {
        APP_LOGE("AbilityImpl::SerUriString contextDeal_ is nullptr");
        return;
    }
    contextDeal_->SerUriString(uri);
}

/**
 * @brief Set deviceId/bundleName/abilityName of the calling ability
 *
 * @param deviceId deviceId of the calling ability
 *
 * @param deviceId bundleName of the calling ability
 *
 * @param deviceId abilityName of the calling ability
 */
void AbilityImpl::SetCallingContext(
    const std::string &deviceId, const std::string &bundleName, const std::string &abilityName)
{
    if (ability_ != nullptr) {
        ability_->SetCallingContext(deviceId, bundleName, abilityName);
    }
}
}  // namespace AppExecFwk
}  // namespace OHOS