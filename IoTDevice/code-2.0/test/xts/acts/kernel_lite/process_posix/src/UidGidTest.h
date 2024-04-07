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

#ifndef KERNEL_LITE_UIDGID_TEST
#define KERNEL_LITE_UIDGID_TEST

#include <gtest/gtest.h>
#include "utils.h"
#include "log.h"
#include "KernelConstants.h"

// max test number of uid/gid, not actual max number
const int MAX_UGID = 100000;

// Assert all uid == expectUid
#define AssertAllUid(expectUid)  do {  \
        uid_t uid, euid, suid;         \
        getresuid(&uid, &euid, &suid); \
        ASSERT_EQ(uid,  expectUid);    \
        ASSERT_EQ(euid, expectUid);    \
        ASSERT_EQ(suid, expectUid);    \
    } while (0)

// Assert all gid == expectGid
#define AssertAllGid(expectGid)  do {  \
        gid_t gid, egid, sgid;         \
        getresgid(&gid, &egid, &sgid); \
        ASSERT_EQ(gid,  expectGid);    \
        ASSERT_EQ(egid, expectGid);    \
        ASSERT_EQ(sgid, expectGid);    \
    } while (0)

class UidGidTest : public testing::Test {
public:
    // return a rand uid or gid, speacial value excluded.
    static uid_t GetRandID()
    {
        int id;
        do {
            id = GetRandom(MAX_UGID);
        } while (id == 0 || id == SHELL_UID || id == SHELL_GID);
        return  id;
    }
protected:
    void TearDown()
    {
        LOG("TearDown: reset uid and gid");
        setuid(SHELL_UID);
        setgid(SHELL_GID);
        AssertAllUid(SHELL_UID);
        AssertAllGid(SHELL_GID);
    }
};

#endif
