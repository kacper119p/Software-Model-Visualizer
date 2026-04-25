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
#ifndef SOFTWARE_MODEL_VISUALIZER_GRAPHICS_MATH_H
#define SOFTWARE_MODEL_VISUALIZER_GRAPHICS_MATH_H
#include <math.h>

#if defined(__GNUC__) || defined(__clang__)
#define GRAPHICS_MATH_INLINE static inline __attribute((always_inline))
#elif defined(_MSC_VER)
#define GRAPHICS_MATH_INLINE __forceinline
#else
#define GRAPHICS_MATH_INLINE static inline
#endif

#if defined(__GNUC__) || defined(__clang__)
#define GRAPHICS_MATH_UNLIKELY(expr) __builtin_expect(!!(expr), 0)
#define GRAPHICS_MATH_LIKELY(expr) __builtin_expect(!!(expr), 1)
#else
#define GRAPHICS_MATH_UNLIKELY(expr) (expr)
#define GRAPHICS_MATH_LIKELY(expr) (expr)
#endif

#if defined(__GNUC__) || defined(__clang__)
#define GRAPHICS_MATH_ALIGNAS(n) __attribute__((aligned(n)))
#elif defined(_MSC_VER)
#define GRAPHICS_MATH_ALIGNAS(n) __declspec(align(n))
#else
#define GRAPHICS_MATH_ALIGNAS(n) alignas(n)
#endif

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

/*******************************************************************************
 * Constants
 ******************************************************************************/

constexpr float Pi = 3.1415926535897932384626433832795028841971693993751058209f;
constexpr float DegToRad = Pi / 180.0f;
constexpr float RadToDeg = 180.0f / Pi;

/*******************************************************************************
 * Types
 ******************************************************************************/

struct GRAPHICS_MATH_ALIGNAS(8) Vec2 {
  float X;
  float Y;
};

struct GRAPHICS_MATH_ALIGNAS(4) Vec3 {
  float X;
  float Y;
  float Z;
};

struct GRAPHICS_MATH_ALIGNAS(16) Vec4 {
  float X;
  float Y;
  float Z;
  float W;
};

struct GRAPHICS_MATH_ALIGNAS(16) Mat4 {
  float m00;
  float m10;
  float m20;
  float m30;
  float m01;
  float m11;
  float m21;
  float m31;
  float m02;
  float m12;
  float m22;
  float m32;
  float m03;
  float m13;
  float m23;
  float m33;
};

/*******************************************************************************
 * Constructor macros
 ******************************************************************************/

#define MAKE_VEC2(x, y) ((struct Vec2){(x), (y)})
#define MAKE_VEC3(x, y, z) ((struct Vec3){(x), (y), (z)})
#define MAKE_VEC4(x, y, z, w) ((struct Vec4){(x), (y), (z), (w)})
#define MAKE_MAT4(m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23,  \
                  m30, m31, m32, m33)                                          \
  ((struct Mat4){(m00), (m01), (m02), (m03), (m10), (m11), (m12), (m13),       \
                 (m20), (m21), (m22), (m23), (m30), (m31), (m32), (m33)})

/*******************************************************************************
 * Common values
 ******************************************************************************/

#define VEC2_ZERO ((struct Vec2){0.0f, 0.0f})
#define VEC2_ONE ((struct Vec2){1.0f, 1.0f})
#define VEC2_UP ((struct Vec2){0.0f, 1.0f})
#define VEC2_RIGHT ((struct Vec2){1.0f, 0.0f})

#define VEC3_ZERO ((struct Vec3){0.0f, 0.0f, 0.0f})
#define VEC3_ONE ((struct Vec3){1.0f, 1.0f, 1.0f})
#define VEC3_UP ((struct Vec3){0.0f, 1.0f, 0.0f})
#define VEC3_RIGHT ((struct Vec3){1.0f, 0.0f, 0.0f})
#define VEC3_FORWARD ((struct Vec3){0.0f, 0.0f, -1.0f})

