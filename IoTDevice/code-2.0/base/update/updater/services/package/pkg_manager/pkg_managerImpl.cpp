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
#include "pkg_manager_impl.h"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <functional>
#include <iterator>
#include <unistd.h>
#include <vector>
#include "pkg_gzipfile.h"
#include "pkg_lz4file.h"
#include "pkg_manager.h"
#include "pkg_upgradefile.h"
#include "pkg_zipfile.h"
#include "securec.h"

using namespace std;

namespace hpackage {
constexpr int32_t BUFFER_SIZE = 4096;
constexpr int32_t DIGEST_INFO_NO_SIGN = 0;
constexpr int32_t DIGEST_INFO_HAS_SIGN = 1;
constexpr int32_t DIGEST_INFO_SIGNATURE = 2;
constexpr int32_t DIGEST_FLAGS_NO_SIGN = 1;
constexpr int32_t DIGEST_FLAGS_HAS_SIGN = 2;
constexpr int32_t DIGEST_FLAGS_SIGNATURE = 4;
constexpr uint32_t VERIFY_FINSH_PERCENT = 100;
constexpr uint32_t VERIFY_DIGEST_PERCENT = 50;

static PkgManagerImpl *g_pkgManagerInstance = nullptr;
PkgManager::PkgManagerPtr PkgManager::GetPackageInstance()
{
    if (g_pkgManagerInstance == nullptr) {
        g_pkgManagerInstance = new PkgManagerImpl();
    }
    return g_pkgManagerInstance;
}

PkgManager::PkgManagerPtr PkgManager::CreatePackageInstance()
{
    return new PkgManagerImpl();
}

void PkgManager::ReleasePackageInstance(PkgManager::PkgManagerPtr manager)
{
    if (manager == nullptr) {
        return;
    }
    if (g_pkgManagerInstance == manager) {
        delete g_pkgManagerInstance;
        g_pkgManagerInstance = nullptr;
    } else {
        delete manager;
    }
    manager = nullptr;
}

PkgManagerImpl::~PkgManagerImpl()
{
    ClearPkgFile();
}

void PkgManagerImpl::ClearPkgFile()
{
    auto iter = pkgFiles_.begin();
    while (iter != pkgFiles_.end()) {
        PkgFilePtr file = (*iter);
        delete file;
        iter = pkgFiles_.erase(iter);
    }
    auto iter1 = pkgStreams_.begin();
    while (iter1 != pkgStreams_.end()) {
        PkgStreamPtr stream = (*iter1).second;
        delete stream;
        iter1 = pkgStreams_.erase(iter1);
    }
}

int32_t PkgManagerImpl::CreatePackage(const std::string &path, const std::string &keyName, PkgInfoPtr header,
    std::vector<std::pair<std::string, ZipFileInfo>> &files)
{
    int32_t ret = SetSignVerifyKeyName(keyName);
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "Invalid keyname");
    PKG_CHECK(files.size() > 0 && header != nullptr, return PKG_INVALID_PARAM, "Invalid param");
    size_t offset = 0;
    PkgFilePtr pkgFile = CreatePackage<ZipFileInfo>(path, header, files, offset);
    if (pkgFile == nullptr) {
        return PKG_INVALID_FILE;
    }
    ret = Sign(pkgFile->GetPkgStream(), offset, header);
    delete pkgFile;
    return ret;
}

int32_t PkgManagerImpl::CreatePackage(const std::string &path, const std::string &keyName, PkgInfoPtr header,
    std::vector<std::pair<std::string, ComponentInfo>> &files)
{
    int32_t ret = SetSignVerifyKeyName(keyName);
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "Invalid keyname");
    PKG_CHECK(files.size() > 0 && header != nullptr, return PKG_INVALID_PARAM, "Invalid param");
    size_t offset = 0;
    PkgFilePtr pkgFile = CreatePackage<ComponentInfo>(path, header, files, offset);
    if (pkgFile == nullptr) {
        return PKG_INVALID_FILE;
    }
    ret = Sign(pkgFile->GetPkgStream(), offset, header);
    delete pkgFile;
    return ret;
}

int32_t PkgManagerImpl::CreatePackage(const std::string &path, const std::string &keyName, PkgInfoPtr header,
    std::vector<std::pair<std::string, Lz4FileInfo>> &files)
{
    int32_t ret = SetSignVerifyKeyName(keyName);
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "Invalid keyname");
    PKG_CHECK(files.size() == 1 && header != nullptr, return PKG_INVALID_PARAM, "Invalid param");
    size_t offset = 0;
    PkgFilePtr pkgFile = CreatePackage<Lz4FileInfo>(path, header, files, offset);
    if (pkgFile == nullptr) {
        return PKG_INVALID_FILE;
    }
    ret = Sign(pkgFile->GetPkgStream(), offset, header);
    delete pkgFile;
    return ret;
}

