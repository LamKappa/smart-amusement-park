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

#ifndef DEMO_ABILITY_H
#define DEMO_ABILITY_H
#include "ability.h"
#include "ability_loader.h"
#include "want.h"

namespace OHOS {
namespace AppExecFwk {
class DemoAbility : public Ability {
protected:
    virtual void OnStart(const Want &want) override;
    virtual void OnStop() override;
    virtual void OnActive() override;
    virtual void OnInactive() override;
    virtual void OnBackground() override;
    virtual void OnForeground(const Want &want) override;
    virtual void OnNewWant(const Want &want) override;
    virtual sptr<IRemoteObject> OnConnect(const Want &want) override;
    virtual void OnDisconnect(const Want &want) override;
    virtual void OnCommand(const AAFwk::Want &want, bool restart, int startId) override;

    virtual void OnRestoreAbilityState(const PacMap &inState);
    virtual void OnSaveAbilityState(PacMap &outState);
    virtual void OnAbilityResult(int requestCode, int resultCode, const Want &resultData);

    virtual std::vector<std::string> GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter);
    virtual int OpenFile(const Uri &uri, const std::string &mode);
    virtual int Delete(const Uri &uri, const DataAbilityPredicates &predicates);
    virtual int Insert(const Uri &uri, const ValuesBucket &value);
    virtual int Update(const Uri &uri, const ValuesBucket &value, const DataAbilityPredicates &predicates);
    virtual int OpenRawFile(const Uri &uri, const std::string &mode);
    virtual bool Reload(const Uri &uri, const PacMap &extras);
    virtual int BatchInsert(const Uri &uri, const std::vector<ValuesBucket> &values);
    virtual std::string GetType(const Uri &uri);
    virtual std::shared_ptr<ResultSet> Query(
        const Uri &uri, const std::vector<std::string> &columns, const DataAbilityPredicates &predicates);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // DEMO_ABILITY_H