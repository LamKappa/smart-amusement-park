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

#ifndef GRAPHIC_LITE_DRAW_UTILS_H
#define GRAPHIC_LITE_DRAW_UTILS_H

#include "gfx_utils/color.h"
#include "common/text.h"
#include "gfx_utils/geometry2d.h"
#include "gfx_utils/graphic_buffer.h"
#include "gfx_utils/graphic_types.h"
#include "gfx_utils/style.h"
#include "gfx_utils/transform.h"

namespace OHOS {
#define SWAP_INT16(x, y)    \
    do {                    \
        int16_t temp = (x); \
        (x) = (y);          \
        (y) = temp;         \
    } while (0)

#define SWAP_POINTS(x1, x2, y1, y2) \
    SWAP_INT16(x1, x2);             \
    SWAP_INT16(y1, y2);

// FixedPointed Related definition.
#define FIXED_NUM_1 32768
#define FO_TRANS_FLOAT_TO_FIXED(f) (static_cast<int32_t>((f) * FIXED_NUM_1))
#define FO_TRANS_INTEGER_TO_FIXED(f) ((static_cast<int32_t>(f)) << 15)
#define FO_DIV(n1, n2) ((static_cast<int64_t>(n1) << 15) / (n2))
#define FO_TO_INTEGER(n) ((n) >= 0 ? ((n) >> 15) : (((n) >> 15) + 1))
#define FO_DECIMAL(n) ((n) >= 0 ? ((n) & 32767) : ((n) | (-32768)))
#define FO_MUL(n1, n2) ((static_cast<int64_t>(n1) * (n2)) >> 15)

struct EdgeSides {
    int16_t left;
    int16_t right;
    int16_t top;
    int16_t bottom;
};

struct LabelLineInfo {
    Point& pos;
    Point& offset;
    const Rect& mask;
    int16_t lineHeight;
    uint16_t lineLength;
    uint8_t shapingId;
    uint8_t opaScale;
    const Style& style;

    const char* text;
    uint16_t length;
    uint16_t start;
    uint8_t fontId;
    uint8_t fontSize;
    uint8_t txtFlag;
    UITextLanguageDirect direct;
    uint32_t* codePoints;
};

struct LabelLetterInfo {
    const Point& pos;
    Rect mask;
    const ColorType& color;
    OpacityType opa;
    int8_t offsetX;
    int8_t offsetY;

    const uint32_t& letter;
    UITextLanguageDirect direct;
    uint8_t fontId;
    uint8_t shapingId;
    uint8_t fontSize;
};

struct TransformInitState {
    // parameters below are Q15 fixed-point number
    int32_t verticalU;
    int32_t verticalV;
    int32_t duHorizon;
    int32_t dvHorizon;
    int32_t duVertical;
    int32_t dvVertical;
    // parameters above are Q15 fixed-point number
};

struct TriangleEdge {
    TriangleEdge() {}
    TriangleEdge(int16_t x1, int16_t y1, int16_t duInt, int16_t dvInt);
    ~TriangleEdge();
    // parameters below are Q15 fixed-point number
    int32_t curX = 0;
    int32_t curY = 0;
    int32_t du = 0;
    int32_t dv = 0;
    // parameters above are Q15 fixed-point number
};

struct TriangleTransformDataInfo {
    const TransformDataInfo& info;
    Point p1;
    Point p2;
    Point p3;
    bool isRightPart;
    bool ignoreJunctionPoint;
};

struct TriangleScanInfo {
    int16_t yMin;
    int16_t yMax;
    TriangleEdge& edge1;
    TriangleEdge& edge2;
    uint8_t* screenBuffer;
    uint8_t bufferPxSize;
    const ColorType& color;
    const OpacityType opaScale;
    TransformInitState& init;
    uint16_t screenBufferWidth;
    uint8_t pixelSize;
    const int32_t srcLineWidth;
    const TransformDataInfo& info;
    const Rect& mask;
    bool isRightPart;
    bool ignoreJunctionPoint;
};

struct TrianglePartInfo {
    const Rect& mask;
    const TransformMap& transMap;
    const Point& position;
    TriangleEdge& edge1;
    TriangleEdge& edge2;
    int16_t yMin;
    int16_t yMax;
    const TransformDataInfo& info;
    const ColorType& color;
    const OpacityType opaScale;
    bool isRightPart;
    bool ignoreJunctionPoint;
};

enum {
    IMG_SRC_VARIABLE,
    IMG_SRC_FILE,
    IMG_SRC_UNKNOWN,
};

class DrawUtils : public HeapBase {
public:
    static DrawUtils* GetInstance();

    void DrawColorArea(BufferInfo& gfxDstBuffer, const Rect& area, const Rect& mask,
                       const ColorType& color, OpacityType opa) const;

    void DrawColorAreaBySides(BufferInfo& gfxDstBuffer, const Rect& mask, const ColorType& color,
                              OpacityType opa, const EdgeSides& sides) const;

    void DrawPixel(BufferInfo& gfxDstBuffer, int16_t x, int16_t y, const Rect& mask,
                   const ColorType& color, OpacityType opa) const;

