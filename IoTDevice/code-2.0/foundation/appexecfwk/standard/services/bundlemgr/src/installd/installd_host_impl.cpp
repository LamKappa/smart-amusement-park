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

#include "installd/installd_host_impl.h"

#include <memory>
#include <cstdio>
#include <fstream>
#include <map>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "directory_ex.h"
#include "bundle_constants.h"
#include "app_log_wrapper.h"
#include "installd/installd_operator.h"

namespace OHOS {
namespace AppExecFwk {

InstalldHostImpl::InstalldHostImpl()
{
    APP_LOGI("installd service instance is created");
}

InstalldHostImpl::~InstalldHostImpl()
{
    APP_LOGI("installd service instance is destroyed");
}

ErrCode InstalldHostImpl::CreateBundleDir(const std::string &bundleDir)
{
    if (bundleDir.empty()) {
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }
    if (InstalldOperator::IsExistDir(bundleDir)) {
        APP_LOGE("bundleDir %{public}s is exist", bundleDir.c_str());
        return ERR_APPEXECFWK_INSTALLD_CREATE_DIR_EXIST;
    }
    if (!InstalldOperator::MkRecursiveDir(bundleDir, true)) {
        APP_LOGE("create bundle dir %{public}s failed", bundleDir.c_str());
        return ERR_APPEXECFWK_INSTALLD_CREATE_DIR_FAILED;
    }
    return ERR_OK;
}

ErrCode InstalldHostImpl::RemoveBundleDir(const std::string &bundleDir)
{
    if (bundleDir.empty()) {
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }
    if (!InstalldOperator::DeleteDir(bundleDir)) {
        APP_LOGE("remove bundle dir %{public}s failed", bundleDir.c_str());
        return ERR_APPEXECFWK_INSTALLD_REMOVE_DIR_FAILED;
    }
    return ERR_OK;
}

ErrCode InstalldHostImpl::ExtractModuleFiles(const std::string &srcModulePath, const std::string &targetPath)
{
    if (srcModulePath.empty() || targetPath.empty()) {
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }
    if (!InstalldOperator::MkRecursiveDir(targetPath, true)) {
        APP_LOGE("create target dir %{public}s failed", targetPath.c_str());
        return ERR_APPEXECFWK_INSTALLD_EXTRACT_FILES_FAILED;
    }
    if (!InstalldOperator::ExtractFiles(srcModulePath, targetPath)) {
        APP_LOGE("extract %{public}s to %{public}s failed", srcModulePath.c_str(), targetPath.c_str());
        InstalldOperator::DeleteDir(targetPath);
        return ERR_APPEXECFWK_INSTALLD_EXTRACT_FILES_FAILED;
    }
    return ERR_OK;
}

ErrCode InstalldHostImpl::RemoveModuleDir(const std::string &moduleDir)
{
    if (moduleDir.empty()) {
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }
    if (!InstalldOperator::DeleteDir(moduleDir)) {
        APP_LOGE("remove module dir %{public}s failed", moduleDir.c_str());
        return ERR_APPEXECFWK_INSTALLD_REMOVE_DIR_FAILED;
    }
    return ERR_OK;
}

ErrCode InstalldHostImpl::RenameModuleDir(const std::string &oldPath, const std::string &newPath)
{
    APP_LOGI("rename %{public}s to %{public}s", oldPath.c_str(), newPath.c_str());
    if (oldPath.empty() || newPath.empty()) {
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }
    if (!InstalldOperator::RenameDir(oldPath, newPath)) {
        APP_LOGE("rename module dir %{public}s to %{public}s failed", oldPath.c_str(), newPath.c_str());
        return ERR_APPEXECFWK_INSTALLD_RNAME_DIR_FAILED;
    }
    return ERR_OK;
}

ErrCode InstalldHostImpl::CreateBundleDataDir(const std::string &bundleDir, const int uid, const int gid)
{
    if (bundleDir.empty() || uid < 0 || gid < 0) {
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }
    std::string createDir;
    if (bundleDir.back() != Constants::PATH_SEPARATOR[0]) {
        createDir = bundleDir + Constants::PATH_SEPARATOR;
    } else {
        createDir = bundleDir;
    }

    if (!InstalldOperator::MkOwnerDir(createDir + Constants::DATA_DIR, true, uid, gid)) {
        return ERR_APPEXECFWK_INSTALLD_CREATE_DIR_FAILED;
    }

    if (!InstalldOperator::MkOwnerDir(createDir + Constants::DATA_BASE_DIR, true, uid, gid)) {
        return ERR_APPEXECFWK_INSTALLD_CREATE_DIR_FAILED;
    }

    if (!InstalldOperator::MkOwnerDir(createDir + Constants::CACHE_DIR, true, uid, gid)) {
        return ERR_APPEXECFWK_INSTALLD_CREATE_DIR_FAILED;
    }

    if (!InstalldOperator::MkOwnerDir(createDir + Constants::SHARED_PREFERENCE_DIR, true, uid, gid)) {
        return ERR_APPEXECFWK_INSTALLD_CREATE_DIR_FAILED;
    }
    return ERR_OK;
}

ErrCode InstalldHostImpl::RemoveBundleDataDir(const std::string &bundleDataDir)
{
    if (bundleDataDir.empty()) {
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }
    if (!InstalldOperator::DeleteDir(bundleDataDir)) {
        APP_LOGE("remove bundle data dir %{public}s failed", bundleDataDir.c_str());
        return ERR_APPEXECFWK_INSTALLD_REMOVE_DIR_FAILED;
    }
    return ERR_OK;
}

ErrCode InstalldHostImpl::CreateModuleDataDir(
    const std::string &ModuleDir, const std::vector<std::string> &abilityDirs, const int uid, const int gid)
{
    if (ModuleDir.empty() || uid < 0 || gid < 0) {
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }
    std::string createDir;
    if (ModuleDir.back() != Constants::PATH_SEPARATOR[0]) {
        createDir = ModuleDir + Constants::PATH_SEPARATOR;
    } else {
        createDir = ModuleDir;
    }

    if (!InstalldOperator::MkOwnerDir(createDir + Constants::SHARED_DIR, true, uid, gid)) {
        return ERR_APPEXECFWK_INSTALLD_CREATE_DIR_FAILED;
    }

    for (auto &abilityDir : abilityDirs) {
        if (!InstalldOperator::MkOwnerDir(
                createDir + abilityDir + Constants::PATH_SEPARATOR + Constants::DATA_DIR,
                true,  
                uid, 
                gid)) {
            return ERR_APPEXECFWK_INSTALLD_CREATE_DIR_FAILED;
        }
        if (!InstalldOperator::MkOwnerDir(
                createDir + abilityDir + Constants::PATH_SEPARATOR + Constants::CACHE_DIR,
                true, 
                uid,  
                gid)) {
            return ERR_APPEXECFWK_INSTALLD_CREATE_DIR_FAILED;
        }
        if (!InstalldOperator::MkOwnerDir(
                createDir + abilityDir + Constants::PATH_SEPARATOR + Constants::DATA_BASE_DIR, 
                true, 
                uid, 
                gid)) {
            return ERR_APPEXECFWK_INSTALLD_CREATE_DIR_FAILED;
        }
        if (!InstalldOperator::MkOwnerDir(
                createDir + abilityDir + Constants::PATH_SEPARATOR + Constants::SHARED_PREFERENCE_DIR,
                true,
                uid,
                gid)) {
            return ERR_APPEXECFWK_INSTALLD_CREATE_DIR_FAILED;
        }
    }

    return ERR_OK;
}

ErrCode InstalldHostImpl::RemoveModuleDataDir(const std::string &moduleDataDir)
{
    if (moduleDataDir.empty()) {
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }
    if (!InstalldOperator::DeleteDir(moduleDataDir)) {
        APP_LOGE("remove module dir %{public}s failed", moduleDataDir.c_str());
        return ERR_APPEXECFWK_INSTALLD_REMOVE_DIR_FAILED;
    }
    return ERR_OK;
}

ErrCode InstalldHostImpl::CleanBundleDataDir(const std::string &dataDir)
{
    if (dataDir.empty()) {
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }

    if (!InstalldOperator::DeleteFiles(dataDir)) {
        return ERR_APPEXECFWK_INSTALLD_CLEAN_DIR_FAILED;
    }
    return ERR_OK;
}

}  // namespace AppExecFwk
}  // namespace OHOS