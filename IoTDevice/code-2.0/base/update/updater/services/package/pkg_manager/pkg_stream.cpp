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
#include "pkg_stream.h"
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include "pkg_manager.h"
#include "pkg_utils.h"
#include "securec.h"

namespace hpackage {
const std::string PkgStreamImpl::GetFileName() const
{
    return fileName_;
}

PkgStreamPtr PkgStreamImpl::ConvertPkgStream(PkgManager::StreamPtr stream)
{
    return (PkgStreamPtr)stream;
}

void PkgStreamImpl::AddRef()
{
    refCount_++;
}

void PkgStreamImpl::DelRef()
{
    refCount_--;
}

bool PkgStreamImpl::IsRef() const
{
    return refCount_ == 0;
}

FileStream::~FileStream()
{
    if (stream_ != nullptr) {
        fflush(stream_);
        fclose(stream_);
        stream_ = nullptr;
    }
}

int32_t FileStream::Read(const PkgBuffer &data, size_t offset, size_t needRead, size_t &readLen)
{
    PKG_CHECK(stream_ != nullptr, return PKG_INVALID_STREAM, "Invalid stream");
    PKG_CHECK(data.length >= needRead, return PKG_INVALID_STREAM, "Invalid stream");
    readLen = 0;
    size_t len = GetFileLength();
    fseek(stream_, offset, SEEK_SET);
    PKG_CHECK(offset <= len, return PKG_INVALID_STREAM, "Invalid offset");
    len = fread(data.buffer, 1, needRead, stream_);
    readLen = len;
    return PKG_SUCCESS;
}

int32_t FileStream::Write(const PkgBuffer &data, size_t size, size_t offset)
{
    PKG_CHECK(streamType_ == PkgStreamType_Write, return PKG_INVALID_STREAM, "Invalid stream type");
    PKG_CHECK(stream_ != nullptr, return PKG_INVALID_STREAM, "Invalid stream");
    fseek(stream_, offset, SEEK_SET);
    size_t len = fwrite(data.buffer, size, 1, stream_);
    PKG_CHECK(len == 1, return PKG_INVALID_STREAM, "Write buffer fail");
    return PKG_SUCCESS;
}

size_t FileStream::GetFileLength()
{
    PKG_CHECK(stream_ != nullptr, return 0, "Invalid stream");
    if (fileLength_ == 0) {
        PKG_CHECK(Seek(0, SEEK_END) == 0, return -1, "Invalid stream");
        fileLength_ = ftell(stream_);
        fseek(stream_, 0, SEEK_SET);
    }
    return fileLength_;
}

int32_t FileStream::Seek(long int offset, int whence)
{
    PKG_CHECK(stream_ != nullptr, return PKG_INVALID_STREAM, "Invalid stream");
    return fseek(stream_, offset, whence);
}

int32_t FileStream::Flush(size_t size)
{
    PKG_CHECK(stream_ != nullptr, return PKG_INVALID_STREAM, "Invalid stream");
    if (fileLength_ == 0) {
        fileLength_ = size;
    }
    fseek(stream_, 0, SEEK_END);
    fileLength_ = ftell(stream_);
    if (size != fileLength_) {
        PKG_LOGE("Flush size %zu local size:%zu", size, fileLength_);
    }
    PKG_CHECK(fflush(stream_) == 0, return PKG_INVALID_STREAM, "Invalid stream");
    return PKG_SUCCESS;
}

MemoryMapStream::~MemoryMapStream()
{
    PKG_CHECK(memMap_ != nullptr, return, "Invalid memory map");
    if (streamType_ == PkgStreamType_MemoryMap) {
        ReleaseMemory(memMap_, memSize_);
    }
}

int32_t MemoryMapStream::Read(const PkgBuffer &data, size_t start, size_t needRead, size_t &readLen)
{
    PKG_CHECK(memMap_ != nullptr, return PKG_INVALID_STREAM, "Invalid memory map");
    PKG_CHECK(start <= memSize_, return PKG_INVALID_STREAM, "Invalid start");
    PKG_CHECK(data.length >= needRead, return PKG_INVALID_STREAM, "Invalid start");

    MemoryMapStream::Seek(start, SEEK_SET);
    size_t copyLen = GetFileLength() - start;
    readLen = ((copyLen > needRead) ? needRead : copyLen);
    PKG_CHECK(!memcpy_s(data.buffer, needRead, memMap_ + currOffset_, readLen), return PKG_NONE_MEMORY,
        "Memcpy failed size:%zu, start:%zu copyLen:%zu %zu", needRead, start, copyLen, readLen);
    return PKG_SUCCESS;
}

int32_t MemoryMapStream::Write(const PkgBuffer &data, size_t size, size_t start)
{
    PKG_CHECK(memMap_ != nullptr, return PKG_INVALID_STREAM, "Invalid memory map");
    PKG_CHECK(start <= memSize_, return PKG_INVALID_STREAM, "Invalid start");

    currOffset_ = start;
    size_t copyLen = memSize_ - start;
    PKG_CHECK(copyLen >= size, return PKG_INVALID_STREAM, "Write fail copyLen %zu, %zu", copyLen, size);
    int32_t ret = memcpy_s(memMap_ + currOffset_, memSize_ - currOffset_, data.buffer, size);
    PKG_CHECK(ret == PKG_SUCCESS, return PKG_INVALID_STREAM, "Write fail");
    return PKG_SUCCESS;
}

int32_t MemoryMapStream::Seek(long int offset, int whence)
{
    if (whence == SEEK_SET) {
        PKG_CHECK(offset >= 0, return PKG_INVALID_STREAM, "Invalid offset");
        PKG_CHECK(static_cast<size_t>(offset) <= memSize_, return PKG_INVALID_STREAM, "Invalid offset");
        currOffset_ = offset;
    } else if (whence == SEEK_CUR) {
        PKG_CHECK(static_cast<size_t>(offset) <= (memSize_ - currOffset_), return PKG_INVALID_STREAM,
            "Invalid offset");
        currOffset_ += offset;
    } else {
        PKG_CHECK(offset <= 0, return PKG_INVALID_STREAM, "Invalid offset");
        PKG_CHECK((memSize_ + offset) <= memSize_, return PKG_INVALID_STREAM, "Invalid offset");
        PKG_CHECK((memSize_ + offset) >= 0, return PKG_INVALID_STREAM, "Invalid offset");
        currOffset_ = memSize_ + offset;
    }
    return PKG_SUCCESS;
}
} // namespace hpackage
