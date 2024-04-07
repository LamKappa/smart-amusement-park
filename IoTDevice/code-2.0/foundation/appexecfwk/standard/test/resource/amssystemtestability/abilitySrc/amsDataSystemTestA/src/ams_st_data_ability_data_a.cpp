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
#include "ams_st_data_ability_data_a.h"

#include <condition_variable>
#include <fstream>
#include <mutex>
#include <stdio.h>

#include "app_log_wrapper.h"
#include "data_ability_helper.h"
#include "file_ex.h"

namespace OHOS {
namespace AppExecFwk {
static const int ABILITY_DATA_A_CODE = 210;
static const std::string OPERATOR_INSERT = "Insert";
static const std::string OPERATOR_DELETE = "Delete";
static const std::string OPERATOR_UPDATE = "Update";
static const std::string OPERATOR_QUERY = "Query";
static const std::string OPERATOR_GETFILETYPES = "GetFileTypes";
static const std::string OPERATOR_OPENFILE = "OpenFile";
static const int DEFAULT_INSERT_RESULT = 1111;
static const int DEFAULT_DELETE_RESULT = 2222;
static const int DEFAULT_UPDATE_RESULT = 3333;
static const std::string ABILITY_TYPE_PAGE = "0";
static const std::string ABILITY_TYPE_SERVICE = "1";
static const std::string ABILITY_TYPE_DATA = "2";

bool AmsStDataAbilityDataA::PublishEvent(const std::string &eventName, const int &code, const std::string &data)
{
    Want want;
    want.SetAction(eventName);
    CommonEventData commonData;
    commonData.SetWant(want);
    commonData.SetCode(code);
    commonData.SetData(data);
    return CommonEventManager::PublishCommonEvent(commonData);
}

void DataTestDataAEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    APP_LOGI("DataTestDataAEventSubscriber::OnReceiveEvent:event=%{public}s", data.GetWant().GetAction().c_str());
    APP_LOGI("DataTestDataAEventSubscriber::OnReceiveEvent:data=%{public}s", data.GetData().c_str());
    APP_LOGI("DataTestDataAEventSubscriber::OnReceiveEvent:code=%{public}d", data.GetCode());
    auto eventName = data.GetWant().GetAction();
    if (eventName.compare(testEventName) == 0 && ABILITY_DATA_A_CODE == data.GetCode()) {
        std::string target = data.GetData();
        STtools::Completed(mainAbility_->event, target, ABILITY_DATA_A_CODE);
    }
}

AmsStDataAbilityDataA::~AmsStDataAbilityDataA()
{
    CommonEventManager::UnSubscribeCommonEvent(subscriber_);
}

void AmsStDataAbilityDataA::SubscribeEvent(const Want &want)
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
    subscriber_ = std::make_shared<DataTestDataAEventSubscriber>(subscribeInfo, this);

    CommonEventManager::SubscribeCommonEvent(subscriber_);
}

void AmsStDataAbilityDataA::OnStart(const Want &want)
{
    APP_LOGI("AmsStDataAbilityDataA OnStart");
    SubscribeEvent(want);
    originWant_ = want;
    Ability::OnStart(want);
    PublishEvent(abilityEventName, ABILITY_DATA_A_CODE, "OnStart");
}

int AmsStDataAbilityDataA::Insert(const Uri &uri, const ValuesBucket &value)
{
    APP_LOGI("AmsStDataAbilityDataA <<<<Insert>>>>");
    PublishEvent(abilityEventName, ABILITY_DATA_A_CODE, "Insert");
    FILE *file = fdopen(fd, "r");
    if (file == nullptr) {
        APP_LOGI("-------------------AmsStDataAbilityDataA <<<<Insert>>>> file == nullptr");
    } else {
        APP_LOGI("-------------------AmsStDataAbilityDataA <<<<Insert>>>> file != nullptr");
    }
    return DEFAULT_INSERT_RESULT;
}

int AmsStDataAbilityDataA::Delete(const Uri &uri, const DataAbilityPredicates &predicates)
{
    APP_LOGI("AmsStDataAbilityDataA <<<<Delete>>>>");
    PublishEvent(abilityEventName, ABILITY_DATA_A_CODE, "Delete");
    return DEFAULT_DELETE_RESULT;
}