int32_t PkgManagerImpl::CreatePackage(const std::string &path, PkgInfoPtr header,
    std::vector<std::pair<std::string, ComponentInfo>> &files, size_t &offset, std::string &hashValue)
{
    PKG_CHECK(files.size() > 0 && header != nullptr, return PKG_INVALID_PARAM, "Invalid param");
    PkgFilePtr pkgFile = CreatePackage<ComponentInfo>(path, header, files, offset);
    if (pkgFile == nullptr) {
        return PKG_INVALID_FILE;
    }
    int32_t ret = 0;
    if (header->pkgFlags & PKG_SUPPORT_L1) {
        size_t digestLen = DigestAlgorithm::GetDigestLen(header->digestMethod);
        std::vector<std::vector<uint8_t>> digestInfos(DIGEST_INFO_SIGNATURE + 1);
        digestInfos[DIGEST_INFO_HAS_SIGN].resize(digestLen);
        ret = GenerateFileDigest(pkgFile->GetPkgStream(),
            header->digestMethod, DIGEST_FLAGS_HAS_SIGN, digestInfos, offset);
        PKG_CHECK(ret == PKG_SUCCESS,
            return ret, "Fail to generate signature %s", pkgFile->GetPkgStream()->GetFileName().c_str());
        hashValue = ConvertShaHex(digestInfos[DIGEST_INFO_HAS_SIGN]);
        PKG_LOGI("PkgManagerImpl::CreatePackage sign offset %zu ", offset);
    } else {
        ret = Sign(pkgFile->GetPkgStream(), offset, header);
    }
    delete pkgFile;
    return ret;
}

template<class T>
PkgFilePtr PkgManagerImpl::CreatePackage(const std::string &path, PkgInfoPtr header,
    std::vector<std::pair<std::string, T>> &files, size_t &offset)
{
    PkgStreamPtr stream = nullptr;
    int32_t ret = CreatePkgStream(stream, path, 0, PkgStream::PkgStreamType_Write);
    PKG_CHECK(ret == PKG_SUCCESS, return nullptr, "CreatePackage fail %s", path.c_str());

    PkgFilePtr pkgFile = CreatePackage(PkgStreamImpl::ConvertPkgStream(stream),
        static_cast<PkgFile::PkgType>(header->pkgType), header);
    PKG_CHECK(pkgFile != nullptr, ClosePkgStream(stream); return nullptr, "CreatePackage fail %s", path.c_str());

    PkgStreamPtr inputStream = nullptr;
    for (size_t i = 0; i < files.size(); i++) {
        ret = CreatePkgStream(inputStream, files[i].first, 0, PkgStream::PkgStreamType_Read);
        PKG_CHECK(ret == PKG_SUCCESS, break, "Create stream fail %s", files[i].first.c_str());
        ret = pkgFile->AddEntry(reinterpret_cast<const FileInfoPtr>(&(files[i].second)), inputStream);
        PKG_CHECK(ret == PKG_SUCCESS, break, "Add entry fail %s", files[i].first.c_str());
        ClosePkgStream(inputStream);
        inputStream = nullptr;
    }
    if (ret != PKG_SUCCESS) {
        ClosePkgStream(inputStream);
        delete pkgFile;
        return nullptr;
    }
    ret = pkgFile->SavePackage(offset);
    if (ret != PKG_SUCCESS) {
        delete pkgFile;
        return nullptr;
    }
    return pkgFile;
}

PkgFilePtr PkgManagerImpl::CreatePackage(PkgStreamPtr stream, PkgFile::PkgType type, PkgInfoPtr header)
{
    PkgFilePtr pkgFile = nullptr;
    switch (type) {
        case PkgFile::PKG_TYPE_UPGRADE:
            pkgFile = new UpgradePkgFile(stream, header);
            break;
        case PkgFile::PKG_TYPE_ZIP:
            pkgFile = new ZipPkgFile(stream);
            break;
        case PkgFile::PKG_TYPE_LZ4:
            pkgFile = new Lz4PkgFile(stream);
            break;
        case PkgFile::PKG_TYPE_GZIP:
            pkgFile = new GZipPkgFile(stream);
            break;
        default:
            return nullptr;
    }
    return pkgFile;
}

