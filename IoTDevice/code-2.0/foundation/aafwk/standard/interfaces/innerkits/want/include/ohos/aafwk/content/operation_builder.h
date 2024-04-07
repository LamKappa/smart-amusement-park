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

#ifndef OHOS_AAFWK_OPERATION_BUILDER_H
#define OHOS_AAFWK_OPERATION_BUILDER_H
#include <string>
#include <vector>
#include <memory>
#include "uri.h"
#include "string_ex.h"

namespace OHOS {
namespace AAFwk {
class Operation;
class OperationBuilder {
public:
    OperationBuilder();
    ~OperationBuilder();

    /**
     * @description: Sets a AbilityName in an OperationBuilder.
     * @return Returns this OperationBuilder object containing the AbilityName.
     */
    OperationBuilder &WithAbilityName(const std::string &abilityName);

    /**
     * @description: Sets a BundleName in an OperationBuilder.
     * @return Returns this OperationBuilder object containing the BundleName.
     */
    OperationBuilder &WithBundleName(const std::string &bundleName);

    /**
     * @description: Sets a DeviceId in an OperationBuilder.
     * @return Returns this OperationBuilder object containing the DeviceId.
     */
    OperationBuilder &WithDeviceId(const std::string &deviceID);

    /**
     * @description: Sets a Action in an OperationBuilder.
     * @return Returns this OperationBuilder object containing the Action.
     */
    OperationBuilder &WithAction(const std::string &action);

    /**
     * @description: Sets a Entities in an OperationBuilder.
     * @return Returns this OperationBuilder object containing the Entities.
     */
    OperationBuilder &WithEntities(const std::vector<std::string> &entities);

    /**
     * @description: Sets a Flags in an OperationBuilder.
     * @return Returns this OperationBuilder object containing the Flags.
     */
    OperationBuilder &WithFlags(unsigned int flags);

    /**
     * @description: Sets a Uri in an OperationBuilder.
     * @return Returns this OperationBuilder object containing the Uri.
     */
    OperationBuilder &WithUri(const Uri &uri);
    std::shared_ptr<Operation> build();

private:
    std::string abilityName_ = "";
    std::string action_ = "";
    std::string bundleName_ = "";
    std::string deviceId_ = "";
    std::vector<std::string> entities_ = {};
    unsigned int flags_ = 0;
    Uri uri_;
};

}  // namespace AAFwk
}  // namespace OHOS

#endif  // OHOS_AAFWK_OPERATION_BUILDER_H