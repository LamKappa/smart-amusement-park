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

#include "ActsMediaCameraTest.h"
#include "ActsTestMediaUtils.h"

using namespace std;
using namespace OHOS;
using namespace OHOS::Media;
using namespace testing::ext;

// SetUpTestCase
void ActsMediaCameraTest::SetUpTestCase(void)
{
    g_testPath = GetCurDir();
    cout << "SetUpTestCase" << endl;
}

// TearDownTestCase
void ActsMediaCameraTest::TearDownTestCase(void)
{
    g_testPath = "";
    cout << "TearDownTestCase" << endl;
}

void ActsMediaCameraTest::SetUp(void)
{
    // CameraSetUp
    CameraFlag::g_onGetCameraAbilityFlag = FLAG0;
    CameraFlag::g_onConfigureFlag = FLAG0;
    CameraFlag::g_onGetSupportedSizesFlag = FLAG0;
    // CameraDeviceCallBack
    CameraFlag::g_onCameraAvailableFlag = FLAG0;
    CameraFlag::g_onCameraUnavailableFlag = FLAG0;
    // CameraStateCallback
    CameraFlag::g_onCreatedFlag = FLAG0;
    CameraFlag::g_onCreateFailedFlag = FLAG0;
    CameraFlag::g_onConfiguredFlag = FLAG0;
    CameraFlag::g_onConfigureFailedFlag = FLAG0;
    CameraFlag::g_onReleasedFlag = FLAG0;
    // FrameStateCallback
    CameraFlag::g_onCaptureTriggerAbortedFlag = FLAG0;
    CameraFlag::g_onCaptureTriggerCompletedFlag = FLAG0;
    CameraFlag::g_onCaptureTriggerStartedFlag = FLAG0;
    CameraFlag::g_onFrameFinishedFlag = FLAG0;
    CameraFlag::g_onGetFrameConfigureType = FLAG0;
    CameraFlag::g_onFrameErrorFlag = FLAG0;
    CameraFlag::g_onFrameProgressedFlag = FLAG0;
    CameraFlag::g_onFrameStartedFlag = FLAG0;
    cout << "SetUp" << endl;
}

// Tear down
void ActsMediaCameraTest::TearDown(void)
{
    cout << "TearDown." << endl;
}

/* *
 * creat Recorder
 */
Recorder *SampleCreateRecorder()
{
    int ret = 0;
    int32_t sampleRate = 48000;
    int32_t channelCount = 1;
    AudioCodecFormat audioFormat = AAC_LC;
    AudioSourceType inputSource = AUDIO_MIC;
    int32_t audioEncodingBitRate = sampleRate;
    VideoSourceType source = VIDEO_SOURCE_SURFACE_ES;
    int32_t frameRate = 30;
    double fps = 30;
    int32_t rate = 4096;
    int32_t sourceId = 0;
    int32_t audioSourceId = 0;
    int32_t width = 1920;
    int32_t height = 1080;
    VideoCodecFormat encoder;
    encoder = HEVC;
    Recorder *recorder = new Recorder();
    if ((ret = recorder->SetVideoSource(source, sourceId)) != SUCCESS) {
        cout << "SetVideoSource failed." << ret << endl;
        delete recorder;
        return nullptr;
    }
    if ((ret = recorder->SetVideoEncoder(sourceId, encoder)) != SUCCESS) {
        cout << "SetVideoEncoder failed." << ret << endl;
        delete recorder;
        return nullptr;
    }
    if ((ret = recorder->SetVideoSize(sourceId, width, height)) != SUCCESS) {
        cout << "SetVideoSize failed." << ret << endl;
        delete recorder;
        return nullptr;
    }
    if ((ret = recorder->SetVideoFrameRate(sourceId, frameRate)) != SUCCESS) {
        cout << "SetVideoFrameRate failed." << ret << endl;
        delete recorder;
        return nullptr;
    }
    if ((ret = recorder->SetVideoEncodingBitRate(sourceId, rate)) != SUCCESS) {
        cout << "SetVideoEncodingBitRate failed." << ret << endl;
        delete recorder;
        return nullptr;
    }
    if ((ret = recorder->SetCaptureRate(sourceId, fps)) != SUCCESS) {
        cout << "SetCaptureRate failed." << ret << endl;
        delete recorder;
        return nullptr;
    }
    if ((ret = recorder->SetAudioSource(inputSource, audioSourceId)) != SUCCESS) {
        cout << "SetAudioSource failed." << ret << endl;
        delete recorder;
        return nullptr;
    }
    if ((ret = recorder->SetAudioEncoder(audioSourceId, audioFormat)) != SUCCESS) {
        cout << "SetAudioEncoder failed." << ret << endl;
        delete recorder;
        return nullptr;
    }
    if ((ret = recorder->SetAudioSampleRate(audioSourceId, sampleRate)) != SUCCESS) {
        cout << "SetAudioSampleRate failed." << ret << endl;
        delete recorder;
        return nullptr;
    }
    if ((ret = recorder->SetAudioChannels(audioSourceId, channelCount)) != SUCCESS) {
        cout << "SetAudioChannels failed." << ret << endl;
        delete recorder;
        return nullptr;
    }
    if ((ret = recorder->SetAudioEncodingBitRate(audioSourceId, audioEncodingBitRate)) != SUCCESS) {
        cout << "SetAudioEncodingBitRate failed." << ret << endl;
        delete recorder;
        return nullptr;
    }
    return recorder;
}

