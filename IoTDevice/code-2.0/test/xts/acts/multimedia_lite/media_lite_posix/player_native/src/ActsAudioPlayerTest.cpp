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
 
#include "unistd.h"
#include <sys/prctl.h>
#include <fcntl.h>
#include "securec.h"
#include "gtest/gtest.h"
#include "source.h"
#include "player.h"
#include "format.h"
#include "PlayerTest.h"
#include "fstream"
#include "iostream"
#include "thread"
#include <climits>

#define DOFUNC_STR_NORET(func, str)                                                              \
    do {                                                                                         \
        HI_S32 s32Ret = 0;                                                                       \
        s32Ret = func;                                                                           \
        if (s32Ret != HI_SUCCESS) {                                                              \
            printf("[liteplayer_sample][%s:%d] ret:%d, %s \n", __FILE__, __LINE__, s32Ret, str); \
            return NULL;                                                                         \
        }                                                                                        \
    } while (0)

#define DOFUNC_STR_RET(func, str)                                                                \
    do {                                                                                         \
        HI_S32 s32Ret = 0;                                                                       \
        s32Ret = func;                                                                           \
        if (s32Ret != HI_SUCCESS) {                                                              \
            printf("[liteplayer_sample][%s:%d] ret:%d, %s \n", __FILE__, __LINE__, s32Ret, str); \
            return HI_FAILURE;                                                                   \
        }                                                                                        \
    } while (0)

#define IS_OK(ret)                                                \
    do {                                                          \
        if (ret != 0) {                                           \
            printf("[%s: %d] ret:%d\n", __func__, __LINE__, ret); \
        }                                                         \
    } while (0)

using namespace std;
using namespace OHOS::Media;
using namespace testing::ext;

namespace OHOS {
using OHOS::Media::Player;
using OHOS::Media::PlayerSeekMode;
using OHOS::Media::Source;
using OHOS::Media::Format;
using OHOS::Media::StreamSource;
using OHOS::Media::StreamCallback;

StreamSourceSample::StreamSourceSample(void)
{
    aviableBuffer.clear();
    pthread_mutex_init(&m_mutex, nullptr);
}

StreamSourceSample::~StreamSourceSample(void)
{
    aviableBuffer.clear();
    pthread_mutex_destroy(&m_mutex);
}

void StreamSourceSample::SetStreamCallback(const std::shared_ptr<StreamCallback> &callback) 
{
    m_callBack = callback;
}

uint8_t *StreamSourceSample::GetBufferAddress(size_t idx)
{
    std::shared_ptr<StreamCallback> callback = m_callBack.lock();
    if (callback == nullptr) {
        return nullptr;
    }
    return callback->GetBuffer(idx);
}

void StreamSourceSample::QueueBuffer(size_t index, size_t offset, size_t size, int64_t timestampUs, uint32_t flags)
{
    std::shared_ptr<StreamCallback> callback = m_callBack.lock();
    if (callback == nullptr) {
        return;
    }
    callback->QueueBuffer(index, offset, size, timestampUs, flags);
}

void StreamSourceSample::OnBufferAvailable(size_t index, size_t offset, size_t size)
{
    IdleBuffer buffer;
    pthread_mutex_lock(&m_mutex);
    buffer.idx = index;
    buffer.offset = offset;
    buffer.size = size;
    aviableBuffer.push_back(buffer);
    pthread_mutex_unlock(&m_mutex);
}

int StreamSourceSample::GetAvailableBuffer(IdleBuffer *buffer)
{
    pthread_mutex_lock(&m_mutex);
    if (aviableBuffer.empty()) {
        pthread_mutex_unlock(&m_mutex);
        return -1;
    }
    *buffer = aviableBuffer[0];
    aviableBuffer.erase(aviableBuffer.begin());
    pthread_mutex_unlock(&m_mutex);
    return 0;
}

void *StreamProcess(void *arg)
{
    const int gReadLen = 1024;
    const int usleepTime = 20000;
    const int flags1 = 8;
    const int flags2 = 4;
    IdleBuffer buffer;
    int ret;
    uint8_t *data = nullptr;
    size_t readLen;
    size_t len;
    TestSample *sample = (TestSample *)arg;
    FILE* pFile = fopen(sample->filePath, "rb");
    if (pFile == nullptr) {
        return nullptr;
    }
    prctl(PR_SET_NAME, "StreamProc", 0, 0, 0);
    printf("[%s,%d] file:%s\n", __func__, __LINE__, sample->filePath);
    while (sample->isThreadRunning) {
        ret = sample->streamSample->GetAvailableBuffer(&buffer);
        if (ret != 0) {
            usleep(usleepTime);
            continue;
        }
        data = sample->streamSample->GetBufferAddress(buffer.idx);
        if (data == nullptr) {
            printf("[%s, %d] get buffer null", __func__, __LINE__);
            break;
        }
        len = (buffer.size < gReadLen) ? buffer.size : gReadLen;
        readLen = fread(data + buffer.offset, 1, len, pFile);
        if (readLen <= len && readLen > 0) {
            sample->streamSample->QueueBuffer(buffer.idx, buffer.offset, readLen, 0, flags1);
        } else {
            sample->streamSample->QueueBuffer(buffer.idx, buffer.offset, readLen, 0, flags2);
            break;
        }
    }
    fclose(pFile);
    printf("[%s,%d]\n", __func__, __LINE__);
    return nullptr;
}

void SetSchParam(void)
{
    struct sched_param param;
    const int priorityNum = 9;
    pthread_attr_t attr;
    pthread_attr_getschedparam(&attr, &param);
    param.sched_priority = priorityNum;
    pthread_setschedparam(pthread_self(), SCHED_RR, &param);
}
} // namespace OHOS

