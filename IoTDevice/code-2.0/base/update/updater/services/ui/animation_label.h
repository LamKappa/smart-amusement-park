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
#ifndef UPDATER_UI_ANIMATION_LABLE_H
#define UPDATER_UI_ANIMATION_LABLE_H
#include <ctime>
#include <thread>
#include <unistd.h>
#include <vector>
#include "frame.h"
#include "png.h"

namespace updater {
constexpr int IMG_LIST_MAX_SIZE = 255;
constexpr int PNG_HEADER_SIZE = 8;
constexpr useconds_t SECOND_PER_MS = 1000;
constexpr int MAX_PICTURE_CHANNELS = 3;
constexpr int MAX_BIT_DEPTH = 8;
class AnimationLable : public View {
public:
    enum PlayMode {
        ANIMATION_MODE = 0,
        STATIC_MODE,
    };
    AnimationLable(int startX, int startY, int w, int h, Frame *parent);
    ~AnimationLable() override;
public:
    void AddImg(const std::string &imgFileName);
    int AddStaticImg(const std::string &imgFileName);
    void SetStaticImg(int picId);
    void SetPlayMode(AnimationLable::PlayMode mode);
    void SetInterval(int ms);
    void Start();
    void Stop();
    std::thread updateThread;
    bool selectable = false;
private:
    struct PictureAttr {
        png_uint_32 pictureWidth;
        png_uint_32 pictureHeight;
        png_byte pictureChannels;
        int bitDepth;
        int colorType;
    };
    void UpdateLoop();
    void* LoadPng(const std::string &imgFileName);
    int LoadPngInternalWithFile(FILE *fp, png_structpp pngPtr, png_infopp pngInfoPtr,
        struct PictureAttr &attr);
    void CopyPictureBuffer(struct PictureAttr &attr, char *pictureBufferTmp,
        BRGA888Pixel *pictureBuffer) const;
    Frame* parent_ {};
    bool needStop_ = false;
    int intervalMs_ = 50;
    std::vector<void*> imgList_ {};
    void *staticImgList_[IMG_LIST_MAX_SIZE] {};
    bool startFlag_ = false;
    int staticShowId_ = 0;
    bool showStatic_ = false;
    int staticImgSize_ = 0;
};
} // namespace updater
#endif // UPDATER_UI_ANIMATION_LABLE_H