/* *
 * Create Frame StateCallback
 */
class SampleFrameStateCallback : public FrameStateCallback {
    void OnFrameFinished(Camera &camera, FrameConfig &fc, FrameResult &result) override
    {
        CameraFlag::g_onFrameStartedFlag = FLAG1;
        CameraFlag::g_onFrameProgressedFlag = FLAG1;
        cout << "Receive frame complete inform." << endl;
        if (fc.GetFrameConfigType() == FRAME_CONFIG_CAPTURE) {
            CameraFlag::g_onGetFrameConfigureType = FLAG1;
            cout << "Capture frame received." << endl;
            list<Surface *> surfaceList = fc.GetSurfaces();
            for (Surface *surface : surfaceList) {
                SurfaceBuffer *buffer = surface->AcquireBuffer();
                if (buffer != nullptr) {
                    char *virtAddr = static_cast<char *>(buffer->GetVirAddr());
                    if (virtAddr != nullptr) {
                        SampleSaveCapture(g_testPath, virtAddr, buffer->GetSize());
                    } else {
                        CameraFlag::g_onFrameErrorFlag = FLAG1;
                    }
                    surface->ReleaseBuffer(buffer);
                } else {
                    CameraFlag::g_onFrameErrorFlag = FLAG1;
                }
                delete surface;
            }
            delete &fc;
        } else {
            CameraFlag::g_onFrameErrorFlag = FLAG1;
        }
        CameraFlag::g_onFrameFinishedFlag = FLAG1;
    }
};

/* *
 * create CameraStateCallback
 */
class SampleCameraStateMng : public CameraStateCallback {
public:
    SampleCameraStateMng() = delete;

    explicit SampleCameraStateMng(EventHandler &eventHdlr) : eventHandler_(eventHdlr) {}

    ~SampleCameraStateMng()
    {
        if (recorder_ != nullptr) {
            recorder_->Release();
            delete recorder_;
        }
        if (cam_ != nullptr) {
            cam_->Release();
        }
    }

    void OnCreated(Camera &c) override
    {
        CameraFlag::g_onCreatedFlag = FLAG1;
        cout << "Sample recv OnCreate camera." << endl;
        auto config = CameraConfig::CreateCameraConfig();
        config->SetFrameStateCallback(&fsCb_, &eventHandler_);
        c.Configure(*config);
        CameraFlag::g_onConfigureFlag = FLAG1;
        cam_ = &c;
    }

    void OnCreateFailed(const std::string cameraId, int32_t errorCode) override
    {
        CameraFlag::g_onCreateFailedFlag = FLAG1;
        cout << "Sample recv OnCreateFailed camera." << endl;
    }

    void OnReleased(Camera &c) override
    {
        CameraFlag::g_onReleasedFlag = FLAG1;
        cout << "Sample recv OnReleased camera." << endl;
    }

