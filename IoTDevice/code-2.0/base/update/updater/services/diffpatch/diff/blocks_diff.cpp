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

#include "blocks_diff.h"
#include <cstdio>
#include <iostream>
#include <vector>
#include "update_diff.h"

using namespace hpackage;
using namespace std;

namespace updatepatch {
#define SWAP(a, b) auto swapTmp = (a); (a) = (b); (b) = swapTmp
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define SET_BUFFER(y, buffer, index) \
    (buffer)[index] = (y) % 256;  (y) -= (buffer)[index];  (y) =  (y) / 256

constexpr uint32_t BUCKET_SIZE = 256;
constexpr uint32_t PATCH_BUFFER = 10240;
constexpr uint32_t MULTIPLE_TWO = 2;
constexpr int64_t BLOCK_SCORE = 8;
constexpr int64_t MIN_LENGTH = 16;

static void WriteLE64(const BlockBuffer &buffer, int64_t value)
{
    int32_t index = 0;
    int64_t y = (value < 0) ? -value : value;
    SET_BUFFER(y, buffer.buffer, index);
    index++;
    SET_BUFFER(y, buffer.buffer, index);
    index++;
    SET_BUFFER(y, buffer.buffer, index);
    index++;
    SET_BUFFER(y, buffer.buffer, index);
    index++;
    SET_BUFFER(y, buffer.buffer, index);
    index++;
    SET_BUFFER(y, buffer.buffer, index);
    index++;
    SET_BUFFER(y, buffer.buffer, index);
    index++;
    SET_BUFFER(y, buffer.buffer, index);
    if (value < 0) {
        buffer.buffer[index] |= 0x80;
    }
}

int32_t BlocksDiff::MakePatch(const std::string &oldFileName, const std::string &newFileName,
    const std::string &patchFileName)
{
    PATCH_LOGI("BlocksDiff::MakePatch %s ", patchFileName.c_str());
    MemMapInfo oldBuffer {};
    MemMapInfo newBuffer {};
    int32_t ret = PatchMapFile(oldFileName, oldBuffer);
    ret |= PatchMapFile(newFileName, newBuffer);
    if (ret == PATCH_SUCCESS) {
        BlockBuffer newInfo = {newBuffer.memory, newBuffer.length};
        BlockBuffer oldInfo = {oldBuffer.memory, oldBuffer.length};
        std::vector<uint8_t> patchData;
        ret = MakePatch(newInfo, oldInfo, patchData);
        PATCH_CHECK(ret == PATCH_SUCCESS, return ret, "Failed to generate patch");
        ret = WriteDataToFile(patchFileName, patchData, patchData.size());
    }
    return ret;
}

int32_t BlocksDiff::MakePatch(const BlockBuffer &newInfo, const BlockBuffer &oldInfo,
    std::vector<uint8_t> &patchData)
{
    if (suffixArray_ == nullptr) {
        suffixArray_.reset(new SuffixArray<int32_t>());
        PATCH_CHECK(suffixArray_ != nullptr, return -1, "Failed to create SuffixArray");
        suffixArray_->Init(oldInfo);
    }

    patchData.resize(PATCH_BUFFER);
    std::vector<ControlData> controlDatas;
    int32_t ret = GetCtrlDatas(newInfo, oldInfo, controlDatas);
    PATCH_CHECK(ret == PATCH_SUCCESS, return ret, "Failed to get control data");

    // 生成补丁数据，使用bzip进行压缩
    ret = memcpy_s(patchData.data(), patchData.size(), BSDIFF_MAGIC.c_str(), BSDIFF_MAGIC.size());
    PATCH_CHECK(ret == 0, return ret, "Failed to copy magic");
    size_t patchSize = BSDIFF_MAGIC.size() + sizeof(uint64_t) + sizeof(uint64_t) + sizeof(uint64_t);

    size_t controlSize = patchSize;
    ret = WriteControlData(controlDatas, patchData, patchSize);
    PATCH_CHECK(ret == BZ_OK, return ret, "Failed to write diff data");
    controlSize = patchSize - controlSize;

    // 写diff数据
    size_t diffDataSize = patchSize;
    ret = WriteDiffData(controlDatas, patchData, patchSize);
    PATCH_CHECK(ret == BZ_OK, return ret, "Failed to write diff data");
    diffDataSize = patchSize - diffDataSize;

    size_t extraDataSize = patchSize;
    ret = WriteExtraData(controlDatas, patchData, patchSize);
    PATCH_CHECK(ret == BZ_OK, return ret, "Failed to write extra data");
    extraDataSize = patchSize - extraDataSize;

    PATCH_DEBUG("MakePatch patchSize:%zu controlSize:%zu diffDataSize:%zu, extraDataSize:%zu",
        patchSize, controlSize, diffDataSize, extraDataSize);

    // 修改为实际的长度
    patchData.resize(patchSize);
    BlockBuffer data = {patchData.data() + BSDIFF_MAGIC.size(), patchSize};
    WriteLE64(data, controlSize);
    BlockBuffer diffData = {patchData.data() + BSDIFF_MAGIC.size() + sizeof(uint64_t), patchSize};
    WriteLE64(diffData, diffDataSize);
    BlockBuffer newData = {patchData.data() + BSDIFF_MAGIC.size() + sizeof(uint64_t) + sizeof(uint64_t), patchSize};
    WriteLE64(newData, newInfo.length);
    return 0;
}

void BlocksDiff::ComputeOldScore(const BlockBuffer &newInfo,
    const BlockBuffer &oldInfo, int64_t &oldScore, int64_t &matchLen)
{
    int64_t newSize = static_cast<int64_t>(newInfo.length);
    for (int64_t begin = currentOffset_ += matchLen; currentOffset_ < newSize; currentOffset_++) {
        BlockBuffer newBuff = {newInfo.buffer + currentOffset_, newInfo.length - currentOffset_};
        matchLen = suffixArray_->Search(newBuff, { oldInfo.buffer, oldInfo.length }, 0, oldInfo.length, matchPos_);
        for (; begin < currentOffset_ + matchLen; begin++) {
            if ((begin + lastOffset_ < static_cast<int64_t>(oldInfo.length))
                && (oldInfo.buffer[begin + lastOffset_] == newInfo.buffer[begin])) {
                oldScore++;
            }
        }
        if (((matchLen == oldScore) && (matchLen != 0)) || (matchLen > (oldScore + BLOCK_SCORE))) {
            break;
        }
        if ((currentOffset_ + lastOffset_ < static_cast<int64_t>(oldInfo.length)) &&
            (oldInfo.buffer[currentOffset_ + lastOffset_] == newInfo.buffer[currentOffset_])) {
            oldScore--;
        }
    }
}

void BlocksDiff::ComputeLength(const BlockBuffer &newInfo,
    const BlockBuffer &oldInfo, int64_t &lengthFront, int64_t &lengthBack)
{
    lengthFront = 0;
    lengthBack = 0;
    int64_t i = 0;
    int64_t s = 0;
    int64_t tmp = 0;
    for (; ((lastScan_ + i) < currentOffset_) && ((lastPos_ + i) < static_cast<int64_t>(oldInfo.length));) {
        if (oldInfo.buffer[lastPos_ + i] == newInfo.buffer[lastScan_ + i]) {
            s++;
        }
        i++;
        if ((s * MULTIPLE_TWO - i) > (tmp * MULTIPLE_TWO - lengthFront)) {
            tmp = s;
            lengthFront = i;
        }
    }
    s = 0;
    tmp = 0;
    if (currentOffset_ < static_cast<int64_t>(newInfo.length)) {
        for (i = 1; (currentOffset_ >= lastScan_ + i) && (matchPos_ >= i); i++) {
            if (oldInfo.buffer[matchPos_ - i] == newInfo.buffer[currentOffset_ - i]) {
                s++;
            }
            if ((s * MULTIPLE_TWO - i) > (tmp * MULTIPLE_TWO - lengthBack)) {
                tmp = s;
                lengthBack = i;
            }
        }
    }

    if (lastScan_ + lengthFront > currentOffset_ - lengthBack) {
        int64_t lens = 0;
        int64_t overlap = (lastScan_ + lengthFront) - (currentOffset_ - lengthBack);
        s = 0;
        tmp = 0;
        for (i = 0; i < overlap; i++) {
            if (newInfo.buffer[lastScan_ + lengthFront - overlap + i] ==
                oldInfo.buffer[lastPos_ + lengthFront - overlap + i]) {
                s++;
            }
            if (newInfo.buffer[currentOffset_ - lengthBack + i] == oldInfo.buffer[matchPos_ - lengthBack + i]) {
                s--;
            }
            if (s > tmp) {
                tmp = s;
                lens = i + 1;
            }
        }
        lengthFront += lens - overlap;
        lengthBack -= lens;
    }
}

int32_t BlocksDiff::GetCtrlDatas(const BlockBuffer &newInfo,
    const BlockBuffer &oldInfo, std::vector<ControlData> &controlDatas)
{
    int64_t matchLen = 0;
    while (currentOffset_ < static_cast<int64_t>(newInfo.length)) {
        int64_t oldScore = 0;
        int64_t lenFront = 0;
        int64_t lenBack = 0;
        ComputeOldScore(newInfo, oldInfo, oldScore, matchLen);
        if ((matchLen == oldScore) && (currentOffset_ != static_cast<int64_t>(newInfo.length))) {
            continue;
        }
        ComputeLength(newInfo, oldInfo, lenFront, lenBack);

        // save ctrl data
        ControlData ctrlData;
        ctrlData.diffLength = lenFront;
        ctrlData.extraLength = (currentOffset_ - lenBack) - (lastScan_ + lenFront);
        ctrlData.offsetIncrement = (matchPos_ - lenBack) - (lastPos_ + lenFront);
        ctrlData.diffNewStart = &newInfo.buffer[lastScan_];
        ctrlData.diffOldStart = &oldInfo.buffer[lastPos_];
        ctrlData.extraNewStart = &newInfo.buffer[lastScan_ + lenFront];
        controlDatas.push_back(ctrlData);
        lastScan_ = currentOffset_ - lenBack;
        lastPos_ = matchPos_ - lenBack;
        lastOffset_ = matchPos_ - currentOffset_;
    }
    return 0;
}

int32_t BlocksDiff::WriteControlData(const std::vector<ControlData> controlDatas,
    std::vector<uint8_t> &patchData, size_t &patchSize) const
{
    std::unique_ptr<BZip2Adapter> bzip2Adapter = std::make_unique<BZip2Adapter>(patchData, patchSize);
    PATCH_CHECK(bzip2Adapter != nullptr, return -1, "Failed to create bzip2Adapter");
    bzip2Adapter->Open();
    int32_t ret = 0;
    uint8_t buffer[sizeof(int64_t)] = {0};
    BlockBuffer srcData = {buffer, sizeof(int64_t)};
    PATCH_DEBUG("WriteControlData patchSize %zu controlDatas %zu", patchSize, controlDatas.size());
    std::vector<int64_t> data;
    for (size_t i = 0; i < controlDatas.size(); i++) {
        WriteLE64(srcData, controlDatas[i].diffLength);
        ret = bzip2Adapter->WriteData(srcData);
        PATCH_CHECK(ret == 0, return ret, "Failed to write data");
        WriteLE64(srcData, controlDatas[i].extraLength);
        ret = bzip2Adapter->WriteData(srcData);
        PATCH_CHECK(ret == 0, return ret, "Failed to write data");
        WriteLE64(srcData, controlDatas[i].offsetIncrement);
        ret = bzip2Adapter->WriteData(srcData);
        PATCH_CHECK(ret == 0, return ret, "Failed to write data");
    }
    ret = bzip2Adapter->FlushData(patchSize);
    bzip2Adapter->Close();
    PATCH_DEBUG("WriteControlData exit patchSize %zu", patchSize);
    return 0;
}

int32_t BlocksDiff::WriteDiffData(const std::vector<ControlData> controlDatas,
    std::vector<uint8_t> &patchData, size_t &patchSize) const
{
    PATCH_DEBUG("WriteDiffData patchSize %zu", patchSize);
    shared_ptr<BZip2Adapter> bzip2Adapter = make_shared<BZip2Adapter>(patchData, patchSize);
    PATCH_CHECK(bzip2Adapter != nullptr, return -1, "Failed to create bzip2Adapter");
    bzip2Adapter->Open();

    std::vector<uint8_t> diffData;
    int32_t ret = 0;
    for (size_t i = 0; i < controlDatas.size(); i++) {
        if (controlDatas[i].diffLength <= 0) {
            continue;
        }

        if (diffData.size() < static_cast<size_t>(controlDatas[i].diffLength)) {
            diffData.resize(controlDatas[i].diffLength);
        }
        for (int64_t k = 0; k < controlDatas[i].diffLength; k++) {
            diffData[k] = controlDatas[i].diffNewStart[k] - controlDatas[i].diffOldStart[k];
        }

        BlockBuffer srcData = {
            reinterpret_cast<uint8_t*>(diffData.data()), static_cast<size_t>(controlDatas[i].diffLength)
        };
        ret = bzip2Adapter->WriteData(srcData);
        PATCH_CHECK(ret == 0, return ret, "Failed to write data");
    }
    ret = bzip2Adapter->FlushData(patchSize);
    bzip2Adapter->Close();
    PATCH_DEBUG("WriteDiffData exit patchSize %zu ", patchSize);
    return 0;
}

int32_t BlocksDiff::WriteExtraData(const std::vector<ControlData> controlDatas,
    std::vector<uint8_t> &patchData, size_t &patchSize) const
{
    PATCH_DEBUG("WriteExtraData patchSize %zu ", patchSize);
    shared_ptr<BZip2Adapter> bzip2Adapter = make_shared<BZip2Adapter>(patchData, patchSize);
    PATCH_CHECK(bzip2Adapter != nullptr, return -1, "Failed to create bzip2Adapter");
    bzip2Adapter->Open();
    int32_t ret = 0;
    for (size_t i = 0; i < controlDatas.size(); i++) {
        if (controlDatas[i].extraLength <= 0) {
            continue;
        }
        BlockBuffer srcData = {controlDatas[i].extraNewStart, static_cast<size_t>(controlDatas[i].extraLength)};
        ret = bzip2Adapter->WriteData(srcData);
        PATCH_CHECK(ret == 0, return ret, "Failed to write data");
    }
    ret = bzip2Adapter->FlushData(patchSize);
    bzip2Adapter->Close();
    PATCH_DEBUG("WriteExtraData exit patchSize %zu", patchSize);
    return 0;
}

template<class DataType>
void SuffixArray<DataType>::Init(const BlockBuffer &oldInfo)
{
    std::vector<DataType> suffixArrayTemp;
    std::vector<DataType> buckets;
    InitBuckets(oldInfo, buckets, suffixArrayTemp);
    DataType i = 0;
    DataType h = 0;
    DataType len = 0;
    for (h = 1; suffixArray_[0] != -(static_cast<DataType>(oldInfo.length) + 1); h += h) {
        len = 0;
        for (i = 0; i < (static_cast<DataType>(oldInfo.length) + 1);) {
            if (suffixArray_[i] < 0) {
                len -= suffixArray_[i];
                i -= suffixArray_[i];
            } else {
                if (len) {
                    suffixArray_[i - len] = -len;
                }
                len = suffixArrayTemp[suffixArray_[i]] + 1 - i;
                Split(suffixArrayTemp, i, len, h);
                i += len;
                len = 0;
            }
        }
        if (len) {
            suffixArray_[i - len] = -len;
        }
    }

    for (i = 0; i < static_cast<DataType>(oldInfo.length) + 1; i++) {
        suffixArray_[suffixArrayTemp[i]] = i;
    }

    PATCH_DEBUG("SuffixArray::Init %d finish", static_cast<int>(oldInfo.length));
}

template<class DataType>
void SuffixArray<DataType>::SplitForLess(std::vector<DataType> &suffixArrayTemp,
    DataType start, DataType len, DataType h)
{
    DataType j = 0;
    for (DataType k = start; k < start + len; k += j) {
        j = 1;
        DataType x = suffixArrayTemp[suffixArray_[k] + h];
        for (DataType i = 1; k + i < start + len; i++) {
            if (suffixArrayTemp[suffixArray_[k + i] + h] < x) {
                x = suffixArrayTemp[suffixArray_[k + i] + h];
                j = 0;
            }
            if (suffixArrayTemp[suffixArray_[k + i] + h] == x) {
                SWAP(suffixArray_[k + j], suffixArray_[k + i]);
                j++;
            }
        }
        for (DataType i = 0; i < j; i++) {
            suffixArrayTemp[suffixArray_[k + i]] = k + j - 1;
        }
        if (j == 1) {
            suffixArray_[k] = -1;
        }
    }
}

template<class DataType>
void SuffixArray<DataType>::Split(std::vector<DataType> &suffixArrayTemp, DataType start, DataType len, DataType h)
{
    if (len < MIN_LENGTH) {
        SplitForLess(suffixArrayTemp, start, len, h);
        return;
    }

    DataType x = suffixArrayTemp[suffixArray_[start + len / MULTIPLE_TWO] + h];
    DataType jj = 0;
    DataType kk = 0;
    for (DataType i = start; i < (start + len); i++) {
        jj = (suffixArrayTemp[suffixArray_[i] + h] < x) ? (jj + 1) : jj;
        kk = (suffixArrayTemp[suffixArray_[i] + h] == x) ? (kk + 1) : kk;
    }
    jj += start;
    kk += jj;
    DataType i = start;
    DataType j = 0;
    DataType k = 0;
    while (i < jj) {
        if (suffixArrayTemp[suffixArray_[i] + h] < x) {
            i++;
        } else if (suffixArrayTemp[suffixArray_[i] + h] == x) {
            SWAP(suffixArray_[i], suffixArray_[jj + j]);
            j++;
        } else {
            SWAP(suffixArray_[i], suffixArray_[kk + k]);
            k++;
        }
    }
    while (jj + j < kk) {
        if (suffixArrayTemp[suffixArray_[jj + j] + h] == x) {
            j++;
        } else {
            SWAP(suffixArray_[jj + j], suffixArray_[kk + k]);
            k++;
        }
    }
    if (jj > start) {
        Split(suffixArrayTemp, start, jj - start, h);
    }

    for (i = 0; i < kk - jj; i++) {
        suffixArrayTemp[suffixArray_[jj + i]] = kk - 1;
    }
    if (jj == kk - 1) {
        suffixArray_[jj] = -1;
    }
    if (start + len > kk) {
        Split(suffixArrayTemp, kk, start + len - kk, h);
    }
}

template<class DataType>
int64_t SuffixArray<DataType>::MatchLength(const BlockBuffer &oldBuffer, const BlockBuffer &newBuffer) const
{
    int64_t i = 0;
    for (; (i < static_cast<int64_t>(oldBuffer.length)) && (i < static_cast<int64_t>(newBuffer.length)); i++) {
        if (oldBuffer.buffer[i] != newBuffer.buffer[i]) {
            break;
        }
    }
    return i;
}

template<class DataType>
int64_t SuffixArray<DataType>::Search(const BlockBuffer &newInfo,
    const BlockBuffer &oldInfo, int64_t start, int64_t end, int64_t &pos) const
{
    int64_t x = 0;
    int64_t y = 0;
    if ((end - start) < MULTIPLE_TWO) {
        BlockBuffer oldStart = {oldInfo.buffer + suffixArray_[start], oldInfo.length - suffixArray_[start]};
        BlockBuffer oldEnd = {oldInfo.buffer + suffixArray_[end], oldInfo.length - suffixArray_[end]};
        x = MatchLength(oldStart, newInfo);
        y = MatchLength(oldEnd, newInfo);
        if (x > y) {
            pos = suffixArray_[start];
            return x;
        } else {
            pos = suffixArray_[end];
            return y;
        }
    }
    x = start + (end - start) / MULTIPLE_TWO;
    if (memcmp(oldInfo.buffer + suffixArray_[x],
        newInfo.buffer, MIN(oldInfo.length - suffixArray_[x], newInfo.length)) < 0) {
        return Search(newInfo, oldInfo, x, end, pos);
    } else {
        return Search(newInfo, oldInfo, start, x, pos);
    }
}

template<class DataType>
void SuffixArray<DataType>::InitBuckets(const BlockBuffer &oldInfo,
    std::vector<DataType> &buckets, std::vector<DataType> &suffixArrayTemp)
{
    suffixArray_.resize(oldInfo.length + 1, 0);
    suffixArrayTemp.resize(oldInfo.length + 1, 0);
    buckets.resize(BUCKET_SIZE, 0);

    for (size_t i = 0; i < oldInfo.length; i++) {
        buckets[oldInfo.buffer[i]]++;
    }
    for (size_t i = 1; i < buckets.size(); i++) {
        buckets[i] += buckets[i - 1];
    }
    for (size_t i = buckets.size() - 1; i > 0; i--) {
        buckets[i] = buckets[i - 1];
    }
    buckets[0] = 0;

    DataType i;
    for (i = 0; i < static_cast<DataType>(oldInfo.length); i++) {
        suffixArray_[++buckets[oldInfo.buffer[i]]] = i;
    }
    suffixArray_[0] = oldInfo.length;

    for (i = 0; i < static_cast<DataType>(oldInfo.length); i++) {
        suffixArrayTemp[i] = buckets[oldInfo.buffer[i]];
    }
    suffixArrayTemp[oldInfo.length] = 0;

    for (i = 1; i < static_cast<DataType>(BUCKET_SIZE); i++) {
        if (buckets[i] == buckets[i - 1] + 1) {
            suffixArray_[buckets[i]] = -1;
        }
    }
    suffixArray_[0] = -1;
}
} // namespace updatepatch