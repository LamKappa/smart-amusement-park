/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "dock/rotate_input_device.h"
#include "dock/focus_manager.h"

#if ENABLE_ROTATE_INPUT
namespace OHOS {
void RotateInputDevice::DispatchEvent(const DeviceData& data)
{
    UIView *view_ = FocusManager::GetInstance()->GetFocusedView();
    if (view_ == nullptr) {
        return;
    } else if (data.rotate == 0 && rotateStart) {
        view_->OnRotateEvent(0);
        rotateStart = false;
        return;
    } else if (MATH_ABS(data.rotate) < threshold_) {
        return;
    } else {
        view_->OnRotateEvent(data.rotate);
        rotateStart = true;
    }
}
} // namespace OHOS
#endif
