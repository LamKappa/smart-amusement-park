/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include <climits>
#include <gtest/gtest.h>

#include "buffer_common.h"
#include "surface.h"
#include "surface_impl.h"

using namespace std;
using namespace testing::ext;

namespace OHOS {
class SurfaceTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

class BufferConsumerTest : public IBufferConsumerListener {
public:
    void OnBufferAvailable();
    ~BufferConsumerTest() {}
};
void BufferConsumerTest::OnBufferAvailable()
{
}

void SurfaceTest::SetUpTestCase(void)
{
}

void SurfaceTest::TearDownTestCase(void)
{
}

void SurfaceTest::SetUp(void)
{
}

void SurfaceTest::TearDown(void)
{
}

/*
 * Feature: Surface
 * Function: new SurfaceBuffer
 * SubFunction: NA
 * FunctionPoints: Surface Buffer initilization.
 * EnvConditions: NA
 * CaseDescription: Verify the Surface Buffer initilization.
 */
HWTEST_F(SurfaceTest, surface_buffer_001, TestSize.Level1)
{
    SurfaceBufferImpl* buffer = new SurfaceBufferImpl();
    EXPECT_TRUE(buffer);
    EXPECT_EQ(0, buffer->GetKey());
    EXPECT_EQ(0, buffer->GetShmid());
    EXPECT_EQ(0, buffer->GetPhyAddr());
    EXPECT_EQ(0, buffer->GetSize());
    EXPECT_EQ(0, buffer->GetUsage());
    EXPECT_EQ(0, buffer->GetDeletePending());
    EXPECT_EQ(0, buffer->GetState());

    int32_t aValue32;
    int32_t ret = buffer->GetInt32(1, aValue32); // key = 1, test has value with key(1). if not, ret < 0.
    EXPECT_LT(ret, 0);
    int64_t aValue64;
    ret = buffer->GetInt64(1, aValue64); // key = 1, test has value with key(1). if not, ret < 0.
    EXPECT_LT(ret, 0);
    delete buffer;
}

/*
 * Feature: Surface
 * Function: new SurfaceBuffer(uint32_t size, uint32_t flag, uint32_t usage)
 * SubFunction: NA
 * FunctionPoints: Surface Buffer initilization.
 * EnvConditions: NA
 * CaseDescription: Verify the Surface Buffer initilization.
 */
HWTEST_F(SurfaceTest, surface_buffer_002, TestSize.Level1)
{
    int32_t usage = BUFFER_CONSUMER_USAGE_HARDWARE; // alloc buffer with usage = BUFFER_CONSUMER_USAGE_HARDWARE
    int32_t size = 1024; // alloc buffer with size = 1024;

    SurfaceBufferImpl* buffer = new SurfaceBufferImpl();
    buffer->SetUsage(usage);
    buffer->SetMaxSize(size);
    EXPECT_TRUE(buffer);
    EXPECT_EQ(0, buffer->GetKey());
    EXPECT_EQ(0, buffer->GetShmid());
    EXPECT_EQ(0, buffer->GetPhyAddr());
    EXPECT_EQ(size, buffer->GetMaxSize());
    EXPECT_EQ(usage, buffer->GetUsage());
    EXPECT_EQ(0, buffer->GetDeletePending());
    EXPECT_EQ(0, buffer->GetState());

    int32_t aValue32;
    int32_t ret = buffer->GetInt32(1, aValue32); // key = 1, test has value with key(1). if not, ret < 0.
    EXPECT_LT(ret, 0);
    int64_t aValue64;
    ret = buffer->GetInt64(1, aValue64); // key = 1, test has value with key(1). if not, ret < 0?
    EXPECT_LT(ret, 0);
    delete buffer;
}

/*
 * Feature: Surface
 * Function: Surface Buffer set/get key-value
 * SubFunction: NA
 * FunctionPoints: buffer attr and extra attr set/get.
 * EnvConditions: NA
 * CaseDescription: Verify the Surface Buffer attr set/get.
 */
HWTEST_F(SurfaceTest, surface_buffer_003, TestSize.Level1)
{
    SurfaceBufferImpl buffer;

    EXPECT_EQ(0, buffer.GetShmid());
    uint32_t shmid = 1;
    buffer.SetShmid(shmid);
    EXPECT_EQ(shmid, buffer.GetShmid());

    EXPECT_EQ(0, buffer.GetPhyAddr());
    uint64_t phyAddr = 0x040a7000; // mock physical address as 0x040a7000
    buffer.SetPhyAddr(phyAddr);
    EXPECT_EQ(phyAddr, buffer.GetPhyAddr());

    EXPECT_EQ(0, buffer.GetMaxSize());
    uint32_t size = 1;
    buffer.SetMaxSize(size);
    EXPECT_EQ(size, buffer.GetMaxSize());

    EXPECT_EQ(0, buffer.GetUsage());
    uint32_t usage = 1;
    buffer.SetUsage(usage);
    EXPECT_EQ(usage, buffer.GetUsage());

    EXPECT_EQ(0, buffer.GetDeletePending());
    uint8_t deletePending = 1;
    buffer.SetDeletePending(deletePending);
    EXPECT_EQ(deletePending, buffer.GetDeletePending());

    EXPECT_EQ(0, buffer.GetState());
    buffer.SetState(BUFFER_STATE_REQUEST);
    EXPECT_EQ(BUFFER_STATE_REQUEST, buffer.GetState());

    int32_t aValue32;
    int32_t ret = buffer.GetInt32(1, aValue32); // key = 1, test has value for key(1). if not, ret < 0.
    EXPECT_LT(ret, 0);

    int32_t key32 = 1; // set key-value , key = 1;
    int32_t value32 = 100; // set key-value, value = 1;
    buffer.SetInt32(key32, value32);
    ret = buffer.GetInt32(key32, aValue32);
    EXPECT_EQ(ret, 0);
    EXPECT_EQ(value32, aValue32);

    int64_t aValue64;
    ret = buffer.GetInt64(1, aValue64); // key = 1, test has value for key(1). if not, ret < 0?
    EXPECT_LT(ret, 0);

    uint32_t key64 = 2; // set key-value , key = 2;
    int64_t value64 = 0x040a7003; // set key-value, value = 0x040a7003;
    buffer.SetInt64(key64, value64);
    ret = buffer.GetInt64(key64, aValue64);
    EXPECT_EQ(ret, 0);
    EXPECT_EQ(value64, aValue64);

    value64 = 0x040a7004; // set key-value, value = 0x040a7004 over cover 0x040a7003;
    buffer.SetInt64(key64, value64);
    ret = buffer.GetInt64(key64, aValue64);
    EXPECT_EQ(ret, 0);
    EXPECT_EQ(value64, aValue64);
}

/*
 * Feature: Surface
 * Function: Surface set width and height
 * SubFunction: NA
 * FunctionPoints: buffer consuctor and set/get width and height.
 * EnvConditions: NA
 * CaseDescription: Surface constuctor and set/set attr.
 */
HWTEST_F(SurfaceTest, surface_set_001, TestSize.Level1)
{
    IBufferConsumerListener* consumerListener = new BufferConsumerTest();
    if (consumerListener == nullptr) {
        return;
    }
    Surface* surface = Surface::CreateSurface();
    if (surface == nullptr) {
        delete consumerListener;
        consumerListener = nullptr;
        return;
    }
    surface->RegisterConsumerListener(*consumerListener);

    EXPECT_EQ(1, surface->GetQueueSize());
    EXPECT_EQ(0, surface->GetUsage()); // default usage is BUFFER_CONSUMER_USAGE_SORTWARE(0)
    EXPECT_EQ(0, surface->GetSize());
    EXPECT_EQ(0, surface->GetWidth());
    EXPECT_EQ(0, surface->GetHeight());
    EXPECT_EQ(101, surface->GetFormat()); // default format is IMAGE_PIXEL_FORMAT_RGB565(101)
    EXPECT_EQ(4, surface->GetStrideAlignment()); // default format stride alignment is 4

    // set width = 0 failed, return default.
    surface->SetWidthAndHeight(0, 1080);
    EXPECT_EQ(0, surface->GetWidth());
    EXPECT_EQ(0, surface->GetHeight());

    // set width = 7681 failed, return default.
    surface->SetWidthAndHeight(7681, 1080);
    EXPECT_EQ(0, surface->GetWidth());
    EXPECT_EQ(0, surface->GetHeight());

    // set width = 7680 succeed, return 7680.
    surface->SetWidthAndHeight(7680, 1080);
    EXPECT_EQ(7680, surface->GetWidth());
    EXPECT_EQ(1080, surface->GetHeight());

    // set width = 1920 succeed, return 1980.
    surface->SetWidthAndHeight(1920, 1080);
    EXPECT_EQ(1920, surface->GetWidth());
    EXPECT_EQ(1080, surface->GetHeight());

    // set height = 0 failed, return default.
    surface->SetWidthAndHeight(1920, 0);
    EXPECT_EQ(1920, surface->GetWidth());
    EXPECT_EQ(1080, surface->GetHeight());

    // set height = 7681 failed, return default.
    surface->SetWidthAndHeight(1920, 7681);
    EXPECT_EQ(1920, surface->GetWidth());
    EXPECT_EQ(1080, surface->GetHeight());

    // set height = 7680 succeed, return 7680.
    surface->SetWidthAndHeight(1920, 7680);
    EXPECT_EQ(1920, surface->GetWidth());
    EXPECT_EQ(7680, surface->GetHeight());

    // set height = 720 succeed, return 720.
    surface->SetWidthAndHeight(1280, 720);
    EXPECT_EQ(1280, surface->GetWidth());
    EXPECT_EQ(720, surface->GetHeight());

    surface->UnregisterConsumerListener();
    delete surface;
    delete consumerListener;
}

/*
 * Feature: Surface
 * Function: Surface set format
 * SubFunction: NA
 * FunctionPoints: buffer consuctor and set/get format.
 * EnvConditions: NA
 * CaseDescription: Surface constuctor and set/set format.
 */
HWTEST_F(SurfaceTest, surface_set_002, TestSize.Level1)
{
    IBufferConsumerListener* consumerListener = new BufferConsumerTest();
    if (consumerListener == nullptr) {
        return;
    }
    Surface* surface = Surface::CreateSurface();
    if (surface == nullptr) {
        delete consumerListener;
        consumerListener = nullptr;
        return;
    }
    surface->RegisterConsumerListener(*consumerListener);

    EXPECT_EQ(1, surface->GetQueueSize());
    EXPECT_EQ(0, surface->GetUsage()); // default usage is BUFFER_CONSUMER_USAGE_SORTWARE(0)
    EXPECT_EQ(0, surface->GetSize());
    EXPECT_EQ(0, surface->GetWidth());
    EXPECT_EQ(0, surface->GetHeight());
    EXPECT_EQ(101, surface->GetFormat()); // default format is IMAGE_PIXEL_FORMAT_RGB565(101)
    EXPECT_EQ(4, surface->GetStrideAlignment()); // default format stride alignment is 4

    // set format 0 failed, return default.
    surface->SetFormat(0);
    EXPECT_EQ(101, surface->GetFormat());

    // set format 105 failed, return default.
    surface->SetFormat(105);
    EXPECT_EQ(101, surface->GetFormat());

    // set format 102 succeed, return 102.
    surface->SetFormat(102);
    EXPECT_EQ(102, surface->GetFormat());

    surface->UnregisterConsumerListener();
    delete surface;
    delete consumerListener;
}

/*
 * Feature: Surface
 * Function: Surface set stride alignment
 * SubFunction: NA
 * FunctionPoints: buffer consuctor and set/get stride alignment.
 * EnvConditions: NA
 * CaseDescription: Surface constuctor and set/set stride alignment.
 */
HWTEST_F(SurfaceTest, surface_set_003, TestSize.Level1)
{
    IBufferConsumerListener* consumerListener = new BufferConsumerTest();
    if (consumerListener == nullptr) {
        return;
    }
    Surface* surface = Surface::CreateSurface();
    if (surface == nullptr) {
        delete consumerListener;
        consumerListener = nullptr;
        return;
    }
    surface->RegisterConsumerListener(*consumerListener);

    EXPECT_EQ(1, surface->GetQueueSize());
    EXPECT_EQ(0, surface->GetUsage()); // default usage is BUFFER_CONSUMER_USAGE_SORTWARE(0)
    EXPECT_EQ(0, surface->GetSize());
    EXPECT_EQ(0, surface->GetWidth());
    EXPECT_EQ(0, surface->GetHeight());
    EXPECT_EQ(101, surface->GetFormat()); // default format is IMAGE_PIXEL_FORMAT_RGB565(101)
    EXPECT_EQ(4, surface->GetStrideAlignment()); // default format stride alignment is 4

    surface->SetWidthAndHeight(99, 90);

    // set stride alignment 3 failed, return default.
    surface->SetStrideAlignment(3);
    EXPECT_EQ(4, surface->GetStrideAlignment());
    EXPECT_EQ(200, surface->GetStride());

    // set stride alignment 33 failed, return default.
    surface->SetStrideAlignment(33);
    EXPECT_EQ(4, surface->GetStrideAlignment());
    EXPECT_EQ(200, surface->GetStride());

    // set stride alignment 32 succeed, return default.
    surface->SetStrideAlignment(32);
    EXPECT_EQ(224, surface->GetStride());

    // set stride alignment 8 succeed, return default.
    surface->SetStrideAlignment(8);
    EXPECT_EQ(200, surface->GetStride());

    surface->UnregisterConsumerListener();
    delete surface;
    delete consumerListener;
}

/*
 * Feature: Surface
 * Function: Surface set size
 * SubFunction: NA
 * FunctionPoints: buffer consuctor and set/get size.
 * EnvConditions: NA
 * CaseDescription: Surface constuctor and set/set size.
 */
HWTEST_F(SurfaceTest, surface_set_004, TestSize.Level1)
{
    IBufferConsumerListener* consumerListener = new BufferConsumerTest();
    if (consumerListener == nullptr) {
        return;
    }
    Surface* surface = Surface::CreateSurface();
    if (surface == nullptr) {
        delete consumerListener;
        consumerListener = nullptr;
        return;
    }
    surface->RegisterConsumerListener(*consumerListener);

    EXPECT_EQ(1, surface->GetQueueSize());
    EXPECT_EQ(0, surface->GetUsage()); // default usage is BUFFER_CONSUMER_USAGE_SORTWARE(0)
    EXPECT_EQ(0, surface->GetSize());
    EXPECT_EQ(0, surface->GetWidth());
    EXPECT_EQ(0, surface->GetHeight());
    EXPECT_EQ(101, surface->GetFormat()); // default format is IMAGE_PIXEL_FORMAT_RGB565(101)
    EXPECT_EQ(4, surface->GetStrideAlignment()); // default format stride alignment is 4

    // set size 0 failed, return default.
    surface->SetSize(0);
    EXPECT_EQ(0, surface->GetSize());

    // set size 58982400 failed, return default.
    surface->SetSize(58982400);
    EXPECT_EQ(0, surface->GetSize());

    // set size 1024 succeed, return default.
    surface->SetSize(1024);
    EXPECT_EQ(1024, surface->GetSize());

    surface->UnregisterConsumerListener();
    delete surface;
    delete consumerListener;
}

/*
 * Feature: Surface
 * Function: Surface set usage
 * SubFunction: NA
 * FunctionPoints: buffer consuctor and set/get usage.
 * EnvConditions: NA
 * CaseDescription: Surface constuctor and set/set usage.
 */
HWTEST_F(SurfaceTest, surface_set_005, TestSize.Level1)
{
    IBufferConsumerListener* consumerListener = new BufferConsumerTest();
    if (consumerListener == nullptr) {
        return;
    }
    Surface* surface = Surface::CreateSurface();
    if (surface == nullptr) {
        delete consumerListener;
        consumerListener = nullptr;
        return;
    }
    surface->RegisterConsumerListener(*consumerListener);

    EXPECT_EQ(1, surface->GetQueueSize());
    EXPECT_EQ(0, surface->GetUsage()); // default usage is BUFFER_CONSUMER_USAGE_SORTWARE(0)
    EXPECT_EQ(0, surface->GetSize());
    EXPECT_EQ(0, surface->GetWidth());
    EXPECT_EQ(0, surface->GetHeight());
    EXPECT_EQ(0, surface->GetHeight());
    EXPECT_EQ(101, surface->GetFormat()); // default format is IMAGE_PIXEL_FORMAT_RGB565(101)
    EXPECT_EQ(4, surface->GetStrideAlignment()); // default format stride alignment is 4

    // set size BUFFER_CONSUMER_USAGE_MAX(4) failed, return default.
    surface->SetUsage(4);
    EXPECT_EQ(0, surface->GetUsage());

    // set size 3 succeed, return default.
    surface->SetUsage(3);
    EXPECT_EQ(3, surface->GetUsage());

    surface->UnregisterConsumerListener();
    delete surface;
    delete consumerListener;
}

/*
 * Feature: Surface
 * Function: Surface set queue size
 * SubFunction: NA
 * FunctionPoints: buffer consuctor and set/get queue size.
 * EnvConditions: NA
 * CaseDescription: Surface constuctor and set/set attr.
 */
HWTEST_F(SurfaceTest, surface_set_006, TestSize.Level1)
{
    IBufferConsumerListener* consumerListener = new BufferConsumerTest();
    if (consumerListener == nullptr) {
        return;
    }
    Surface* surface = Surface::CreateSurface();
    if (surface == nullptr) {
        delete consumerListener;
        consumerListener = nullptr;
        return;
    }
    surface->RegisterConsumerListener(*consumerListener);

    EXPECT_EQ(1, surface->GetQueueSize());
    EXPECT_EQ(0, surface->GetUsage()); // default usage is BUFFER_CONSUMER_USAGE_SORTWARE(0)
    EXPECT_EQ(0, surface->GetSize());
    EXPECT_EQ(0, surface->GetWidth());
    EXPECT_EQ(0, surface->GetHeight());
    EXPECT_EQ(0, surface->GetHeight());
    EXPECT_EQ(101, surface->GetFormat()); // default format is IMAGE_PIXEL_FORMAT_RGB565(101)
    EXPECT_EQ(4, surface->GetStrideAlignment()); // default format stride alignment is 4

    // set queue size failed, return default.
    surface->SetQueueSize(0);
    EXPECT_EQ(1, surface->GetQueueSize());

    // set queue size failed more than SURFACE_MAX_QUEUE_SIZE(10), return default.
    surface->SetQueueSize(11);
    EXPECT_EQ(1, surface->GetQueueSize());

    // set queue size SURFACE_MAX_QUEUE_SIZE(10), return 10.
    surface->SetQueueSize(10);
    EXPECT_EQ(10, surface->GetQueueSize());

    // set queue size 5 succeed, return 5.
    surface->SetQueueSize(5);
    EXPECT_EQ(5, surface->GetQueueSize());

    surface->UnregisterConsumerListener();
    delete surface;
    delete consumerListener;
}

/*
 * Feature: Surface
 * Function: Surface set user data
 * SubFunction: NA
 * FunctionPoints: buffer consuctor and set/get user data.
 * EnvConditions: NA
 * CaseDescription: Surface constuctor and set/set user data.
 */
HWTEST_F(SurfaceTest, surface_set_007, TestSize.Level1)
{
    IBufferConsumerListener* consumerListener = new BufferConsumerTest();
    if (consumerListener == nullptr) {
        return;
    }
    Surface* surface = Surface::CreateSurface();
    if (surface == nullptr) {
        delete consumerListener;
        consumerListener = nullptr;
        return;
    }
    surface->RegisterConsumerListener(*consumerListener);

    EXPECT_EQ(1, surface->GetQueueSize());
    EXPECT_EQ(0, surface->GetUsage()); // default usage is BUFFER_CONSUMER_USAGE_SORTWARE(0)
    EXPECT_EQ(0, surface->GetSize());
    EXPECT_EQ(0, surface->GetWidth());
    EXPECT_EQ(0, surface->GetHeight());
    EXPECT_EQ(101, surface->GetFormat()); // default format is IMAGE_PIXEL_FORMAT_RGB565(101)
    EXPECT_EQ(4, surface->GetStrideAlignment()); // default format stride alignment is 4

    surface->SetUserData("testkey", "testvalue");
    EXPECT_EQ("testvalue", surface->GetUserData("testkey"));

    surface->UnregisterConsumerListener();
    delete surface;
    delete consumerListener;
}

/*
 * Feature: Surface
 * Function: Surface single process constuctor.
 * SubFunction: NA
 * FunctionPoints: buffer constuctor and set/get attr.
 * EnvConditions: NA
 * CaseDescription: Surface constuctor.
 */
HWTEST_F(SurfaceTest, surface_001, TestSize.Level1)
{
    IBufferConsumerListener* consumerListener = new BufferConsumerTest();
    if (consumerListener == nullptr) {
        return;
    }
    Surface* surface = Surface::CreateSurface();
    if (surface == nullptr) {
        delete consumerListener;
        consumerListener = nullptr;
        return;
    }
    surface->RegisterConsumerListener(*consumerListener);

    EXPECT_EQ(1, surface->GetQueueSize());
    EXPECT_EQ(0, surface->GetUsage()); // default usage is BUFFER_CONSUMER_USAGE_SORTWARE(0)
    EXPECT_EQ(0, surface->GetSize());
    EXPECT_EQ(0, surface->GetWidth());
    EXPECT_EQ(0, surface->GetHeight());
    EXPECT_EQ(0, surface->GetHeight());
    EXPECT_EQ(101, surface->GetFormat()); // default format is IMAGE_PIXEL_FORMAT_RGB565(101)
    EXPECT_EQ(4, surface->GetStrideAlignment()); // default format stride alignment is 4

    surface->SetSize(1024); // Set alloc 1024B SHM
    EXPECT_EQ(1024, surface->GetSize());

    surface->SetWidthAndHeight(101, 202); // set width(101), height(202)
    surface->SetFormat(102); // set format IMAGE_PIXEL_FORMAT_ARGB1555(102)
    surface->SetStrideAlignment(8); // set format stride alignment is 8

    EXPECT_EQ(101, surface->GetWidth());
    EXPECT_EQ(202, surface->GetHeight());
    EXPECT_EQ(102, surface->GetFormat());
    EXPECT_EQ(8, surface->GetStrideAlignment());
    EXPECT_EQ(208, surface->GetStride()); // calculate by width, height, format, stride alignment.
    EXPECT_EQ(42016, surface->GetSize()); // calculate by width, height, format, stride alignment.

    surface->UnregisterConsumerListener();
    delete surface;
    delete consumerListener;
}

/*
 * Feature: Surface
 * Function: Surface single process request and cancel Buffer
 * SubFunction: NA
 * FunctionPoints: buffer request and cancel.
 * EnvConditions: NA
 * CaseDescription: Surface single process request and cancel Buffer.
 */
HWTEST_F(SurfaceTest, surface_002, TestSize.Level1)
{
    Surface* surface = Surface::CreateSurface();
    if (surface == nullptr) {
        return;
    }

    SurfaceBuffer* bufferFirst = surface->RequestBuffer(); // no size, return null pointer
    EXPECT_FALSE(bufferFirst);

    surface->SetSize(1024); // Set alloc 1024B SHM
    bufferFirst = surface->RequestBuffer();
    EXPECT_TRUE(bufferFirst);

    SurfaceBuffer* bufferSecond = surface->RequestBuffer(); // default queue size = 1, second return null pointer

    EXPECT_FALSE(bufferSecond);
    surface->CancelBuffer(bufferFirst);
    EXPECT_TRUE(surface->RequestBuffer());

    delete surface;
}

/*
 * Feature: Surface
 * Function: Surface single process request and flush Buffer
 * SubFunction: NA
 * FunctionPoints: buffer request and flush.
 * EnvConditions: NA
 * CaseDescription: Surface single process request and flush Buffer.
 */
HWTEST_F(SurfaceTest, surface_003, TestSize.Level1)
{
    Surface* surface = Surface::CreateSurface();
    ASSERT_TRUE(surface);

    SurfaceBuffer* requestBuffer = surface->RequestBuffer(); // no size, return null pointer
    EXPECT_FALSE(requestBuffer);

    surface->SetSize(1024); // Set alloc 1024B SHM
    requestBuffer = surface->RequestBuffer();
    EXPECT_TRUE(requestBuffer);

    SurfaceBufferImpl* buffer = new SurfaceBufferImpl();
    EXPECT_TRUE(surface->FlushBuffer(buffer) != 0); // Not allocated by surface, could not flush.
    EXPECT_EQ(0, surface->FlushBuffer(requestBuffer)); // allocated by surface, could flush.

    delete buffer;
    delete surface;
}

/*
 * Feature: Surface
 * Function: Surface single process acquire Buffer
 * SubFunction: NA
 * FunctionPoints: buffer acquire buffer
 * EnvConditions: NA
 * CaseDescription: Surface single process acquire Buffer.
 */
HWTEST_F(SurfaceTest, surface_004, TestSize.Level1)
{
    Surface* surface = Surface::CreateSurface();
    ASSERT_TRUE(surface);

    SurfaceBuffer* acquireBuffer = surface->AcquireBuffer(); // no size, return null pointer
    EXPECT_FALSE(acquireBuffer);

    surface->SetSize(1024); // Set alloc 1024B SHM
    SurfaceBuffer* requestBuffer = surface->RequestBuffer();
    if (requestBuffer == nullptr) {
        delete surface;
        return;
    }
    requestBuffer->SetInt32(10, 11); // set key-value <10, 11>

    EXPECT_EQ(0, surface->FlushBuffer(requestBuffer)); // allocated by surface, could flush.

    acquireBuffer = surface->AcquireBuffer();
    ASSERT_TRUE(acquireBuffer);
    int32_t value;
    acquireBuffer->GetInt32(10, value);
    EXPECT_EQ(11, value);

    delete surface;
}

/*
 * Feature: Surface
 * Function: Surface single process release Buffer
 * SubFunction: NA
 * FunctionPoints: buffer release buffer
 * EnvConditions: NA
 * CaseDescription: Surface single process release Buffer.
 */
HWTEST_F(SurfaceTest, surface_005, TestSize.Level1)
{
    Surface* surface = Surface::CreateSurface();
    ASSERT_TRUE(surface);

    SurfaceBufferImpl* buffer = new SurfaceBufferImpl();
    EXPECT_FALSE(surface->ReleaseBuffer(buffer)); // Not allocated by surface, could not release

    SurfaceBuffer* acquireBuffer = surface->AcquireBuffer(); // no size, return null pointer
    EXPECT_FALSE(acquireBuffer);

    surface->SetSize(1024); // Set alloc 1024B SHM
    SurfaceBuffer* requestBuffer = surface->RequestBuffer();
    if (requestBuffer == nullptr) {
        delete buffer;
        delete surface;
        return;
    }
    requestBuffer->SetInt32(10, 11); // set key-value <10, 11>

    EXPECT_EQ(0, surface->FlushBuffer(requestBuffer)); // allocated by surface, could flush.

    acquireBuffer = surface->AcquireBuffer();
    if (acquireBuffer == nullptr) {
        delete buffer;
        delete surface;
        return;
    }
    int32_t value;
    acquireBuffer->GetInt32(10, value);
    EXPECT_EQ(11, value);

    EXPECT_TRUE(surface->ReleaseBuffer(acquireBuffer));
    EXPECT_TRUE(acquireBuffer->GetInt32(10, value) != 0);

    delete buffer;
    delete surface;
}

/*
 * Feature: Surface
 * Function: Surface single process release Buffer
 * SubFunction: NA
 * FunctionPoints: buffer release buffer
 * EnvConditions: NA
 * CaseDescription: Surface single process release Buffer.
 */
HWTEST_F(SurfaceTest, surface_006, TestSize.Level1)
{
    IBufferConsumerListener* consumerListener = new BufferConsumerTest();
    if (consumerListener == nullptr) {
        return;
    }
    Surface* surface = Surface::CreateSurface();
    if (surface == nullptr) {
        delete consumerListener;
        consumerListener = nullptr;
        return;
    }
    surface->RegisterConsumerListener(*consumerListener);

    SurfaceBufferImpl* buffer = new SurfaceBufferImpl();
    EXPECT_FALSE(surface->ReleaseBuffer(buffer)); // Not allocated by surface, could not release

    SurfaceBuffer* acquireBuffer = surface->AcquireBuffer(); // no size, return null pointer
    EXPECT_FALSE(acquireBuffer);

    surface->SetSize(1024); // Set alloc 1024B SHM
    SurfaceBuffer* requestBuffer = surface->RequestBuffer();
    if (requestBuffer == nullptr) {
        delete buffer;
        buffer = nullptr;
        delete surface;
        surface = nullptr;
        delete consumerListener;
        consumerListener = nullptr;
        return;
    }

    requestBuffer->SetInt32(10, 11); // set key-value <10, 11>

    EXPECT_EQ(0, surface->FlushBuffer(requestBuffer)); // allocated by surface, could flush.

    acquireBuffer = surface->AcquireBuffer();
    if (acquireBuffer == nullptr) {
        delete buffer;
        buffer = nullptr;
        delete surface;
        surface = nullptr;
        delete consumerListener;
        consumerListener = nullptr;
        return;
    }

    int32_t value;
    acquireBuffer->GetInt32(10, value);
    EXPECT_EQ(11, value);

    EXPECT_TRUE(surface->ReleaseBuffer(acquireBuffer));
    EXPECT_TRUE(acquireBuffer->GetInt32(10, value) != 0);

    delete buffer;
    delete surface;
    delete consumerListener;
}
} // namespace OHOS