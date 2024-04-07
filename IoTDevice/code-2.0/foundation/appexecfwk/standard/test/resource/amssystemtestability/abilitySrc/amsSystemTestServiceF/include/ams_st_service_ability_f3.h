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

#ifndef AMS_ST_SERVICE_ABILITY_F3_
#define AMS_ST_SERVICE_ABILITY_F3_
#include <string>
#include "ability_loader.h"

namespace OHOS {
namespace AppExecFwk {
class AmsStServiceAbilityF3 : public Ability {
protected:
    virtual void OnStart(const Want &want) override;
    virtual void OnStop() override;
    virtual void OnActive() override;
    virtual void OnInactive() override;
    virtual void OnBackground() override;
    virtual void OnNewWant(const Want &want) override;
    virtual void OnCommand(const AAFwk::Want &want, bool restart, int startId) override;
    virtual std::vector<std::string> GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter) override;

private:
    void Clear();
    void GetWantInfo(const Want &want);
    bool PublishEvent(const std::string &eventName, const int &code, const std::string &data);

    std::string shouldReturn_ = {};
    std::string targetBundle_ = {};
    std::string targetAbility_ = {};
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // AMS_ST_SERVICE_ABILITY_F3_