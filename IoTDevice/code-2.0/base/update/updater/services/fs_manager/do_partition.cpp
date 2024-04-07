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
#include <cerrno>
#include <cstring>
#include <linux/blkpg.h>
#include <linux/fs.h>
#include <libgen.h>
#include <string>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
#include "fs_manager/cmp_partition.h"
#include "fs_manager/mount.h"
#include "fs_manager/partitions.h"
#include "log/log.h"
#include "partition_const.h"
#include "securec.h"

namespace updater {
namespace {
const std::string USERDATA_PARTNAME = "userdata";
const std::string UPDATER_PARTNAME = "updater";
}

static int BlkpgPartCommand(const Partition &part, struct blkpg_partition &pg, int op)
{
    struct blkpg_ioctl_arg args {};
    args.op = op;
    args.flags = 0;
    args.datalen = sizeof(struct blkpg_partition);
    args.data = static_cast<void *>(&pg);

    int ret = 0;
#ifndef UPDATER_UT
    ret = ioctl(part.partfd, BLKPG, &args);
#endif
    UPDATER_ERROR_CHECK_NOT_RETURN(ret >= 0, "ioctl of partition " << part.partName <<
        " with operation " << op << " failed");
    return ret;
}

static int DoUmountDiskPartition(const Partition &part)
{
    std::string partName = std::string("/") + part.partName;
    int ret = UmountForPath(partName);
    UPDATER_ERROR_CHECK(ret != -1, "Umount " << partName << " failed: " << errno, return 0);
    return 1;
}

static int DoFsync(const BlockDevice &dev)
{
    BlockSpecific* bs = BLOCK_SPECIFIC(&dev);
    int status;

    while (true) {
        status = fsync (bs->fd);
        if (status >= 0) {
            break;
        }
    }
    return 1;
}

static int BlockSync(const Disk &disk)
{
    UPDATER_CHECK_ONLY_RETURN(!disk.dev->readOnly, return 0);
    UPDATER_CHECK_ONLY_RETURN(DoFsync(*(disk.dev)), return 0);
    return 1;
}

static int BlkpgRemovePartition(const Partition &part)
{
    struct blkpg_partition blkPart {};
    UPDATER_CHECK_ONLY_RETURN(memset_s(&blkPart, sizeof(blkPart), 0, sizeof(blkPart))==0, return -1);
    blkPart.pno = part.partNum;
    return BlkpgPartCommand(part, blkPart, BLKPG_DEL_PARTITION);
}

static int BlockDiskOpen(Disk &disk)
{
    disk.dev->fd = open(disk.dev->devPath.c_str(), RW_MODE);
    UPDATER_WARNING_CHECK_NOT_RETURN(disk.dev->fd >= 0, "open fail: " << disk.dev->devPath << errno);
    return disk.dev->fd;
}

static void BlockDiskClose(Disk &disk)
{
    if (disk.dev != nullptr) {
        if (disk.dev->fd > 0) {
            close(disk.dev->fd);
            disk.dev->fd = -1;
        }
    }
}

static bool DoRmPartition(const Disk &disk, int partn)
{
    Partition *part = nullptr;
    part = GetPartition(disk, partn);
    UPDATER_ERROR_CHECK(part, "Cannot get partition info for partition number: " << partn, return false);

    UPDATER_CHECK_ONLY_RETURN(disk.dev->fd >= 0, return false);
    part->partfd = disk.dev->fd;
    int ret = BlkpgRemovePartition(*part);
    part->partfd = -1;
    UPDATER_ERROR_CHECK(ret >= 0, "Delete part failed", return false);
    return true;
}

static int BlkpgAddPartition(Partition &part)
{
    struct blkpg_partition blkPart {};
    UPDATER_CHECK_ONLY_RETURN(!memset_s(&blkPart, sizeof(blkPart), 0, sizeof(blkPart)), return 0);
    blkPart.start = part.start * SECTOR_SIZE_DEFAULT;
    LOG(INFO) << "blkPart.start " << blkPart.start;
    blkPart.length = part.length * SECTOR_SIZE_DEFAULT;
    LOG(INFO) << "blkPart.length " << blkPart.length;
    blkPart.pno = part.partNum;
    LOG(INFO) << "blkPart.pno " << blkPart.pno;
    UPDATER_CHECK_ONLY_RETURN(strncpy_s(blkPart.devname, BLKPG_DEVNAMELTH, part.devName.c_str(),
        part.devName.size()) == 0, return 0);
    LOG(INFO) << "blkPart.devname " << blkPart.devname;
    UPDATER_CHECK_ONLY_RETURN(strncpy_s(blkPart.volname, BLKPG_VOLNAMELTH, part.partName.c_str(),
        part.partName.size()) == 0, return 0);
    LOG(INFO) << "blkPart.volname " << blkPart.volname;
    UPDATER_CHECK_ONLY_RETURN(BlkpgPartCommand(part, blkPart, BLKPG_ADD_PARTITION)>=0, return 0);
    return 1;
}

static bool DoAddPartition(const Disk &disk, Partition &part)
{
    UPDATER_CHECK_ONLY_RETURN(disk.dev->fd >= 0, return false);

    part.partfd = disk.dev->fd;
    int ret = BlkpgAddPartition(part);
    part.partfd = -1;
    UPDATER_ERROR_CHECK(ret != 0, "Add partition failed", return false);
    return true;
}

static void DestroyDiskPartitions(Disk &disk)
{
    if (!disk.partList.empty()) {
        for (auto& p : disk.partList) {
            if (p != nullptr) {
                free(p);
            }
        }
    }
    disk.partList.clear();
}

static void DestroyDiskDevices(const Disk &disk)
{
    if (disk.dev != nullptr) {
        if (disk.dev->specific != nullptr) {
            free(disk.dev->specific);
        }
        free(disk.dev);
    }
}

static bool WriteDiskPartitionToMisc(PartitonList &nlist)
{
    UPDATER_CHECK_ONLY_RETURN(nlist.empty()==0, return false);
    char blkdevparts[BUFFER_SIZE] = "mmcblk0:";
    std::sort(nlist.begin(), nlist.end(), [](const struct Partition *a, const struct Partition *b) {
            return (a->start < b->start);
    }); // Sort in ascending order
    char tmp[SMALL_BUFFER_SIZE] = {0};
    size_t size = 0;
    for (auto& p : nlist) {
        UPDATER_CHECK_ONLY_RETURN(memset_s(tmp, sizeof(tmp), 0, sizeof(tmp)) == 0, return false);
        if (p->partName == "userdata") {
            UPDATER_CHECK_ONLY_RETURN(snprintf_s(tmp, sizeof(tmp), sizeof(tmp) - 1, "-(%s),",
                p->partName.c_str()) != -1, return false);
        } else {
            size = static_cast<size_t>(p->length * SECTOR_SIZE_DEFAULT / DEFAULT_SIZE_1MB);
            UPDATER_CHECK_ONLY_RETURN(snprintf_s(tmp, sizeof(tmp), sizeof(tmp) - 1, "%luM(%s),",
                size, p->partName.c_str()) != -1, return false);
        }
        int ncatRet = strncat_s(blkdevparts, BUFFER_SIZE - 1, tmp, strlen(tmp));
        UPDATER_ERROR_CHECK(ncatRet == EOK, "Block device name overflow", return false);
    }

    blkdevparts[strlen(blkdevparts) - 1] = '\0';
    LOG(INFO) << "blkdevparts is " << blkdevparts;

    const std::string miscDevPath = GetBlockDeviceByMountPoint("/misc");
    FILE *fp = fopen(miscDevPath.c_str(), "rb+");
    UPDATER_ERROR_CHECK(fp, "fopen error " << errno, return false);

    fseek(fp, DEFAULT_SIZE_2KB, SEEK_SET);
    size_t ret = fwrite(blkdevparts, sizeof(blkdevparts), 1, fp);
    UPDATER_ERROR_CHECK(ret >= 0, "fwrite error " << errno, fclose(fp); return false);

    int fd = fileno(fp);
    fsync(fd);
    fclose(fp);
    return true;
}

static bool AddPartitions(const Disk &disk, const PartitonList &ulist, int &partitionAddedCounter)
{
    if (!ulist.empty()) {
        int userNum = GetPartitionNumByPartName(USERDATA_PARTNAME, disk.partList);
        int step = 1;
        char pdevname[DEVPATH_SIZE] = {0};
        for (auto& p2 : ulist) {
            if (p2->partName == USERDATA_PARTNAME) {
                LOG(INFO) << "Change userdata image is not support.";
                continue;
            }
            UPDATER_ERROR_CHECK(p2->partName != UPDATER_PARTNAME, "Change updater image is not supported.", continue);
            p2->partNum = userNum + step;
            UPDATER_CHECK_ONLY_RETURN(snprintf_s(pdevname, sizeof(pdevname), sizeof(pdevname) - 1,
                "%sp%d", MMC_DEV.c_str(), p2->partNum) != -1, return false);
            p2->devName.clear();
            p2->devName = pdevname;
            LOG(INFO) << "Adding partition " << p2->partName;
            UPDATER_ERROR_CHECK(DoAddPartition (disk, *p2), "Add partition fail for " << p2->partName, return false);
            step++;
            partitionAddedCounter++;
        }
    }
    return true;
}

static bool RemovePartitions(const Disk &disk, int &partitionRemovedCounter)
{
    PartitonList pList = disk.partList;
    for (const auto &it : pList) {
        if (it->changeType == NOT_CHANGE) {
            continue;
        }
        UPDATER_ERROR_CHECK(it->partName != UPDATER_PARTNAME, "Cannot delete updater partition.", continue);

        if (it->partName == USERDATA_PARTNAME) {
            LOG(INFO) << "Cannot delete userdata partition.";
            continue;
        }
        UPDATER_CHECK_ONLY_RETURN(DoUmountDiskPartition(*it), continue);
        LOG(INFO) << "Removing partition " << it->partName;
        UPDATER_ERROR_CHECK(DoRmPartition (disk, it->partNum), "Remove partition failed.", return false);
        partitionRemovedCounter++;
    }
    return true;
}

int DoPartitions(PartitonList &nlist)
{
    LOG(INFO) << "do_partitions start ";
    UPDATER_ERROR_CHECK(!nlist.empty(), "newpartitionlist is empty ", return 0);
    const std::string path = MMC_PATH;
    int get = 0;
    int fd = -1;
    int partitionChangedCounter = 1;
    PartitonList ulist;
    ulist.clear();
    int ret = DiskAlloc(path);
    UPDATER_ERROR_CHECK(ret, "path not exist " << path, return 0);
    int sum = ProbeAllPartitions();
    UPDATER_ERROR_CHECK(sum, "partition sum  is zero! ", return 0);
    Disk *disk = GetRegisterBlockDisk(MMC_PATH);
    UPDATER_ERROR_CHECK(disk, "getRegisterdisk fail! ", return 0);
    int reg = RegisterUpdaterPartitionList(nlist, disk->partList);
    UPDATER_ERROR_CHECK(reg, "register updater list fail! ", free(disk); return 0);
    get = GetRegisterUpdaterPartitionList(ulist);
    UPDATER_ERROR_CHECK(get, "get updater list fail! ", goto error);

    fd = BlockDiskOpen(*disk);
    UPDATER_CHECK_ONLY_RETURN(fd >= 0, goto error);
    UPDATER_CHECK_ONLY_RETURN(RemovePartitions(*disk, partitionChangedCounter), goto error);
    BlockSync (*disk);
    UPDATER_CHECK_ONLY_RETURN(AddPartitions(*disk, ulist, partitionChangedCounter), goto error);
    BlockSync (*disk);
    WriteDiskPartitionToMisc(nlist);
    BlockDiskClose(*disk);
    DestroyDiskPartitions(*disk);
    DestroyDiskDevices(*disk);
    free(disk);
    return partitionChangedCounter;
error:
    BlockDiskClose(*disk);
    DestroyDiskPartitions(*disk);
    DestroyDiskDevices(*disk);
    free(disk);
    return 0;
}
} // updater

