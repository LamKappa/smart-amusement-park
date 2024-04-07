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

#ifndef __UPDATER_CMP_PARTITIONS_H
#define __UPDATER_CMP_PARTITIONS_H

#include "fs_manager/partitions.h"

namespace updater {
extern int RegisterUpdaterPartitionList(const PartitonList &nlist, const PartitonList &olist);
extern int GetRegisterUpdaterPartitionList(PartitonList &ulist);
} // updater
#endif
