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

#ifndef __UPDATER_FS_MANAGER_PARTITIONS_H
#define __UPDATER_FS_MANAGER_PARTITIONS_H

#include <fcntl.h>
#include <string>
#include <vector>
#include <sys/types.h>


namespace updater {
#define RD_MODE (O_RDONLY)
#define WR_MODE (O_WRONLY)
#define RW_MODE (O_RDWR)
#define EX_MODE (O_EXCL)

typedef enum {
    PARTITION_NEW,
    PARTITION_OLD,
} PartitionChange;

typedef enum {
    NORMAL_CHANGE,
    NOT_CHANGE,
    NEW_PARTITION,
    START_CHANGE,
    LENGTH_CHANGE,
    PARTNUM_CHANGE,
    PARTNAME_CHANGE,
    FSTYPE_CHANGE,
} PartitionChangeType;

typedef enum {
    GPT,
    MBR,
} DiskType;

typedef enum {
    NORMAL,
    LOGICAL,
    EXTENDED,
} PartitionType;

typedef enum {
    DEVICE_UNKNOWN = 0,
    DEVICE_SCSI = 1,
    DEVICE_EMMC = 2,
} DeviceType;

/** We can address 2^63 sectors */
typedef off64_t updater_sector_t;

struct Partition;
using PartitonList = std::vector<struct Partition *>;

struct BlockDevice {
    std::string devPath;
    std::string model;         // description of hardware(manufacturer, model)
    DeviceType type;           // SCSI, MMC, etc
    size_t sectorSize;         // logical sector size
    size_t physSectorSize;     // physical sector size
    updater_sector_t length;   // device length (LBA) */
    bool readOnly;
    int  fd;
    void *specific;
};

struct Partition {
    size_t start;
    size_t length;
    int partNum; // Partition number.
    int partfd;
    std::string devName;
    std::string partName;
    std::string fsType; // File system type, ext4, f2fs etc.
    PartitionType type;
    PartitionChangeType changeType;
};

struct Disk {
    struct BlockDevice* dev;
    PartitonList partList;
    DiskType type;
    int partsum;
};

struct BlockSpecific {
    int fd;
    int major;
    int minor;
};

#define BLOCK_SPECIFIC(dev) ((BlockSpecific*) (dev)->specific)
extern int DiskAlloc(const std::string &path);
extern int ProbeAllPartitions();
extern Disk *GetRegisterBlockDisk(const std::string &path);
extern struct Partition *GetPartition(const Disk &disk, int partn);
extern int GetPartitionNumByPartName(const std::string &partname, const PartitonList &plist);
extern int DoPartitions(PartitonList &nlist);
extern bool SetBlockDeviceMode(BlockDevice &dev);
} // updater
#endif // __UPDATER_FS_MANAGER_PARTITIONS_H

