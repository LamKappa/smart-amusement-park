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

#ifndef LIFECYCLE_TEST_BASE_H
#define LIFECYCLE_TEST_BASE_H

#include <ctime>
#include <pthread.h>
#include <semaphore.h>
#include <iremote_object.h>
#include "ability_manager_service.h"
#include "ability_record.h"
#include "ability_scheduler.h"

/**
 * @class LifeTestCommand
 * asynchronous test thread use LifeTestCommand to call AbilityManagerService API.
 */
class LifeTestCommand {
public:
    LifeTestCommand()
        : state_(OHOS::AAFwk::AbilityState::INITIAL),
          expectState_(OHOS::AAFwk::AbilityState::INITIAL),
          abnormalState_(OHOS::AAFwk::AbilityState::INITIAL),
          callback_(true)
    {
        sem_init(&sem_, 0, 0);
    }

    ~LifeTestCommand()
    {
        sem_destroy(&sem_);
    }

    OHOS::AAFwk::AbilityState state_;          // actual life
    OHOS::AAFwk::AbilityState expectState_;    // expect life
    OHOS::AAFwk::AbilityState abnormalState_;  // test abnormal condition life
    bool callback_;                            // if false client won't callback_ to construct timeout case.
    sem_t sem_;
};

class LifecycleTestBase {
public:
    virtual bool StartLauncherAbility() = 0;
    virtual bool StartNextAbility() = 0;
    virtual int AttachAbility(
        const OHOS::sptr<OHOS::AAFwk::AbilityScheduler> &scheduler, const OHOS::sptr<OHOS::IRemoteObject> &token) = 0;

    static constexpr uint32_t DELAY_TEST_TIME = 1000;  // ms
    static constexpr long MILLISECONDS = 1000;
    static constexpr long NANOSECONDS = 1000000000;

    static void *AbilityStartThread(void *command)
    {
        auto c = static_cast<LifeTestCommand *>(command);
        if (c == nullptr) {
            return nullptr;
        }
        if (c->callback_) {
            if (c->abnormalState_ != OHOS::AAFwk::AbilityState::INITIAL) {
                c->state_ = c->abnormalState_;
                ;
            } else {
                c->state_ = c->expectState_;
            }
            sem_post(&(c->sem_));
        }
        return c;
    }

    static int SemTimedWaitMillis(long msecs, sem_t &sem)
    {
        if (msecs <= 0) {
            return 0;
        }
        msecs = msecs % MILLISECONDS;
        long secs = msecs / MILLISECONDS;
        struct timespec ts = {0, 0};
        clock_gettime(CLOCK_REALTIME, &ts);
        msecs = msecs * MILLISECONDS * MILLISECONDS + ts.tv_nsec;
        long add = msecs / NANOSECONDS;
        ts.tv_sec += (add + secs);
        ts.tv_nsec = msecs % NANOSECONDS;
        return sem_timedwait(&sem, &ts);
    }
};

#endif  // LIFECYCLE_TEST_BASE_H