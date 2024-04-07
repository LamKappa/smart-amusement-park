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

#ifndef FOUNDATION_APPEXECFWK_OHOS_ELEMENTS_CALLBACK_H
#define FOUNDATION_APPEXECFWK_OHOS_ELEMENTS_CALLBACK_H

#include "dummy_configuration.h"

namespace OHOS {
namespace AppExecFwk {

class ElementsCallback {
public:
    ElementsCallback() = default;
    virtual ~ElementsCallback() = default;

    /**
     *
     * Called when the system configuration of the device changes.
     *
     * @param config Indicates the new Configuration object.
     * @param ability Indicates the new Ability object.
     */
    virtual void OnConfigurationUpdated(const std::shared_ptr<Ability> &ability, const Configuration &config) = 0;

    /**
     *
     * Will be called when the system has determined to trim the memory
     *
     * @param level Indicates the memory trim level, which shows the current memory usage status.
     */
    virtual void OnMemoryLevel(int level) = 0;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_OHOS_ELEMENTS_CALLBACK_H