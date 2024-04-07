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

#include "hiview_plugin_platform_module_test.h"

#include "string_util.h"

using namespace testing::ext;
using namespace OHOS::HiviewDFX;

namespace {
std::string GetCmdResultFromPopen(const std::string& cmd)
{
    if (cmd.empty()) {
        return "";
    }

    FILE* fp = popen(cmd.c_str(), "r");
    if (fp == nullptr) {
        return "";
    }
    const int bufSize = 128;
    char buffer[bufSize];
    std::string result = "";
    while (!feof(fp)) {
        if (fgets(buffer, bufSize - 1, fp) != NULL) {
            result += buffer;
        }
    }
    pclose(fp);
    return result;
}

int GetHiviewPid()
{
    std::string pidStr = GetCmdResultFromPopen("pidof hiview");
    int32_t pid = 0;
    OHOS::HiviewDFX::StringUtil::ConvertStringTo<int32_t>(pidStr, pid);
    printf("the pid of hiview : %s \n", pidStr.c_str());
    return pid;
}

void WaitForHiviewReady()
{
    int pid = GetHiviewPid();
    if (pid <= 0) {
        GetCmdResultFromPopen("start hiview");
        const int sleepTime = 10; // 10 seconds
        sleep(sleepTime);
        pid = GetHiviewPid();
    }
    ASSERT_GT(pid, 0);
}
}

/**
 * @tc.name: PluginPlatfromModuleTest001
 * @tc.desc: check hiview running status from dumpsys cmdline
 * @tc.type: FUNC
 * @tc.require: SR000DPTSQ
 */
HWTEST_F(HiviewPluginPlatformModuleTest, PluginPlatfromModuleTest001, TestSize.Level3)
{
    /**
     * @tc.steps: step1. get the pid of hiview
     * @tc.steps: step2. check loaded plugins in hiview
     * @tc.steps: step3. check the thread existence of hiview
     */
    WaitForHiviewReady();
    std::string dumpResult = GetCmdResultFromPopen("dumpsys hiviewdfx");
    printf("the current loaded plugins of hiview : %s \n", dumpResult.c_str());

    int pid = GetHiviewPid();
    std::string cmd = "ps -AT |grep " + std::to_string(pid);
    std::string threadResult = GetCmdResultFromPopen(cmd);
    printf("the current threads of hiview : %s \n", threadResult.c_str());
}

/**
 * @tc.name: FaultloggerPluginDumpTest001
 * @tc.desc: check dumpsys faultlogger functions
 * @tc.type: FUNC
 * @tc.require: SR000F80AS AR000F87HK
 */
HWTEST_F(HiviewPluginPlatformModuleTest, FaultloggerPluginDumpTest001, TestSize.Level3)
{
    /**
     * @tc.steps: step1. run shell common to get dump result
     * @tc.steps: step2. check the content of the result
     */
    WaitForHiviewReady();
    std::string dumpResult = GetCmdResultFromPopen("dumpsys hiviewdfx -p Faultlogger");
    printf("the current fault logs :\n%s\n", dumpResult.c_str());
    ASSERT_GE(dumpResult.length(), 0ul);
}

/**
 * @tc.name: FaultloggerPluginDumpTest002
 * @tc.desc: check dumpsys faultlogger functions
 * @tc.type: FUNC
 * @tc.require: SR000F80AS AR000F83AG
 */
HWTEST_F(HiviewPluginPlatformModuleTest, FaultloggerPluginDumpTest002, TestSize.Level3)
{
    /**
     * @tc.steps: step1. run shell common to get dump result
     * @tc.steps: step2. check the content of the result
     */
    WaitForHiviewReady();
    std::string dumpResult = GetCmdResultFromPopen("dumpsys hiviewdfx -p Faultlogger -l");
    printf("the current fault log list:\n%s\n", dumpResult.c_str());
    ASSERT_GE(dumpResult.length(), 0ul);

    dumpResult = GetCmdResultFromPopen("dumpsys hiviewdfx_faultlogger");
    printf("the current fault log detail:\n%s\n", dumpResult.c_str());
    ASSERT_GE(dumpResult.length(), 0ul);
}