int32_t PkgManagerImpl::LoadPackageWithoutUnPack(const std::string &packagePath,
    std::vector<std::string> &fileIds)
{
    PkgFile::PkgType pkgType = GetPkgTypeByName(packagePath);
    int32_t ret = LoadPackage(packagePath, fileIds, pkgType);
    PKG_CHECK(ret == PKG_SUCCESS, ClearPkgFile(); return ret, "Parse %s fail ", packagePath.c_str());
    return PKG_SUCCESS;
}

int32_t PkgManagerImpl::ParsePackage(StreamPtr stream, std::vector<std::string> &fileIds, int32_t type)
{
    PKG_CHECK(stream != nullptr, return PKG_INVALID_PARAM, "Invalid stream");
    PkgFilePtr pkgFile = CreatePackage(static_cast<PkgStreamPtr>(stream), static_cast<PkgFile::PkgType>(type), nullptr);
    PKG_CHECK(pkgFile != nullptr, return PKG_INVALID_PARAM, "Create package fail %s", stream->GetFileName().c_str());

    int32_t ret = pkgFile->LoadPackage(fileIds,
        [this](const PkgInfoPtr info, const std::vector<uint8_t> &digest, const std::vector<uint8_t> &signature)->int {
            return PKG_SUCCESS;
        });
    PKG_CHECK(ret == PKG_SUCCESS, pkgFile->SetPkgStream(); delete pkgFile;
        return ret, "Load package fail %s", stream->GetFileName().c_str());
    pkgFiles_.push_back(pkgFile);
    return PKG_SUCCESS;
}

int32_t PkgManagerImpl::LoadPackage(const std::string &packagePath, const std::string &keyPath,
    std::vector<std::string> &fileIds)
{
    if (access(packagePath.c_str(), 0) != 0) {
        return PKG_INVALID_FILE;
    }
    int32_t ret = SetSignVerifyKeyName(keyPath);
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "Invalid keyname");

    // Check if package already loaded
    for (auto iter : pkgFiles_) {
        PkgFilePtr pkgFile = iter;
        if (pkgFile != nullptr && pkgFile->GetPkgStream()->GetFileName().compare(packagePath) == 0) {
            return PKG_SUCCESS;
        }
    }

    PkgFile::PkgType pkgType = GetPkgTypeByName(packagePath);
    ret = PKG_INVALID_FILE;
    unzipToFile_ = ((pkgType == PkgFile::PKG_TYPE_GZIP) ? true : unzipToFile_);
    if (pkgType == PkgFile::PKG_TYPE_UPGRADE) {
        ret = LoadPackage(packagePath, fileIds, pkgType);
        PKG_CHECK(ret == PKG_SUCCESS, ClearPkgFile(); return ret, "Parse %s fail ", packagePath.c_str());
    } else if (pkgType != PkgFile::PKG_TYPE_NONE) {
        std::vector<std::string> innerFileNames;
        ret = LoadPackage(packagePath, innerFileNames, pkgType);
        PKG_CHECK(ret == PKG_SUCCESS, ClearPkgFile(); return ret, "Unzip %s fail ", packagePath.c_str());

        std::string path = GetFilePath(packagePath);
        for (auto name : innerFileNames) {
            pkgType = GetPkgTypeByName(name);
            if (pkgType == PkgFile::PKG_TYPE_NONE) {
                fileIds.push_back(name);
                continue;
            }
            ret = ExtraAndLoadPackage(path, name, pkgType, fileIds);
            PKG_CHECK(ret == PKG_SUCCESS, ClearPkgFile();
                return ret, "unpack %s fail in package %s ", name.c_str(), packagePath.c_str());
        }
    }
    return PKG_SUCCESS;
}

int32_t PkgManagerImpl::ExtraAndLoadPackage(const std::string &path, const std::string &name,
    PkgFile::PkgType type, std::vector<std::string> &fileIds)
{
    int32_t ret = PKG_SUCCESS;
    const FileInfo *info = GetFileInfo(name);
    PKG_CHECK(info != nullptr, return PKG_INVALID_FILE, "Create middle stream fail %s", name.c_str());

    PkgStreamPtr stream = nullptr;
    // Extract package to file or memory
    if (unzipToFile_) {
        ret = CreatePkgStream(stream, path + name + ".tmp", info->unpackedSize, PkgStream::PkgStreamType_Write);
    } else {
        ret = CreatePkgStream(stream, path + name + ".tmp", info->unpackedSize, PkgStream::PkgStreamType_MemoryMap);
    }
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "Create middle stream fail %s", name.c_str());

    ret = ExtractFile(name, stream);
    PKG_CHECK(ret == PKG_SUCCESS, ClosePkgStream(stream); return ret, "Extract file fail %s", name.c_str());
    return LoadPackageWithStream(path, fileIds, type, stream);
}