    void StartRecord()
    {
        int ret;
        if (isRecording_) {
            cout << "Camera is already recording." << endl;
            return;
        }
        if (recorder_ == nullptr) {
            recorder_ = SampleCreateRecorder();
        }
        if (recorder_ == nullptr) {
            cout << "Recorder not available" << endl;
            return;
        }
        string path = GetCurDir();
        ret = recorder_->SetOutputPath(path);
        if (ret != SUCCESS) {
            cout << "SetOutputPath failed :" << ret << std::endl;
            return;
        }

        ret = recorder_->Prepare();
        if (ret != SUCCESS) {
            cout << "Prepare failed.=" << ret << endl;
            return;
        }
        Surface *surface = (recorder_->GetSurface(0)).get();
        surface->SetWidthAndHeight(WIDTH, HEIGHT);
        surface->SetQueueSize(QUEUE_SIZE);
        surface->SetSize(BUFFER_SIZE * BUFFER_SIZE);
        FrameConfig *fc = new FrameConfig(FRAME_CONFIG_RECORD);
        fc->AddSurface(*surface);
        ret = recorder_->Start();
        if (ret != SUCCESS) {
            delete fc;
            cout << "recorder start failed. ret=" << ret << endl;
            return;
        }
        static int cnt = 3;
        while (cam_ == nullptr) {
            if (cnt-- < 0)
                break;
            cout << "Wait camera created success" << endl;
            sleep(1);
        }

        ret = cam_->TriggerLoopingCapture(*fc);
        if (ret != 0) {
            delete fc;
            cout << "camera start recording failed. ret=" << ret << endl;
            return;
        }
        isRecording_ = true;
        cout << "camera start recording succeed." << endl;
    }

    void StartPreview()
    {
        if (isPreviewing_) {
            cout << "Camera is already previewing." << endl;
            return;
        }
        FrameConfig *fc = new FrameConfig(FRAME_CONFIG_PREVIEW);
        Surface *surface = Surface::CreateSurface();
        if (surface == nullptr) {
            delete fc;
            cout << "CreateSurface failed" << endl;
            return;
        }
        surface->SetWidthAndHeight(WIDTH, HEIGHT);
        surface->SetUserData("region_position_x", "0");
        surface->SetUserData("region_position_y", "0");
        surface->SetUserData("region_width", "480");
        surface->SetUserData("region_height", "480");
        fc->AddSurface(*surface);
        static int cnt = 3;
        while (cam_ == nullptr) {
            if (cnt-- < 0)
                break;
            cout << "Wait camera created success" << endl;
            sleep(1);
        }
        int32_t ret = cam_->TriggerLoopingCapture(*fc);
        if (ret != 0) {
            delete fc;
            cout << "camera start preview failed. ret=" << ret << endl;
            return;
        }
        CameraFlag::g_onCaptureTriggerCompletedFlag = FLAG1;
        delete surface;
        isPreviewing_ = true;
        cout << "camera start preview succeed." << endl;
    }

    void Capture()
    {
        FrameConfig *fc = new FrameConfig(FRAME_CONFIG_CAPTURE);
        Surface *surface = Surface::CreateSurface();
        if (surface == nullptr) {
            delete fc;
            cout << "CreateSurface failed" << endl;
            return;
        }
        surface->SetWidthAndHeight(1920, 1080); /* 1920:width,1080:height */
        fc->AddSurface(*surface);
        static int cnt = 3;
        while (cam_ == nullptr) {
            if (cnt-- < 0)
                break;
            cout << "Wait camera created success" << endl;
            sleep(1);
        }
        CameraFlag::g_onCaptureTriggerStartedFlag = FLAG1;
        cam_->TriggerSingleCapture(*fc);
        CameraFlag::g_onCaptureTriggerCompletedFlag = FLAG1;
    }

    void Stop()
    {
        if (recorder_ != nullptr) {
            recorder_->Stop(false);
        }

        while (cam_ == nullptr) {
            cout << "Camera is not ready." << endl;
            return;
        }
        cam_->StopLoopingCapture();
        isPreviewing_ = false;
        isRecording_ = false;
    }

