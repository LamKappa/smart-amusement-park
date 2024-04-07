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

#ifndef BZIP2_ADAPTER_H
#define BZIP2_ADAPTER_H
#include <iostream>
#include <vector>
#include "bzlib.h"
#include "deflate_adapter.h"
#include "diffpatch.h"
#include "pkg_manager.h"
#include "securec.h"

namespace updatepatch {
class BZip2Adapter : public DeflateAdapter {
public:
    BZip2Adapter(std::vector<uint8_t> &buffer, size_t offset)
        : DeflateAdapter(), buffer_(buffer), offset_(offset) {}
    ~BZip2Adapter() override {}

    int32_t Open() override;
    int32_t Close() override;

    int32_t WriteData(const BlockBuffer &srcData) override;
    int32_t FlushData(size_t &offset) override;
private:
    static constexpr int32_t BLOCK_SIZE_BEST = 9;
    std::vector<uint8_t> &buffer_;
    size_t offset_;
    bz_stream stream_ {};
};

class BZip2ReadAdapter {
public:
    BZip2ReadAdapter(size_t offset, size_t length) : offset_(offset), dataLength_(length) {}
    virtual ~BZip2ReadAdapter()
    {
        Close();
    }

    virtual int32_t Open() = 0;
    virtual int32_t Close()
    {
        return 0;
    };
    virtual int32_t ReadData(BlockBuffer &info) = 0;
protected:
    bool init_ { false };
    size_t offset_ { 0 };
    size_t dataLength_ = { 0 };
    bz_stream stream_ {};
};

class BZip2BufferReadAdapter : public BZip2ReadAdapter {
public:
    BZip2BufferReadAdapter(size_t offset, size_t length, const BlockBuffer &info)
        : BZip2ReadAdapter(offset, length), buffer_(info) {}
    ~BZip2BufferReadAdapter() override {}

    int32_t Open() override;
    int32_t Close() override;

    int32_t ReadData(BlockBuffer &info) override;
private:
    BlockBuffer buffer_ {};
};
} // namespace updatepatch
#endif // BZIP2_ADAPTER_H