    void DrawLetter(BufferInfo& gfxDstBuffer, const LabelLetterInfo& letterInfo) const;

    void DrawImage(BufferInfo& gfxDstBuffer, const Rect& area, const Rect& mask,
                   const uint8_t* image, OpacityType opa, uint8_t pxBitSize,
                   ColorMode colorMode, LutColorMode lutColorMode = LUT_UNKNOW) const;

    static void
        GetXAxisErrForJunctionLine(bool ignoreJunctionPoint, bool isRightPart, int16_t& xMinErr, int16_t& xMaxErr);

    static void GetTransformInitState(const TransformMap& transMap,
                                      const Point& position,
                                      const Rect& trans,
                                      TransformInitState& init);

    static void DrawTriangleTransform(BufferInfo& gfxDstBuffer,
                                      const Rect& mask,
                                      const Point& position,
                                      const ColorType& color,
                                      OpacityType opaScale,
                                      const TransformMap& transMap,
                                      const TriangleTransformDataInfo& dataInfo);

    void DrawTransform(BufferInfo& gfxDstBuffer,
                       const Rect& mask,
                       const Point& position,
                       const ColorType& color,
                       OpacityType opaScale,
                       const TransformMap& transMap,
                       const TransformDataInfo& dataInfo) const;

    void DrawTranspantArea(BufferInfo& gfxDstBuffer, const Rect& rect, const Rect& mask);

    void DrawWithBuffer(BufferInfo& gfxDstBuffer, const Rect& rect, const Rect& mask, const ColorType* colorBuf);

    static uint8_t GetPxSizeByColorMode(uint8_t colorMode);

    static uint8_t GetByteSizeByColorMode(uint8_t colorMode);

    static LutColorMode GetLutColorModeBySize(uint8_t size);

    static OpacityType GetMixOpacity(OpacityType opa1, OpacityType opa2)
    {
        // 8: Shift right 8 bits
        OpacityType opaMix = (opa1 == OPA_OPAQUE) ? opa2 : ((static_cast<uint16_t>(opa1) * opa2) >> 8);
        return opaMix;
    }

    void DrawAdjPixelInLine(BufferInfo& gfxDstBuffer,
                            int16_t x1,
                            int16_t y1,
                            int16_t x2,
                            int16_t y2,
                            const Rect& mask,
                            const ColorType& color,
                            OpacityType opa,
                            uint16_t w) const;

    void DrawPixelInLine(BufferInfo& gfxDstBuffer, int16_t x, int16_t y, const Rect& mask,
                         const ColorType& color, OpacityType opa, uint16_t w) const;

    void DrawVerPixelInLine(BufferInfo& gfxDstBuffer,
                            int16_t x,
                            int16_t y,
                            int8_t dir,
                            const Rect& mask,
                            const ColorType& color,
                            OpacityType opa,
                            uint16_t weight) const;

    void DrawHorPixelInLine(BufferInfo& gfxDstBuffer,
                            int16_t x,
                            int16_t y,
                            int8_t dir,
                            const Rect& mask,
                            const ColorType& color,
                            OpacityType opa,
                            uint16_t weight) const;

    void BlendWithSoftWare(const uint8_t* src1,
                           const Rect& srcRect,
                           uint32_t srcStride,
                           uint32_t srcLineNumber,
                           ColorMode srcMode,
                           uint32_t color,
                           OpacityType opa,
                           uint8_t* dst,
                           uint32_t destStride,
                           ColorMode destMode,
                           uint32_t x,
                           uint32_t y) const;

    void FillAreaWithSoftWare(BufferInfo& gfxDstBuffer,
                              const Rect& fillArea,
                              const ColorType& color,
                              const OpacityType& opa) const;
private:
    using DrawTriangleTransformFuc = void (*)(const TriangleScanInfo& triangle, const ColorMode bufferMode);

    static void DrawTriangleTrueColorNearest(const TriangleScanInfo& triangle, const ColorMode bufferMode);

    static void DrawTriangleAlphaBilinear(const TriangleScanInfo& triangle, const ColorMode bufferMode);

    static void DrawTriangleTrueColorBilinear565(const TriangleScanInfo& triangle, const ColorMode bufferMode);

    static void DrawTriangleTrueColorBilinear888(const TriangleScanInfo& triangle, const ColorMode bufferMode);

    static void DrawTriangleTrueColorBilinear8888(const TriangleScanInfo& triangle, const ColorMode bufferMode);

    inline static void StepToNextLine(TriangleEdge& edg1, TriangleEdge& edg2);

    static void DrawTriangleTransformPart(BufferInfo& gfxDstBuffer, const TrianglePartInfo& part);

    static OpacityType GetPxAlphaForAlphaImg(const TransformDataInfo& dataInfo, const Point& point);

    void FillArea(BufferInfo& gfxDstBuffer, const Rect& rect, const Rect& mask,
                  bool isTransparent, const ColorType* colorBuf);
};
} // namespace OHOS
#endif // GRAPHIC_LITE_DRAW_UTILS_H
