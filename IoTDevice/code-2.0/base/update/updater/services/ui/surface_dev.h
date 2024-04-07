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

#ifndef UPDATER_UI_SURFACE_DEV_H
#define UPDATER_UI_SURFACE_DEV_H
#include <cstdio>
#include <cstdlib>
#include <mutex>

namespace updater {
class SurfaceDev {
public:
    enum DevType {
        FB_DEVICE = 0,
        DRM_DEVICE,
    };

    explicit SurfaceDev(SurfaceDev::DevType deviceType);
    ~SurfaceDev();
    void Flip(void* buf);
    void GetScreenSize(int &w, int &h);
private:
    std::mutex mMutex;
};
} // namespace updater
#endif
