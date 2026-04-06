#include "graphicsMath.h"
#include "testFramework.h"

static constexpr float epsilon = 1e-5f;

TEST(test_vec3_add) {
  vec3 r = vec3Add(MAKE_VEC3(1, 2, 3), MAKE_VEC3(4, 5, 6));
  ASSERT_CLOSE_FLOAT(r.X, 5.0f, epsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 7.0f, epsilon);
  ASSERT_CLOSE_FLOAT(r.Z, 9.0f, epsilon);
}

TEST(test_vec3_dot) {
  float d = vec3Dot(MAKE_VEC3(1, 0, 0), MAKE_VEC3(0, 1, 0));
  ASSERT_CLOSE_FLOAT(d, 0.0f, epsilon);

  d = vec3Dot(MAKE_VEC3(1, 0, 0), MAKE_VEC3(1, 0, 0));
  ASSERT_CLOSE_FLOAT(d, 1.0f, epsilon);
}

TEST(test_vec3_cross) {
  vec3 r = vec3Cross(MAKE_VEC3(1, 0, 0), MAKE_VEC3(0, 1, 0));
  ASSERT_CLOSE_FLOAT(r.X, 0.0f, epsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 0.0f, epsilon);
  ASSERT_CLOSE_FLOAT(r.Z, 1.0f, epsilon);
}

TEST(test_vec3_normalize) {
  vec3 r = vec3Normalize(MAKE_VEC3(3, 0, 0));
  ASSERT_CLOSE_FLOAT(vec3Len(r), 1.0f, epsilon);
}

TEST(test_mat4_identity_mul) {
  vec4 v = MAKE_VEC4(1, 2, 3, 1);
  vec4 r = mat4MulVec4(MAT4_IDENTITY, v);
  ASSERT_CLOSE_FLOAT(r.X, 1.0f, epsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 3.0f, epsilon);
  ASSERT_CLOSE_FLOAT(r.Z, 3.0f, epsilon);
  ASSERT_CLOSE_FLOAT(r.W, 1.0f, epsilon);
}

TEST(test_mat4_translation) {
  mat4 t = makeMat4Translation(MAKE_VEC3(1, 2, 3));
  vec4 r = mat4MulVec4(t, MAKE_VEC4(0, 0, 0, 1));
  ASSERT_CLOSE_FLOAT(r.X, 1.0f, epsilon);
  ASSERT_CLOSE_FLOAT(r.Y, 2.0f, epsilon);
  ASSERT_CLOSE_FLOAT(r.Z, 3.0f, epsilon);
}
