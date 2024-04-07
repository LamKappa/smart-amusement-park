/* Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "PlayerTest.h"

const char *g_videoFileName = "1080P_25fps.mp4";

static void InitSurface()
{
    g_surface->SetUserData("region_position_x", "0");
    g_surface->SetUserData("region_position_y", "0");
    g_surface->SetUserData("region_width", "720");
    g_surface->SetUserData("region_height", "540");
}

class ActsVideoPlayerTest : public testing::Test {
protected:
// SetUpTestCase:The preset action of the test suite is executed before the first TestCase
    static void SetUpTestCase(void) 
    {
    }
// TearDownTestCase:The test suite cleanup action is executed after the last TestCase
    static void TearDownTestCase(void) 
    {
    }
// SetUp:Execute before each test case
    virtual void SetUp()
    {
        SetSchParam();
        g_tagTestSample.adaptr = std::make_shared<Player>();
        InitSurface();
    }
// TearDown:Execute after each test case
    virtual void TearDown()
    {
        g_tagTestSample.adaptr->Reset();
        g_tagTestSample.adaptr->Release();
        const int audioType = 2;
        if (g_tagTestSample.sourceType == audioType) {
            pthread_mutex_lock(&g_tagTestSample.mutex);
            g_tagTestSample.isThreadRunning = 0;
            pthread_mutex_unlock(&g_tagTestSample.mutex);
            pthread_join(g_tagTestSample.process, nullptr);
            pthread_mutex_destroy(&g_tagTestSample.mutex);
        }
    }
};

static int32_t FileCheck(const char* argv)
{
    if (realpath(argv, g_tagTestSample.filePath) == nullptr) {
        printf("realpath input file failed, errno: %d!\n", errno);
        return -1;
    }
    return 0;
}

static int32_t CreateAndSetVideoSource()
{
    std::string uri(g_tagTestSample.filePath);
    Source source(uri);
    int32_t ret = g_tagTestSample.adaptr->SetSource(source);
    string ret1 = source.GetSourceUri();
    source.GetSourceType();
    return ret;
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_DIFFERENT_FORMAT_0300
 * @tc.name      : Set Video Source 01
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsVideoPlayerTest, Test_SetSource03, Function | MediumTest | Level0)
{
    int32_t ret = FileCheck(g_videoFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    std::string uri(g_tagTestSample.filePath);
    Source source(uri);
    ret = g_tagTestSample.adaptr->SetSource(source);
    string ret1 = source.GetSourceUri();
    source.GetSourceType();
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    sleep(1);
    EXPECT_EQ(HI_SUCCESS, ret);
    bool flag = g_tagTestSample.adaptr->IsPlaying();
    EXPECT_EQ(true, flag);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_DIFFERENT_FORMAT_0400
 * @tc.name      : Set Video Source 02
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsVideoPlayerTest, Test_SetSource04, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck(g_videoFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    Format formats;
    formats.PutIntValue("frame-rate", 30);
    int32_t value1;
    bool flag = formats.GetIntValue("frame-rate", value1);
    EXPECT_EQ(true, flag);
    formats.PutLongValue("bitrate", 39200);
    long long value2;
    flag = formats.GetLongValue("bitrate", value2);
    EXPECT_EQ(true, flag);
    formats.PutFloatValue("color-format", 2413.5f);
    float value3;
    flag = formats.GetFloatValue("color-format", value3);
    EXPECT_EQ(true, flag);
    formats.PutDoubleValue("color-format", 9930708);
    double value4;
    flag = formats.GetDoubleValue("color-format", value4);
    EXPECT_EQ(true, flag);
    ret = CreateAndSetVideoSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(10);
    ret = g_tagTestSample.adaptr->Reset();
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_DIFFERENT_FORMAT_0500
 * @tc.name      : Set Video Source 03
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsVideoPlayerTest, Test_SetSource05, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck(g_videoFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    Format formats;
    std::map<std::string, FormatData *>  ret3 = formats.GetFormatMap();
    FormatData Formats1(FORMAT_TYPE_INT32);
    Formats1.SetValue(23);
    int32_t vax1;
    bool flag = Formats1.GetInt32Value(vax1);
    EXPECT_EQ(true, flag);
    cout<<"+++"<<vax1<<"==="<<endl;
    FormatData Formats2(FORMAT_TYPE_FLOAT);
    Formats2.SetValue(25.5f);
    float vax2;
    flag = Formats2.GetFloatValue(vax2);
    EXPECT_EQ(true, flag);
    cout<<"+++"<<"==="<<vax2<<"---"<<endl;
    FormatData Formats3(FORMAT_TYPE_STRING);
    Formats3.SetValue("color-format");
    string vax3;
    flag = Formats3.GetStringValue(vax3);
    EXPECT_EQ(true, flag);
    cout<<"+++"<<"==="<<"---"<<vax3<<endl;
    FormatData Formats4(FORMAT_TYPE_DOUBLE);
    Formats4.SetValue(20.6);
    double vax4;
    flag = Formats4.GetDoubleValue(vax4);
    EXPECT_EQ(true, flag);
    cout<<"+++"<<"==="<<vax4<<"---"<<endl;
    ret = CreateAndSetVideoSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(10);
    ret = g_tagTestSample.adaptr->Reset();
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_3000
 * @tc.name      : Video Prepare 01
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsVideoPlayerTest, Test_Prepare04, Function | MediumTest | Level0)
{
    int32_t ret = FileCheck(g_videoFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetVideoSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    bool flag = g_tagTestSample.adaptr->IsPlaying();
    EXPECT_EQ(false, flag);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_3100
 * @tc.name      : Video Prepare 02
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsVideoPlayerTest, Test_Prepare05, Function | MediumTest | Level0)
{
    int32_t ret = FileCheck(g_videoFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetVideoSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_FAILURE, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_3200
 * @tc.name      : Video Play 01
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsVideoPlayerTest, Test_Play05, Function | MediumTest | Level0)
{
    int32_t ret = FileCheck(g_videoFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetVideoSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_FAILURE, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    sleep(2);
    EXPECT_EQ(HI_SUCCESS, ret);
    bool flag = g_tagTestSample.adaptr->IsPlaying();
    EXPECT_EQ(true, flag);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_3300
 * @tc.name      : Video Play 02
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsVideoPlayerTest, Test_Play06, Function | MediumTest | Level0)
{
    int32_t ret = FileCheck(g_videoFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetVideoSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    sleep(1);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    sleep(1);
    EXPECT_EQ(HI_SUCCESS, ret);
    bool flag = g_tagTestSample.adaptr->IsPlaying();
    EXPECT_EQ(true, flag);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_3400
 * @tc.name      : Video Stop 01
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsVideoPlayerTest, Test_Stop06, Function | MediumTest | Level0)
{
    int32_t ret = FileCheck(g_videoFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetVideoSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    std::shared_ptr<PlayerCallback> cb;
    g_tagTestSample.adaptr->SetPlayerCallback(cb);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(2);
    ret = g_tagTestSample.adaptr->Stop();
    EXPECT_EQ(HI_SUCCESS, ret);
    bool flag = g_tagTestSample.adaptr->IsPlaying();
    EXPECT_EQ(false, flag);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_3500
 * @tc.name      : Video Stop 02
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsVideoPlayerTest, Test_Stop07, Function | MediumTest | Level0)
{
    int32_t ret = FileCheck(g_videoFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetVideoSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(2);
    ret = g_tagTestSample.adaptr->Stop();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Stop();
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_3600
 * @tc.name      : Video Stop 03
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsVideoPlayerTest, Test_Stop08, Function | MediumTest | Level0)
{
    int32_t ret = FileCheck(g_videoFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetVideoSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Stop();
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_3700
 * @tc.name      : Video Stop 04
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsVideoPlayerTest, Test_Stop09, Function | MediumTest | Level0)
{
    int32_t ret = FileCheck(g_videoFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetVideoSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(10);
    ret = g_tagTestSample.adaptr->Pause();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Stop();
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_3800
 * @tc.name      : Video Stop 05
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsVideoPlayerTest, Test_Stop10, Function | MediumTest | Level0)
{
    int32_t ret = FileCheck(g_videoFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetVideoSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(1);
    ret = g_tagTestSample.adaptr->Pause();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(2);
    ret = g_tagTestSample.adaptr->Stop();
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_3900
 * @tc.name      : Video Pause 01
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsVideoPlayerTest, Test_Pause04, Function | MediumTest | Level0)
{
    int32_t ret = FileCheck(g_videoFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetVideoSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(2);
    ret = g_tagTestSample.adaptr->Pause();
    EXPECT_EQ(HI_SUCCESS, ret);
    bool flag = g_tagTestSample.adaptr->IsPlaying();
    EXPECT_EQ(false, flag);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_4000
 * @tc.name      : Video Pause 02
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsVideoPlayerTest, Test_Pause05, Function | MediumTest | Level0)
{
    int32_t ret = FileCheck(g_videoFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetVideoSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(2);
    ret = g_tagTestSample.adaptr->Pause();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Pause();
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_4100
 * @tc.name      : Video Set Volume 01
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsVideoPlayerTest, Test_SetVolume06, Function | MediumTest | Level0)
{
    int32_t ret = FileCheck(g_videoFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetVideoSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVolume(30, 30);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(2);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_4200
 * @tc.name      : Video Set Volume 02
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsVideoPlayerTest, Test_SetVolume07, Function | MediumTest | Level0)
{
    int32_t ret = FileCheck(g_videoFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetVideoSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVolume(300, 300);
    sleep(2);
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_4300
 * @tc.name      : Video Set Volume 03
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsVideoPlayerTest, Test_SetVolume08, Function | MediumTest | Level0)
{
    int32_t ret = FileCheck(g_videoFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetVideoSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVolume(0, 0);
    sleep(2);
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_4400
 * @tc.name      : Judging Video Single Loop Play 01
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsVideoPlayerTest, Test_IsSingleLooping04, Function | MediumTest | Level0)
{
    int32_t ret = FileCheck(g_videoFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetVideoSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(10);
    bool flag = g_tagTestSample.adaptr->IsSingleLooping();
    EXPECT_EQ(false, flag);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_4500
 * @tc.name      : Judging Video Single Loop Play 02
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsVideoPlayerTest, Test_IsSingleLooping05, Function | MediumTest | Level0)
{
    int32_t ret = FileCheck(g_videoFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetVideoSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    sleep(10);
    EXPECT_EQ(HI_SUCCESS, ret);
    bool loop = true;
    ret = g_tagTestSample.adaptr->EnableSingleLooping(loop);
    EXPECT_EQ(HI_SUCCESS, ret);
    bool flag = g_tagTestSample.adaptr->IsSingleLooping();
    EXPECT_EQ(true, flag);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_4600
 * @tc.name      : Judging Video Single Loop Play 03
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsVideoPlayerTest, Test_IsSingleLooping06, Function | MediumTest | Level0)
{
    int32_t ret = FileCheck(g_videoFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetVideoSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    bool loop = true;
    ret = g_tagTestSample.adaptr->EnableSingleLooping(loop);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(10);
    bool flag = g_tagTestSample.adaptr->IsSingleLooping();
    EXPECT_EQ(true, flag);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_4700
 * @tc.name      : Get Current Time of Video 01
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsVideoPlayerTest, Test_GetCurrentTime05, Function | MediumTest | Level0)
{
    int32_t ret = FileCheck(g_videoFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetVideoSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(1);
    int64_t currentPosition;
    ret = g_tagTestSample.adaptr->GetCurrentTime(currentPosition);
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_4800
 * @tc.name      : Get Current Time of Video 02
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsVideoPlayerTest, Test_GetCurrentTime06, Function | MediumTest | Level0)
{
    int32_t ret = FileCheck(g_videoFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetVideoSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(2);
    ret = g_tagTestSample.adaptr->Rewind(1, PLAYER_SEEK_NEXT_SYNC);
    EXPECT_EQ(HI_SUCCESS, ret);
    int64_t currentPosition;
    ret = g_tagTestSample.adaptr->GetCurrentTime(currentPosition);
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_4900
 * @tc.name      : Get Duration of Video 01
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsVideoPlayerTest, Test_GetDuration03, Function | MediumTest | Level0)
{
    int32_t ret = FileCheck(g_videoFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetVideoSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(2);
    ret = g_tagTestSample.adaptr->Pause();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Rewind(1, PLAYER_SEEK_NEXT_SYNC);
    EXPECT_EQ(HI_SUCCESS, ret);
    int64_t currentPosition;
    ret = g_tagTestSample.adaptr->GetCurrentTime(currentPosition);
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_5000
 * @tc.name      : Get Duration of Video 02
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsVideoPlayerTest, Test_GetDuration04, Function | MediumTest | Level0)
{
    int32_t ret = FileCheck(g_videoFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetVideoSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(2);
    int64_t duration;
    ret = g_tagTestSample.adaptr->GetDuration(duration);
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_5100
 * @tc.name      : Get Duration of Video 03
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsVideoPlayerTest, Test_GetDuration05, Function | MediumTest | Level0)
{
    int32_t ret = FileCheck(g_videoFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetVideoSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    int64_t duration;
    ret = g_tagTestSample.adaptr->GetDuration(duration);
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_5200
 * @tc.name      : Get Video Surface Size 01
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsVideoPlayerTest, Test_GetVideoSurfaceSize01, Function | MediumTest | Level0)
{
    int32_t ret = FileCheck(g_videoFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetVideoSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    int32_t videoWidth;
    ret = g_tagTestSample.adaptr->GetVideoWidth(videoWidth);
    EXPECT_EQ(HI_SUCCESS, ret);
    int32_t videoHeight;
    ret = g_tagTestSample.adaptr->GetVideoHeight(videoHeight);
    EXPECT_EQ(HI_SUCCESS, ret);
}


/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_5300
 * @tc.name      : Video Reset 01
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsVideoPlayerTest, Test_Reset03, Function | MediumTest | Level0)
{
    int32_t ret = FileCheck(g_videoFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetVideoSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(2);
    ret = g_tagTestSample.adaptr->Reset();
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_5400
 * @tc.name      : Video Reset 02
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsVideoPlayerTest, Test_Reset04, Function | MediumTest | Level0)
{
    int32_t ret = FileCheck(g_videoFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetVideoSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Reset();
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_5500
 * @tc.name      : Video Release 01
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsVideoPlayerTest, Test_Release03, Function | MediumTest | Level0)
{
    int32_t ret = FileCheck(g_videoFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetVideoSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Release();
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_5600
 * @tc.name      : Video Release 02
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsVideoPlayerTest, Test_Release04, Function | MediumTest | Level0)
{
    int32_t ret = FileCheck(g_videoFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetVideoSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(2);
    ret = g_tagTestSample.adaptr->Release();
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_DIFFERENT_SCENE_0500
 * @tc.name      : Video Play 01
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsVideoPlayerTest, Test_Play07, Function | MediumTest | Level0)
{
    int32_t ret = FileCheck(g_videoFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetVideoSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    sleep(2);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Stop();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_FAILURE, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_DIFFERENT_SCENE_0600
 * @tc.name      : Video Play 02
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsVideoPlayerTest, Test_Play08, Function | MediumTest | Level0)
{
    int32_t ret = FileCheck(g_videoFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetVideoSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    sleep(2);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Pause();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Pause();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    sleep(2);
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_DIFFERENT_SCENE_0700
 * @tc.name      : Video Pause 01
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsVideoPlayerTest, Test_Pause06, Function | MediumTest | Level0)
{
    int32_t ret = FileCheck(g_videoFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetVideoSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Pause();
    EXPECT_EQ(HI_FAILURE, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_DIFFERENT_SCENE_0800
 * @tc.name      : Video Prepare 01
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsVideoPlayerTest, Test_Prepare06, Function | MediumTest | Level0)
{
    int32_t ret = FileCheck(g_videoFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetVideoSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Reset();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_FAILURE, ret);
}