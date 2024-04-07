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
#ifndef AMS_ST_DATA_ABILITY_DATA_C1_H
#define AMS_ST_DATA_ABILITY_DATA_C1_H
#include <string>

#include "ability.h"
#include "ability_loader.h"
#include "common_event_manager.h"
#include "event.h"
#include "skills.h"
#include "stoperator.h"

namespace OHOS {
namespace AppExecFwk {
using namespace OHOS::EventFwk;

const std::string abilityEventName = "event_data_ability_callback";
const std::string testEventName = "event_data_test_action";
class DataTestDataC1EventSubscriber;
class AmsStDataAbilityDataC1 : public Ability {
public:
    void SubscribeEvent(const Want &want);
    bool PublishEvent(const std::string &eventName, const int &code, const std::string &data);
    STtools::Event event;
    int fd;
    ~AmsStDataAbilityDataC1();

protected:
    virtual void OnStart(const Want &want) override;
    virtual int Insert(const Uri &uri, const ValuesBucket &value) override;
    virtual int Delete(const Uri &uri, const DataAbilityPredicates &predicates) override;
    virtual int Update(const Uri &uri, const ValuesBucket &value, const DataAbilityPredicates &predicates) override;
    virtual std::shared_ptr<ResultSet> Query(
        const Uri &uri, const std::vector<std::string> &columns, const DataAbilityPredicates &predicates) override;
    virtual std::vector<std::string> GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter) override;
    virtual int OpenFile(const Uri &uri, const std::string &mode) override;
    friend class DataTestDataC1EventSubscriber;

private:
    Want originWant_;
    std::shared_ptr<DataTestDataC1EventSubscriber> subscriber_;
};

class DataTestDataC1EventSubscriber : public CommonEventSubscriber {
public:
    DataTestDataC1EventSubscriber(const CommonEventSubscribeInfo &sp, AmsStDataAbilityDataC1 *ability)
        : CommonEventSubscriber(sp)
    {
        mainAbility_ = ability;
    };
    virtual void OnReceiveEvent(const CommonEventData &data);

    void GetResultBySelf(std::shared_ptr<STtools::StOperator> child, AmsStDataAbilityDataC1 *mainAbility,
        Uri dataAbilityUri, string &result);
    void TestPost(const std::string funName = "");
    std::vector<std::string> vectorOperator_;
    AmsStDataAbilityDataC1 *mainAbility_;
    ~DataTestDataC1EventSubscriber(){};
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // AMS_ST_DATA_ABILITY_DATA_C1_H