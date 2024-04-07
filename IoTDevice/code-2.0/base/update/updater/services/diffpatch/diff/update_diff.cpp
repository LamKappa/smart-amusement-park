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

#include "update_diff.h"
#include <cstdlib>
#include <memory>
#include "image_diff.h"
#include "pkg_manager.h"

using namespace hpackage;

namespace updatepatch {
ImageParser::~ImageParser()
{
    hpackage::PkgManager::ReleasePackageInstance(pkgManager_);
    pkgManager_ = nullptr;
}

int32_t ImageParser::GetPkgBuffer(BlockBuffer &buffer) const
{
    int32_t ret = -1;
    hpackage::PkgBuffer pkgBuffer {};
    if (stream_ != nullptr) {
        ret = stream_->GetBuffer(pkgBuffer);
        buffer.buffer = pkgBuffer.buffer;
        buffer.length = pkgBuffer.length;
    }
    return ret;
}

const hpackage::FileInfo *ImageParser::GetFileInfo(const std::string &fileName) const
{
    if (pkgManager_ != nullptr) {
        return pkgManager_->GetFileInfo(fileName);
    }
    return nullptr;
}

int32_t ImageParser::Parse(const std::string &packageName)
{
    pkgManager_ = hpackage::PkgManager::CreatePackageInstance();
    PATCH_CHECK(pkgManager_ != nullptr, return PATCH_INVALID_PARAM, "Failed to get pkg manager");
    int32_t ret = PatchMapFile(packageName, memMap_);
    PATCH_CHECK(ret == 0, return -1, "Failed to read file");

    PkgBuffer buffer {memMap_.memory, memMap_.length};
    ret = pkgManager_->CreatePkgStream(stream_, packageName, buffer);
    PATCH_CHECK(ret == 0, return -1, "Failed to create pkg stream");

    // parse img and get image type
    type_ = PKG_PACK_TYPE_ZIP;
    ret = pkgManager_->ParsePackage(stream_, fileIds_, type_);
    if (ret == 0) {
        return 0;
    }
    type_ = PKG_PACK_TYPE_LZ4;
    ret = pkgManager_->ParsePackage(stream_, fileIds_, type_);
    if (ret == 0) {
        return 0;
    }
    type_ = PKG_PACK_TYPE_GZIP;
    ret = pkgManager_->ParsePackage(stream_, fileIds_, type_);
    if (ret == 0) {
        return 0;
    }
    type_ = PKG_PACK_TYPE_NONE;
    return 0;
}

int32_t ImageParser::Extract(const std::string &fileName, std::vector<uint8_t> &buffer)
{
    PATCH_CHECK(pkgManager_ != nullptr, return PATCH_INVALID_PARAM, "Failed to get pkg manager");
    size_t bufferSize = 0;
    hpackage::PkgManager::StreamPtr outStream = nullptr;
    int32_t ret = pkgManager_->CreatePkgStream(outStream, fileName,
        [&buffer, &bufferSize](const PkgBuffer &data, size_t size,
            size_t start, bool isFinish, const void *context) ->int {
            if (isFinish) {
                return 0;
            }
            bufferSize += size;
            if ((start + bufferSize) > buffer.size()) {
                buffer.resize(IGMDIFF_LIMIT_UNIT * ((start + bufferSize) / IGMDIFF_LIMIT_UNIT + 1));
            }
            return memcpy_s(buffer.data() + start, buffer.size(), data.buffer, size);
        }, nullptr);
    PATCH_CHECK(ret == 0, return -1, "Failed to extract data");

    ret = pkgManager_->ExtractFile(fileName, outStream);
    pkgManager_->ClosePkgStream(outStream);

    const FileInfo *fileInfo = pkgManager_->GetFileInfo(fileName);
    PATCH_CHECK(fileInfo != nullptr, return -1, "Failed to get file info");
    PATCH_CHECK(fileInfo->unpackedSize == bufferSize, return -1,
        "Failed to check uncompress data size %zu %zu", fileInfo->unpackedSize, bufferSize);
    return ret;
}

int32_t UpdateDiff::MakePatch(const std::string &oldFileName,
    const std::string &newFileName, const std::string &patchFileName)
{
    if (blockDiff_) {
        std::unique_ptr<BlocksDiff> blockDiff(new BlocksDiff());
        PATCH_CHECK(blockDiff != nullptr, return -1, "Failed to create block diff");
        return blockDiff->MakePatch(oldFileName, newFileName, patchFileName);
    }

    newParser_.reset(new ImageParser());
    oldParser_.reset(new ImageParser());
    PATCH_CHECK(newParser_ != nullptr, return -1, "Failed to create new parser");
    PATCH_CHECK(oldParser_ != nullptr, return -1, "Failed to create old parser");
    int32_t ret = newParser_->Parse(newFileName);
    ret |= oldParser_->Parse(oldFileName);
    PATCH_CHECK(ret == 0, return -1, "Failed to parse image");

    std::unique_ptr<ImageDiff> imageDiff = nullptr;
    PATCH_DEBUG("UpdateDiff::MakePatch type: %d %d", newParser_->GetType(), oldParser_->GetType());
    if (newParser_->GetType() != oldParser_->GetType()) {
        imageDiff.reset(new ImageDiff(limit_, newParser_.get(), oldParser_.get()));
        PATCH_CHECK(imageDiff != nullptr, return -1, "Failed to diff file");
        return imageDiff->MakePatch(patchFileName);
    }

    switch (newParser_->GetType()) {
        case PKG_PACK_TYPE_ZIP:
            imageDiff.reset(new ZipImageDiff(limit_, newParser_.get(), oldParser_.get()));
            break;
        case PKG_PACK_TYPE_LZ4:
            imageDiff.reset(new Lz4ImageDiff(limit_, newParser_.get(), oldParser_.get()));
            break;
        case PKG_PACK_TYPE_GZIP:
            imageDiff.reset(new GZipImageDiff(limit_, newParser_.get(), oldParser_.get()));
            break;
        default:
            imageDiff.reset(new ImageDiff(limit_, newParser_.get(), oldParser_.get()));
            break;
    }
    PATCH_CHECK(imageDiff != nullptr, return -1, "Failed to diff file");
    return imageDiff->MakePatch(patchFileName);
}

int32_t UpdateDiff::DiffImage(size_t limit, const std::string &oldFileName,
    const std::string &newFileName, const std::string &patchFileName)
{
    std::unique_ptr<UpdateDiff> updateDiff(new UpdateDiff(limit, false));
    PATCH_CHECK(updateDiff != nullptr, return -1, "Failed to create update diff");
    return updateDiff->MakePatch(oldFileName, newFileName, patchFileName);
}

int32_t UpdateDiff::DiffBlock(const std::string &oldFileName,
    const std::string &newFileName, const std::string &patchFileName)
{
    std::unique_ptr<UpdateDiff> updateDiff(new UpdateDiff(0, true));
    PATCH_CHECK(updateDiff != nullptr, return -1, "Failed to create update diff");
    return updateDiff->MakePatch(oldFileName, newFileName, patchFileName);
}
} // namespace updatepatch