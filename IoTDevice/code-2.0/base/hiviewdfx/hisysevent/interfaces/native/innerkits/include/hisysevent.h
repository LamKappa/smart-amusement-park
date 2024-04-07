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

#ifndef HI_SYS_EVENT_H
#define HI_SYS_EVENT_H
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

namespace OHOS {
namespace HiviewDFX {
class HiSysEvent {
public:
    // system event domain list
    class Domain {
    public:
        static const std::string AAFWK;
        static const std::string APPEXECFWK;
        static const std::string ACCOUNT;
        static const std::string ACE;
        static const std::string AI;
        static const std::string BARRIER_FREE;
        static const std::string BIOMETRICS;
        static const std::string CCRUNTIME;
        static const std::string COMMUNICATION;
        static const std::string DEVELOPTOOLS;
        static const std::string DISTRIBUTED_DATAMGR;
        static const std::string DISTRIBUTED_SCHEDULE;
        static const std::string GLOBAL;
        static const std::string GRAPHIC;
        static const std::string HIVIEWDFX;
        static const std::string IAWARE;
        static const std::string INTELLI_ACCESSORIES;
        static const std::string INTELLI_TV;
        static const std::string IVI_HARDWARE;
        static const std::string LOCATION;
        static const std::string MSDP;
        static const std::string MULTI_MEDIA;
        static const std::string MULTI_MODAL_INPUT;
        static const std::string NOTIFICATION;
        static const std::string POWERMGR;
        static const std::string ROUTER;
        static const std::string SECURITY;
        static const std::string SENSORS;
        static const std::string SOURCE_CODE_TRANSFORMER;
        static const std::string STARTUP;
        static const std::string TELEPHONY;
        static const std::string UPDATE;
        static const std::string USB;
        static const std::string WEARABLE_HARDWARE;
        static const std::string WEARABLE;
        static const std::string OTHERS;
    };

public:
    enum EventType {
        FAULT     = 1,    // system fault event
        STATISTIC = 2,    // system statistic event
        SECURITY  = 3,    // system security event
        BEHAVIOR  = 4     // system behavior event
    };

    /**
     * @brief write system event
     * @param domain    system event domain name
     * @param eventName system event name
     * @param type      system event type
     * @param keyValues system event parameter name or value
     * @return 0 success, other fail
     */
    template<typename... Types> static int Write(const std::string &domain, const std::string &eventName,
        EventType type, Types... keyValues)
    {
        std::stringstream jsonStr;
        jsonStr << "{";
        WritebaseInfo(jsonStr, domain, eventName, type);
        InnerWrite(jsonStr, domain, eventName, type, keyValues...);
        jsonStr << "}";
        SendSysEvent(jsonStr);
        return 0;
    }

private:
    template<typename... Types> static void InnerWrite(std::stringstream &jsonStr, const std::string &domain,
        const std::string &eventName, EventType type, const std::string &key, bool value, Types... keyValues)
    {
        AppendData<bool>(jsonStr, key, value);
        InnerWrite(jsonStr, domain, eventName, type, keyValues...);
    }

    template<typename... Types> static void InnerWrite(std::stringstream &jsonStr, const std::string &domain,
        const std::string &eventName, EventType type, const std::string &key, const char value, Types... keyValues)
    {
        AppendData<short>(jsonStr, key, value);
        InnerWrite(jsonStr, domain, eventName, type, keyValues...);
    }

    template<typename... Types> static void InnerWrite(std::stringstream &jsonStr, const std::string &domain,
        const std::string &eventName, EventType type, const std::string &key, const unsigned char value,
        Types... keyValues)
    {
        AppendData<unsigned short>(jsonStr, key, value);
        InnerWrite(jsonStr, domain, eventName, type, keyValues...);
    }

    template<typename... Types> static void InnerWrite(std::stringstream &jsonStr, const std::string &domain,
        const std::string &eventName, EventType type, const std::string &key, const short value, Types... keyValues)
    {
        AppendData<short>(jsonStr, key, value);
        InnerWrite(jsonStr, domain, eventName, type, keyValues...);
    }