#define VEC4_ZERO ((struct Vec4){0.0f, 0.0f, 0.0f, 0.0f})
#define VEC4_ONE ((struct Vec4){1.0f, 1.0f, 1.0f, 1.0f})

#define MAT4_ZERO                                                              \
  ((struct Mat4){0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,   \
                 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f})
#define MAT4_IDENTITY                                                          \
  ((struct Mat4){1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,   \
                 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f})

/*******************************************************************************
 * Vec2 operations
 ******************************************************************************/

GRAPHICS_MATH_INLINE void vec2Copy(const struct Vec2 Src, struct Vec2* Dst) {
  *Dst = Src;
}

GRAPHICS_MATH_INLINE struct Vec2 vec2Add(const struct Vec2 A,
                                         const struct Vec2 B) {
  return (struct Vec2){A.X + B.X, A.Y + B.Y};
}

GRAPHICS_MATH_INLINE struct Vec2 vec2Sub(const struct Vec2 A,
                                         const struct Vec2 B) {
  return (struct Vec2){A.X - B.X, A.Y - B.Y};
}

GRAPHICS_MATH_INLINE struct Vec2 vec2Mul(const struct Vec2 A,
                                         const struct Vec2 B) {
  return (struct Vec2){A.X * B.X, A.Y * B.Y};
}

GRAPHICS_MATH_INLINE struct Vec2 vec2Scale(const struct Vec2 A,
                                           const float Scalar) {
  return (struct Vec2){A.X * Scalar, A.Y * Scalar};
}

GRAPHICS_MATH_INLINE struct Vec2 vec2Div(const struct Vec2 A,
                                         const float Scalar) {
  const float inverse = 1.0f / Scalar;
  return (struct Vec2){A.X * inverse, A.Y * inverse};
}

GRAPHICS_MATH_INLINE float vec2Dot(const struct Vec2 A, const struct Vec2 B) {
  return A.X * B.X + A.Y * B.Y;
}

GRAPHICS_MATH_INLINE float vec2LenSq(const struct Vec2 A) {
  return vec2Dot(A, A);
}

GRAPHICS_MATH_INLINE float vec2Len(const struct Vec2 A) {
  return sqrtf(vec2LenSq(A));
}

GRAPHICS_MATH_INLINE struct Vec2 vec2Normalize(const struct Vec2 A) {
  return vec2Div(A, vec2Len(A));
}

GRAPHICS_MATH_INLINE float vec2Dist(const struct Vec2 A, const struct Vec2 B) {
  return vec2Len(vec2Sub(A, B));
}

/*******************************************************************************
 * Vec3 operations
 ******************************************************************************/

GRAPHICS_MATH_INLINE void vec3Copy(const struct Vec3 Src, struct Vec3* Dst) {
  *Dst = Src;
}

GRAPHICS_MATH_INLINE struct Vec3 vec3Add(const struct Vec3 A,
                                         const struct Vec3 B) {
  return (struct Vec3){A.X + B.X, A.Y + B.Y, A.Z + B.Z};
}

GRAPHICS_MATH_INLINE struct Vec3 vec3Sub(const struct Vec3 A,
                                         const struct Vec3 B) {
  return (struct Vec3){A.X - B.X, A.Y - B.Y, A.Z - B.Z};
}

GRAPHICS_MATH_INLINE struct Vec3 vec3Mul(const struct Vec3 A,
                                         const struct Vec3 B) {
  return (struct Vec3){A.X * B.X, A.Y * B.Y, A.Z * B.Z};
}

GRAPHICS_MATH_INLINE struct Vec3 vec3Scale(const struct Vec3 A,
                                           const float Scalar) {
  return (struct Vec3){A.X * Scalar, A.Y * Scalar, A.Z * Scalar};
}

GRAPHICS_MATH_INLINE struct Vec3 vec3Div(const struct Vec3 A,
                                         const float Scalar) {
  const float inverse = 1.0f / Scalar;
  return (struct Vec3){A.X * inverse, A.Y * inverse, A.Z * inverse};
}

