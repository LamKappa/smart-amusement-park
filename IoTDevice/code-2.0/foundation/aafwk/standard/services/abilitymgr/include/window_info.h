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

#ifndef OHOS_AAFWK_WINDOW_INFO_H
#define OHOS_AAFWK_WINDOW_INFO_H

#include <stdint.h>

namespace OHOS {
namespace AAFwk {
/**
 * @class WindowInfo
 * Record window info include window id and window state.
 */
class WindowInfo {
public:
    WindowInfo() = default;
    explicit WindowInfo(int windowToken) : windowToken_(windowToken)
    {}
    virtual ~WindowInfo()
    {}

    int32_t windowToken_ = -1;  // window id
    bool isVisible_ = false;
};
}  // namespace AAFwk
}  // namespace OHOS
#endif  // OHOS_AAFWK_WINDOW_INFO_H