    template<typename... Types> static void InnerWrite(std::stringstream &jsonStr, const std::string &domain,
        const std::string &eventName, EventType type, const std::string &key, const unsigned short value,
        Types... keyValues)
    {
        AppendData<unsigned short>(jsonStr, key, value);
        InnerWrite(jsonStr, domain, eventName, type, keyValues...);
    }

    template<typename... Types> static void InnerWrite(std::stringstream &jsonStr, const std::string &domain,
        const std::string &eventName, EventType type, const std::string &key, const int value, Types... keyValues)
    {
        AppendData<int>(jsonStr, key, value);
        InnerWrite(jsonStr, domain, eventName, type, keyValues...);
    }

    template<typename... Types> static void InnerWrite(std::stringstream &jsonStr, const std::string &domain,
        const std::string &eventName, EventType type, const std::string &key, const unsigned int value,
        Types... keyValues)
    {
        AppendData<unsigned int>(jsonStr, key, value);
        InnerWrite(jsonStr, domain, eventName, type, keyValues...);
    }

    template<typename... Types> static void InnerWrite(std::stringstream &jsonStr, const std::string &domain,
        const std::string &eventName, EventType type, const std::string &key, const long value, Types... keyValues)
    {
        AppendData<long>(jsonStr, key, value);
        InnerWrite(jsonStr, domain, eventName, type, keyValues...);
    }

    template<typename... Types> static void InnerWrite(std::stringstream &jsonStr, const std::string &domain,
        const std::string &eventName, EventType type, const std::string &key, const unsigned long value,
        Types... keyValues)
    {
        AppendData<unsigned long>(jsonStr, key, value);
        InnerWrite(jsonStr, domain, eventName, type, keyValues...);
    }

    template<typename... Types> static void InnerWrite(std::stringstream &jsonStr, const std::string &domain,
        const std::string &eventName, EventType type, const std::string &key, const long long value,
        Types... keyValues)
    {
        AppendData<long long>(jsonStr, key, value);
        InnerWrite(jsonStr, domain, eventName, type, keyValues...);
    }

    template<typename... Types> static void InnerWrite(std::stringstream &jsonStr, const std::string &domain,
        const std::string &eventName, EventType type, const std::string &key, const unsigned long long value,
        Types... keyValues)
    {
        AppendData<unsigned long long>(jsonStr, key, value);
        InnerWrite(jsonStr, domain, eventName, type, keyValues...);
    }

    template<typename... Types> static void InnerWrite(std::stringstream &jsonStr, const std::string &domain,
        const std::string &eventName, EventType type, const std::string &key, const float value,
        Types... keyValues)
    {
        AppendData<float>(jsonStr, key, value);
        InnerWrite(jsonStr, domain, eventName, type, keyValues...);
    }

    template<typename... Types> static void InnerWrite(std::stringstream &jsonStr, const std::string &domain,
        const std::string &eventName, EventType type, const std::string &key, const double value,
        Types... keyValues)
    {
        AppendData<double>(jsonStr, key, value);
        InnerWrite(jsonStr, domain, eventName, type, keyValues...);
    }

    template<typename... Types> static void InnerWrite(std::stringstream &jsonStr, const std::string &domain,
        const std::string &eventName, EventType type, const std::string &key, const std::string &value,
        Types... keyValues)
    {
        AppendData(jsonStr, key, value);
        InnerWrite(jsonStr, domain, eventName, type, keyValues...);
    }

    template<typename... Types> static void InnerWrite(std::stringstream &jsonStr, const std::string &domain,
        const std::string &eventName, EventType type, const std::string &key, const char *value,
        Types... keyValues)
    {
        AppendData(jsonStr, key, std::string(value));
        InnerWrite(jsonStr, domain, eventName, type, keyValues...);
    }

