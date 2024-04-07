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

#include "gfx_utils/graphic_math.h"

#include <climits>
#include <gtest/gtest.h>

using namespace std;
using namespace testing::ext;
namespace OHOS {
class MathTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
};

void MathTest::SetUpTestCase()
{
    return;
}

void MathTest::TearDownTestCase()
{
    return;
}

/**
 * @tc.number   SUB_GRAPHIC_MATH_SIN_0100
 * @tc.name     test math sin api
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(MathTest, Graphic_Math_Test_Sin_0100, Function | MediumTest | Level0)
{
    EXPECT_EQ(Sin(0), 0);
    EXPECT_EQ(Sin(90), 1);
    EXPECT_EQ(Sin(180), 0);
}

/**
 * @tc.number   SUB_GRAPHIC_MATH_FASTATAN2_0200
 * @tc.name     test math fast-atan2 api
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(MathTest, Graphic_Math_Test_FastAtan2_0200, Function | MediumTest | Level0)
{
    EXPECT_EQ(FastAtan2(0, 1), 0);
    EXPECT_EQ(FastAtan2(1, 0), 90);
    EXPECT_EQ(FastAtan2(0, -1), 180);
    EXPECT_EQ(FastAtan2(-1, 0), 270);
}


/**
 * @tc.number   SUB_GRAPHIC_MATH_FLOATTOINT64_0300
 * @tc.name     test math float-to-int64 api
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(MathTest, Graphic_Math_Test_FloatToInt64_0300, Function | MediumTest | Level0)
{
    EXPECT_EQ(FloatToInt64(1), 256);
}

/**
 * @tc.number   SUB_GRAPHIC_MATH_SQRT_0400
 * @tc.name     test math sqrt api
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(MathTest, Graphic_Math_Test_Sqrt_0400, Function | MediumTest | Level0)
{
    const float testInteger = 2.0;
    const float testIntegerSquared = testInteger * testInteger;
    const float testFloat = 2.121320; // 2.121320: 4.5 squaring results
    const float testFloatSquared = 4.5;
    const float accuracy = 0.000001;

    EXPECT_EQ(Sqrt(0), 0);
    float ret = Sqrt(testIntegerSquared);
    if (ret > testInteger - accuracy && ret < testInteger + accuracy) {
        EXPECT_EQ(0, 0);
    } else {
        EXPECT_NE(0, 0);
    }
    
    ret = Sqrt(testFloatSquared);
    if (ret > testFloat - accuracy && ret < testFloat + accuracy) {
        EXPECT_EQ(0, 0);
    } else {
        EXPECT_NE(0, 0);
    }
}

/**
 * @tc.number   SUB_GRAPHIC_MATH_VECTOR2_DOT_0500
 * @tc.name     test math vector2-dot api
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(MathTest, Graphic_Vector2_Test_Dot_0500, Function | MediumTest | Level0)
{
    Vector2<uint16_t>* vector1 = new Vector2<uint16_t>(5, 4);
    Vector2<uint16_t>* vector2 = new Vector2<uint16_t>(3, 2);

    EXPECT_EQ(vector1->Dot(*vector2), 23);

    delete vector1;
    delete vector2;
}

/**
 * @tc.number   SUB_GRAPHIC_MATH_VECTOR2_CROSS_0600
 * @tc.name     test math vector2-cross api
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(MathTest, Graphic_Vector2_Test_Cross_0600, Function | MediumTest | Level0)
{
    Vector2<uint16_t>* vector1 = new Vector2<uint16_t>(5, 4);
    Vector2<uint16_t>* vector2 = new Vector2<uint16_t>(3, 5);

    EXPECT_EQ(vector1->Cross(*vector2), 13);

    delete vector1;
    delete vector2;
}

/**
 * @tc.number   SUB_GRAPHIC_MATH_VECTOR2_OPERATOR_0700
 * @tc.name     test math vector2-operator-minus api
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(MathTest, Graphic_Vector2_Test_operator_Minus_0700, Function | MediumTest | Level0)
{
    Vector2<uint16_t>* vector1 = new Vector2<uint16_t>(5, 4);
    Vector2<uint16_t> vector2 = vector1->operator-();

    EXPECT_EQ(vector2.x_, 65531);
    EXPECT_EQ(vector2.y_, 65532);

    delete vector1;

    Vector2<uint16_t>* vector3 = new Vector2<uint16_t>(5, 7);
    Vector2<uint16_t>* vector4 = new Vector2<uint16_t>(3, 5);
    Vector2<uint16_t> vector5 = vector3->operator-(*vector4);

    EXPECT_EQ(vector5.x_, 2);
    EXPECT_EQ(vector5.y_, 2);

    delete vector3;
    delete vector4;
}

/**
 * @tc.number   SUB_GRAPHIC_MATH_VECTOR2_OPERATOR_0800
 * @tc.name     test math vector2-operator-plus api
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(MathTest, Graphic_Vector2_Test_operator_Plus_0800, Function | MediumTest | Level0)
{
    Vector2<uint16_t>* vector1 = new Vector2<uint16_t>(5, 7);
    Vector2<uint16_t>* vector2 = new Vector2<uint16_t>(3, 5);
    Vector2<uint16_t> vector3 = vector1->operator+(*vector2);

    EXPECT_EQ(vector3.x_, 8);
    EXPECT_EQ(vector3.y_, 12);

    delete vector1;
    delete vector2;
}

/**
 * @tc.number   SUB_GRAPHIC_MATH_VECTOR2_OPERATOR_0900
 * @tc.name     test math vector2-operator-star api
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(MathTest, Graphic_Vector2_Test_operator_Star_0900, Function | MediumTest | Level0)
{
    Vector2<uint16_t>* vector1 = new Vector2<uint16_t>(5, 7);
    Vector2<uint16_t> vector2 = vector1->operator*(2);

    EXPECT_EQ(vector2.x_, 10);
    EXPECT_EQ(vector2.y_, 14);

    delete vector1;
}

/**
 * @tc.number   SUB_GRAPHIC_MATH_VECTOR2_OPERATOR_1000
 * @tc.name     test math vector2-operator-duo-equal api
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(MathTest, Graphic_Vector2_Test_operator_DuoEqual_1000, Function | MediumTest | Level0)
{
    Vector2<uint16_t>* vector1 = new Vector2<uint16_t>(5, 7);
    Vector2<uint16_t>* vector2 = new Vector2<uint16_t>(5, 7);
    Vector2<uint16_t>* vector3 = new Vector2<uint16_t>(5, 8);

    EXPECT_EQ(vector1->operator==(*vector2), true);
    EXPECT_EQ(vector1->operator==(*vector3), false);

    delete vector1;
    delete vector2;
    delete vector3;
}

/**
 * @tc.number   SUB_GRAPHIC_MATH_VECTOR2_OPERATOR_1100
 * @tc.name     test math vector2-operator-equal api
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(MathTest, Graphic_Vector2_Test_operator_Equal_1100, Function | MediumTest | Level0)
{
    Vector2<uint16_t>* vector1 = new Vector2<uint16_t>(5, 7);
    Vector2<uint16_t>* vector2 = new Vector2<uint16_t>(1, 2);
    vector1->operator=(*vector2);
    EXPECT_EQ(vector1->x_, 1);
    EXPECT_EQ(vector1->y_, 2);

    delete vector1;
    delete vector2;
}

/**
 * @tc.number   SUB_GRAPHIC_MATH_VECTOR2_OPERATOR_1200
 * @tc.name     test math vector2-operator-plus-equal api
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(MathTest, Graphic_Vector2_Test_operator_PlusEqual_1200, Function | MediumTest | Level0)
{
    Vector2<uint16_t>* vector1 = new Vector2<uint16_t>(5, 7);
    Vector2<uint16_t>* vector2 = new Vector2<uint16_t>(1, 2);
    vector1->operator+=(*vector2);
    EXPECT_EQ(vector1->x_, 6);
    EXPECT_EQ(vector1->y_, 9);

    delete vector1;
    delete vector2;
}

/**
 * @tc.number   SUB_GRAPHIC_MATH_VECTOR2_OPERATOR_1300
 * @tc.name     test math vector2-operator-minus-equal api
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(MathTest, Graphic_Vector2_Test_operator_MinusEqual_1300, Function | MediumTest | Level0)
{
    Vector2<uint16_t>* vector1 = new Vector2<uint16_t>(5, 7);
    Vector2<uint16_t>* vector2 = new Vector2<uint16_t>(1, 2);
    vector1->operator-=(*vector2);
    EXPECT_EQ(vector1->x_, 4);
    EXPECT_EQ(vector1->y_, 5);

    delete vector1;
    delete vector2;
}

/**
 * @tc.number   SUB_GRAPHIC_MATH_VECTOR3_OPERATOR_1400
 * @tc.name     test math vector3-operator-brackets api
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(MathTest, Graphic_Vector3_Test_operator_Brackets_1400, Function | MediumTest | Level0)
{
    Vector3<uint16_t>* vector1 = new Vector3<uint16_t>(3, 5, 7);
    EXPECT_EQ(vector1->operator[](0), 3);
    EXPECT_EQ(vector1->operator[](1), 5);
    EXPECT_EQ(vector1->operator[](2), 7);

    delete vector1;
}

/**
 * @tc.number   SUB_GRAPHIC_MATH_VECTOR3_OPERATOR_1500
 * @tc.name     test math vector3-operator-duo-equal api
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(MathTest, Graphic_Vector3_Test_operator_DuoEqual_1500, Function | MediumTest | Level0)
{
    Vector3<uint16_t>* vector1 = new Vector3<uint16_t>(3, 5, 7);
    Vector3<uint16_t>* vector2 = new Vector3<uint16_t>(3, 5, 7);
    Vector3<uint16_t>* vector3 = new Vector3<uint16_t>(3, 5, 9);

    EXPECT_EQ(vector1->operator==(*vector2), true);
    EXPECT_EQ(vector1->operator==(*vector3), false);

    delete vector1;
    delete vector2;
    delete vector3;
}

/**
 * @tc.number   SUB_GRAPHIC_MATH_MATRIX3_GETDATA_1600
 * @tc.name     test math matrix3-getdata api
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(MathTest, Graphic_Matrix3_Test_GetData_1600, Function | MediumTest | Level0)
{
    Matrix3<uint16_t>* matrix = new Matrix3<uint16_t>(1, 1, 1, 1, 1, 1, 1, 1, 1);

    EXPECT_EQ(matrix->GetData()[0], 1);
    EXPECT_EQ(matrix->GetData()[5], 1);

    delete matrix;
}

/**
 * @tc.number   SUB_GRAPHIC_MATH_MATRIX3_OPERATOR_1700
 * @tc.name     test math matrix3-operator-star api
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(MathTest, Graphic_Matrix3_Test_operator_Star_1700, Function | MediumTest | Level0)
{
    Matrix3<uint16_t> matrix1(1, 1, 1, 1, 1, 1, 1, 1, 1);
    Matrix3<uint16_t> matrix2(1, 2, 3, 4, 5, 6, 7, 8, 9);
    Matrix3<uint16_t> matrix3 = matrix1 * matrix2;

    EXPECT_EQ(matrix3.GetData()[0], 6);
    EXPECT_EQ(matrix3.GetData()[1], 6);
    EXPECT_EQ(matrix3.GetData()[2], 6);
    EXPECT_EQ(matrix3.GetData()[3], 15);
    EXPECT_EQ(matrix3.GetData()[4], 15);
    EXPECT_EQ(matrix3.GetData()[5], 15);
    EXPECT_EQ(matrix3.GetData()[6], 24);
    EXPECT_EQ(matrix3.GetData()[7], 24);
    EXPECT_EQ(matrix3.GetData()[8], 24);

    Vector3<uint16_t> vector1(1, 2, 3);
    Vector3<uint16_t> vector2 = matrix1 * vector1;

    EXPECT_EQ(vector2.x_, 6);
    EXPECT_EQ(vector2.y_, 6);
    EXPECT_EQ(vector2.z_, 6);
}

/**
 * @tc.number   SUB_GRAPHIC_MATH_MATRIX3_OPERATOR_1800
 * @tc.name     test math matrix3-operator-brackets api
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(MathTest, Graphic_Matrix3_Test_operator_Brackets_1800, Function | MediumTest | Level0)
{
    Matrix3<uint16_t>* matrix = new Matrix3<uint16_t>(1, 2, 3, 4, 5, 6, 7, 8, 9);

    EXPECT_EQ(*matrix->operator[](0), 1);
    EXPECT_EQ(*matrix->operator[](1), 4);
    EXPECT_EQ(*matrix->operator[](2), 7);

    delete matrix;
}

/**
 * @tc.number   SUB_GRAPHIC_MATH_MATRIX3_OPERATOR_1900
 * @tc.name     test math matrix3-operator-equal api
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(MathTest, Graphic_Matrix3_Test_operator_Equal_1900, Function | MediumTest | Level0)
{
    Matrix3<uint16_t>* matrix1 = new Matrix3<uint16_t>(1, 2, 3, 4, 5, 6, 7, 8, 9);
    Matrix3<uint16_t>* matrix2 = new Matrix3<uint16_t>(1, 1, 1, 1, 1, 1, 1, 1, 1);
    matrix1->operator=(*matrix2);
    EXPECT_EQ(matrix1->GetData()[0], 1);
    EXPECT_EQ(matrix1->GetData()[1], 1);
    EXPECT_EQ(matrix1->GetData()[2], 1);
    EXPECT_EQ(matrix1->GetData()[3], 1);
    EXPECT_EQ(matrix1->GetData()[4], 1);
    EXPECT_EQ(matrix1->GetData()[5], 1);
    EXPECT_EQ(matrix1->GetData()[6], 1);
    EXPECT_EQ(matrix1->GetData()[7], 1);
    EXPECT_EQ(matrix1->GetData()[8], 1);

    delete matrix1;
    delete matrix2;
}

/**
 * @tc.number   SUB_GRAPHIC_MATH_MATRIX3_DETERMINANT_2000
 * @tc.name     test math matrix3-determinant api
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(MathTest, Graphic_Matrix3_Test_Determinant_2000, Function | MediumTest | Level0)
{
    Matrix3<uint16_t>* matrix1 = new Matrix3<uint16_t>(1, 2, 2, 2, 1, 2, 2, 2, 1);
    EXPECT_EQ(matrix1->Determinant(), 5);

    delete matrix1;
}

/**
 * @tc.number   SUB_GRAPHIC_MATH_MATRIX3_INVERSE_2100
 * @tc.name     test math matrix3-inverse api
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(MathTest, Graphic_Matrix3_Test_Inverse_2100, Function | MediumTest | Level0)
{
    Matrix3<uint16_t>* matrix1 = new Matrix3<uint16_t>(1, 1, 1, 1, 1, 1, 1, 1, 1);
    Matrix3<uint16_t> matrix2 = matrix1->Inverse();
    EXPECT_EQ(matrix2.GetData()[0], 1);
    EXPECT_EQ(matrix2.GetData()[1], 1);
    EXPECT_EQ(matrix2.GetData()[2], 1);
    EXPECT_EQ(matrix2.GetData()[3], 1);
    EXPECT_EQ(matrix2.GetData()[4], 1);
    EXPECT_EQ(matrix2.GetData()[5], 1);
    EXPECT_EQ(matrix2.GetData()[6], 1);
    EXPECT_EQ(matrix2.GetData()[7], 1);
    EXPECT_EQ(matrix2.GetData()[8], 1);
    delete matrix1;
}

/**
 * @tc.number   SUB_GRAPHIC_MATH_MATRIX3_OPERATOR_2200
 * @tc.name     test math matrix3-operator-duo-equal api
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(MathTest, Graphic_Matrix3_Test_operator_DuoEqual_2200, Function | MediumTest | Level0)
{
    Matrix3<uint16_t>* matrix1 = new Matrix3<uint16_t>(1, 1, 1, 1, 1, 1, 1, 1, 1);
    Matrix3<uint16_t>* matrix2 = new Matrix3<uint16_t>(1, 1, 1, 1, 1, 1, 1, 1, 1);
    Matrix3<uint16_t>* matrix3 = new Matrix3<uint16_t>(1, 1, 1, 1, 1, 1, 8, 1, 1);
    EXPECT_EQ(matrix1->operator==(*matrix2), true);
    EXPECT_EQ(matrix1->operator==(*matrix3), false);

    delete matrix1;
    delete matrix2;
    delete matrix3;
}

/**
 * @tc.number   SUB_GRAPHIC_MATH_MATRIX3_ROTATE_2300
 * @tc.name     test math matrix3-rotate api
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(MathTest, Graphic_Matrix3_Test_Rotate_2300, Function | MediumTest | Level0)
{
    Matrix3<uint64_t> rotate = Matrix3<uint64_t>::Rotate(0, Vector2<uint64_t>(0, 0));
    EXPECT_EQ(rotate.GetData()[0], 1);
    EXPECT_EQ(rotate.GetData()[1], 0);
    EXPECT_EQ(rotate.GetData()[3], 0);
    EXPECT_EQ(rotate.GetData()[4], 1);
}

/**
 * @tc.number   SUB_GRAPHIC_MATH_MATRIX3_SCALE_2400
 * @tc.name     test math matrix3-scale api
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(MathTest, Graphic_Matrix3_Test_Scale_2400, Function | MediumTest | Level0)
{
    Matrix3<int64_t> scale = Matrix3<int64_t>::Scale(Vector2<int64_t>(256, 256), Vector2<int64_t>(0, 0));
    EXPECT_EQ(scale.GetData()[0], 256);
    EXPECT_EQ(scale.GetData()[4], 256);
    EXPECT_EQ(scale.GetData()[6], 0);
    EXPECT_EQ(scale.GetData()[7], 0);
    EXPECT_EQ(scale.GetData()[8], 1);
}

/**
 * @tc.number   SUB_GRAPHIC_MATH_MATRIX3_TRANSLATE_2500
 * @tc.name     test math matrix3-translate api
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(MathTest, Graphic_Matrix3_Test_Translate_2500, Function | MediumTest | Level0)
{
    Matrix3<int64_t> translate = Matrix3<int64_t>::Translate(Vector2<int64_t>(0, 0));
    EXPECT_EQ(translate.GetData()[0], 1);
    EXPECT_EQ(translate.GetData()[1], 0);
    EXPECT_EQ(translate.GetData()[2], 0);
    EXPECT_EQ(translate.GetData()[3], 0);
    EXPECT_EQ(translate.GetData()[4], 1);
    EXPECT_EQ(translate.GetData()[5], 0);
    EXPECT_EQ(translate.GetData()[6], 0);
    EXPECT_EQ(translate.GetData()[7], 0);
    EXPECT_EQ(translate.GetData()[8], 1);
}
} // namespace OHOS