int AmsStDataAbilityDataA::Update(const Uri &uri, const ValuesBucket &value, const DataAbilityPredicates &predicates)
{
    APP_LOGI("AmsStDataAbilityDataA <<<<Update>>>>");
    PublishEvent(abilityEventName, ABILITY_DATA_A_CODE, "Update");
    return DEFAULT_UPDATE_RESULT;
}

std::shared_ptr<ResultSet> AmsStDataAbilityDataA::Query(
    const Uri &uri, const std::vector<std::string> &columns, const DataAbilityPredicates &predicates)
{
    subscriber_->vectorOperator_ = columns;
    APP_LOGI("AmsStDataAbilityDataA <<<<Query>>>>");
    PublishEvent(abilityEventName, ABILITY_DATA_A_CODE, OPERATOR_QUERY);

    STtools::WaitCompleted(event, OPERATOR_QUERY, ABILITY_DATA_A_CODE);
    subscriber_->TestPost();

    std::shared_ptr<ResultSet> resultValue = std::make_shared<ResultSet>(OPERATOR_QUERY);
    return resultValue;
}

std::vector<std::string> AmsStDataAbilityDataA::GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter)
{
    APP_LOGI("AmsStDataAbilityDataA <<<<GetFileTypes>>>>");
    PublishEvent(abilityEventName, ABILITY_DATA_A_CODE, "GetFileTypes");
    std::vector<std::string> fileType{
        "filetypes",
    };
    return fileType;
}

int AmsStDataAbilityDataA::OpenFile(const Uri &uri, const std::string &mode)
{
    APP_LOGI("-------------------------------AmsStDataAbilityDataA <<<<OpenFile>>>>");

    FILE *fd1 = fopen("/system/vendor/test.txt", "r");
    if (fd1 == nullptr) {
        APP_LOGI("-------------------------------AmsStDataAbilityDataA <<<<OpenFile>>>> fdr == nullptr");
        return -1;
    }
    fd = fileno(fd1);
    APP_LOGI("--------------------------------AmsStDataAbilityDataA fd: %{public}d", fd);
    PublishEvent(abilityEventName, ABILITY_DATA_A_CODE, "OpenFile");
    return fd;
}

static void GetResult(std::shared_ptr<STtools::StOperator> child, std::shared_ptr<DataAbilityHelper> helper,
    Uri dataAbilityUri, string &result)
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
    }
}

void DataTestDataAEventSubscriber::TestPost(const std::string funName)
{
    APP_LOGI("DataTestDataAEventSubscriber::TestPost %{public}s", funName.c_str());
    STtools::StOperator allOperator{};
    STtools::DeserializationStOperatorFromVector(allOperator, vectorOperator_);
    for (auto child : allOperator.GetChildOperator()) {
        APP_LOGI("---------data--------targetBundle:%{public}s", child->GetBundleName().c_str());
        APP_LOGI("---------data--------targetAbility:%{public}s", child->GetAbilityName().c_str());
        APP_LOGI("---------data--------targetAbilityType:%{public}s", child->GetAbilityType().c_str());
        APP_LOGI("---------data--------operatorName:%{public}s", child->GetOperatorName().c_str());
        APP_LOGI("---------data--------childOperatorNum:%{public}d", child->GetChildOperator().size());
    }
    std::shared_ptr<DataAbilityHelper> helper = DataAbilityHelper::Creator(mainAbility_->GetContext());
    for (auto child : allOperator.GetChildOperator()) {
        /// data ability
        if (child->GetAbilityType() == ABILITY_TYPE_DATA) {
            APP_LOGI("---------------------targetAbility_--------------------");
            Uri dataAbilityUri("dataability:///" + child->GetBundleName() + "." + child->GetAbilityName());
            std::string result;
            if (helper != nullptr) {
                APP_LOGI("---------------------helper--------------------");
                GetResult(child, helper, dataAbilityUri, result);
            }
            mainAbility_->PublishEvent(abilityEventName, ABILITY_DATA_A_CODE, child->GetOperatorName() + " " + result);
        } else if (child->GetAbilityType() == ABILITY_TYPE_PAGE) {
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
            mainAbility_->PublishEvent(abilityEventName, ABILITY_DATA_A_CODE, child->GetOperatorName());
        }
    }
}
REGISTER_AA(AmsStDataAbilityDataA);
}  // namespace AppExecFwk
}  // namespace OHOS