    template<typename... Types> static void InnerWrite(std::stringstream &jsonStr, const std::string &domain,
        const std::string &eventName, EventType type, const std::string &key, const std::vector<bool> &value,
        Types... keyValues)
    {
        AppendArrayData<bool>(jsonStr, key, value);
        InnerWrite(jsonStr, domain, eventName, type, keyValues...);
    }

    template<typename... Types> static void InnerWrite(std::stringstream &jsonStr, const std::string &domain,
        const std::string &eventName, EventType type, const std::string &key, const std::vector<char> &value,
        Types... keyValues)
    {
        AppendArrayData(jsonStr, key, value);
        InnerWrite(jsonStr, domain, eventName, type, keyValues...);
    }

    template<typename... Types> static void InnerWrite(std::stringstream &jsonStr, const std::string &domain,
        const std::string &eventName, EventType type, const std::string &key, const std::vector<unsigned char> &value,
        Types... keyValues)
    {
        AppendArrayData(jsonStr, key, value);
        InnerWrite(jsonStr, domain, eventName, type, keyValues...);
    }

    template<typename... Types> static void InnerWrite(std::stringstream &jsonStr, const std::string &domain,
        const std::string &eventName, EventType type, const std::string &key, const std::vector<short> &value,
        Types... keyValues)
    {
        AppendArrayData<short>(jsonStr, key, value);
        InnerWrite(jsonStr, domain, eventName, type, keyValues...);
    }

    template<typename... Types> static void InnerWrite(std::stringstream &jsonStr, const std::string &domain,
        const std::string &eventName, EventType type, const std::string &key, const std::vector<unsigned short> &value,
        Types... keyValues)
    {
        AppendArrayData<unsigned short>(jsonStr, key, value);
        InnerWrite(jsonStr, domain, eventName, type, keyValues...);
    }

    template<typename... Types> static void InnerWrite(std::stringstream &jsonStr, const std::string &domain,
        const std::string &eventName, EventType type, const std::string &key, const std::vector<int> &value,
        Types... keyValues)
    {
        AppendArrayData<int>(jsonStr, key, value);
        InnerWrite(jsonStr, domain, eventName, type, keyValues...);
    }

    template<typename... Types> static void InnerWrite(std::stringstream &jsonStr, const std::string &domain,
        const std::string &eventName, EventType type, const std::string &key, const std::vector<unsigned int> &value,
        Types... keyValues)
    {
        AppendArrayData<unsigned int>(jsonStr, key, value);
        InnerWrite(jsonStr, domain, eventName, type, keyValues...);
    }

    template<typename... Types> static void InnerWrite(std::stringstream &jsonStr, const std::string &domain,
        const std::string &eventName, EventType type, const std::string &key, const std::vector<long> &value,
        Types... keyValues)
    {
        AppendArrayData<long>(jsonStr, key, value);
        InnerWrite(jsonStr, domain, eventName, type, keyValues...);
    }

    template<typename... Types> static void InnerWrite(std::stringstream &jsonStr, const std::string &domain,
        const std::string &eventName, EventType type, const std::string &key, const std::vector<unsigned long> &value,
        Types... keyValues)
    {
        AppendArrayData<unsigned long>(jsonStr, key, value);
        InnerWrite(jsonStr, domain, eventName, type, keyValues...);
    }

    template<typename... Types> static void InnerWrite(std::stringstream &jsonStr, const std::string &domain,
        const std::string &eventName, EventType type, const std::string &key, const std::vector<long long> &value,
        Types... keyValues)
    {
        AppendArrayData<long long>(jsonStr, key, value);
        InnerWrite(jsonStr, domain, eventName, type, keyValues...);
    }

    template<typename... Types> static void InnerWrite(std::stringstream &jsonStr, const std::string &domain,
        const std::string &eventName, EventType type,
        const std::string &key, const std::vector<unsigned long long> &value, Types... keyValues)
    {
        AppendArrayData<unsigned long long>(jsonStr, key, value);
        InnerWrite(jsonStr, domain, eventName, type, keyValues...);
    }

