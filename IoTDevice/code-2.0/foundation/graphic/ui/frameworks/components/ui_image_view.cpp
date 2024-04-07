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

#include "components/ui_image_view.h"
#include "common/image.h"
#include "common/typed_text.h"
#include "draw/draw_image.h"
#include "draw/draw_label.h"
#include "engines/gfx/gfx_engine_manager.h"
#include "gfx_utils/file.h"
#include "gfx_utils/image_info.h"
#include "gfx_utils/mem_api.h"
#include "imgdecode/cache_manager.h"
#ifndef VERSION_LITE
#include "gif_lib.h"
#endif

namespace OHOS {
#ifndef VERSION_LITE
class GifImageAnimator : public Animator, public AnimatorCallback {
public:
    GifImageAnimator(UIView* view, const char* src)
        : Animator(this, view, 0, true),
          gifFileType_(nullptr),
          imageIndex_(0),
          delayTime_(0),
          lastRunTime_(0),
          deltaTime_(0),
          gifDataSize_(0),
          src_(src)
    {
    }

    virtual ~GifImageAnimator()
    {
        CloseGifFile();
    }

    void Callback(UIView* view) override;

    void SetGifFileType(GifFileType* gifFileType)
    {
        gifFileType_ = gifFileType;
    }

    uint32_t SetGifFrame(GifFileType* gifFileType, int32_t imageIndex, UIImageView* imageView) const;
    void DealGifImageData(const GifFileType* gifFileType,
                          const GifImageDesc* gifImageDesc,
                          const SavedImage* savedImage,
                          GraphicsControlBlock gcb,
                          const ColorMapObject* colorMap) const;
    const void OpenGifFile(const char* src);
    void CloseGifFile();

private:
    GifFileType* GetGifFileType()
    {
        if (gifFileType_ == nullptr) {
            OpenGifFile(src_);
        }
        return gifFileType_;
    }

