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

typedef union GRAPHICS_MATH_ALIGNAS(8) vec2 {
  struct {
    float X;
    float Y;
  };
  struct {
    float U;
    float V;
  };
  struct {
    float S;
    float T;
  };
  float ValuePtr[2];
} vec2;

typedef GRAPHICS_MATH_ALIGNAS(4) union vec3 {
  struct {
    float X;
    float Y;
    float Z;
  };
  struct {
    float R;
    float G;
    float B;
  };
  float ValuePtr[3];
} vec3;

typedef GRAPHICS_MATH_ALIGNAS(16) union vec4 {
  struct {
    float X;
    float Y;
    float Z;
    float W;
  };
  struct {
    float R;
    float G;
    float B;
    float A;
  };
  float ValuePtr[4];
} vec4;

typedef GRAPHICS_MATH_ALIGNAS(16) union mat4 {
  float ValuePtr[16];
  float Value[4][4];
  vec4 Columns[4];
} mat4;

/*******************************************************************************
 * Constructor macros
 ******************************************************************************/

#define MAKE_VEC2(x, y) ((vec2){{(x), (y)}})
#define MAKE_VEC3(x, y, z) ((vec3){{(x), (y), (z)}})
#define MAKE_VEC4(x, y, z, w) ((vec4){{(x), (y), (z), (w)}})
#define MAKE_MAT4(c0, c1, c2, c3) ((mat4){.Columns = {(c0), (c1), (c2), (c3)}})

/*******************************************************************************
 * Common values
 ******************************************************************************/

#define VEC2_ZERO ((vec2){{0.0f, 0.0f}})
#define VEC2_ONE ((vec2){{1.0f, 1.0f}})
#define VEC2_UP ((vec2){{0.0f, 1.0f}})
#define VEC2_RIGHT ((vec2){{1.0f, 0.0f}})

#define VEC3_ZERO ((vec3){{0.0f, 0.0f, 0.0f}})
#define VEC3_ONE ((vec3){{1.0f, 1.0f, 1.0f}})
#define VEC3_UP ((vec3){{0.0f, 1.0f, 0.0f}})
#define VEC3_RIGHT ((vec3){{1.0f, 0.0f, 0.0f}})
#define VEC3_FORWARD ((vec3){{0.0f, 0.0f, -1.0f}})

#define VEC4_ZERO ((vec4){{0.0f, 0.0f, 0.0f, 0.0f}})
#define VEC4_ONE ((vec4){{1.0f, 1.0f, 1.0f, 1.0f}})

#define MAT4_ZERO                                                              \
  ((mat4){.ValuePtr = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,   \
                       0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}})
#define MAT4_IDENTITY                                                          \
  ((mat4){.ValuePtr = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,   \
                       0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}})

/*******************************************************************************
 * Vec2 operations
 ******************************************************************************/

GRAPHICS_MATH_INLINE void vec2Copy(const vec2 Src, vec2* Dst) { *Dst = Src; }

GRAPHICS_MATH_INLINE vec2 vec2Add(const vec2 A, const vec2 B) {
  return (vec2){{A.X + B.X, A.Y + B.Y}};
}

GRAPHICS_MATH_INLINE vec2 vec2Sub(const vec2 A, const vec2 B) {
  return (vec2){{A.X - B.X, A.Y - B.Y}};
}

GRAPHICS_MATH_INLINE vec2 vec2Mul(const vec2 A, const vec2 B) {
  return (vec2){{A.X * B.X, A.Y * B.Y}};
}

GRAPHICS_MATH_INLINE vec2 vec2Scale(const vec2 A, const float Scalar) {
  return (vec2){{A.X * Scalar, A.Y * Scalar}};
}

GRAPHICS_MATH_INLINE vec2 vec2Div(const vec2 A, const float Scalar) {
  const float inverse = 1.0f / Scalar;
  return (vec2){{A.X * inverse, A.Y * inverse}};
}

GRAPHICS_MATH_INLINE float vec2Dot(const vec2 A, const vec2 B) {
  return A.X * B.X + A.Y * B.Y;
}

GRAPHICS_MATH_INLINE float vec2LenSq(const vec2 A) { return vec2Dot(A, A); }

