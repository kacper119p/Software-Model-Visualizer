#include "graphicsMath.h"
#include "testFramework.h"

static constexpr float floatEpsilon = 0.00001f;

TEST(testVec3Add) {
  const vec3 r = vec3Add(MAKE_VEC3(1, 2, 3), MAKE_VEC3(4, 5, 6));
  ASSERT_CLOSE_FLOAT(r.X, 5.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 7.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Z, 9.0f, floatEpsilon);
}

TEST(testVec3Sub) {
  const vec3 r = vec3Sub(MAKE_VEC3(5, 7, 9), MAKE_VEC3(1, 2, 3));
  ASSERT_CLOSE_FLOAT(r.X, 4.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 5.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Z, 6.0f, floatEpsilon);
}

TEST(testVec3Scale) {
  const vec3 r = vec3Scale(MAKE_VEC3(1, 2, 3), 2.0f);
  ASSERT_CLOSE_FLOAT(r.X, 2.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 4.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Z, 6.0f, floatEpsilon);
}

TEST(testVec3Dot) {
  float d = vec3Dot(MAKE_VEC3(1, 0, 0), MAKE_VEC3(0, 1, 0));
  ASSERT_CLOSE_FLOAT(d, 0.0f, floatEpsilon);

  d = vec3Dot(MAKE_VEC3(1, 0, 0), MAKE_VEC3(1, 0, 0));
  ASSERT_CLOSE_FLOAT(d, 1.0f, floatEpsilon);
}

TEST(testVec3Cross) {
  const vec3 r = vec3Cross(MAKE_VEC3(1, 0, 0), MAKE_VEC3(0, 1, 0));
  ASSERT_CLOSE_FLOAT(r.X, 0.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 0.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Z, 1.0f, floatEpsilon);
}

TEST(testVec3Normalize) {
  const vec3 r = vec3Normalize(MAKE_VEC3(3, 0, 0));
  ASSERT_CLOSE_FLOAT(vec3Len(r), 1.0f, floatEpsilon);
}

TEST(testVec4Add) {
  const vec4 r = vec4Add(MAKE_VEC4(1, 2, 3, 4), MAKE_VEC4(5, 6, 7, 8));
  ASSERT_CLOSE_FLOAT(r.X, 6.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 8.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Z, 10.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.W, 12.0f, floatEpsilon);
}

TEST(testVec4Dot) {
  const float d = vec4Dot(MAKE_VEC4(1, 2, 3, 4), MAKE_VEC4(5, 6, 7, 8));
  ASSERT_CLOSE_FLOAT(d, 70.0f, floatEpsilon);
}

TEST(testMat4IdentityMul) {
  const vec4 v = MAKE_VEC4(1, 2, 3, 1);
  const vec4 r = mat4MulVec4(MAT4_IDENTITY, v);
  ASSERT_CLOSE_FLOAT(r.X, 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 2.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Z, 3.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.W, 1.0f, floatEpsilon);
}

TEST(testMat4Mul) {
  const mat4 a = makeMat4Translation(MAKE_VEC3(1, 0, 0));
  const mat4 b = makeMat4Translation(MAKE_VEC3(2, 0, 0));
  const mat4 r = mat4Mul(a, b);
  const vec4 v = mat4MulVec4(r, MAKE_VEC4(0, 0, 0, 1));
  ASSERT_CLOSE_FLOAT(v.X, 3.0f, floatEpsilon);
}

TEST(testMat4Translation) {
  const mat4 t = makeMat4Translation(MAKE_VEC3(1, 2, 3));
  const vec4 r = mat4MulVec4(t, MAKE_VEC4(0, 0, 0, 1));
  ASSERT_CLOSE_FLOAT(r.X, 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 2.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Z, 3.0f, floatEpsilon);
}

