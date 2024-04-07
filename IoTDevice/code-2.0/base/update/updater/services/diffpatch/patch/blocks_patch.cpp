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

#include "blocks_patch.h"
#include <cstdio>
#include <iostream>
#include <vector>
#include "diffpatch.h"

using namespace hpackage;
using namespace std;

namespace updatepatch {
#define PATCH_MIN BSDIFF_MAGIC.size() + sizeof(int64_t) * 3
#define GET_BYTE_FROM_BUFFER(v, index, buffer)  (y) = (y) * 256; (y) += buffer[index]
constexpr uint8_t BUFFER_MASK = 0x80;

static int64_t ReadLE64(const uint8_t *buffer)
{
    if (buffer == nullptr) {
        return 0;
    }
    int64_t y = 0;
    int32_t index = static_cast<int32_t>(sizeof(int64_t)) - 1;
    y = buffer[index] & (~BUFFER_MASK);
    index--;
    GET_BYTE_FROM_BUFFER(y, index, buffer);
    index--;
    GET_BYTE_FROM_BUFFER(y, index, buffer);
    index--;
    GET_BYTE_FROM_BUFFER(y, index, buffer);
    index--;
    GET_BYTE_FROM_BUFFER(y, index, buffer);
    index--;
    GET_BYTE_FROM_BUFFER(y, index, buffer);
    index--;
    GET_BYTE_FROM_BUFFER(y, index, buffer);
    index--;
    GET_BYTE_FROM_BUFFER(y, index, buffer);

    index = static_cast<int32_t>(sizeof(int64_t));
    if (buffer[index - 1] & BUFFER_MASK) {
        y = -y;
    }
    return y;
}

int32_t BlocksPatch::ApplyPatch()
{
    PATCH_LOGI("BlocksPatch::ApplyPatch");
    int64_t controlDataSize = 0;
    int64_t diffDataSize = 0;
    int32_t ret = ReadHeader(controlDataSize, diffDataSize, newSize_);
    PATCH_CHECK(ret == 0, return -1, "Failed to read header ");

    while (newOffset_ < newSize_) {
        ControlData ctrlData {};
        int32_t ret = ReadControlData(ctrlData);
        PATCH_CHECK(ret == 0, return ret, "Failed to read control data");
        PATCH_CHECK(newOffset_ + ctrlData.diffLength <= newSize_, return PATCH_INVALID_PATCH,
            "Failed to check new offset %ld %zu", ctrlData.diffLength, newOffset_);

        ret = RestoreDiffData(ctrlData);
        PATCH_CHECK(ret == 0, return ret, "Failed to read diff data");
        oldOffset_ += ctrlData.diffLength;
        newOffset_ += ctrlData.diffLength;
        PATCH_CHECK(newOffset_ + ctrlData.extraLength <= newSize_, return PATCH_INVALID_PATCH,
            "Failed to check new offset %ld %zu", ctrlData.diffLength, newOffset_);

        ret = RestoreExtraData(ctrlData);
        PATCH_CHECK(ret == 0, return ret, "Failed to read extra data");

        newOffset_ += ctrlData.extraLength;
        oldOffset_ += ctrlData.offsetIncrement;
    }
    controlDataReader_->Close();
    diffDataReader_->Close();
    extraDataReader_->Close();
    PATCH_LOGI("BlocksPatch::ApplyPatch %zu newSize: %zu", newOffset_, newSize_);
    return 0;
}

int32_t BlocksPatch::ReadHeader(int64_t &controlDataSize, int64_t &diffDataSize, int64_t &newSize)
{
    PATCH_LOGI("BlocksPatch::ApplyPatch %p %zu %zu", patchInfo_.buffer, patchInfo_.start, patchInfo_.length);
    PATCH_CHECK(patchInfo_.buffer != nullptr && patchInfo_.length > PATCH_MIN, return -1, "Invalid parm");
    BlockBuffer patchData = {patchInfo_.buffer + patchInfo_.start, patchInfo_.length - patchInfo_.start};
    PATCH_LOGI("Restore patch hash %zu %s",
        patchInfo_.length - patchInfo_.start, GeneraterBufferHash(patchData).c_str());
    uint8_t *header = patchInfo_.buffer + patchInfo_.start;
    // Compare header
    PATCH_CHECK(memcmp(header, BSDIFF_MAGIC.c_str(), BSDIFF_MAGIC.size()) == 0,
        return -1, "Corrupt patch, patch head != BSDIFF40");

    /* Read lengths from header */
    size_t offset = BSDIFF_MAGIC.size();
    controlDataSize = ReadLE64(header + offset);
    offset += sizeof(int64_t);
    diffDataSize = ReadLE64(header + offset);
    offset += sizeof(int64_t);
    newSize = ReadLE64(header + offset);
    offset += sizeof(int64_t);

    PATCH_CHECK(controlDataSize >= 0, return -1, "Invalid control data size");
    PATCH_CHECK(newSize >= 0, return -1, "Invalid new data size");
    PATCH_CHECK(diffDataSize >= 0 && (diffDataSize + controlDataSize) <= static_cast<int64_t>(patchInfo_.length),
        return -1, "Invalid patch data size");

    BlockBuffer patchBuffer = {header, patchInfo_.length - patchInfo_.start};
    controlDataReader_.reset(new BZip2BufferReadAdapter(offset, static_cast<size_t>(controlDataSize), patchBuffer));
    offset += controlDataSize;
    diffDataReader_.reset(new BZip2BufferReadAdapter(offset, static_cast<size_t>(diffDataSize), patchBuffer));
    offset += diffDataSize;
    extraDataReader_.reset(new BZip2BufferReadAdapter(offset,
        patchInfo_.length - patchInfo_.start - offset, patchBuffer));
    PATCH_CHECK(controlDataReader_ != nullptr && diffDataReader_ != nullptr && extraDataReader_ != nullptr,
        return -1, "Failed to create reader");
    controlDataReader_->Open();
    diffDataReader_->Open();
    extraDataReader_->Open();
    return 0;
}

int32_t BlocksPatch::ReadControlData(ControlData &ctrlData)
{
    std::vector<uint8_t> data(sizeof(int64_t), 0);
    BlockBuffer info = {data.data(), sizeof(int64_t)};
    int32_t ret = controlDataReader_->ReadData(info);
    PATCH_CHECK(ret == 0, return ret, "Failed to read diffLength");
    ctrlData.diffLength = ReadLE64(info.buffer);
    ret = controlDataReader_->ReadData(info);
    PATCH_CHECK(ret == 0, return ret, "Failed to read extraLength");
    ctrlData.extraLength = ReadLE64(info.buffer);
    ret = controlDataReader_->ReadData(info);
    PATCH_CHECK(ret == 0, return ret, "Failed to read offsetIncrement");
    ctrlData.offsetIncrement = ReadLE64(info.buffer);
    return 0;
}

int32_t BlocksBufferPatch::ReadHeader(int64_t &controlDataSize, int64_t &diffDataSize, int64_t &newSize)
{
    int32_t ret = BlocksPatch::ReadHeader(controlDataSize, diffDataSize, newSize);
    PATCH_CHECK(ret == 0, return -1, "Failed to read header");
    PATCH_LOGI("ReadHeader controlDataSize: %ld %ld %ld", controlDataSize, diffDataSize, newSize);
    newData_.resize(newSize);
    return 0;
}

int32_t BlocksBufferPatch::RestoreDiffData(const ControlData &ctrlData)
{
    if (ctrlData.diffLength <= 0) {
        return 0;
    }
    BlockBuffer diffData = {newData_.data() + newOffset_, static_cast<size_t>(ctrlData.diffLength)};
    int32_t ret = diffDataReader_->ReadData(diffData);
    PATCH_CHECK(ret == 0, return ret, "Failed to read diff data");

    for (int64_t i = 0; i < ctrlData.diffLength; i++) {
        if (((oldOffset_ + i) >= 0) && (static_cast<size_t>(oldOffset_ + i) < oldInfo_.length)) {
            newData_[newOffset_ + i] += oldInfo_.buffer[oldOffset_ + i];
        }
    }
    return 0;
}

int32_t BlocksBufferPatch::RestoreExtraData(const ControlData &ctrlData)
{
    if (ctrlData.extraLength <= 0) {
        return 0;
    }
    BlockBuffer extraData = {newData_.data() + newOffset_, static_cast<size_t>(ctrlData.extraLength)};
    int32_t ret = extraDataReader_->ReadData(extraData);
    PATCH_CHECK(ret == 0, return ret, "Failed to read extra data");
    return 0;
}

int32_t BlocksStreamPatch::RestoreDiffData(const ControlData &ctrlData)
{
    if (ctrlData.diffLength <= 0) {
        return 0;
    }
    std::vector<uint8_t> diffData(ctrlData.diffLength);
    BlockBuffer diffBuffer = {diffData.data(), diffData.size()};
    int32_t ret = diffDataReader_->ReadData(diffBuffer);
    PATCH_CHECK(ret == 0, return ret, "Failed to read diff data");

    size_t oldOffset = oldOffset_;
    size_t oldLength = stream_->GetFileLength();
    PkgBuffer buffer {};
    if (stream_->GetStreamType() == PkgStream::PkgStreamType_MemoryMap ||
        stream_->GetStreamType() == PkgStream::PkgStreamType_Buffer) {
        ret = stream_->GetBuffer(buffer);
        PATCH_CHECK(ret == 0, return ret, "Failed to get old buffer");
    } else {
        std::vector<uint8_t> oldData(ctrlData.diffLength);
        size_t readLen = 0;
        ret = stream_->Read(buffer, oldOffset_, ctrlData.diffLength, readLen);
        PATCH_CHECK(ret == 0 && readLen == static_cast<size_t>(ctrlData.diffLength),
            return ret, "Failed to get old buffer");
        oldOffset = 0;
    }
    for (int64_t i = 0; i < ctrlData.diffLength; i++) {
        if ((oldOffset_ + i >= 0) && (static_cast<size_t>(oldOffset_ + i) < oldLength)) {
            diffData[i] += buffer.buffer[oldOffset + i];
        }
    }
    // write
    return writer_->Write(newOffset_, diffBuffer, static_cast<size_t>(ctrlData.diffLength));
}

int32_t BlocksStreamPatch::RestoreExtraData(const ControlData &ctrlData)
{
    if (ctrlData.extraLength <= 0) {
        return 0;
    }
    std::vector<uint8_t> extraData(ctrlData.extraLength);
    BlockBuffer extraBuffer = {extraData.data(), static_cast<size_t>(ctrlData.extraLength)};
    int32_t ret = extraDataReader_->ReadData(extraBuffer);
    PATCH_CHECK(ret == 0, return ret, "Failed to read extra data");
    // write
    return writer_->Write(newOffset_, extraBuffer, static_cast<size_t>(ctrlData.extraLength));
}
} // namespace updatepatch
