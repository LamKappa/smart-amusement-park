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
#include "ams_st_kit_data_ability_data_a1.h"

#include <condition_variable>
#include <mutex>
#include <stdio.h>

#include "app_log_wrapper.h"
#include "data_ability_helper.h"

namespace OHOS {
namespace AppExecFwk {
static const int ABILITY_DATA_CODE = 250;
static const int LIFECYCLE_CALLBACKS = 251;
static const int LIFECYCLE_OBSERVER = 252;
static const std::string OPERATOR_INSERT = "Insert";
static const std::string OPERATOR_DELETE = "Delete";
static const std::string OPERATOR_UPDATE = "Update";
static const std::string OPERATOR_QUERY = "Query";
static const std::string OPERATOR_GETFILETYPES = "GetFileTypes";
static const std::string OPERATOR_OPENFILE = "OpenFile";
static const std::string OPERATOR_GETTYPE = "GetType";
static const std::string OPERATOR_TEST_LIFECYCLE = "TestLifeCycle";
static const std::string OPERATOR_GET_LIFECYCLE_STATE = "GetLifecycleState";
static const int DEFAULT_INSERT_RESULT = 1111;
static const int DEFAULT_DELETE_RESULT = 2222;
static const int DEFAULT_UPDATE_RESULT = 3333;
static const std::string ABILITY_TYPE_PAGE = "0";
static const std::string ABILITY_TYPE_DATA = "2";

void AmsStKitDataAbilityDataA1LifecycleCallbacks::OnAbilityStart(const std::shared_ptr<Ability> &ability)
{
    APP_LOGI("AmsStKitDataAbilityDataA1LifecycleCallbacks  OnAbilityStart");
    std::string abilityName = ability->GetAbilityName();
    if (abilityName == mainAbility_->GetAbilityName()) {
        mainAbility_->PublishEvent(abilityEventName, LIFECYCLE_CALLBACKS, "OnStart");
    }
}

void AmsStKitDataAbilityDataA1LifecycleCallbacks::OnAbilityInactive(const std::shared_ptr<Ability> &ability)
{
    APP_LOGI("AmsStKitDataAbilityDataA1LifecycleCallbacks  OnAbilityInactive");
    std::string abilityName = ability->GetAbilityName();
    if (abilityName == mainAbility_->GetAbilityName()) {
        mainAbility_->PublishEvent(abilityEventName, LIFECYCLE_CALLBACKS, "OnInactive");
    }
}

void AmsStKitDataAbilityDataA1LifecycleCallbacks::OnAbilityBackground(const std::shared_ptr<Ability> &ability)
{
    APP_LOGI("AmsStKitDataAbilityDataA1LifecycleCallbacks  OnAbilityBackground");
    std::string abilityName = ability->GetAbilityName();
    if (abilityName == mainAbility_->GetAbilityName()) {
        mainAbility_->PublishEvent(abilityEventName, LIFECYCLE_CALLBACKS, "OnBackground");
    }
}

void AmsStKitDataAbilityDataA1LifecycleCallbacks::OnAbilityForeground(const std::shared_ptr<Ability> &ability)
{
    APP_LOGI("AmsStKitDataAbilityDataA1LifecycleCallbacks  OnAbilityForeground");
    std::string abilityName = ability->GetAbilityName();
    if (abilityName == mainAbility_->GetAbilityName()) {
        mainAbility_->PublishEvent(abilityEventName, LIFECYCLE_CALLBACKS, "OnForeground");
    }
}

void AmsStKitDataAbilityDataA1LifecycleCallbacks::OnAbilityActive(const std::shared_ptr<Ability> &ability)
{
    APP_LOGI("AmsStKitDataAbilityDataA1LifecycleCallbacks  OnAbilityActive");
    std::string abilityName = ability->GetAbilityName();
    if (abilityName == mainAbility_->GetAbilityName()) {
        mainAbility_->PublishEvent(abilityEventName, LIFECYCLE_CALLBACKS, "OnActive");
    }
}

void AmsStKitDataAbilityDataA1LifecycleCallbacks::OnAbilityStop(const std::shared_ptr<Ability> &ability)
{
    APP_LOGI("AmsStKitDataAbilityDataA1LifecycleCallbacks  OnAbilityStop");
    std::string abilityName = ability->GetAbilityName();
    if (abilityName == mainAbility_->GetAbilityName()) {
        mainAbility_->PublishEvent(abilityEventName, LIFECYCLE_CALLBACKS, "OnStop");
    }
}

void AmsStKitDataAbilityDataA1LifecycleCallbacks::OnAbilitySaveState(const PacMap &outState)
{
    APP_LOGI("AmsStKitDataAbilityDataA1LifecycleCallbacks  OnAbilitySaveState");
    mainAbility_->PublishEvent(abilityEventName, LIFECYCLE_CALLBACKS, "OnSaveState");
}

void AmsStKitDataAbilityDataA1LifecycleObserver::OnActive()
{
    APP_LOGI("AmsStKitDataAbilityDataA1LifecycleObserver  OnActive");
    mainAbility_->PublishEvent(abilityEventName, LIFECYCLE_OBSERVER, "OnActive");
}

void AmsStKitDataAbilityDataA1LifecycleObserver::OnBackground()
{
    APP_LOGI("AmsStKitDataAbilityDataA1LifecycleObserver  OnBackground");
    mainAbility_->PublishEvent(abilityEventName, LIFECYCLE_OBSERVER, "OnBackground");
}

void AmsStKitDataAbilityDataA1LifecycleObserver::OnForeground(const Want &want)
{
    APP_LOGI("AmsStKitDataAbilityDataA1LifecycleObserver  OnForeground");
    mainAbility_->PublishEvent(abilityEventName, LIFECYCLE_OBSERVER, "OnForeground");
}

void AmsStKitDataAbilityDataA1LifecycleObserver::OnInactive()
{
    APP_LOGI("AmsStKitDataAbilityDataA1LifecycleObserver  OnInactive");
    mainAbility_->PublishEvent(abilityEventName, LIFECYCLE_OBSERVER, "OnInactive");
}

void AmsStKitDataAbilityDataA1LifecycleObserver::OnStart(const Want &want)
{
    APP_LOGI("AmsStKitDataAbilityDataA1LifecycleObserver  OnStart");
    mainAbility_->PublishEvent(abilityEventName, LIFECYCLE_OBSERVER, "OnStart");
}

void AmsStKitDataAbilityDataA1LifecycleObserver::OnStop()
{
    APP_LOGI("AmsStKitDataAbilityDataA1LifecycleObserver  OnStop");
    mainAbility_->PublishEvent(abilityEventName, LIFECYCLE_OBSERVER, "OnStop");
}

void AmsStKitDataAbilityDataA1LifecycleObserver::OnStateChanged(LifeCycle::Event event, const Want &want)
{
    APP_LOGI("AmsStKitDataAbilityDataA1LifecycleObserver  OnStateChanged");
    mainAbility_->PublishEvent(abilityEventName, LIFECYCLE_OBSERVER, "OnStateChanged");
}

void AmsStKitDataAbilityDataA1LifecycleObserver::OnStateChanged(LifeCycle::Event event)
{
    APP_LOGI("AmsStKitDataAbilityDataA1LifecycleObserver  OnStateChanged");
    mainAbility_->PublishEvent(abilityEventName, LIFECYCLE_OBSERVER, "OnStateChanged");
}

bool AmsStKitDataAbilityDataA1::PublishEvent(const std::string &eventName, const int &code, const std::string &data)
{
    Want want;
    want.SetAction(eventName);
    CommonEventData commonData;
    commonData.SetWant(want);
    commonData.SetCode(code);
    commonData.SetData(data);
    return CommonEventManager::PublishCommonEvent(commonData);
}

void KitTestDataA1EventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    APP_LOGI("KitTestDataA1EventSubscriber::OnReceiveEvent:event=%{public}s", data.GetWant().GetAction().c_str());
    APP_LOGI("KitTestDataA1EventSubscriber::OnReceiveEvent:data=%{public}s", data.GetData().c_str());
    APP_LOGI("KitTestDataA1EventSubscriber::OnReceiveEvent:code=%{public}d", data.GetCode());
    auto eventName = data.GetWant().GetAction();
    if (eventName.compare(testEventName) == 0 && ABILITY_DATA_CODE == data.GetCode()) {
        std::string target = data.GetData();
        STtools::Completed(mainAbility_->event, target, ABILITY_DATA_CODE);
    }
}

void AmsStKitDataAbilityDataA1::Init(const std::shared_ptr<AbilityInfo> &abilityInfo,
    const std::shared_ptr<OHOSApplication> &application, std::shared_ptr<AbilityHandler> &handler,
    const sptr<IRemoteObject> &token)
{
    APP_LOGI("AmsStKitDataAbilityDataA1::Init called.");
    Ability::Init(abilityInfo, application, handler, token);
    auto callback = std::make_shared<AmsStKitDataAbilityDataA1LifecycleCallbacks>();
    callback->mainAbility_ = this;
    Ability::GetApplication()->RegisterAbilityLifecycleCallbacks(callback);
    auto observer = std::make_shared<AmsStKitDataAbilityDataA1LifecycleObserver>();
    observer->mainAbility_ = this;
    Ability::GetLifecycle()->AddObserver(observer);
}

AmsStKitDataAbilityDataA1::~AmsStKitDataAbilityDataA1()
{
    CommonEventManager::UnSubscribeCommonEvent(subscriber_);
}

void AmsStKitDataAbilityDataA1::SubscribeEvent(const Want &want)
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
    subscriber_ = std::make_shared<KitTestDataA1EventSubscriber>(subscribeInfo, this);