GRAPHICS_MATH_INLINE float vec2Len(const vec2 A) { return sqrtf(vec2LenSq(A)); }

GRAPHICS_MATH_INLINE vec2 vec2Normalize(const vec2 A) {
  return vec2Div(A, vec2Len(A));
}

GRAPHICS_MATH_INLINE float vec2Dist(const vec2 A, const vec2 B) {
  return vec2Len(vec2Sub(A, B));
}

/*******************************************************************************
 * Vec3 operations
 ******************************************************************************/

GRAPHICS_MATH_INLINE void vec3Copy(const vec3 Src, vec3* Dst) { *Dst = Src; }

GRAPHICS_MATH_INLINE vec3 vec3Add(const vec3 A, const vec3 B) {
  return (vec3){{A.X + B.X, A.Y + B.Y, A.Z + B.Z}};
}

GRAPHICS_MATH_INLINE vec3 vec3Sub(const vec3 A, const vec3 B) {
  return (vec3){{A.X - B.X, A.Y - B.Y, A.Z - B.Z}};
}

GRAPHICS_MATH_INLINE vec3 vec3Mul(const vec3 A, const vec3 B) {
  return (vec3){{A.X * B.X, A.Y * B.Y, A.Z * B.Z}};
}

GRAPHICS_MATH_INLINE vec3 vec3Scale(const vec3 A, const float Scalar) {
  return (vec3){{A.X * Scalar, A.Y * Scalar, A.Z * Scalar}};
}

GRAPHICS_MATH_INLINE vec3 vec3Div(const vec3 A, const float Scalar) {
  const float inverse = 1.0f / Scalar;
  return (vec3){{A.X * inverse, A.Y * inverse, A.Z * inverse}};
}

GRAPHICS_MATH_INLINE float vec3Dot(const vec3 A, const vec3 B) {
  return A.X * B.X + A.Y * B.Y + A.Z * B.Z;
}

GRAPHICS_MATH_INLINE float vec3LenSq(const vec3 A) { return vec3Dot(A, A); }

GRAPHICS_MATH_INLINE float vec3Len(const vec3 A) { return sqrtf(vec3LenSq(A)); }

GRAPHICS_MATH_INLINE vec3 vec3Normalize(const vec3 A) {
  return vec3Div(A, vec3Len(A));
}

GRAPHICS_MATH_INLINE float vec3Dist(const vec3 A, const vec3 B) {
  return vec3Len(vec3Sub(A, B));
}

GRAPHICS_MATH_INLINE vec3 vec3Cross(const vec3 A, const vec3 B) {
  return (vec3){
      {A.Y * B.Z - A.Z * B.Y, A.Z * B.X - A.X * B.Z, A.X * B.Y - A.Y * B.X}};
}

/*******************************************************************************
 * Vec4 operations
 ******************************************************************************/

GRAPHICS_MATH_INLINE void vec4Copy(const vec4 Src, vec4* Dst) { *Dst = Src; }

GRAPHICS_MATH_INLINE vec4 vec4Add(const vec4 A, const vec4 B) {
  return (vec4){{A.X + B.X, A.Y + B.Y, A.Z + B.Z, A.W + B.W}};
}

GRAPHICS_MATH_INLINE vec4 vec4Sub(const vec4 A, const vec4 B) {
  return (vec4){{A.X - B.X, A.Y - B.Y, A.Z - B.Z, A.W - B.W}};
}

GRAPHICS_MATH_INLINE vec4 vec4Mul(const vec4 A, const vec4 B) {
  return (vec4){{A.X * B.X, A.Y * B.Y, A.Z * B.Z, A.W * B.W}};
}

GRAPHICS_MATH_INLINE vec4 vec4Scale(const vec4 A, const float Scalar) {
  return (vec4){{A.X * Scalar, A.Y * Scalar, A.Z * Scalar, A.W * Scalar}};
}

GRAPHICS_MATH_INLINE vec4 vec4Div(const vec4 A, const float Scalar) {
  const float inverse = 1.0f / Scalar;
  return (vec4){{A.X * inverse, A.Y * inverse, A.Z * inverse, A.W * inverse}};
}

