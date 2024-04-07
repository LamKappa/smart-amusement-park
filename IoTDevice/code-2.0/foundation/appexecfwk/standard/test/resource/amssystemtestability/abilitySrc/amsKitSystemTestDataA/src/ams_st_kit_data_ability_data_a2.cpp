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
#include "ams_st_kit_data_ability_data_a2.h"

#include <condition_variable>
#include <mutex>
#include <stdio.h>

#include "app_log_wrapper.h"
#include "data_ability_helper.h"

namespace OHOS {
namespace AppExecFwk {
static const int ABILITY_DATA_CODE = 260;
static const int LIFECYCLE_CALLBACKS = 261;
static const int LIFECYCLE_OBSERVER = 262;
static const std::string OPERATOR_INSERT = "Insert";
static const std::string OPERATOR_DELETE = "Delete";
static const std::string OPERATOR_UPDATE = "Update";
static const std::string OPERATOR_QUERY = "Query";
static const std::string OPERATOR_GETFILETYPES = "GetFileTypes";
static const std::string OPERATOR_OPENFILE = "OpenFile";
static const std::string OPERATOR_GETTYPE = "GetType";
static const int DEFAULT_INSERT_RESULT = 1111;
static const int DEFAULT_DELETE_RESULT = 2222;
static const int DEFAULT_UPDATE_RESULT = 3333;
static const std::string ABILITY_TYPE_PAGE = "0";
static const std::string ABILITY_TYPE_DATA = "2";

void AmsStKitDataAbilityDataA2LifecycleCallbacks::OnAbilityStart(const std::shared_ptr<Ability> &ability)
{
    APP_LOGI("AmsStKitDataAbilityDataA2LifecycleCallbacks  OnAbilityStart");
    std::string abilityName = ability->GetAbilityName();
    if (abilityName == mainAbility_->GetAbilityName()) {
        mainAbility_->PublishEvent(abilityEventName, LIFECYCLE_CALLBACKS, "OnStart");
    }
}

void AmsStKitDataAbilityDataA2LifecycleCallbacks::OnAbilityInactive(const std::shared_ptr<Ability> &ability)
{
    APP_LOGI("AmsStKitDataAbilityDataA2LifecycleCallbacks  OnAbilityInactive");
    std::string abilityName = ability->GetAbilityName();
    if (abilityName == mainAbility_->GetAbilityName()) {
        mainAbility_->PublishEvent(abilityEventName, LIFECYCLE_CALLBACKS, "OnInactive");
    }
}

void AmsStKitDataAbilityDataA2LifecycleCallbacks::OnAbilityBackground(const std::shared_ptr<Ability> &ability)
{
    APP_LOGI("AmsStKitDataAbilityDataA2LifecycleCallbacks  OnAbilityBackground");
    std::string abilityName = ability->GetAbilityName();
    if (abilityName == mainAbility_->GetAbilityName()) {
        mainAbility_->PublishEvent(abilityEventName, LIFECYCLE_CALLBACKS, "OnBackground");
    }
}

void AmsStKitDataAbilityDataA2LifecycleCallbacks::OnAbilityForeground(const std::shared_ptr<Ability> &ability)
{
    APP_LOGI("AmsStKitDataAbilityDataA2LifecycleCallbacks  OnAbilityForeground");
    std::string abilityName = ability->GetAbilityName();
    if (abilityName == mainAbility_->GetAbilityName()) {
        mainAbility_->PublishEvent(abilityEventName, LIFECYCLE_CALLBACKS, "OnForeground");
    }
}

void AmsStKitDataAbilityDataA2LifecycleCallbacks::OnAbilityActive(const std::shared_ptr<Ability> &ability)
{
    APP_LOGI("AmsStKitDataAbilityDataA2LifecycleCallbacks  OnAbilityActive");
    std::string abilityName = ability->GetAbilityName();
    if (abilityName == mainAbility_->GetAbilityName()) {
        mainAbility_->PublishEvent(abilityEventName, LIFECYCLE_CALLBACKS, "OnActive");
    }
}

void AmsStKitDataAbilityDataA2LifecycleCallbacks::OnAbilityStop(const std::shared_ptr<Ability> &ability)
{
    APP_LOGI("AmsStKitDataAbilityDataA2LifecycleCallbacks  OnAbilityStop");
    std::string abilityName = ability->GetAbilityName();
    if (abilityName == mainAbility_->GetAbilityName()) {
        mainAbility_->PublishEvent(abilityEventName, LIFECYCLE_CALLBACKS, "OnStop");
    }
}

void AmsStKitDataAbilityDataA2LifecycleCallbacks::OnAbilitySaveState(const PacMap &outState)
{
    APP_LOGI("AmsStKitDataAbilityDataA2LifecycleCallbacks  OnAbilitySaveState");
    mainAbility_->PublishEvent(abilityEventName, LIFECYCLE_CALLBACKS, "OnSaveState");
}

void AmsStKitDataAbilityDataA2LifecycleObserver::OnActive()
{
    APP_LOGI("AmsStKitDataAbilityDataA2LifecycleObserver  OnActive");
    mainAbility_->PublishEvent(abilityEventName, LIFECYCLE_OBSERVER, "OnActive");
}

void AmsStKitDataAbilityDataA2LifecycleObserver::OnBackground()
{
    APP_LOGI("AmsStKitDataAbilityDataA2LifecycleObserver  OnBackground");
    mainAbility_->PublishEvent(abilityEventName, LIFECYCLE_OBSERVER, "OnBackground");
}

void AmsStKitDataAbilityDataA2LifecycleObserver::OnForeground(const Want &want)
{
    APP_LOGI("AmsStKitDataAbilityDataA2LifecycleObserver  OnForeground");
    mainAbility_->PublishEvent(abilityEventName, LIFECYCLE_OBSERVER, "OnForeground");
}

void AmsStKitDataAbilityDataA2LifecycleObserver::OnInactive()
{
    APP_LOGI("AmsStKitDataAbilityDataA2LifecycleObserver  OnInactive");
    mainAbility_->PublishEvent(abilityEventName, LIFECYCLE_OBSERVER, "OnInactive");
}

void AmsStKitDataAbilityDataA2LifecycleObserver::OnStart(const Want &want)
{
    APP_LOGI("AmsStKitDataAbilityDataA2LifecycleObserver  OnStart");
    mainAbility_->PublishEvent(abilityEventName, LIFECYCLE_OBSERVER, "OnStart");
}

void AmsStKitDataAbilityDataA2LifecycleObserver::OnStop()
{
    APP_LOGI("AmsStKitDataAbilityDataA2LifecycleObserver  OnStop");
    mainAbility_->PublishEvent(abilityEventName, LIFECYCLE_OBSERVER, "OnStop");
}

void AmsStKitDataAbilityDataA2LifecycleObserver::OnStateChanged(LifeCycle::Event event, const Want &want)
{
    APP_LOGI("AmsStKitDataAbilityDataA2LifecycleObserver  OnStateChanged");
    mainAbility_->PublishEvent(abilityEventName, LIFECYCLE_OBSERVER, "OnStateChanged");
}

void AmsStKitDataAbilityDataA2LifecycleObserver::OnStateChanged(LifeCycle::Event event)
{
    APP_LOGI("AmsStKitDataAbilityDataA2LifecycleObserver  OnStateChanged");
    mainAbility_->PublishEvent(abilityEventName, LIFECYCLE_OBSERVER, "OnStateChanged");
}

bool AmsStKitDataAbilityDataA2::PublishEvent(const std::string &eventName, const int &code, const std::string &data)
{
    Want want;
    want.SetAction(eventName);
    CommonEventData commonData;
    commonData.SetWant(want);
    commonData.SetCode(code);
    commonData.SetData(data);
    return CommonEventManager::PublishCommonEvent(commonData);
}

void KitTestDataA2EventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    APP_LOGI("KitTestDataA2EventSubscriber::OnReceiveEvent:event=%{public}s", data.GetWant().GetAction().c_str());
    APP_LOGI("KitTestDataA2EventSubscriber::OnReceiveEvent:data=%{public}s", data.GetData().c_str());
    APP_LOGI("KitTestDataA2EventSubscriber::OnReceiveEvent:code=%{public}d", data.GetCode());
    auto eventName = data.GetWant().GetAction();
    if (eventName.compare(testEventName) == 0 && ABILITY_DATA_CODE == data.GetCode()) {
        std::string target = data.GetData();
        STtools::Completed(mainAbility_->event, target, ABILITY_DATA_CODE);
    }
}

void AmsStKitDataAbilityDataA2::Init(const std::shared_ptr<AbilityInfo> &abilityInfo,
    const std::shared_ptr<OHOSApplication> &application, std::shared_ptr<AbilityHandler> &handler,
    const sptr<IRemoteObject> &token)
{
    APP_LOGI("AmsStKitDataAbilityDataA2::Init called.");
    Ability::Init(abilityInfo, application, handler, token);
    auto callback = std::make_shared<AmsStKitDataAbilityDataA2LifecycleCallbacks>();
    callback->mainAbility_ = this;
    Ability::GetApplication()->RegisterAbilityLifecycleCallbacks(callback);
    auto observer = std::make_shared<AmsStKitDataAbilityDataA2LifecycleObserver>();
    observer->mainAbility_ = this;
    Ability::GetLifecycle()->AddObserver(observer);
}

AmsStKitDataAbilityDataA2::~AmsStKitDataAbilityDataA2()
{
    CommonEventManager::UnSubscribeCommonEvent(subscriber_);
}

void AmsStKitDataAbilityDataA2::SubscribeEvent(const Want &want)
{
    std::vector<std::string> eventList = {
        testEventName,
    };
    MatchingSkills matchingSkills;
    for (const auto &e : eventList) {
        matchingSkills.AddEvent(e);
    }
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    subscriber_ = std::make_shared<KitTestDataA2EventSubscriber>(subscribeInfo, this);
    CommonEventManager::SubscribeCommonEvent(subscriber_);
}

void AmsStKitDataAbilityDataA2::OnStart(const Want &want)
{
    APP_LOGI("AmsStKitDataAbilityDataA2 OnStart");
    SubscribeEvent(want);
    originWant_ = want;
    Ability::OnStart(want);
    PublishEvent(abilityEventName, ABILITY_DATA_CODE, "OnStart");
}

void AmsStKitDataAbilityDataA2::OnStop()
{
    APP_LOGI("AmsStKitDataAbilityDataA2 OnStop");
    Ability::OnStop();
    PublishEvent(abilityEventName, ABILITY_DATA_CODE, "OnStop");
}

void AmsStKitDataAbilityDataA2::OnActive()
{
    APP_LOGI("AmsStKitDataAbilityDataA2 OnActive");
    Ability::OnActive();
    PublishEvent(abilityEventName, ABILITY_DATA_CODE, "OnActive");
}

void AmsStKitDataAbilityDataA2::OnInactive()
{
    APP_LOGI("AmsStKitDataAbilityDataA2 OnInactive");
    Ability::OnInactive();
    PublishEvent(abilityEventName, ABILITY_DATA_CODE, "OnInactive");
}

void AmsStKitDataAbilityDataA2::OnForeground(const Want &want)
{
    APP_LOGI("AmsStKitDataAbilityDataA2 OnForeground");
    Ability::OnForeground(want);
    PublishEvent(abilityEventName, ABILITY_DATA_CODE, "OnForeground");
}

void AmsStKitDataAbilityDataA2::OnBackground()
{
    APP_LOGI("AmsStKitDataAbilityDataA2 OnBackground");
    Ability::OnBackground();
    PublishEvent(abilityEventName, ABILITY_DATA_CODE, "OnBackground");
}

void AmsStKitDataAbilityDataA2::OnNewWant(const Want &want)
{
    APP_LOGI("AmsStKitDataAbilityDataA2::OnNewWant");
    originWant_ = want;
    Ability::OnNewWant(want);
    PublishEvent(abilityEventName, ABILITY_DATA_CODE, "OnNewWant");
}

int AmsStKitDataAbilityDataA2::Insert(const Uri &uri, const ValuesBucket &value)
{
    APP_LOGI("AmsStKitDataAbilityDataA2 <<<<Insert>>>>");
    PublishEvent(abilityEventName, ABILITY_DATA_CODE, "Insert");
    return DEFAULT_INSERT_RESULT;
}

int AmsStKitDataAbilityDataA2::Delete(const Uri &uri, const DataAbilityPredicates &predicates)
{
    APP_LOGI("AmsStKitDataAbilityDataA2 <<<<Delete>>>>");
    PublishEvent(abilityEventName, ABILITY_DATA_CODE, "Delete");
    return DEFAULT_DELETE_RESULT;
}

int AmsStKitDataAbilityDataA2::Update(
    const Uri &uri, const ValuesBucket &value, const DataAbilityPredicates &predicates)
{
    APP_LOGI("AmsStKitDataAbilityDataA2 <<<<Update>>>>");
    PublishEvent(abilityEventName, ABILITY_DATA_CODE, "Update");
    return DEFAULT_UPDATE_RESULT;
}

std::shared_ptr<ResultSet> AmsStKitDataAbilityDataA2::Query(
    const Uri &uri, const std::vector<std::string> &columns, const DataAbilityPredicates &predicates)
{
    subscriber_->vectorOperator_ = columns;
    APP_LOGI("AmsStKitDataAbilityDataA2 <<<<Query>>>>");
    PublishEvent(abilityEventName, ABILITY_DATA_CODE, OPERATOR_QUERY);

    STtools::WaitCompleted(event, OPERATOR_QUERY, ABILITY_DATA_CODE);
    subscriber_->TestPost();

    std::shared_ptr<ResultSet> resultValue = std::make_shared<ResultSet>(OPERATOR_QUERY);
    return resultValue;
}

std::vector<std::string> AmsStKitDataAbilityDataA2::GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter)
{
    APP_LOGI("AmsStKitDataAbilityDataA2 <<<<GetFileTypes>>>>");
    PublishEvent(abilityEventName, ABILITY_DATA_CODE, "GetFileTypes");
    std::vector<std::string> fileType{"filetypes"};
    return fileType;
}

