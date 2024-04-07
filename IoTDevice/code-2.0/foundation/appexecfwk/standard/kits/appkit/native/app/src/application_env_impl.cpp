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

#include "application_env_impl.h"
#include "application_info.h"

namespace OHOS {
namespace AppExecFwk {

/**
 * @brief Sets L1 information about the runtime environment of the application to which the
 *        ability belongs, including the bundle name, source code path, and data path.
 * @param appInfo
 * @return void
 */
void ApplicationEnvImpl::SetAppInfo(const AppInfo &appInfo)
{
    bundleName_ = appInfo.bundleName;
    dataPath_ = appInfo.dataPath;
    srcPath_ = appInfo.srcPath;
}

/**
 * @brief Sets information about the runtime environment of the application to which the
 *        ability belongs, including the bundle name, source code path, and data path.
 * @param appInfo indicates
 * @return void
 */
void ApplicationEnvImpl::SetAppInfo(const ApplicationInfo &appInfo)
{
    bundleName_ = appInfo.bundleName;
    dataPath_ = appInfo.dataDir;
    srcPath_ = appInfo.codePath;
}

/**
 * @brief Gets the bundlename of the application's runtime environment
 * @param -
 * @return bundleName
 */
const std::string &ApplicationEnvImpl::GetBundleName() const
{
    return bundleName_;
}

/**
 * @brief Gets the SrcPath of the application's runtime environment
 * @param -
 * @return SrcPath
 */
const std::string &ApplicationEnvImpl::GetSrcPath() const
{
    return srcPath_;
}

/**
 * @brief Gets the DataPath of the application's runtime environment
 * @param -
 * @return DataPath
 */
const std::string &ApplicationEnvImpl::GetDataPath() const
{
    return dataPath_;
}

}  // namespace AppExecFwk
}  // namespace OHOS
