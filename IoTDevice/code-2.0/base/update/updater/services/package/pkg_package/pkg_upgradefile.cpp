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
#include "pkg_upgradefile.h"
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <limits>
#include <memory>
#include "pkg_lz4file.h"
#include "pkg_manager.h"
#include "pkg_pkgfile.h"
#include "pkg_stream.h"
#include "pkg_utils.h"
#include "pkg_zipfile.h"
#include "securec.h"

#define CHECK_TLV(tlv, tlvType, len, fileLen)                                                    \
    do {                                                                                         \
        if (!((tlv)->length < (fileLen) && (tlv)->length >= (len) && (tlv)->type == (tlvType) && \
              ((tlv)->length + sizeof(PkgTlv)) < (fileLen))) {                                   \
            PKG_LOGE("Invalid tlv type: %d length %u ", tlvType, ((tlv)->length));               \
            return PKG_INVALID_FILE;                                                             \
        }                                                                                        \
    } while (0)

using namespace std;

namespace hpackage {
constexpr int32_t UPGRADE_FILE_HEADER_LEN = 3 * sizeof(PkgTlv) + sizeof(UpgradePkgHeader) + sizeof(UpgradePkgTime);
constexpr int32_t UPGRADE_RESERVE_LEN = 16;
constexpr int16_t TLV_TYPE_FOR_SHA256 = 0x0001;
constexpr int16_t TLV_TYPE_FOR_SHA384 = 0x0011;

int32_t UpgradeFileEntry::Init(const PkgManager::FileInfoPtr fileInfo, PkgStreamPtr inStream)
{
    int32_t ret = PkgEntry::Init(&fileInfo_.fileInfo, fileInfo, inStream);
    PKG_CHECK(ret == PKG_SUCCESS, return PKG_INVALID_PARAM, "Fail to check input param");
    ComponentInfo *info = (ComponentInfo *)fileInfo;
    if (info != nullptr) {
        fileInfo_.version = info->version;
        fileInfo_.id = info->id;
        fileInfo_.resType = info->resType;
        fileInfo_.type = info->type;
        fileInfo_.compFlags = info->compFlags;
        fileInfo_.originalSize = info->originalSize;
        PKG_CHECK(!memcpy_s(fileInfo_.digest, sizeof(fileInfo_.digest), info->digest, sizeof(info->digest)),
            return PKG_NONE_MEMORY, "UpgradeFileEntry memcpy failed");
    }
    return PKG_SUCCESS;
}

size_t UpgradePkgFile::GetUpgradeSignatureLen() const
{
    return SIGN_SHA256_LEN + SIGN_SHA384_LEN;
}

size_t UpgradePkgFile::GetDigestLen() const
{
    return DigestAlgorithm::GetDigestLen(pkgInfo_.pkgInfo.digestMethod);
}

int32_t UpgradePkgFile::AddEntry(const PkgManager::FileInfoPtr file, const PkgStreamPtr inStream)
{
    PKG_CHECK(file != nullptr && inStream != nullptr, return PKG_INVALID_PARAM, "Fail to check input param");
    PKG_CHECK(CheckState({PKG_FILE_STATE_IDLE, PKG_FILE_STATE_WORKING}, PKG_FILE_STATE_WORKING),
        return PKG_INVALID_STATE, "error state curr %d ", state_);
    PKG_CHECK(pkgEntryMapId_.size() < pkgInfo_.pkgInfo.entryCount, return PKG_INVALID_PARAM,
        "More entry for and for %s %zu", file->identity.c_str(), pkgEntryMapId_.size());
    PKG_LOGI("Add file %s to package", file->identity.c_str());

    size_t compDataLen = 0;
    for (auto &it : pkgEntryMapId_) {
        compDataLen += (*it.second).GetFileInfo()->packedSize;
    }

    UpgradeFileEntry *entry = static_cast<UpgradeFileEntry *>(AddPkgEntry(file->identity));
    PKG_CHECK(entry != nullptr, return PKG_NONE_MEMORY, "Fail create pkg node for %s", file->identity.c_str());
    int32_t ret = entry->Init(file, inStream);
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "Fail init entry for %s", file->identity.c_str());

