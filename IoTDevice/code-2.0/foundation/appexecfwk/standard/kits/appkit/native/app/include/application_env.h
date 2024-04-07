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

#ifndef OHOS_APPEXECFWK_APPLICATION_ENV_H
#define OHOS_APPEXECFWK_APPLICATION_ENV_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @brief Obtains the bundle name of the application
 * @param -
 * @return Returns the pointer to the bundle name if the operation is successful; returns a null pointer otherwise.
 */
const char *GetBundleName();

/**
 * @brief Obtains the source code path of this application.
 * @param -
 * @return Returns the pointer to the source code path of this application.
 */
const char *GetSrcPath();

/**
 * @brief Obtains the data path of this application.
 * @param -
 * @return Returns the pointer to the data path of this application.
 */
const char *GetDataPath();

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // OHOS_ABILITY_ENV_H