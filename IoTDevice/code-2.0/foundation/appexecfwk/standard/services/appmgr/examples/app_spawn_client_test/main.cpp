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

#include <cstdio>

#include "app_spawn_client.h"

using OHOS::AppExecFwk::AppSpawnClient;
using OHOS::AppExecFwk::AppSpawnStartMsg;

int main()
{
    std::shared_ptr<AppSpawnClient> service = std::make_shared<AppSpawnClient>();
    AppSpawnStartMsg startMsg = {0, 0, {0}, "test", "test"};
    pid_t pid = 0;
    service->StartProcess(startMsg, pid);
    printf("AppSpawnClientTest end with new PID : %d", pid);
    return 0;
}