int32_t PkgManagerImpl::LoadPackage(const std::string &packagePath, std::vector<std::string> &fileIds,
    PkgFile::PkgType type)
{
    PkgStreamPtr stream = nullptr;
    int32_t ret = CreatePkgStream(stream, packagePath, 0, PkgStream::PkgStreamType_Read);
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "Create input stream fail %s", packagePath.c_str());
    return LoadPackageWithStream(packagePath, fileIds, type, stream);
}

int32_t PkgManagerImpl::LoadPackageWithStream(const std::string &packagePath,
    std::vector<std::string> &fileIds, PkgFile::PkgType type, PkgStreamPtr stream)
{
    int32_t ret = PKG_SUCCESS;
    PkgFilePtr pkgFile = CreatePackage(stream, type, nullptr);
    PKG_CHECK(pkgFile != nullptr, ClosePkgStream(stream);
        return PKG_INVALID_PARAM, "Create package fail %s", packagePath.c_str());

    ret = pkgFile->LoadPackage(fileIds,
        [this](const PkgInfoPtr info, const std::vector<uint8_t> &digest, const std::vector<uint8_t> &signature)->int {
            return Verify(info->digestMethod, digest, signature);
        });

    PKG_CHECK(ret == PKG_SUCCESS, delete pkgFile; return ret, "Load package fail %s", packagePath.c_str());
    pkgFiles_.push_back(pkgFile);
    return PKG_SUCCESS;
}

int32_t PkgManagerImpl::ExtractFile(const std::string &name, PkgManager::StreamPtr output)
{
    PKG_CHECK(output != nullptr, return PKG_INVALID_STREAM, "Invalid stream");
    int32_t ret = PKG_INVALID_FILE;
    PkgEntryPtr pkgEntry = GetPkgEntry(name);
    if (pkgEntry != nullptr && pkgEntry->GetPkgFile() != nullptr) {
        ret = pkgEntry->GetPkgFile()->ExtractFile(pkgEntry, PkgStreamImpl::ConvertPkgStream(output));
    } else {
        PKG_LOGE("Can not find file %s", name.c_str());
    }
    return ret;
}

const PkgInfo *PkgManagerImpl::GetPackageInfo(const std::string &packagePath)
{
    for (auto iter : pkgFiles_) {
        PkgFilePtr pkgFile = iter;
        if (pkgFile != nullptr && pkgFile->GetPkgType() == PkgFile::PKG_TYPE_UPGRADE) {
            return pkgFile->GetPkgInfo();
        }
    }
    return nullptr;
}

const FileInfo *PkgManagerImpl::GetFileInfo(const std::string &path)
{
    PkgEntryPtr pkgEntry = GetPkgEntry(path);
    if (pkgEntry != nullptr) {
        return pkgEntry->GetFileInfo();
    }
    return nullptr;
}

PkgEntryPtr PkgManagerImpl::GetPkgEntry(const std::string &fileId)
{
    // Find out pkgEntry by fileId.
    for (auto iter : pkgFiles_) {
        PkgFilePtr pkgFile = iter;
        PkgEntryPtr pkgEntry = pkgFile->FindPkgEntry(fileId);
        if (pkgEntry == nullptr) {
            continue;
        }
        return pkgEntry;
    }
    return nullptr;
}

int32_t PkgManagerImpl::CreatePkgStream(StreamPtr &stream, const std::string &fileName, size_t size, int32_t type)
{
    PkgStreamPtr pkgStream;
    int32_t ret = CreatePkgStream(pkgStream, fileName, size, type);
    PKG_CHECK(pkgStream != nullptr, return -1, "Failed to create stream");
    stream = pkgStream;
    return ret;
}

int32_t PkgManagerImpl::CreatePkgStream(StreamPtr &stream, const std::string &fileName, const PkgBuffer &buffer)
{
    PkgStreamPtr pkgStream = new MemoryMapStream(fileName, buffer, PkgStream::PkgStreamType_Buffer);
    PKG_CHECK(pkgStream != nullptr, return -1, "Failed to create stream");
    stream = pkgStream;
    return PKG_SUCCESS;
}

