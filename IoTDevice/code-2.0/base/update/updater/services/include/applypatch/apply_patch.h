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
#ifndef UPDATER_APPLY_PATCH_H
#define UPDATER_APPLY_PATCH_H
#include <string>
#include <vector>

namespace updater {
struct BlockDiffParameters {
    std::vector<std::string> fields;
    size_t pos; /* position of command line */
    std::string cmdName; /* bsdiff, imgdiff, new, move etc. */
    std::string cmdLine;
    int fd; /* File descriptor of partitions */
    uint8_t* patchStart;
    std::vector<uint8> buffer;
    size_t written; /* how many block writted */

    bool targetVerified;
};
} // updater
#endif // UPDATER_APPLY_PATCH_H
