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

#ifndef UPDATER_SPARSE_IMAGE_HANDLER_H
#define UPDATER_SPARSE_IMAGE_HANDLER_H
#include <cerrno>

#include "log/log.h"
#include "securec.h"
#include "sparse_image.h"

namespace updater {
class ISparseImageHandler {
public:
    // Seek sparse image to specific offset.
    virtual void Seek(int64_t offset) = 0;
    virtual int64_t GetOffset() const = 0;
    virtual void SetOffset(int64_t offset) = 0;
    virtual int CopyToSparseImage(struct SparseImage *si, int64_t size, uint32_t blockIndex) = 0;
    virtual int ReadContent(void *buff, int64_t len) = 0;
    virtual ~ISparseImageHandler() {}
};

class SparseImageHandlerFromBuffer : public ISparseImageHandler {
public:
    explicit SparseImageHandlerFromBuffer(char *buff) : buff_(buff), offset_(0) {}
    ~SparseImageHandlerFromBuffer() override {}
    void Seek(int64_t offset) override
    {
        buff_ += offset;
        offset_ += offset;
    }

    int64_t GetOffset() const override
    {
        return offset_;
    }

    void SetOffset(int64_t offset) override
    {
        buff_ -= offset_; // Move buff_ back to beginning.
        buff_ += offset;
        offset_ = offset;
    }

    int CopyToSparseImage(struct SparseImage *si, int64_t size, uint32_t blockIndex) override
    {
        // block list should not be null.
        struct BlockList *bl = si->bl;
        char *buffer = buff_;
        return AddRawDataToSparseImage(bl, buffer, size, blockIndex);
    }

    int ReadContent(void *buff, int64_t size) override
    {
        if (memcpy_s(buff, static_cast<size_t>(size), buff_, static_cast<size_t>(size)) != EOK) {
            return -EINVAL;
        }
        Seek(size);
        return 0;
    }

private:
    char *buff_;
    int64_t offset_;
};
} // namespace updater
#endif // UPDATER_SPARSE_IMAGE_HANDLER_H