int32_t PkgManagerImpl::CreatePkgStream(StreamPtr &stream, const std::string &fileName,
    PkgStream::ExtractFileProcessor processor, const void *context)
{
    PkgStreamPtr pkgStream;
    int32_t ret = CreatePkgStream(pkgStream, fileName, processor, context);
    stream = pkgStream;
    return ret;
}

void PkgManagerImpl::ClosePkgStream(StreamPtr &stream)
{
    PkgStreamPtr pkgStream = static_cast<PkgStreamPtr>(stream);
    ClosePkgStream(pkgStream);
    stream = nullptr;
}

int32_t PkgManagerImpl::CreatePkgStream(PkgStreamPtr &stream, const std::string &fileName, size_t size, int32_t type)
{
    if (type == PkgStream::PkgStreamType_Write || type == PkgStream::PkgStreamType_Read) {
        static char const *modeFlags[] = { "rb", "wb+" };
        int32_t ret = CheckFile(fileName);
        PKG_CHECK(ret == PKG_SUCCESS, return ret, "Fail to check file %s ", fileName.c_str());

        if (pkgStreams_.find(fileName) != pkgStreams_.end()) {
            PkgStreamPtr mapStream = pkgStreams_[fileName];
            mapStream->AddRef();
            stream = mapStream;
            return PKG_SUCCESS;
        }

        FILE *file = nullptr;
        file = fopen(fileName.c_str(), modeFlags[type]);
        PKG_CHECK(file != nullptr, return PKG_INVALID_FILE, "Fail to open file %s ", fileName.c_str());
        stream = new FileStream(fileName, file, type);
    } else if (type == PkgStream::PkgStreamType_MemoryMap) {
        size_t fileSize = size;
        if (fileSize == 0) {
            if (access(fileName.c_str(), 0) != 0) {
                return PKG_INVALID_FILE;
            }
            fileSize = GetFileSize(fileName);
        }
        PKG_CHECK(fileSize > 0, return PKG_INVALID_FILE, "Fail to check file size %s ", fileName.c_str());
        uint8_t *memoryMap = MapMemory(fileName, fileSize);
        PKG_CHECK(memoryMap != nullptr, return PKG_INVALID_FILE, "Fail to map memory %s ", fileName.c_str());
        PkgBuffer buffer(memoryMap, fileSize);
        stream = new MemoryMapStream(fileName, buffer);
    } else {
        return -1;
    }
    pkgStreams_[fileName] = stream;
    return PKG_SUCCESS;
}

int32_t PkgManagerImpl::CreatePkgStream(PkgStreamPtr &stream, const std::string &fileName,
    PkgStream::ExtractFileProcessor processor, const void *context)
{
    stream = new ProcessorStream(fileName, processor, context);
    PKG_CHECK(stream != nullptr, return -1, "Failed to create stream");
    return PKG_SUCCESS;
}

void PkgManagerImpl::ClosePkgStream(PkgStreamPtr &stream)
{
    PkgStreamPtr mapStream = stream;
    if (mapStream == nullptr) {
        return;
    }

    auto iter = pkgStreams_.find(mapStream->GetFileName());
    if (iter != pkgStreams_.end()) {
        mapStream->DelRef();
        if (mapStream->IsRef()) {
            return;
        }
        pkgStreams_.erase(iter);
    }
    delete mapStream;
    stream = nullptr;
}

PkgFile::PkgType PkgManagerImpl::GetPkgTypeByName(const std::string &path)
{
    int32_t pos = path.find_last_of('.');
    if (pos < 0) {
        return PkgFile::PKG_TYPE_NONE;
    }
    std::string postfix = path.substr(pos + 1, -1);
    std::transform(postfix.begin(), postfix.end(), postfix.begin(), ::tolower);

    if (path.substr(pos + 1, -1).compare("bin") == 0) {
        return PkgFile::PKG_TYPE_UPGRADE;
    } else if (path.substr(pos + 1, -1).compare("zip") == 0) {
        return PkgFile::PKG_TYPE_ZIP;
    } else if (path.substr(pos + 1, -1).compare("lz4") == 0) {
        return PkgFile::PKG_TYPE_LZ4;
    } else if (path.substr(pos + 1, -1).compare("gz") == 0) {
        return PkgFile::PKG_TYPE_GZIP;
    }
    return PkgFile::PKG_TYPE_NONE;
}