GRAPHICS_MATH_INLINE float vec4Dot(const vec4 A, const vec4 B) {
  return A.X * B.X + A.Y * B.Y + A.Z * B.Z + A.W * B.W;
}

GRAPHICS_MATH_INLINE float vec4LenSq(const vec4 A) { return vec4Dot(A, A); }

GRAPHICS_MATH_INLINE float vec4Len(const vec4 A) { return sqrtf(vec4LenSq(A)); }

GRAPHICS_MATH_INLINE vec4 vec4Normalize(const vec4 A) {
  return vec4Div(A, vec4Len(A));
}

GRAPHICS_MATH_INLINE float vec4Dist(const vec4 A, const vec4 B) {
  return vec4Len(vec4Sub(A, B));
}

/*******************************************************************************
 * Mat4 operations
 ******************************************************************************/

GRAPHICS_MATH_INLINE void mat4Copy(const mat4 Src, mat4* Dst) { *Dst = Src; }

GRAPHICS_MATH_INLINE mat4 mat4Add(const mat4 A, const mat4 B) {
  return (mat4){
      .ValuePtr = {
          A.ValuePtr[0] + B.ValuePtr[0], A.ValuePtr[1] + B.ValuePtr[1],
          A.ValuePtr[2] + B.ValuePtr[2], A.ValuePtr[3] + B.ValuePtr[3],
          A.ValuePtr[4] + B.ValuePtr[4], A.ValuePtr[5] + B.ValuePtr[5],
          A.ValuePtr[6] + B.ValuePtr[6], A.ValuePtr[7] + B.ValuePtr[7],
          A.ValuePtr[8] + B.ValuePtr[8], A.ValuePtr[9] + B.ValuePtr[9],
          A.ValuePtr[10] + B.ValuePtr[10], A.ValuePtr[11] + B.ValuePtr[11],
          A.ValuePtr[12] + B.ValuePtr[12], A.ValuePtr[13] + B.ValuePtr[13],
          A.ValuePtr[14] + B.ValuePtr[14], A.ValuePtr[15] + B.ValuePtr[15]}};
}

GRAPHICS_MATH_INLINE mat4 mat4Sub(const mat4 A, const mat4 B) {
  return (mat4){
      .ValuePtr = {
          A.ValuePtr[0] - B.ValuePtr[0], A.ValuePtr[1] - B.ValuePtr[1],
          A.ValuePtr[2] - B.ValuePtr[2], A.ValuePtr[3] - B.ValuePtr[3],
          A.ValuePtr[4] - B.ValuePtr[4], A.ValuePtr[5] - B.ValuePtr[5],
          A.ValuePtr[6] - B.ValuePtr[6], A.ValuePtr[7] - B.ValuePtr[7],
          A.ValuePtr[8] - B.ValuePtr[8], A.ValuePtr[9] - B.ValuePtr[9],
          A.ValuePtr[10] - B.ValuePtr[10], A.ValuePtr[11] - B.ValuePtr[11],
          A.ValuePtr[12] - B.ValuePtr[12], A.ValuePtr[13] - B.ValuePtr[13],
          A.ValuePtr[14] - B.ValuePtr[14], A.ValuePtr[15] - B.ValuePtr[15]}};
}

GRAPHICS_MATH_INLINE mat4 mat4Scale(const mat4 A, const float Scalar) {
  return (mat4){.ValuePtr = {A.ValuePtr[0] * Scalar, A.ValuePtr[1] * Scalar,
                             A.ValuePtr[2] * Scalar, A.ValuePtr[3] * Scalar,
                             A.ValuePtr[4] * Scalar, A.ValuePtr[5] * Scalar,
                             A.ValuePtr[6] * Scalar, A.ValuePtr[7] * Scalar,
                             A.ValuePtr[8] * Scalar, A.ValuePtr[9] * Scalar,
                             A.ValuePtr[10] * Scalar, A.ValuePtr[11] * Scalar,
                             A.ValuePtr[12] * Scalar, A.ValuePtr[13] * Scalar,
                             A.ValuePtr[14] * Scalar, A.ValuePtr[15] * Scalar}};
}

