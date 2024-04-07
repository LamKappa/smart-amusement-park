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

#ifndef TEST_UTILS_H_
#define TEST_UTILS_H_
#include "ability_info.h"
#include "application_info.h"
#include "process_info.h"
#include "want.h"

namespace OHOS {
namespace AppExecFwk {
using namespace OHOS::AAFwk;
const std::string EVENT_RESP_LIFECYCLE_CALLBACK = "resp_com_ohos_amsst_appkit_service_ability_lifecycle_callbacks";
const std::string EVENT_RESP_LIFECYCLE_OBSERVER = "resp_com_ohos_amsst_appkit_service_lifecycle_observer";
const std::string EVENT_RESP_FIRST_ABILITY = "resp_com_ohos_amsst_appkit_service_mainabilitydemo";
const std::string EVENT_RESP_SECOND_ABILITY = "resp_com_ohos_amsst_appkit_service_secondability";
const std::string EVENT_RESP_KITTEST_COMPLETE = "resp_com_ohos_amsst_appkit_service_kittest_completed";
const std::string EVENT_REQU_KITTEST = "requ_com_ohos_amsst_appkit_service_kittest";
const std::string EVENT_RESP_SECOND_ABILITY_CONTEXT =
    "resp_com_ohos_amsst_appkit_service_secondability_ability_context";
const std::string EVENT_REQU_KITTEST_SECOND = "resp_com_ohos_amsst_appkit_service_secondability_kittest";

const std::string APP_LIFE_CYCLE_CALL_BACKS_RESP_EVENT_NAME = "resp_com_ohos_amsst_service_app_life_cycle_call_backs";
const std::string APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME = "req_com_ohos_amsst_service_app_life_cycle_call_backs";

const std::string APP_ABILITY_CONTEXT_RESP_EVENT_NAME = "resp_com_ohos_amsst_service_app_ability_context";
const std::string APP_ABILITY_CONTEXT_REQ_EVENT_NAME = "req_com_ohos_amsst_service_app_ability_context";

const std::string APP_ABILITY_RESP_EVENT_NAME = "resp_com_ohos_amsst_service_app_ability";
const std::string APP_ABILITY_REQ_EVENT_NAME = "req_com_ohos_amsst_service_app_ability";

const std::string APP_ABILITY_CONNECTION_RESP_EVENT_NAME = "resp_com_ohos_amsst_service_app_ability_connection";
const std::string APP_ABILITY_CONNECTION_REQ_EVENT_NAME = "req_com_ohos_amsst_service_app_ability_connection";

const std::string APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME = "resp_com_ohos_amsst_service_app_ability_life_cycle";
const std::string APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME = "req_com_ohos_amsst_service_app_ability_life_cycle";

const std::string APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME = "resp_com_ohos_amsst_service_app_life_cycle_observer";
const std::string APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME = "req_com_ohos_amsst_service_app_life_cycle_observer";

enum class CaseIndex {
    ONE = 1,
    TWO = 2,
    THREE = 3,
    FOUR = 4,
    FIVE = 5,
    SIX = 6,
    SEVEN = 7,
    EIGHT = 8,
    NINE = 9,
    TEN = 10,
    ELEVEN = 11,
    TWELVE = 12,
    THIRTEEN = 13,
    FOURTEEN = 14,
    FIFTEEN = 15,
    SIXTEEN = 16,
    SEVENTEEN = 17,
    EIGHTEEN = 18,
    NINETEEN = 19,
    TWENTY = 20,
    HANDRED = 1024,
};

class TestUtils {
public:
    TestUtils() = default;
    virtual ~TestUtils() = default;
    static bool PublishEvent(const std::string &eventName, const int &code, const std::string &data);
    static Want MakeWant(std::string deviceId, std::string abilityName, std::string bundleName,
        std::map<std::string, std::string> params);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // TEST_UTILS_H_