GRAPHICS_MATH_INLINE float vec3Dot(const struct Vec3 A, const struct Vec3 B) {
  return A.X * B.X + A.Y * B.Y + A.Z * B.Z;
}

GRAPHICS_MATH_INLINE float vec3LenSq(const struct Vec3 A) {
  return vec3Dot(A, A);
}

GRAPHICS_MATH_INLINE float vec3Len(const struct Vec3 A) {
  return sqrtf(vec3LenSq(A));
}

GRAPHICS_MATH_INLINE struct Vec3 vec3Normalize(const struct Vec3 A) {
  return vec3Div(A, vec3Len(A));
}

GRAPHICS_MATH_INLINE float vec3Dist(const struct Vec3 A, const struct Vec3 B) {
  return vec3Len(vec3Sub(A, B));
}

GRAPHICS_MATH_INLINE struct Vec3 vec3Cross(const struct Vec3 A,
                                           const struct Vec3 B) {
  return (struct Vec3){A.Y * B.Z - A.Z * B.Y, A.Z * B.X - A.X * B.Z,
                       A.X * B.Y - A.Y * B.X};
}

/*******************************************************************************
 * Vec4 operations
 ******************************************************************************/

GRAPHICS_MATH_INLINE void vec4Copy(const struct Vec4 Src, struct Vec4* Dst) {
  *Dst = Src;
}

GRAPHICS_MATH_INLINE struct Vec4 vec4Add(const struct Vec4 A,
                                         const struct Vec4 B) {
  return MAKE_VEC4(A.X + B.X, A.Y + B.Y, A.Z + B.Z, A.W + B.W);
}

GRAPHICS_MATH_INLINE struct Vec4 vec4Sub(const struct Vec4 A,
                                         const struct Vec4 B) {
  return MAKE_VEC4(A.X - B.X, A.Y - B.Y, A.Z - B.Z, A.W - B.W);
}

GRAPHICS_MATH_INLINE struct Vec4 vec4Mul(const struct Vec4 A,
                                         const struct Vec4 B) {
  return MAKE_VEC4(A.X * B.X, A.Y * B.Y, A.Z * B.Z, A.W * B.W);
}

GRAPHICS_MATH_INLINE struct Vec4 vec4Scale(const struct Vec4 A,
                                           const float Scalar) {
  return MAKE_VEC4(A.X * Scalar, A.Y * Scalar, A.Z * Scalar, A.W * Scalar);
}

GRAPHICS_MATH_INLINE struct Vec4 vec4Div(const struct Vec4 A,
                                         const float Scalar) {
  const float inverse = 1.0f / Scalar;
  return MAKE_VEC4(A.X * inverse, A.Y * inverse, A.Z * inverse, A.W * inverse);
}

GRAPHICS_MATH_INLINE float vec4Dot(const struct Vec4 A, const struct Vec4 B) {
  return A.X * B.X + A.Y * B.Y + A.Z * B.Z + A.W * B.W;
}

GRAPHICS_MATH_INLINE float vec4LenSq(const struct Vec4 A) {
  return vec4Dot(A, A);
}

GRAPHICS_MATH_INLINE float vec4Len(const struct Vec4 A) {
  return sqrtf(vec4LenSq(A));
}

GRAPHICS_MATH_INLINE struct Vec4 vec4Normalize(const struct Vec4 A) {
  return vec4Div(A, vec4Len(A));
}

GRAPHICS_MATH_INLINE float vec4Dist(const struct Vec4 A, const struct Vec4 B) {
  return vec4Len(vec4Sub(A, B));
}

/*******************************************************************************
 * Mat4 operations
 ******************************************************************************/

GRAPHICS_MATH_INLINE void mat4Copy(const struct Mat4 Src, struct Mat4* Dst) {
  *Dst = Src;
}

