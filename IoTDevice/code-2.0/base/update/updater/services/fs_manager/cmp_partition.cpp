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

#include "fs_manager/cmp_partition.h"
#include <cstring>
#include "log/log.h"
#include "partition_const.h"

using namespace std;

namespace updater {
static PartitonList g_updaterPlist;
static PartitionChangeType ComparePartition(const PartitonList &plist, struct Partition &newp)
{
    PartitionChange cg = PARTITION_NEW;
    struct Partition *oldp = nullptr;
    for (auto& p : plist) {
        if (!strcmp(p->partName.c_str(), newp.partName.c_str())) {
            LOG(INFO) << "compare_partition old " << p->partName;
            cg = PARTITION_OLD;
            oldp = p;
            break;
        }
    }
    PartitionChangeType ct = NOT_CHANGE;
    switch (cg) {
        case PARTITION_NEW:
            ct = NEW_PARTITION;
            newp.changeType = NEW_PARTITION;
            break;
        case PARTITION_OLD:
            if (oldp->start != newp.start) {
                LOG(INFO) << "newp.start " << newp.start;
                ct = START_CHANGE;
                oldp->changeType = START_CHANGE;
                newp.changeType = START_CHANGE;
            } else if (oldp->length != newp.length) {
                LOG(INFO) << "newp.length " << newp.length;
                ct =  LENGTH_CHANGE;
                oldp->changeType = LENGTH_CHANGE;
                newp.changeType = LENGTH_CHANGE;
            } else {
                ct =  NOT_CHANGE;
                oldp->changeType = NOT_CHANGE;
            }
            break;
        default:
            break;
    }
    return ct;
}

static int TraversePartitionList(const PartitonList &nlist, const PartitonList &olist, PartitonList &ulist)
{
    UPDATER_CHECK_ONLY_RETURN(!nlist.empty() && !olist.empty(), return 0);

    ulist.clear();
    PartitionChangeType changeType = NOT_CHANGE;
    for (auto& p : nlist) {
        changeType = ComparePartition(olist, *p);
        if (changeType != NOT_CHANGE) {
            LOG(INFO) << "change p->partName " << p->partName;
            ulist.push_back(p);
        }
    }
    return 1;
}

int RegisterUpdaterPartitionList(const PartitonList &nlist, const PartitonList &olist)
{
    UPDATER_CHECK_ONLY_RETURN(!nlist.empty() && !olist.empty(), return 0);

    g_updaterPlist.clear();
    int ret = TraversePartitionList(nlist, olist, g_updaterPlist);

    return ret;
}

int GetRegisterUpdaterPartitionList(PartitonList &ulist)
{
    UPDATER_CHECK_ONLY_RETURN(!g_updaterPlist.empty(), return 1);

    ulist.clear();
    ulist.assign(g_updaterPlist.begin(), g_updaterPlist.end());
    UPDATER_CHECK_ONLY_RETURN(!ulist.empty(), return 0);
    return 1;
}
} // namespace updater