    GifFileType* gifFileType_;
    int32_t imageIndex_;
    uint32_t delayTime_;
    uint32_t lastRunTime_;
    uint32_t deltaTime_;
    uint32_t gifDataSize_;
    uint8_t* gifImageData_ = nullptr;
    const char* src_;
};

const void GifImageAnimator::OpenGifFile(const char* src)
{
    int32_t error = D_GIF_SUCCEEDED;
    GifFileType* gifFileType = DGifOpenFileName(src, &error);
    if (error != D_GIF_SUCCEEDED) {
        return;
    }
    DGifSlurp(gifFileType);
    /* 3 : when change single pixel to byte, the buffer should divided by 8, equal to shift right 3 bits. */
    uint8_t pixelByteSize = DrawUtils::GetPxSizeByColorMode(ARGB8888) >> 3;
    gifDataSize_ = gifFileType->SWidth * gifFileType->SHeight * pixelByteSize;
    gifImageData_ = static_cast<uint8_t*>(UIMalloc(gifDataSize_));
    if (gifImageData_ == nullptr) {
        CloseGifFile();
        return;
    }
    SetGifFileType(gifFileType);
}

void GifImageAnimator::CloseGifFile()
{
    GifFileType* gifFileType = GetGifFileType();
    if (gifFileType != nullptr) {
        DGifCloseFile(gifFileType, nullptr);
    }
    if (gifImageData_ != nullptr) {
        UIFree(reinterpret_cast<void*>(const_cast<uint8_t*>(gifImageData_)));
        gifImageData_ = nullptr;
    }
}

void GifImageAnimator::Callback(UIView* view)
{
    if (view == nullptr) {
        return;
    }
    UIImageView* imageView = static_cast<UIImageView*>(view);
    uint32_t curTime = GetRunTime();
    if (curTime != 0) {
        if (curTime + deltaTime_ - lastRunTime_ >= delayTime_) {
            deltaTime_ = curTime + deltaTime_ - lastRunTime_ - delayTime_;
            lastRunTime_ = curTime;
        } else {
            return;
        }
    }
    GifFileType* gifFileType = GetGifFileType();
    if (gifFileType != nullptr) {
        delayTime_ = SetGifFrame(gifFileType, imageIndex_, imageView);
        imageIndex_ = (imageIndex_ < gifFileType->ImageCount - 1) ? (imageIndex_ + 1) : 0;
    }
}

uint32_t GifImageAnimator::SetGifFrame(GifFileType* gifFileType, int32_t imageIndex, UIImageView* imageView) const
{
    SavedImage* savedImage = &(gifFileType->SavedImages[imageIndex]);
    if (savedImage == nullptr) {
        return 0;
    }
    GifImageDesc* gifImageDesc = &(savedImage->ImageDesc);
    if (gifImageDesc == nullptr) {
        return 0;
    }
    GraphicsControlBlock gcb;
    int32_t ret = DGifSavedExtensionToGCB(gifFileType, imageIndex, &gcb);
    if (ret != GIF_OK) {
        return 0;
    }
    ColorMapObject* colorMap = nullptr;
    if (gifImageDesc->ColorMap != nullptr) {
        colorMap = gifImageDesc->ColorMap;
    } else {
        colorMap = gifFileType->SColorMap;
    }

    DealGifImageData(gifFileType, gifImageDesc, savedImage, gcb, colorMap);
    if (gifImageData_ == nullptr) {
        return 0;
    }
    imageView->gifFrameFlag_ = true;
    ImageInfo gifFrame;
    gifFrame.header.width = gifFileType->SWidth;
    gifFrame.header.height = gifFileType->SHeight;
    gifFrame.header.colorMode = ARGB8888;
    gifFrame.dataSize = gifDataSize_;
    gifFrame.data = gifImageData_;
    imageView->SetSrc(&gifFrame);

    if (gcb.DelayTime >= 0) {
        return static_cast<uint32_t>(gcb.DelayTime) * 10; // 10: change hundredths (1/100) of a second to millisecond
    } else {
        return 0;
    }
}

void GifImageAnimator::DealGifImageData(const GifFileType* gifFileType,
                                        const GifImageDesc* gifImageDesc,
                                        const SavedImage* savedImage,
                                        GraphicsControlBlock gcb,
                                        const ColorMapObject* colorMap) const
{
    if ((gifFileType == nullptr) || (gifImageDesc == nullptr) || (savedImage == nullptr) ||
        (savedImage->RasterBits == nullptr) || (colorMap == nullptr) || (colorMap->Colors == nullptr)) {
        return;
    }
    uint8_t colorIndex = 0;
    GifColorType* gifColorType = nullptr;
    uint32_t index = 0;
    bool transparentColor = true;
    int32_t loc = 0;
    for (int32_t x = 0; x < gifFileType->SHeight; x++) {
        for (int32_t y = 0; y < gifFileType->SWidth; y++) {
            transparentColor = true;
            if ((x >= gifImageDesc->Top) && (x < gifImageDesc->Top + gifImageDesc->Height) &&
                (y >= gifImageDesc->Left) && (y < gifImageDesc->Left + gifImageDesc->Width)) {
                loc = (x - gifImageDesc->Top) * gifImageDesc->Width + (y - gifImageDesc->Left);
                colorIndex = savedImage->RasterBits[loc];

                if ((gcb.DisposalMode != DISPOSE_DO_NOT) || (gcb.TransparentColor == NO_TRANSPARENT_COLOR) ||
                    (colorIndex != gcb.TransparentColor)) {
                    transparentColor = false;
                }
            }
            if (transparentColor) {
                index += 4; // 4: skip color index, keep last frame color
            } else {
                gifColorType = &colorMap->Colors[colorIndex];
                gifImageData_[index++] = gifColorType->Blue;
                gifImageData_[index++] = gifColorType->Green;
                gifImageData_[index++] = gifColorType->Red;
                gifImageData_[index++] = OPA_OPAQUE;
            }
        }
    }
}
#endif

UIImageView::UIImageView()
    : imageWidth_(0),
      imageHeight_(0),
      autoEnable_(true),
      needRefresh_(false),
      colorFormat_(UNKNOW),
      blurLevel_(BlurLevel::LEVEL0),
      algorithm_(TransformAlgorithm::BILINEAR),
      reserve_(0)
{
    style_ = &(StyleDefault::GetBackgroundTransparentStyle());
#ifndef VERSION_LITE
    gifImageAnimator_ = nullptr;
    gifFrameFlag_ = false;
#endif
}

UIImageView::~UIImageView()
{
#ifndef VERSION_LITE
    RemoveAndStopGifAnimator();
#endif
}

bool UIImageView::OnPreDraw(Rect& invalidatedArea) const
{
    if ((image_.GetSrcType() == IMG_SRC_UNKNOWN)) {
        return true;
    }

    if ((colorFormat_ == RGB565) || (colorFormat_ == RGB888)) {
        if (GetRect().IsContains(invalidatedArea)) {
            return true;
        }
        invalidatedArea.Intersect(invalidatedArea, GetRect());
    }

    return false;
}

void UIImageView::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    OpacityType opa = GetMixOpaScale();
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, GetRect(), invalidatedArea, *style_, opa);
    if ((imageHeight_ == 0) || (imageWidth_ == 0)) {
        return;
    }

    Rect viewRect = GetContentRect();
    Rect trunc(invalidatedArea);
    if (trunc.Intersect(trunc, viewRect)) {
        uint8_t srcType = image_.GetSrcType();
        if ((srcType == IMG_SRC_FILE) || (srcType == IMG_SRC_VARIABLE)) {
            Rect cordsTmp;
            cordsTmp.SetTop(viewRect.GetY());
            cordsTmp.SetBottom(viewRect.GetY() + imageHeight_ - 1);

            if ((transMap_ == nullptr) || transMap_->IsInvalid()) {
                while (cordsTmp.GetTop() <= viewRect.GetBottom()) {
                    cordsTmp.SetLeft(viewRect.GetX());
                    cordsTmp.SetRight(viewRect.GetX() + imageWidth_ - 1);
                    while (cordsTmp.GetLeft() <= viewRect.GetRight()) {
                        image_.DrawImage(gfxDstBuffer, cordsTmp, trunc, *style_, opa);
                        cordsTmp.SetLeft(cordsTmp.GetLeft() + imageWidth_);
                        cordsTmp.SetRight(cordsTmp.GetRight() + imageWidth_);
                    }
                    cordsTmp.SetTop(cordsTmp.GetTop() + imageHeight_);
                    cordsTmp.SetBottom(cordsTmp.GetBottom() + imageHeight_);
                }
            } else if ((transMap_ != nullptr) && !transMap_->IsInvalid()) {
                ImageInfo imgInfo;
                if (srcType == IMG_SRC_FILE) {
                    CacheEntry entry;
                    RetCode ret = CacheManager::GetInstance().Open(GetPath(), *style_, entry);
                    if (ret != RetCode::OK) {
                        return;
                    }
                    imgInfo = entry.GetImageInfo();
                } else {
                    imgInfo = *(GetImageInfo());
                }
                uint8_t pxSize = DrawUtils::GetPxSizeByColorMode(imgInfo.header.colorMode);
                TransformDataInfo imageTranDataInfo = {imgInfo.header, imgInfo.data, pxSize,
                                                       static_cast<BlurLevel>(blurLevel_),
                                                       static_cast<TransformAlgorithm>(algorithm_)};

                Rect origRect = GetOrigRect();
                transMap_->SetTransMapRect(origRect);
                OpacityType opaScale = DrawUtils::GetMixOpacity(opa, style_->imageOpa_);
                BaseGfxEngine::GetInstance()->DrawTransform(gfxDstBuffer, invalidatedArea, {0, 0}, Color::Black(),
                                                            opaScale, *transMap_, imageTranDataInfo);
            }
        }
    }
}

