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
#include "ams_st_kit_data_ability_service_a.h"

#include <cstring>
#include <condition_variable>
#include <mutex>

#include "app_log_wrapper.h"
#include "data_ability_helper.h"
#include "dummy_data_ability_predicates.h"
#include "dummy_result_set.h"
#include "dummy_values_bucket.h"

namespace OHOS {
namespace AppExecFwk {
static const int ABILITY_SERVICE_CODE = 310;
static const std::string OPERATOR_INSERT = "Insert";
static const std::string OPERATOR_DELETE = "Delete";
static const std::string OPERATOR_UPDATE = "Update";
static const std::string OPERATOR_QUERY = "Query";
static const std::string OPERATOR_GETFILETYPES = "GetFileTypes";
static const std::string OPERATOR_OPENFILE = "OpenFile";
static const std::string OPERATOR_GETTYPE = "GetType";

bool AmsStKitDataAbilityServiceA::PublishEvent(const std::string &eventName, const int &code, const std::string &data)
{
    Want want;
    want.SetAction(eventName);
    CommonEventData commonData;
    commonData.SetWant(want);
    commonData.SetCode(code);
    commonData.SetData(data);
    return CommonEventManager::PublishCommonEvent(commonData);
}

void KitTestServiceAEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    APP_LOGI("KitTestServiceAEventSubscriber::OnReceiveEvent:event=%{public}s", data.GetWant().GetAction().c_str());
    APP_LOGI("KitTestServiceAEventSubscriber::OnReceiveEvent:data=%{public}s", data.GetData().c_str());
    APP_LOGI("KitTestServiceAEventSubscriber::OnReceiveEvent:code=%{public}d", data.GetCode());
    auto eventName = data.GetWant().GetAction();
    if (eventName.compare("event_data_test_action") == 0 && ABILITY_SERVICE_CODE == data.GetCode()) {
        auto target = data.GetData();
        auto func = mapTestFunc_.find(target);
        if (func != mapTestFunc_.end()) {
            func->second();
        } else {
            APP_LOGI("OnReceiveEvent: CommonEventData error(%{public}s)", target.c_str());
        }
    }
}

AmsStKitDataAbilityServiceA::~AmsStKitDataAbilityServiceA()
{
    CommonEventManager::UnSubscribeCommonEvent(subscriber_);
}

void AmsStKitDataAbilityServiceA::SubscribeEvent(const Want &want)
{
    Want mwant{want};
    std::vector<std::string> eventList = {
        "event_data_test_action",
    };
    MatchingSkills matchingSkills;
    for (const auto &e : eventList) {
        matchingSkills.AddEvent(e);
    }
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    subscriber_ = std::make_shared<KitTestServiceAEventSubscriber>(subscribeInfo, this);
    subscriber_->vectorOperator_ = mwant.GetStringArrayParam("operator");
    subscriber_->mainAbility_ = this;

    CommonEventManager::SubscribeCommonEvent(subscriber_);
}

void AmsStKitDataAbilityServiceA::OnStart(const Want &want)
{
    APP_LOGI("AmsStKitDataAbilityServiceA::onStart");
    originWant_ = want;
    SubscribeEvent(want);
    Ability::OnStart(want);
    PublishEvent(abilityEventName, ABILITY_SERVICE_CODE, "onStart");
}

void AmsStKitDataAbilityServiceA::OnNewWant(const Want &want)
{
    APP_LOGI("AmsStKitDataAbilityServiceA::OnNewWant");
    originWant_ = want;
    Ability::OnNewWant(want);
}

void AmsStKitDataAbilityServiceA::OnStop()
{
    APP_LOGI("AmsStKitDataAbilityServiceA::onStop");
    Ability::OnStop();
    PublishEvent(abilityEventName, ABILITY_SERVICE_CODE, "OnStop");
}

void AmsStKitDataAbilityServiceA::OnActive()
{
    APP_LOGI("AmsStKitDataAbilityServiceA::OnActive");
    Ability::OnActive();
    PublishEvent(abilityEventName, ABILITY_SERVICE_CODE, "OnActive");
}

void AmsStKitDataAbilityServiceA::OnInactive()
{
    APP_LOGI("AmsStKitDataAbilityServiceA::OnInactive");
    Ability::OnInactive();
    PublishEvent(abilityEventName, ABILITY_SERVICE_CODE, "OnInactive");
}

void AmsStKitDataAbilityServiceA::OnBackground()
{
    APP_LOGI("AmsStKitDataAbilityServiceA::OnBackground");
    Ability::OnBackground();
    PublishEvent(abilityEventName, ABILITY_SERVICE_CODE, "OnBackground");
}

void AmsStKitDataAbilityServiceA::OnCommand(const AAFwk::Want &want, bool restart, int startId)
{
    APP_LOGI("AmsStServiceAbilityA1::OnCommand");

    GetWantInfo(want);
    Ability::OnCommand(want, restart, startId);
    PublishEvent(abilityEventName, ABILITY_SERVICE_CODE, "OnCommand");
}

sptr<IRemoteObject> AmsStKitDataAbilityServiceA::OnConnect(const Want &want)
{
    APP_LOGI("AmsStServiceAbilityA1::OnConnect");

    sptr<IRemoteObject> ret = Ability::OnConnect(want);
    PublishEvent(abilityEventName, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, "OnConnect");
    return ret;
}
void AmsStKitDataAbilityServiceA::OnDisconnect(const Want &want)
{
    APP_LOGI("AmsStServiceAbilityA1::OnDisconnect");

    Ability::OnDisconnect(want);
    PublishEvent(abilityEventName, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, "OnDisconnect");
}

void AmsStKitDataAbilityServiceA::GetWantInfo(const Want &want)
{
    Want mWant(want);
    STtools::StOperator allOperator{};
    std::vector<std::string> vectorOperator = mWant.GetStringArrayParam("operator");
    STtools::DeserializationStOperatorFromVector(allOperator, vectorOperator);

    for (auto child : allOperator.GetChildOperator()) {
        APP_LOGI("-----------------targetBundle:%{public}s", child->GetBundleName().c_str());
        APP_LOGI("-----------------targetAbility:%{public}s", child->GetAbilityName().c_str());
        APP_LOGI("-----------------targetAbilityType:%{public}s", child->GetAbilityType().c_str());
        APP_LOGI("-----------------operatorName:%{public}s", child->GetOperatorName().c_str());
        APP_LOGI("-----------------childOperatorNum:%{public}d", child->GetChildOperator().size());
    }
}

static void GetResult(std::shared_ptr<STtools::StOperator> child, std::shared_ptr<DataAbilityHelper> helper,
    AmsStKitDataAbilityServiceA *mainAbility, Uri dataAbilityUri, string &result)
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

void KitTestServiceAEventSubscriber::TestPost(const std::string funName)
{
    APP_LOGI("KitTestServiceAEventSubscriber::TestPost %{public}s", funName.c_str());
    STtools::StOperator allOperator{};
    STtools::DeserializationStOperatorFromVector(allOperator, vectorOperator_);
    std::shared_ptr<DataAbilityHelper> helper = DataAbilityHelper::Creator(mainAbility_->GetContext());
    for (auto child : allOperator.GetChildOperator()) {
        /// data ability
        if (child->GetAbilityType() == "2") {
            APP_LOGI("---------------------targetAbility_--------------------");
            Uri dataAbilityUri("dataability:///" + child->GetBundleName() + "." + child->GetAbilityName());
            std::string result;
            if (helper != nullptr) {
                APP_LOGI("---------------------helper--------------------");
                GetResult(child, helper, mainAbility_, dataAbilityUri, result);
            }
            mainAbility_->PublishEvent(abilityEventName, ABILITY_SERVICE_CODE, child->GetOperatorName() + " " + result);
        } else if (child->GetAbilityType() == "0") {
            APP_LOGI("---------------------StartPageAbility--------------------");
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
            mainAbility_->PublishEvent(abilityEventName, ABILITY_SERVICE_CODE, child->GetOperatorName());
        }
    }
}
REGISTER_AA(AmsStKitDataAbilityServiceA);
}  // namespace AppExecFwk
}  // namespace OHOS