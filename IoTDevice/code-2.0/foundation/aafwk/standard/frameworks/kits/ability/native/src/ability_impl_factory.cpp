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

#include "ability_impl_factory.h"
#include "app_log_wrapper.h"
#include "data_ability_impl.h"
#include "page_ability_impl.h"
#include "service_ability_impl.h"

namespace OHOS {
namespace AppExecFwk {
/**
 *
 * default constructor
 *
 */
AbilityImplFactory::AbilityImplFactory()
{}

/**
 *
 * default Destructor
 *
 */
AbilityImplFactory::~AbilityImplFactory()
{}

/**
 * @brief Create impl object based on abilitytype
 *
 * @param type AbilityType:PAGE/SERVICE/PROVIDER
 *
 * @return AbilityImpl object
 */
std::shared_ptr<AbilityImpl> AbilityImplFactory::MakeAbilityImplObject(const std::shared_ptr<AbilityInfo> &info)
{
    if (info == nullptr) {
        APP_LOGE("AbilityImplFactory::MakeAbilityImplObject is error nullptr == info ");
        return nullptr;
    }

    std::shared_ptr<AbilityImpl> abilityImpl = nullptr;
    APP_LOGI("AbilityImplFactory::MakeAbilityImplObject type:%{public}d", info->type);

    switch (info->type) {
        case AppExecFwk::AbilityType::PAGE:
            abilityImpl = std::make_shared<PageAbilityImpl>();
            break;
        case AppExecFwk::AbilityType::SERVICE:
            abilityImpl = std::make_shared<ServiceAbilityImpl>();
            break;
        case AppExecFwk::AbilityType::DATA:
            abilityImpl = std::make_shared<DataAbilityImpl>();
            break;
        default:
            APP_LOGE("AbilityImplFactory::MakeAbilityImplObject is error");
            break;
    }

    return abilityImpl;
}
}  // namespace AppExecFwk
}  // namespace OHOS