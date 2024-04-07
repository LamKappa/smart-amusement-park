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
#ifndef UPDATER_PARTITION_UPDATE_RECORD_H
#define UPDATER_PARTITION_UPDATE_RECORD_H

#include <cstdio>
#include <unistd.h>
#include <cassert>
#include "fs_manager/mount.h"
#include "misc_info/misc_info.h"

#ifndef O_BINARY
#define O_BINARY 0
#endif

namespace updater {
// Each partition takes 126 bytes
// So total size of partition record is
// 126 * 8 = 1008. round up to 1024 bytes
constexpr int MAX_PARTITION_NUM = 8;
// Refer to updater package format restriction.
constexpr int PARTITION_NAME_LEN = 125;
constexpr int PARTITION_RECORD_INFO_LEN = 126;
constexpr size_t PARTITION_UPDATER_RECORD_SIZE = 1024;
constexpr size_t PARTITION_RECORD_OFFSET_SIZE = 16;
constexpr size_t PARTITION_UPDATER_RECORD_MSG_SIZE = PARTITION_UPDATER_RECORD_SIZE - PARTITION_RECORD_OFFSET_SIZE;
constexpr size_t PARTITION_RECORD_START = MISC_PARTITION_RECORD_OFFSET + PARTITION_RECORD_OFFSET_SIZE;

struct PartitionRecordInfo {
    char partitionName[PARTITION_NAME_LEN];
    // true: this partition is updated already.
    // false: this partition is not updated.
    bool updated;
};

static_assert(sizeof(PartitionRecordInfo) == PARTITION_RECORD_INFO_LEN,
    "Size of PartitionRecord is not correct");

class PartitionRecord {
public:
    static PartitionRecord &GetInstance();

    // Check if partition updated in previous updating.
    bool IsPartitionUpdated(const std::string &partitionName);

    bool RecordPartitionUpdateStatus(const std::string &partitionName, bool status);

    bool ClearRecordPartitionOffset();
private:
    PartitionRecord()
    {
        offset_ = 0;
    }

    ~PartitionRecord() {}
private:
    std::string GetMiscPartitionPath(const std::string &mountPoint = "/misc");

    PartitionRecordInfo info_;
    // offset of partition record in misc.
    // offset is not start from zero, but
    // start from the global offset of misc partition.
    size_t offset_;
};
} // namespace updater
#endif //  UPDATER_PARTITION_UPDATE_RECORD_H