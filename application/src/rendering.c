#include "rendering.h"

#include <stdlib.h>

static inline uint32_t lerpColor(const uint32_t A, const uint32_t B,
                                 const uint32_t C, const float w0,
                                 const float w1, const float w2) {
  uint8_t r = (uint8_t)((((A >> 16) & 0xFF) * w0) + (((B >> 16) & 0xFF) * w1) +
                        (((C >> 16) & 0xFF) * w2));
  uint8_t g = (uint8_t)(((A >> 8) & 0xFF) * w0 + ((B >> 8) & 0xFF) * w1 +
                        ((C >> 8) & 0xFF) * w2);
  uint8_t b = (uint8_t)((A & 0xFF) * w0 + (B & 0xFF) * w1 + (C & 0xFF) * w2);
  return (uint32_t)r << 16 | (uint32_t)g << 8 | (uint32_t)b;
}

static inline float edgeFunction(const vec2 A, const vec2 B, const vec2 C) {
  return (A.X - B.X) * (C.Y - A.Y) - (A.Y - B.Y) * (C.X - A.X);
}

static inline void ndcToScreenNormalized(const vec3 Ndc, vec3* Normalized) {
  Normalized->X = (Ndc.X + 1.0f) * 0.5f;
  Normalized->Y = (1.0f - Ndc.Y) * 0.5f;
  Normalized->Z = (Ndc.Z + 1.0f) * 0.5f;
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

void drawLine(const Framebuffer* const Framebuffer, vec3 V0, vec3 V1,
              const uint32_t Color) {
  vec3 norm0, norm1;
  ndcToScreenNormalized(V0, &norm0);
  ndcToScreenNormalized(V1, &norm1);

  int32_t x0 = (int32_t)(norm0.X * Framebuffer->Width);
  int32_t y0 = (int32_t)(norm0.Y * Framebuffer->Height);
  const int32_t x1 = (int32_t)(norm1.X * Framebuffer->Width);
  const int32_t y1 = (int32_t)(norm1.Y * Framebuffer->Height);

  const int32_t dx = abs(x1 - x0);
  const int32_t dy = -abs(y1 - y0);
  const int32_t sx = x0 < x1 ? 1 : -1;
  const int32_t sy = y0 < y1 ? 1 : -1;
  int32_t err = dx + dy;

  const int32_t steps = dx > -dy ? dx : -dy;
  const float depthStep = steps > 0 ? (norm1.Z - norm0.Z) / (float)steps : 0.0f;
  float depth = norm0.Z;

  while (true) {
    if ((uint32_t)x0 < Framebuffer->Width &&
        (uint32_t)y0 < Framebuffer->Height) {
      const size_t pixelIndex = (size_t)y0 * Framebuffer->Width + (size_t)x0;
      if (depth < Framebuffer->DepthBuffer[pixelIndex] && depth >= -1.0f &&
          depth <= 1.0f) {
        Framebuffer->DepthBuffer[pixelIndex] = depth;
        Framebuffer->ColorBuffer[pixelIndex] = Color;
      }
    }

    if (x0 == x1 && y0 == y1) {
      break;
    }

    const int32_t e2 = 2 * err;
    if (e2 >= dy) {
      err += dy;
      x0 += sx;
    }
    if (e2 <= dx) {
      err += dx;
      y0 += sy;
    }
    depth += depthStep;
  }
}

void drawTriangle(const Framebuffer* Framebuffer, vec3 V0, vec3 V1, vec3 V2,
                  const uint32_t) {
  vec3 norm0, norm1, norm2;
  ndcToScreenNormalized(V0, &norm0);
  ndcToScreenNormalized(V1, &norm1);
  ndcToScreenNormalized(V2, &norm2);

  const vec2 pixelV0 =
      MAKE_VEC2(norm0.X * Framebuffer->Width, norm0.Y * Framebuffer->Height);
  const vec2 pixelV1 =
      MAKE_VEC2(norm1.X * Framebuffer->Width, norm1.Y * Framebuffer->Height);
  const vec2 pixelV2 =
      MAKE_VEC2(norm2.X * Framebuffer->Width, norm2.Y * Framebuffer->Height);

  const float area = edgeFunction(pixelV0, pixelV1, pixelV2);

  if (area <= 0.0f) {
    return;
  }

  uint32_t color0 = 0x00FF0000;
  uint32_t color1 = 0x0000FF00;
  uint32_t color2 = 0x000000FF;

  const float areaInv = 1.0f / area;

  const float minXF = fminf(pixelV0.X, fminf(pixelV1.X, pixelV2.X));
  const float minYF = fminf(pixelV0.Y, fminf(pixelV1.Y, pixelV2.Y));
  const float maxXF = fmaxf(pixelV0.X, fmaxf(pixelV1.X, pixelV2.X));
  const float maxYF = fmaxf(pixelV0.Y, fmaxf(pixelV1.Y, pixelV2.Y));

  const int32_t minX = max(0, (int32_t)floorf(minXF));
  const int32_t minY = max(0, (int32_t)floorf(minYF));
  const int32_t maxX = min((int32_t)Framebuffer->Width, (int32_t)ceilf(maxXF));
  const int32_t maxY = min((int32_t)Framebuffer->Height, (int32_t)ceilf(maxYF));

  const float stepXW0 = pixelV2.Y - pixelV1.Y;
  const float stepYW0 = pixelV1.X - pixelV2.X;

  const float stepXW1 = pixelV0.Y - pixelV2.Y;
  const float stepYW1 = pixelV2.X - pixelV0.X;

  const float stepXW2 = pixelV1.Y - pixelV0.Y;
  const float stepYW2 = pixelV0.X - pixelV1.X;

  const vec2 initialPixel = MAKE_VEC2((float)minX, (float)minY);
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

        const float depth = b0 * norm0.Z + b1 * norm1.Z + b2 * norm2.Z;
        const size_t pixelIndex = y * Framebuffer->Width + x;

        const bool notCulled = depth < Framebuffer->DepthBuffer[pixelIndex] &&
                               depth >= -1.0f && depth <= 1.0f;

        if (notCulled) {
          Framebuffer->DepthBuffer[pixelIndex] = depth;
          Framebuffer->ColorBuffer[pixelIndex] =
              lerpColor(color0, color1, color2, b0, b1, b2);
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
    vec4 obj0 = MAKE_VEC4(Model->Vertices[index0].X, Model->Vertices[index0].Y,
                          Model->Vertices[index0].Z, 1.0f);
    vec4 obj1 = MAKE_VEC4(Model->Vertices[index1].X, Model->Vertices[index1].Y,
                          Model->Vertices[index1].Z, 1.0f);
    vec4 obj2 = MAKE_VEC4(Model->Vertices[index2].X, Model->Vertices[index2].Y,
                          Model->Vertices[index2].Z, 1.0f);

    vec4 clip0, clip1, clip2;

    clip0 = mat4MulVec4(Transform, obj0);
    clip1 = mat4MulVec4(Transform, obj1);
    clip2 = mat4MulVec4(Transform, obj2);

    vec3 ndc0 =
        MAKE_VEC3(clip0.X / clip0.W, clip0.Y / clip0.W, clip0.Z / clip0.W);
    vec3 ndc1 =
        MAKE_VEC3(clip1.X / clip1.W, clip1.Y / clip1.W, clip1.Z / clip1.W);
    vec3 ndc2 =
        MAKE_VEC3(clip2.X / clip2.W, clip2.Y / clip2.W, clip2.Z / clip2.W);

    drawTriangle(Framebuffer, ndc0, ndc1, ndc2, Model->Colors[i / 3]);
  }
}

void drawModelMesh(const Framebuffer* Framebuffer, const Model* Model,
                   mat4 Transform, const uint32_t Color) {
  for (size_t i = 0; i < Model->IndexCount; i += 3) {
    const uint32_t index0 = Model->Indices[i];
    const uint32_t index1 = Model->Indices[i + 1];
    const uint32_t index2 = Model->Indices[i + 2];
    vec4 obj0 = MAKE_VEC4(Model->Vertices[index0].X, Model->Vertices[index0].Y,
                          Model->Vertices[index0].Z, 1.0f);
    vec4 obj1 = MAKE_VEC4(Model->Vertices[index1].X, Model->Vertices[index1].Y,
                          Model->Vertices[index1].Z, 1.0f);
    vec4 obj2 = MAKE_VEC4(Model->Vertices[index2].X, Model->Vertices[index2].Y,
                          Model->Vertices[index2].Z, 1.0f);

    vec4 clip0, clip1, clip2;

    clip0 = mat4MulVec4(Transform, obj0);
    clip1 = mat4MulVec4(Transform, obj1);
    clip2 = mat4MulVec4(Transform, obj2);

    vec3 ndc0 =
        MAKE_VEC3(clip0.X / clip0.W, clip0.Y / clip0.W, clip0.Z / clip0.W);
    vec3 ndc1 =
        MAKE_VEC3(clip1.X / clip1.W, clip1.Y / clip1.W, clip1.Z / clip1.W);
    vec3 ndc2 =
        MAKE_VEC3(clip2.X / clip2.W, clip2.Y / clip2.W, clip2.Z / clip2.W);

    drawLine(Framebuffer, ndc0, ndc1, Color);
    drawLine(Framebuffer, ndc1, ndc2, Color);
    drawLine(Framebuffer, ndc2, ndc0, Color);
  }
}