    bool isPreviewing_ = false;
    bool isRecording_ = false;
    EventHandler &eventHandler_;
    Camera *cam_ = nullptr;
    Recorder *recorder_ = nullptr;
    SampleFrameStateCallback fsCb_;
};

/* *
 * create CameraStateCallback for state test
 */
class SampleCameraStateCallback : public CameraStateCallback {
public:
    SampleCameraStateCallback() = delete;

    explicit SampleCameraStateCallback(EventHandler &eventHdlr) : eventHandler_(eventHdlr) {}

    ~SampleCameraStateCallback()
    {
        if (cam_ != nullptr) {
            cam_->Release();
        }
    }

    void OnCreated(Camera &c) override
    {
        CameraFlag::g_onCreatedFlag = FLAG1;
        cout << "camera Create success." << endl;
        auto config = CameraConfig::CreateCameraConfig();
        config->SetFrameStateCallback(&fsCb_, &eventHandler_);
        c.Configure(*config);
        cam_ = &c;
    }

    void OnCreateFailed(const std::string cameraId, int32_t errorCode) override
    {
        CameraFlag::g_onCreateFailedFlag = FLAG1;
        cout << "Camera Create Failed." << endl;
    }

    void OnReleased(Camera &c) override
    {
        CameraFlag::g_onReleasedFlag = FLAG1;
        cout << "camera Releasedsuccess." << endl;
    }

    void OnConfigured(Camera &c) override
    {
        CameraFlag::g_onConfiguredFlag = FLAG1;
        cout << "Camera Configured success." << endl;
    }

    void OnConfigureFailed(const std::string cameraId, int32_t errorCode) override
    {
        CameraFlag::g_onConfigureFailedFlag = FLAG1;
        cout << "Camera Configured failed." << endl;
    }

    EventHandler &eventHandler_;
    Camera *cam_ = nullptr;
    SampleFrameStateCallback fsCb_;
};

/* *
 * Creat camera device callback
 */
class SampleCameraDeviceCallback : public CameraDeviceCallback {
public:
    SampleCameraDeviceCallback() {}

    ~SampleCameraDeviceCallback() {}

    // camera device status changed
    void OnCameraStatus(std::string cameraId, int32_t status) override
    {
        cout << "SampleCameraDeviceCallback OnCameraStatus\n" << endl;
        if (status == CAMERA_DEVICE_STATE_AVAILABLE) {
            CameraFlag::g_onCameraAvailableFlag = FLAG1;
            cout << "SampleCameraDeviceCallback onCameraAvailable\n" << endl;
        } else if (status == CAMERA_DEVICE_STATE_UNAVAILABLE) {
            CameraFlag::g_onCameraUnavailableFlag = FLAG1;
            cout << "SampleCameraDeviceCallback onCameraUnavailable\n" << endl;
        }
    }
};

/* *
 * Get camera Id
 */
void GetCameraId(CameraKit *cameraKit, list<string> &camList, string &camId)
{
    cameraKit = CameraKit::GetInstance();
    camList = cameraKit->GetCameraIds();
    for (auto &cam : camList) {
        cout << "camera name:" << cam << endl;
        const CameraAbility *ability = cameraKit->GetCameraAbility(cam);
        ASSERT_NE(ability, nullptr);
        CameraFlag::g_onGetCameraAbilityFlag = FLAG1;
        /* find camera which fits user's ability */
        list<CameraPicSize> sizeList = ability->GetSupportedSizes(0);
        if (sizeList.size() != 0) {
            CameraFlag::g_onGetSupportedSizesFlag = FLAG1;
        }
        for (auto &pic : sizeList) {
            cout << "Pic size: " << pic.width << "x" << pic.height << endl;
            if (pic.width == WIDTH && pic.height == HEIGHT) {
                /* 1920:width,1080:height */
                camId = cam;
                break;
            }
        }
    }
    if (camId.empty()) {
        cout << "No available camera.(1080p wanted)" << endl;
        return;
    }
}