GRAPHICS_MATH_INLINE struct Mat4 mat4Add(const struct Mat4 A,
                                         const struct Mat4 B) {
  return (struct Mat4){
      A.m00 + B.m00, A.m10 + B.m10, A.m20 + B.m20, A.m30 + B.m30,
      A.m01 + B.m01, A.m11 + B.m11, A.m21 + B.m21, A.m31 + B.m31,
      A.m02 + B.m02, A.m12 + B.m12, A.m22 + B.m22, A.m32 + B.m32,
      A.m03 + B.m03, A.m13 + B.m13, A.m23 + B.m23, A.m33 + B.m33};
}

GRAPHICS_MATH_INLINE struct Mat4 mat4Sub(const struct Mat4 A,
                                         const struct Mat4 B) {
  return (struct Mat4){
      A.m00 - B.m00, A.m10 - B.m10, A.m20 - B.m20, A.m30 - B.m30,
      A.m01 - B.m01, A.m11 - B.m11, A.m21 - B.m21, A.m31 - B.m31,
      A.m02 - B.m02, A.m12 - B.m12, A.m22 - B.m22, A.m32 - B.m32,
      A.m03 - B.m03, A.m13 - B.m13, A.m23 - B.m23, A.m33 - B.m33};
}

GRAPHICS_MATH_INLINE struct Mat4 mat4Scale(const struct Mat4 A,
                                           const float Scalar) {
  return (struct Mat4){
      A.m00 * Scalar, A.m10 * Scalar, A.m20 * Scalar, A.m30 * Scalar,
      A.m01 * Scalar, A.m11 * Scalar, A.m21 * Scalar, A.m31 * Scalar,
      A.m02 * Scalar, A.m12 * Scalar, A.m22 * Scalar, A.m32 * Scalar,
      A.m03 * Scalar, A.m13 * Scalar, A.m23 * Scalar, A.m33 * Scalar};
}

GRAPHICS_MATH_INLINE struct Mat4 mat4Div(const struct Mat4 A,
                                         const float Scalar) {
  const float inverse = 1.0f / Scalar;
  return (struct Mat4){
      A.m00 * inverse, A.m10 * inverse, A.m20 * inverse, A.m30 * inverse,
      A.m01 * inverse, A.m11 * inverse, A.m21 * inverse, A.m31 * inverse,
      A.m02 * inverse, A.m12 * inverse, A.m22 * inverse, A.m32 * inverse,
      A.m03 * inverse, A.m13 * inverse, A.m23 * inverse, A.m33 * inverse};
}

GRAPHICS_MATH_INLINE struct Mat4 mat4Mul(const struct Mat4 A,
                                         const struct Mat4 B) {
  return (struct Mat4){
      A.m00 * B.m00 + A.m01 * B.m10 + A.m02 * B.m20 + A.m03 * B.m30,
      A.m10 * B.m00 + A.m11 * B.m10 + A.m12 * B.m20 + A.m13 * B.m30,
      A.m20 * B.m00 + A.m21 * B.m10 + A.m22 * B.m20 + A.m23 * B.m30,
      A.m30 * B.m00 + A.m31 * B.m10 + A.m32 * B.m20 + A.m33 * B.m30,

      A.m00 * B.m01 + A.m01 * B.m11 + A.m02 * B.m21 + A.m03 * B.m31,
      A.m10 * B.m01 + A.m11 * B.m11 + A.m12 * B.m21 + A.m13 * B.m31,
      A.m20 * B.m01 + A.m21 * B.m11 + A.m22 * B.m21 + A.m23 * B.m31,
      A.m30 * B.m01 + A.m31 * B.m11 + A.m32 * B.m21 + A.m33 * B.m31,

      A.m00 * B.m02 + A.m01 * B.m12 + A.m02 * B.m22 + A.m03 * B.m32,
      A.m10 * B.m02 + A.m11 * B.m12 + A.m12 * B.m22 + A.m13 * B.m32,
      A.m20 * B.m02 + A.m21 * B.m12 + A.m22 * B.m22 + A.m23 * B.m32,
      A.m30 * B.m02 + A.m31 * B.m12 + A.m32 * B.m22 + A.m33 * B.m32,

      A.m00 * B.m03 + A.m01 * B.m13 + A.m02 * B.m23 + A.m03 * B.m33,
      A.m10 * B.m03 + A.m11 * B.m13 + A.m12 * B.m23 + A.m13 * B.m33,
      A.m20 * B.m03 + A.m21 * B.m13 + A.m22 * B.m23 + A.m23 * B.m33,
      A.m30 * B.m03 + A.m31 * B.m13 + A.m32 * B.m23 + A.m33 * B.m33,
  };
}

