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

#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <gtest/gtest.h>
#include "log.h"
#include "utils.h"
#include "ClockID.h"

using namespace testing::ext;

const int SLEEP_ACCURACY = 21 * 1000; // 20 ms, with 1ms deviation
const int ACCURACY_TEST_LOOPS = 5;    // loops for accuracy test, than count average value

class UsleepParamTest : public testing::TestWithParam<int> {};
class SleepParamTest : public testing::TestWithParam<int> {};
class SleepTest : public testing::Test {};

/**
 * @tc.number SUB_KERNEL_TIME_API_USLEEP_0100
 * @tc.name   usleep accuracy test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_P(UsleepParamTest, testUsleepAccuracy, Performance | SmallTest | Level1)
{
    int interval = GetParam();
    LOG("\ntest interval:%d", interval);
    struct timespec time1 = {0}, time2 = {0};
    long duration; // unit: us
    double d = 0.0;
    for (int i = 1; i <= ACCURACY_TEST_LOOPS; i++) {
        clock_gettime(CLOCK_MONOTONIC, &time1);
        int rt = usleep(interval);
        clock_gettime(CLOCK_MONOTONIC, &time2);
        EXPECT_EQ(rt, 0);
        duration = (time2.tv_sec*1000000 + time2.tv_nsec/1000) - (time1.tv_sec*1000000 + time1.tv_nsec/1000);
        LOG("testloop %d, actual usleep duration: %ld us", i, duration);
        d += duration;
    }
    d = d / ACCURACY_TEST_LOOPS; // average
    LOG("average duration: %.2f", d);
    EXPECT_GE(d, interval) << "actual sleep time shoud greater or equal to the input-parameter\n";
    ASSERT_NEAR(d, interval, SLEEP_ACCURACY) << "usleep accuracy check fail\n";
}
INSTANTIATE_TEST_CASE_P(SleepTest, UsleepParamTest,
    testing::Values(1000, 10*1000, 20*1000, 30*1000, 300*1000, 3000*1000));

/**
 * @tc.number SUB_KERNEL_TIME_API_SLEEP_0100
 * @tc.name   sleep accuracy test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_P(SleepParamTest, testSleepAccuracy, Performance | SmallTest | Level1)
{
    int testLoop = 3;
    int interval = GetParam();
    LOG("\ntest interval:%d", interval);
    struct timespec time1 = {0}, time2 = {0};
    double duration;
    double d = 0.0;
    for (int i = 1; i <= testLoop; i++) {
        clock_gettime(CLOCK_MONOTONIC, &time1);
        int rt = sleep(interval);
        clock_gettime(CLOCK_MONOTONIC, &time2);
        EXPECT_EQ(rt, 0);
        duration = (time2.tv_sec - time1.tv_sec) + (time2.tv_nsec - time1.tv_nsec)/1000000000.0;
        LOG("testloop %d, actual sleep duration: %.1f s", i, duration);
        d += duration;
    }
    d = d / testLoop; // average
    LOG("average duration: %.2f", d);
    ASSERT_NEAR(d, interval, interval*0.03) << "sleep accuracy check fail\n";
}
INSTANTIATE_TEST_CASE_P(SleepTest, SleepParamTest, testing::Values(1, 5, 30));

/**
 * @tc.number SUB_KERNEL_TIME_API_NANOSLEEP_0100
 * @tc.name   nanosleep accuracy test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(SleepTest, testNanosleepAccuracy, Performance | SmallTest | Level2)
{
    long interval = 50*1000*1000;
    struct timespec req = {0, interval};
    struct timespec rem = {0, 0};

    struct timespec time1 = {0}, time2 = {0};
    double duration;
    double d = 0.0;
    for (int i = 1; i <= ACCURACY_TEST_LOOPS; i++) {
        clock_gettime(CLOCK_MONOTONIC, &time1);
        int rt = nanosleep(&req, &rem);
        clock_gettime(CLOCK_MONOTONIC, &time2);
        EXPECT_EQ(rt, 0);
        duration = (time2.tv_sec*1000000 + time2.tv_nsec/1000) - (time1.tv_sec*1000000 + time1.tv_nsec/1000);
        LOG("testloop %d, actual sleep duration: %.1f s", i, duration);
        d += duration;
    }
    d = d / ACCURACY_TEST_LOOPS; // average
    LOG("average duration: %.2f", d);
    ASSERT_NEAR(d, interval/1000, SLEEP_ACCURACY) << "sleep accuracy check fail\n";
}

/**
 * @tc.number SUB_KERNEL_TIME_API_CLOCK_NANOSLEEP_0100
 * @tc.name   clock_nanosleep accuracy test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(SleepTest, testClockNanosleepAccuracy, Performance | SmallTest | Level2)
{
    long interval = 25*1000*1000;
    struct timespec req = {0, interval};
    struct timespec rem = {0, 0};
    struct timespec time1 = {0}, time2 = {0};
    double duration;
    double d = 0.0;
    for (int i = 1; i <= ACCURACY_TEST_LOOPS; i++) {
        clock_gettime(CLOCK_MONOTONIC, &time1);
        int rt = clock_nanosleep(CLOCK_REALTIME, 0, &req, &rem);
        clock_gettime(CLOCK_MONOTONIC, &time2);
        EXPECT_EQ(rt, 0);
        duration = (time2.tv_sec*1000000 + time2.tv_nsec/1000) - (time1.tv_sec*1000000 + time1.tv_nsec/1000);
        LOG("testloop %d, actual sleep duration: %.1f s", i, duration);
        d += duration;
    }
    d = d / ACCURACY_TEST_LOOPS; // average
    LOG("average duration: %.2f", d);
    ASSERT_NEAR(d, interval/1000, SLEEP_ACCURACY) << "sleep accuracy check fail\n";
}

/**
 * @tc.number SUB_KERNEL_TIME_API_CLOCK_NANOSLEEP_0200
 * @tc.name   clock_nanosleep fail test - non-support clock_id
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_P(AllClockIDTest, testClockNanosleepInvalidID, Reliability | SmallTest | Level2)
{
    clockid_t cid = GetParam();
    const char* cname = ALL_CLOCKS_NAME[cid];
    LOG("test %s", cname);
    struct timespec req = {0, 100};
    struct timespec rem = {0};
    int rt = clock_nanosleep(cid, 0, &req, &rem);
    if (cid == CLOCK_REALTIME) {
        ASSERT_EQ(rt, 0);
    } else if (cid == CLOCK_THREAD_CPUTIME_ID) {
        ASSERT_EQ(rt, EINVAL) << cname << " should not support.\n";
    } else {
        ASSERT_EQ(rt, ENOTSUP) << cname << " should not support.\n";
    }
}
INSTANTIATE_TEST_CASE_P(SleepTest, AllClockIDTest, ALL_CLOCK_IDS);


/**
 * @tc.number SUB_KERNEL_TIME_API_CLOCK_NANOSLEEP_0300
 * @tc.name   clock_nanosleep fail test - invalid parameter
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(SleepTest, testClockNanosleepInvalidPara, Reliability | SmallTest | Level2)
{
    struct timespec req = {0, 100};
    struct timespec rem = {0};
    int rt;

    // invlid clock_id
    int id = GetRandom(1000) + 12;
    LOG("check invlid clockid: %d...", id);
    rt = clock_nanosleep(id, 0, &req, &rem);
    EXPECT_EQ(rt, EINVAL);

    id = -GetRandom(1000) - 12;
    LOG("check invlid clockid: %d...", id);
    rt = clock_nanosleep(id, 0, &req, &rem);
    EXPECT_EQ(rt, EINVAL);

    // invlid flag
    int flag = TIMER_ABSTIME;
    LOG("check invlid flag: %d...", flag);
    rt = clock_nanosleep(CLOCK_REALTIME, flag, &req, &rem);
    EXPECT_EQ(rt, ENOTSUP);
    flag = GetRandom(100) + 1;
    LOG("check invlid flag: %d...", flag);
    rt = clock_nanosleep(CLOCK_REALTIME, flag, &req, &rem);
    EXPECT_EQ(rt, EINVAL);
    flag = -GetRandom(100) - 1;
    LOG("check invlid flag: %d...", flag);
    rt = clock_nanosleep(CLOCK_REALTIME, flag, &req, &rem);
    EXPECT_EQ(rt, EINVAL);

    // invlid timespec
    req.tv_sec  = -1;
    req.tv_nsec = 1;
    LOG("check invlid timespec: tv_sec=-1 ...");
    rt = clock_nanosleep(CLOCK_REALTIME, 0, &req, &rem);
    EXPECT_EQ(rt, EINVAL);
    req.tv_sec  = 1;
    req.tv_nsec = -1;
    LOG("check invlid timespec: tv_nsec=-1 ...");
    rt = clock_nanosleep(CLOCK_REALTIME, 0, &req, &rem);
    EXPECT_EQ(rt, EINVAL);
    req.tv_sec  = 1;
    req.tv_nsec = 1000*1000*1000 + 1;
    LOG("check invlid timespec: tv_nsec overflow ...");
    rt = clock_nanosleep(CLOCK_REALTIME, 0, &req, &rem);
    EXPECT_EQ(rt, EINVAL);

    // invlid remain
    // para not used, so not tested.
}