int AmsStKitDataAbilityDataA2::OpenFile(const Uri &uri, const std::string &mode)
{
    APP_LOGI("AmsStKitDataAbilityDataA2 <<<<OpenFile>>>>");
    FILE *fd1 = fopen("/system/vendor/test.txt", "r");
    if (fd1 == nullptr)
        return -1;
    int fd = fileno(fd1);
    APP_LOGI("AmsStKitDataAbilityDataA2 fd: %{public}d", fd);
    PublishEvent(abilityEventName, ABILITY_DATA_CODE, "OpenFile");

    return fd;
}

void AmsStKitDataAbilityDataA2::TestLifeCycle()
{
    APP_LOGI("AmsStKitDataAbilityDataA2::TestLifeCycle");
    // ability_lifecycle.h
    auto lifecycle = Ability::GetLifecycle();
    PublishEvent(abilityEventName, ABILITY_DATA_CODE, "GetLifecycle");
    Want want;
    lifecycle->DispatchLifecycle(LifeCycle::Event::ON_START, want);
    lifecycle->DispatchLifecycle(LifeCycle::Event::ON_FOREGROUND, want);
    lifecycle->DispatchLifecycle(LifeCycle::Event::ON_ACTIVE);
    lifecycle->DispatchLifecycle(LifeCycle::Event::ON_BACKGROUND);
    lifecycle->DispatchLifecycle(LifeCycle::Event::ON_INACTIVE);
    lifecycle->DispatchLifecycle(LifeCycle::Event::ON_STOP);
    // valueOf
    // values
}