int32_t PkgManagerImpl::VerifyPackage(const std::string &packagePath, const std::string &keyPath,
    const std::string &version, const PkgBuffer &digest, VerifyCallback verifyCallback)
{
    int32_t ret = SetSignVerifyKeyName(keyPath);
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "Invalid keyname");

    PkgStreamPtr stream = nullptr;
    ret = CreatePkgStream(stream, packagePath, 0, PkgStream::PkgStreamType_Read);
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "Create input stream fail %s", packagePath.c_str());
    size_t fileLen = stream->GetFileLength();
    PKG_CHECK(fileLen > 0, ClosePkgStream(stream); return PKG_INVALID_FILE, "invalid file to load");

    PkgFile::PkgType type = GetPkgTypeByName(packagePath);
    int8_t digestMethod = DigestAlgorithm::GetDigestMethod(version);
    size_t digestLen = DigestAlgorithm::GetDigestLen(digestMethod);
    size_t signatureLen = DigestAlgorithm::GetSignatureLen(digestMethod);
    PKG_CHECK(digestLen == digest.length, return PKG_INVALID_PARAM, "Invalid digestLen");
    std::vector<std::vector<uint8_t>> digestInfos(DIGEST_INFO_SIGNATURE + 1);
    digestInfos[DIGEST_INFO_HAS_SIGN].resize(digestLen);
    digestInfos[DIGEST_INFO_NO_SIGN].resize(digestLen);
    digestInfos[DIGEST_INFO_SIGNATURE].resize(signatureLen);

    if (type != PkgFile::PKG_TYPE_UPGRADE) {
        ret = GenerateFileDigest(stream, digestMethod, 0xf, digestInfos); // Grab all data
        if ((digest.buffer != nullptr)
            && (memcmp(digestInfos[DIGEST_INFO_HAS_SIGN].data(), digest.buffer, digest.length) != 0)) {
            PKG_LOGE("Fail to verify package");
            ret = PKG_INVALID_SIGNATURE;
        }
        verifyCallback(ret, VERIFY_DIGEST_PERCENT);
        if (ret == PKG_SUCCESS) {
            ret = Verify(digestMethod, digestInfos[DIGEST_INFO_NO_SIGN], digestInfos[DIGEST_INFO_SIGNATURE]);
            verifyCallback(ret, VERIFY_FINSH_PERCENT);
        }
    } else if (digest.buffer != nullptr) {
        // update.bin include signature infomation, verify entire file.
        ret = GenerateFileDigest(stream, digestMethod, DIGEST_FLAGS_HAS_SIGN, digestInfos);
        if (memcmp(digestInfos[DIGEST_INFO_HAS_SIGN].data(), digest.buffer, digest.length) != 0) {
            PKG_LOGE("Fail to verify package %s", packagePath.c_str());
            ret = PKG_INVALID_SIGNATURE;
        }
        verifyCallback(ret, VERIFY_FINSH_PERCENT);
    }
    ClosePkgStream(stream);
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "Verify file %s fail", packagePath.c_str());
    PKG_LOGW("Verify file %s success", packagePath.c_str());
    return ret;
}

