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

#ifndef BLOCKS_DIFF_H
#define BLOCKS_DIFF_H

#include <iostream>
#include <vector>
#include "bzip2_adapter.h"
#include "diffpatch.h"
#include "pkg_manager.h"
#include "securec.h"

namespace updatepatch {
class BlocksPatch {
public:
    BlocksPatch() = delete;
    explicit BlocksPatch(const PatchBuffer &patchInfo) : patchInfo_(patchInfo) {}
    virtual ~BlocksPatch() {}

    int32_t ApplyPatch();
protected:
    int32_t ReadControlData(ControlData &ctrlData);

    virtual int32_t ReadHeader(int64_t &controlDataSize, int64_t &diffDataSize, int64_t &newSize);
    virtual int32_t RestoreDiffData(const ControlData &ctrlData) = 0;
    virtual int32_t RestoreExtraData(const ControlData &ctrlData) = 0;

    PatchBuffer patchInfo_ { nullptr };
    int64_t newSize_ = { 0 };
    int64_t oldOffset_ { 0 };
    int64_t newOffset_ { 0 };

    std::unique_ptr<BZip2ReadAdapter> controlDataReader_ { nullptr };
    std::unique_ptr<BZip2ReadAdapter> diffDataReader_ { nullptr };
    std::unique_ptr<BZip2ReadAdapter> extraDataReader_ { nullptr };
};

class BlocksBufferPatch : public BlocksPatch {
public:
    BlocksBufferPatch(const PatchBuffer &patchInfo, const BlockBuffer &oldInfo, std::vector<uint8_t> &newData)
        : BlocksPatch(patchInfo), oldInfo_(oldInfo), newData_(newData) {}
    ~BlocksBufferPatch() override {}

private:
    int32_t ReadHeader(int64_t &controlDataSize, int64_t &diffDataSize, int64_t &newSize) override;
    int32_t RestoreDiffData(const ControlData &ctrlData) override;
    int32_t RestoreExtraData(const ControlData &ctrlData) override;

    BlockBuffer oldInfo_ { nullptr, 0 };
    std::vector<uint8_t>  &newData_;
};

class BlocksStreamPatch : public BlocksPatch {
public:
    BlocksStreamPatch(const PatchBuffer &patchInfo,
        hpackage::PkgManager::StreamPtr stream, UpdatePatchWriterPtr writer)
        : BlocksPatch(patchInfo), stream_(stream), writer_(writer) {}
    ~BlocksStreamPatch() override {}
private:
    int32_t RestoreDiffData(const ControlData &ctrlData) override;
    int32_t RestoreExtraData(const ControlData &ctrlData) override;

    hpackage::PkgManager::StreamPtr stream_ { nullptr };
    UpdatePatchWriterPtr writer_ { nullptr };
};
} // namespace updatepatch
#endif // BLOCKS_DIFF_H