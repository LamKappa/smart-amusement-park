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
#ifndef MISC_INFO_H
#define MISC_INFO_H

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>

namespace updater {
constexpr int MAX_COMMAND_SIZE = 20;
constexpr int MAX_UPDATE_SIZE = 100;

// misc partition offset definition.
constexpr off_t MISC_BASE_OFFSET = 0;
constexpr off_t MISC_UPDATE_MESSAGE_OFFSET = MISC_BASE_OFFSET;
constexpr off_t MISC_PARTITION_RECORD_OFFSET = MISC_UPDATE_MESSAGE_OFFSET + MAX_COMMAND_SIZE + MAX_UPDATE_SIZE;

struct UpdateMessage {
    char command[MAX_COMMAND_SIZE];
    char update[MAX_UPDATE_SIZE];
};

bool WriteUpdaterMessage(const std::string &path, const UpdateMessage &boot);
bool ReadUpdaterMessage(const std::string &path, UpdateMessage &boot);
} // updater
#endif /* MISC_INFO_H */
