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
#ifndef AMS_ST_DATA_ABILITY_DATA_A_H
#define AMS_ST_DATA_ABILITY_DATA_A_H
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
class DataTestDataAEventSubscriber;
class AmsStDataAbilityDataA : public Ability {
public:
    void SubscribeEvent(const Want &want);
    bool PublishEvent(const std::string &eventName, const int &code, const std::string &data);
    STtools::Event event;
    int fd;
    ~AmsStDataAbilityDataA();

protected:
    virtual void OnStart(const Want &want) override;
    virtual int Insert(const Uri &uri, const ValuesBucket &value) override;
    virtual int Delete(const Uri &uri, const DataAbilityPredicates &predicates) override;
    virtual int Update(const Uri &uri, const ValuesBucket &value, const DataAbilityPredicates &predicates) override;
    virtual std::shared_ptr<ResultSet> Query(
        const Uri &uri, const std::vector<std::string> &columns, const DataAbilityPredicates &predicates) override;
    virtual std::vector<std::string> GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter) override;
    virtual int OpenFile(const Uri &uri, const std::string &mode) override;

private:
    Want originWant_;
    std::shared_ptr<DataTestDataAEventSubscriber> subscriber_;
};

class DataTestDataAEventSubscriber : public CommonEventSubscriber {
public:
    DataTestDataAEventSubscriber(const CommonEventSubscribeInfo &sp, AmsStDataAbilityDataA *ability)
        : CommonEventSubscriber(sp)
    {
        mainAbility_ = ability;
    };
    virtual void OnReceiveEvent(const CommonEventData &data);

    void TestPost(const std::string funName = "");
    std::vector<std::string> vectorOperator_;
    AmsStDataAbilityDataA *mainAbility_;
    ~DataTestDataAEventSubscriber(){};
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // AMS_ST_DATA_ABILITY_DATA_A_H