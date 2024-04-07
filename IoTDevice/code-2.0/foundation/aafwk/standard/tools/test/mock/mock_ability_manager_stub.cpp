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

#include "mock_ability_manager_stub.h"

using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;

int MockAbilityManagerStub::StartAbility(const Want &want, int requestCode)
{
    HILOG_INFO("[%{public}s(%{public}s)] enter", __FILE__, __FUNCTION__);

    ElementName element = want.GetElement();

    std::string abilityName = element.GetAbilityName();
    HILOG_INFO("abilityName: %{public}s", abilityName.c_str());
    if (abilityName == STRING_ABILITY_NAME_INVALID) {
        return RESOLVE_ABILITY_ERR;
    }

    std::string bundleName = element.GetBundleName();
    HILOG_INFO("bundleName: %{public}s", bundleName.c_str());
    if (bundleName == STRING_BUNDLE_NAME_INVALID) {
        return RESOLVE_APP_ERR;
    }

    return ERR_OK;
}

void MockAbilityManagerStub::DumpState(const std::string &args, std::vector<std::string> &state)
{
    HILOG_INFO("[%{public}s(%{public}s)] enter", __FILE__, __FUNCTION__);

    std::vector<std::string> argList;
    SplitStr(args, " ", argList);

    std::string command = argList[0];
    if (command == "--all" || command == "-a") {
        // do nothing
    } else if (command == "--stack-list" || command == "-l") {
        // do nothing
    } else if (command == "--stack" || command == "-s") {
        state.push_back(argList[1]);
    } else if (command == "--mission" || command == "-m") {
        state.push_back(argList[1]);
    } else {
        // do nothing
    }
}

int MockAbilityManagerStub::StopServiceAbility(const Want &want)
{
    HILOG_INFO("[%{public}s(%{public}s)] enter", __FILE__, __FUNCTION__);

    ElementName element = want.GetElement();

    std::string abilityName = element.GetAbilityName();
    HILOG_INFO("abilityName: %{public}s", abilityName.c_str());
    if (abilityName == STRING_ABILITY_NAME_INVALID) {
        return RESOLVE_ABILITY_ERR;
    }

    std::string bundleName = element.GetBundleName();
    HILOG_INFO("bundleName: %{public}s", bundleName.c_str());
    if (bundleName == STRING_BUNDLE_NAME_INVALID) {
        return RESOLVE_APP_ERR;
    }

    return ERR_OK;
}