    size_t dataOffset = UPGRADE_FILE_HEADER_LEN + pkgInfo_.pkgInfo.entryCount * sizeof(UpgradeCompInfo);
    dataOffset += UPGRADE_RESERVE_LEN + GetUpgradeSignatureLen();
    dataOffset += compDataLen;
    size_t encodeLen = 0;
    ret = entry->Pack(inStream, dataOffset, encodeLen);
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "Fail Pack for %s", file->identity.c_str());
    packedFileSize_ += encodeLen;

    size_t offset = UPGRADE_FILE_HEADER_LEN + (pkgEntryMapId_.size() - 1) * sizeof(UpgradeCompInfo);
    ret = entry->EncodeHeader(inStream, offset, encodeLen);
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "Fail encode header for %s", file->identity.c_str());

    PKG_LOGI("Header offset:%zu data offset:%zu packedFileSize: %zu", offset, dataOffset, packedFileSize_);
    return PKG_SUCCESS;
}

int32_t UpgradePkgFile::SavePackage(size_t &signOffset)
{
    PKG_LOGI("SavePackage %s", pkgStream_->GetFileName().c_str());
    PKG_CHECK(CheckState({PKG_FILE_STATE_WORKING}, PKG_FILE_STATE_CLOSE),
        return PKG_INVALID_STATE, "error state curr %d ", state_);
    // Allocate buffer size with max possible size
    size_t buffSize = GetUpgradeSignatureLen() + UPGRADE_RESERVE_LEN;
    buffSize = ((UPGRADE_FILE_HEADER_LEN > buffSize) ? UPGRADE_FILE_HEADER_LEN : buffSize);
    std::vector<uint8_t> buffer(buffSize);

    size_t offset = 0;
    // Package header information
    WriteLE16(buffer.data(), GetPackageTlvType()); // Type is 1 for package header in TLV format
    WriteLE16(buffer.data() + sizeof(uint16_t), sizeof(UpgradePkgHeader));
    offset += sizeof(PkgTlv);
    UpgradePkgHeader *header = reinterpret_cast<UpgradePkgHeader *>(buffer.data() + offset);
    header->pkgInfoLength = sizeof(PkgTlv) + sizeof(PkgTlv) + sizeof(PkgTlv) + sizeof(UpgradePkgHeader) +
        sizeof(UpgradePkgTime) + pkgInfo_.pkgInfo.entryCount * sizeof(UpgradeCompInfo) + UPGRADE_RESERVE_LEN;
    WriteLE32(reinterpret_cast<uint8_t *>(&header->updateFileVersion), pkgInfo_.updateFileVersion);
    int32_t ret = memcpy_s(header->softwareVersion, sizeof(header->softwareVersion), pkgInfo_.softwareVersion.data(),
        pkgInfo_.softwareVersion.size());
    PKG_CHECK(ret == EOK, return ret, "Fail to memcpy_s %s ret：%d", pkgStream_->GetFileName().c_str(), ret);
    ret = memcpy_s(header->productUpdateId, sizeof(header->productUpdateId), pkgInfo_.productUpdateId.data(),
        pkgInfo_.productUpdateId.size());
    PKG_CHECK(ret == EOK, return ret, "Fail to memcpy_s %s ret：%d", pkgStream_->GetFileName().c_str(), ret);
    offset += sizeof(UpgradePkgHeader);
    // 时间tlv
    WriteLE16(buffer.data() + offset, 0x02); // Type is 2 for time in TLV format
    WriteLE16(buffer.data() + offset + sizeof(uint16_t), sizeof(UpgradePkgTime));
    offset += sizeof(PkgTlv);
    UpgradePkgTime *time = reinterpret_cast<UpgradePkgTime *>(buffer.data() + offset);
    ret = memcpy_s(time->date, sizeof(time->date), pkgInfo_.date.data(), pkgInfo_.date.size());
    PKG_CHECK(ret == EOK, return ret, "Fail to memcpy_s %s ret：%d", pkgStream_->GetFileName().c_str(), ret);
    ret = memcpy_s(time->time, sizeof(time->time), pkgInfo_.time.data(), pkgInfo_.time.size());
    PKG_CHECK(ret == EOK, return ret, "Fail to memcpy_s %s ret：%d", pkgStream_->GetFileName().c_str(), ret);
    offset += sizeof(UpgradePkgTime);
    // 组件的tlv
    WriteLE16(buffer.data() + offset, 0x05); // Type is 5 for component in TLV format
    WriteLE16(buffer.data() + offset + sizeof(uint16_t), pkgInfo_.pkgInfo.entryCount * sizeof(UpgradeCompInfo));
    offset += sizeof(PkgTlv);
    ret = pkgStream_->Write(buffer, UPGRADE_FILE_HEADER_LEN, 0);
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "Fail write upgrade file header for %s ret：%d",
        pkgStream_->GetFileName().c_str(), ret);

    // Clear buffer and save signature information
    offset += pkgInfo_.pkgInfo.entryCount * sizeof(UpgradeCompInfo);
    signOffset = (pkgInfo_.pkgInfo.digestMethod == PKG_DIGEST_TYPE_SHA384) ?
        (offset + UPGRADE_RESERVE_LEN + SIGN_SHA256_LEN) : (offset + UPGRADE_RESERVE_LEN);

    buffer.assign(buffer.capacity(), 0);
    ret = pkgStream_->Write(buffer, GetUpgradeSignatureLen() + UPGRADE_RESERVE_LEN, offset);
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "Fail write sign for %s", pkgStream_->GetFileName().c_str());
    PKG_LOGI("SavePackage success file length: %zu signOffset %zu", pkgStream_->GetFileLength(), signOffset);
    pkgStream_->Flush(offset);
    return PKG_SUCCESS;
}