GRAPHICS_MATH_INLINE mat4 mat4Div(const mat4 A, const float Scalar) {
  const float inverse = 1.0f / Scalar;
  return (mat4){.ValuePtr = {A.ValuePtr[0] * inverse, A.ValuePtr[1] * inverse,
                             A.ValuePtr[2] * inverse, A.ValuePtr[3] * inverse,
                             A.ValuePtr[4] * inverse, A.ValuePtr[5] * inverse,
                             A.ValuePtr[6] * inverse, A.ValuePtr[7] * inverse,
                             A.ValuePtr[8] * inverse, A.ValuePtr[9] * inverse,
                             A.ValuePtr[10] * inverse, A.ValuePtr[11] * inverse,
                             A.ValuePtr[12] * inverse, A.ValuePtr[13] * inverse,
                             A.ValuePtr[14] * inverse,
                             A.ValuePtr[15] * inverse}};
}

GRAPHICS_MATH_INLINE mat4 mat4Mul(const mat4 A, const mat4 B) {
  return (mat4){
      .Value = {
          {A.Value[0][0] * B.Value[0][0] + A.Value[1][0] * B.Value[0][1] +
               A.Value[2][0] * B.Value[0][2] + A.Value[3][0] * B.Value[0][3],
           A.Value[0][1] * B.Value[0][0] + A.Value[1][1] * B.Value[0][1] +
               A.Value[2][1] * B.Value[0][2] + A.Value[3][1] * B.Value[0][3],
           A.Value[0][2] * B.Value[0][0] + A.Value[1][2] * B.Value[0][1] +
               A.Value[2][2] * B.Value[0][2] + A.Value[3][2] * B.Value[0][3],
           A.Value[0][3] * B.Value[0][0] + A.Value[1][3] * B.Value[0][1] +
               A.Value[2][3] * B.Value[0][2] + A.Value[3][3] * B.Value[0][3]},

          {A.Value[0][0] * B.Value[1][0] + A.Value[1][0] * B.Value[1][1] +
               A.Value[2][0] * B.Value[1][2] + A.Value[3][0] * B.Value[1][3],
           A.Value[0][1] * B.Value[1][0] + A.Value[1][1] * B.Value[1][1] +
               A.Value[2][1] * B.Value[1][2] + A.Value[3][1] * B.Value[1][3],
           A.Value[0][2] * B.Value[1][0] + A.Value[1][2] * B.Value[1][1] +
               A.Value[2][2] * B.Value[1][2] + A.Value[3][2] * B.Value[1][3],
           A.Value[0][3] * B.Value[1][0] + A.Value[1][3] * B.Value[1][1] +
               A.Value[2][3] * B.Value[1][2] + A.Value[3][3] * B.Value[1][3]},

          {A.Value[0][0] * B.Value[2][0] + A.Value[1][0] * B.Value[2][1] +
               A.Value[2][0] * B.Value[2][2] + A.Value[3][0] * B.Value[2][3],
           A.Value[0][1] * B.Value[2][0] + A.Value[1][1] * B.Value[2][1] +
               A.Value[2][1] * B.Value[2][2] + A.Value[3][1] * B.Value[2][3],
           A.Value[0][2] * B.Value[2][0] + A.Value[1][2] * B.Value[2][1] +
               A.Value[2][2] * B.Value[2][2] + A.Value[3][2] * B.Value[2][3],
           A.Value[0][3] * B.Value[2][0] + A.Value[1][3] * B.Value[2][1] +
               A.Value[2][3] * B.Value[2][2] + A.Value[3][3] * B.Value[2][3]},

          {A.Value[0][0] * B.Value[3][0] + A.Value[1][0] * B.Value[3][1] +
               A.Value[2][0] * B.Value[3][2] + A.Value[3][0] * B.Value[3][3],
           A.Value[0][1] * B.Value[3][0] + A.Value[1][1] * B.Value[3][1] +
               A.Value[2][1] * B.Value[3][2] + A.Value[3][1] * B.Value[3][3],
           A.Value[0][2] * B.Value[3][0] + A.Value[1][2] * B.Value[3][1] +
               A.Value[2][2] * B.Value[3][2] + A.Value[3][2] * B.Value[3][3],
           A.Value[0][3] * B.Value[3][0] + A.Value[1][3] * B.Value[3][1] +
               A.Value[2][3] * B.Value[3][2] + A.Value[3][3] * B.Value[3][3]},
      }};
}

