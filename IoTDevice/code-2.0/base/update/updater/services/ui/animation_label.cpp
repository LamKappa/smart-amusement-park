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
#include "animation_label.h"
#include <cerrno>
#include <cstdio>
#include <string>
#include "frame.h"
#include "log/log.h"
#include "png.h"
#include "securec.h"
#include "view.h"

namespace updater {
AnimationLable::AnimationLable(int startX, int startY, int w, int h, Frame *mParent)
{
    startX_ = startX;
    startY_ = startY;
    this->CreateBuffer(w, h, View::PixelFormat::BGRA888);
    parent_ = mParent;
    SetFocusAble(false);
    needStop_ = false;
    parent_->ViewRegister(this);
    startFlag_ = false;
    updateThread = std::thread(&AnimationLable::UpdateLoop, this);
    updateThread.detach();
}

AnimationLable::~AnimationLable()
{
    needStop_ = true;
    FreeBuffer();
    int imgSize = imgList_.size();

    for (int i = 0; i < imgSize; i++) {
        free(imgList_[i]);
    }
    imgList_.clear();
}

void AnimationLable::Start()
{
    startFlag_ = true;
}

void AnimationLable::Stop()
{
    startFlag_ = false;
}

void AnimationLable::SetStaticImg(int picId)
{
    staticShowId_ = picId;
}

void AnimationLable::SetPlayMode(AnimationLable::PlayMode mode)
{
    if (mode == AnimationLable::PlayMode::ANIMATION_MODE) {
        showStatic_ = false;
    } else if (mode == AnimationLable::PlayMode::STATIC_MODE) {
        showStatic_ = true;
    }
}

void AnimationLable::UpdateLoop()
{
    unsigned int index = 0;
    while (!needStop_) {
        if (showStatic_) {
            usleep(SECOND_PER_MS * SECOND_PER_MS);
        } else {
            usleep(intervalMs_ * SECOND_PER_MS);
        }
        if (imgList_.size() <= 0) {
            continue;
        }
        if (!startFlag_ && IsVisiable()) {
            continue;
        }

        if (imgList_.size() <= index) {
            index = 0;
        }
        SyncBuffer();
        mutex_.lock();

        if (showStatic_) {
            if (staticShowId_ < staticImgSize_) {
                DrawSubView(0, 0, viewWidth_, viewHeight_, staticImgList_[staticShowId_]);
            }
        } else {
            DrawSubView(0, 0, viewWidth_, viewHeight_, imgList_[index]);
        }
        mutex_.unlock();
        if (parent_ != nullptr) {
            parent_->OnDraw();
        }
        index++;
    }
    LOG(DEBUG) << "anim loop end";
}

void AnimationLable::AddImg(const std::string &imgFileName)
{
    mutex_.lock();
    void *buf = LoadPng(imgFileName);
    imgList_.push_back(buf);
    mutex_.unlock();
}

int AnimationLable::AddStaticImg(const std::string &imgFileName)
{
    int id = staticImgSize_;
    mutex_.lock();
    staticImgList_[id] = LoadPng(imgFileName);
    staticImgSize_++;
    mutex_.unlock();
    return id;
}

int AnimationLable::LoadPngInternalWithFile(FILE *fp, png_structpp pngPtr, png_infopp pngInfoPtr,
    struct PictureAttr &attr)
{
    if (fp == nullptr) {
        return -1;
    }
    uint8_t header[PNG_HEADER_SIZE];
    size_t bytesRead = fread(header, 1, sizeof(header), fp);
    if (bytesRead != sizeof(header)) {
        LOG(ERROR) << "read header from file failed: " << errno;
        return -1;
    }
    if (png_sig_cmp(header, 0, sizeof(header))) {
        LOG(ERROR) << "png file header is not valid";
        return -1;
    }

    *pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (*pngPtr == nullptr) {
        LOG(ERROR) << "creat png struct failed";
        return -1;
    }

    *pngInfoPtr = png_create_info_struct(*pngPtr);
    if (*pngInfoPtr == nullptr) {
        LOG(ERROR) << "Create png info failed";
        return -1;
    }
    png_init_io(*pngPtr, fp);
    png_set_sig_bytes(*pngPtr, sizeof(header));
    png_read_info(*pngPtr, *pngInfoPtr);
    png_get_IHDR(*pngPtr, *pngInfoPtr, &attr.pictureWidth, &attr.pictureHeight, &attr.bitDepth, &attr.colorType,
        nullptr, nullptr, nullptr);
    attr.pictureChannels = png_get_channels(*pngPtr, *pngInfoPtr);
    if (attr.bitDepth <= MAX_BIT_DEPTH && attr.pictureChannels == 1 && attr.colorType == PNG_COLOR_TYPE_PALETTE) {
        // paletted images: expand to 8-bit RGB.  Note that we DON'T
        // currently expand the tRNS chunk (if any) to an alpha
        // channel, because minui doesn't support alpha channels in
        // general.
        png_set_palette_to_rgb(*pngPtr);
        attr.pictureChannels = MAX_PICTURE_CHANNELS;
    }