static void GetResult(std::shared_ptr<STtools::StOperator> child, std::shared_ptr<DataAbilityHelper> helper,
    AmsStKitDataAbilityDataA2 *mainAbility, Uri dataAbilityUri, string &result)
{
    AppExecFwk::DataAbilityPredicates predicates;
    AppExecFwk::ValuesBucket bucket;
    result = "failed";
    if (child->GetOperatorName() == OPERATOR_INSERT) {
        result = std::to_string(helper->Insert(dataAbilityUri, bucket));
    } else if (child->GetOperatorName() == OPERATOR_DELETE) {
        result = std::to_string(helper->Delete(dataAbilityUri, predicates));
    } else if (child->GetOperatorName() == OPERATOR_UPDATE) {
        result = std::to_string(helper->Update(dataAbilityUri, bucket, predicates));
    } else if (child->GetOperatorName() == OPERATOR_QUERY) {
        std::vector<std::string> columns = STtools::SerializationStOperatorToVector(*child);
        std::shared_ptr<ResultSet> resultValue = helper->Query(dataAbilityUri, columns, predicates);
        result = (resultValue != nullptr) ? (resultValue->testInf_) : "failed";
    } else if (child->GetOperatorName() == OPERATOR_GETFILETYPES) {
        std::vector<std::string> types = helper->GetFileTypes(dataAbilityUri, child->GetMessage());
        result = (types.size() > 0) ? types[0] : "failed";
    } else if (child->GetOperatorName() == OPERATOR_OPENFILE) {
        int fd = helper->OpenFile(dataAbilityUri, child->GetMessage());
        if (fd < 0) {
            return;
        }
        FILE *file = fdopen(fd, "r");
        if (file == nullptr) {
            return;
        }
        result = std::to_string(fd);
        char str[5];
        if (!feof(file))
            fgets(str, 5, file);
        result = str;
        fclose(file);
    } else if (child->GetOperatorName() == OPERATOR_GETTYPE) {
        result = helper->GetType(dataAbilityUri);
        result = (result != "") ? OPERATOR_GETTYPE : result;
    }
}

