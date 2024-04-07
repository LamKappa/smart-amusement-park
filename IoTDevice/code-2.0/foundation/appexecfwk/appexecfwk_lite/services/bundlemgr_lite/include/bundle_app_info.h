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

#ifndef BUNDLE_APP_INFO_H
#define BUNDLE_APP_INFO_H

#include "ohos_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef enum {
    BUNDLE_INSTALL_DOING = 0,
    BUNDLE_INSTALL_OK = 1,
    BUNDLE_INSTALL_FAIL = 2
} InstallState;

/* update bundle state to UI task */
typedef enum {
    BUNDLE_INSTALL,
    BUNDLE_UNINSTALL,
    BUNDLE_UPDATE,
} BundleState;

typedef struct {
    char *bundleName;
    char *label;
    char *smallIconPath;
    char *bigIconPath;
    InstallState installState;
    uint8 installProcess;
} BundleInstallMsg;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* BUNDLE_APP_INFO_H */