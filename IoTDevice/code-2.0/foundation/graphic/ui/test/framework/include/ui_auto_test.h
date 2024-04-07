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

#ifndef GRAPHIC_LITE_UI_AUTO_TEST_H
#define GRAPHIC_LITE_UI_AUTO_TEST_H

#include "components/ui_view.h"

namespace OHOS {
class UIAutoTest {
public:
    UIAutoTest() {}
    virtual ~UIAutoTest() {}

    virtual void RunTestList() = 0;
    virtual void Reset() const = 0;

    void ResetMainMenu() const;
    void EnterSubMenu(const char* id) const;
    void ClickViewById(const char* id) const;
    void DragViewToHead(const char* id) const;
    void CompareByBinary(const char* fileName) const;
};
} // namespace OHOS
#endif // GRAPHIC_LITE_UI_AUTO_TEST_H
