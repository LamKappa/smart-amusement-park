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
#ifndef PKG_ALGORITHM_DEFLATE_H
#define PKG_ALGORITHM_DEFLATE_H

#include "pkg_algorithm.h"
#include "pkg_stream.h"
#include "pkg_utils.h"
#include "zlib.h"

namespace hpackage {
class PkgAlgoDeflate : public PkgAlgorithm {
public:
    explicit PkgAlgoDeflate(const ZipFileInfo &info)
    {
        level_ = info.level;
        method_ = info.method;
        windowBits_ = info.windowBits;
        memLevel_ = info.memLevel;
        strategy_ = info.strategy;
    }

    ~PkgAlgoDeflate() override {}

    int32_t Pack(const PkgStreamPtr inStream, const PkgStreamPtr outStream,
        PkgAlgorithmContext &context) override;

    int32_t Unpack(const PkgStreamPtr inStream,
        const PkgStreamPtr outStream, PkgAlgorithmContext &context) override;
private:
    int32_t UnpackCalculate(PkgAlgorithmContext &context, const PkgStreamPtr inStream,
        const PkgStreamPtr outStream, DigestAlgorithm::DigestAlgorithmPtr algorithm);

    int32_t InitStream(z_stream &zstream, bool zip, PkgBuffer &inBuffer, PkgBuffer &outBuffer);

    void ReleaseStream(z_stream &zstream, bool zip) const;

    int32_t DeflateData(const PkgStreamPtr outStream,
        z_stream &zstream, int32_t flush, PkgBuffer &outBuffer, size_t &destOffset) const;
private:
    int32_t level_ {0};
    int32_t method_ {0};
    int32_t windowBits_ {0};
    int32_t memLevel_ {0};
    int32_t strategy_ {0};
};
} // namespace hpackage
#endif