GRAPHICS_MATH_INLINE vec4 mat4MulVec4(const mat4 M, const vec4 V) {
  return (vec4){{
      M.Value[0][0] * V.X + M.Value[1][0] * V.Y + M.Value[2][0] * V.Z +
          M.Value[3][0] * V.W,
      M.Value[0][1] * V.X + M.Value[1][1] * V.Y + M.Value[2][1] * V.Z +
          M.Value[3][1] * V.W,
      M.Value[0][2] * V.X + M.Value[1][2] * V.Y + M.Value[2][2] * V.Z +
          M.Value[3][2] * V.W,
      M.Value[0][3] * V.X + M.Value[1][3] * V.Y + M.Value[2][3] * V.Z +
          M.Value[3][3] * V.W,
  }};
}

GRAPHICS_MATH_INLINE mat4 mat4Inverse(const mat4 M) {
  const float a0 =
      M.Value[0][0] * M.Value[1][1] - M.Value[0][1] * M.Value[1][0];
  const float a1 =
      M.Value[0][0] * M.Value[1][2] - M.Value[0][2] * M.Value[1][0];
  const float a2 =
      M.Value[0][0] * M.Value[1][3] - M.Value[0][3] * M.Value[1][0];
  const float a3 =
      M.Value[0][1] * M.Value[1][2] - M.Value[0][2] * M.Value[1][1];
  const float a4 =
      M.Value[0][1] * M.Value[1][3] - M.Value[0][3] * M.Value[1][1];
  const float a5 =
      M.Value[0][2] * M.Value[1][3] - M.Value[0][3] * M.Value[1][2];

  const float b0 =
      M.Value[2][0] * M.Value[3][1] - M.Value[2][1] * M.Value[3][0];
  const float b1 =
      M.Value[2][0] * M.Value[3][2] - M.Value[2][2] * M.Value[3][0];
  const float b2 =
      M.Value[2][0] * M.Value[3][3] - M.Value[2][3] * M.Value[3][0];
  const float b3 =
      M.Value[2][1] * M.Value[3][2] - M.Value[2][2] * M.Value[3][1];
  const float b4 =
      M.Value[2][1] * M.Value[3][3] - M.Value[2][3] * M.Value[3][1];
  const float b5 =
      M.Value[2][2] * M.Value[3][3] - M.Value[2][3] * M.Value[3][2];

  const float det = a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;
  if (GRAPHICS_MATH_UNLIKELY(det > -1e-6f && det < 1e-6f))
    return MAT4_ZERO;
  const float invDet = 1.0f / det;

  return (mat4){
      .ValuePtr = {
          (M.Value[1][1] * b5 - M.Value[1][2] * b4 + M.Value[1][3] * b3) *
              invDet,
          (-M.Value[0][1] * b5 + M.Value[0][2] * b4 - M.Value[0][3] * b3) *
              invDet,
          (M.Value[3][1] * a5 - M.Value[3][2] * a4 + M.Value[3][3] * a3) *
              invDet,
          (-M.Value[2][1] * a5 + M.Value[2][2] * a4 - M.Value[2][3] * a3) *
              invDet,

          (-M.Value[1][0] * b5 + M.Value[1][2] * b2 - M.Value[1][3] * b1) *
              invDet,
          (M.Value[0][0] * b5 - M.Value[0][2] * b2 + M.Value[0][3] * b1) *
              invDet,
          (-M.Value[3][0] * a5 + M.Value[3][2] * a2 - M.Value[3][3] * a1) *
              invDet,
          (M.Value[2][0] * a5 - M.Value[2][2] * a2 + M.Value[2][3] * a1) *
              invDet,

          (M.Value[1][0] * b4 - M.Value[1][1] * b2 + M.Value[1][3] * b0) *
              invDet,
          (-M.Value[0][0] * b4 + M.Value[0][1] * b2 - M.Value[0][3] * b0) *
              invDet,
          (M.Value[3][0] * a4 - M.Value[3][1] * a2 + M.Value[3][3] * a0) *
              invDet,
          (-M.Value[2][0] * a4 + M.Value[2][1] * a2 - M.Value[2][3] * a0) *
              invDet,

          (-M.Value[1][0] * b3 + M.Value[1][1] * b1 - M.Value[1][2] * b0) *
              invDet,
          (M.Value[0][0] * b3 - M.Value[0][1] * b1 + M.Value[0][2] * b0) *
              invDet,
          (-M.Value[3][0] * a3 + M.Value[3][1] * a1 - M.Value[3][2] * a0) *
              invDet,
          (M.Value[2][0] * a3 - M.Value[2][1] * a1 + M.Value[2][2] * a0) *
              invDet,
      }};
}