TEST(testMat4RotationY) {
  const mat4 r = makeMat4RotationY(Pi / 2.0f);
  const vec4 v = mat4MulVec4(r, MAKE_VEC4(1, 0, 0, 1));
  ASSERT_CLOSE_FLOAT(v.X, 0.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(v.Z, 1.0f, floatEpsilon);
}

TEST(testMat4Scale) {
  const mat4 s = makeMat4Scale(MAKE_VEC3(2, 3, 4));
  const vec4 v = mat4MulVec4(s, MAKE_VEC4(1, 1, 1, 1));
  ASSERT_CLOSE_FLOAT(v.X, 2.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(v.Y, 3.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(v.Z, 4.0f, floatEpsilon);
}

TEST(testMat4Inverse) {
  const mat4 t = makeMat4Translation(MAKE_VEC3(1, 2, 3));
  const mat4 inv = mat4Inverse(t);
  const mat4 r = mat4Mul(t, inv);
  ASSERT_CLOSE_FLOAT(r.Value[0][0], 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Value[1][1], 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Value[2][2], 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Value[3][3], 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Value[3][0], 0.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Value[3][1], 0.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Value[3][2], 0.0f, floatEpsilon);
}

TEST(testMat4LookAt) {
  const mat4 v = makeMat4LookAt(MAKE_VEC3(0, 0, 5), MAKE_VEC3(0, 0, 0),
                                 MAKE_VEC3(0, 1, 0));
  const vec4 p = mat4MulVec4(v, MAKE_VEC4(0, 0, 0, 1));
  ASSERT_CLOSE_FLOAT(p.Z, -5.0f, floatEpsilon);
}

TEST(testVec2Add) {
  const vec2 r = vec2Add(MAKE_VEC2(1, 2), MAKE_VEC2(3, 4));
  ASSERT_CLOSE_FLOAT(r.X, 4.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 6.0f, floatEpsilon);
}

TEST(testVec2Sub) {
  const vec2 r = vec2Sub(MAKE_VEC2(5, 7), MAKE_VEC2(2, 3));
  ASSERT_CLOSE_FLOAT(r.X, 3.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 4.0f, floatEpsilon);
}

TEST(testVec2Dot) {
  const float d = vec2Dot(MAKE_VEC2(1, 2), MAKE_VEC2(3, 4));
  ASSERT_CLOSE_FLOAT(d, 11.0f, floatEpsilon);
}

TEST(testVec2Normalize) {
  const vec2 r = vec2Normalize(MAKE_VEC2(3, 0));
  ASSERT_CLOSE_FLOAT(vec2Len(r), 1.0f, floatEpsilon);
}

TEST(testVec3Mul) {
  const vec3 r = vec3Mul(MAKE_VEC3(2, 3, 4), MAKE_VEC3(5, 6, 7));
  ASSERT_CLOSE_FLOAT(r.X, 10.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 18.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Z, 28.0f, floatEpsilon);
}

TEST(testVec3Div) {
  const vec3 r = vec3Div(MAKE_VEC3(10, 20, 30), 10.0f);
  ASSERT_CLOSE_FLOAT(r.X, 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 2.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Z, 3.0f, floatEpsilon);
}

TEST(testVec4Sub) {
  const vec4 r = vec4Sub(MAKE_VEC4(10, 20, 30, 40), MAKE_VEC4(1, 2, 3, 4));
  ASSERT_CLOSE_FLOAT(r.X, 9.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 18.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Z, 27.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.W, 36.0f, floatEpsilon);
}

TEST(testVec4Scale) {
  const vec4 r = vec4Scale(MAKE_VEC4(1, 2, 3, 4), 2.0f);
  ASSERT_CLOSE_FLOAT(r.X, 2.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 4.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Z, 6.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.W, 8.0f, floatEpsilon);
}

TEST(testVec4Normalize) {
  const vec4 r = vec4Normalize(MAKE_VEC4(3, 0, 0, 0));
  ASSERT_CLOSE_FLOAT(vec4Len(r), 1.0f, floatEpsilon);
}

TEST(testMat4RotationX) {
  const mat4 r = makeMat4RotationX(Pi / 2.0f);
  const vec4 v = mat4MulVec4(r, MAKE_VEC4(0, 1, 0, 1));
  ASSERT_CLOSE_FLOAT(v.Y, 0.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(v.Z, 1.0f, floatEpsilon);
}

TEST(testMat4RotationZ) {
  const mat4 r = makeMat4RotationZ(Pi / 2.0f);
  const vec4 v = mat4MulVec4(r, MAKE_VEC4(1, 0, 0, 1));
  ASSERT_CLOSE_FLOAT(v.X, 0.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(v.Y, 1.0f, floatEpsilon);
}

TEST(testMat4ScaleUniform) {
  const mat4 s = makeMat4ScaleUniform(2.0f);
  const vec4 v = mat4MulVec4(s, MAKE_VEC4(1, 1, 1, 1));
  ASSERT_CLOSE_FLOAT(v.X, 2.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(v.Y, 2.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(v.Z, 2.0f, floatEpsilon);
}

TEST(testMat4Transpose) {
  const mat4 m = makeMat4Translation(MAKE_VEC3(1, 2, 3));
  const mat4 t = mat4Transpose(m);
  ASSERT_CLOSE_FLOAT(t.Value[0][3], 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(t.Value[1][3], 2.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(t.Value[2][3], 3.0f, floatEpsilon);
}

TEST(testPerspectiveProjection) {
  const mat4 p = makePerspectiveProjectionMatrix(Pi / 4.0f, 1.0f, 0.1f, 100.0f);
  const vec4 v = mat4MulVec4(p, MAKE_VEC4(0, 0, -1, 1));
  ASSERT_CLOSE_FLOAT(v.W, 1.0f, floatEpsilon);
}

TEST(testVec2Mul) {
  const vec2 r = vec2Mul(MAKE_VEC2(2, 3), MAKE_VEC2(4, 5));
  ASSERT_CLOSE_FLOAT(r.X, 8.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 15.0f, floatEpsilon);
}

TEST(testVec2Scale) {
  const vec2 r = vec2Scale(MAKE_VEC2(2, 3), 3.0f);
  ASSERT_CLOSE_FLOAT(r.X, 6.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 9.0f, floatEpsilon);
}

TEST(testVec2Div) {
  const vec2 r = vec2Div(MAKE_VEC2(10, 20), 5.0f);
  ASSERT_CLOSE_FLOAT(r.X, 2.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 4.0f, floatEpsilon);
}

TEST(testVec2LenSq) {
  const float lenSq = vec2LenSq(MAKE_VEC2(3, 4));
  ASSERT_CLOSE_FLOAT(lenSq, 25.0f, floatEpsilon);
}

TEST(testVec2Len) {
  const float len = vec2Len(MAKE_VEC2(3, 4));
  ASSERT_CLOSE_FLOAT(len, 5.0f, floatEpsilon);
}

TEST(testVec2Dist) {
  const float dist = vec2Dist(MAKE_VEC2(0, 0), MAKE_VEC2(3, 4));
  ASSERT_CLOSE_FLOAT(dist, 5.0f, floatEpsilon);
}

TEST(testVec3LenSq) {
  const float lenSq = vec3LenSq(MAKE_VEC3(1, 2, 2));
  ASSERT_CLOSE_FLOAT(lenSq, 9.0f, floatEpsilon);
}

TEST(testVec3Dist) {
  const float dist = vec3Dist(MAKE_VEC3(0, 0, 0), MAKE_VEC3(1, 2, 2));
  ASSERT_CLOSE_FLOAT(dist, 3.0f, floatEpsilon);
}

TEST(testVec4Mul) {
  const vec4 r = vec4Mul(MAKE_VEC4(2, 3, 4, 5), MAKE_VEC4(6, 7, 8, 9));
  ASSERT_CLOSE_FLOAT(r.X, 12.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 21.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Z, 32.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.W, 45.0f, floatEpsilon);
}

TEST(testVec4Div) {
  const vec4 r = vec4Div(MAKE_VEC4(10, 20, 30, 40), 10.0f);
  ASSERT_CLOSE_FLOAT(r.X, 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 2.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Z, 3.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.W, 4.0f, floatEpsilon);
}

TEST(testVec4LenSq) {
  const float lenSq = vec4LenSq(MAKE_VEC4(1, 2, 2, 0));
  ASSERT_CLOSE_FLOAT(lenSq, 9.0f, floatEpsilon);
}

TEST(testVec4Dist) {
  const float dist = vec4Dist(MAKE_VEC4(0, 0, 0, 0), MAKE_VEC4(1, 2, 2, 0));
  ASSERT_CLOSE_FLOAT(dist, 3.0f, floatEpsilon);
}

TEST(testMat4Add) {
  const mat4 a = makeMat4ScaleUniform(2.0f);
  const mat4 b = makeMat4ScaleUniform(3.0f);
  const mat4 r = mat4Add(a, b);
  ASSERT_CLOSE_FLOAT(r.Value[0][0], 5.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Value[1][1], 5.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Value[2][2], 5.0f, floatEpsilon);
}

TEST(testMat4Sub) {
  const mat4 a = makeMat4ScaleUniform(5.0f);
  const mat4 b = makeMat4ScaleUniform(2.0f);
  const mat4 r = mat4Sub(a, b);
  ASSERT_CLOSE_FLOAT(r.Value[0][0], 3.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Value[1][1], 3.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Value[2][2], 3.0f, floatEpsilon);
}

TEST(testMat4ScaleScalar) {
  const mat4 m = makeMat4ScaleUniform(2.0f);
  const mat4 r = mat4Scale(m, 3.0f);
  ASSERT_CLOSE_FLOAT(r.Value[0][0], 6.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Value[1][1], 6.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Value[2][2], 6.0f, floatEpsilon);
}

TEST(testMat4DivScalar) {
  const mat4 m = makeMat4ScaleUniform(6.0f);
  const mat4 r = mat4Div(m, 2.0f);
  ASSERT_CLOSE_FLOAT(r.Value[0][0], 3.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Value[1][1], 3.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Value[2][2], 3.0f, floatEpsilon);
}

TEST(testMat4AffineInverse) {
  const mat4 t = makeMat4Translation(MAKE_VEC3(1, 2, 3));
  const mat4 inv = mat4AffineInverse(t);
  const mat4 r = mat4Mul(t, inv);
  ASSERT_CLOSE_FLOAT(r.Value[0][0], 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Value[1][1], 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Value[2][2], 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Value[3][3], 1.0f, floatEpsilon);
}

TEST(testMat4Rotation) {
  const mat4 r = makeMat4Rotation(MAKE_VEC3(0, 1, 0), Pi / 2.0f);
  const vec4 v = mat4MulVec4(r, MAKE_VEC4(1, 0, 0, 1));
  ASSERT_CLOSE_FLOAT(v.X, 0.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(v.Z, -1.0f, floatEpsilon);
}

TEST(testOrthographicProjection) {
  const mat4 p = makeOrthographicProjectionMatrix(-1, 1, -1, 1, 0.1f, 100.0f);
  const vec4 v = mat4MulVec4(p, MAKE_VEC4(0, 0, 0, 1));
  ASSERT_CLOSE_FLOAT(v.X, 0.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(v.Y, 0.0f, floatEpsilon);
}

TEST(testVec3Len) {
  const float len = vec3Len(MAKE_VEC3(1, 2, 2));
  ASSERT_CLOSE_FLOAT(len, 3.0f, floatEpsilon);
}

TEST(testVec4Len) {
  const float len = vec4Len(MAKE_VEC4(1, 2, 2, 0));
  ASSERT_CLOSE_FLOAT(len, 3.0f, floatEpsilon);
}

TEST(testVec3CrossAnticommutative) {
  const vec3 a = vec3Cross(MAKE_VEC3(1, 0, 0), MAKE_VEC3(0, 1, 0));
  const vec3 b = vec3Cross(MAKE_VEC3(0, 1, 0), MAKE_VEC3(1, 0, 0));
  ASSERT_CLOSE_FLOAT(a.Z, -b.Z, floatEpsilon);
}

TEST(testMat4InverseSingular) {
  const mat4 inv = mat4Inverse(MAT4_ZERO);
  ASSERT_CLOSE_FLOAT(inv.Value[0][0], 0.0f, floatEpsilon);
}

TEST(testVec2Copy) {
  vec2 dst;
  vec2Copy(MAKE_VEC2(1, 2), &dst);
  ASSERT_CLOSE_FLOAT(dst.X, 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(dst.Y, 2.0f, floatEpsilon);
}

TEST(testVec3Copy) {
  vec3 dst;
  vec3Copy(MAKE_VEC3(1, 2, 3), &dst);
  ASSERT_CLOSE_FLOAT(dst.X, 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(dst.Y, 2.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(dst.Z, 3.0f, floatEpsilon);
}

TEST(testVec4Copy) {
  vec4 dst;
  vec4Copy(MAKE_VEC4(1, 2, 3, 4), &dst);
  ASSERT_CLOSE_FLOAT(dst.X, 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(dst.Y, 2.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(dst.Z, 3.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(dst.W, 4.0f, floatEpsilon);
}

TEST(testMat4Copy) {
  mat4 dst;
  mat4Copy(MAT4_IDENTITY, &dst);
  ASSERT_CLOSE_FLOAT(dst.Value[0][0], 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(dst.Value[1][1], 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(dst.Value[2][2], 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(dst.Value[3][3], 1.0f, floatEpsilon);
}

TEST(testMulMat4ScaleUniform) {
  const mat4 r = mulMat4ScaleUniform(MAT4_IDENTITY, 2.0f);
  ASSERT_CLOSE_FLOAT(r.Value[0][0], 2.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Value[1][1], 2.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Value[2][2], 2.0f, floatEpsilon);
}

TEST(testMulMat4Scale) {
  const mat4 r = mulMat4Scale(MAT4_IDENTITY, MAKE_VEC3(2, 3, 4));
  ASSERT_CLOSE_FLOAT(r.Value[0][0], 2.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Value[1][1], 3.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(r.Value[2][2], 4.0f, floatEpsilon);
}

TEST(testMulMat4Rotation) {
  const mat4 r = mulMat4Rotation(MAT4_IDENTITY, MAKE_VEC3(0, 1, 0), Pi / 2.0f);
  const vec4 v = mat4MulVec4(r, MAKE_VEC4(1, 0, 0, 1));
  ASSERT_CLOSE_FLOAT(v.X, 0.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(v.Z, -1.0f, floatEpsilon);
}

TEST(testMulMat4RotationX) {
  const mat4 r = mulMat4RotationX(MAT4_IDENTITY, Pi / 2.0f);
  const vec4 v = mat4MulVec4(r, MAKE_VEC4(0, 1, 0, 1));
  ASSERT_CLOSE_FLOAT(v.Y, 0.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(v.Z, 1.0f, floatEpsilon);
}

TEST(testMulMat4RotationY) {
  const mat4 r = mulMat4RotationY(MAT4_IDENTITY, Pi / 2.0f);
  const vec4 v = mat4MulVec4(r, MAKE_VEC4(1, 0, 0, 1));
  ASSERT_CLOSE_FLOAT(v.X, 0.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(v.Z, 1.0f, floatEpsilon);
}

TEST(testMulMat4RotationZ) {
  const mat4 r = mulMat4RotationZ(MAT4_IDENTITY, Pi / 2.0f);
  const vec4 v = mat4MulVec4(r, MAKE_VEC4(1, 0, 0, 1));
  ASSERT_CLOSE_FLOAT(v.X, 0.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(v.Y, 1.0f, floatEpsilon);
}

TEST(testMulMat4Translation) {
  const mat4 r = mulMat4Translation(MAT4_IDENTITY, MAKE_VEC3(1, 2, 3));
  const vec4 v = mat4MulVec4(r, MAKE_VEC4(0, 0, 0, 1));
  ASSERT_CLOSE_FLOAT(v.X, 1.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(v.Y, 2.0f, floatEpsilon);
  ASSERT_CLOSE_FLOAT(v.Z, 3.0f, floatEpsilon);
}
