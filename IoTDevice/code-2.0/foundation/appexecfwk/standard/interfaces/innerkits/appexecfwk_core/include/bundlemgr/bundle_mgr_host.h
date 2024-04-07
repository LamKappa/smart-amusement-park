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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLEMGR_BUNDLE_MGR_HOST_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLEMGR_BUNDLE_MGR_HOST_H

#include <mutex>

#include "iremote_stub.h"
#include "nocopyable.h"

#include "appexecfwk_errors.h"
#include "bundle_mgr_interface.h"
#include "bundle_mgr_proxy.h"

namespace OHOS {
namespace AppExecFwk {

class BundleMgrHost : public IRemoteStub<IBundleMgr> {
public:
    BundleMgrHost() = default;
    virtual ~BundleMgrHost() = default;

    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
    /**
     * @attention This function is implement in the proxy side.
     */
    virtual int GetUidByBundleName([[maybe_unused]] const std::string &bundleName, const int userId) override;
    /**
     * @attention This function is implement in the proxy side.
     */
    virtual std::string GetAppType([[maybe_unused]] const std::string &bundleName) override;

private:
    /**
     * @brief Handles the GetApplicationInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetApplicationInfo(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetApplicationInfos function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetApplicationInfos(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetBundleInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundleInfo(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetBundleInfos function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundleInfos(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetBundleNameForUid function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundleNameForUid(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetBundleGids function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundleGids(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetBundleInfosByMetaData function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundleInfosByMetaData(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the QueryAbilityInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleQueryAbilityInfo(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the QueryAbilityInfoByUri function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleQueryAbilityInfoByUri(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the QueryKeepAliveBundleInfos function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleQueryKeepAliveBundleInfos(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetAbilityLabel function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetAbilityLabel(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the CheckIsSystemAppByUid function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleCheckIsSystemAppByUid(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetBundleArchiveInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundleArchiveInfo(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetHapModuleInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetHapModuleInfo(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetLaunchWantForBundle function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetLaunchWantForBundle(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the CheckPublicKeys function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleCheckPublicKeys(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the CheckPermission function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleCheckPermission(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetPermissionDef function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetPermissionDef(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetAllPermissionGroupDefs function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetAllPermissionGroupDefs(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetAppsGrantedPermissions function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetAppsGrantedPermissions(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the HasSystemCapability function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleHasSystemCapability(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetSystemAvailableCapabilities function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetSystemAvailableCapabilities(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the IsSafeMode function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleIsSafeMode(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the CleanBundleCacheFiles function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleCleanBundleCacheFiles(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the CleanBundleDataFiles function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleCleanBundleDataFiles(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the RegisterBundleStatusCallback function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleRegisterBundleStatusCallback(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the ClearBundleStatusCallback function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleClearBundleStatusCallback(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the UnregisterBundleStatusCallback function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleUnregisterBundleStatusCallback(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the DumpInfos function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleDumpInfos(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetBundleInstaller function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundleInstaller(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the IsApplicationEnabled function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleIsApplicationEnabled(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the SetApplicationEnabled function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleSetApplicationEnabled(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the CanRequestPermission function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleCanRequestPermission(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the RequestPermissionFromUser function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleRequestPermissionFromUser(Parcel &data, Parcel &reply);

private:
    /**
     * @brief Write a parcelabe vector objects to the proxy node.
     * @param parcelableVector Indicates the objects to be write.
     * @param reply Indicates the reply to be sent;
     * @return Returns true if objects send successfully; returns false otherwise.
     */
    template<typename T>
    bool WriteParcelableVector(std::vector<T> &parcelableVector, Parcel &reply);

    DISALLOW_COPY_AND_MOVE(BundleMgrHost);
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLEMGR_BUNDLE_MGR_HOST_H