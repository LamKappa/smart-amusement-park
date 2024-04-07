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

#ifndef UPDATE_PATCH_H
#define UPDATE_PATCH_H
#include <fstream>
#include <iostream>
#include "package/pkg_manager.h"
#include "openssl/sha.h"

namespace updatepatch {
struct PatchParam {
    uint8_t* oldBuff;
    size_t oldSize;
    uint8_t* patch;
    size_t patchSize;
};

struct PatchBuffer {
    uint8_t *buffer;
    size_t start;
    size_t length;
};

using BlockBuffer = hpackage::PkgBuffer;

class UpdatePatchWriter {
public:
    UpdatePatchWriter() = default;
    virtual ~UpdatePatchWriter() {}

    virtual int32_t Init() = 0;
    virtual int32_t Write(size_t start, const BlockBuffer &buffer, size_t len) = 0;
    virtual int32_t Finish() = 0;
};

using UpdatePatchWriterPtr = UpdatePatchWriter *;

class UpdatePatch {
public:
    using ImageProcessor = std::function<int(size_t start, const BlockBuffer &data, size_t size)>;

    static int32_t ApplyImagePatch(const PatchParam &param, const std::vector<uint8_t> &bonusData,
        ImageProcessor writer, const std::string& expected);
    static int32_t ApplyImagePatch(const PatchParam &param,
        UpdatePatchWriterPtr writer, const std::vector<uint8_t> &bonusData);

    static int32_t ApplyBlockPatch(const PatchBuffer &patchInfo,
        const BlockBuffer &oldInfo, UpdatePatchWriterPtr writer);
    static int32_t ApplyBlockPatch(const PatchBuffer &patchInfo,
        const BlockBuffer &oldInfo, std::vector<uint8_t> &newData);
    static int32_t ApplyBlockPatch(const PatchBuffer &patchInfo,
        hpackage::PkgManager::StreamPtr stream, UpdatePatchWriterPtr writer);
    static int32_t ApplyPatch(const std::string &patchName, const std::string &oldfile, const std::string &newFile);
};

class FilePatchWriter : public UpdatePatchWriter {
public:
    FilePatchWriter(const std::string &newFileName) : UpdatePatchWriter(), newFileName_(newFileName) {}
    ~FilePatchWriter() override {}

    int32_t Init() override;
    int32_t Write(size_t start, const BlockBuffer &buffer, size_t len) override;
    int32_t Finish() override;
private:
    bool init_ { false };
    std::string newFileName_;
    std::ofstream stream_;
};

class ImagePatchWriter : public UpdatePatchWriter {
public:
    ImagePatchWriter(UpdatePatch::ImageProcessor writer,
        const std::string &expected, const std::string &partitionName) : UpdatePatchWriter(),
        writer_(writer), expected_(expected), partitionName_(partitionName) {}
    ~ImagePatchWriter() override {}

    int32_t Init() override;
    int32_t Write(size_t start, const BlockBuffer &buffer, size_t len) override;
    int32_t Finish() override;
private:
    bool init_ { false };
    SHA256_CTX sha256Ctx_ {};
    UpdatePatch::ImageProcessor writer_;
    std::string expected_;
    std::string partitionName_;
};
} // namespace updatepatch
#endif // UPDATE_PATCH_H