    template<typename... Types> static void InnerWrite(std::stringstream &jsonStr, const std::string &domain,
        const std::string &eventName, EventType type, const std::string &key, const std::vector<float> &value,
        Types... keyValues)
    {
        AppendArrayData<float>(jsonStr, key, value);
        InnerWrite(jsonStr, domain, eventName, type, keyValues...);
    }

    template<typename... Types> static void InnerWrite(std::stringstream &jsonStr, const std::string &domain,
        const std::string &eventName, EventType type, const std::string &key, const std::vector<double> &value,
        Types... keyValues)
    {
        AppendArrayData<double>(jsonStr, key, value);
        InnerWrite(jsonStr, domain, eventName, type, keyValues...);
    }

    template<typename... Types> static void InnerWrite(std::stringstream &jsonStr, const std::string &domain,
        const std::string &eventName, EventType type, const std::string &key, const std::vector<std::string> &value,
        Types... keyValues)
    {
        AppendArrayData(jsonStr, key, value);
        InnerWrite(jsonStr, domain, eventName, type, keyValues...);
    }

    template<typename T> static void AppendData(std::stringstream &jsonStr, const std::string &key, T value)
    {
        jsonStr << "\"" << key << "\":" << value << ",";
    }

    static void AppendArrayData(std::stringstream &jsonStr,
        const std::string &key, const std::vector<std::string> &value)
    {
        if (value.empty()) {
            jsonStr << "\"" << key << "\":[]";
            return;
        }
        jsonStr << "\"" << key << "\":[";
        for (auto item = value.begin(); item != value.end(); item++) {
            jsonStr << "\"" << (*item) << "\",";
        }
        if (jsonStr.tellp() != 0) {
            jsonStr.seekp(-1, std::ios_base::end);
        }
        jsonStr << "],";
    }

    static void AppendArrayData(std::stringstream &jsonStr, const std::string &key, const std::vector<char> &value)
    {
        if (value.empty()) {
            jsonStr << "\"" << key << "\":[]";
            return;
        }
        jsonStr << "\"" << key << "\":[";
        for (auto item = value.begin(); item != value.end(); item++) {
            jsonStr << static_cast<short>(*item) << ",";
        }
        if (jsonStr.tellp() != 0) {
            jsonStr.seekp(-1, std::ios_base::end);
        }
        jsonStr << "],";
    }

    static void AppendArrayData(std::stringstream &jsonStr,
        const std::string &key, const std::vector<unsigned char> &value)
    {
        if (value.empty()) {
            jsonStr << "\"" << key << "\":[]";
            return;
        }
        jsonStr << "\"" << key << "\":[";
        for (auto item = value.begin(); item != value.end(); item++) {
            jsonStr << static_cast<short>(*item) << ",";
        }
        if (jsonStr.tellp() != 0) {
            jsonStr.seekp(-1, std::ios_base::end);
        }
        jsonStr << "],";
    }

    template<typename T> static void AppendArrayData(std::stringstream &jsonStr,
        const std::string &key, const std::vector<T> &value)
    {
        if (value.empty()) {
            jsonStr << "\"" << key << "\":[]";
            return;
        }
        jsonStr << "\"" << key << "\":[";
        for (auto item = value.begin(); item != value.end(); item++) {
            jsonStr << (*item) << ",";
        }
        if (jsonStr.tellp() != 0) {
            jsonStr.seekp(-1, std::ios_base::end);
        }
        jsonStr << "],";
    }

    static void AppendHexData(std::stringstream &jsonStr, const std::string &key, uint64_t value);
    static void AppendData(std::stringstream &jsonStr, const std::string &key, const std::string &value);
    static void WritebaseInfo(std::stringstream &jsonStr, const std::string &domain, const std::string &eventName,
        EventType type);
    static void InnerWrite(std::stringstream &jsonStr, const std::string &domain, const std::string &eventName,
        EventType type);
    static void SendSysEvent(std::stringstream &jsonStr);
}; // HiSysEvent
} // HiviewDFX
} // OHOS
#endif // HI_SYS_EVENT_H