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
#ifndef UPDATER_UI_TEXT_LABLE_H
#define UPDATER_UI_TEXT_LABLE_H

#include <cstdio>
#include <iostream>
#include <string>
#include "frame.h"
#include "view.h"

namespace updater {
constexpr int MAX_FONT_BUFFER_SIZE_HW  = 4096;
constexpr int MAX_TEXT_SIZE = 512;
const std::string DEFAULT_FONT_NAME = "font";

class TextLable : public View {
using ClickCallback = std::function<void(int id)>;
enum FontType {
    DEFAULT_FONT,
};

enum AlignmentMethod {
    ALIGN_CENTER,
    ALIGN_TO_LEFT,
    ALIGN_TO_TOP,
};
public:
    TextLable(int startX, int startY, int w, int h, Frame *parent);
    ~TextLable() override {};
    void SetText(const char *str);
    void SetTextColor(BRGA888Pixel color);
    void SetFont(FontType fType);
    void SetOutLineBold(bool topBold, bool bottomBold);
    void SetTextAlignmentMethod(AlignmentMethod methodH, AlignmentMethod methodV);
    void SetOnClickCallback(ClickCallback cb);
    void OnKeyEvent(int key) override;
    void OnDraw() override;
private:
    void InitFont();
    void DrawText();
    void DrawOutline();
    void DrawFocus();

    ClickCallback callBack_;
    char textBuf_[MAX_TEXT_SIZE + 1] {};
    Frame *parent_ {};

    AlignmentMethod fontAligMethodLevel_ = ALIGN_TO_LEFT;
    AlignmentMethod fontAligMethodUpright_ = ALIGN_CENTER;

    BRGA888Pixel outlineColor_ {};
    BRGA888Pixel actionBgColor_ {};
    BRGA888Pixel normalBgColor_ {};
    BRGA888Pixel textColor_ {};

    bool boldTopLine_ = false;
    bool boldBottomLine_ = false;

    FontType fontType_ { DEFAULT_FONT };
    char fontBuf_[MAX_FONT_BUFFER_SIZE_HW * MAX_FONT_BUFFER_SIZE_HW] {};
    unsigned int fontWidth_ = 0;
    unsigned int fontHeight_ = 0;
};
} // namespace updater
#endif
