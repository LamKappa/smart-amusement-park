/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef UPDATER_FS_MANAGER_PARTITION_CONST_H
#define UPDATER_FS_MANAGER_PARTITION_CONST_H
namespace updater {
constexpr int SCSI_CDROM_MAJOR = 11;
constexpr int SCSI_DISK0_MAJOR = 8;
constexpr int SDMMC_MAJOR = 179;
constexpr int BUFFER_SIZE = 256;
constexpr int SMALL_BUFFER_SIZE = 64;
constexpr int DEVPATH_SIZE = 128;
constexpr int DEFAULT_PARTSUM = 16;
constexpr size_t SECTOR_SIZE_DEFAULT = 512;
constexpr int DEFAULT_SIZE_2KB = 2048;
constexpr size_t DEFAULT_SIZE_1MB = 1048576;

const std::string SDA_PATH = "/dev/sda";
const std::string SDA_DEV = "sda";
const std::string SDB_PATH = "/dev/sdb";
const std::string SDB_DEV = "sdb";
const std::string MMC_PATH = "/dev/block/mmcblk0";
const std::string MMC_DEV = "mmcblk0";

const std::string P_TYPE[] = {
    "GPT",
    "MBR",
};

#define SCSI_BLK_MAJOR(M) ((M) == SCSI_DISK0_MAJOR || (M) == SCSI_CDROM_MAJOR)
} // namespace updater
#endif // UPDATER_FS_MANAGER_PARTITION_CONST_H