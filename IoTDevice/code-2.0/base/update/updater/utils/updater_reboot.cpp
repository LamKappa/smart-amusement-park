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
#include <cstring>
#include <string>
#include "log/log.h"
#include "utils.h"

int main(int argc, char **argv)
{
    if (argc > updater::utils::ARGC_TWO_NUMS) {
        updater::LOG(updater::ERROR) << "param must be nullptr or updater.";
        return 1;
    }
    if (argc == 1) {
        updater::LOG(updater::INFO) << "updater::utils::DoReboot nullptr";
        updater::utils::DoReboot("");
    } else {
        std::string updaterStr = "updater";
        if (!memcmp(argv[1], updaterStr.c_str(), updaterStr.length())) {
            updater::utils::DoReboot(argv[1]);
        } else {
            updater::LOG(updater::INFO) << "param must be updater!";
        }
    }
    return 0;
}

