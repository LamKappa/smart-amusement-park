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

#ifndef UPDATER_DATA_WRITER_H
#define UPDATER_DATA_WRITER_H

#include <cstdio>
#include <memory>
#include <string>
#include <unistd.h>

namespace updater {
enum WriteMode : int {
    WRITE_SPARSE = 1,
    WRITE_RAW = 2,
    WRITE_DECRYPT = 3,
    WRITE_BLOCK = 4,
};

class DataWriter {
public:
    using DataWriterPtr = DataWriter *;
    virtual bool Write(const uint8_t *addr, size_t len, WriteMode mode, const std::string &partitionName) = 0;
    virtual int OpenPartition(const std::string &partitionName);
    virtual ~DataWriter() {}
    static std::unique_ptr<DataWriter> CreateDataWriter(WriteMode mode, const std::string &partitionName);
    static void ReleaseDataWriter(std::unique_ptr<DataWriter> &writer);
};

// Maybe we should read sector size from flash.
#define DEFAULT_SECTOR_SIZE (512)
} // updater
#endif // UPDATER_DATA_WRITER_H
