/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "pkg_lz4file.h"
#include "pkg_algo_lz4.h"

using namespace std;

namespace hpackage {
int32_t Lz4FileEntry::Init(const PkgManager::FileInfoPtr fileInfo, PkgStreamPtr inStream)
{
    int32_t ret = PkgEntry::Init(&fileInfo_.fileInfo, fileInfo, inStream);
    PKG_CHECK(ret == PKG_SUCCESS, return PKG_INVALID_PARAM, "Fail to check input param");
    Lz4FileInfo *info = (Lz4FileInfo *)fileInfo;
    if (info != nullptr) {
        fileInfo_.compressionLevel = info->compressionLevel;
        fileInfo_.blockIndependence = info->blockIndependence;
        fileInfo_.blockSizeID = info->blockSizeID;
        fileInfo_.contentChecksumFlag = info->contentChecksumFlag;
    }
    return PKG_SUCCESS;
}

int32_t Lz4FileEntry::EncodeHeader(PkgStreamPtr inStream, size_t startOffset, size_t &encodeLen)
{
    encodeLen = 0;
    fileInfo_.fileInfo.headerOffset = startOffset;
    fileInfo_.fileInfo.dataOffset = startOffset;
    return PKG_SUCCESS;
}

int32_t Lz4FileEntry::Pack(PkgStreamPtr inStream, size_t startOffset, size_t &encodeLen)
{
    PkgAlgorithm::PkgAlgorithmPtr algorithm = PkgAlgorithmFactory::GetAlgorithm(&fileInfo_.fileInfo);
    PkgStreamPtr outStream = pkgFile_->GetPkgStream();
    PKG_CHECK(fileInfo_.fileInfo.headerOffset == startOffset, return PKG_INVALID_PARAM, "start offset error for %s",
        fileInfo_.fileInfo.identity.c_str());
    PKG_CHECK(algorithm != nullptr && outStream != nullptr && inStream != nullptr, return PKG_INVALID_PARAM,
        "outStream or inStream null for %s", fileInfo_.fileInfo.identity.c_str());
    fileInfo_.fileInfo.dataOffset = startOffset;
    PkgAlgorithmContext context = {
        {0, startOffset},
        {fileInfo_.fileInfo.packedSize, fileInfo_.fileInfo.unpackedSize},
        0, fileInfo_.fileInfo.digestMethod
    };
    int32_t ret = algorithm->Pack(inStream, outStream, context);
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "Fail Compress for %s", fileInfo_.fileInfo.identity.c_str());
    fileInfo_.fileInfo.packedSize = context.packedSize;
    encodeLen = fileInfo_.fileInfo.packedSize;
    PKG_LOGI("Pack packedSize:%zu unpackedSize: %zu offset: %zu %zu", fileInfo_.fileInfo.packedSize,
        fileInfo_.fileInfo.unpackedSize, fileInfo_.fileInfo.headerOffset, fileInfo_.fileInfo.dataOffset);
    return PKG_SUCCESS;
}

int32_t Lz4FileEntry::Unpack(PkgStreamPtr outStream)
{
    PkgAlgorithm::PkgAlgorithmPtr algorithm = PkgAlgorithmFactory::GetAlgorithm(&fileInfo_.fileInfo);
    PKG_CHECK(algorithm != nullptr, return PKG_INVALID_PARAM, "can not algorithm for %s",
        fileInfo_.fileInfo.identity.c_str());

    PkgStreamPtr inStream = pkgFile_->GetPkgStream();
    PKG_CHECK(outStream != nullptr && inStream != nullptr, return PKG_INVALID_PARAM,
        "outStream or inStream null for %s", fileInfo_.fileInfo.identity.c_str());
    PkgAlgorithmContext context = {
        {fileInfo_.fileInfo.dataOffset, 0},
        {fileInfo_.fileInfo.packedSize, fileInfo_.fileInfo.unpackedSize},
        0, fileInfo_.fileInfo.digestMethod
    };
    int32_t ret = algorithm->Unpack(inStream, outStream, context);
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "Failed decompress for %s", fileInfo_.fileInfo.identity.c_str());
    fileInfo_.fileInfo.packedSize = context.packedSize;
    fileInfo_.fileInfo.unpackedSize = context.unpackedSize;
    PKG_LOGI("packedSize: %zu unpackedSize: %zu  offset header: %zu data: %zu", fileInfo_.fileInfo.packedSize,
        fileInfo_.fileInfo.unpackedSize, fileInfo_.fileInfo.headerOffset, fileInfo_.fileInfo.dataOffset);
    outStream->Flush(fileInfo_.fileInfo.unpackedSize);
    algorithm->UpdateFileInfo(&fileInfo_.fileInfo);
    return PKG_SUCCESS;
}

