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

#define private public
#include "module_test_dump_util.h"
#undef private

namespace OHOS {
namespace MTUtil {
std::mutex MTDumpUtil::mutex_;
std::shared_ptr<MTDumpUtil> MTDumpUtil::instance_ = nullptr;

std::shared_ptr<MTDumpUtil> MTDumpUtil::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> lock_l(mutex_);
        if (instance_ == nullptr) {
            instance_ = std::make_shared<MTDumpUtil>();
        }
    }
    return instance_;
}

MTDumpUtil::MTDumpUtil()
{
    findRgx_["Intent"] = regex(".*intent\\[(.+)\\].*");
    findRgx_["AbilityName"] = regex(".*main name \\[(.+)\\].*");
    findRgx_["AppName"] = regex(".*app name \\[(.+)\\].*");
    findRgx_["BundleName"] = regex(".*bundle name \\[(.+)\\].*");
    findRgx_["AbilityType"] = regex(".*ability type \\[(.+)\\].*");
    findRgx_["PreAbilityName"] = regex(".*previous ability app name \\[(.+)\\].*");
    findRgx_["PreAppName"] = regex(".*previous ability file name \\[(.+)\\].*");
    findRgx_["NextAbilityName"] = regex(".*next ability app name \\[(.+)\\].*");
    findRgx_["NextAppName"] = regex(".*next ability file name \\[(.+)\\].*");
    findRgx_["State"] = regex(".*state #(.+)  .*");
    findRgx_["StartTime"] = regex(".*start time \\[(.+)\\].*");
    findRgx_["MissionRecordID"] = regex(".*MissionRecord ID #(.+)  bottom app.*");
    findRgx_["MissionBottomApp"] = regex(".*bottom app \\[(.+)\\].*");
    findRgx_["UserID"] = regex(".*User ID #(.+)\\].*");
    findRgx_["MissionStackID"] = regex(".*MissionStack ID #(.+) \\[.*");
    findRgx_["Uri"] = regex(".*uri \\[(.+)\\].*");
    findRgx_["AbilityRecordID"] = regex(".*AbilityRecord ID #(.+)   state.*");
    findRgx_["Bindings"] = regex(".+s: (\\d+).*");
    findRgx_["Component"] = regex(".*> (.+)   .+#.+");
    findRgx_["BindState"] = regex(".*> .+/.+ #(.+)");
}

MTDumpUtil::~MTDumpUtil()
{
    instance_ = nullptr;
    instance_.reset();
}

bool MTDumpUtil::CompStrVec(const str_vec &strVec_1, const str_vec &strVec_2)
{
    if (strVec_1.size() != strVec_2.size()) {
        return false;
    }
    for (unsigned int i = 0; i < strVec_1.size(); ++i) {
        if (strVec_1[i].compare(strVec_2[i]) != 0) {
            return false;
        }
    }
    return true;
}

str_iter MTDumpUtil::GetSpecific(const string &matchStr, const str_vec &dumpInfo, const str_iter &begin)
{
    auto checkCondition = [&matchStr](const str_vec::value_type &value) -> bool {
        return string::npos != value.find(matchStr);
    };
    str_iter end;
    std::advance(end, dumpInfo.size());
    return std::find_if(begin, end, checkCondition);
}

bool MTDumpUtil::MatchRegex(const regex &rgx, const string &text, string &result)
{
    std::smatch baseMatch;
    if (std::regex_match(text, baseMatch, rgx)) {
        if (baseMatch.size() == 2) {
            result = baseMatch[1].str();
            return true;
        }
    }
    return false;
}

size_t MTDumpUtil::GetAll(const string &args, const str_vec &dumpInfo, str_vec &results)
{
    results.clear();
    // args not exist
    if (findRgx_.find(args) == findRgx_.end()) {
        return 0;
    }
    string findResult;
    for (const auto &info : dumpInfo) {
        if (MatchRegex(findRgx_[args], info, findResult)) {
            results.emplace_back(findResult);
        }
    }
    return results.size();
}

str_iter MTDumpUtil::GetFirst(const string &args, const str_vec &dumpInfo, const str_iter &begin, string &result)
{
    result.clear();
    str_iter end;
    std::advance(end, dumpInfo.size());
    // args not exist
    if (findRgx_.find(args) == findRgx_.end()) {
        return end;
    }
    string findResult;
    for (auto it = begin; it != dumpInfo.end(); ++it) {
        if (MatchRegex(findRgx_[args], *it, findResult)) {
            result = std::move(findResult);
            return it;
        }
    }
    return end;
}

string MTDumpUtil::GetBy(const string &key, const string &value, const string &args, const str_vec &dumpInfo)
{
    str_vec items;
    size_t preSize = GetAll(key, dumpInfo, items);
    auto iter = GetSpecific(value, items, items.begin());
    if (iter != items.end() && preSize == GetAll(args, dumpInfo, items)) {
        return *iter;
    }
    return "";
}

size_t MTDumpUtil::GetBindingsByUri(const string &uri, const str_vec &dumpInfo, str_vec &result)
{
    result.clear();
    str_vec dump(dumpInfo);
    string bindings;
    auto uriBegin = GetSpecific("uri [" + uri + "]", dump, dump.begin());
    auto bindingsBegin = GetFirst("Bindings", dump, uriBegin, bindings) + 1;
    size_t ret = std::stoul("0" + bindings);
    std::for_each(bindingsBegin,
        (((bindingsBegin + ret) > dump.end()) ? (dump.end()) : (bindingsBegin + ret)),
        [&result](auto &&it) { result.push_back(it); });
    return ret;
}

}  // namespace MTUtil
}  // namespace OHOS