GRAPHICS_MATH_INLINE struct Vec4 mat4MulVec4(const struct Mat4 M,
                                             const struct Vec4 V) {
  return (struct Vec4){
      M.m00 * V.X + M.m01 * V.Y + M.m02 * V.Z + M.m03 * V.W,
      M.m10 * V.X + M.m11 * V.Y + M.m12 * V.Z + M.m13 * V.W,
      M.m20 * V.X + M.m21 * V.Y + M.m22 * V.Z + M.m23 * V.W,
      M.m30 * V.X + M.m31 * V.Y + M.m32 * V.Z + M.m33 * V.W,
  };
}

GRAPHICS_MATH_INLINE struct Mat4 mat4Inverse(const struct Mat4 M) {
  const float a0 = M.m00 * M.m11 - M.m10 * M.m01;
  const float a1 = M.m00 * M.m21 - M.m20 * M.m01;
  const float a2 = M.m00 * M.m31 - M.m30 * M.m01;
  const float a3 = M.m10 * M.m21 - M.m20 * M.m11;
  const float a4 = M.m10 * M.m31 - M.m30 * M.m11;
  const float a5 = M.m20 * M.m31 - M.m30 * M.m21;

  const float b0 = M.m02 * M.m13 - M.m12 * M.m03;
  const float b1 = M.m02 * M.m23 - M.m22 * M.m03;
  const float b2 = M.m02 * M.m33 - M.m32 * M.m03;
  const float b3 = M.m12 * M.m23 - M.m22 * M.m13;
  const float b4 = M.m12 * M.m33 - M.m32 * M.m13;
  const float b5 = M.m22 * M.m33 - M.m32 * M.m23;

  const float det = a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;
  if (GRAPHICS_MATH_UNLIKELY(det > -1e-6f && det < 1e-6f))
    return MAT4_ZERO;
  const float invDet = 1.0f / det;

  return (struct Mat4){
      (M.m11 * b5 - M.m21 * b4 + M.m31 * b3) * invDet,
      (-M.m10 * b5 + M.m20 * b4 - M.m30 * b3) * invDet,
      (M.m13 * a5 - M.m23 * a4 + M.m33 * a3) * invDet,
      (-M.m12 * a5 + M.m22 * a4 - M.m32 * a3) * invDet,

      (-M.m01 * b5 + M.m21 * b2 - M.m31 * b1) * invDet,
      (M.m00 * b5 - M.m20 * b2 + M.m30 * b1) * invDet,
      (-M.m03 * a5 + M.m23 * a2 - M.m33 * a1) * invDet,
      (M.m02 * a5 - M.m22 * a2 + M.m32 * a1) * invDet,

      (M.m01 * b4 - M.m11 * b2 + M.m31 * b0) * invDet,
      (-M.m00 * b4 + M.m10 * b2 - M.m30 * b0) * invDet,
      (M.m03 * a4 - M.m13 * a2 + M.m33 * a0) * invDet,
      (-M.m02 * a4 + M.m12 * a2 - M.m32 * a0) * invDet,

      (-M.m01 * b3 + M.m11 * b1 - M.m21 * b0) * invDet,
      (M.m00 * b3 - M.m10 * b1 + M.m20 * b0) * invDet,
      (-M.m03 * a3 + M.m13 * a1 - M.m23 * a0) * invDet,
      (M.m02 * a3 - M.m12 * a1 + M.m22 * a0) * invDet,
  };
}