using namespace OHOS;
using namespace std;

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_0100
 * @tc.name      : Get cameraIDs
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaCameraTest, Test_GetCameraIDs, Function | MediumTest | Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> camList;
    string camId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, camList, camId);
    EXPECT_FALSE(camId.empty());
    EXPECT_EQ("main", camId);
    cameraKit = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_0200
 * @tc.name      : Get cameraAbility
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaCameraTest, Test_GetCameraAbility, Function | MediumTest | Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> camList;
    string camId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, camList, camId);
    SampleCameraStateMng camStateMng(eventHdlr);
    EXPECT_EQ(CameraFlag::g_onGetCameraAbilityFlag, FLAG1);
    cameraKit = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_0300
 * @tc.name      : CameraKit get instance
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaCameraTest, TestCreatCamerakit, Function | MediumTest | Level1)
{
    CameraKit *cameraKit = nullptr;
    cameraKit = CameraKit::GetInstance();
    EXPECT_NE(nullptr, cameraKit);
    cameraKit = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_0400
 * @tc.name      : Get cameraKit deviceCallback
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaCameraTest, TestNewDeviceCallback, Function | MediumTest | Level1)
{
    SampleCameraDeviceCallback *deviceCallback = nullptr;
    deviceCallback = new SampleCameraDeviceCallback();
    EXPECT_NE(nullptr, deviceCallback);
    delete deviceCallback;
    deviceCallback = nullptr;
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_0500
 * @tc.name      : Get cameraKit supported Size
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaCameraTest, TestGetSupportedSizes, Function | MediumTest | Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> camList;
    string camId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, camList, camId);
    EXPECT_EQ(CameraFlag::g_onGetSupportedSizesFlag, FLAG1);
    cameraKit = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_0600
 * @tc.name      : Register Camera device callback
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaCameraTest, Test_RegisterCameraDeviceCallback, Function | MediumTest | Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> camList;
    string camId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, camList, camId);
    SampleCameraStateMng camStateMng(eventHdlr);
    SampleCameraDeviceCallback *deviceCallback = nullptr;
    deviceCallback = new SampleCameraDeviceCallback();
    ASSERT_NE(nullptr, deviceCallback);
    cameraKit->RegisterCameraDeviceCallback(*deviceCallback, eventHdlr);
    sleep(1);
    EXPECT_EQ(CameraFlag::g_onCameraAvailableFlag, FLAG1);
    delete deviceCallback;
    deviceCallback = nullptr;
    cameraKit = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_0700
 * @tc.name      : Unregister Camera Device Callback
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaCameraTest, Test_UnregisterCameraDeviceCallback, Function | MediumTest | Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> camList;
    string camId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, camList, camId);
    SampleCameraStateMng camStateMng(eventHdlr);
    SampleCameraDeviceCallback *deviceCallback = nullptr;
    deviceCallback = new SampleCameraDeviceCallback();
    ASSERT_NE(nullptr, deviceCallback);
    cameraKit->RegisterCameraDeviceCallback(*deviceCallback, eventHdlr);
    sleep(1);
    EXPECT_EQ(CameraFlag::g_onCameraAvailableFlag, FLAG1);
    cameraKit->UnregisterCameraDeviceCallback(*deviceCallback);
    sleep(1);
    EXPECT_EQ(CameraFlag::g_onCameraUnavailableFlag, FLAG0);
    delete deviceCallback;
    deviceCallback = nullptr;
    cameraKit = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_0800
 * @tc.name      : Capture On Configure
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaCameraTest, TestCaptureOnConfigure, Function | MediumTest | Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> camList;
    string camId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, camList, camId);
    SampleCameraStateMng camStateMng(eventHdlr);
    sleep(1);
    cameraKit->CreateCamera(camId, camStateMng, eventHdlr);
    sleep(1);
    EXPECT_EQ(CameraFlag::g_onConfigureFlag, FLAG1);
    EXPECT_NE(camStateMng.cam_, nullptr);
    cameraKit = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_0900
 * @tc.name      : Create Camera
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaCameraTest, TestCreateCamera, Function | MediumTest | Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> camList;
    string camId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, camList, camId);
    SampleCameraStateMng camStateMng(eventHdlr);
    sleep(3);
    cameraKit->CreateCamera(camId, camStateMng, eventHdlr);
    sleep(3);
    EXPECT_EQ(CameraFlag::g_onCreatedFlag, FLAG1);
    EXPECT_NE(CameraFlag::g_onCreateFailedFlag, FLAG1);
    EXPECT_NE(camStateMng.cam_, nullptr);
    cameraKit = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_1000
 * @tc.name      : Create Camera with error camera id
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaCameraTest, TestCapture_Create_Camera_ERROR_cameraId, Function | MediumTest | Level1)
{
    CameraKit *cameraKit = nullptr;
    cameraKit = CameraKit::GetInstance();
    string camId = "0";
    EventHandler eventHdlr;
    SampleCameraStateMng camStateMng(eventHdlr);
    sleep(3);
    cameraKit->CreateCamera(camId, camStateMng, eventHdlr);
    sleep(3);
    EXPECT_EQ(CameraFlag::g_onCreatedFlag, FLAG0);
    EXPECT_EQ(CameraFlag::g_onCreateFailedFlag, FLAG1);
    cameraKit = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_1100
 * @tc.name      : Test Capture Frame config
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaCameraTest, TestCapture_Frame_config, Function | MediumTest | Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> camList;
    string camId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, camList, camId);
    SampleCameraStateMng camStateMng(eventHdlr);
    cameraKit->CreateCamera(camId, camStateMng, eventHdlr);
    sleep(3);
    FrameConfig *fc = new FrameConfig(FRAME_CONFIG_CAPTURE);
    EXPECT_NE(fc, nullptr);
    delete fc;
    cameraKit = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_1200
 * @tc.name      : Test Create Surface
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaCameraTest, TestCapture_Create_Surface, Function | MediumTest | Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> camList;
    string camId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, camList, camId);
    SampleCameraStateMng camStateMng(eventHdlr);
    sleep(3);
    cameraKit->CreateCamera(camId, camStateMng, eventHdlr);
    sleep(3);
    FrameConfig *fc = new FrameConfig(FRAME_CONFIG_CAPTURE);
    Surface *surface = Surface::CreateSurface();
    if (surface == nullptr) {
        delete fc;
        cout << "CreateSurface failed" << endl;
    }
    EXPECT_NE(surface, nullptr);
    delete surface;
    delete fc;
    cameraKit = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_1300
 * @tc.name      : Get Surface Size
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaCameraTest, TestCapture_GetSurfaceSize, Function | MediumTest | Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> camList;
    string camId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, camList, camId);
    SampleCameraStateMng camStateMng(eventHdlr);
    sleep(3);
    cameraKit->CreateCamera(camId, camStateMng, eventHdlr);
    sleep(3);
    FrameConfig *fc = new FrameConfig(FRAME_CONFIG_CAPTURE);
    Surface *surface = Surface::CreateSurface();
    if (surface == nullptr) {
        delete fc;
        cout << "CreateSurface failed" << endl;
    }
    surface->SetWidthAndHeight(1920, 1080); /* 1920:width,1080:height */
    EXPECT_EQ(1920, surface->GetWidth());
    EXPECT_EQ(1080, surface->GetHeight());
    delete surface;
    delete fc;
    cameraKit = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_1400
 * @tc.name      : Get FrameConfig size
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaCameraTest, TestCapture_frameConfig_getsize, Function | MediumTest | Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> camList;
    string camId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, camList, camId);
    SampleCameraStateMng camStateMng(eventHdlr);
    sleep(3);
    cameraKit->CreateCamera(camId, camStateMng, eventHdlr);
    sleep(3);
    FrameConfig *fc = new FrameConfig(FRAME_CONFIG_CAPTURE);
    Surface *surface = Surface::CreateSurface();
    if (surface == nullptr) {
        delete fc;
        cout << "CreateSurface failed" << endl;
    }
    surface->SetWidthAndHeight(1920, 1080); /* 1920:width,1080:height */
    fc->AddSurface(*surface);
    EXPECT_NE(0, fc->GetSurfaces().size());
    delete surface;
    delete fc;
    cameraKit = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_1500
 * @tc.name      : Test Get Frame
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaCameraTest, TestOnFrameProgressed, Function | MediumTest | Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> camList;
    string camId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, camList, camId);
    SampleCameraStateMng camStateMng(eventHdlr);
    sleep(3);
    cameraKit->CreateCamera(camId, camStateMng, eventHdlr);
    sleep(3);
    camStateMng.Capture();
    sleep(1);
    EXPECT_EQ(CameraFlag::g_onFrameStartedFlag, FLAG1);
    EXPECT_EQ(CameraFlag::g_onFrameProgressedFlag, FLAG1);
    cameraKit = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_1600
 * @tc.name      : Get FrameConfig onConfig
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaCameraTest, TestGetFrameConfigureType, Function | MediumTest | Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> camList;
    string camId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, camList, camId);
    SampleCameraStateMng camStateMng(eventHdlr);
    sleep(3);
    cameraKit->CreateCamera(camId, camStateMng, eventHdlr);
    sleep(3);
    camStateMng.Capture();
    sleep(1);
    EXPECT_EQ(CameraFlag::g_onGetFrameConfigureType, FLAG1);
    cameraKit = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_1700
 * @tc.name      : Test Get Frame finished
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaCameraTest, TestFrameCompletedFlag, Function | MediumTest | Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> camList;
    string camId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, camList, camId);
    SampleCameraStateMng camStateMng(eventHdlr);
    sleep(3);
    cameraKit->CreateCamera(camId, camStateMng, eventHdlr);
    sleep(3);
    camStateMng.Capture();
    sleep(1);
    EXPECT_EQ(CameraFlag::g_onFrameFinishedFlag, FLAG1);
    cameraKit = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_1800
 * @tc.name      : Test GetFrame Error
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaCameraTest, TestonFrameErrorFlag, Function | MediumTest | Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> camList;
    string camId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, camList, camId);
    SampleCameraStateMng camStateMng(eventHdlr);
    sleep(3);
    cameraKit->CreateCamera(camId, camStateMng, eventHdlr);
    sleep(3);
    camStateMng.Capture();
    sleep(1);
    EXPECT_NE(CameraFlag::g_onFrameErrorFlag, FLAG1);
    cameraKit = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_1900
 * @tc.name      : Test Capture Success
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaCameraTest, TestCapture01, Function | MediumTest | Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> camList;
    string camId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, camList, camId);
    SampleCameraStateMng camStateMng(eventHdlr);
    sleep(3);
    cameraKit->CreateCamera(camId, camStateMng, eventHdlr);
    sleep(3);
    camStateMng.Capture();
    sleep(1);
    EXPECT_EQ(CameraFlag::g_onCaptureTriggerStartedFlag, FLAG1);
    EXPECT_EQ(CameraFlag::g_onCaptureTriggerCompletedFlag, FLAG1);
    cameraKit = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_2000
 * @tc.name      : Test capture and record
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaCameraTest, TestRecord01, Function | MediumTest | Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> camList;
    string camId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, camList, camId);
    SampleCameraStateMng camStateMng(eventHdlr);
    sleep(3);
    cameraKit->CreateCamera(camId, camStateMng, eventHdlr);
    sleep(3);
    EXPECT_EQ(CameraFlag::g_onCreatedFlag, FLAG1);
    EXPECT_EQ(CameraFlag::g_onConfigureFlag, FLAG1);
    camStateMng.Capture();
    sleep(3);
    camStateMng.StartRecord();
    sleep(3);
    camStateMng.Stop();
    cameraKit = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_2100
 * @tc.name      : Test get event handler
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaCameraTest, TestGetEventHandler, Function | MediumTest | Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> camList;
    string camId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, camList, camId);
    SampleCameraStateMng camStateMng(eventHdlr);
    sleep(3);
    cameraKit->CreateCamera(camId, camStateMng, eventHdlr);
    sleep(3);
    EXPECT_EQ(CameraFlag::g_onCreatedFlag, FLAG1);
    EXPECT_EQ(CameraFlag::g_onConfigureFlag, FLAG1);
    camStateMng.Capture();
    sleep(3);
    camStateMng.StartRecord();
    sleep(3);
    camStateMng.Stop();
    cameraKit = NULL;
}