const char *g_audioFileName = "Audiochannel_001.aac";
const char *g_audioFileName1 = "Audiochannel_002.m4a";

class ActsAudioPlayerTest : public testing::Test {
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

static int32_t FileCheck1(const char* argv)
{
    if (realpath(argv, g_tagTestSample.filePath) == nullptr) {
        printf("realpath input file failed, errno: %d!\n", errno);
        return -1;
    }
    return 0;
}

static int32_t CreateAndSetAudioSource1()
{
    std::string uri(g_tagTestSample.filePath);
    std::map<std::string, std::string> header;
    Source source(uri, header);
    int32_t ret = g_tagTestSample.adaptr->SetSource(source);
    return ret;
}

static int32_t FileCheck2(const char* argv)
{
    if (realpath(argv, g_tagTestSample.filePath) == nullptr) {
        printf("realpath input file failed, errno: %d!\n", errno);
        return -1;
    }
    struct stat stFileState = {0};
    if (lstat(g_tagTestSample.filePath, &stFileState) != 0) {
        printf("lstat %s failed, please check the file exist, errno:%d\n", g_tagTestSample.filePath, errno);
        return -1;
    }
    g_tagTestSample.streamSample = std::make_shared<StreamSourceSample>();
    return 0;
}

static int32_t CreateAndSetAudioSource2()
{
    Format formats;
    formats.PutStringValue(CODEC_MIME, MIME_AUDIO_AAC);
    std::string value;
    bool flag = formats.GetStringValue(CODEC_MIME, value);
    EXPECT_EQ(true, flag);
    Source source(g_tagTestSample.streamSample, formats);
    int32_t ret1 = g_tagTestSample.adaptr->SetSource(source); 
    EXPECT_EQ(HI_SUCCESS, ret1);
    flag = formats.CopyFrom(formats);
    EXPECT_EQ(true, flag);
    std::shared_ptr<StreamSource>  ret2 = source.GetSourceStream();
    std::map<std::string, FormatData *>  ret3 = formats.GetFormatMap();
    pthread_mutex_init(&g_tagTestSample.mutex, nullptr);
    g_tagTestSample.isThreadRunning = 1;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 0x10000);
    int32_t ret = pthread_create(&g_tagTestSample.process, &attr, StreamProcess, &g_tagTestSample);
    if (ret != 0) {
        printf("pthread_create failed %d\n", ret);
        g_tagTestSample.isThreadRunning = 0;
        return -1;
    }
    return 0;
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_DIFFERENT_FORMAT_0100
 * @tc.name      : Set Audio Source 01 
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsAudioPlayerTest, Test_SetSource01, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck2(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetAudioSource2();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    sleep(5);
    EXPECT_EQ(HI_SUCCESS, ret);
    bool flag = g_tagTestSample.adaptr->IsPlaying();  
    EXPECT_EQ(true, flag);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_DIFFERENT_FORMAT_0200
 * @tc.name      : Set Audio Source 02 
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsAudioPlayerTest, Test_SetSource02, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck2(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetAudioSource2();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    sleep(5);
    EXPECT_EQ(HI_SUCCESS, ret);
    bool flag = g_tagTestSample.adaptr->IsPlaying();  
    EXPECT_EQ(true, flag);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_0100
 * @tc.name      : Audio Prepare 01
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsAudioPlayerTest, Test_Prepare01, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck2(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetAudioSource2();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    bool flag = g_tagTestSample.adaptr->IsPlaying();
    EXPECT_EQ(false, flag);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_0200
 * @tc.name      : Audio Prepare 02
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsAudioPlayerTest, Test_Prepare02, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck2(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetAudioSource2();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_FAILURE, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_0300
 * @tc.name      : Audio Play 01
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsAudioPlayerTest, Test_Play01, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck2(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetAudioSource2();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_FAILURE, ret);
    ret = g_tagTestSample.adaptr->Play();
    sleep(8);
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_0400
 * @tc.name      : Audio Play 02
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsAudioPlayerTest, Test_Play02, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck2(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetAudioSource2();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    sleep(8);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_0500
 * @tc.name      : Audio Stop 01
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsAudioPlayerTest, Test_Stop01, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck2(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetAudioSource2();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(5);
    ret = g_tagTestSample.adaptr->Stop();
    EXPECT_EQ(HI_SUCCESS, ret);
    bool flag = g_tagTestSample.adaptr->IsPlaying();
    EXPECT_EQ(false, flag);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_0600
 * @tc.name      : Audio Stop 02
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsAudioPlayerTest, Test_Stop02, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck2(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetAudioSource2();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(5);
    ret = g_tagTestSample.adaptr->Stop();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Stop();
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_0700
 * @tc.name      : Audio Stop 03
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsAudioPlayerTest, Test_Stop03, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck2(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetAudioSource2();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(5);
    ret = g_tagTestSample.adaptr->Pause();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Stop();
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_0800
 * @tc.name      : Audio Stop 04
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsAudioPlayerTest, Test_Stop04, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck2(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetAudioSource2();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(5);
    ret = g_tagTestSample.adaptr->Pause();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(5);
    ret = g_tagTestSample.adaptr->Stop();
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_0900
 * @tc.name      : Audio Stop 05
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsAudioPlayerTest, Test_Stop05, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck2(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetAudioSource2();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Stop();
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_1000
 * @tc.name      : Audio Pause 01
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsAudioPlayerTest, Test_Pause01, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck2(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetAudioSource2();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(5);
    ret = g_tagTestSample.adaptr->Pause();
    EXPECT_EQ(HI_SUCCESS, ret);
    bool flag = g_tagTestSample.adaptr->IsPlaying();
    EXPECT_EQ(false, flag);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_1100
 * @tc.name      : Audio Pause 02
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsAudioPlayerTest, Test_Pause02, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck2(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetAudioSource2();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(5);
    ret = g_tagTestSample.adaptr->Pause();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Pause();
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_1200
 * @tc.name      : Audio Set Volume 01
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsAudioPlayerTest, Test_SetVolume01, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck2(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetAudioSource2();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVolume(20, 20);
    sleep(10);
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_1300
 * @tc.name      : Audio Set Volume 02
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsAudioPlayerTest, Test_SetVolume02, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck2(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetAudioSource2();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVolume(300, 300);
    sleep(10);
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_1400
 * @tc.name      : Audio Set Volume 03
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsAudioPlayerTest, Test_SetVolume03, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck2(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetAudioSource2();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVolume(0, 301);
    sleep(10);
    EXPECT_EQ(HI_FAILURE, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_1500
 * @tc.name      : Audio Set Volume 04
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsAudioPlayerTest, Test_SetVolume04, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck2(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetAudioSource2();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVolume(5, 5);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    sleep(10);
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_1600
 * @tc.name      : Audio Set Volume 05
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsAudioPlayerTest, Test_SetVolume05, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck2(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetAudioSource2();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVolume(-5, 5);
    EXPECT_EQ(HI_FAILURE, ret);
    ret = g_tagTestSample.adaptr->Play();
    sleep(10);
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_1700
 * @tc.name      : Get Current Time of Audio 01
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsAudioPlayerTest, Test_GetCurrentTime01, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck1(g_audioFileName1);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetAudioSource1();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(3);
    int64_t currentPosition;
    ret = g_tagTestSample.adaptr->GetCurrentTime(currentPosition);
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_1800
 * @tc.name      : Get Current Time of Audio 02
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsAudioPlayerTest, Test_GetCurrentTime02, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck1(g_audioFileName1);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetAudioSource1();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(2);
    ret = g_tagTestSample.adaptr->Rewind(4, PLAYER_SEEK_NEXT_SYNC);
    EXPECT_EQ(HI_SUCCESS, ret);
    int64_t currentPosition;
    ret = g_tagTestSample.adaptr->GetCurrentTime(currentPosition);
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_1900
 * @tc.name      : Get Current Time of Audio 03
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsAudioPlayerTest, Test_GetCurrentTime03, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck1(g_audioFileName1);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetAudioSource1();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(2);
    ret = g_tagTestSample.adaptr->Pause();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Rewind(4, PLAYER_SEEK_NEXT_SYNC);
    EXPECT_EQ(HI_SUCCESS, ret);
    int64_t currentPosition;
    ret = g_tagTestSample.adaptr->GetCurrentTime(currentPosition);
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_2000
 * @tc.name      : Get Current Time of Audio 04
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsAudioPlayerTest, Test_GetCurrentTime04, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck1(g_audioFileName1);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetAudioSource1();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    int64_t currentPosition;
    ret = g_tagTestSample.adaptr->GetCurrentTime(currentPosition);
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_2100
 * @tc.name      : Judging Audio Single Loop Play 01
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsAudioPlayerTest, Test_IsSingleLooping01, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck2(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetAudioSource2();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(15);
    bool flag = g_tagTestSample.adaptr->IsSingleLooping();
    EXPECT_EQ(false, flag);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_2200
 * @tc.name      : Judging Audio Single Loop Play 02
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsAudioPlayerTest, Test_IsSingleLooping02, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck2(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetAudioSource2();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    sleep(15);
    EXPECT_EQ(HI_SUCCESS, ret);
    bool loop = true;
    ret = g_tagTestSample.adaptr->EnableSingleLooping(loop);
    EXPECT_EQ(HI_SUCCESS, ret);
    bool flag = g_tagTestSample.adaptr->IsSingleLooping();
    EXPECT_EQ(true, flag);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_2300
 * @tc.name      : Judging Audio Single Loop Play 03
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsAudioPlayerTest, Test_IsSingleLooping03, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck2(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetAudioSource2();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    bool loop = true;
    ret = g_tagTestSample.adaptr->EnableSingleLooping(loop);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(15);
    bool flag = g_tagTestSample.adaptr->IsSingleLooping();
    EXPECT_EQ(true, flag);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_2400
 * @tc.name      : Get Duration of Audio 01
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsAudioPlayerTest, Test_GetDuration01, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck1(g_audioFileName1);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetAudioSource1();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(5);
    int64_t duration;
    ret = g_tagTestSample.adaptr->GetDuration(duration);
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_2500
 * @tc.name      : Get Duration of Audio 02
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsAudioPlayerTest, Test_GetDuration02, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck1(g_audioFileName1);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetAudioSource1();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    int64_t duration;
    ret = g_tagTestSample.adaptr->GetDuration(duration);
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_2600
 * @tc.name      : Audio Reset 01
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsAudioPlayerTest, Test_Reset01, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck2(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetAudioSource2();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(5);
    ret = g_tagTestSample.adaptr->Reset();
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_2700
 * @tc.name      : Audio Reset 02
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsAudioPlayerTest, Test_Reset02, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck2(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetAudioSource2();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Reset();
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_2800
 * @tc.name      : Audio Release 01
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsAudioPlayerTest, Test_Release01, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck2(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetAudioSource2();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Release();
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_2900
 * @tc.name      : Audio Release 02
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsAudioPlayerTest, Test_Release02, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck2(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetAudioSource2();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(5);
    ret = g_tagTestSample.adaptr->Release();
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_DIFFERENT_SCENE_0100
 * @tc.name      : Audio Play 01
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsAudioPlayerTest, Test_Play03, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck2(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetAudioSource2();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    sleep(10);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Stop();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    sleep(10);
    EXPECT_EQ(HI_FAILURE, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_DIFFERENT_SCENE_0200
 * @tc.name      : Audio Play 02
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsAudioPlayerTest, Test_Play04, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck2(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetAudioSource2();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    sleep(10);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Pause();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Pause();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    sleep(10);
    EXPECT_EQ(HI_SUCCESS, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_DIFFERENT_SCENE_0300
 * @tc.name      : Audio Pause 01
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsAudioPlayerTest, Test_Pause03, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck2(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetAudioSource2();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Pause();
    EXPECT_EQ(HI_FAILURE, ret);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_DIFFERENT_SCENE_0400
 * @tc.name      : Audio Prepare 01
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : FUNC
 * @tc.level     : Level 0
 */
HWTEST_F(ActsAudioPlayerTest, Test_Prepare03, Function | MediumTest | Level0)
{
    int32_t ret=FileCheck2(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetAudioSource2();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Reset();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_FAILURE, ret);
}