int32_t UpgradePkgFile::LoadPackage(std::vector<std::string> &fileNames, VerifyFunction verifier)
{
    PKG_CHECK(verifier != nullptr, return PKG_INVALID_SIGNATURE, "Check verifier nullptr");
    PKG_CHECK(CheckState({PKG_FILE_STATE_IDLE}, PKG_FILE_STATE_WORKING),
        return PKG_INVALID_STATE, "error state curr %d ", state_);
    PKG_LOGI("LoadPackage %s ", pkgStream_->GetFileName().c_str());
    size_t fileLen = pkgStream_->GetFileLength();
    // Allocate buffer with smallest package size
    size_t buffSize = UPGRADE_FILE_HEADER_LEN + sizeof(UpgradeCompInfo) +
        GetUpgradeSignatureLen() + UPGRADE_RESERVE_LEN;
    PKG_CHECK(fileLen > 0 && fileLen <= static_cast<size_t>(0xffffffff) && fileLen >= buffSize,
        return PKG_INVALID_FILE, "Invalid file %s fileLen:%zu ", pkgStream_->GetFileName().c_str(), fileLen);

    DigestAlgorithm::DigestAlgorithmPtr algorithm = nullptr;
    // Parse header
    PkgBuffer buffer(buffSize);
    size_t parsedLen = 0;
    int32_t ret = ReadUpgradePkgHeader(buffer, parsedLen, algorithm);
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "Decode header fail %d", ret);

    ret = ReadComponents(buffer, parsedLen, algorithm, fileNames);
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "Decode components fail %d", ret);
    PKG_CHECK(parsedLen + UPGRADE_RESERVE_LEN + GetUpgradeSignatureLen() < fileLen,
        return ret, "Decode components fail %d", ret);

    // Read signature information
    std::vector<uint8_t> reversedData(UPGRADE_RESERVE_LEN + GetUpgradeSignatureLen(), 0);
    PkgBuffer signBuffer(reversedData);
    algorithm->Update(signBuffer, UPGRADE_RESERVE_LEN + GetUpgradeSignatureLen());
    size_t readLen = 0;
    if (pkgInfo_.pkgInfo.digestMethod == PKG_DIGEST_TYPE_SHA384) {
        reversedData.resize(SIGN_SHA384_LEN);
        ret = pkgStream_->Read(reversedData,
            parsedLen + UPGRADE_RESERVE_LEN + SIGN_SHA256_LEN, SIGN_SHA384_LEN, readLen);
    } else {
        reversedData.resize(SIGN_SHA256_LEN);
        ret = pkgStream_->Read(reversedData, parsedLen + UPGRADE_RESERVE_LEN, SIGN_SHA256_LEN, readLen);
    }
    PKG_LOGI("signOffset %zu", parsedLen + UPGRADE_RESERVE_LEN);
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "read header struct fail");
    parsedLen += UPGRADE_RESERVE_LEN + GetUpgradeSignatureLen();

    // Calculate digest
    size_t offset = parsedLen;
    size_t readBytes = 0;
    while (offset + readBytes < pkgStream_->GetFileLength()) {
        offset += readBytes;
        readBytes = 0;
        size_t remainBytes = pkgStream_->GetFileLength() - offset;
        remainBytes = ((remainBytes > buffSize) ? buffSize : remainBytes);
        ret = pkgStream_->Read(buffer, offset, remainBytes, readBytes);
        PKG_CHECK(ret == PKG_SUCCESS, return ret, "Fail to read data ");
        algorithm->Update(buffer, readBytes);
    }

    PkgBuffer digest(GetDigestLen());
    algorithm->Final(digest);
    ret = verifier(&pkgInfo_.pkgInfo, digest.data, reversedData);
    PKG_CHECK(ret == 0, return PKG_INVALID_SIGNATURE, "Fail to verifier signature");
    return PKG_SUCCESS;
}

