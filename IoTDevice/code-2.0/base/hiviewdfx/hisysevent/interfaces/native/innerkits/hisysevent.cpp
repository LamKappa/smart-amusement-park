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

#include "hisysevent.h"

#include <chrono>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <securec.h>
#include "hilog/log.h"

namespace OHOS {
namespace HiviewDFX {
const std::string HiSysEvent::Domain::AAFWK = "AAFWK";
const std::string HiSysEvent::Domain::APPEXECFWK = "APPEXECFWK";
const std::string HiSysEvent::Domain::ACCOUNT = "ACCOUNT";
const std::string HiSysEvent::Domain::ACE = "ACE";
const std::string HiSysEvent::Domain::AI = "AI";
const std::string HiSysEvent::Domain::BARRIER_FREE = "BARRIERFREE";
const std::string HiSysEvent::Domain::BIOMETRICS = "BIOMETRICS";
const std::string HiSysEvent::Domain::CCRUNTIME = "CCRUNTIME";
const std::string HiSysEvent::Domain::COMMUNICATION = "COMMUNICATION";
const std::string HiSysEvent::Domain::DEVELOPTOOLS = "DEVELOPTOOLS";
const std::string HiSysEvent::Domain::DISTRIBUTED_DATAMGR = "DISTDATAMGR";
const std::string HiSysEvent::Domain::DISTRIBUTED_SCHEDULE = "DISTSCHEDULE";
const std::string HiSysEvent::Domain::GLOBAL = "GLOBAL";
const std::string HiSysEvent::Domain::GRAPHIC = "GRAPHIC";
const std::string HiSysEvent::Domain::HIVIEWDFX = "HIVIEWDFX";
const std::string HiSysEvent::Domain::IAWARE = "IAWARE";
const std::string HiSysEvent::Domain::INTELLI_ACCESSORIES = "INTELLIACC";
const std::string HiSysEvent::Domain::INTELLI_TV = "INTELLITV";
const std::string HiSysEvent::Domain::IVI_HARDWARE = "IVIHARDWARE";
const std::string HiSysEvent::Domain::LOCATION = "LOCATION";
const std::string HiSysEvent::Domain::MSDP = "MSDP";
const std::string HiSysEvent::Domain::MULTI_MEDIA = "MULTIMEDIA";
const std::string HiSysEvent::Domain::MULTI_MODAL_INPUT = "MULTIMODALINPUT";
const std::string HiSysEvent::Domain::NOTIFICATION = "NOTIFICATION";
const std::string HiSysEvent::Domain::POWERMGR = "POWERMGR";
const std::string HiSysEvent::Domain::ROUTER = "ROUTER";
const std::string HiSysEvent::Domain::SECURITY = "SECURITY";
const std::string HiSysEvent::Domain::SENSORS = "SENSORS";
const std::string HiSysEvent::Domain::SOURCE_CODE_TRANSFORMER = "SRCTRANSFORMER";
const std::string HiSysEvent::Domain::STARTUP = "STARTUP";
const std::string HiSysEvent::Domain::TELEPHONY = "TELEPHONY";
const std::string HiSysEvent::Domain::UPDATE = "UPDATE";
const std::string HiSysEvent::Domain::USB = "USB";
const std::string HiSysEvent::Domain::WEARABLE_HARDWARE = "WEARABLEHW";
const std::string HiSysEvent::Domain::WEARABLE = "WEARABLE";
const std::string HiSysEvent::Domain::OTHERS = "OTHERS";

static constexpr HiLogLabel LABEL = { LOG_CORE, 0xD002D08, "HISYSEVENT" };

static inline uint64_t GetMilliseconds()
{
    auto now = std::chrono::system_clock::now();
    auto millisecs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return millisecs.count();
}

void HiSysEvent::AppendHexData(std::stringstream &jsonStr, const std::string &key, uint64_t value)
{
    jsonStr << "\"" << key << "\":\"" << std::hex << value << "\"," << std::dec;
}

void HiSysEvent::AppendData(std::stringstream &jsonStr, const std::string &key, const std::string &value)
{
    jsonStr << "\"" << key << "\":\"" << value << "\",";
}

void HiSysEvent::WritebaseInfo(std::stringstream &jsonStr, const std::string &domain, const std::string &eventName,
    EventType type)
{
    AppendData(jsonStr, "domain_", domain);
    AppendData(jsonStr, "event_name_", eventName);
    AppendData(jsonStr, "event_type_", type);
    AppendData(jsonStr, "time_", GetMilliseconds());
    AppendData(jsonStr, "pid_", getpid());
    AppendData(jsonStr, "tid_", gettid());
}

void HiSysEvent::InnerWrite(std::stringstream &jsonStr, const std::string &domain, const std::string &eventName,
    EventType type)
{
    if (jsonStr.tellp() != 0) {
        jsonStr.seekp(-1, std::ios_base::end);
    }
}

void HiSysEvent::SendSysEvent(std::stringstream &jsonStr)
{
    struct sockaddr_un serverAddr;
    serverAddr.sun_family = AF_UNIX;
    if (strcpy_s(serverAddr.sun_path, sizeof(serverAddr.sun_path), "/dev/socket/hisysevent") != EOK) {
        HiLog::Error(LABEL, "can not assign server path");
        return;
    }
    serverAddr.sun_path[sizeof(serverAddr.sun_path) - 1] = '\0';

    int socketId = socket(AF_UNIX, SOCK_DGRAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if (socketId < 0) {
        HiLog::Error(LABEL, "create hisysevent client socket fail");
        return;
    }
    if (sendto(socketId, jsonStr.str().c_str(), jsonStr.str().length(), 0, reinterpret_cast<sockaddr*>(&serverAddr),
        sizeof(serverAddr)) < 0) {
        close(socketId);
        HiLog::Error(LABEL, "send data to hisysevent server fail");
        return;
    }
    close(socketId);
    HiLog::Debug(LABEL, "HiSysEvent has send data");
}
} // HiviewDFX
} // OHOS
