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
#ifndef UPDATER_UI_FRAME_H
#define UPDATER_UI_FRAME_H

#include <list>
#include <map>
#include <thread>
#include "surface_dev.h"
#include "view.h"

namespace updater {
struct CmpByStartY {
    bool operator()(const View *v1, const View *v2) const
    {
        return v1->startY_ < v2->startY_;
    }
};

class Frame : public View {
public:
    Frame(unsigned int w, unsigned   int h, View::PixelFormat pixType, SurfaceDev *sfDev);

    ~Frame() override;

    void OnDraw() override;

    void ViewRegister(View *view);

    void DispatchKeyEvent(int key);
private:
    void FlushThreadLoop();

    void DownFoucs();

    void UpFoucs();

    void SendKey(int key);

    void ProcessKeyLoop();

    void DoEvent(int key);

    int currentActionIndex_ = 0;
    int maxActionIndex_ = 0;
    int listIndex_ = 0;
    int frameViewId = 0;
    bool flushFlag_ = false;
    bool needStop_ = false;
    bool keyEventNotify_ = false;
    SurfaceDev *sfDev_ = nullptr;
    std::thread flushLoop_;
    std::thread keyProcessLoop_;
    std::mutex frameMutex_;
    std::mutex keyMutex_;
    std::condition_variable_any mCondFlush_;
    std::condition_variable_any mCondKey_;
    std::list<int> keyFifo_;
    std::map<View*, int, CmpByStartY> viewMapList_;
};
} // namespace updater
#endif // UPDATER_UI_FRAME_H
