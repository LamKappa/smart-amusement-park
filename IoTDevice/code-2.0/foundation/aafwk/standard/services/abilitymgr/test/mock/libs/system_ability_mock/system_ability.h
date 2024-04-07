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

#ifndef FOUNDATION_AAFWK_SERVICES_TEST_MOCK_SYSTEM_ABILITY_H
#define FOUNDATION_AAFWK_SERVICES_TEST_MOCK_SYSTEM_ABILITY_H

#include "hilog/log.h"
#include "iremote_object.h"

namespace OHOS {

#define REGISTER_SYSTEM_ABILITY_BY_ID(a, b, c)
#define REGISTER_SYSTEM_ABILITY(abilityClassName, abilityId, runOnCreate)
#define DECLEAR_SYSTEM_ABILITY(className)

static constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0xD001100, "MockSystemAbility"};

class SystemAbility {
public:
    static bool MakeAndRegisterAbility(SystemAbility *)
    {
        return true;
    }

protected:
    virtual void OnStart()
    {
        HiviewDFX::HiLog::Debug(LABEL, "Mock SystemAbility OnStart called");
    }

    virtual void OnStop()
    {
        HiviewDFX::HiLog::Debug(LABEL, "Mock SystemAbility OnStop called");
    }

    bool Publish(sptr<IRemoteObject> systemAbility)
    {
        HiviewDFX::HiLog::Debug(LABEL, "Mock SystemAbility Publish called");
        systemAbility.ForceSetRefPtr(nullptr);
        // For test just mock to return true
        return true;
    }

    SystemAbility(bool runOnCreate = false)
    {
        HiviewDFX::HiLog::Debug(LABEL, "Mock SystemAbility default Creator called %d", runOnCreate);
    }

    SystemAbility(const int32_t serviceId, bool runOnCreate = false)
    {
        HiviewDFX::HiLog::Debug(LABEL, "Mock SystemAbility Creator called %d", runOnCreate);
    }

    virtual ~SystemAbility()
    {
        HiviewDFX::HiLog::Debug(LABEL, "Mock SystemAbility Destructor called");
    }
};

}  // namespace OHOS
#endif  // FOUNDATION_AAFWK_SERVICES_TEST_MOCK_SYSTEM_ABILITY_H
