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

#include "ui_auto_test_group.h"
#include "graphic_config.h"
#include "test_render/ui_auto_test_render.h"

namespace OHOS {
List<UIAutoTest*> UIAutoTestGroup::testCaseList_;

void UIAutoTestGroup::SetUpTestCase()
{
    testCaseList_.PushBack(new UIAutoTestRender());
}

List<UIAutoTest*>& UIAutoTestGroup::GetTestCase()
{
    return testCaseList_;
}

void UIAutoTestGroup::TearDownTestCase()
{
    ListNode<UIAutoTest*>* node = testCaseList_.Begin();
    while (node != testCaseList_.End()) {
        delete node->data_;
        node->data_ = nullptr;
        node = node->next_;
    }
    testCaseList_.Clear();
}
} // namespace OHOS
