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

#ifndef CAMERA_DEMO_TEST_H
#define CAMERA_DEMO_TEST_H

#include "gtest/gtest.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <string>
#include <sys/time.h>
#include "camera_kit.h"
#include "recorder.h"

const int32_t FRAME_RATE_DEFAULT = 30;
const int32_t OK = 0;
const int32_t FAIL = -1;
const int32_t QUEUE_SIZE = 3;
const int32_t BUFFER_SIZE = 1024;
static std::string g_filePath = "/test_root/multimedia/";
std::string g_testPath;

// CallBack Flag
enum TestCallBackFlag {
    FLAG0 = 0,
    FLAG1 = 1,
};

// VideoSize
enum TestVideoSize {
    WIDTH = 1920,
    HEIGHT = 1080,
};

class ActsMediaCameraTest : public testing::Test {
public:
    // SetUpTestCase: before all testcasee
    static void SetUpTestCase(void);
    // TearDownTestCase: after all testcase
    static void TearDownTestCase(void);
    // SetUp
    void SetUp(void);
    // TearDown
    void TearDown(void);
};

class CameraFlag {
public:
    // CameraSetupFlag
    static int32_t g_onGetCameraAbilityFlag;
    static int32_t g_onConfigureFlag;
    static int32_t g_onGetSupportedSizesFlag;
    // CameraDeviceCallBack Flag
    static int32_t g_onCameraAvailableFlag;
    static int32_t g_onCameraUnavailableFlag;
    // CameraStateCallback
    static int32_t g_onCreatedFlag;
    static int32_t g_onCreateFailedFlag;
    static int32_t g_onReleasedFlag;
    static int32_t g_onConfiguredFlag;
    static int32_t g_onConfigureFailedFlag;
    // FrameStateCallback
    static int32_t g_onCaptureTriggerAbortedFlag;
    static int32_t g_onCaptureTriggerCompletedFlag;
    static int32_t g_onCaptureTriggerStartedFlag;
    static int32_t g_onGetFrameConfigureType;
    static int32_t g_onFrameFinishedFlag;
    static int32_t g_onFrameErrorFlag;
    static int32_t g_onFrameProgressedFlag;
    static int32_t g_onFrameStartedFlag;
};
// CameraSetup
int32_t CameraFlag::g_onGetCameraAbilityFlag = FLAG0;
int32_t CameraFlag::g_onConfigureFlag = FLAG0;
int32_t CameraFlag::g_onGetSupportedSizesFlag = FLAG0;
// CameraDeviceCallBack
int32_t CameraFlag::g_onCameraAvailableFlag = FLAG0;
int32_t CameraFlag::g_onCameraUnavailableFlag = FLAG0;
// CameraStateCallback
int32_t CameraFlag::g_onCreatedFlag = FLAG0;
int32_t CameraFlag::g_onCreateFailedFlag = FLAG0;
int32_t CameraFlag::g_onConfiguredFlag = FLAG0;
int32_t CameraFlag::g_onConfigureFailedFlag = FLAG0;
int32_t CameraFlag::g_onReleasedFlag = FLAG0;
// FrameStateCallback
int32_t CameraFlag::g_onCaptureTriggerAbortedFlag = FLAG0;
int32_t CameraFlag::g_onCaptureTriggerCompletedFlag = FLAG0;
int32_t CameraFlag::g_onCaptureTriggerStartedFlag = FLAG0;
int32_t CameraFlag::g_onGetFrameConfigureType = FLAG0;
int32_t CameraFlag::g_onFrameFinishedFlag = FLAG0;
int32_t CameraFlag::g_onFrameErrorFlag = FLAG0;
int32_t CameraFlag::g_onFrameProgressedFlag = FLAG0;
int32_t CameraFlag::g_onFrameStartedFlag = FLAG0;

#endif