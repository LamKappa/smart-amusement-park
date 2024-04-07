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

#ifndef LZ4_ADAPTER_H
#define LZ4_ADAPTER_H
#include <iostream>
#include <vector>
#include "deflate_adapter.h"
#include "diffpatch.h"
#include "lz4.h"
#include "lz4frame.h"
#include "lz4hc.h"
#include "pkg_manager.h"
#include "securec.h"

namespace updatepatch {
#define LZ4_BLOCK_SIZE(blockId) (1 << (8 + (2 * (blockId))))

class Lz4Adapter : public DeflateAdapter {
public:
    static constexpr uint32_t LZ4B_BLOCK_SIZE = 1 << 22; // (4M)
    static constexpr uint32_t LZ4B_MAGIC_NUMBER = 0x184C2102;
    static constexpr uint32_t LZ4B_REVERSED_LEN = 4;
    Lz4Adapter(UpdatePatchWriterPtr outStream, size_t offset, const hpackage::PkgManager::FileInfoPtr fileInfo);
    ~Lz4Adapter() override {}

    int32_t Open() override;
    int32_t Close() override;
    int32_t WriteData(const BlockBuffer &srcData) override;
    int32_t FlushData(size_t &offset) override;
protected:
    virtual int32_t CompressData(const BlockBuffer &srcData) = 0;
    std::vector<uint8_t> buffer_ {};
    UpdatePatchWriterPtr outStream_ { nullptr };

    size_t offset_;
    int32_t compressionLevel_ { 0 };
    int32_t blockIndependence_ { 0 };
    int32_t contentChecksumFlag_ { 0 };
    int32_t blockSizeID_ { 0 };
    int32_t autoFlush_ {1};
};

class Lz4FrameAdapter : public Lz4Adapter {
public:
    Lz4FrameAdapter(UpdatePatchWriterPtr outStream, size_t offset,
        const hpackage::PkgManager::FileInfoPtr fileInfo) : Lz4Adapter(outStream, offset, fileInfo) {}
    ~Lz4FrameAdapter() override {}

    int32_t Open() override;
    int32_t Close() override;
    int32_t WriteData(const BlockBuffer &srcData) override;
    int32_t FlushData(size_t &offset) override;
protected:
    int32_t CompressData(const BlockBuffer &srcData) override;
    size_t currDataSize_ = 0;
    std::vector<uint8_t> inData_ {};
private:
    LZ4F_compressionContext_t compressionContext_ {};
};

class Lz4BlockAdapter : public Lz4FrameAdapter {
public:
    Lz4BlockAdapter(UpdatePatchWriterPtr outStream, size_t offset,
        const hpackage::PkgManager::FileInfoPtr fileInfo) : Lz4FrameAdapter(outStream, offset, fileInfo) {}
    ~Lz4BlockAdapter() override {}

    int32_t Open() override;
    int32_t Close() override;
    int32_t FlushData(size_t &offset) override;
private:
    int32_t CompressData(const BlockBuffer &srcData) override;
};
} // namespace updatepatch
#endif // LZ4_ADAPTER_H