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
#ifndef __UPDATER_FS_MANAGER_FSTAB_H
#define __UPDATER_FS_MANAGER_FSTAB_H
#include <map>
#include <string>
#include <vector>

namespace updater {
/* Fs manager flags definition */
#define FS_MANAGER_CHECK  0x00000001
#define FS_MANAGER_WAIT  0x00000002

#define VALID_FS_MANAGER_FLAGS (FS_MANAGER_CHECK | FS_MANAGER_WAIT)
#define FS_MANAGER_FLAGS_ENABLED(item, flag) ((item.fsManagerFlags & FS_MANAGER_##flag) != 0)

#define FM_MANAGER_CHECK_ENABLED(item, flag) FS_MANAGER_FLAGS_ENABLED(item, check)
#define FM_MANAGER_WAIT_ENABLED(item, flag) FS_MANAGER_FLAGS_ENABLED(item, wait)

struct FstabItem {
    std::string deviceName;  // Block device name
    std::string mountPoint;  // Mount point
    std::string fsType;      // File system type
    std::string mountOptions;  // File system mount options. readonly, rw, remount etc.
    unsigned int fsManagerFlags;  // flags defined by fs manager.
};

using Fstab = std::vector<struct FstabItem>;
} // updater
#endif // __UPDATER_FS_MANAGER_H