void KitTestDataA2EventSubscriber::TestPost(const std::string funName)
{
    STtools::StOperator allOperator{};
    STtools::DeserializationStOperatorFromVector(allOperator, vectorOperator_);
    std::shared_ptr<DataAbilityHelper> helper = DataAbilityHelper::Creator(mainAbility_->GetContext());
    for (auto child : allOperator.GetChildOperator()) {
        /// data ability
        if (child->GetAbilityType() == ABILITY_TYPE_DATA) {
            Uri dataAbilityUri("dataability:///" + child->GetBundleName() + "." + child->GetAbilityName());
            std::string result;
            if (helper != nullptr) {
                GetResult(child, helper, mainAbility_, dataAbilityUri, result);
            }
            mainAbility_->PublishEvent(abilityEventName, ABILITY_DATA_CODE, child->GetOperatorName() + " " + result);
        } else if (child->GetAbilityType() == ABILITY_TYPE_PAGE) {
            std::vector<std::string> vectoroperator;
            if (child->GetChildOperator().size() != 0) {
                vectoroperator = STtools::SerializationStOperatorToVector(*child);
            }
            std::string targetBundle = child->GetBundleName();
            std::string targetAbility = child->GetAbilityName();
            Want want;
            want.SetElementName(targetBundle, targetAbility);
            want.SetParam("operator", vectoroperator);
            mainAbility_->StartAbility(want);
            mainAbility_->PublishEvent(abilityEventName, ABILITY_DATA_CODE, child->GetOperatorName());
        }
    }
}
REGISTER_AA(AmsStKitDataAbilityDataA2);
}  // namespace AppExecFwk
}  // namespace OHOS