GRAPHICS_MATH_INLINE struct Mat4 mat4AffineInverse(const struct Mat4 M) {
  const float x = 1.0f / (M.m00 * M.m00 + M.m10 * M.m10 + M.m20 * M.m20);
  const float y = 1.0f / (M.m01 * M.m01 + M.m11 * M.m11 + M.m21 * M.m21);
  const float z = 1.0f / (M.m02 * M.m02 + M.m12 * M.m12 + M.m22 * M.m22);
  const float r00 = M.m00 * x, r10 = M.m10 * x, r20 = M.m20 * x;
  const float r01 = M.m01 * y, r11 = M.m11 * y, r21 = M.m21 * y;
  const float r02 = M.m02 * z, r12 = M.m12 * z, r22 = M.m22 * z;
  return (struct Mat4){r00,
                       r10,
                       r20,
                       0.0f,
                       r01,
                       r11,
                       r21,
                       0.0f,
                       r02,
                       r12,
                       r22,
                       0.0f,
                       -(M.m03 * r00 + M.m13 * r10 + M.m23 * r20),
                       -(M.m03 * r01 + M.m13 * r11 + M.m23 * r21),
                       -(M.m03 * r02 + M.m13 * r12 + M.m23 * r22),
                       1.0f};
}

GRAPHICS_MATH_INLINE struct Mat4 mat4Transpose(const struct Mat4 M) {
  return (struct Mat4){M.m00, M.m01, M.m02, M.m03, M.m10, M.m11, M.m12, M.m13,
                       M.m20, M.m21, M.m22, M.m23, M.m30, M.m31, M.m32, M.m33};
}

/*******************************************************************************
 * Transformations
 ******************************************************************************/

GRAPHICS_MATH_INLINE struct Mat4
makePerspectiveProjectionMatrix(const float FovY, const float Aspect,
                                const float Near, const float Far) {
  const float tanInverse = 1.0f / tanf(FovY * 0.5f);
  const float inverseNearFar = 1.0f / (Near - Far);
  return (struct Mat4){
      tanInverse / Aspect,
      0.0f,
      0.0f,
      0.0f,

      0.0f,
      tanInverse,
      0.0f,
      0.0f,

      0.0f,
      0.0f,
      (Far + Near) * inverseNearFar,
      -1.0f,

      0.0f,
      0.0f,
      2.0f * Far * Near * inverseNearFar,
      0.0f,
  };
}

GRAPHICS_MATH_INLINE struct Mat4
makeOrthographicProjectionMatrix(const float Left, const float Right,
                                 const float Bottom, const float Top,
                                 const float Near, const float Far) {
  const float inverseRightLeft = 1.0f / (Right - Left);
  const float inverseTopBottom = 1.0f / (Top - Bottom);
  const float inverseNearFar = 1.0f / (Near - Far);
  return (struct Mat4){
      2.0f * inverseRightLeft,
      0.0f,
      0.0f,
      0.0f,
      0.0f,
      2.0f * inverseTopBottom,
      0.0f,
      0.0f,

      0.0f,
      0.0f,
      2.0f * inverseNearFar,
      0.0f,

      -(Right + Left) * inverseRightLeft,
      -(Top + Bottom) * inverseTopBottom,
      (Far + Near) * inverseNearFar,
      1.0f,
  };
}

GRAPHICS_MATH_INLINE struct Mat4 makeMat4ScaleUniform(const float S) {
  return (struct Mat4){S, 0, 0, 0, 0, S, 0, 0, 0, 0, S, 0, 0, 0, 0, 1};
}

GRAPHICS_MATH_INLINE struct Mat4 mulMat4ScaleUniform(const struct Mat4 M,
                                                     const float S) {
  return mat4Mul(M, makeMat4ScaleUniform(S));
}

GRAPHICS_MATH_INLINE struct Mat4 makeMat4Scale(const struct Vec3 S) {
  return (struct Mat4){S.X, 0, 0, 0, 0, S.Y, 0, 0, 0, 0, S.Z, 0, 0, 0, 0, 1};
}