int32_t UpgradePkgFile::ReadComponents(const PkgBuffer &buffer, size_t &parsedLen,
    DigestAlgorithm::DigestAlgorithmPtr algorithm, std::vector<std::string> &fileNames)
{
    size_t fileLen = pkgStream_->GetFileLength();
    size_t readLen = 0;
    int32_t ret = pkgStream_->Read(buffer, parsedLen, buffer.length, readLen);
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "Read component fail");
    PkgTlv tlv;
    tlv.type = ReadLE16(buffer.buffer);
    tlv.length = ReadLE16(buffer.buffer + sizeof(uint16_t));
    CHECK_TLV(&tlv, 5, sizeof(UpgradeCompInfo), fileLen); // component type is 5
    algorithm->Update(buffer, sizeof(PkgTlv)); // tlv generate digest

    parsedLen += sizeof(PkgTlv);
    size_t dataOffset = parsedLen + tlv.length + GetUpgradeSignatureLen() + UPGRADE_RESERVE_LEN;
    size_t srcOffset = 0;
    size_t currLen = sizeof(PkgTlv);
    while (srcOffset < tlv.length) {
        if (currLen + sizeof(UpgradeCompInfo) > readLen) {
            readLen = 0;
            ret = pkgStream_->Read(buffer, parsedLen + srcOffset, buffer.length, readLen);
            PKG_CHECK(ret == PKG_SUCCESS, return ret, "Fail to read data");
            currLen = 0;
        }

        UpgradeFileEntry *entry = new UpgradeFileEntry(this, nodeId_++);
        PKG_CHECK(entry != nullptr, return PKG_NONE_MEMORY, "Fail create upgrade node for %s",
            pkgStream_->GetFileName().c_str());

        // Extract header information from file
        size_t decodeLen = 0;
        PkgBuffer headerBuff(buffer.buffer + currLen, readLen - currLen);
        ret = entry->DecodeHeader(headerBuff, parsedLen + srcOffset, dataOffset, decodeLen);
        PKG_CHECK(ret == PKG_SUCCESS, delete entry; return ret, "Fail to decode header");

        // Save entry
        pkgEntryMapId_.insert(pair<uint32_t, PkgEntryPtr>(entry->GetNodeId(), entry));
        pkgEntryMapFileName_.insert(std::pair<std::string, PkgEntryPtr>(entry->GetFileName(), entry));
        fileNames.push_back(entry->GetFileName());

        PkgBuffer signBuffer(buffer.buffer + currLen, decodeLen);
        algorithm->Update(signBuffer, decodeLen); // Generate digest for components

        currLen += decodeLen;
        srcOffset += decodeLen;
        dataOffset += entry->GetFileInfo()->packedSize;
        pkgInfo_.pkgInfo.entryCount++;
        PKG_LOGI("Component packedSize %zu unpackedSize %zu %s", entry->GetFileInfo()->packedSize,
            entry->GetFileInfo()->unpackedSize, entry->GetFileInfo()->identity.c_str());
    }
    parsedLen += srcOffset;
    return PKG_SUCCESS;
}

