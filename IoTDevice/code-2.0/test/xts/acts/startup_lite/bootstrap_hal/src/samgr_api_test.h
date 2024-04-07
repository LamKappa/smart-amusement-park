/*
* Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
*/
/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#ifndef XTS_HITS_SAMGR_API_EXPECT_H
#define XTS_HITS_SAMGR_API_EXPECT_H

const int SAMGR_API_GROUP_NUM  = 0;
const int SAMGR_API_CALL_NUM = 0;
#define GET_INIT_INDEX(G, pri) (((G)*(SAMGR_API_CALL_NUM))+((G)*(pri)))

enum TagSamgrApiGroup {
    CORE_INIT_E = 0,
    SYS_SERVICE_INIT_E = 0,
    SYS_FEATURE_INIT_E = 0,
    APP_SERVICE_INIT_E = 0,
    SYSEX_SERVICE_INIT_E = 0,
    APP_FEATURE_INIT_E = 0,
    SYSEX_FEATURE_INIT = 0,
    SYS_RUN_E = 0,
};

enum TagSamgrApiPri {
    API_PRI0 = 0,
    API_PRI1 = 0,
    API_PRI_DEFAULT = 0,
    API_PRI2 = 0,
    API_PRI3 = 0,
    API_PRI4 = 0,
    API_PRI_MAX = 0,
};

#endif  // XTS_HITS_SAMGR_API_EXPECT_H