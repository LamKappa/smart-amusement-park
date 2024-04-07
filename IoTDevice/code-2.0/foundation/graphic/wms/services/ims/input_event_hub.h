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

#ifndef GRAPHIC_LITE_INPUT_EVENT_HUB_H
#define GRAPHIC_LITE_INPUT_EVENT_HUB_H

#include <map>
#include "gfx_utils/input_event_info.h"
#include "input-event-codes.h"
#include "input_manager.h"
#include "securec.h"

namespace OHOS {
/**
 * @brief Hub of input event.
 */
class InputEventHub {
using ReadCallback = void (*)(const RawEvent*);
public:
    static InputEventHub* GetInstance();

    /**
     * @brief SetUp hub. This operation will open all input devices.
     *
     */
    void SetUp();

    /**
     * @brief TearDown hub. This operation will close all input devices.
     *
     */
    void TearDown();

    /**
     * @brief Registration read callback.
     *
     * @param [in] callback callback of read callback.
     *
     * @returns return -1: register callbacke failed; return 0: register success.
     */
    int32_t RegisterReadCallback(ReadCallback callback)
    {
        if (callback == nullptr) {
            return -1;
        }
        readCallback_ = callback;
        return 0;
    }

private:
    static InputDevType GetDeviceType(uint32_t devIndex);
    static void EventCallback(const EventPackage **pkgs, uint32_t count, uint32_t devIndex);
    uint8_t ScanInputDevice();
    InputEventHub();
    ~InputEventHub() {}

    InputEventHub(const InputEventHub&) = delete;
    InputEventHub& operator=(const InputEventHub&) = delete;
    InputEventHub(InputEventHub&&) = delete;
    InputEventHub& operator=(InputEventHub&&) = delete;

    uint32_t mountDevIndex_[MAX_INPUT_DEVICE_NUM];
    uint32_t openDev_;
    RawEvent data_;

    static IInputInterface* inputInterface_;
    static InputReportEventCb callback_;
    static ReadCallback readCallback_;
};
} // namespace OHOS
#endif
