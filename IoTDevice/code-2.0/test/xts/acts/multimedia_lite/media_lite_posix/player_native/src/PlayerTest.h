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
 
#ifndef TEST_XTS_ACTS_MULTIMEDIA_LITE_MULTIMEDIA_POSIX_PLAYER_SRC_PLAYERTEST_H
#define TEST_XTS_ACTS_MULTIMEDIA_LITE_MULTIMEDIA_POSIX_PLAYER_SRC_PLAYERTEST_H
#include <sys/prctl.h>
#include "unistd.h"
#include <fcntl.h>
#include "securec.h"
#include "gtest/gtest.h"
#include "source.h"
#include "player.h"
#include "format.h"
#include "fstream"
#include "iostream"
#include "thread"
#include <climits>
#endif

const int FRAME_RATE_DEFAULT = 30;
const int FILE_PATH_LEN = 2048;

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

namespace OHOS {
using OHOS::Media::Player;
using OHOS::Media::PlayerSeekMode;
using OHOS::Media::Source;
using OHOS::Media::Format;
using OHOS::Media::StreamSource;
using OHOS::Media::StreamCallback;
using OHOS::Media::SourceType;
using OHOS::Media::PlayerCallback;

using namespace std;
using namespace OHOS::Media;
using namespace testing::ext;

class StreamSourceSample;
using TestSample = struct TagTestSample {
    std::shared_ptr<Player> adaptr;
    pthread_t process;
    pthread_mutex_t mutex;
    int32_t isThreadRunning;
    int32_t sourceType;
    char filePath[FILE_PATH_LEN];
    std::shared_ptr<StreamSourceSample> streamSample;
};

using IdleBuffer =  struct TagIdleBuffer {
    size_t idx;
    size_t offset;
    size_t size;
};

class StreamSourceSample : public StreamSource {
public:
    StreamSourceSample(void);
    ~StreamSourceSample(void);
    void OnBufferAvailable(size_t index, size_t offset, size_t size);
    void SetStreamCallback(const std::shared_ptr<StreamCallback> &callback);
    uint8_t *GetBufferAddress(size_t idx);
    void QueueBuffer(size_t index, size_t offset, size_t size, int64_t timestampUs, uint32_t flags);
    int GetAvailableBuffer(IdleBuffer *buffer);
    std::weak_ptr<StreamCallback> m_callBack;
    pthread_mutex_t m_mutex;
private:
    std::vector<IdleBuffer> aviableBuffer;
};

void *StreamProcess(const void *arg);
void SetSchParam(void);
} // namespace OHOS

using namespace OHOS;
const int HI_SUCCESS = 0;
const int HI_FAILURE = -1;
static TagTestSample g_tagTestSample;
static Surface *g_surface = Surface::CreateSurface();
