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

#ifndef FOUNDATION_APPEXECFWK_KITS_APPKIT_TEST_MOCK_BUNDLE_MGR_SERVICE_H
#define FOUNDATION_APPEXECFWK_KITS_APPKIT_TEST_MOCK_BUNDLE_MGR_SERVICE_H

#include "gmock/gmock.h"
#include "semaphore_ex.h"
#include "bundle_mgr_host.h"

namespace OHOS {
namespace AppExecFwk {
class MockBundleMgrService : public BundleMgrHost {
public:
    MOCK_METHOD4(GetApplicationInfo,
        bool(const std::string &appName, const ApplicationFlag flag, const int userId, ApplicationInfo &appInfo));
    virtual bool GetApplicationInfos(
        const ApplicationFlag flag, const int userId, std::vector<ApplicationInfo> &appInfos) override
    {
        return false;
    }
    virtual bool GetBundleInfo(const std::string &bundleName, const BundleFlag flag, BundleInfo &bundleInfo) override
    {
        return false;
    }
    virtual bool GetBundleInfos(const BundleFlag flag, std::vector<BundleInfo> &bundleInfos) override
    {
        return false;
    }
    virtual int GetUidByBundleName(const std::string &bundleName, const int userId) override
    {
        return 0;
    }
    virtual bool GetBundleNameForUid(const int uid, std::string &bundleName) override
    {
        return false;
    }
    MOCK_METHOD2(GetBundleGids, bool(const std::string &bundleName, std::vector<int> &gids));
    std::string GetAppType(const std::string &bundleName)
    {
        GTEST_LOG_(INFO) << "MockBundleMgrService::GetAppType called";
        return "ModuleTestType";
    }
    virtual bool CheckIsSystemAppByUid(const int uid) override
    {
        return false;
    }
    MOCK_METHOD2(GetBundleInfosByMetaData, bool(const std::string &metaData, std::vector<BundleInfo> &bundleInfos));
    MOCK_METHOD2(QueryAbilityInfo, bool(const Want &want, AbilityInfo &abilityInfo));
    MOCK_METHOD2(QueryAbilityInfoByUri, bool(const std::string &abilityUri, AbilityInfo &abilityInfo));
    MOCK_METHOD1(QueryKeepAliveBundleInfos, bool(std::vector<BundleInfo> &bundleInfos));
    MOCK_METHOD2(GetAbilityLabel, std::string(const std::string &bundleName, const std::string &className));
    MOCK_METHOD3(
        GetBundleArchiveInfo, bool(const std::string &hapFilePath, const BundleFlag flag, BundleInfo &bundleInfo));
    MOCK_METHOD2(GetHapModuleInfo, bool(const std::string &hapFilePath, HapModuleInfo &hapModuleInfo));
    MOCK_METHOD2(GetHapModuleInfo, bool(const AbilityInfo &abilityInfo, HapModuleInfo &hapModuleInfo));
    MOCK_METHOD2(GetLaunchWantForBundle, bool(const std::string &bundleName, Want &want));
    MOCK_METHOD2(CheckPublicKeys, int(const std::string &firstBundleName, const std::string &secondBundleName));
    MOCK_METHOD2(CheckPermission, int(const std::string &bundleName, const std::string &permission));
    MOCK_METHOD2(GetPermissionDef, bool(const std::string &permissionName, PermissionDef &permissionDef));
    MOCK_METHOD1(GetAllPermissionGroupDefs, bool(std::vector<PermissionDef> &permissionDefs));
    MOCK_METHOD2(GetAppsGrantedPermissions,
        bool(const std::vector<std::string> &permissions, std::vector<std::string> &appNames));
    MOCK_METHOD1(HasSystemCapability, bool(const std::string &capName));
    MOCK_METHOD1(GetSystemAvailableCapabilities, bool(std::vector<std::string> &systemCaps));
    MOCK_METHOD0(IsSafeMode, bool());
    MOCK_METHOD2(CleanBundleCacheFiles,
        bool(const std::string &bundleName, const sptr<ICleanCacheCallback> &cleanCacheCallback));
    MOCK_METHOD1(CleanBundleDataFiles, bool(const std::string &bundleName));
    MOCK_METHOD1(RegisterBundleStatusCallback, bool(const sptr<IBundleStatusCallback> &bundleStatusCallback));
    MOCK_METHOD1(ClearBundleStatusCallback, bool(const sptr<IBundleStatusCallback> &bundleStatusCallback));
    MOCK_METHOD0(UnregisterBundleStatusCallback, bool());
    virtual bool DumpInfos(const DumpFlag flag, const std::string &bundleName, std::string &result) override
    {
        return false;
    }
    MOCK_METHOD0(GetBundleInstaller, sptr<IBundleInstaller>());
    MOCK_METHOD1(GetBundleInstaller, bool(const std::string &));
    MOCK_METHOD2(SetApplicationEnabled, bool(const std::string &, bool));
    MOCK_METHOD1(IsApplicationEnabled, bool(const std::string &));

    virtual bool CanRequestPermission(
        const std::string &bundleName, const std::string &permissionName, const int userId) override
    {
        return true;
    }
    virtual bool RequestPermissionFromUser(
        const std::string &bundleName, const std::string &permission, const int userId) override
    {
        return true;
    }
    void Wait()
    {
        sem_.Wait();
    }

    int Post()
    {
        sem_.Post();
        return 0;
    }

    void PostVoid()
    {
        sem_.Post();
    }

private:
    Semaphore sem_;
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_KITS_APPKIT_TEST_MOCK_BUNDLE_MGR_SERVICE_H