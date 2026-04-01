#include "rendering.h"

#include "cglm/mat4.h"

static inline float edgeFunction(const vec2 A, const vec2 B, const vec2 C) {
  return (A[0] - B[0]) * (C[1] - A[1]) - (A[1] - B[1]) * (C[0] - A[0]);
}

static inline void ndcToScreenNormalized(const vec3 Ndc, vec3 Normalized) {
  Normalized[0] = (Ndc[0] + 1.0f) * 0.5f;
  Normalized[1] = (1.0f - Ndc[1]) * 0.5f;
  Normalized[2] = (Ndc[2] + 1.0f) * 0.5f;
}

void clearColorBuffer(const Framebuffer* const Framebuffer,
                      const uint32_t ClearValue) {
  uint32_t* pixel = Framebuffer->ColorBuffer;
  const uint32_t* const end =
      Framebuffer->ColorBuffer + Framebuffer->Width * Framebuffer->Height;

  while (pixel < end) {
    *pixel = ClearValue;
    ++pixel;
  }
}

void clearDepthBuffer(const Framebuffer* const Framebuffer,
                      const float ClearValue) {
  float* pixel = Framebuffer->DepthBuffer;
  const float* const end =
      Framebuffer->DepthBuffer + Framebuffer->Width * Framebuffer->Height;
  while (pixel < end) {
    *pixel = ClearValue;
    ++pixel;
  }
}

void drawPixel(const Framebuffer* const Framebuffer, const uint32_t X,
               const uint32_t Y, const uint32_t Color, const float Depth) {
  if (X >= Framebuffer->Width || Y >= Framebuffer->Height) {
    return;
  }
  const size_t pixelIndex = Y * Framebuffer->Width + X;
  Framebuffer->ColorBuffer[pixelIndex] = Color;
  Framebuffer->DepthBuffer[pixelIndex] = Depth;
}

void drawTriangle(const Framebuffer* Framebuffer, vec3 V0, vec3 V1, vec3 V2,
                  const uint32_t Color) {
  const vec2 pixelV0 = {(V0[0] * Framebuffer->Width),
                        (V0[1] * Framebuffer->Height)};
  const vec2 pixelV1 = {(V1[0] * Framebuffer->Width),
                        (V1[1] * Framebuffer->Height)};
  const vec2 pixelV2 = {(V2[0] * Framebuffer->Width),
                        (V2[1] * Framebuffer->Height)};

  const float area = edgeFunction(pixelV0, pixelV1, pixelV2);

  if (area <= 0.0f) {
    return;
  }

  const float areaInv = 1.0f / area;

  const float minXF = fminf(pixelV0[0], fminf(pixelV1[0], pixelV2[0]));
  const float minYF = fminf(pixelV0[1], fminf(pixelV1[1], pixelV2[1]));
  const float maxXF = fmaxf(pixelV0[0], fmaxf(pixelV1[0], pixelV2[0]));
  const float maxYF = fmaxf(pixelV0[1], fmaxf(pixelV1[1], pixelV2[1]));

  const int32_t minX = max(0, (int32_t)floorf(minXF));
  const int32_t minY = max(0, (int32_t)floorf(minYF));
  const int32_t maxX = min((int32_t)Framebuffer->Width, (int32_t)ceilf(maxXF));
  const int32_t maxY = min((int32_t)Framebuffer->Height, (int32_t)ceilf(maxYF));

  const float stepXW0 = pixelV2[1] - pixelV1[1];
  const float stepYW0 = pixelV1[0] - pixelV2[0];

  const float stepXW1 = pixelV0[1] - pixelV2[1];
  const float stepYW1 = pixelV2[0] - pixelV0[0];

  const float stepXW2 = pixelV1[1] - pixelV0[1];
  const float stepYW2 = pixelV0[0] - pixelV1[0];

  const vec2 initialPixel = {(float)minX, (float)minY};
  float w0Row = edgeFunction(pixelV1, pixelV2, initialPixel);
  float w1Row = edgeFunction(pixelV2, pixelV0, initialPixel);
  float w2Row = edgeFunction(pixelV0, pixelV1, initialPixel);

  for (int32_t y = minY; y < maxY; ++y) {
    float w0 = w0Row;
    float w1 = w1Row;
    float w2 = w2Row;

    for (int32_t x = minX; x < maxX; ++x) {
      if (w0 > 0.0f && w1 > 0.0f && w2 > 0.0f) {
        const float b0 = w0 * areaInv;
        const float b1 = w1 * areaInv;
        const float b2 = w2 * areaInv;

        const float depth = b0 * V0[2] + b1 * V1[2] + b2 * V2[2];
        const size_t pixelIndex = y * Framebuffer->Width + x;

        if (depth < Framebuffer->DepthBuffer[pixelIndex]) {
          Framebuffer->DepthBuffer[pixelIndex] = depth;
          Framebuffer->ColorBuffer[pixelIndex] = Color;
        }
      }
      w0 += stepXW0;
      w1 += stepXW1;
      w2 += stepXW2;
    }

    w0Row += stepYW0;
    w1Row += stepYW1;
    w2Row += stepYW2;
  }
}

void drawModel(const Framebuffer* Framebuffer, const Model* Model,
               mat4 Transform) {
  for (size_t i = 0; i < Model->IndexCount; i += 3) {
    const uint32_t index0 = Model->Indices[i];
    const uint32_t index1 = Model->Indices[i + 1];
    const uint32_t index2 = Model->Indices[i + 2];
    vec4 obj0 = {Model->Vertices[index0][0], Model->Vertices[index0][1],
                 Model->Vertices[index0][2], 1.0f};
    vec4 obj1 = {Model->Vertices[index1][0], Model->Vertices[index1][1],
                 Model->Vertices[index1][2], 1.0f};
    vec4 obj2 = {Model->Vertices[index2][0], Model->Vertices[index2][1],
                 Model->Vertices[index2][2], 1.0f};

    vec4 clip0, clip1, clip2;

    glm_mat4_mulv(Transform, obj0, clip0);
    glm_mat4_mulv(Transform, obj1, clip1);
    glm_mat4_mulv(Transform, obj2, clip2);

    vec3 ndc0, ndc1, ndc2;
    glm_vec3_scale(clip0, 1.0f / clip0[3], ndc0);
    glm_vec3_scale(clip1, 1.0f / clip1[3], ndc1);
    glm_vec3_scale(clip2, 1.0f / clip2[3], ndc2);

    vec3 norm0, norm1, norm2;
    ndcToScreenNormalized(ndc0, norm0);
    ndcToScreenNormalized(ndc1, norm1);
    ndcToScreenNormalized(ndc2, norm2);

    drawTriangle(Framebuffer, norm0, norm1, norm2, Model->Colors[i / 3]);
  }
}