GRAPHICS_MATH_INLINE mat4 mat4AffineInverse(const mat4 M) {
  const float x =
      1.0f / (M.Value[0][0] * M.Value[0][0] + M.Value[0][1] * M.Value[0][1] +
              M.Value[0][2] * M.Value[0][2]);
  const float y =
      1.0f / (M.Value[1][0] * M.Value[1][0] + M.Value[1][1] * M.Value[1][1] +
              M.Value[1][2] * M.Value[1][2]);
  const float z =
      1.0f / (M.Value[2][0] * M.Value[2][0] + M.Value[2][1] * M.Value[2][1] +
              M.Value[2][2] * M.Value[2][2]);
  const float r00 = M.Value[0][0] * x, r10 = M.Value[0][1] * x,
              r20 = M.Value[0][2] * x;
  const float r01 = M.Value[1][0] * y, r11 = M.Value[1][1] * y,
              r21 = M.Value[1][2] * y;
  const float r02 = M.Value[2][0] * z, r12 = M.Value[2][1] * z,
              r22 = M.Value[2][2] * z;
  return (mat4){.Value = {
                    {r00, r10, r20, 0.0f},
                    {r01, r11, r21, 0.0f},
                    {r02, r12, r22, 0.0f},
                    {
                        -(M.Value[3][0] * r00 + M.Value[3][1] * r10 +
                          M.Value[3][2] * r20),
                        -(M.Value[3][0] * r01 + M.Value[3][1] * r11 +
                          M.Value[3][2] * r21),
                        -(M.Value[3][0] * r02 + M.Value[3][1] * r12 +
                          M.Value[3][2] * r22),
                        1.0f,
                    },
                }};
}

GRAPHICS_MATH_INLINE mat4 mat4Transpose(const mat4 M) {
  return (mat4){
      .Value = {
          {M.Value[0][0], M.Value[1][0], M.Value[2][0], M.Value[3][0]},
          {M.Value[0][1], M.Value[1][1], M.Value[2][1], M.Value[3][1]},
          {M.Value[0][2], M.Value[1][2], M.Value[2][2], M.Value[3][2]},
          {M.Value[0][3], M.Value[1][3], M.Value[2][3], M.Value[3][3]},
      }};
}

/*******************************************************************************
 * Transformations
 ******************************************************************************/

GRAPHICS_MATH_INLINE mat4 makePerspectiveProjectionMatrix(const float FovY,
                                                          const float Aspect,
                                                          const float Near,
                                                          const float Far) {
  const float tanInverse = 1.0f / tanf(FovY * 0.5f);
  const float inverseNearFar = 1.0f / (Near - Far);
  return (mat4){.ValuePtr = {
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
                }};
}

GRAPHICS_MATH_INLINE mat4 makeOrthographicProjectionMatrix(
    const float Left, const float Right, const float Bottom, const float Top,
    const float Near, const float Far) {
  const float inverseRightLeft = 1.0f / (Right - Left);
  const float inverseTopBottom = 1.0f / (Top - Bottom);
  const float inverseNearFar = 1.0f / (Near - Far);
  return (mat4){.ValuePtr = {
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
                }};
}

GRAPHICS_MATH_INLINE mat4 makeMat4ScaleUniform(const float S) {
  return (mat4){
      .Value = {{S, 0, 0, 0}, {0, S, 0, 0}, {0, 0, S, 0}, {0, 0, 0, 1}}};
}

