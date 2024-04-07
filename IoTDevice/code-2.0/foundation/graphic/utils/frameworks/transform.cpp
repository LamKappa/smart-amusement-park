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

#include "gfx_utils/transform.h"

#include "gfx_utils/graphic_math.h"
namespace OHOS {
constexpr uint8_t VERTEX_NUM_MIN = 3;

TransformMap::TransformMap()
    : angle_(0),
      isInvalid_(false),
      scaleCoeff_({1.0f, 1.0f}),
      scalePivot_({0, 0}),
      rotatePivot_({0, 0}),
      rect_{0, 0, 0, 0},
      polygon_(Rect(0, 0, 0, 0))
{
    scale_ = Matrix3<float>::Scale(Vector2<float>(1.0f, 1.0f), Vector2<float>(0, 0));
    rotate_ = Matrix3<float>::Rotate(0, Vector2<float>(0, 0));
    translate_ = Matrix3<float>::Translate(Vector2<float>(0, 0));
    trans_[ROTATE] = &rotate_;
    trans_[SCALE] = &scale_;
    trans_[TRANSLATE] = &translate_;
    opOrder_[ROTATE] = ROTATE;
    opOrder_[SCALE] = SCALE;
    opOrder_[TRANSLATE] = TRANSLATE;

    UpdateMap();
}

TransformMap::TransformMap(const Rect& rect)
    : angle_(0), isInvalid_(false), scaleCoeff_({1.0f, 1.0f}), scalePivot_({0, 0}), rotatePivot_({0, 0})
{
    scale_ = Matrix3<float>::Scale(Vector2<float>(1.0f, 1.0f), Vector2<float>(0, 0));
    rotate_ = Matrix3<float>::Rotate(0, Vector2<float>(0, 0));
    translate_ = Matrix3<float>::Translate(Vector2<float>(0, 0));
    trans_[ROTATE] = &rotate_;
    trans_[SCALE] = &scale_;
    trans_[TRANSLATE] = &translate_;
    opOrder_[ROTATE] = ROTATE;
    opOrder_[SCALE] = SCALE;
    opOrder_[TRANSLATE] = TRANSLATE;
    rect_ = rect;
    polygon_ = rect;
    UpdateMap();
}

bool TransformMap::GetClockWise() const
{
    int16_t count = 0;
    int32_t c;

    uint8_t vertexNum = polygon_.GetVertexNum();
    if (vertexNum < VERTEX_NUM_MIN) {
        return false;
    }

    uint8_t i = 0;
    uint8_t j;
    uint8_t k;
    for (; i < vertexNum; i++) {
        j = (i + 1) % vertexNum; // 1: the next vertex
        k = (i + 2) % vertexNum; // 2: the after next vertex
        c = (static_cast<int32_t>(polygon_[j].x_ - polygon_[i].x_) * (polygon_[k].y_ - polygon_[j].y_)) -
            (static_cast<int32_t>(polygon_[j].y_ - polygon_[i].y_) * (polygon_[k].x_ - polygon_[j].x_));
        if (c < 0) {
            count--;
        } else if (c > 0) {
            count++;
        }
    }
    if (count > 0) {
        return true;
    }
    return false;
}

void TransformMap::SetTransMapRect(const Rect& rect)
{
    rect_ = rect;
    polygon_ = rect;
    UpdateMap();
}

void TransformMap::Scale(const Vector2<float> scale, const Vector2<float>& pivot)
{
    scaleCoeff_ = scale;
    scalePivot_ = pivot;
    AddOp(SCALE);
    UpdateMap();
}

bool TransformMap::IsInvalid() const
{
    if (isInvalid_) {
        return true;
    }
    if ((angle_ % CIRCLE_IN_DEGREE == 0) && FloatEqual(scaleCoeff_.x_, 1.0f) && FloatEqual(scaleCoeff_.y_, 1.0f)) {
        return true;
    }
    for (uint8_t i = 0; i < polygon_.GetVertexNum(); i++) {
        if (polygon_[i].x_ != 0 || polygon_[i].y_ != 0) {
            return false;
        }
    }
    return true;
}

void TransformMap::Rotate(int16_t angle, const Vector2<float>& pivot)
{
    angle_ = angle;
    rotatePivot_ = pivot;
    AddOp(ROTATE);
    UpdateMap();
}

void TransformMap::Translate(const Vector2<int16_t>& trans)
{
    translate_ = Matrix3<float>::Translate(Vector2<float>(trans.x_, trans.y_));
    AddOp(TRANSLATE);
    UpdateMap();
}

bool TransformMap::operator==(const TransformMap& other) const
{
    if (rotate_ == other.rotate_ && translate_ == other.translate_ && scale_ == other.scale_ && rect_ == other.rect_) {
        return true;
    }
    return false;
}

void TransformMap::UpdateMap()
{
    trans_[ROTATE] = &rotate_;
    trans_[SCALE] = &scale_;
    trans_[TRANSLATE] = &translate_;
    polygon_ = rect_;
    rotate_ =
        Matrix3<float>::Rotate(angle_, Vector2<float>(rotatePivot_.x_ + rect_.GetX(), rotatePivot_.y_ + rect_.GetY()));
    scale_ = Matrix3<float>::Scale(scaleCoeff_,
                                   Vector2<float>(scalePivot_.x_ + rect_.GetX(), scalePivot_.y_ + rect_.GetY()));

    matrix_ = (*trans_[opOrder_[TRANSLATE]]) * (*trans_[opOrder_[SCALE]]) * (*trans_[opOrder_[ROTATE]]);
    uint8_t vertexNum = polygon_.GetVertexNum();
    Vector3<float> imgPoint3;
    for (uint8_t i = 0; i < vertexNum; i++) {
        Vector3<float> point(polygon_[i].x_, polygon_[i].y_, 1);
        imgPoint3 = matrix_ * point;
        if (imgPoint3.x_ < COORD_MIN) {
            polygon_[i].x_ = COORD_MIN;
        } else if (imgPoint3.x_ > COORD_MAX) {
            polygon_[i].x_ = COORD_MAX;
        } else {
            polygon_[i].x_ = imgPoint3.x_;
        }

        if (imgPoint3.y_ < COORD_MIN) {
            polygon_[i].y_ = COORD_MIN;
        } else if (imgPoint3.y_ > COORD_MAX) {
            polygon_[i].y_ = COORD_MAX;
        } else {
            polygon_[i].y_ = imgPoint3.y_;
        }
    }
    Matrix3<float> translate = Matrix3<float>::Translate(Vector2<float>(rect_.GetX(), rect_.GetY()));
    matrix_ = matrix_ * translate;
    invMatrix_ = matrix_.Inverse();
}

void TransformMap::AddOp(uint8_t op)
{
    if (opOrder_[TRANSLATE] == op) {
        return;
    } else if (opOrder_[SCALE] == op) {
        opOrder_[SCALE] = opOrder_[TRANSLATE];
        opOrder_[TRANSLATE] = op;
    } else {
        opOrder_[ROTATE] = opOrder_[SCALE];
        opOrder_[SCALE] = opOrder_[TRANSLATE];
        opOrder_[TRANSLATE] = op;
    }
}

void Rotate(const Vector2<int16_t>& point, int16_t angle, const Vector2<int16_t>& pivot, Vector2<int16_t>& out)
{
    float sinma = Sin(angle);
    float cosma = Sin(angle + 90); // 90: cos

    int16_t xt = point.x_ - pivot.x_;
    int16_t yt = point.y_ - pivot.y_;

    /* 0.5: round up */
    float temp = cosma * xt - sinma * yt;
    out.x_ = static_cast<int16_t>((temp > 0) ? (temp + 0.5f) : (temp - 0.5f)) + pivot.x_;
    temp = sinma * xt + cosma * yt;
    out.y_ = static_cast<int16_t>((temp > 0) ? (temp + 0.5f) : (temp - 0.5f)) + pivot.y_;
}

void Rotate(const Line& origLine, int16_t angle, const Vector2<int16_t>& pivot, Line& out)
{
    Vector2<int16_t> pt1 = origLine[0];
    Vector2<int16_t> pt2 = origLine[1];

    Rotate(pt1, angle, pivot, out[1]); // 1: the first point of line
    Rotate(pt2, angle, pivot, out[2]); // 2: the second point of line
}

void Rotate(const Rect& origRect, int16_t angle, const Vector2<int16_t>& pivot, Polygon& out)
{
    Vector2<int16_t> pt1 = {origRect.GetLeft(), origRect.GetTop()};
    Vector2<int16_t> pt2 = {origRect.GetRight(), origRect.GetTop()};
    Vector2<int16_t> pt3 = {origRect.GetRight(), origRect.GetBottom()};
    Vector2<int16_t> pt4 = {origRect.GetLeft(), origRect.GetBottom()};

    Rotate(pt1, angle, pivot, out[1]); // 1: the first point
    Rotate(pt2, angle, pivot, out[2]); // 2: the second point
    Rotate(pt3, angle, pivot, out[3]); // 3: the third point
    Rotate(pt4, angle, pivot, out[4]); // 4: the fourth point

    out.SetVertexNum(4); // 4: number of vertex
}
} // namespace OHOS
