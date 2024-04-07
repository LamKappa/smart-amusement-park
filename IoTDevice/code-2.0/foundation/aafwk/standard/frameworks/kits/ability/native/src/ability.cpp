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

#include "ability.h"
#include "ability_loader.h"
#include "app_log_wrapper.h"
#include "display_type.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "task_handler_client.h"
#include "ohos_application.h"

namespace OHOS {
namespace AppExecFwk {
REGISTER_AA(Ability)
const std::string Ability::SYSTEM_UI("com.ohos.systemui");
const std::string Ability::STATUS_BAR("com.ohos.systemui.statusbar.MainAbility");
const std::string Ability::NAVIGATION_BAR("com.ohos.systemui.navigationbar.MainAbility");

void Ability::Init(const std::shared_ptr<AbilityInfo> &abilityInfo, const std::shared_ptr<OHOSApplication> &application,
    std::shared_ptr<AbilityHandler> &handler, const sptr<IRemoteObject> &token)
{
    APP_LOGI("Ability::Init called.");

    abilityInfo_ = abilityInfo;
    handler_ = handler;
    AbilityContext::token_ = token;

    // page ability only.
    if (abilityInfo_->type == AbilityType::PAGE) {
        abilityWindow_ = std::make_shared<AbilityWindow>();
#ifdef WMS_COMPILE
        if (abilityWindow_ != nullptr) {
            abilityWindow_->Init(handler_, shared_from_this());
        }
#endif
    }
    lifecycle_ = std::make_shared<LifeCycle>();
    abilityLifecycleExecutor_ = std::make_shared<AbilityLifecycleExecutor>();
    application_ = application;
}

/**
 * @brief Obtains a resource manager.
 *
 * @return Returns a ResourceManager object.
 */
std::shared_ptr<Global::Resource::ResourceManager> Ability::GetResourceManager() const
{
    return AbilityContext::GetResourceManager();
}

/**
 * Will be called when ability start. You should override this function
 *
 * @param want ability start information
 */
void Ability::OnStart(const Want &want)
{
    APP_LOGI("Ability::OnStart called.");
#ifdef WMS_COMPILE
    if (abilityInfo_ == nullptr) {
        APP_LOGE("Ability::OnStart falied abilityInfo_ is nullptr.");
        return;
    }

    if (abilityInfo_->type == AppExecFwk::AbilityType::PAGE) {
        WindowConfig config = {};  // should called in ace ability onStart methord.
        config.height = WindowManager::GetInstance()->GetMaxHeight();
        config.width = WindowManager::GetInstance()->GetMaxWidth();
        config.format = PIXEL_FMT_RGBA_8888;
        config.pos_x = 0;
        config.pos_y = 0;

        if (abilityInfo_->bundleName == SYSTEM_UI) {
            if (abilityInfo_->name == STATUS_BAR) {
                config.type = OHOS::WindowType::WINDOW_TYPE_STATUS_BAR;
                APP_LOGI("Ability::OnStart STATUS_BAR : set config.type = %{public}d", config.type);
            }
            if (abilityInfo_->name == NAVIGATION_BAR) {
                config.type = OHOS::WindowType::WINDOW_TYPE_NAVI_BAR;
                APP_LOGI("Ability::OnStart NAVIGATION_BAR : set config.type = %{public}d", config.type);
            }
        }
        SetUIContent(config);

        if (abilityWindow_ != nullptr) {
            abilityWindow_->OnPostAbilityStart();
        }
    }
    // should called in ace ability onStart methord.
#endif  // WMS_COMPILE

    SetWant(want);
    if (abilityLifecycleExecutor_ == nullptr) {
        APP_LOGE("Ability::OnStart error. abilityLifecycleExecutor_ == nullptr.");
        return;
    }
    abilityLifecycleExecutor_->DispatchLifecycleState(AbilityLifecycleExecutor::LifecycleState::INACTIVE);

    if (lifecycle_ == nullptr) {
        APP_LOGE("Ability::OnStart error. lifecycle_ == nullptr.");
        return;
    }
    lifecycle_->DispatchLifecycle(LifeCycle::Event::ON_START, want);
}

/**
 * @brief Called when this ability enters the <b>STATE_STOP</b> state.
 *
 * The ability in the <b>STATE_STOP</b> is being destroyed.
 * You can override this function to implement your own processing logic.
 */
void Ability::OnStop()
{
    APP_LOGI("Ability::OnStop called");

    if (abilityWindow_ != nullptr && abilityInfo_->type == AppExecFwk::AbilityType::PAGE) {
        abilityWindow_->OnPostAbilityStop();
    }
    if (abilityLifecycleExecutor_ == nullptr) {
        APP_LOGE("Ability::OnStop error. abilityLifecycleExecutor_ == nullptr.");
        return;
    }
    abilityLifecycleExecutor_->DispatchLifecycleState(AbilityLifecycleExecutor::LifecycleState::INITIAL);

    if (lifecycle_ == nullptr) {
        APP_LOGE("Ability::OnStop error. lifecycle_ == nullptr.");
        return;
    }
    lifecycle_->DispatchLifecycle(LifeCycle::Event::ON_STOP);
}

/**
 * @brief Called when this ability enters the <b>STATE_ACTIVE</b> state.
 *
 * The ability in the <b>STATE_ACTIVE</b> state is visible and has focus.
 * You can override this function to implement your own processing logic.
 *
 * @param Want Indicates the {@link Want} structure containing activation information about the ability.
 */
void Ability::OnActive()
{
    APP_LOGI("Ability::OnActive called");

    if (abilityWindow_ != nullptr) {
        abilityWindow_->OnPostAbilityActive();
    }
    bWindowFocus_ = true;
    if (abilityLifecycleExecutor_ == nullptr) {
        APP_LOGE("Ability::OnActive error. abilityLifecycleExecutor_ == nullptr.");
        return;
    }
    abilityLifecycleExecutor_->DispatchLifecycleState(AbilityLifecycleExecutor::LifecycleState::ACTIVE);

    if (lifecycle_ == nullptr) {
        APP_LOGE("Ability::OnActive error. lifecycle_ == nullptr.");
        return;
    }
    lifecycle_->DispatchLifecycle(LifeCycle::Event::ON_ACTIVE);
}

/**
 * @brief Called when this ability enters the <b>STATE_INACTIVE</b> state.
 *
 * <b>STATE_INACTIVE</b> is an instantaneous state. The ability in this state may be visible but does not have
 * focus.You can override this function to implement your own processing logic.
 */
void Ability::OnInactive()
{
    APP_LOGI("Ability::OnInactive called");

    if (abilityWindow_ != nullptr && abilityInfo_->type == AppExecFwk::AbilityType::PAGE) {
        abilityWindow_->OnPostAbilityInactive();
    }
    bWindowFocus_ = false;
    if (abilityLifecycleExecutor_ == nullptr) {
        APP_LOGE("Ability::OnInactive error. abilityLifecycleExecutor_ == nullptr.");
        return;
    }
    abilityLifecycleExecutor_->DispatchLifecycleState(AbilityLifecycleExecutor::LifecycleState::INACTIVE);

    if (lifecycle_ == nullptr) {
        APP_LOGE("Ability::OnInactive error. lifecycle_ == nullptr.");
        return;
    }
    lifecycle_->DispatchLifecycle(LifeCycle::Event::ON_INACTIVE);
}

/**
 * @brief Called when this ability enters the <b>STATE_FOREGROUND</b> state.
 *
 *
 * The ability in the <b>STATE_FOREGROUND</b> state is visible.
 * You can override this function to implement your own processing logic.
 */
void Ability::OnForeground(const Want &want)
{
    APP_LOGI("Ability::OnForeground called");

    if (abilityWindow_ != nullptr) {
        abilityWindow_->OnPostAbilityForeground();
    }

    if (abilityLifecycleExecutor_ == nullptr) {
        APP_LOGE("Ability::OnForeground error. abilityLifecycleExecutor_ == nullptr.");
        return;
    }
    abilityLifecycleExecutor_->DispatchLifecycleState(AbilityLifecycleExecutor::LifecycleState::INACTIVE);

    if (lifecycle_ == nullptr) {
        APP_LOGE("Ability::OnForeground error. lifecycle_ == nullptr.");
        return;
    }
    lifecycle_->DispatchLifecycle(LifeCycle::Event::ON_FOREGROUND, want);
}

/**
 * @brief Called when this ability enters the <b>STATE_BACKGROUND</b> state.
 *
 *
 * The ability in the <b>STATE_BACKGROUND</b> state is invisible.
 * You can override this function to implement your own processing logic.
 */
void Ability::OnBackground()
{
    APP_LOGI("Ability::OnBackground called");

    if (abilityWindow_ != nullptr && abilityInfo_->type == AppExecFwk::AbilityType::PAGE) {
        abilityWindow_->OnPostAbilityBackground();
    }

    if (abilityLifecycleExecutor_ == nullptr) {
        APP_LOGE("Ability::OnBackground error. abilityLifecycleExecutor_ == nullptr.");
        return;
    }
    abilityLifecycleExecutor_->DispatchLifecycleState(AbilityLifecycleExecutor::LifecycleState::BACKGROUND);

    if (lifecycle_ == nullptr) {
        APP_LOGE("Ability::OnBackground error. lifecycle_ == nullptr.");
        return;
    }
    lifecycle_->DispatchLifecycle(LifeCycle::Event::ON_BACKGROUND);
}

/**
 * @brief Called when this Service ability is connected for the first time.
 *
 * You can override this function to implement your own processing logic.
 *
 * @param want Indicates the {@link Want} structure containing connection information about the Service ability.
 * @return Returns a pointer to the <b>sid</b> of the connected Service ability.
 */
sptr<IRemoteObject> Ability::OnConnect(const Want &want)
{
    if (abilityLifecycleExecutor_ == nullptr) {
        APP_LOGE("Ability::OnConnect error. abilityLifecycleExecutor_ == nullptr.");
        return nullptr;
    }
    abilityLifecycleExecutor_->DispatchLifecycleState(AbilityLifecycleExecutor::LifecycleState::ACTIVE);

    if (lifecycle_ == nullptr) {
        APP_LOGE("Ability::OnConnect error. lifecycle_ == nullptr.");
        return nullptr;
    }
    lifecycle_->DispatchLifecycle(LifeCycle::Event::ON_ACTIVE);

    return nullptr;
}

/**
 * @brief Called when all abilities connected to this Service ability are disconnected.
 *
 * You can override this function to implement your own processing logic.
 *
 */
void Ability::OnDisconnect(const Want &want)
{}

/**
 * Start other ability for result.
 *
 * @param want information of other ability
 * @param requestCode request code for abilityMS to return result
 */
void Ability::StartAbilityForResult(const Want &want, int requestCode)
{
    if (abilityInfo_ == nullptr) {
        APP_LOGE("Ability::StartAbilityForResult abilityInfo_ == nullptr");
        return;
    }
    APP_LOGI("Ability::StartAbilityForResult called type = %{public}d", abilityInfo_->type);
    if (abilityInfo_->type == AppExecFwk::AbilityType::PAGE) {
        AbilityContext::StartAbility(want, requestCode);
    }
}

/**
 * Starts an ability with specific start settings and returns the execution result when the ability is destroyed.
 * When the ability is destroyed, onAbilityResult(int,int,ohos.aafwk.content.Want) is called and the returned
 * requestCode is transferred to the current method. The given requestCode is customized and cannot be a negative
 * number.
 *
 * @param want Indicates the ability to start.
 * @param requestCode Indicates the request code returned after the ability is started. You can define the request code
 * to identify the results returned by abilities. The value ranges from 0 to 65535.
 * @param abilityStartSetting Indicates the setting ability used to start.
 */
void Ability::StartAbilityForResult(const Want &want, int requestCode, AbilityStartSetting abilityStartSetting)
{
    if (abilityInfo_ == nullptr) {
        APP_LOGE("Ability::StartAbilityForResult abilityInfo_ == nullptr");
        return;
    }
    APP_LOGI("Ability::StartAbilityForResult called type = %{public}d", abilityInfo_->type);
    if (abilityInfo_->type == AppExecFwk::AbilityType::PAGE) {
        AbilityContext::StartAbility(want, requestCode, abilityStartSetting);
    }
}

/**
 * Starts a new ability with specific start settings.
 * A Page or Service ability uses this method to start a specific ability.
 * The system locates the target ability from installed abilities based on
 * the value of the intent parameter and then starts it. You can specify the
 * ability to start using the intent parameter.
 *
 * @param want Indicates the ability to start.
 * @param abilityStartSetting Indicates the setting ability used to start.
 */
void Ability::StartAbility(const Want &want, AbilityStartSetting abilityStartSetting)
{
    if (abilityInfo_ == nullptr) {
        APP_LOGE("Ability::StartAbility abilityInfo_ == nullptr");
        return;
    }
    APP_LOGI("Ability::StartAbility called type = %{public}d", abilityInfo_->type);
    if (abilityInfo_->type == AppExecFwk::AbilityType::PAGE || abilityInfo_->type == AppExecFwk::AbilityType::SERVICE) {
        AbilityContext::StartAbility(want, -1, abilityStartSetting);
    }
}

/**
 * @brief Called when a key is pressed. When any component in the Ability gains focus, the key-down event for
 * the component will be handled first. This callback will not be invoked if the callback triggered for the
 * key-down event of the component returns true. The default implementation of this callback does nothing
 * and returns false.
 *
 * @param keyCode Indicates the code of the key pressed.
 * @param keyEvent Indicates the key-down event.
 *
 * @return Returns true if this event is handled and will not be passed further; returns false if this event
 * is not handled and should be passed to other handlers.
 */
bool Ability::OnKeyDown(int keyCode, const KeyEvent &keyEvent)
{
    APP_LOGI("Ability::OnKeyDown called");
    return false;
}

/**
 * @brief Called when a key is released. When any component in the Ability gains focus, the key-up event for
 * the component will be handled first. This callback will not be invoked if the callback triggered for the
 * key-up event of the component returns true. The default implementation of this callback does nothing and
 * returns false.
 *
 * @param keyCode Indicates the code of the key released.
 * @param keyEvent Indicates the key-up event.
 *
 * @return Returns true if this event is handled and will not be passed further; returns false if this event
 * is not handled and should be passed to other handlers.
 */
bool Ability::OnKeyUp(int keyCode, const KeyEvent &keyEvent)
{
    APP_LOGI("Ability::OnKeyUp called");
    return false;
}

/**
 * @brief Called when a touch event is dispatched to this ability. The default implementation of this callback
 * does nothing and returns false.
 *
 * @param event  Indicates information about the touch event.
 *
 * @return Returns true if the event is handled; returns false otherwise.
 */
bool Ability::OnTouchEvent(const TouchEvent &touchEvent)
{
    APP_LOGI("Ability::OnTouchEvent called");
    return false;
}

/**
 * @brief Inflates UI controls by using ComponentContainer.
 * You can create a ComponentContainer instance that contains multiple components.
 *
 * @param componentContainer Indicates a set of customized components.
 */
void Ability::SetUIContent(const ComponentContainer &componentContainer)
{}

/**
 * @brief Inflates layout resources by using the layout resource ID.
 *
 * @param layoutRes Indicates the layout resource ID, which cannot be a negative number.
 */
void Ability::SetUIContent(int layoutRes)
{}

/**
 * @brief Inflates UI controls by using ComponentContainer.
 * You can create a ComponentContainer instance that contains multiple components.
 *
 * @param componentContainer Indicates the component layout defined by the user.
 * @param context Indicates the context to use.
 * @param typeFlag Indicates the window type.
 */
void Ability::SetUIContent(
    const ComponentContainer &componentContainer, std::shared_ptr<Context> &context, int typeFlag)
{}

/**
 * @brief Inflates layout resources by using the layout resource ID.
 *
 * @param layoutRes Indicates the layout resource ID, which cannot be a negative number.
 * @param context Indicates the context to use.
 * @param typeFlag Indicates the window type.
 */
void Ability::SetUIContent(int layoutRes, std::shared_ptr<Context> &context, int typeFlag)
{}

#ifdef WMS_COMPILE
/**
 * @brief Inflates UI controls by using WindowConfig.
 *
 * @param config Indicates the window config defined by the user.
 */
void Ability::SetUIContent(const WindowConfig &config)
{
    if (abilityInfo_ == nullptr) {
        APP_LOGE("Ability::SetUIContent abilityWindow_ is nullptr");
        return;
    }

    APP_LOGI("Ability::SetUIContent called");
    abilityWindow_->SetWindowConfig(config);
}

/**
 * @brief Get the window belong to the ability.
 *
 * @return Returns a IWindowsManager object pointer.
 */
const std::unique_ptr<Window> &Ability::GetWindow()
{
    return abilityWindow_->GetWindow();
}
#endif  // WMS_COMPILE

/**
 * @brief Obtains the type of audio whose volume is adjusted by the volume button.
 *
 * @return Returns the AudioManager.AudioVolumeType.
 */
int Ability::GetVolumeTypeAdjustedByKey()
{
    return 0;
}

/**
 * @brief Checks whether the main window of this ability has window focus.
 *
 * @return Returns true if this ability currently has window focus; returns false otherwise.
 */
bool Ability::HasWindowFocus()
{
    if (abilityInfo_ == nullptr) {
        APP_LOGI("Ability::HasWindowFocus abilityInfo_ == nullptr");
        return false;
    }

    if (abilityInfo_->type == AppExecFwk::AbilityType::PAGE) {
        return bWindowFocus_;
    }

    return false;
}

/**
 * @brief Called when a key is lone pressed.
 *
 * @param keyCode Indicates the code of the key long pressed.
 * @param keyEvent Indicates the key-long-press event.
 *
 * @return Returns true if this event is handled and will not be passed further; returns false if this event
 * is not handled and should be passed to other handlers.
 */
bool Ability::OnKeyPressAndHold(int keyCode, const std::shared_ptr<KeyEvent> &keyEvent)
{
    return false;
}

/**
 * @brief Called back after permissions are requested by using
 * AbilityContext.requestPermissionsFromUser(java.lang.String[],int).
 *
 * @param requestCode Indicates the request code passed to this method from
 * AbilityContext.requestPermissionsFromUser(java.lang.String[],int).
 * @param permissions Indicates the list of permissions requested by using
 * AbilityContext.requestPermissionsFromUser(java.lang.String[],int). This parameter cannot be null.
 * @param grantResults Indicates the granting results of the corresponding permissions requested using
 * AbilityContext.requestPermissionsFromUser(java.lang.String[],int). The value 0 indicates that a
 * permission is granted, and the value -1 indicates not.
 *
 */
void Ability::OnRequestPermissionsFromUserResult(
    int requestCode, const std::vector<std::string> &permissions, const std::vector<int> &grantResults)
{}

/**
 * @brief Called when this ability is about to leave the foreground and enter the background due to a user operation,
 * for example, when the user touches the Home key.
 *
 */
void Ability::OnLeaveForeground()
{}

/**
 * @brief Obtains the MIME type matching the data specified by the URI of the Data ability. This method should be
 * implemented by a Data ability. Data abilities supports general data types, including text, HTML, and JPEG.
 *
 * @param uri Indicates the URI of the data.
 *
 * @return Returns the MIME type that matches the data specified by uri.
 */
std::string Ability::GetType(const Uri &uri)
{
    return "";
}

/**
 * @brief Inserts a data record into the database. This method should be implemented by a Data ability.
 *
 * @param uri Indicates the position where the data is to insert.
 * @param value Indicates the data to insert.
 *
 * @return Returns the index of the newly inserted data record.
 */
int Ability::Insert(const Uri &uri, const ValuesBucket &value)
{
    return 0;
}

/**
 * @brief Called when the system configuration is updated.
 *
 * @param configuration Indicates the updated configuration information.
 */
void Ability::OnConfigurationUpdated(const Configuration &configuration)
{}

/**
 * @brief Called when the system configuration is updated.
 *
 * @param level Indicates the memory trim level, which shows the current memory usage status.
 *
 */
void Ability::OnMemoryLevel(int level)
{}

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
int Ability::OpenRawFile(const Uri &uri, const std::string &mode)
{
    return -1;
}

/**
 * @brief Updates one or more data records in the database. This method should be implemented by a Data ability.
 *
 * @param uri Indicates the database table storing the data to update.
 * @param value Indicates the data to update. This parameter can be null.
 * @param predicates Indicates filter criteria. If this parameter is null, all data records will be updated by default.
 *
 * @return Returns the number of data records updated.
 */
int Ability::Update(const Uri &uri, const ValuesBucket &value, const DataAbilityPredicates &predicates)
{
    return 0;
}

/**
 * @brief get application witch the ability belong
 *
 * @return Returns the application ptr
 */
std::shared_ptr<OHOSApplication> Ability::GetApplication()
{
    APP_LOGI("Ability::GetApplication called");

    if (application_ == nullptr) {
        APP_LOGE("Ability::GetApplication error. application_ == nullptr.");
        return nullptr;
    }

    return application_;
}

/**
 * @brief Obtains the class name in this ability name, without the prefixed bundle name.
 *
 * @return Returns the class name of this ability.
 */
std::string Ability::GetAbilityName()
{
    if (abilityInfo_ == nullptr) {
        APP_LOGE("Ability::GetAbilityName abilityInfo_ is nullptr");
        return "";
    }

    return abilityInfo_->name;
}

/**
 * @brief OChecks whether the current ability is being destroyed.
 * An ability is being destroyed if you called terminateAbility() on it or someone else requested to destroy it.
 *
 * @return Returns true if the current ability is being destroyed; returns false otherwise.
 */
bool Ability::IsTerminating()
{
    return false;
}

/**
 * @brief Called when startAbilityForResult(ohos.aafwk.content.Want,int) is called to start an ability and the result is
 * returned. This method is called only on Page abilities. You can start a new ability to perform some calculations and
 * use setResult (int,ohos.aafwk.content.Want) to return the calculation result. Then the system calls back the current
 * method to use the returned data to execute its own logic.
 *
 * @param requestCode Indicates the request code returned after the ability is started. You can define the request code
 * to identify the results returned by abilities. The value ranges from 0 to 65535.
 * @param resultCode Indicates the result code returned after the ability is started. You can define the result code to
 * identify an error.
 * @param resultData Indicates the data returned after the ability is started. You can define the data returned. The
 * value can be null.
 *
 */
void Ability::OnAbilityResult(int requestCode, int resultCode, const Want &want)
{}

/**
 * @brief Called back when the Back key is pressed.
 * The default implementation destroys the ability. You can override this method.
 *
 */
void Ability::OnBackPressed()
{
    APP_LOGI("Ability::OnBackPressed called");
    if (abilityInfo_ == nullptr) {
        APP_LOGE("Ability::OnBackPressed abilityInfo_ is nullptr");
        return;
    }

    if (abilityInfo_->isLauncherAbility == false) {
        APP_LOGI("Ability::OnBackPressed the ability is not Launcher");
        TerminateAbility();
    }
}

/**
 * @brief Called when the launch mode of an ability is set to singleInstance. This happens when you re-launch an
 * ability that has been at the top of the ability stack.
 *
 * @param want Indicates the new Want containing information about the ability.
 */
void Ability::OnNewWant(const Want &want)
{
    APP_LOGI("Ability::OnNewWant called");
}

/**
 * @brief Restores data and states of an ability when it is restored by the system. This method should be implemented by
 * a Page ability. This method is called if an ability was destroyed at a certain time due to resource reclaim or was
 * unexpectedly destroyed and the onSaveAbilityState(ohos.utils.PacMap) method was called to save its user data and
 * states. Generally, this method is called after the onStart(ohos.aafwk.content.Want) method.
 *
 *  @param inState Indicates the PacMap object used for storing data and states. This parameter can not be null.
 *
 */
void Ability::OnRestoreAbilityState(const PacMap &inState)
{
    APP_LOGI("Ability::OnRestoreAbilityState called");
}

/**
 * @brief Saves temporary data and states of this ability. This method should be implemented by a Page ability.
 * This method is called when the system determines that the ability may be destroyed in an unexpected situation, for
 * example, when the screen orientation changes or the user touches the Home key. Generally, this method is used only to
 * save temporary states.
 *
 *  @param outState Indicates the PacMap object used for storing user data and states. This parameter cannot be null.
 *
 */
void Ability::OnSaveAbilityState(PacMap &outState)
{
    APP_LOGI("Ability::OnSaveAbilityState called");
}

/**
 * @brief Called every time a key, touch, or trackball event is dispatched to this ability.
 * You can override this callback method if you want to know that the user has interacted with
 * the device in a certain way while this ability is running. This method, together with onLeaveForeground(),
 * is designed to help abilities intelligently manage status bar notifications. Specifically, they help
 * abilities determine when to cancel a notification.
 *
 */
void Ability::OnEventDispatch()
{}

/**
 * @brief Called when this ability gains or loses window focus.
 * The focus state refers to the global state, which is independent of the ability lifecycle states.
 * Although the lifecycle state change of an ability may lead to a focus change (for example, onStop()
 * may cause the ability to lose window focus), there is no particular sequence between this callback
 * and other lifecycle callbacks such as onBackground().
 * Generally, the ability in the foreground will have window focus. The ability will lose focus when
 * another dialog box or pop-up window is displayed. The ability will also lose focus when a system-level
 * window (such as the status bar or a system alarm) is displayed.
 *
 * @param hasFocus Specifies whether this ability has focus.
 */
void Ability::OnWindowFocusChanged(bool hasFocus)
{}

/**
 * @brief Sets the want object that can be obtained by calling getWant().
 * @param Want information of other ability
 */
void Ability::SetWant(const AAFwk::Want &want)
{
    setWant_ = std::make_shared<AAFwk::Want>(want);
}

/**
 * @brief Obtains the Want object that starts this ability.
 *
 * @return Returns the Want object that starts this ability.
 */
std::shared_ptr<AAFwk::Want> Ability::GetWant()
{
    return setWant_;
}

/**
 * @brief Sets the result code and data to be returned by this Page ability to the caller.
 * When a Page ability is destroyed, the caller overrides the AbilitySlice#onAbilityResult(int, int, Want) method to
 * receive the result set in the current method. This method can be called only after the ability has been initialized.
 *
 * @param resultCode Indicates the result code returned after the ability is destroyed. You can define the result code
 * to identify an error.
 * @param resultData Indicates the data returned after the ability is destroyed. You can define the data returned. This
 * parameter can be null.
 */
void Ability::SetResult(int resultCode, const Want &resultData)
{
    if (abilityInfo_ == nullptr) {
        APP_LOGE("Ability::SetResult abilityInfo_ == nullptr");
        return;
    }
    APP_LOGI("Ability::SetResult called type = %{public}d", abilityInfo_->type);
    if (abilityInfo_->type == AppExecFwk::AbilityType::PAGE) {
        AbilityContext::resultWant_ = resultData;
        AbilityContext::resultCode_ = resultCode;
    }
}

/**
 * @brief Sets the type of audio whose volume will be adjusted by the volume button.
 *
 * @param volumeType Indicates the AudioManager.AudioVolumeType to set.
 */
void Ability::SetVolumeTypeAdjustedByKey(int volumeType)
{}

/**
 * @brief Called back when Service is started.
 * This method can be called only by Service. You can use the StartAbility(ohos.aafwk.content.Want) method to start
 * Service. Then the system calls back the current method to use the transferred want parameter to execute its own
 * logic.
 *
 * @param want Indicates the want of Service to start.
 * @param restart Indicates the startup mode. The value true indicates that Service is restarted after being destroyed,
 * and the value false indicates a normal startup.
 * @param startId Indicates the number of times the Service ability has been started. The startId is incremented by 1
 * every time the ability is started. For example, if the ability has been started for six times, the value of startId
 * is 6.
 */
void Ability::OnCommand(const AAFwk::Want &want, bool restart, int startId)
{
    if (abilityLifecycleExecutor_ == nullptr) {
        APP_LOGE("Ability::OnCommand error. abilityLifecycleExecutor_ == nullptr.");
        return;
    }
    abilityLifecycleExecutor_->DispatchLifecycleState(AbilityLifecycleExecutor::LifecycleState::ACTIVE);

    if (lifecycle_ == nullptr) {
        APP_LOGE("Ability::OnCommand error. lifecycle_ == nullptr.");
        return;
    }
    lifecycle_->DispatchLifecycle(LifeCycle::Event::ON_ACTIVE);
}

/**
 * @brief dump ability info
 *
 * @param extra dump ability info
 */
void Ability::Dump(const std::string &extra)
{
    APP_LOGI("Ability::Dump called");
    // abilityInfo
    APP_LOGI("==============AbilityInfo==============");
    if (abilityInfo_ != nullptr) {
        APP_LOGI("abilityInfo: package: %{public}s", abilityInfo_->package.c_str());
        APP_LOGI("abilityInfo: name: %{public}s", abilityInfo_->name.c_str());
        APP_LOGI("abilityInfo: label: %{public}s", abilityInfo_->label.c_str());
        APP_LOGI("abilityInfo: description: %{public}s", abilityInfo_->description.c_str());
        APP_LOGI("abilityInfo: iconPath: %{public}s", abilityInfo_->iconPath.c_str());
        APP_LOGI("abilityInfo: visible: %{public}d", abilityInfo_->visible);
        APP_LOGI("abilityInfo: kind: %{public}s", abilityInfo_->kind.c_str());
        APP_LOGI("abilityInfo: type: %{public}d", abilityInfo_->type);
        APP_LOGI("abilityInfo: orientation: %{public}d", abilityInfo_->orientation);
        APP_LOGI("abilityInfo: launchMode: %{public}d", abilityInfo_->launchMode);
        for (auto permission : abilityInfo_->permissions) {
            APP_LOGI("abilityInfo: permission: %{public}s", permission.c_str());
        }
        APP_LOGI("abilityInfo: bundleName: %{public}s", abilityInfo_->bundleName.c_str());
        APP_LOGI("abilityInfo: applicationName: %{public}s", abilityInfo_->applicationName.c_str());
    } else {
        APP_LOGI("abilityInfo is nullptr");
    }

    // lifecycle_Event
    APP_LOGI("==============lifecycle_Event==============");
    if (lifecycle_ != nullptr) {
        APP_LOGI("lifecycle_Event: launchMode: %{public}d", lifecycle_->GetLifecycleState());
    } else {
        APP_LOGI("lifecycle is nullptr");
    }

    // lifecycle_State
    APP_LOGI("==============lifecycle_State==============");
    if (abilityLifecycleExecutor_ != nullptr) {
        APP_LOGI("lifecycle_State: launchMode: %{public}d", abilityLifecycleExecutor_->GetState());
    } else {
        APP_LOGI("abilityLifecycleExecutor is nullptr");
    }

    // applicationInfo
    APP_LOGI("==============applicationInfo==============");
    std::shared_ptr<ApplicationInfo> ApplicationInfoPtr = GetApplicationInfo();
    if (ApplicationInfoPtr != nullptr) {
        APP_LOGI("applicationInfo: name: %{public}s", ApplicationInfoPtr->name.c_str());
        APP_LOGI("applicationInfo: bundleName: %{public}s", ApplicationInfoPtr->bundleName.c_str());
    } else {
        APP_LOGI("ApplicationInfoPtr is nullptr");
    }
}

/**
 * @brief Keeps this Service ability in the background and displays a notification bar.
 * To use this method, you need to request the ohos.permission.KEEP_BACKGROUND_RUNNING permission from the system.
 * The ohos.permission.KEEP_BACKGROUND_RUNNING permission is of the normal level.
 * This method can be called only by Service abilities after the onStart(ohos.aafwk.content.Want) method is called.
 *
 * @param id Identifies the notification bar information.
 * @param notificationRequest Indicates the NotificationRequest instance containing information for displaying a
 * notification bar.
 */
void Ability::KeepBackgroundRunning(int id, const NotificationRequest &notificationRequest)
{}

/**
 * @brief Cancels background running of this ability to free up system memory.
 * This method can be called only by Service abilities when the onStop() method is called.
 *
 */
void Ability::CancelBackgroundRunning()
{}

/**
 * @brief Converts the given uri that refer to the Data ability into a normalized URI. A normalized URI can be used
 * across devices, persisted, backed up, and restored. It can refer to the same item in the Data ability even if the
 * context has changed. If you implement URI normalization for a Data ability, you must also implement
 * denormalizeUri(ohos.utils.net.Uri) to enable URI denormalization. After this feature is enabled, URIs passed to any
 * method that is called on the Data ability must require normalization verification and denormalization. The default
 * implementation of this method returns null, indicating that this Data ability does not support URI normalization.
 *
 * @param uri Indicates the Uri object to normalize.
 *
 * @return Returns the normalized Uri object if the Data ability supports URI normalization; returns null otherwise.
 */
const std::shared_ptr<Uri> Ability::NormalizeUri(const Uri &uri)
{
    return nullptr;
}

/**
 * @brief Deletes one or more data records. This method should be implemented by a Data ability.
 *
 * @param uri Indicates the database table storing the data to delete.
 * @param predicates Indicates filter criteria. If this parameter is null, all data records will be deleted by default.
 *
 * @return Returns the number of data records deleted.
 */
int Ability::Delete(const Uri &uri, const DataAbilityPredicates &predicates)
{
    return 0;
}

/**
 * @brief Obtains the MIME type of files. This method should be implemented by a Data ability.
 *
 * @param uri Indicates the path of the files to obtain.
 * @param mimeTypeFilter Indicates the MIME type of the files to obtain. This parameter cannot be set to null.
 * 1. * / *: Obtains all types supported by a Data ability.
 * 2. image/ *: Obtains files whose main type is image of any subtype.
 * 3. * /jpg: Obtains files whose subtype is JPG of any main type.
 *
 * @return Returns the MIME type of the matched files; returns null if there is no type that matches the Data ability.
 */
std::vector<std::string> Ability::GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter)
{
    return types_;
}

/**
 * @brief Opens a file. This method should be implemented by a Data ability.
 *
 * @param uri Indicates the path of the file to open.
 * @param mode Indicates the open mode, which can be "r" for read-only access, "w" for write-only access
 * (erasing whatever data is currently in the file), "wt" for write access that truncates any existing file,
 * "wa" for write-only access to append to any existing data, "rw" for read and write access on any existing data,
 * or "rwt" for read and write access that truncates any existing file.
 *
 * @return Returns the FileDescriptor object of the file descriptor.
 */
int Ability::OpenFile(const Uri &uri, const std::string &mode)
{
    return -1;
}

/**
 * @brief Queries one or more data records in the database. This method should be implemented by a Data ability.
 *
 * @param uri Indicates the database table storing the data to query.
 * @param columns Indicates the columns to be queried, in array, for example, {"name","age"}. You should define the
 * processing logic when this parameter is null.
 * @param predicates Indicates filter criteria. If this parameter is null, all data records will be queried by default.
 *
 * @return Returns the queried data.
 */
std::shared_ptr<ResultSet> Ability::Query(
    const Uri &uri, const std::vector<std::string> &columns, const DataAbilityPredicates &predicates)
{
    return nullptr;
}

/**
 * @brief Reloads data in the database.
 * The default implementation of this method returns false. You must implement this method in the child class
 * of your Data Ability if you want to use it.
 *
 * @param uri Indicates the position where the data is to reload.
 * @param extras Indicates the additional parameters contained in the PacMap object to be passed in this call.
 *
 * @return Returns true if the data is successfully reloaded; returns false otherwise.
 */
bool Ability::Reload(const Uri &uri, const PacMap &extras)
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
int Ability::BatchInsert(const Uri &uri, const std::vector<ValuesBucket> &values)
{
    int amount = 0;
    for (auto it = values.begin(); it != values.end(); it++) {
        if (Insert(uri, *it) >= 0) {
            amount++;
        }
    }
    return amount;
}

/**
 * @brief Migrates this ability to the given device on the same distributed network in a reversible way that allows this
 * ability to be migrated back to the local device through reverseContinueAbility(). The ability to migrate and its
 * ability slices must implement the IAbilityContinuation interface. Otherwise, an exception is thrown, indicating that
 * the ability does not support migration.
 *
 * @param deviceId Indicates the ID of the target device where this ability will be migrated to. If this parameter is
 * null, this method has the same effect as ContinueAbilityReversibly().
 *
 */
void Ability::ContinueAbilityReversibly(const std::string &deviceId)
{}

/**
 * @brief Migrates this ability to another device on the same distributed network in a reversible way that allows this
 * ability to be migrated back to the local device through reverseContinueAbility(). If there are multiple candidate
 * devices, a pop-up will be displayed for users to choose the desired one. The ability to migrate and its ability
 * slices must implement the IAbilityContinuation interface. Otherwise, an exception is thrown, indicating that the
 * ability does not support migration.
 *
 */
void Ability::ContinueAbilityReversibly()
{}

/**
 * @brief  public final String getOriginalDeviceId​() throws UnsupportedOperationException
 * Obtains the ID of the source device from which this ability is migrated.
 *
 * @return Returns the source device ID.
 */
std::string Ability::GetOriginalDeviceId()
{
    return "";
}

/**
 * @brief Obtains the migration state of this ability.
 * @return Returns the migration state.
 */
std::shared_ptr<ContinuationState> Ability::GetContinuationState()
{
    return nullptr;
}

/**
 * @brief Migrates this ability from another device on the same distributed network back to the local device.
 * @return Returns true if the migration request is successful; returns false otherwise.
 */
bool Ability::ReverseContinueAbility()
{
    return false;
}

/**
 * @brief Obtains the singleton AbilityPackage object to which this ability belongs.
 *
 * @return Returns the singleton AbilityPackage object to which this ability belongs.
 */
std::shared_ptr<AbilityPackage> Ability::GetAbilityPackage()
{
    return nullptr;
}

/**
 * @brief Converts the given normalized uri generated by normalizeUri(ohos.utils.net.Uri) into a denormalized one.
 * The default implementation of this method returns the original URI passed to it.
 *
 * @param uri uri Indicates the Uri object to denormalize.
 *
 * @return Returns the denormalized Uri object if the denormalization is successful; returns the original Uri passed to
 * this method if there is nothing to do; returns null if the data identified by the original Uri cannot be found in the
 * current environment.
 */
std::shared_ptr<Uri> Ability::DenormalizeUri(const Uri &uri)
{
    return nullptr;
}

/**
 * @brief Obtains the Lifecycle object of the current ability.
 *
 * @return Returns the Lifecycle object.
 */
std::shared_ptr<LifeCycle> Ability::GetLifecycle()
{
    APP_LOGI("Ability::GetLifecycle called");
    return lifecycle_;
}

/**
 * @brief Obtains the lifecycle state of this ability.
 *
 * @return Returns the lifecycle state of this ability.
 */
AbilityLifecycleExecutor::LifecycleState Ability::GetState()
{
    APP_LOGI("Ability::GetState called");

    if (abilityLifecycleExecutor_ == nullptr) {
        APP_LOGE("Ability::GetState error. abilityLifecycleExecutor_ == nullptr.");
        return AbilityLifecycleExecutor::LifecycleState::UNINITIALIZED;
    }

    return (AbilityLifecycleExecutor::LifecycleState)abilityLifecycleExecutor_->GetState();
}

/**
 * @brief A Page or Service ability uses this method to start a specific ability. The system locates the target ability
 * from installed abilities based on the value of the intent parameter and then starts it. You can specify the ability
 * to start using the intent parameter.
 *
 * @param intent Indicates the ability to start.
 */
void Ability::StartAbility(const Want &want)
{
    APP_LOGI("Ability::StartAbility called");
    AbilityContext::StartAbility(want, -1);
}

/**
 * @brief Destroys this Page or Service ability.
 * After a Page or Service ability performs all operations, it can use this method to destroy itself
 * to free up memory. This method can be called only after the ability is initialized.
 */
void Ability::TerminateAbility()
{
    APP_LOGI("Ability::TerminateAbility called");
    AbilityContext::TerminateAbility();
}

/**
 * @brief Destroys ability.
 *
 * @param want Indicates the want containing information about TerminateAbility
 *
 * @return Returns the result of TerminateAbility
 */
int Ability::TerminateAbility(Want &want)
{
    return -1;
}

/**
 * @brief Sets the main route for this ability.
 *
 * The main route, also called main entry, refers to the default <b>AbilitySlice</b> to present for this ability.
 * This function should be called only on Feature Abilities. If this function is not called in the
 * {@link OnStart(const Want &want)} function for a Feature Ability, the Feature Ability will fail to start.
 *
 * @param entry Indicates the main entry, which is the class name of the <b>AbilitySlice</b> instance to start.
 *
 * @return Returns the result of SetMainRouter
 */
void Ability::SetMainRoute(const std::string &entry)
{}

/**
 * @brief By binding an action, you can set different action parameters in Intent to present different initial pages.
 *         You must register actions in the profile file.
 *
 * @param action Indicates the action to bind.
 *
 * @param entry Indicates the entry, which is the fully qualified name of your AbilitySlice class.
 *
 * @return Returns the result of AddActionRoute
 */
void Ability::AddActionRoute(const std::string &action, const std::string &entry)
{}

/**
 * @brief Sets the background color of the window in RGB color mode.
 *
 * @param red The value ranges from 0 to 255.
 *
 * @param green The value ranges from 0 to 255.
 *
 * @param blue The value ranges from 0 to 255.
 *
 * @return Returns the result of SetWindowBackgroundColor
 */
int Ability::SetWindowBackgroundColor(int red, int green, int blue)
{
    return -1;
}

/**
 * @brief Connects the current ability to an ability using the AbilityInfo.AbilityType.SERVICE template.
 *
 * @param want Indicates the want containing information about the ability to connect
 *
 * @param conn Indicates the callback object when the target ability is connected.
 *
 * @return True means success and false means failure
 */
bool Ability::ConnectAbility(const Want &want, const sptr<AAFwk::IAbilityConnection> &conn)
{
    return AbilityContext::ConnectAbility(want, conn);
}

/**
 * @brief Disconnects the current ability from an ability
 *
 * @param conn Indicates the IAbilityConnection callback object passed by connectAbility after the connection
 *              is set up. The IAbilityConnection object uniquely identifies a connection between two abilities.
 */
void Ability::DisconnectAbility(const sptr<AAFwk::IAbilityConnection> &conn)
{
    return AbilityContext::DisconnectAbility(conn);
}

/**
 * @brief Destroys another ability that uses the AbilityInfo.AbilityType.SERVICE template.
 * The current ability using either the AbilityInfo.AbilityType.SERVICE or AbilityInfo.AbilityType.PAGE
 * template can call this method to destroy another ability that uses the AbilityInfo.AbilityType.SERVICE
 * template. The current ability itself can be destroyed by calling the terminateAbility() method.
 *
 * @param want Indicates the Want containing information about the ability to destroy.
 *
 * @return Returns true if the ability is destroyed successfully; returns false otherwise.
 */
bool Ability::StopAbility(const AAFwk::Want &want)
{
    return AbilityContext::StopAbility(want);
}

/**
 * @brief Posts a scheduled Runnable task to a new non-UI thread.
 * The task posted via this method will be executed in a new thread, which allows you to perform certain time-consuming
 * operations. To use this method, you must also override the supportHighPerformanceUI() method.
 *
 * @param task Indicates the Runnable task to post.
 *
 * @param delayTime Indicates the number of milliseconds after which the task will be executed.
 *
 * @return -
 */
void Ability::PostTask(std::function<void()> task, long delayTime)
{
    APP_LOGI("Ability::PostTask called");
    TaskHandlerClient::GetInstance()->PostTask(task, delayTime);
}

}  // namespace AppExecFwk
}  // namespace OHOS
