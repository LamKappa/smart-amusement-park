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
#ifndef PKG_MANAGER_IMPL_H
#define PKG_MANAGER_IMPL_H

#include <functional>
#include <map>
#include <memory>
#include "pkg_lz4file.h"
#include "pkg_manager.h"
#include "pkg_pkgfile.h"
#include "pkg_stream.h"
#include "pkg_utils.h"

namespace hpackage {
class PkgManagerImpl : public PkgManager {
public:
    PkgManagerImpl() {};

    ~PkgManagerImpl() override;

    int32_t CreatePackage(const std::string &path, const std::string &keyName, PkgInfoPtr header,
        std::vector<std::pair<std::string, ZipFileInfo>> &files) override;

    int32_t CreatePackage(const std::string &path, const std::string &keyName, PkgInfoPtr header,
        std::vector<std::pair<std::string, ComponentInfo>> &files) override;

    int32_t CreatePackage(const std::string &path, const std::string &keyName, PkgInfoPtr header,
        std::vector<std::pair<std::string, Lz4FileInfo>> &files) override;

    int32_t CreatePackage(const std::string &path, PkgInfoPtr header,
        std::vector<std::pair<std::string, ComponentInfo>> &files, size_t &offset, std::string &hashValue) override;

    int32_t VerifyPackage(const std::string &packagePath, const std::string &keyPath, const std::string &version,
        const PkgBuffer &digest, VerifyCallback cb) override;

    int32_t LoadPackage(const std::string &packagePath, const std::string &keyPath,
        std::vector<std::string> &fileIds) override;

    int32_t ExtractFile(const std::string &path, PkgManager::StreamPtr output) override;

    int32_t CreatePkgStream(StreamPtr &stream, const std::string &, size_t, int32_t) override;

    int32_t CreatePkgStream(StreamPtr &stream, const std::string &fileName,
        PkgStream::ExtractFileProcessor processor, const void *context) override;

    void ClosePkgStream(StreamPtr &stream) override;

    const FileInfo *GetFileInfo(const std::string &path) override;

    const PkgInfo *GetPackageInfo(const std::string &packagePath) override;

    PkgEntryPtr GetPkgEntry(const std::string &path);

    int32_t DecompressBuffer(FileInfoPtr info, const PkgBuffer &buffer, StreamPtr stream) const override;

    int32_t CompressBuffer(FileInfoPtr info, const PkgBuffer &buffer, StreamPtr stream) const override;

    int32_t LoadPackageWithoutUnPack(const std::string &packagePath, std::vector<std::string> &fileIds) override;

    int32_t ParsePackage(StreamPtr stream, std::vector<std::string> &fileIds, int32_t type) override;

    int32_t CreatePkgStream(StreamPtr &stream, const std::string &fileName, const PkgBuffer &buffer) override;
private:
    PkgFilePtr CreatePackage(PkgStreamPtr stream, PkgFile::PkgType type, PkgInfoPtr header = nullptr);

    template<class T>
    PkgFilePtr CreatePackage(const std::string &path,
        PkgInfoPtr header, std::vector<std::pair<std::string, T>> &files, size_t &offset);

    int32_t ExtraAndLoadPackage(const std::string &path, const std::string &name, PkgFile::PkgType,
        std::vector<std::string> &fileIds);
    int32_t LoadPackage(const std::string &path, std::vector<std::string> &fileNames, PkgFile::PkgType type);

    int32_t LoadPackageWithStream(const std::string &path, std::vector<std::string> &fileNames,
        PkgFile::PkgType type, PkgStreamPtr stream);

    PkgFile::PkgType GetPkgTypeByName(const std::string &path);

    int32_t Sign(PkgStreamPtr stream, size_t offset, const PkgInfoPtr &info);

    int32_t Verify(uint8_t digestMethod, const std::vector<uint8_t> &digest, const std::vector<uint8_t> &signature);

    int32_t GenerateFileDigest(PkgStreamPtr stream,
        uint8_t digestMethod, uint8_t flags, std::vector<std::vector<uint8_t>> &digestInfo, size_t hashBufferLen = 0);

    void ClearPkgFile();

    int32_t SetSignVerifyKeyName(const std::string &keyName);

    int32_t CreatePkgStream(PkgStreamPtr &stream, const std::string &, size_t, int32_t);

    int32_t CreatePkgStream(PkgStreamPtr &stream, const std::string &fileName,
        PkgStream::ExtractFileProcessor processor, const void *context);

    void ClosePkgStream(PkgStreamPtr &stream);
private:
    bool unzipToFile_ {true};
    std::vector<PkgFilePtr> pkgFiles_ {};
    std::map<std::string, PkgStreamPtr> pkgStreams_ {};
    std::string signVerifyKeyName_ {};
};
} // namespace hpackage
#endif // PKG_MANAGER_IMPL_H