GRAPHICS_MATH_INLINE mat4 mulMat4ScaleUniform(const mat4 M, const float S) {
  return mat4Mul(M, makeMat4ScaleUniform(S));
}

GRAPHICS_MATH_INLINE mat4 makeMat4Scale(const vec3 S) {
  return (mat4){
      .Value = {{S.X, 0, 0, 0}, {0, S.Y, 0, 0}, {0, 0, S.Z, 0}, {0, 0, 0, 1}}};
}

GRAPHICS_MATH_INLINE mat4 mulMat4Scale(const mat4 M, const vec3 S) {
  return mat4Mul(M, makeMat4Scale(S));
}

GRAPHICS_MATH_INLINE mat4 makeMat4Rotation(const vec3 Axis, const float Angle) {
  const float c = cosf(Angle), s = sinf(Angle), t = 1.0f - c;
  const float x = Axis.X;
  const float y = Axis.Y;
  const float z = Axis.Z;
  return (mat4){.Value = {
                    {t * x * x + c, t * x * y + s * z, t * x * z - s * y, 0},
                    {t * x * y - s * z, t * y * y + c, t * y * z + s * x, 0},
                    {t * x * z + s * y, t * y * z - s * x, t * z * z + c, 0},
                    {0, 0, 0, 1},
                }};
}

GRAPHICS_MATH_INLINE mat4 mulMat4Rotation(const mat4 M, const vec3 Axis,
                                          const float Angle) {
  return mat4Mul(M, makeMat4Rotation(Axis, Angle));
}

GRAPHICS_MATH_INLINE mat4 makeMat4RotationX(const float Angle) {
  const float c = cosf(Angle);
  const float s = sinf(Angle);
  return (mat4){
      .Value = {{1, 0, 0, 0}, {0, c, s, 0}, {0, -s, c, 0}, {0, 0, 0, 1}}};
}

GRAPHICS_MATH_INLINE mat4 mulMat4RotationX(const mat4 M, const float Angle) {
  return mat4Mul(M, makeMat4RotationX(Angle));
}

GRAPHICS_MATH_INLINE mat4 makeMat4RotationY(const float Angle) {
  const float c = cosf(Angle);
  const float s = sinf(Angle);
  return (mat4){
      .Value = {{c, 0, s, 0}, {0, 1, 0, 0}, {-s, 0, c, 0}, {0, 0, 0, 1}}};
}

GRAPHICS_MATH_INLINE mat4 mulMat4RotationY(const mat4 M, const float Angle) {
  return mat4Mul(M, makeMat4RotationY(Angle));
}

GRAPHICS_MATH_INLINE mat4 makeMat4RotationZ(const float Angle) {
  const float c = cosf(Angle);
  const float s = sinf(Angle);
  return (mat4){
      .Value = {{c, s, 0, 0}, {-s, c, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}};
}

GRAPHICS_MATH_INLINE mat4 mulMat4RotationZ(const mat4 M, const float Angle) {
  return mat4Mul(M, makeMat4RotationZ(Angle));
}

GRAPHICS_MATH_INLINE mat4 makeMat4Translation(const vec3 T) {
  return (mat4){
      .Value = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {T.X, T.Y, T.Z, 1}}};
}

GRAPHICS_MATH_INLINE mat4 mulMat4Translation(const mat4 M, const vec3 T) {
  return mat4Mul(M, makeMat4Translation(T));
}

GRAPHICS_MATH_INLINE mat4 makeMat4LookAt(const vec3 Eye, const vec3 Center,
                                         const vec3 Up) {
  const vec3 f = vec3Normalize(vec3Sub(Center, Eye));
  const vec3 r = vec3Normalize(vec3Cross(f, Up));
  const vec3 u = vec3Cross(r, f);
  return (mat4){.Value = {
                    {r.X, u.X, -f.X, 0},
                    {r.Y, u.Y, -f.Y, 0},
                    {r.Z, u.Z, -f.Z, 0},
                    {-vec3Dot(r, Eye), -vec3Dot(u, Eye), vec3Dot(f, Eye), 1},
                }};
}

#endif // SOFTWARE_MODEL_VISUALIZER_GRAPHICS_MATH_H