int32_t UpgradePkgFile::ReadUpgradePkgHeader(const PkgBuffer &buffer, size_t &realLen,
    DigestAlgorithm::DigestAlgorithmPtr &algorithm)
{
    size_t fileLen = pkgStream_->GetFileLength();
    size_t readLen = 0;
    size_t currLen = 0;
    int32_t ret = pkgStream_->Read(buffer, 0, buffer.length, readLen);
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "Fail to read header");
    pkgInfo_.pkgInfo.pkgType = PkgFile::PKG_TYPE_UPGRADE;

    PkgTlv tlv;
    tlv.type = ReadLE16(buffer.buffer);
    tlv.length = ReadLE16(buffer.buffer + sizeof(uint16_t));
    if (tlv.type == TLV_TYPE_FOR_SHA256) {
        pkgInfo_.pkgInfo.signMethod = PKG_SIGN_METHOD_RSA;
        pkgInfo_.pkgInfo.digestMethod = PKG_DIGEST_TYPE_SHA256;
    } else if (tlv.type == TLV_TYPE_FOR_SHA384) {
        pkgInfo_.pkgInfo.signMethod = PKG_SIGN_METHOD_RSA;
        pkgInfo_.pkgInfo.digestMethod = PKG_DIGEST_TYPE_SHA384;
    }

    // Header information
    currLen = sizeof(PkgTlv);
    UpgradePkgHeader *header = reinterpret_cast<UpgradePkgHeader *>(buffer.buffer + currLen);
    pkgInfo_.updateFileVersion = ReadLE32(buffer.buffer + currLen + offsetof(UpgradePkgHeader, updateFileVersion));
    PkgFile::ConvertBufferToString(pkgInfo_.softwareVersion, {header->softwareVersion,
        sizeof(header->softwareVersion)});
    PkgFile::ConvertBufferToString(pkgInfo_.productUpdateId, {header->productUpdateId,
        sizeof(header->productUpdateId)});

    if (currLen + tlv.length >= readLen) { // Extra TLV information, read it.
        realLen = currLen + tlv.length;
        algorithm->Update(buffer, realLen);
        ret = pkgStream_->Read(buffer, realLen, buffer.length, readLen);
        PKG_CHECK(ret == PKG_SUCCESS, return ret, "Fail to read header");
        currLen = 0;
    } else {
        currLen += tlv.length;
    }

    // Time information
    tlv.type = ReadLE16(buffer.buffer + currLen);
    tlv.length = ReadLE16(buffer.buffer + currLen + sizeof(uint16_t));
    CHECK_TLV(&tlv, sizeof(uint16_t), sizeof(UpgradePkgTime), fileLen);
    currLen += sizeof(PkgTlv);
    UpgradePkgTime *time = reinterpret_cast<UpgradePkgTime *>(buffer.buffer + currLen);
    PkgFile::ConvertBufferToString(pkgInfo_.date, {time->date, sizeof(time->date)});
    PkgFile::ConvertBufferToString(pkgInfo_.time, {time->time, sizeof(time->time)});
    currLen += tlv.length;
    realLen += currLen;

    // Parser header to get compressional algorithm
    algorithm = PkgAlgorithmFactory::GetDigestAlgorithm(pkgInfo_.pkgInfo.digestMethod);
    PKG_CHECK(algorithm != nullptr, return PKG_NOT_EXIST_ALGORITHM,
        "Invalid file %s", pkgStream_->GetFileName().c_str());
    algorithm->Init();
    algorithm->Update(buffer, currLen); // Generate digest
    return PKG_SUCCESS;
}

