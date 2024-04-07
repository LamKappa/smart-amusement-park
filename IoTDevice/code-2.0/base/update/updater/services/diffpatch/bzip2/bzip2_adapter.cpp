/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "bzip2_adapter.h"
#include <iostream>
#include <vector>
#include "bzlib.h"

using namespace hpackage;

namespace updatepatch {
int32_t BZip2Adapter::Open()
{
    memset_s(&stream_, sizeof(bz_stream), 0, sizeof(bz_stream));
    int32_t ret = BZ2_bzCompressInit(&stream_, BLOCK_SIZE_BEST, 0, 0);
    PATCH_CHECK(ret == BZ_OK, return ret, "Failed to bzcompressinit %d", ret);
    init_ = true;
    return ret;
}

int32_t BZip2Adapter::Close()
{
    if (!init_) {
        return PATCH_SUCCESS;
    }
    int32_t ret = BZ2_bzCompressEnd(&stream_);
    PATCH_CHECK(ret == BZ_OK, return ret, "Failed to bz_compressEnd %d", ret);
    init_ = false;
    return ret;
}

int32_t BZip2Adapter::WriteData(const BlockBuffer &srcData)
{
    stream_.next_in = reinterpret_cast<char*>(srcData.buffer);
    if (offset_ + srcData.length > buffer_.size()) {
        buffer_.resize(buffer_.size() + srcData.length);
    }
    stream_.avail_in = srcData.length;
    stream_.avail_out =  buffer_.size() - offset_;
    stream_.next_out = reinterpret_cast<char*>(buffer_.data() + offset_);
    int32_t ret = BZ_RUN_OK;
    do {
        ret = BZ2_bzCompress(&stream_, BZ_RUN);
        if (stream_.avail_in == 0) {
            break;
        }
    } while (ret == BZ_RUN_OK);
    PATCH_CHECK(ret == BZ_RUN_OK, return ret, "Failed to write data ret %d", ret);
    PATCH_CHECK(stream_.avail_in == 0, return ret, "Failed to write data");
    offset_ = stream_.next_out - reinterpret_cast<char*>(buffer_.data());
    return PATCH_SUCCESS;
}

int32_t BZip2Adapter::FlushData(size_t &offset)
{
    PATCH_DEBUG("FlushData offset_ %d offset %zu ", offset_, offset);
    stream_.next_in = nullptr;
    stream_.avail_in = 0;
    stream_.avail_out = buffer_.size() - offset_;
    stream_.next_out = reinterpret_cast<char*>(buffer_.data() + offset_);
    int ret = BZ_FINISH_OK;
    while (ret == BZ_FINISH_OK) {
        ret = BZ2_bzCompress(&stream_, BZ_FINISH);
        if (stream_.avail_out == 0) {
            offset_ = stream_.next_out - reinterpret_cast<char*>(buffer_.data());
            buffer_.resize(buffer_.size() + IGMDIFF_LIMIT_UNIT);
            stream_.avail_out = buffer_.size() - offset_;
            stream_.next_out = reinterpret_cast<char*>(buffer_.data() + offset_);
        }
    }
    PATCH_CHECK(ret == BZ_RUN_OK || ret == BZ_STREAM_END, return ret, "Failed to write data %d", ret);
    offset_ = stream_.next_out - reinterpret_cast<char*>(buffer_.data());
    PATCH_DEBUG("FlushData offset_ %zu ", offset_);
    offset = offset_;
    return 0;
}

int32_t BZip2BufferReadAdapter::Open()
{
    PATCH_CHECK(!init_, return -1, "State error %d", init_);
    PATCH_CHECK(dataLength_ <= buffer_.length, return -1, "Invalid buffer length");

    PATCH_DEBUG("BZip2BufferReadAdapter::Open %zu dataLength_ %zu", offset_, dataLength_);
    memset_s(&stream_, sizeof(bz_stream), 0, sizeof(bz_stream));
    int32_t ret = BZ2_bzDecompressInit(&stream_, 0, 0);
    PATCH_CHECK(ret == BZ_OK, return -1, "Failed to open read mem ret %d", ret);
    stream_.avail_in = static_cast<unsigned int>(dataLength_);
    stream_.next_in  = reinterpret_cast<char*>(buffer_.buffer + offset_);

    init_ = true;
    return PATCH_SUCCESS;
}

int32_t BZip2BufferReadAdapter::Close()
{
    if (!init_) {
        return PATCH_SUCCESS;
    }
    int32_t ret = 0;
    ret = BZ2_bzDecompressEnd(&stream_);
    PATCH_CHECK(ret == BZ_OK, return -1, "Failed to close read mem ret %d", ret);
    init_ = false;
    return PATCH_SUCCESS;
}

int32_t BZip2BufferReadAdapter::ReadData(BlockBuffer &info)
{
    PATCH_CHECK(init_, return -1, "State error %d", init_);
    int32_t ret = 0;
    size_t readLen = 0;
    stream_.next_out = reinterpret_cast<char*>(info.buffer);
    stream_.avail_out = info.length;
    while (1) {
        ret = BZ2_bzDecompress(&stream_);
        if (ret == BZ_STREAM_END) {
            readLen = info.length - stream_.avail_out;
            break;
        }
        PATCH_CHECK(ret == BZ_OK, return -1, "Failed to decompress ret %d", ret);
        if (stream_.avail_out == 0) {
            readLen = info.length;
            break;
        }
        PATCH_CHECK(stream_.avail_in != 0, return -1, "Not enough buffer to decompress");
    }
    if (readLen < info.length) {
        PATCH_LOGE("Failed to read mem ret %zu length %zu", readLen, info.length);
        return -1;
    }
    return 0;
}
} // namespace updatepatch
