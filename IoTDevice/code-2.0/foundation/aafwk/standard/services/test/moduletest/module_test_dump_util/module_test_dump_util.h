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

#ifndef OHOS_MODULE_TEST_DUMP_UTIL_H
#define OHOS_MODULE_TEST_DUMP_UTIL_H

#include <mutex>
#include <vector>
#include <string>
#include <regex>
#include <unordered_map>
#include <algorithm>
#include <iterator>

namespace OHOS {
namespace MTUtil {

using str_vec = std::vector<std::string>;
using str_iter = std::vector<std::string>::iterator;
using dump_info_map = std::unordered_map<std::string, std::string>;
using std::regex;
using std::string;

class MTDumpUtil {
public:
    virtual ~MTDumpUtil();
    static std::shared_ptr<MTDumpUtil> GetInstance();

    /**
     *
     * @param  {string} args                  : type to find
     * @param  {std::vector<string>} dumpInfo : dump info
     * @param  {std::vector<string>} results  : all strings found that include @args
     * @return {int}                          : size of @results
     * @Introduction: Find all strings specified by @args in @dumpInfo and store them in @results.
     */
    size_t GetAll(const string &args, const str_vec &dumpInfo, str_vec &results);

    /**
     *
     * @param  {string} args                  : type to find
     * @param  {std::vector<string>} dumpInfo : dump info
     * @param  {string} result                : the first string found that includes @args
     * @return {str_iter}                         : find or not
     * @Introduction: Find the first string specified by @args in @dumpInfo and store it in @result.
     */
    str_iter GetFirst(const string &args, const str_vec &dumpInfo, const str_iter &begin, string &result);

    /**
     *
     * @param  {string} args                  : specific string to find
     * @param  {std::vector<string>} dumpInfo : dump info
     * @param  {str_iter} begin               : where to start searching
     * @return {str_iter}                     : iter pointing to string containing @args
     * @Introduction: Match the first @matchStr in @dumpInfo from @begin and return the iter.
     */
    str_iter GetSpecific(const string &args, const str_vec &dumpInfo, const str_iter &begin);

    /**
     *
     * @param  {str_vec} strVec_1          : the first vector of string
     * @param  {str_vec} strVec_2          : the second vector of string
     * @return {bool}                      : the comparison result of the the params
     * @Introduction: Return true if each item in @strVec_1 equals the corresponding one in @strVec_2.
     */
    bool CompStrVec(const str_vec &strVec_1, const str_vec &strVec_2);

    /**
     *
     * @param  {string} key          : known type used to find info
     * @param  {string} value        : value of the {key}
     * @param  {string} args         : type to find
     * @param  {str_vec} dumpInfo    : dump info
     * @return {string}              : string found in dumpInfo
     * @Introduction: Return the value specified by @args with @key and @value in @dumpInfo.
     */
    string GetBy(const string &key, const string &value, const string &args, const str_vec &dumpInfo);

    size_t GetBindingsByUri(const string &uri, const str_vec &dumpInfo, str_vec &result);

private:
    MTDumpUtil();
    static std::mutex mutex_;
    static std::shared_ptr<MTDumpUtil> instance_;
    std::unordered_map<string, regex> findRgx_;

    inline bool MatchRegex(const regex &regex, const string &str, string &result);
};

}  // namespace MTUtil
}  // namespace OHOS
#endif  // OHOS_MODULE_TEST_DUMP_UTIL_H
