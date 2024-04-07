/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_PMS_INTERFACE_INNER_H
#define OHOS_PMS_INTERFACE_INNER_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*
 * Request the unique device ID of the device(UDID).
 * This function is applicable to the process with uid less than 1000, which is used when requesting a UDID.
 *
 * udid: It is used to store the UDID finally obtained. Its size must be greater than 65, including '\0'.
 * size: The size of array to store UDID. It must be 65.
 * Returns 0 if the UDID is successfully obtained, other returns an error code.
 */
int RequestDevUdid(unsigned char *udid, int size);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif // OHOS_PMS_INTERFACE_INNER_H

