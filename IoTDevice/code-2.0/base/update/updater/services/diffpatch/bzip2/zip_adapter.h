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

#ifndef ZIP_ADAPTER_H
#define ZIP_ADAPTER_H
#include <iostream>
#include <vector>
#include "deflate_adapter.h"
#include "diffpatch.h"
#include "pkg_manager.h"
#include "securec.h"
#include "zlib.h"

namespace updatepatch {
class ZipAdapter : public DeflateAdapter {
public:
    ZipAdapter(UpdatePatchWriterPtr outStream, size_t offset, const hpackage::PkgManager::FileInfoPtr fileInfo);
    ~ZipAdapter() override {}

    int32_t Open() override;
    int32_t Close() override;

    int32_t WriteData(const BlockBuffer &srcData) override;
    int32_t FlushData(size_t &offset) override;
private:
    std::vector<uint8_t> buffer_;
    UpdatePatchWriterPtr outStream_;
    z_stream zstream_;
    size_t offset_;

    int32_t level_ {0};
    int32_t method_ {0};
    int32_t windowBits_ {0};
    int32_t memLevel_ {0};
    int32_t strategy_ {0};
};
} // namespace updatepatch
#endif // ZIP_ADAPTER_H