int32_t UpgradeFileEntry::EncodeHeader(PkgStreamPtr inStream, size_t startOffset, size_t &encodeLen)
{
    PkgStreamPtr outStream = pkgFile_->GetPkgStream();
    PKG_CHECK(outStream != nullptr && inStream != nullptr, return PKG_INVALID_PARAM,
        "outStream or inStream null for %s", fileName_.c_str());

    UpgradeCompInfo comp;
    PKG_CHECK(!memset_s(&comp, sizeof(comp), 0, sizeof(comp)),
        return PKG_NONE_MEMORY, "UpgradeFileEntry memset_s failed");

    size_t len = 0;
    int32_t ret = PkgFile::ConvertStringToBuffer(
        fileInfo_.fileInfo.identity, {comp.address, sizeof(comp.address)}, len);
    ret |= PkgFile::ConvertStringToBuffer(fileInfo_.version, {comp.version, sizeof(comp.version)}, len);
    PKG_CHECK(ret == PKG_SUCCESS, return PKG_INVALID_PARAM, "outStream or inStream null for %s", fileName_.c_str());

    ret = memcpy_s(comp.digest, sizeof(comp.digest), fileInfo_.digest, sizeof(fileInfo_.digest));
    PKG_CHECK(ret == EOK, return ret, "Fail to memcpy_s ret：%d", ret);
    WriteLE32(reinterpret_cast<uint8_t *>(&comp.size), fileInfo_.fileInfo.unpackedSize);
    WriteLE16(reinterpret_cast<uint8_t *>(&comp.id), fileInfo_.id);
    WriteLE32(reinterpret_cast<uint8_t *>(&comp.originalSize), fileInfo_.fileInfo.unpackedSize);
    comp.resType = fileInfo_.resType;
    comp.flags = fileInfo_.compFlags;
    comp.type = fileInfo_.type;

    headerOffset_ = startOffset;
    PkgBuffer buffer(reinterpret_cast<uint8_t *>(&comp), sizeof(comp));
    ret = outStream->Write(buffer, sizeof(comp), startOffset);
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "Fail write header for %s", fileName_.c_str());
    encodeLen = sizeof(UpgradeCompInfo);

    PKG_LOGI("EncodeHeader startOffset: %zu %zu packedSize:%zu %zu ", headerOffset_, dataOffset_,
        fileInfo_.fileInfo.packedSize, fileInfo_.fileInfo.unpackedSize);
    return PKG_SUCCESS;
}

int32_t UpgradeFileEntry::Pack(PkgStreamPtr inStream, size_t startOffset, size_t &encodeLen)
{
    PkgAlgorithm::PkgAlgorithmPtr algorithm = PkgAlgorithmFactory::GetAlgorithm(&fileInfo_.fileInfo);
    PkgStreamPtr outStream = pkgFile_->GetPkgStream();
    PKG_CHECK(algorithm != nullptr && outStream != nullptr && inStream != nullptr, return PKG_INVALID_PARAM,
        "outStream or inStream null for %s", fileName_.c_str());

    PkgAlgorithmContext context = {
        {0, startOffset},
        {fileInfo_.fileInfo.packedSize, fileInfo_.fileInfo.unpackedSize},
        0, fileInfo_.fileInfo.digestMethod
    };
    PKG_CHECK(!memcpy_s(context.digest, sizeof(context.digest), fileInfo_.digest, sizeof(fileInfo_.digest)),
        return PKG_NONE_MEMORY, "UpgradeFileEntry pack memcpy failed");
    int32_t ret = algorithm->Pack(inStream, outStream, context);
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "Fail Compress for %s", fileName_.c_str());

    // Fill digest and compressed size of file
    PKG_CHECK(!memcpy_s(fileInfo_.digest, sizeof(fileInfo_.digest), context.digest, sizeof(context.digest)),
        return PKG_NONE_MEMORY, "UpgradeFileEntry pack memcpy failed");
    fileInfo_.fileInfo.packedSize = context.packedSize;
    dataOffset_ = startOffset;
    encodeLen = fileInfo_.fileInfo.packedSize;
    PKG_LOGI("Pack start:%zu unpackSize:%zu packSize:%zu", startOffset, fileInfo_.fileInfo.unpackedSize,
        fileInfo_.fileInfo.packedSize);
    return PKG_SUCCESS;
}

