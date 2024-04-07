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

#ifndef UPDATE_DIFF_H
#define UPDATE_DIFF_H

#include <iostream>
#include <memory>
#include <vector>
#include "diffpatch.h"
#include "package/package.h"
#include "package/pkg_manager.h"
#include "securec.h"

namespace updatepatch {
class ImageParser {
public:
    ImageParser() = default;
    ~ImageParser();

    int32_t Parse(const std::string &packageName);
    int32_t Extract(const std::string &fileName, std::vector<uint8_t> &buffer);
    PkgPackType GetType() const
    {
        return type_;
    }
    const std::vector<std::string> &GetFileIds() const
    {
        return fileIds_;
    }
    int32_t GetPkgBuffer(BlockBuffer &buffer) const;
    const hpackage::FileInfo *GetFileInfo(const std::string &fileName) const;
private:
    PkgPackType type_ = PKG_PACK_TYPE_NONE;
    MemMapInfo memMap_ {};
    std::vector<std::string> fileIds_;
    hpackage::PkgManager::StreamPtr stream_ { nullptr };
    hpackage::PkgManager::PkgManagerPtr pkgManager_ { nullptr };
};

class UpdateDiff {
public:
    using ImageParserPtr = ImageParser *;
    UpdateDiff(size_t limit, bool blockDiff) : limit_(limit * IGMDIFF_LIMIT_UNIT), blockDiff_(blockDiff) {}
    ~UpdateDiff() {}

    int32_t MakePatch(const std::string &oldFileName, const std::string &newFileName, const std::string &patchFileName);

    static int32_t DiffImage(size_t limit, const std::string &oldFileName,
        const std::string &newFileName, const std::string &patchFileName);

    static int32_t DiffBlock(const std::string &oldFileName,
        const std::string &newFileName, const std::string &patchFileName);

private:
    size_t limit_ { 0 };
    bool blockDiff_ { true };
    std::unique_ptr<ImageParser> newParser_ { nullptr };
    std::unique_ptr<ImageParser> oldParser_ { nullptr };
};
} // namespace updatepatch
#endif // UPDATE_DIFF_H