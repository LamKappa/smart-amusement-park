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
#include "operation_builder.h"
#include "operation.h"

namespace OHOS {
namespace AAFwk {

OperationBuilder::OperationBuilder()
    : abilityName_(""), action_(""), bundleName_(""), deviceId_(""), flags_(0), uri_("")
{
    entities_.clear();
}

OperationBuilder::~OperationBuilder()
{}

/**
 * @description: Sets a AbilityName in an OperationBuilder.
 * @return Returns this OperationBuilder object containing the AbilityName.
 */
OperationBuilder &OperationBuilder::WithAbilityName(const std::string &abilityName)
{
    abilityName_ = abilityName;
    return *this;
}

/**
 * @description: Sets a BundleName in an OperationBuilder.
 * @return Returns this OperationBuilder object containing the BundleName.
 */
OperationBuilder &OperationBuilder::WithBundleName(const std::string &bundleName)
{
    bundleName_ = bundleName;
    return *this;
}

/**
 * @description: Sets a DeviceId in an OperationBuilder.
 * @return Returns this OperationBuilder object containing the DeviceId.
 */
OperationBuilder &OperationBuilder::WithDeviceId(const std::string &deviceID)
{
    deviceId_ = deviceID;
    return *this;
}

/**
 * @description: Sets a Action in an OperationBuilder.
 * @return Returns this OperationBuilder object containing the Action.
 */
OperationBuilder &OperationBuilder::WithAction(const std::string &action)
{
    action_ = action;
    return *this;
}

/**
 * @description: Sets a Entities in an OperationBuilder.
 * @return Returns this OperationBuilder object containing the Entities.
 */
OperationBuilder &OperationBuilder::WithEntities(const std::vector<std::string> &entities)
{
    entities_ = entities;
    return *this;
}

/**
 * @description: Sets a Flags in an OperationBuilder.
 * @return Returns this OperationBuilder object containing the Flags.
 */
OperationBuilder &OperationBuilder::WithFlags(unsigned int flags)
{
    flags_ = flags;
    return *this;
}

/**
 * @description: Sets a Uri in an OperationBuilder.
 * @return Returns this OperationBuilder object containing the Uri.
 */
OperationBuilder &OperationBuilder::WithUri(const Uri &uri)
{
    uri_ = uri;
    return *this;
}

std::shared_ptr<Operation> OperationBuilder::build()
{
    std::shared_ptr<Operation> operation = std::make_shared<Operation>();
    operation->abilityName_ = abilityName_;
    operation->bundleName_ = bundleName_;
    operation->deviceId_ = deviceId_;
    operation->action_ = action_;
    operation->entities_ = entities_;
    operation->flags_ = flags_;
    operation->uri_ = uri_;

    return operation;
}

}  // namespace AAFwk
}  // namespace OHOS