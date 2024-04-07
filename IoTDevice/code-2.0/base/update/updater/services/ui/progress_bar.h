/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef UPDATER_UI_PROGRESS_BAR_H
#define UPDATER_UI_PROGRESS_BAR_H
#include "frame.h"
#include "view.h"

namespace updater {
class ProgressBar : public View {
public:
    ProgressBar(const int startX, const int startY, const int w, const int h, Frame *parent);
    ~ProgressBar() override {};
    void OnDraw() override;
    void SetProgressValue(int value);
private:
    void DrawProgress();
    BRGA888Pixel progressColor_ {};
    BRGA888Pixel normalColor_ {};
    int pValue_ { 0 };
    Frame *parent_ {};
};
} // namespace updater
#endif // UPDATER_UI_PROGRESS_BAR_H