    CommonEventManager::SubscribeCommonEvent(subscriber_);
}

void AmsStKitDataAbilityDataA1::OnStart(const Want &want)
{
    APP_LOGI("AmsStKitDataAbilityDataA1 OnStart");
    SubscribeEvent(want);
    originWant_ = want;
    Ability::OnStart(want);
    PublishEvent(abilityEventName, ABILITY_DATA_CODE, "OnStart");
}

void AmsStKitDataAbilityDataA1::OnStop()
{
    APP_LOGI("AmsStKitDataAbilityDataA1 OnStop");
    Ability::OnStop();
    PublishEvent(abilityEventName, ABILITY_DATA_CODE, "OnStop");
}

void AmsStKitDataAbilityDataA1::OnActive()
{
    APP_LOGI("AmsStKitDataAbilityDataA1 OnActive");
    Ability::OnActive();
    PublishEvent(abilityEventName, ABILITY_DATA_CODE, "OnActive");
}

void AmsStKitDataAbilityDataA1::OnInactive()
{
    APP_LOGI("AmsStKitDataAbilityDataA1 OnInactive");
    Ability::OnInactive();
    PublishEvent(abilityEventName, ABILITY_DATA_CODE, "OnInactive");
}

void AmsStKitDataAbilityDataA1::OnForeground(const Want &want)
{
    APP_LOGI("AmsStKitDataAbilityDataA1 OnForeground");
    Ability::OnForeground(want);
    PublishEvent(abilityEventName, ABILITY_DATA_CODE, "OnForeground");
}

