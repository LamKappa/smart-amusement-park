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

#ifndef AMS_SERVICE_ABILITY_TEST_DEF_H_
#define AMS_SERVICE_ABILITY_TEST_DEF_H_
#include <string>
#include <map>

namespace OHOS {
namespace AppExecFwk {
using MAP_STR_STR = std::map<std::string, std::string>;

const std::string BUNDLE_NAME_BASE = "com.ohos.amsst.service.app";
const std::string ABILITY_NAME_BASE = "AmsStServiceAbility";
const std::string HAP_NAME_BASE = "amsSystemTestService";
const std::string LAUNCHER_BUNDLE_NAME = "com.ix.launcher";
const std::string DUMP_STACK_LIST = "--stack-list";
const std::string DUMP_STACK = "--stack";
const std::string DUMP_SERVICE = "--serv";
const std::string DUMP_DATA = "--data";

const std::string DUMP_MISSION = "--mission";
const std::string DUMP_TOP = "--top";
const std::string DUMP_ALL = "-a";
constexpr int WAIT_TIME = 2 * 1000 * 1000;
constexpr int WAIT_LAUNCHER_OK = 2 * 1000 * 1000;
constexpr size_t TIMES = 3;
constexpr int DELAY_TIME = 6;
constexpr int BUFFER_SIZE = 1024;

enum AbilityState_Test {
    INITIAL = 0,
    INACTIVE,
    ACTIVE,
    BACKGROUND,
    SUSPENDED,
    INACTIVATING,
    ACTIVATING,
    MOVING_BACKGROUND,
    TERMINATING,
    USER_DEFINE,
};
const std::string PAGE_STATE_ON_START = "OnStart";
const std::string PAGE_STATE_ON_STOP = "OnStop";
const std::string PAGE_STATE_ON_ACTIVE = "OnActive";
const std::string PAGE_STATE_ON_INACTIVE = "OnInactive";
const std::string PAGE_STATE_ON_BACKGROUND = "OnBackground";

const std::string SERVICE_STATE_ON_START = "OnStart";
const std::string SERVICE_STATE_ON_STOP = "OnStop";
const std::string SERVICE_STATE_ON_COMMAND = "OnCommand";
const std::string SERVICE_STATE_ON_CONNECT = "OnConnect";
const std::string SERVICE_STATE_ON_DISCONNECT = "OnDisconnect";
const std::string SERVICE_STATE_ONINACTIVE = "OnInactive";
const std::string SERVICE_STATE_ON_BACKGROUND = "OnBackground";

const std::string DATA_STATE_ON_START = "OnStart";
const std::string DATA_STATE_INSERT = "Insert";
const std::string DATA_STATE_DELETE = "Delete";
const std::string DATA_STATE_UPDATE = "Update";

const std::string OPERATION_START_OTHER_ABILITY = "StartOtherAbility";
const std::string OPERATION_CONNECT_OTHER_ABILITY = "ConnectOtherAbility";
const std::string OPERATION_DISCONNECT_OTHER_ABILITY = "DisConnectOtherAbility";
const std::string OPERATION_GET_DATA_BY_DATA_ABILITY = "GetDataByDataAbility";
const std::string OPERATION_FROM_DATA_ABILITY = "GetDataByDataAbility";
const std::string REQ_EVENT_NAME_APP_A1 = "req_com_ohos_amsst_service_app_a1";
const std::string REQ_EVENT_NAME_APP_B2 = "req_com_ohos_amsst_service_app_b2";
const std::string REQ_EVENT_NAME_APP_B3 = "req_com_ohos_amsst_service_app_b3";
const std::string REQ_EVENT_NAME_APP_C4 = "req_com_ohos_amsst_service_app_c4";
const std::string REQ_EVENT_NAME_APP_D1 = "req_com_ohos_amsst_service_app_d1";
const std::string REQ_EVENT_NAME_APP_E2 = "req_com_ohos_amsst_service_app_e2";
const std::string REQ_EVENT_NAME_APP_G1 = "req_com_ohos_amsst_service_app_g1";
const std::string REQ_EVENT_NAME_APP_H1 = "req_com_ohos_amsst_service_app_h1";

const std::string DEVICE_ID = "device";
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // AMS_SERVICE_ABILITY_TEST_DEF_H_