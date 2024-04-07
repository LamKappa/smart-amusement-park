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

#include "font/glyphs_manager.h"
#include "font/ui_font_builder.h"
#include "gfx_utils/file.h"
#include "securec.h"

namespace OHOS {
GlyphsManager::GlyphsManager()
    : binHeader_{{0}},
      fontNum_(0),
      start_(0),
      fontHeaderSectionStart_(0),
      fontIndexSectionStart_(0),
      curFontIndexSectionStart_(0),
      glyphNodeSectionStart_(0),
      curGlyphNodeSectionStart_(0),
      bitMapSectionStart_(0),
      curBitMapSectionStart_(0),
      ramAddr_(nullptr),
      ramUsedLen_(0),
      fontHeaderCache_(nullptr),
      indexCache_(nullptr),
      curIndexCache_(nullptr),
      nodeCache_(nullptr),
      cacheStatus_(nullptr),
      fp_(-1),
      curFontHeader_(nullptr),
      curGlyphNode_(nullptr),
      isRamSet_(false),
      isFileSet_(false),
      isFontIdSet_(false)
{
    fontId_ = UIFontBuilder::GetInstance()->GetBitmapFontIdMax();
}
GlyphsManager::~GlyphsManager() {}

int8_t GlyphsManager::GlyphNodeCacheInit()
{
    uint32_t size = 0;
    for (int32_t i = 0; i < fontNum_; i++) {
        size += fontHeaderCache_[i].indexLen;
    }

    indexCache_ = ramAddr_ + ramUsedLen_;
    ramUsedLen_ += size;
    /* align up to 4 byte, power of 2 */
    ramUsedLen_ = AlignUp(ramUsedLen_, 2);

    int32_t ret = read(fp_, indexCache_, size);
    if (ret != static_cast<int32_t>(size)) {
        return INVALID_RET_VALUE;
    }

    cacheStatus_ = reinterpret_cast<CacheState*>(ramAddr_ + ramUsedLen_);
    ramUsedLen_ += sizeof(CacheState);
    /* align up to 4 byte, power of 2 */
    ramUsedLen_ = AlignUp(ramUsedLen_, 2);
    for (int32_t font = 0; font < FONT_HASH_NR; font++) {
        for (int32_t uc = 0; uc < UNICODE_HASH_NR; uc++) {
            (*cacheStatus_)[font][uc] = 0;
        }
    }

    nodeCache_ = reinterpret_cast<CacheType*>(ramAddr_ + ramUsedLen_);
    ramUsedLen_ += sizeof(CacheType);
    /* align up to 4 byte, power of 2 */
    ramUsedLen_ = AlignUp(ramUsedLen_, 2);
    for (int32_t font = 0; font < FONT_HASH_NR; font++) {
        for (int32_t uc = 0; uc < UNICODE_HASH_NR; uc++) {
            for (int32_t node = 0; node < NODE_HASH_NR; node++) {
                (*nodeCache_)[font][uc][node].unicode = 0;
            }
        }
    }

    return RET_VALUE_OK;
}

GlyphNode* GlyphsManager::GetNodeFromCache(uint32_t unicode)
{
    GlyphNode* node = nullptr;

    uint8_t font = fontId_ & FONT_HASH_MASK;
    uint8_t uc = unicode & UNICODE_HASH_MASK;
    for (uint8_t i = 0; i < NODE_HASH_NR; i++) {
        GlyphNode* p = &((*nodeCache_)[font][uc][i]);
        if ((p->unicode == unicode) && (p->reserve == fontId_)) {
            node = p;
            break;
        }
    }
    return node;
}

GlyphNode* GlyphsManager::GetNodeCacheSpace(uint32_t unicode)
{
    uint8_t font, uc, i;
    GlyphNode* node = nullptr;

    font = fontId_ & FONT_HASH_MASK;
    uc = unicode & UNICODE_HASH_MASK;
    i = (*cacheStatus_)[font][uc];
    node = &((*nodeCache_)[font][uc][i]);

    i++;
    if (i >= NODE_HASH_NR) {
        i = 0;
    }
    (*cacheStatus_)[font][uc] = i;

    return node;
}

GlyphNode* GlyphsManager::GetNodeFromFile(uint32_t unicode)
{
    uint16_t idx = 0;
    uint8_t key;
    uint32_t offset;

    for (int32_t i = RADIX_SHIFT_START; i >= 0; i -= RADIX_TREE_BITS) {
        offset = idx * sizeof(IndexNode);
        key = static_cast<uint8_t>((unicode >> static_cast<uint8_t>(i)) & RADIX_TREE_MASK);
        offset += key * sizeof(uint16_t);
        idx = *(reinterpret_cast<uint16_t*>(curIndexCache_ + offset));
        if (idx == 0) {
            return nullptr;
        }
    }

    offset = curGlyphNodeSectionStart_ + (idx - 1) * sizeof(GlyphNode);
    int32_t ret = lseek(fp_, offset, SEEK_SET);
    if (ret != static_cast<int32_t>(offset)) {
        return nullptr;
    }
    GlyphNode* node = GetNodeCacheSpace(unicode);
    ret = read(fp_, node, sizeof(GlyphNode));
    if (ret < 0) {
        return nullptr;
    }

    return node;
}

void GlyphsManager::SetRamBuffer(uintptr_t ramAddr)
{
    ramAddr_ = reinterpret_cast<uint8_t*>(ramAddr);
    isRamSet_ = true;
}

int8_t GlyphsManager::SetFile(int32_t fp, uint32_t start)
{
    if (!isRamSet_) {
        return INVALID_RET_VALUE;
    }

    fp_ = fp;
    start_ = start;
    int32_t ret = lseek(fp_, start_, SEEK_SET);
    if (ret < 0) {
        return INVALID_RET_VALUE;
    }
    ret = read(fp_, &binHeader_, sizeof(binHeader_));
    if (ret != sizeof(binHeader_)) {
        return INVALID_RET_VALUE;
    }
    if (strncmp(binHeader_.fontMagic, FONT_MAGIC_NUMBER, FONT_MAGIC_NUM_LEN) != 0) {
        return INVALID_RET_VALUE;
    }
    if (binHeader_.fontNum > UIFontBuilder::GetInstance()->GetBitmapFontIdMax()) {
        return INVALID_RET_VALUE;
    }

    fontNum_ = binHeader_.fontNum;
    fontHeaderSectionStart_ = start_ + sizeof(binHeader_);
    int32_t size = sizeof(FontHeader) * fontNum_;
    fontIndexSectionStart_ = fontHeaderSectionStart_ + size;

    fontHeaderCache_ = reinterpret_cast<FontHeader*>(ramAddr_);
    /* align up to 4 byte, power of 2 */
    ramUsedLen_ = AlignUp(size, 2);

    ret = read(fp_, fontHeaderCache_, size);
    if (ret != size) {
        return INVALID_RET_VALUE;
    }

    FontHeader* last = fontHeaderCache_ + fontNum_ - 1;
    size = last->indexOffset + last->indexLen;
    glyphNodeSectionStart_ = fontIndexSectionStart_ + size;

    size = 0;
    for (uint32_t i = 0; i < fontNum_; i++) {
        size += fontHeaderCache_[i].glyphNum * sizeof(GlyphNode);
    }
    bitMapSectionStart_ = glyphNodeSectionStart_ + size;
    ret = GlyphNodeCacheInit();
    if (ret == RET_VALUE_OK) {
        isFileSet_ = true;
    }

    fontId_ = UIFontBuilder::GetInstance()->GetBitmapFontIdMax();
    return ret;
}

int8_t GlyphsManager::SetCurrentFontId(uint8_t fontId)
{
    uint16_t fontIdx = 0;
    if (!isFileSet_) {
        return INVALID_RET_VALUE;
    }
    if (fontId > UIFontBuilder::GetInstance()->GetBitmapFontIdMax()) {
        return INVALID_RET_VALUE;
    }
    if (fontId_ == fontId) {
        return RET_VALUE_OK;
    }

    int32_t low = 0;
    int32_t high = binHeader_.fontNum - 1;
    bool found = false;

    while (low <= high) {
        int32_t mid = (low + high) / 2; // 2 means half
        if (fontHeaderCache_[mid].fontId == fontId) {
            fontIdx = mid;
            found = true;
            break;
        } else if (fontHeaderCache_[mid].fontId > fontId) {
            high = mid - 1;
        } else if (fontHeaderCache_[mid].fontId < fontId) {
            low = mid + 1;
        }
    }
    if (!found) {
        isFontIdSet_ = false;
        curFontHeader_ = nullptr;
        fontId_ = UIFontBuilder::GetInstance()->GetBitmapFontIdMax();
        return INVALID_RET_VALUE;
    }

    uint32_t size = 0;
    fontId_ = fontId;
    curFontHeader_ = fontHeaderCache_ + fontIdx;
    curGlyphNode_ = nullptr;
    curFontIndexSectionStart_ = fontIndexSectionStart_ + curFontHeader_->indexOffset;
    for (uint32_t i = 0; i < fontIdx; i++) {
        size += fontHeaderCache_[i].glyphNum * sizeof(GlyphNode);
    }
    curGlyphNodeSectionStart_ = glyphNodeSectionStart_ + size;
    curBitMapSectionStart_ = bitMapSectionStart_ + curFontHeader_->glyphOffset;
    curIndexCache_ = indexCache_ + curFontHeader_->indexOffset;
    isFontIdSet_ = true;

    return RET_VALUE_OK;
}

int32_t GlyphsManager::GetRamUsedLen() const
{
    if (!isFileSet_) {
        return INVALID_RET_VALUE;
    }
    return ramUsedLen_;
}

int8_t GlyphsManager::GetFontVersion(char* version, uint8_t len) const
{
    if (!isFileSet_ || (version == nullptr) || (len > FONT_VERSION_LEN)) {
        return INVALID_RET_VALUE;
    }
    if (memset_s(version, len, 0, len) != EOK) {
        return INVALID_RET_VALUE;
    }
    if (strcpy_s(version, len, binHeader_.fontVersion) != EOK) {
        return INVALID_RET_VALUE;
    }
    return RET_VALUE_OK;
}

const FontHeader* GlyphsManager::GetCurrentFontHeader() const
{
    if (!isFontIdSet_) {
        return nullptr;
    }

    if (curFontHeader_ == nullptr) {
        return nullptr;
    }

    return curFontHeader_;
}

const GlyphNode* GlyphsManager::GetGlyphNode(uint32_t unicode)
{
    if (!isFontIdSet_) {
        return nullptr;
    }

    if (curGlyphNode_ != nullptr) {
        if ((curGlyphNode_->unicode == unicode) && (curGlyphNode_->reserve == fontId_)) {
            return curGlyphNode_;
        }
    }
    GlyphNode* node = GetNodeFromCache(unicode);
    if (node == nullptr) {
        node = GetNodeFromFile(unicode);
        if (node != nullptr) {
            node->reserve = fontId_;
        }
    }

    curGlyphNode_ = node;
    return node;
}

int16_t GlyphsManager::GetFontHeight() const
{
    if (!isFontIdSet_) {
        return INVALID_RET_VALUE;
    }

    if (curFontHeader_ == nullptr) {
        return INVALID_RET_VALUE;
    }

    return curFontHeader_->fontHeight;
}

int16_t GlyphsManager::GetFontWidth(uint32_t unicode)
{
    const GlyphNode* node = nullptr;

    if (!isFontIdSet_) {
        return INVALID_RET_VALUE;
    }
    node = GetGlyphNode(unicode);
    if (node == nullptr) {
        return INVALID_RET_VALUE;
    }
    return node->advance;
}

int8_t GlyphsManager::GetBitmap(uint32_t unicode, uint8_t* bitmap)
{
    if (bitmap == nullptr) {
        return INVALID_RET_VALUE;
    }
    if (!isFontIdSet_) {
        return INVALID_RET_VALUE;
    }

    const GlyphNode* node = GetGlyphNode(unicode);
    if (node == nullptr) {
        return INVALID_RET_VALUE;
    }

    uint32_t offset = curBitMapSectionStart_ + node->dataOff;
    uint32_t size = node->kernOff - node->dataOff;
    int32_t ret = lseek(fp_, offset, SEEK_SET);
    if (ret != static_cast<int32_t>(offset)) {
        return INVALID_RET_VALUE;
    }

    int32_t readSize = read(fp_, bitmap, size);
    if (readSize != static_cast<int32_t>(size)) {
        return INVALID_RET_VALUE;
    }

    return RET_VALUE_OK;
}
} // namespace OHOS
