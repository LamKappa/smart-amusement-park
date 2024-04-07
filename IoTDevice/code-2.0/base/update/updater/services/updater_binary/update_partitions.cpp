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
#include "update_partitions.h"
#include <cerrno>
#include <cstdio>
#include <sstream>
#include <string>
#include "cJSON.h"
#include "log/log.h"
#include "updater/updater_const.h"
#include "utils.h"

using namespace std;
using namespace uscript;
using namespace hpackage;
using namespace updater;
constexpr int MIN_PARTITIONS_NUM = 2;
constexpr int MAX_PARTITIONS_NUM = 20;
namespace updater {
int UpdatePartitions::ParsePartitionInfo(const std::string &partitionInfo, PartitonList &newPartList) const
{
    cJSON* root = cJSON_Parse(partitionInfo.c_str());
    UPDATER_ERROR_CHECK(root != nullptr, "Error get root", return -1);
    cJSON* partitions = cJSON_GetObjectItem(root, "Partition");
    UPDATER_ERROR_CHECK(partitions != nullptr, "Error get Partitions", cJSON_Delete(root); return -1);
    int number = cJSON_GetArraySize(partitions);
    UPDATER_ERROR_CHECK(number > MIN_PARTITIONS_NUM, "Error partitions number < 3", cJSON_Delete(root); return -1);
    UPDATER_ERROR_CHECK(number < MAX_PARTITIONS_NUM, "Error partitions number > 20", cJSON_Delete(root); return -1);
    LOG(INFO) << "Partitions numbers " << number;
    int i = 0;
    for (i = 0; i < number; i++) {
        struct Partition* myPartition = static_cast<struct Partition*>(calloc(1, sizeof(struct Partition)));
        if (!myPartition) {
            LOG(ERROR) << "Allocate memory for partition failed: " << errno;
            cJSON_Delete(root);
            return 0;
        }
        cJSON* thisPartion = cJSON_GetArrayItem(partitions, i);
        UPDATER_ERROR_CHECK(thisPartion != nullptr, "Error get thisPartion", cJSON_Delete(root); break);

        cJSON* item = cJSON_GetObjectItem(thisPartion, "start");
        UPDATER_ERROR_CHECK(item != nullptr, "Error get start", cJSON_Delete(root); break);
        myPartition->start = item->valueint;

        item = cJSON_GetObjectItem(thisPartion, "length");
        UPDATER_ERROR_CHECK(item != nullptr, "Error get length", cJSON_Delete(root); break);
        myPartition->length = item->valueint;
        myPartition->partNum = 0;
        myPartition->devName = "mmcblk0px";

        item = cJSON_GetObjectItem(thisPartion, "partName");
        UPDATER_ERROR_CHECK(item != nullptr, "Error get partName", cJSON_Delete(root); break);
        myPartition->partName = (item->valuestring);

        item = cJSON_GetObjectItem(thisPartion, "fsType");
        UPDATER_ERROR_CHECK(item != nullptr, "Error get fsType", cJSON_Delete(root); break);
        myPartition->fsType = (item->valuestring);

        LOG(INFO) << "<start> <length> <devname> <partname> <fstype>";
        LOG(INFO) << myPartition->start << myPartition->length << myPartition->devName <<
            myPartition->partName << myPartition->fsType;
        newPartList.push_back(myPartition);
    }
    cJSON_Delete(root);
    return 1;
}

int UpdatePartitions::DoNewPartitions(PartitonList &newPartList)
{
    int ret = DoPartitions(newPartList);
    newPartList.clear();
    if (ret <= 0) {
        LOG(INFO) << "do_partitions FAIL ";
    } else if (ret == 1) {
        LOG(INFO) << "new partition == old partition ";
    } else if (ret > 1) {
        LOG(INFO) << "do_partitions success reboot";
#ifndef UPDATER_UT
        utils::DoReboot("updater");
#endif
    }
    return ret;
}

int32_t UpdatePartitions::Execute(uscript::UScriptEnv &env, uscript::UScriptContext &context)
{
    LOG(INFO) << "enter UpdatePartitions::Execute ";
    if (context.GetParamCount() != 1) {
        LOG(ERROR) << "Invalid UpdatePartitions::Execute param";
        return USCRIPT_INVALID_PARAM;
    }
    std::string filePath;
    int32_t ret = context.GetParam(0, filePath);
    if (ret != USCRIPT_SUCCESS) {
        LOG(ERROR) << "Fail to get filePath";
        return USCRIPT_INVALID_PARAM;
    } else {
        LOG(INFO) << "UpdatePartitions::Execute filePath " << filePath;
    }
    hpackage::PkgManager::StreamPtr outStream = nullptr;
    const FileInfo *info = env.GetPkgManager()->GetFileInfo(filePath);
    UPDATER_ERROR_CHECK(info != nullptr, "Error to get file info", return USCRIPT_ERROR_EXECUTE);
    std::string tmpPath = "/data/updater";
    std::string tmpPath1 = tmpPath + filePath;
    ret = env.GetPkgManager()->CreatePkgStream(outStream,
        tmpPath1, info->unpackedSize, PkgStream::PkgStreamType_Write);
    UPDATER_ERROR_CHECK(outStream != nullptr, "Error to create output stream", return USCRIPT_ERROR_EXECUTE);
    ret = env.GetPkgManager()->ExtractFile(filePath, outStream);
    UPDATER_ERROR_CHECK(ret == USCRIPT_SUCCESS, "Error to extract file",
        env.GetPkgManager()->ClosePkgStream(outStream); return USCRIPT_ERROR_EXECUTE);
    FILE *fp = fopen(tmpPath1.c_str(), "rb");
    if (!fp) {
        LOG(ERROR) << "Open " << tmpPath1 << " failed: " << errno;
        env.GetPkgManager()->ClosePkgStream(outStream);
        return USCRIPT_ERROR_EXECUTE;
    }
    char partitionInfo[MAX_LOG_BUF_SIZE];
    int32_t partitionCount = fread(partitionInfo, 1, MAX_LOG_BUF_SIZE, fp);
    fclose(fp);
    if (partitionCount <= LEAST_PARTITION_COUNT) {
        env.GetPkgManager()->ClosePkgStream(outStream);
        LOG(ERROR) << "Invalid partition size, too small";
        return USCRIPT_ERROR_EXECUTE;
    }
    PartitonList newPartList {};
    UPDATER_CHECK_ONLY_RETURN(ParsePartitionInfo(std::string(partitionInfo), newPartList),
        env.GetPkgManager()->ClosePkgStream(outStream); return USCRIPT_ABOART);
    if (newPartList.empty()) {
        LOG(ERROR) << "Partition is empty ";
        env.GetPkgManager()->ClosePkgStream(outStream);
        return USCRIPT_SUCCESS; // Partitions table is empty not require partition.
    }
    DoNewPartitions(newPartList);
    env.GetPkgManager()->ClosePkgStream(outStream);
    return USCRIPT_SUCCESS;
}
} // namespace updater
