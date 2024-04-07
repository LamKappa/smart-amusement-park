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

#include "hiappevent_verify.h"

#include <iterator>
#include <regex>

#include "hiappevent_base.h"
#include "hilog/log.h"

using namespace OHOS::HiviewDFX::ErrorCode;

namespace OHOS {
namespace HiviewDFX {
namespace {
static constexpr HiLogLabel LABEL = { LOG_CORE, HIAPPEVENT_DOMAIN, "HiAppEvent_HDL" };

static constexpr int MAX_LENGTH_OF_EVENT_NAME = 32;
static constexpr int MAX_LENGTH_OF_PARAM_NAME = 16;
static constexpr int MAX_NUM_OF_PARAMS = 32;
static constexpr int MAX_LENGTH_OF_STR_PARAM = 256;
static constexpr int MAX_SIZE_OF_LIST_PARAM = 100;

static constexpr int HITRACE_PARAMS_NUM = 4;
static const std::string HITRACE_PARAMS[HITRACE_PARAMS_NUM] = {"traceid_", "spanid_", "pspanid_", "trace_flag_"};


bool CheckEventName(const std::string& eventName)
{
    if (eventName.empty() || eventName.length() > MAX_LENGTH_OF_EVENT_NAME) {
        HiLog::Error(LABEL, "eventName cannot be empty or exceed 32.");
        return false;
    }

    if (!std::regex_match(eventName, std::regex("^[a-zA-Z]\\w*$"))) {
        HiLog::Error(LABEL, "event name must start with a letter, consist of letters, digits and underscores.");
        return false;
    }

    return true;
}

bool CheckParamName(const std::string& paramName)
{
    if (paramName.empty() || paramName.length() > MAX_LENGTH_OF_PARAM_NAME) {
        HiLog::Error(LABEL, "paramName=%{public}s length cannot be empty or exceed 16.", paramName.c_str());
        return false;
    }

    for (int i = 0; i < HITRACE_PARAMS_NUM; i++) {
        if (paramName == HITRACE_PARAMS[i]) {
            return true;
        }
    }

    if (!std::regex_match(paramName, std::regex("^[a-zA-Z]\\w*[a-zA-Z0-9]$"))) {
        HiLog::Error(LABEL, "param name must start with a letter and cannot end with an underscore.");
        return false;
    }

    return true;
}

bool CheckStrParamLength(const std::string& strParamValue)
{
    if (strParamValue.empty()) {
        HiLog::Info(LABEL, "str param value is empty.");
        return true;
    }

    if (strParamValue.length() > MAX_LENGTH_OF_STR_PARAM) {
        HiLog::Error(LABEL, "str param value cannot exceed 256 characters.");
        return false;
    }

    return true;
}

bool CheckListValueSize(const AppEventParam& param)
{
    if (param.type <= AppEventParamType::STRING) {
        return true;
    }

    size_t size = 0;
    if (param.type == AppEventParamType::BVECTOR) {
        size = param.value.valueUnion.bs_.size();
    } else if (param.type == AppEventParamType::CVECTOR) {
        size = param.value.valueUnion.cs_.size();
    } else if (param.type == AppEventParamType::SHVECTOR) {
        size = param.value.valueUnion.shs_.size();
    } else if (param.type == AppEventParamType::IVECTOR) {
        size = param.value.valueUnion.is_.size();
    } else if (param.type == AppEventParamType::LVECTOR) {
        size = param.value.valueUnion.ls_.size();
    } else if (param.type == AppEventParamType::LLVECTOR) {
        size = param.value.valueUnion.lls_.size();
    } else if (param.type == AppEventParamType::FVECTOR) {
        size = param.value.valueUnion.fs_.size();
    } else if (param.type == AppEventParamType::DVECTOR) {
        size = param.value.valueUnion.ds_.size();
    } else if (param.type == AppEventParamType::STRVECTOR) {
        size = param.value.valueUnion.strs_.size();
    } else {
        HiLog::Error(LABEL, "key=%{public}s unknown event param type.", param.name.c_str());
        return false;
    }

    return (size > MAX_SIZE_OF_LIST_PARAM) ? false : true;
}

bool CheckStringLengthOfList(const std::vector<std::string>& strs)
{
    if (strs.empty()) {
        return true;
    }

    for (auto str : strs) {
        if (!CheckStrParamLength(str)) {
            HiLog::Error(LABEL, "the string length of the list cannot exceed 256.");
            return false;
        }
    }

    return true;
}

bool CheckParamsNum(std::list<AppEventParam>& baseParams)
{
    if (baseParams.size() == 0) {
        return true;
    }

    int maxParamsNum = MAX_NUM_OF_PARAMS;
    if (baseParams.begin()->name == HITRACE_PARAMS[0]) {
        maxParamsNum += HITRACE_PARAMS_NUM;
    }

    int listSize = baseParams.size();
    if (listSize > maxParamsNum) {
        auto delStartPtr = baseParams.begin();
        std::advance(delStartPtr, maxParamsNum);
        baseParams.erase(delStartPtr, baseParams.end());
        return false;
    }

    return true;
}
}

int VerifyAppEvent(std::shared_ptr<AppEventPack>& appEventPack)
{
    HiLog::Debug(LABEL, "start to verify app event.");

    if (!CheckEventName(appEventPack->GetEventName())) {
        HiLog::Error(LABEL, "eventName=%{public}s is invalid.", appEventPack->GetEventName().c_str());
        return ERROR_INVALID_EVENT_NAME;
    }

    int verifyRes = HIAPPEVENT_VERIFY_SUCCESSFUL;
    std::list<AppEventParam>& baseParams = appEventPack->baseParams_;
    if (!CheckParamsNum(baseParams)) {
        HiLog::Error(LABEL, "the number of params cannot exceed 32.");
        verifyRes = ERROR_INVALID_PARAM_NUM;
    }

    for (auto it = baseParams.begin(); it != baseParams.end();) {
        if (!CheckParamName(it->name)) {
            HiLog::Error(LABEL, "paramName=%{public}s is invalid.", it->name.c_str());
            verifyRes = ERROR_INVALID_PARAM_NAME;
            baseParams.erase(it++);
            continue;
        }

        if (it->type == AppEventParamType::STRING && !CheckStrParamLength(it->value.valueUnion.str_)) {
            HiLog::Error(LABEL, "key=%{public}s string value length cannot exceed 256.", it->name.c_str());
            verifyRes = ERROR_INVALID_PARAM_VALUE_LENGTH;
            baseParams.erase(it++);
            continue;
        }

        if (!CheckListValueSize(*it)) {
            HiLog::Error(LABEL, "key=%{public}s list size cannot exceed 100.", it->name.c_str());
            verifyRes = ERROR_INVALID_LIST_PARAM_SIZE;
            baseParams.erase(it++);
            continue;
        }

        if (it->type == AppEventParamType::STRVECTOR && !CheckStringLengthOfList(it->value.valueUnion.strs_)) {
            HiLog::Error(LABEL, "key=%{public}s list string length cannot exceed 256.", it->name.c_str());
            verifyRes = ERROR_INVALID_PARAM_VALUE_LENGTH;
            baseParams.erase(it++);
            continue;
        }
        it++;
    }

    HiLog::Debug(LABEL, "end the verification of app event.");
    return verifyRes;
}
} // HiviewDFX
} // OHOS