int32_t Lz4FileEntry::DecodeHeader(const PkgBuffer &buffer, size_t headerOffset, size_t dataOffset,
    size_t &decodeLen)
{
    fileInfo_.fileInfo.identity = "lz4_";
    fileInfo_.fileInfo.identity.append(std::to_string(nodeId_));
    fileName_ = fileInfo_.fileInfo.identity;
    fileInfo_.fileInfo.digestMethod = PKG_DIGEST_TYPE_NONE;
    uint32_t magicNumber = ReadLE32(buffer.buffer);
    if (magicNumber == PkgAlgorithmLz4::LZ4S_MAGIC_NUMBER) {
        fileInfo_.fileInfo.packMethod = PKG_COMPRESS_METHOD_LZ4;
    } else if (magicNumber == PkgAlgorithmLz4::LZ4B_MAGIC_NUMBER) {
        fileInfo_.fileInfo.packMethod = PKG_COMPRESS_METHOD_LZ4_BLOCK;
    }
    fileInfo_.fileInfo.headerOffset = headerOffset;
    fileInfo_.fileInfo.dataOffset = dataOffset;
    fileInfo_.fileInfo.unpackedSize = pkgFile_->GetPkgStream()->GetFileLength();
    fileInfo_.fileInfo.packedSize = pkgFile_->GetPkgStream()->GetFileLength();
    return PKG_SUCCESS;
}

int32_t Lz4PkgFile::AddEntry(const PkgManager::FileInfoPtr file, const PkgStreamPtr inStream)
{
    PKG_CHECK(file != nullptr && inStream != nullptr, return PKG_INVALID_PARAM, "Fail to check input param");
    PKG_CHECK(CheckState({ PKG_FILE_STATE_IDLE,
        PKG_FILE_STATE_WORKING
    }, PKG_FILE_STATE_CLOSE), return PKG_INVALID_STATE, "error state curr %d ", state_);
    PKG_LOGI("Add file %s to package", file->identity.c_str());

    Lz4FileEntry *entry = static_cast<Lz4FileEntry *>(AddPkgEntry(file->identity));
    PKG_CHECK(entry != nullptr, return PKG_NONE_MEMORY, "Fail create pkg node for %s", file->identity.c_str());
    int32_t ret = entry->Init(file, inStream);
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "Fail init entry for %s", file->identity.c_str());

    size_t encodeLen = 0;
    ret = entry->EncodeHeader(inStream, currentOffset_, encodeLen);
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "Fail encode header for %s", file->identity.c_str());
    currentOffset_ += encodeLen;
    ret = entry->Pack(inStream, currentOffset_, encodeLen);
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "Fail Pack for %s", file->identity.c_str());
    currentOffset_ += encodeLen;
    PKG_LOGI("offset:%zu ", currentOffset_);
    pkgStream_->Flush(currentOffset_);
    return PKG_SUCCESS;
}

int32_t Lz4PkgFile::SavePackage(size_t &offset)
{
    AddSignData(pkgInfo_.digestMethod, currentOffset_, offset);
    return PKG_SUCCESS;
}

int32_t Lz4PkgFile::LoadPackage(std::vector<std::string> &fileNames, VerifyFunction verifier)
{
    UNUSED(verifier);
    PKG_CHECK(CheckState({ PKG_FILE_STATE_IDLE }, PKG_FILE_STATE_WORKING), return PKG_INVALID_STATE,
        "error state curr %d ", state_);
    PKG_LOGI("LoadPackage %s ", pkgStream_->GetFileName().c_str());

    size_t srcOffset = 0;
    size_t readLen = 0;
    PkgBuffer buffer(sizeof(PkgAlgorithmLz4::LZ4B_MAGIC_NUMBER));
    int32_t ret = pkgStream_->Read(buffer, srcOffset, buffer.length, readLen);
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "Fail to read buffer");
    PKG_CHECK(readLen == sizeof(PkgAlgorithmLz4::LZ4B_MAGIC_NUMBER), return PKG_LZ4_FINISH, "Fail to read buffer");

    srcOffset += sizeof(PkgAlgorithmLz4::LZ4B_MAGIC_NUMBER);
    uint32_t magicNumber = ReadLE32(buffer.buffer);
    PKG_LOGI("LoadPackage magic 0x%x", magicNumber);
    ret = PKG_INVALID_FILE;
    if (magicNumber == PkgAlgorithmLz4::LZ4S_MAGIC_NUMBER ||
        magicNumber == PkgAlgorithmLz4::LZ4B_MAGIC_NUMBER) {
        Lz4FileEntry *entry = new Lz4FileEntry(this, nodeId_++);
        PKG_CHECK(entry != nullptr, return PKG_LZ4_FINISH, "Fail create upgrade node for %s",
            pkgStream_->GetFileName().c_str());
        ret = entry->DecodeHeader(buffer, 0, srcOffset, readLen);
        srcOffset += readLen;

        // 保存entry文件
        pkgEntryMapId_.insert(std::pair<uint32_t, PkgEntryPtr>(entry->GetNodeId(), entry));
        pkgEntryMapFileName_.insert(std::pair<std::string, PkgEntryPtr>(entry->GetFileName(), entry));
        fileNames.push_back(entry->GetFileName());
    }
    return ret;
}
} // namespace hpackage