    if (attr.pictureChannels < MAX_PICTURE_CHANNELS) {
        LOG(ERROR) << "need rgb format pic";
        return -1;
    }
    return 0;
}

void AnimationLable::CopyPictureBuffer(struct PictureAttr &attr, char *pictureBufferTmp,
    BRGA888Pixel *pictureBuffer) const
{
    int copyHeight = (viewHeight_ < static_cast<int>(attr.pictureHeight)) ? viewHeight_ :
        static_cast<int>(attr.pictureHeight);
    int copyWidth = (viewWidth_ < static_cast<int>(attr.pictureWidth)) ? viewWidth_ :
        static_cast<int>(attr.pictureWidth);
    auto *rgb = reinterpret_cast<RGB888Pixel*>(pictureBufferTmp);
    for (int y = 0; y < copyHeight; y++) {
        for (int x = 0; x < copyWidth; x++) {
            unsigned int colorValue = rgb[x + y * attr.pictureWidth].r +
            rgb[x + y * attr.pictureWidth].g + rgb[x + y * attr.pictureWidth].b;
            if (colorValue > 0) {
                pictureBuffer[x + y * viewWidth_].r = rgb[x + y * attr.pictureWidth].r;
                pictureBuffer[x + y * viewWidth_].g = rgb[x + y * attr.pictureWidth].g;
                pictureBuffer[x + y * viewWidth_].b = rgb[x + y * attr.pictureWidth].b;
                pictureBuffer[x + y * viewWidth_].a = 0xff;
            }
        }
    }
}

void *AnimationLable::LoadPng(const std::string &imgFileName)
{
    png_structp pngPtr = nullptr;
    png_infop pngInfoPtr = nullptr;
    struct PictureAttr attr {};
    char *pictureBufferTmp = nullptr;
    BRGA888Pixel *pictureBuffer = nullptr;
    char *backgroundBuffer = static_cast<char*>(GetRawBuffer());
    int pictureBufferSize = viewHeight_ * viewWidth_ * sizeof(BRGA888Pixel);
    uint8_t *pictureRow = nullptr;

    FILE *fp = fopen(imgFileName.c_str(), "rb");
    UPDATER_FILE_CHECK(fp != nullptr, "open font file failed", return nullptr);
    if (LoadPngInternalWithFile(fp, &pngPtr, &pngInfoPtr, attr) < 0) {
        png_destroy_read_struct(&pngPtr, &pngInfoPtr, 0);
        fclose(fp);
        fp = nullptr;
        return nullptr;
    }
    unsigned int pictureRowSize = attr.pictureWidth * attr.pictureChannels;
    pictureBufferTmp = static_cast<char *>(malloc(pictureRowSize * attr.pictureHeight));
    UPDATER_ERROR_CHECK(pictureBufferTmp != nullptr, "Allocate memory failed", goto err);

    for (unsigned int y = 0; y < attr.pictureHeight; y++) {
        pictureRow = reinterpret_cast<uint8_t *>((pictureBufferTmp) + y * pictureRowSize);
        png_read_row(pngPtr, pictureRow, nullptr);
    }

    pictureBuffer = static_cast<BRGA888Pixel*>(malloc(pictureBufferSize));
    UPDATER_ERROR_CHECK(pictureBuffer != nullptr, "Allocate memory failed", goto err);
    if (memcpy_s(reinterpret_cast<char *>(pictureBuffer), pictureBufferSize,
        backgroundBuffer, pictureBufferSize) != EOK) {
        goto err;
    }
    CopyPictureBuffer(attr, pictureBufferTmp, pictureBuffer);
    free(pictureBufferTmp);
    pictureBufferTmp = nullptr;
    fclose(fp);
    fp = nullptr;
    return static_cast<void *>(pictureBuffer);
err:
    if (pictureBuffer != nullptr) {
        free(pictureBuffer);
        pictureBuffer = nullptr;
    }
    if (pictureBufferTmp != nullptr) {
        free(pictureBufferTmp);
        pictureBufferTmp = nullptr;
    }
    if (fp != nullptr) {
        fclose(fp);
        fp = nullptr;
    }
    return nullptr;
}

void AnimationLable::SetInterval(int ms)
{
    intervalMs_ = ms;
}
} // namespace updater
