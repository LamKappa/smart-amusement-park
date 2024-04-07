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
#ifndef OHOS_AAFWK_MATCH_TYPE_H
#define OHOS_AAFWK_MATCH_TYPE_H

namespace OHOS {
namespace AAFwk {
enum class MatchType {
    PATTERN_LITERAL = 0,
    PATTERN_PREFIX = 1,
    PATTERN_REGEX = 2,
    PATTERN_SIMPLE_GLOB = 3,
};
}
}  // namespace OHOS

#endif  // OHOS_AAFWK_MATCH_TYPE_H
