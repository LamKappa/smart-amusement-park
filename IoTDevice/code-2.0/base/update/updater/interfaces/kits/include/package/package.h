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
#ifndef PACKAGE_UPDATER_H
#define PACKAGE_UPDATER_H

#include <cstdlib>
#include <iostream>
#include <functional>
#include <memory>
#include <cstdio>

#ifdef __cplusplus
extern "C" {
#endif

constexpr uint32_t DIGEST_MAX_LEN = 32;
constexpr int8_t MID_COMPRESS_LEVEL = 2;
constexpr int8_t PKG_SUPPORT_L1 = 0x01;

enum PkgPackType {
    PKG_PACK_TYPE_NONE = 0,
    PKG_PACK_TYPE_UPGRADE,              // upgrade package
    PKG_PACK_TYPE_ZIP,                  // zip package
    PKG_PACK_TYPE_LZ4,                  // lz4 package
    PKG_PACK_TYPE_GZIP                  // gzip package
};

/**
 * Supported compression methods
 */
enum PkgCompressMethod {
    PKG_COMPRESS_METHOD_NONE = 0,       // no compression, for upgrade
    PKG_COMPRESS_METHOD_ZIP,            // standard deflate
    PKG_COMPRESS_METHOD_LZ4,            // lz4
    PKG_COMPRESS_METHOD_LZ4_BLOCK,      // lz4 block
    PKG_COMPRESS_METHOD_GZIP            // gzip
};

/**
 * Digest algorithm
 */
enum PkgDigestMethod {
    PKG_DIGEST_TYPE_NONE = 0,
    PKG_DIGEST_TYPE_CRC = 1,            // crc for zip
    PKG_DIGEST_TYPE_SHA256,
    PKG_DIGEST_TYPE_SHA384,
    PKG_DIGEST_TYPE_MAX
};

/**
 * signature algorithm
 */
enum PkgSignMethod {
    PKG_SIGN_METHOD_NONE = 0,
    PKG_SIGN_METHOD_RSA = 1,
    PKG_SIGN_METHOD_ECDSA = 2,
    PKG_SIGN_METHOD_MAX
};

/**
 * C function used by Python to create an update package
 */
struct UpgradePkgInfoExt {
    uint8_t digestMethod = PKG_DIGEST_TYPE_SHA256;
    uint8_t signMethod = PKG_SIGN_METHOD_RSA;
    uint8_t pkgType = 0;
    uint32_t entryCount = 0;
    uint32_t updateFileVersion = 0;
    char *productUpdateId;
    char *softwareVersion;
    char *date;
    char *time;
};

struct ComponentInfoExt {
    uint8_t digest[DIGEST_MAX_LEN];
    char *filePath;
    char *componentAddr;
    char *version;
    uint32_t size = 0;
    uint32_t id;
    uint32_t originalSize;
    uint8_t resType;
    uint8_t type;
    uint8_t flags;
};

/**
 * create upgrade package
 *
 * @param pkgInfo       package information
 * @param comp          component information list
 * @param path          package file output path
 * @param keyPath       path of the key used for signature
 * @return              update package creation result
 */
int32_t CreatePackage(const UpgradePkgInfoExt *pkgInfo, ComponentInfoExt comp[],
    const char *path, const char *keyPath);

/**
 * create upgrade package
 *
 * @param pkgInfo       package information
 * @param comp          component information list
 * @param path          package file output path
 * @param offset        offset for sign
 * @param hashCode      hash code for sign
 * @return              update package creation result
 */
int32_t CreatePackageL1(const UpgradePkgInfoExt *pkgInfo, ComponentInfoExt comp[],
    const char *path, uint32_t *offset, char **hashCode);

/**
 * Signature verification of upgrade package
 *
 * @param packagePath       path of the update package
 * @param keyPath           path of the key used for verification
 * @param version           file versioin
 * @param digest            digest value
 * @param size              digest value size
 * @return                  verification result
 */
int32_t VerifyPackage(const char *packagePath, const char *keyPath,
    const char *version, const uint8_t *digest, size_t size);

/**
 * 对升级包进行签名验证
 *
 * @param packagePath 包文件的文件名
 * @param keyPath 运来校验的key的文件名
 * @param cb 校验进度回调函数
 * @return 校验结果
 */
int32_t VerifyPackageWithCallback(const std::string &packagePath, const std::string &keyPath,
    std::function<void(int32_t result, uint32_t percent)> cb);

#ifdef __cplusplus
}
#endif
#endif // PACKAGE_UPDATER_H
