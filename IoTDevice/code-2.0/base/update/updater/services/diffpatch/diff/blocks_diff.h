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
#include "update_diff.h"

namespace updatepatch {
template<class DataType>
class SuffixArray {
public:
    SuffixArray() = default;
    ~SuffixArray() {}

    void Init(const BlockBuffer &oldInfo);
    int64_t Search(const BlockBuffer &newInfo,
        const BlockBuffer &oldInfo, int64_t st, int64_t en, int64_t &pos) const;
private:
    void InitBuckets(const BlockBuffer &oldInfo,
        std::vector<DataType> &buckets, std::vector<DataType> &suffixArrayTemp);
    void Split(std::vector<DataType> &suffixArrayTemp, DataType start, DataType len, DataType h);
    void SplitForLess(std::vector<DataType> &suffixArrayTemp, DataType start, DataType len, DataType h);
    int64_t MatchLength(const BlockBuffer &oldBuffer, const BlockBuffer &newBuffer) const;

    std::vector<DataType> suffixArray_;
};

class BlocksDiff {
public:
    BlocksDiff() = default;
    virtual ~BlocksDiff() {}

    int32_t MakePatch(const std::string &oldFileName,
        const std::string &newFileName, const std::string &patchFileName);
    int32_t MakePatch(const BlockBuffer &newInfo,
        const BlockBuffer &oldInfo, std::vector<uint8_t> &patchData);
private:
    int32_t GetCtrlDatas(const BlockBuffer &newInfo,
        const BlockBuffer &oldInfo, std::vector<ControlData> &controlDatas);
    int32_t WriteControlData(const std::vector<ControlData> controlDatas,
        std::vector<uint8_t> &patchData, size_t &patchSize) const;
    int32_t WriteDiffData(const std::vector<ControlData> controlDatas,
        std::vector<uint8_t> &patchData, size_t &patchSize) const;
    int32_t WriteExtraData(const std::vector<ControlData> controlDatas,
        std::vector<uint8_t> &patchData, size_t &patchSize) const;

    void ComputeOldScore(const BlockBuffer &newInfo,
        const BlockBuffer &oldInfo, int64_t &oldScore, int64_t &matchLen);
    void ComputeLength(const BlockBuffer &newInfo,
        const BlockBuffer &oldInfo, int64_t &lengthFront, int64_t &lengthBack);

    std::unique_ptr<SuffixArray<int32_t>> suffixArray_ {nullptr};
    std::vector<ControlData> controls_ {};
    int64_t matchPos_ { 0 };
    int64_t currentOffset_ { 0 };
    int64_t lastOffset_ { 0 };
    int64_t lastScan_ { 0 };
    int64_t lastPos_ { 0 };
};
} // namespace updatepatch
#endif // BLOCKS_DIFF_H