int32_t UpgradeFileEntry::DecodeHeader(const PkgBuffer &buffer, size_t headerOffset, size_t dataOffset,
    size_t &decodeLen)
{
    PkgStreamPtr inStream = pkgFile_->GetPkgStream();
    PKG_CHECK(inStream != nullptr, return PKG_INVALID_PARAM, "outStream or inStream null for %s", fileName_.c_str());
    PKG_CHECK(buffer.length >= sizeof(UpgradeCompInfo),
        return PKG_INVALID_PKG_FORMAT, "Fail to check buffer %zu", buffer.length);

    UpgradeCompInfo *info = reinterpret_cast<UpgradeCompInfo *>(buffer.buffer);
    fileInfo_.fileInfo.packedSize = ReadLE32(buffer.buffer + offsetof(UpgradeCompInfo, size));
    fileInfo_.fileInfo.unpackedSize = ReadLE32(buffer.buffer + offsetof(UpgradeCompInfo, originalSize));
    fileInfo_.fileInfo.packMethod = PKG_COMPRESS_METHOD_NONE;
    fileInfo_.fileInfo.digestMethod = PKG_DIGEST_TYPE_SHA256;
    int32_t ret = memcpy_s(fileInfo_.digest, sizeof(fileInfo_.digest), info->digest, sizeof(info->digest));
    PKG_CHECK(ret == EOK, return ret, "Fail to memcpy_s ret：%d", ret);
    PkgFile::ConvertBufferToString(fileInfo_.fileInfo.identity, {info->address, sizeof(info->address)});
    PkgFile::ConvertBufferToString(fileInfo_.version, {info->version, sizeof(info->version)});
    fileName_ = fileInfo_.fileInfo.identity;
    fileInfo_.id = ReadLE16(buffer.buffer + offsetof(UpgradeCompInfo, id));
    fileInfo_.resType = info->resType;
    fileInfo_.compFlags = info->flags;
    fileInfo_.type = info->type;
    fileInfo_.originalSize = fileInfo_.fileInfo.unpackedSize;

    headerOffset_ = headerOffset;
    dataOffset_ = dataOffset;
    decodeLen = sizeof(UpgradeCompInfo);

    PKG_LOGI("Component offset: %zu %zu packedSize:%zu %zu %s", headerOffset, dataOffset,
        fileInfo_.fileInfo.packedSize, fileInfo_.fileInfo.unpackedSize, fileName_.c_str());
    return PKG_SUCCESS;
}

int32_t UpgradeFileEntry::Unpack(PkgStreamPtr outStream)
{
    PkgAlgorithm::PkgAlgorithmPtr algorithm = PkgAlgorithmFactory::GetAlgorithm(&fileInfo_.fileInfo);
    PKG_CHECK(algorithm != nullptr, return PKG_INVALID_PARAM, "can not algorithm for %s", fileName_.c_str());

    PkgStreamPtr inStream = pkgFile_->GetPkgStream();
    PKG_CHECK(outStream != nullptr && inStream != nullptr, return PKG_INVALID_PARAM,
        "outStream or inStream null for %s", fileName_.c_str());
    PkgAlgorithmContext context = {
        {this->dataOffset_, 0},
        {fileInfo_.fileInfo.packedSize, fileInfo_.fileInfo.unpackedSize},
        0, fileInfo_.fileInfo.digestMethod
    };
    int32_t ret = memcpy_s(context.digest, sizeof(context.digest), fileInfo_.digest, sizeof(fileInfo_.digest));
    PKG_CHECK(ret == EOK, return ret, "Fail to memcpy_s ret：%d", ret);
    ret = algorithm->Unpack(inStream, outStream, context);
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "Fail Decompress for %s", fileName_.c_str());
    PKG_LOGI("Unpack %s data offset:%zu packedSize:%zu unpackedSize:%zu", fileName_.c_str(), dataOffset_,
        fileInfo_.fileInfo.packedSize, fileInfo_.fileInfo.unpackedSize);
    outStream->Flush(fileInfo_.fileInfo.unpackedSize);
    return PKG_SUCCESS;
}

int16_t UpgradePkgFile::GetPackageTlvType()
{
    static int16_t packageTlvType[PKG_DIGEST_TYPE_MAX] = {
        TLV_TYPE_FOR_SHA256, TLV_TYPE_FOR_SHA256, TLV_TYPE_FOR_SHA256, TLV_TYPE_FOR_SHA384
    };
    if (pkgInfo_.pkgInfo.digestMethod < PKG_DIGEST_TYPE_MAX) {
        return packageTlvType[pkgInfo_.pkgInfo.digestMethod];
    }
    return TLV_TYPE_FOR_SHA256;
}
} // namespace hpackage
