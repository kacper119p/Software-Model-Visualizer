/* MIT License
 *
 * Copyright (c) 2026 Kacper Pastuszka
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "graphicsMath.h"
#include "testFramework.h"

static constexpr float floatEpsilon = 0.00001f;

TEST(vec3Add) {
  const struct Vec3 r = vec3Add(MAKE_VEC3(1, 2, 3), MAKE_VEC3(4, 5, 6));
  ASSERT_CLOSE_FLOAT(r.X, 5.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 7.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Z, 9.0f, floatEpsilon);
}

TEST(vec3Sub) {
  const struct Vec3 r = vec3Sub(MAKE_VEC3(5, 7, 9), MAKE_VEC3(1, 2, 3));
  ASSERT_CLOSE_FLOAT(r.X, 4.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 5.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Z, 6.0f, floatEpsilon);
}

TEST(vec3Scale) {
  const struct Vec3 r = vec3Scale(MAKE_VEC3(1, 2, 3), 2.0f);
  ASSERT_CLOSE_FLOAT(r.X, 2.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 4.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Z, 6.0f, floatEpsilon);
}

TEST(vec3Dot) {
  float d = vec3Dot(MAKE_VEC3(1, 0, 0), MAKE_VEC3(0, 1, 0));
  ASSERT_CLOSE_FLOAT(d, 0.0f, floatEpsilon);

  d = vec3Dot(MAKE_VEC3(1, 0, 0), MAKE_VEC3(1, 0, 0));
  ASSERT_CLOSE_FLOAT(d, 1.0f, floatEpsilon);
}

TEST(vec3Cross) {
  const struct Vec3 r = vec3Cross(MAKE_VEC3(1, 0, 0), MAKE_VEC3(0, 1, 0));
  ASSERT_CLOSE_FLOAT(r.X, 0.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 0.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Z, 1.0f, floatEpsilon);
}

TEST(vec3Normalize) {
  const struct Vec3 r = vec3Normalize(MAKE_VEC3(3, 0, 0));
  ASSERT_CLOSE_FLOAT(vec3Len(r), 1.0f, floatEpsilon);
}

TEST(vec4Add) {
  const struct Vec4 r = vec4Add(MAKE_VEC4(1, 2, 3, 4), MAKE_VEC4(5, 6, 7, 8));
  ASSERT_CLOSE_FLOAT(r.X, 6.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 8.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Z, 10.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.W, 12.0f, floatEpsilon);
}

TEST(vec4Dot) {
  const float d = vec4Dot(MAKE_VEC4(1, 2, 3, 4), MAKE_VEC4(5, 6, 7, 8));
  ASSERT_CLOSE_FLOAT(d, 70.0f, floatEpsilon);
}

TEST(mat4IdentityMul) {
  const struct Vec4 v = MAKE_VEC4(1, 2, 3, 1);
  const struct Vec4 r = mat4MulVec4(MAT4_IDENTITY, v);
  ASSERT_CLOSE_FLOAT(r.X, 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 2.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Z, 3.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.W, 1.0f, floatEpsilon);
}

TEST(mat4Mul) {
  const struct Mat4 a = makeMat4Translation(MAKE_VEC3(1, 0, 0));
  const struct Mat4 b = makeMat4Translation(MAKE_VEC3(2, 0, 0));
  const struct Mat4 r = mat4Mul(a, b);
  const struct Vec4 v = mat4MulVec4(r, MAKE_VEC4(0, 0, 0, 1));
  ASSERT_CLOSE_FLOAT(v.X, 3.0f, floatEpsilon);
}

TEST(mat4Translation) {
  const struct Mat4 t = makeMat4Translation(MAKE_VEC3(1, 2, 3));
  const struct Vec4 r = mat4MulVec4(t, MAKE_VEC4(0, 0, 0, 1));
  ASSERT_CLOSE_FLOAT(r.X, 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 2.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Z, 3.0f, floatEpsilon);
}

TEST(mat4RotationY) {
  const struct Mat4 r = makeMat4RotationY(Pi / 2.0f);
  const struct Vec4 v = mat4MulVec4(r, MAKE_VEC4(1, 0, 0, 1));
  ASSERT_CLOSE_FLOAT(v.X, 0.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(v.Z, 1.0f, floatEpsilon);
}

TEST(mat4Scale) {
  const struct Mat4 s = makeMat4Scale(MAKE_VEC3(2, 3, 4));
  const struct Vec4 v = mat4MulVec4(s, MAKE_VEC4(1, 1, 1, 1));
  ASSERT_CLOSE_FLOAT(v.X, 2.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(v.Y, 3.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(v.Z, 4.0f, floatEpsilon);
}

TEST(mat4Inverse) {
  const struct Mat4 t = makeMat4Translation(MAKE_VEC3(1, 2, 3));
  const struct Mat4 inv = mat4Inverse(t);
  const struct Mat4 r = mat4Mul(t, inv);
  ASSERT_CLOSE_FLOAT(r.m00, 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.m11, 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.m22, 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.m33, 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.m30, 0.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.m31, 0.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.m32, 0.0f, floatEpsilon);
}

TEST(mat4LookAt) {
  const struct Mat4 v = makeMat4LookAt(MAKE_VEC3(0, 0, 5), MAKE_VEC3(0, 0, 0),
                                       MAKE_VEC3(0, 1, 0));
  const struct Vec4 p = mat4MulVec4(v, MAKE_VEC4(0, 0, 0, 1));
  ASSERT_CLOSE_FLOAT(p.Z, -5.0f, floatEpsilon);
}

TEST(vec2Add) {
  const struct Vec2 r = vec2Add(MAKE_VEC2(1, 2), MAKE_VEC2(3, 4));
  ASSERT_CLOSE_FLOAT(r.X, 4.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 6.0f, floatEpsilon);
}

TEST(vec2Sub) {
  const struct Vec2 r = vec2Sub(MAKE_VEC2(5, 7), MAKE_VEC2(2, 3));
  ASSERT_CLOSE_FLOAT(r.X, 3.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 4.0f, floatEpsilon);
}

TEST(vec2Dot) {
  const float d = vec2Dot(MAKE_VEC2(1, 2), MAKE_VEC2(3, 4));
  ASSERT_CLOSE_FLOAT(d, 11.0f, floatEpsilon);
}

TEST(vec2Normalize) {
  const struct Vec2 r = vec2Normalize(MAKE_VEC2(3, 0));
  ASSERT_CLOSE_FLOAT(vec2Len(r), 1.0f, floatEpsilon);
}

TEST(vec3Mul) {
  const struct Vec3 r = vec3Mul(MAKE_VEC3(2, 3, 4), MAKE_VEC3(5, 6, 7));
  ASSERT_CLOSE_FLOAT(r.X, 10.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 18.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Z, 28.0f, floatEpsilon);
}

TEST(vec3Div) {
  const struct Vec3 r = vec3Div(MAKE_VEC3(10, 20, 30), 10.0f);
  ASSERT_CLOSE_FLOAT(r.X, 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 2.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Z, 3.0f, floatEpsilon);
}

TEST(vec4Sub) {
  const struct Vec4 r =
      vec4Sub(MAKE_VEC4(10, 20, 30, 40), MAKE_VEC4(1, 2, 3, 4));
  ASSERT_CLOSE_FLOAT(r.X, 9.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 18.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Z, 27.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.W, 36.0f, floatEpsilon);
}

TEST(vec4Scale) {
  const struct Vec4 r = vec4Scale(MAKE_VEC4(1, 2, 3, 4), 2.0f);
  ASSERT_CLOSE_FLOAT(r.X, 2.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 4.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Z, 6.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.W, 8.0f, floatEpsilon);
}

TEST(vec4Normalize) {
  const struct Vec4 r = vec4Normalize(MAKE_VEC4(3, 0, 0, 0));
  ASSERT_CLOSE_FLOAT(vec4Len(r), 1.0f, floatEpsilon);
}

TEST(mat4RotationX) {
  const struct Mat4 r = makeMat4RotationX(Pi / 2.0f);
  const struct Vec4 v = mat4MulVec4(r, MAKE_VEC4(0, 1, 0, 1));
  ASSERT_CLOSE_FLOAT(v.Y, 0.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(v.Z, 1.0f, floatEpsilon);
}

TEST(mat4RotationZ) {
  const struct Mat4 r = makeMat4RotationZ(Pi / 2.0f);
  const struct Vec4 v = mat4MulVec4(r, MAKE_VEC4(1, 0, 0, 1));
  ASSERT_CLOSE_FLOAT(v.X, 0.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(v.Y, 1.0f, floatEpsilon);
}

TEST(mat4ScaleUniform) {
  const struct Mat4 s = makeMat4ScaleUniform(2.0f);
  const struct Vec4 v = mat4MulVec4(s, MAKE_VEC4(1, 1, 1, 1));
  ASSERT_CLOSE_FLOAT(v.X, 2.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(v.Y, 2.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(v.Z, 2.0f, floatEpsilon);
}

TEST(mat4Transpose) {
  const struct Mat4 m = makeMat4Translation(MAKE_VEC3(1, 2, 3));
  const struct Mat4 t = mat4Transpose(m);
  ASSERT_CLOSE_FLOAT(t.m03, 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(t.m13, 2.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(t.m23, 3.0f, floatEpsilon);
}

TEST(perspectiveProjection) {
  const struct Mat4 p =
      makePerspectiveProjectionMatrix(Pi / 4.0f, 1.0f, 0.1f, 100.0f);
  const struct Vec4 v = mat4MulVec4(p, MAKE_VEC4(0, 0, -1, 1));
  ASSERT_CLOSE_FLOAT(v.W, 1.0f, floatEpsilon);
}

TEST(vec2Mul) {
  const struct Vec2 r = vec2Mul(MAKE_VEC2(2, 3), MAKE_VEC2(4, 5));
  ASSERT_CLOSE_FLOAT(r.X, 8.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 15.0f, floatEpsilon);
}

TEST(vec2Scale) {
  const struct Vec2 r = vec2Scale(MAKE_VEC2(2, 3), 3.0f);
  ASSERT_CLOSE_FLOAT(r.X, 6.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 9.0f, floatEpsilon);
}

TEST(vec2Div) {
  const struct Vec2 r = vec2Div(MAKE_VEC2(10, 20), 5.0f);
  ASSERT_CLOSE_FLOAT(r.X, 2.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 4.0f, floatEpsilon);
}

TEST(vec2LenSq) {
  const float lenSq = vec2LenSq(MAKE_VEC2(3, 4));
  ASSERT_CLOSE_FLOAT(lenSq, 25.0f, floatEpsilon);
}

TEST(vec2Len) {
  const float len = vec2Len(MAKE_VEC2(3, 4));
  ASSERT_CLOSE_FLOAT(len, 5.0f, floatEpsilon);
}

TEST(vec2Dist) {
  const float dist = vec2Dist(MAKE_VEC2(0, 0), MAKE_VEC2(3, 4));
  ASSERT_CLOSE_FLOAT(dist, 5.0f, floatEpsilon);
}

TEST(vec3LenSq) {
  const float lenSq = vec3LenSq(MAKE_VEC3(1, 2, 2));
  ASSERT_CLOSE_FLOAT(lenSq, 9.0f, floatEpsilon);
}

TEST(vec3Dist) {
  const float dist = vec3Dist(MAKE_VEC3(0, 0, 0), MAKE_VEC3(1, 2, 2));
  ASSERT_CLOSE_FLOAT(dist, 3.0f, floatEpsilon);
}

TEST(vec4Mul) {
  const struct Vec4 r = vec4Mul(MAKE_VEC4(2, 3, 4, 5), MAKE_VEC4(6, 7, 8, 9));
  ASSERT_CLOSE_FLOAT(r.X, 12.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 21.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Z, 32.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.W, 45.0f, floatEpsilon);
}

TEST(vec4Div) {
  const struct Vec4 r = vec4Div(MAKE_VEC4(10, 20, 30, 40), 10.0f);
  ASSERT_CLOSE_FLOAT(r.X, 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 2.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Z, 3.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.W, 4.0f, floatEpsilon);
}

TEST(vec4LenSq) {
  const float lenSq = vec4LenSq(MAKE_VEC4(1, 2, 2, 0));
  ASSERT_CLOSE_FLOAT(lenSq, 9.0f, floatEpsilon);
}

TEST(vec4Dist) {
  const float dist = vec4Dist(MAKE_VEC4(0, 0, 0, 0), MAKE_VEC4(1, 2, 2, 0));
  ASSERT_CLOSE_FLOAT(dist, 3.0f, floatEpsilon);
}

TEST(mat4Add) {
  const struct Mat4 a = makeMat4ScaleUniform(2.0f);
  const struct Mat4 b = makeMat4ScaleUniform(3.0f);
  const struct Mat4 r = mat4Add(a, b);
  ASSERT_CLOSE_FLOAT(r.m00, 5.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.m11, 5.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.m22, 5.0f, floatEpsilon);
}

TEST(mat4Sub) {
  const struct Mat4 a = makeMat4ScaleUniform(5.0f);
  const struct Mat4 b = makeMat4ScaleUniform(2.0f);
  const struct Mat4 r = mat4Sub(a, b);
  ASSERT_CLOSE_FLOAT(r.m00, 3.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.m11, 3.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.m22, 3.0f, floatEpsilon);
}

TEST(mat4ScaleScalar) {
  const struct Mat4 m = makeMat4ScaleUniform(2.0f);
  const struct Mat4 r = mat4Scale(m, 3.0f);
  ASSERT_CLOSE_FLOAT(r.m00, 6.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.m11, 6.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.m22, 6.0f, floatEpsilon);
}

TEST(mat4DivScalar) {
  const struct Mat4 m = makeMat4ScaleUniform(6.0f);
  const struct Mat4 r = mat4Div(m, 2.0f);
  ASSERT_CLOSE_FLOAT(r.m00, 3.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.m11, 3.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.m22, 3.0f, floatEpsilon);
}

TEST(mat4AffineInverse) {
  const struct Mat4 t = makeMat4Translation(MAKE_VEC3(1, 2, 3));
  const struct Mat4 inv = mat4AffineInverse(t);
  const struct Mat4 r = mat4Mul(t, inv);
  ASSERT_CLOSE_FLOAT(r.m00, 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.m11, 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.m22, 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.m33, 1.0f, floatEpsilon);
}

TEST(mat4Rotation) {
  const struct Mat4 r = makeMat4Rotation(MAKE_VEC3(0, 1, 0), Pi / 2.0f);
  const struct Vec4 v = mat4MulVec4(r, MAKE_VEC4(1, 0, 0, 1));
  ASSERT_CLOSE_FLOAT(v.X, 0.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(v.Z, -1.0f, floatEpsilon);
}

TEST(orthographicProjection) {
  const struct Mat4 p =
      makeOrthographicProjectionMatrix(-1, 1, -1, 1, 0.1f, 100.0f);
  const struct Vec4 v = mat4MulVec4(p, MAKE_VEC4(0, 0, 0, 1));
  ASSERT_CLOSE_FLOAT(v.X, 0.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(v.Y, 0.0f, floatEpsilon);
}

TEST(vec3Len) {
  const float len = vec3Len(MAKE_VEC3(1, 2, 2));
  ASSERT_CLOSE_FLOAT(len, 3.0f, floatEpsilon);
}

TEST(vec4Len) {
  const float len = vec4Len(MAKE_VEC4(1, 2, 2, 0));
  ASSERT_CLOSE_FLOAT(len, 3.0f, floatEpsilon);
}

TEST(vec3CrossAnticommutative) {
  const struct Vec3 a = vec3Cross(MAKE_VEC3(1, 0, 0), MAKE_VEC3(0, 1, 0));
  const struct Vec3 b = vec3Cross(MAKE_VEC3(0, 1, 0), MAKE_VEC3(1, 0, 0));
  ASSERT_CLOSE_FLOAT(a.Z, -b.Z, floatEpsilon);
}

TEST(mat4InverseSingular) {
  const struct Mat4 inv = mat4Inverse(MAT4_ZERO);
  ASSERT_CLOSE_FLOAT(inv.m00, 0.0f, floatEpsilon);
}

TEST(vec2Copy) {
  struct Vec2 dst;
  vec2Copy(MAKE_VEC2(1, 2), &dst);
  ASSERT_CLOSE_FLOAT(dst.X, 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(dst.Y, 2.0f, floatEpsilon);
}

TEST(vec3Copy) {
  struct Vec3 dst;
  vec3Copy(MAKE_VEC3(1, 2, 3), &dst);
  ASSERT_CLOSE_FLOAT(dst.X, 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(dst.Y, 2.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(dst.Z, 3.0f, floatEpsilon);
}

TEST(vec4Copy) {
  struct Vec4 dst;
  vec4Copy(MAKE_VEC4(1, 2, 3, 4), &dst);
  ASSERT_CLOSE_FLOAT(dst.X, 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(dst.Y, 2.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(dst.Z, 3.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(dst.W, 4.0f, floatEpsilon);
}

TEST(mat4Copy) {
  struct Mat4 dst;
  mat4Copy(MAT4_IDENTITY, &dst);
  ASSERT_CLOSE_FLOAT(dst.m00, 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(dst.m11, 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(dst.m22, 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(dst.m33, 1.0f, floatEpsilon);
}

TEST(mulMat4ScaleUniform) {
  const struct Mat4 r = mulMat4ScaleUniform(MAT4_IDENTITY, 2.0f);
  ASSERT_CLOSE_FLOAT(r.m00, 2.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.m11, 2.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.m22, 2.0f, floatEpsilon);
}

TEST(mulMat4Scale) {
  const struct Mat4 r = mulMat4Scale(MAT4_IDENTITY, MAKE_VEC3(2, 3, 4));
  ASSERT_CLOSE_FLOAT(r.m00, 2.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.m11, 3.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.m22, 4.0f, floatEpsilon);
}

TEST(mulMat4Rotation) {
  const struct Mat4 r =
      mulMat4Rotation(MAT4_IDENTITY, MAKE_VEC3(0, 1, 0), Pi / 2.0f);
  const struct Vec4 v = mat4MulVec4(r, MAKE_VEC4(1, 0, 0, 1));
  ASSERT_CLOSE_FLOAT(v.X, 0.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(v.Z, -1.0f, floatEpsilon);
}

TEST(mulMat4RotationX) {
  const struct Mat4 r = mulMat4RotationX(MAT4_IDENTITY, Pi / 2.0f);
  const struct Vec4 v = mat4MulVec4(r, MAKE_VEC4(0, 1, 0, 1));
  ASSERT_CLOSE_FLOAT(v.Y, 0.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(v.Z, 1.0f, floatEpsilon);
}

TEST(mulMat4RotationY) {
  const struct Mat4 r = mulMat4RotationY(MAT4_IDENTITY, Pi / 2.0f);
  const struct Vec4 v = mat4MulVec4(r, MAKE_VEC4(1, 0, 0, 1));
  ASSERT_CLOSE_FLOAT(v.X, 0.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(v.Z, 1.0f, floatEpsilon);
}

TEST(mulMat4RotationZ) {
  const struct Mat4 r = mulMat4RotationZ(MAT4_IDENTITY, Pi / 2.0f);
  const struct Vec4 v = mat4MulVec4(r, MAKE_VEC4(1, 0, 0, 1));
  ASSERT_CLOSE_FLOAT(v.X, 0.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(v.Y, 1.0f, floatEpsilon);
}

TEST(mulMat4Translation) {
  const struct Mat4 r = mulMat4Translation(MAT4_IDENTITY, MAKE_VEC3(1, 2, 3));
  const struct Vec4 v = mat4MulVec4(r, MAKE_VEC4(0, 0, 0, 1));
  ASSERT_CLOSE_FLOAT(v.X, 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(v.Y, 2.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(v.Z, 3.0f, floatEpsilon);
}