int32_t PkgManagerImpl::GenerateFileDigest(PkgStreamPtr stream,
    uint8_t digestMethod, uint8_t flags, std::vector<std::vector<uint8_t>> &digestInfos, size_t hashBufferLen)
{
    size_t fileLen = (hashBufferLen == 0) ? stream->GetFileLength() : hashBufferLen;
    size_t digestLen = DigestAlgorithm::GetDigestLen(digestMethod);
    size_t signatureLen = DigestAlgorithm::GetSignatureLen(digestMethod);
    // Check entire package
    DigestAlgorithm::DigestAlgorithmPtr algorithm = PkgAlgorithmFactory::GetDigestAlgorithm(digestMethod);
    PKG_CHECK(algorithm != nullptr, return PKG_NOT_EXIST_ALGORITHM, "Invalid file %s", stream->GetFileName().c_str());
    algorithm->Init();

    // Get verify algorithm
    DigestAlgorithm::DigestAlgorithmPtr algorithmInner = PkgAlgorithmFactory::GetDigestAlgorithm(digestMethod);
    PKG_CHECK(algorithm != nullptr, return PKG_NOT_EXIST_ALGORITHM, "Invalid file %s", stream->GetFileName().c_str());
    algorithmInner->Init();

    size_t offset = 0;
    size_t readLen = 0;
    size_t needReadLen = fileLen;
    size_t buffSize = BUFFER_SIZE;
    PkgBuffer buff(buffSize);
    if (flags & DIGEST_FLAGS_SIGNATURE) {
        PKG_ONLY_CHECK(SIGN_TOTAL_LEN < fileLen, return PKG_INVALID_SIGNATURE);
        needReadLen = fileLen - SIGN_TOTAL_LEN;
    }
    while (offset < needReadLen) {
        PKG_ONLY_CHECK((needReadLen - offset) >= buffSize, buffSize = needReadLen - offset);
        int32_t ret = stream->Read(buff, offset, buffSize, readLen);
        PKG_CHECK(ret == PKG_SUCCESS, return ret, "read buffer fail %s", stream->GetFileName().c_str());
        PKG_IS_TRUE_DONE(flags & DIGEST_FLAGS_HAS_SIGN, algorithm->Update(buff, readLen));
        PKG_IS_TRUE_DONE(flags & DIGEST_FLAGS_NO_SIGN, algorithmInner->Update(buff, readLen));
        offset += readLen;
        readLen = 0;
    }

    // Read last signatureLen
    if (flags & DIGEST_FLAGS_SIGNATURE) {
        readLen = 0;
        int32_t ret = stream->Read(buff, offset, SIGN_TOTAL_LEN, readLen);
        PKG_CHECK(ret == PKG_SUCCESS, return ret, "read buffer failed %s", stream->GetFileName().c_str());
        PKG_IS_TRUE_DONE(flags & DIGEST_FLAGS_HAS_SIGN, algorithm->Update(buff, readLen));
        PkgBuffer data(SIGN_TOTAL_LEN);
        PKG_IS_TRUE_DONE(flags & DIGEST_FLAGS_NO_SIGN, algorithmInner->Update(data, SIGN_TOTAL_LEN));
    }
    PKG_IS_TRUE_DONE(flags & DIGEST_FLAGS_HAS_SIGN,
        PkgBuffer result(digestInfos[DIGEST_INFO_HAS_SIGN].data(), digestLen); algorithm->Final(result));
    PKG_IS_TRUE_DONE(flags & DIGEST_FLAGS_NO_SIGN,
        PkgBuffer result(digestInfos[DIGEST_INFO_NO_SIGN].data(), digestLen); algorithmInner->Final(result));
    if (flags & DIGEST_FLAGS_SIGNATURE) {
        if (digestMethod == PKG_DIGEST_TYPE_SHA256) {
            PKG_CHECK(!memcpy_s(digestInfos[DIGEST_INFO_SIGNATURE].data(), signatureLen, buff.buffer, signatureLen),
                return PKG_NONE_MEMORY, "GenerateFileDigest memcpy failed");
        } else {
            PKG_CHECK(!memcpy_s(digestInfos[DIGEST_INFO_SIGNATURE].data(), signatureLen, buff.buffer + SIGN_SHA256_LEN,
                signatureLen), return PKG_NONE_MEMORY, "GenerateFileDigest memcpy failed");
        }
    }
    return PKG_SUCCESS;
}

int32_t PkgManagerImpl::Verify(uint8_t digestMethod, const std::vector<uint8_t> &digest,
    const std::vector<uint8_t> &signature)
{
    SignAlgorithm::SignAlgorithmPtr signAlgorithm = PkgAlgorithmFactory::GetVerifyAlgorithm(
        signVerifyKeyName_, digestMethod);
    PKG_CHECK(signAlgorithm != nullptr, return PKG_INVALID_SIGNATURE, "Invalid sign algo");
    return signAlgorithm->VerifyBuffer(digest, signature);
}

int32_t PkgManagerImpl::Sign(PkgStreamPtr stream, size_t offset, const PkgInfoPtr &info)
{
    PKG_CHECK(info != nullptr, return PKG_INVALID_PARAM, "Invalid param");
    if (info->signMethod == PKG_SIGN_METHOD_NONE) {
        return PKG_SUCCESS;
    }

    size_t digestLen = DigestAlgorithm::GetDigestLen(info->digestMethod);
    std::vector<std::vector<uint8_t>> digestInfos(DIGEST_INFO_SIGNATURE + 1);
    digestInfos[DIGEST_INFO_HAS_SIGN].resize(digestLen);

    int32_t ret = GenerateFileDigest(stream, info->digestMethod, DIGEST_FLAGS_HAS_SIGN, digestInfos);
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "Fail to generate signature %s", stream->GetFileName().c_str());
    SignAlgorithm::SignAlgorithmPtr signAlgorithm =
        PkgAlgorithmFactory::GetSignAlgorithm(signVerifyKeyName_, info->signMethod, info->digestMethod);
    PKG_CHECK(signAlgorithm != nullptr, return PKG_INVALID_SIGNATURE, "Invalid sign algo");

    size_t signLen = DigestAlgorithm::GetSignatureLen(info->digestMethod);
    std::vector<uint8_t> signedData(signLen, 0);
    // Clear buffer
    PkgBuffer signBuffer(signedData);
    ret = stream->Write(signBuffer, signLen, offset);
    size_t signDataLen = 0;
    signedData.clear();
    PkgBuffer digest(digestInfos[DIGEST_INFO_HAS_SIGN].data(), digestLen);
    ret = signAlgorithm->SignBuffer(digest, signedData, signDataLen);
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "Fail to SignBuffer %s", stream->GetFileName().c_str());
    PKG_CHECK(signDataLen <= signLen, return PKG_INVALID_SIGNATURE, "SignData len %zu more %zu", signDataLen, signLen);
    PKG_LOGI("Signature %zu %zu %s", offset, signDataLen, stream->GetFileName().c_str());
    ret = stream->Write(signBuffer, signDataLen, offset);
    stream->Flush(offset + signedData.size());
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "Fail to Write signature %s", stream->GetFileName().c_str());
    PKG_LOGW("Sign file %s success", stream->GetFileName().c_str());
    return ret;
}