void AmsStKitDataAbilityDataA1::OnBackground()
{
    APP_LOGI("AmsStKitDataAbilityDataA1 OnBackground");
    Ability::OnBackground();
    PublishEvent(abilityEventName, ABILITY_DATA_CODE, "OnBackground");
}

void AmsStKitDataAbilityDataA1::OnNewWant(const Want &want)
{
    APP_LOGI("AmsStKitDataAbilityDataA1::OnNewWant");
    originWant_ = want;
    Ability::OnNewWant(want);
    PublishEvent(abilityEventName, ABILITY_DATA_CODE, "OnNewWant");
}

int AmsStKitDataAbilityDataA1::Insert(const Uri &uri, const ValuesBucket &value)
{
    APP_LOGI("AmsStKitDataAbilityDataA1 <<<<Insert>>>>");
    PublishEvent(abilityEventName, ABILITY_DATA_CODE, "Insert");
    return DEFAULT_INSERT_RESULT;
}

int AmsStKitDataAbilityDataA1::Delete(const Uri &uri, const DataAbilityPredicates &predicates)
{
    APP_LOGI("AmsStKitDataAbilityDataA1 <<<<Delete>>>>");
    PublishEvent(abilityEventName, ABILITY_DATA_CODE, "Delete");
    return DEFAULT_DELETE_RESULT;
}

