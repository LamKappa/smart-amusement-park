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

#include "applypatch/partition_record.h"
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include "fs_manager/mount.h"
#include "log/log.h"
#include "securec.h"

namespace updater {
PartitionRecord &PartitionRecord::GetInstance()
{
    static PartitionRecord partitionRecord;
    return partitionRecord;
}
bool PartitionRecord::IsPartitionUpdated(const std::string &partitionName)
{
    auto miscBlockDevice = GetMiscPartitionPath();
    uint8_t buffer[PARTITION_UPDATER_RECORD_MSG_SIZE];
    if (!miscBlockDevice.empty()) {
        int fd = open(miscBlockDevice.c_str(), O_RDONLY | O_EXCL | O_CLOEXEC | O_BINARY);
        UPDATER_FILE_CHECK(fd >= 0, "PartitionRecord: Open misc to recording partition failed", return false);
        UPDATER_CHECK_FILE_OP(lseek(fd, PARTITION_RECORD_START, SEEK_CUR) >= 0,
            "PartitionRecord: Seek misc to specific offset failed", fd, return false);
        UPDATER_CHECK_FILE_OP(read(fd, buffer, PARTITION_UPDATER_RECORD_MSG_SIZE) == PARTITION_UPDATER_RECORD_MSG_SIZE,
            "PartitionRecord: Read from misc partition failed", fd, return false);
        for (uint8_t *p = buffer; p < buffer + PARTITION_UPDATER_RECORD_MSG_SIZE; p += sizeof(PartitionRecordInfo)) {
            PartitionRecordInfo *pri = reinterpret_cast<PartitionRecordInfo*>(p);
            if (strcmp(pri->partitionName, partitionName.c_str()) == 0) {
                LOG(DEBUG) << "PartitionRecord: Found " << partitionName << " record in misc partition";
                LOG(DEBUG) << "PartitionRecord: update status: " << pri->updated;
                close(fd);
                return pri->updated;
            }
        }
        fsync(fd);
        close(fd);
        LOG(INFO) << "PartitionRecord: Cannot found " << partitionName << " record in misc partition";
    }
    return false;
}

bool PartitionRecord::RecordPartitionUpdateStatus(const std::string &partitionName, bool updated)
{
    auto miscBlockDevice = GetMiscPartitionPath();
    if (!miscBlockDevice.empty()) {
        int fd = open(miscBlockDevice.c_str(), O_RDWR | O_EXCL | O_CLOEXEC | O_BINARY);
        UPDATER_FILE_CHECK(fd >= 0, "PartitionRecord: Open misc to recording partition failed", return false);
        off_t newOffset = 0;
        UPDATER_CHECK_FILE_OP(lseek(fd, MISC_PARTITION_RECORD_OFFSET, SEEK_SET) >= 0,
                "PartitionRecord: Seek misc to record offset failed", fd, return false);
        UPDATER_CHECK_FILE_OP(read(fd, &newOffset, sizeof(off_t)) == sizeof(off_t),
            "PartitionRecord: Read offset failed", fd, return false);

        offset_ = newOffset;
        UPDATER_CHECK_FILE_OP(lseek(fd, PARTITION_RECORD_START + offset_, SEEK_SET) >= 0,
            "PartitionRecord: Seek misc to specific offset failed", fd, return false);
        if (offset_ + sizeof(PartitionRecordInfo) < PARTITION_UPDATER_RECORD_SIZE) {
            UPDATER_ERROR_CHECK(!strncpy_s(info_.partitionName, PARTITION_NAME_LEN, partitionName.c_str(),
                PARTITION_NAME_LEN - 1), "PartitionRecord: strncpy_s failed", return false);
            info_.updated = updated;
            UPDATER_CHECK_FILE_OP(write(fd, &info_, sizeof(PartitionRecordInfo)) == sizeof(PartitionRecordInfo),
                "PartitionRecord: write failed", fd, return false);
            offset_ += sizeof(PartitionRecordInfo);
            UPDATER_CHECK_FILE_OP(lseek(fd, MISC_PARTITION_RECORD_OFFSET, SEEK_SET) >= 0,
                "PartitionRecord: Seek misc to record offset failed", fd, return false);
            UPDATER_CHECK_FILE_OP(write(fd, &offset_, sizeof(off_t)) == sizeof(off_t),
                "PartitionRecord: Seek misc to record offset failed", fd, return false);
            LOG(DEBUG) << "PartitionRecord: offset is " << offset_;
        } else {
            LOG(WARNING) << "PartitionRecord: partition record overflow, offset = " << offset_;
            close(fd);
            return false;
        }
        LOG(DEBUG) << "PartitionRecord: record " << partitionName << " successfully.";
        fsync(fd);
        close(fd);
    }
    return true;
}

bool PartitionRecord::ClearRecordPartitionOffset()
{
    auto miscBlockDevice = GetMiscPartitionPath();
    if (!miscBlockDevice.empty()) {
        int fd = open(miscBlockDevice.c_str(), O_RDWR | O_EXCL | O_CLOEXEC | O_BINARY);
        UPDATER_FILE_CHECK(fd >= 0, "Open misc to recording partition failed", return false);
        UPDATER_CHECK_FILE_OP(lseek(fd, MISC_PARTITION_RECORD_OFFSET, SEEK_SET) >= 0,
            "Seek misc to specific offset failed", fd, return false);

        off_t initOffset = 0;
        UPDATER_CHECK_FILE_OP(write(fd, &initOffset, sizeof(off_t)) == sizeof(off_t),
            "StartUpdater: Write misc initOffset 0 failed", fd, return false);
        fsync(fd);
        close(fd);
    }
    return true;
}

std::string PartitionRecord::GetMiscPartitionPath(const std::string &misc)
{
    auto miscBlockDevice = GetBlockDeviceByMountPoint(misc);
    if (miscBlockDevice.empty()) {
        LOG(WARNING) << "Can not find misc partition";
    }
    return miscBlockDevice;
}
} // namespace updater