int32_t PkgManagerImpl::SetSignVerifyKeyName(const std::string &keyName)
{
    if (access(keyName.c_str(), 0) != 0) {
        return PKG_INVALID_FILE;
    }
    signVerifyKeyName_ = keyName;
    return PKG_SUCCESS;
}

int32_t PkgManagerImpl::DecompressBuffer(FileInfoPtr info, const PkgBuffer &data, StreamPtr output) const
{
    PKG_CHECK(info != nullptr && data.buffer != nullptr && output != nullptr, return PKG_INVALID_PARAM,
        "Param is null");
    PkgAlgorithm::PkgAlgorithmPtr algorithm = PkgAlgorithmFactory::GetAlgorithm(info);
    PKG_CHECK(algorithm != nullptr, return PKG_INVALID_PARAM, "Can not get algorithm for %s", info->identity.c_str());

    std::shared_ptr<MemoryMapStream> inStream = std::make_shared<MemoryMapStream>(info->identity,
        data, PkgStream::PkgStreamType_Buffer);
    PKG_CHECK(inStream != nullptr, return PKG_INVALID_PARAM, "Can not create stream for %s", info->identity.c_str());
    PkgAlgorithmContext context = {{0, 0}, {data.length, 0}, 0, info->digestMethod};
    int32_t ret = algorithm->Unpack(inStream.get(), PkgStreamImpl::ConvertPkgStream(output), context);
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "Fail Decompress for %s", info->identity.c_str());
    PKG_LOGI("packedSize: %zu unpackedSize: %zu ", data.length, context.unpackedSize);
    PkgStreamImpl::ConvertPkgStream(output)->Flush(context.unpackedSize);
    info->packedSize = context.packedSize;
    info->unpackedSize = context.unpackedSize;
    algorithm->UpdateFileInfo(info);
    return PKG_SUCCESS;
}

int32_t PkgManagerImpl::CompressBuffer(FileInfoPtr info, const PkgBuffer &data, StreamPtr output) const
{
    PKG_CHECK(info != nullptr && data.buffer != nullptr && output != nullptr,
              return PKG_INVALID_PARAM, "Param is null ");
    PkgAlgorithm::PkgAlgorithmPtr algorithm = PkgAlgorithmFactory::GetAlgorithm(info);
    PKG_CHECK(algorithm != nullptr, return PKG_INVALID_PARAM, "Can not get algorithm for %s", info->identity.c_str());

    std::shared_ptr<MemoryMapStream> inStream = std::make_shared<MemoryMapStream>(info->identity,
        data, PkgStream::PkgStreamType_Buffer);
    PKG_CHECK(inStream != nullptr, return PKG_INVALID_PARAM, "Can not create stream for %s", info->identity.c_str());
    PkgAlgorithmContext context = {{0, 0}, {0, data.length}, 0, info->digestMethod};
    int32_t ret = algorithm->Pack(inStream.get(), PkgStreamImpl::ConvertPkgStream(output), context);
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "Fail Decompress for %s", info->identity.c_str());
    PKG_LOGI("packedSize: %zu unpackedSize: %zu ", context.packedSize, context.unpackedSize);
    PkgStreamImpl::ConvertPkgStream(output)->Flush(context.packedSize);
    info->packedSize = context.packedSize;
    info->unpackedSize = context.unpackedSize;
    return PKG_SUCCESS;
}
} // namespace hpackage