int AmsStKitDataAbilityDataA1::Update(
    const Uri &uri, const ValuesBucket &value, const DataAbilityPredicates &predicates)
{
    APP_LOGI("AmsStKitDataAbilityDataA1 <<<<Update>>>>");
    PublishEvent(abilityEventName, ABILITY_DATA_CODE, "Update");
    return DEFAULT_UPDATE_RESULT;
}

std::shared_ptr<ResultSet> AmsStKitDataAbilityDataA1::Query(
    const Uri &uri, const std::vector<std::string> &columns, const DataAbilityPredicates &predicates)
{
    subscriber_->vectorOperator_ = columns;
    APP_LOGI("AmsStKitDataAbilityDataA1 <<<<Query>>>>");
    PublishEvent(abilityEventName, ABILITY_DATA_CODE, OPERATOR_QUERY);

    STtools::WaitCompleted(event, OPERATOR_QUERY, ABILITY_DATA_CODE);
    subscriber_->TestPost();

    std::shared_ptr<ResultSet> resultValue = std::make_shared<ResultSet>(OPERATOR_QUERY);
    return resultValue;
}

std::vector<std::string> AmsStKitDataAbilityDataA1::GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter)
{
    APP_LOGI("AmsStKitDataAbilityDataA1 <<<<GetFileTypes>>>>");
    PublishEvent(abilityEventName, ABILITY_DATA_CODE, "GetFileTypes");
    std::vector<std::string> fileType{"filetypes"};
    return fileType;
}

int AmsStKitDataAbilityDataA1::OpenFile(const Uri &uri, const std::string &mode)
{
    APP_LOGI("AmsStKitDataAbilityDataA1 <<<<OpenFile>>>>");
    FILE *fd1 = fopen("/system/vendor/test.txt", "r");
    if (fd1 == nullptr)
        return -1;
    int fd = fileno(fd1);
    APP_LOGI("AmsStKitDataAbilityDataA1 fd: %{public}d", fd);
    PublishEvent(abilityEventName, ABILITY_DATA_CODE, "OpenFile");
    return fd;
}

void AmsStKitDataAbilityDataA1::TestLifeCycle()
{
    APP_LOGI("AmsStKitDataAbilityDataA1::TestLifeCycle");
    // ability_lifecycle.h
    auto lifecycle = Ability::GetLifecycle();
    PublishEvent(abilityEventName, ABILITY_DATA_CODE, "GetLifecycle");
    Want want;
    lifecycle->DispatchLifecycle(LifeCycle::Event::ON_START, want);
    lifecycle->DispatchLifecycle(LifeCycle::Event::ON_FOREGROUND, want);
    lifecycle->DispatchLifecycle(LifeCycle::Event::ON_ACTIVE);
    lifecycle->DispatchLifecycle(LifeCycle::Event::ON_BACKGROUND, want);
    lifecycle->DispatchLifecycle(LifeCycle::Event::ON_BACKGROUND);
    lifecycle->DispatchLifecycle(LifeCycle::Event::ON_INACTIVE);
    lifecycle->DispatchLifecycle(LifeCycle::Event::ON_STOP);
}

static void GetResult(std::shared_ptr<STtools::StOperator> child, std::shared_ptr<DataAbilityHelper> helper,
    AmsStKitDataAbilityDataA1 *mainAbility, Uri dataAbilityUri, string &result)
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
    } else if (child->GetOperatorName() == OPERATOR_TEST_LIFECYCLE) {
        mainAbility->TestLifeCycle();
        result = OPERATOR_TEST_LIFECYCLE;
    } else if (child->GetOperatorName() == OPERATOR_GET_LIFECYCLE_STATE) {
        auto lifecycle = mainAbility->GetLifecycle();
        auto state = lifecycle->GetLifecycleState();
        result = (state != LifeCycle::Event::UNDEFINED) ? OPERATOR_GET_LIFECYCLE_STATE : "UNDEFINED";
    }
}

void KitTestDataA1EventSubscriber::TestPost(const std::string funName)
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
REGISTER_AA(AmsStKitDataAbilityDataA1);
}  // namespace AppExecFwk
}  // namespace OHOS