GRAPHICS_MATH_INLINE struct Mat4 mulMat4Scale(const struct Mat4 M,
                                              const struct Vec3 S) {
  return mat4Mul(M, makeMat4Scale(S));
}

GRAPHICS_MATH_INLINE struct Mat4 makeMat4Rotation(const struct Vec3 Axis,
                                                  const float Angle) {
  const float c = cosf(Angle), s = sinf(Angle), t = 1.0f - c;
  const float x = Axis.X;
  const float y = Axis.Y;
  const float z = Axis.Z;
  return (struct Mat4){t * x * x + c,
                       t * x * y + s * z,
                       t * x * z - s * y,
                       0,
                       t * x * y - s * z,
                       t * y * y + c,
                       t * y * z + s * x,
                       0,
                       t * x * z + s * y,
                       t * y * z - s * x,
                       t * z * z + c,
                       0,
                       0,
                       0,
                       0,
                       1};
}

GRAPHICS_MATH_INLINE struct Mat4 mulMat4Rotation(const struct Mat4 M,
                                                 const struct Vec3 Axis,
                                                 const float Angle) {
  return mat4Mul(M, makeMat4Rotation(Axis, Angle));
}

GRAPHICS_MATH_INLINE struct Mat4 makeMat4RotationX(const float Angle) {
  const float c = cosf(Angle);
  const float s = sinf(Angle);
  return (struct Mat4){1, 0, 0, 0, 0, c, s, 0, 0, -s, c, 0, 0, 0, 0, 1};
}

GRAPHICS_MATH_INLINE struct Mat4 mulMat4RotationX(const struct Mat4 M,
                                                  const float Angle) {
  return mat4Mul(M, makeMat4RotationX(Angle));
}

GRAPHICS_MATH_INLINE struct Mat4 makeMat4RotationY(const float Angle) {
  const float c = cosf(Angle);
  const float s = sinf(Angle);
  return (struct Mat4){c, 0, s, 0, 0, 1, 0, 0, -s, 0, c, 0, 0, 0, 0, 1};
}

GRAPHICS_MATH_INLINE struct Mat4 mulMat4RotationY(const struct Mat4 M,
                                                  const float Angle) {
  return mat4Mul(M, makeMat4RotationY(Angle));
}

GRAPHICS_MATH_INLINE struct Mat4 makeMat4RotationZ(const float Angle) {
  const float c = cosf(Angle);
  const float s = sinf(Angle);
  return (struct Mat4){c, s, 0, 0, -s, c, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
}

GRAPHICS_MATH_INLINE struct Mat4 mulMat4RotationZ(const struct Mat4 M,
                                                  const float Angle) {
  return mat4Mul(M, makeMat4RotationZ(Angle));
}

GRAPHICS_MATH_INLINE struct Mat4 makeMat4Translation(const struct Vec3 T) {
  return (struct Mat4){1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, T.X, T.Y, T.Z, 1};
}

GRAPHICS_MATH_INLINE struct Mat4 mulMat4Translation(const struct Mat4 M,
                                                    const struct Vec3 T) {
  return mat4Mul(M, makeMat4Translation(T));
}

GRAPHICS_MATH_INLINE struct Mat4 makeMat4LookAt(const struct Vec3 Eye,
                                                const struct Vec3 Center,
                                                const struct Vec3 Up) {
  const struct Vec3 f = vec3Normalize(vec3Sub(Center, Eye));
  const struct Vec3 r = vec3Normalize(vec3Cross(f, Up));
  const struct Vec3 u = vec3Cross(r, f);
  return (struct Mat4){r.X,
                       u.X,
                       -f.X,
                       0,
                       r.Y,
                       u.Y,
                       -f.Y,
                       0,
                       r.Z,
                       u.Z,
                       -f.Z,
                       0,
                       -vec3Dot(r, Eye),
                       -vec3Dot(u, Eye),
                       vec3Dot(f, Eye),
                       1};
}

#endif // SOFTWARE_MODEL_VISUALIZER_GRAPHICS_MATH_H
