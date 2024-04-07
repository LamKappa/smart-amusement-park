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

#include "application_env.h"
#include "application_env_impl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Obtains the bundle name of the application
 * @param -
 * @return Returns the pointer to the bundle name if the operation is successful; returns a null pointer otherwise.
 */
const char *GetBundleName()
{
    OHOS::AppExecFwk::ApplicationEnvImpl *pApplicationEnvIml = OHOS::AppExecFwk::ApplicationEnvImpl::GetInstance();
    const char *pBundleName = nullptr;

    if (pApplicationEnvIml) {
        pBundleName = pApplicationEnvIml->GetBundleName().c_str();
    }

    return pBundleName;
}

/**
 * @brief Obtains the source code path of this application.
 * @param -
 * @return Returns the pointer to the source code path of this application.
 */
const char *GetSrcPath()
{
    OHOS::AppExecFwk::ApplicationEnvImpl *pApplicationEnvIml = OHOS::AppExecFwk::ApplicationEnvImpl::GetInstance();
    const char *pSrcPath = nullptr;

    if (pApplicationEnvIml) {
        pSrcPath = pApplicationEnvIml->GetSrcPath().c_str();
    }

    return pSrcPath;
}

/**
 * @brief Obtains the data path of this application.
 * @param -
 * @return Returns the pointer to the data path of this application.
 */
const char *GetDataPath()
{
    OHOS::AppExecFwk::ApplicationEnvImpl *pApplicationEnvIml = OHOS::AppExecFwk::ApplicationEnvImpl::GetInstance();
    const char *pDataPath = nullptr;

    if (pApplicationEnvIml) {
        pDataPath = pApplicationEnvIml->GetDataPath().c_str();
    }

    return pDataPath;
}

#ifdef __cplusplus
}  // extern "C"
#endif