void UIImageView::SetSrc(const char* src)
{
#ifndef VERSION_LITE
    if (src == nullptr) {
        return;
    }
    const static uint8_t IMG_BYTES_TO_CHECK = 4; // 4: check 4 bytes of image file
    char buf[IMG_BYTES_TO_CHECK] = {0};
    int32_t fd = open(src, O_RDONLY);
    if (fd < 0) {
        return;
    }
    if (read(fd, buf, IMG_BYTES_TO_CHECK) != IMG_BYTES_TO_CHECK) {
        close(fd);
        return;
    }
    close(fd);
    bool updated = false;
    RemoveAndStopGifAnimator();
    // 0x47 0x49 0x46: GIF file's header
    if ((static_cast<uint8_t>(buf[0]) == 0x47) && (static_cast<uint8_t>(buf[1]) == 0x49) &&
        (static_cast<uint8_t>(buf[2]) == 0x46)) { // 2: array index of GIF file's header
        if (gifImageAnimator_ == nullptr) {
            gifImageAnimator_ = new GifImageAnimator(this, src);
            if (gifImageAnimator_ == nullptr) {
                GRAPHIC_LOGE("new GifImageAnimator fail");
                return;
            }
        }
        AddAndStartGifAnimator();
        updated = true;
    } else {
        updated = image_.SetSrc(src);
    }
#else
    bool updated = image_.SetSrc(src);
#endif
    if (!updated) {
        return;
    }
    needRefresh_ = true;
    if (autoEnable_) {
        UIImageView::ReMeasure();
    }
    Invalidate();
}

void UIImageView::ReMeasure()
{
    if (!needRefresh_) {
        return;
    }
    needRefresh_ = false;

    ImageHeader header = {0};
    image_.GetHeader(header);

    imageWidth_ = header.width;
    imageHeight_ = header.height;
    colorFormat_ = header.colorMode;

    if (autoEnable_) {
        Resize(imageWidth_, imageHeight_);
        Invalidate();
    }
}

void UIImageView::SetSrc(const ImageInfo* src)
{
#ifndef VERSION_LITE
    if (!gifFrameFlag_ && (gifImageAnimator_ != nullptr)) {
        RemoveAndStopGifAnimator();
    }
    gifFrameFlag_ = false;
#endif
    bool updated = image_.SetSrc(src);
    if (!updated) {
        return;
    }
    needRefresh_ = true;
    if (autoEnable_) {
        UIImageView::ReMeasure();
    }
    Invalidate();
}

#ifndef VERSION_LITE
void UIImageView::AddAndStartGifAnimator()
{
    if (gifImageAnimator_ != nullptr) {
        gifImageAnimator_->Start();
    }
}

void UIImageView::RemoveAndStopGifAnimator()
{
    if (gifImageAnimator_ != nullptr) {
        gifImageAnimator_->Stop();
        delete gifImageAnimator_;
        gifImageAnimator_ = nullptr;
    }
}
#endif
} // namespace OHOS