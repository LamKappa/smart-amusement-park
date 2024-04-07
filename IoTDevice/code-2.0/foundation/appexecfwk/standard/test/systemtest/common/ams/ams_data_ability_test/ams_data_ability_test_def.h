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
#ifndef _AMS_DATA_ABILITY_TEST_DEF_H_
#define _AMS_DATA_ABILITY_TEST_DEF_H_
#include <string>

namespace OHOS {
namespace AppExecFwk {
const int PAGE_ABILITY_CODE = 1;
const int DATA_ABILITY_CODE = 2;
const int SERVICE_ABILITY_CODE = 3;

const int ABILITY_PAGE_A_CODE = 110;
const int ABILITY_DATA_A_CODE = 210;
const int ABILITY_PAGE_B_CODE = 120;
const int ABILITY_DATA_B_CODE = 220;
const int ABILITY_DATA_C1_CODE = 230;
const int ABILITY_DATA_C2_CODE = 240;

const int ABILITY_KIT_PAGE_A_CODE = 130;
const int ABILITY_KIT_SERVICE_A_CODE = 310;
const int ABILITY_KIT_DATA_A1_CODE = 250;
const int LIFECYCLE_CALLBACKS_A1 = 251;
const int LIFECYCLE_OBSERVER_A1 = 252;
const int ABILITY_KIT_DATA_A2_CODE = 260;
const int LIFECYCLE_CALLBACKS_A2 = 261;
const int LIFECYCLE_OBSERVER_A2 = 262;
const int ABILITY_KIT_DATA_A3_CODE = 290;
const int LIFECYCLE_CALLBACKS_A3 = 291;
const int LIFECYCLE_OBSERVER_A3 = 292;

const int ABILITY_KIT_PAGE_B_CODE = 140;
const int ABILITY_KIT_SERVICE_B_CODE = 320;
const int ABILITY_KIT_DATA_B_CODE = 270;
const int LIFECYCLE_CALLBACKS_B = 271;
const int LIFECYCLE_OBSERVER_B = 272;

const std::string DEVICE_ID = "device";

const std::string ABILITY_TYPE_PAGE = "0";
const std::string ABILITY_TYPE_SERVICE = "1";
const std::string ABILITY_TYPE_DATA = "2";

const std::string ABILITY_EVENT_NAME = "event_data_ability_callback";
const std::string TEST_EVENT_NAME = "event_data_test_action";

const std::string PAGE_STATE_ONSTART = "onStart";
const std::string PAGE_STATE_ONSTOP = "OnStop";
const std::string PAGE_STATE_ONACTIVE = "OnActive";
const std::string PAGE_STATE_ONINACTIVE = "OnInactive";
const std::string PAGE_STATE_ONFOREGROUND = "OnForeground";
const std::string PAGE_STATE_ONBACKGROUND = "OnBackground";

const std::string DATA_STATE_ONSTART = "OnStart";
const std::string DATA_STATE_ONSTOP = "OnStop";
const std::string DATA_STATE_ONACTIVE = "OnActive";
const std::string DATA_STATE_ONINACTIVE = "OnInactive";
const std::string DATA_STATE_ONFOREGROUND = "OnForeground";
const std::string DATA_STATE_ONBACKGROUND = "OnBackground";
const std::string DATA_STATE_INSERT = "Insert";
const std::string DATA_STATE_DELETE = "Delete";
const std::string DATA_STATE_UPDATE = "Update";
const std::string DATA_STATE_QUERY = "Query";
const std::string DATA_STATE_GETFILETYPES = "GetFileTypes";
const std::string DATA_STATE_OPENFILE = "OpenFile";

const std::string SERVICE_STATE_ONSTART = "onStart";
const std::string SERVICE_STATE_ONSTOP = "OnStop";
const std::string SERVICE_STATE_ONACTIVE = "OnActive";
const std::string SERVICE_STATE_ONINACTIVE = "OnInactive";
const std::string SERVICE_STATE_ONFOREGROUND = "OnForeground";
const std::string SERVICE_STATE_ONBACKGROUND = "OnBackground";
const std::string SERVICE_STATE_ONCOMMAND = "OnCommand";

const std::string DATA_STATE_CHANGE = "OnStateChanged";
const std::string DATA_STATE_ONNEWWANT = "OnNewWant";

const std::string OPERATOR_INSERT = "Insert";
const std::string OPERATOR_DELETE = "Delete";
const std::string OPERATOR_UPDATE = "Update";
const std::string OPERATOR_QUERY = "Query";
const std::string OPERATOR_GETFILETYPES = "GetFileTypes";
const std::string OPERATOR_OPENFILE = "OpenFile";
const std::string OPERATOR_GETTYPE = "GetType";
const std::string OPERATOR_TEST_LIFECYCLE = "TestLifeCycle";
const std::string OPERATOR_GETLIFECYCLESTATE = "GetLifecycleState";
const std::string OPERATOR_DEFAULT = "Default";

const std::string DEFAULT_INSERT_RESULT = "1111";
const std::string DEFAULT_DELETE_RESULT = "2222";
const std::string DEFAULT_UPDATE_RESULT = "3333";
const std::string DEFAULT_OPENFILE_RESULT = "4444";
const std::string DEFAULT_GETFILETYPE_RESULT = "filetypes";
const std::string DEFAULT_QUERY_RESULT = "Query";
const std::string DEFAULT_GETTYPE_RESULT = "GetType";

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  //_AMS_DATA_ABILITY_TEST_DEF_H_