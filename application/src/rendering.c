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
#include "rendering.h"

#include <stdlib.h>

static inline float edgeFunction(const struct Vec2 A, const struct Vec2 B,
                                 const struct Vec2 C) {
  return (A.X - B.X) * (C.Y - A.Y) - (A.Y - B.Y) * (C.X - A.X);
}

static inline void ndcToScreenNormalized(const struct Vec3 Ndc,
                                         struct Vec3* Normalized) {
  Normalized->X = (Ndc.X + 1.0f) * 0.5f;
  Normalized->Y = (1.0f - Ndc.Y) * 0.5f;
  Normalized->Z = (Ndc.Z + 1.0f) * 0.5f;
}

void clearColorBuffer(const struct Framebuffer* const Framebuffer,
                      const uint32_t ClearValue) {
  uint32_t* pixel = Framebuffer->ColorBuffer;
  const uint32_t* const end =
      Framebuffer->ColorBuffer + Framebuffer->Width * Framebuffer->Height;

  while (pixel < end) {
    *pixel = ClearValue;
    ++pixel;
  }
}

void clearDepthBuffer(const struct Framebuffer* const Framebuffer,
                      const float ClearValue) {
  float* pixel = Framebuffer->DepthBuffer;
  const float* const end =
      Framebuffer->DepthBuffer + Framebuffer->Width * Framebuffer->Height;
  while (pixel < end) {
    *pixel = ClearValue;
    ++pixel;
  }
}

void drawPixel(const struct Framebuffer* const Framebuffer, const uint32_t X,
               const uint32_t Y, const uint32_t Color, const float Depth) {
  if (X >= Framebuffer->Width || Y >= Framebuffer->Height) {
    return;
  }
  const size_t pixelIndex = Y * Framebuffer->Width + X;
  Framebuffer->ColorBuffer[pixelIndex] = Color;
  Framebuffer->DepthBuffer[pixelIndex] = Depth;
}

void drawLine(const struct Framebuffer* const Framebuffer, const struct Vec3 V0,
              const struct Vec3 V1, const uint32_t Color) {
  struct Vec3 norm0, norm1;
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

void drawTriangle(const struct Framebuffer* Framebuffer, const struct Vec3 V0,
                  const struct Vec3 V1, const struct Vec3 V2, const uint32_t) {
  struct Vec3 norm0;
  struct Vec3 norm1;
  struct Vec3 norm2;
  ndcToScreenNormalized(V0, &norm0);
  ndcToScreenNormalized(V1, &norm1);
  ndcToScreenNormalized(V2, &norm2);

  const struct Vec2 pixelV0 =
      MAKE_VEC2(norm0.X * Framebuffer->Width, norm0.Y * Framebuffer->Height);
  const struct Vec2 pixelV1 =
      MAKE_VEC2(norm1.X * Framebuffer->Width, norm1.Y * Framebuffer->Height);
  const struct Vec2 pixelV2 =
      MAKE_VEC2(norm2.X * Framebuffer->Width, norm2.Y * Framebuffer->Height);

  const float area = edgeFunction(pixelV0, pixelV1, pixelV2);

  if (area <= 0.0f) {
    return;
  }

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

  const struct Vec2 initialPixel =
      MAKE_VEC2((float)minX + 0.5f, (float)minY + 0.5f);
  float w0Row = edgeFunction(pixelV1, pixelV2, initialPixel);
  float w1Row = edgeFunction(pixelV2, pixelV0, initialPixel);
  float w2Row = edgeFunction(pixelV0, pixelV1, initialPixel);

  const bool isTopLeft0 =
      (stepXW0 > 0.0f) || (stepXW0 == 0.0f && stepYW0 < 0.0f);
  const bool isTopLeft1 =
      (stepXW1 > 0.0f) || (stepXW1 == 0.0f && stepYW1 < 0.0f);
  const bool isTopLeft2 =
      (stepXW2 > 0.0f) || (stepXW2 == 0.0f && stepYW2 < 0.0f);

  size_t rowStartIndex = minY * Framebuffer->Width;

  for (int32_t y = minY; y < maxY; ++y) {
    float w0 = w0Row;
    float w1 = w1Row;
    float w2 = w2Row;

    for (int32_t x = minX; x < maxX; ++x) {
      const bool pass0 = (w0 > 0.0f) || (w0 == 0.0f && isTopLeft0);
      const bool pass1 = (w1 > 0.0f) || (w1 == 0.0f && isTopLeft1);
      const bool pass2 = (w2 > 0.0f) || (w2 == 0.0f && isTopLeft2);

      if (pass0 && pass1 && pass2) {
        const float b0 = w0 * areaInv;
        const float b1 = w1 * areaInv;
        const float b2 = w2 * areaInv;

        const float depth = b0 * norm0.Z + b1 * norm1.Z + b2 * norm2.Z;
        const size_t pixelIndex = rowStartIndex + x;

        const bool notCulled = depth < Framebuffer->DepthBuffer[pixelIndex] &&
                               depth >= -1.0f && depth <= 1.0f;

        if (notCulled) {
          Framebuffer->DepthBuffer[pixelIndex] = depth;
          constexpr uint32_t color1 = 0xFF0000FF;
          constexpr uint32_t color2 = 0xFF00FF00;
          constexpr uint32_t color3 = 0xFFFF0000;
          Framebuffer->ColorBuffer[pixelIndex] =
              lerpColorBarycentric(color1, color2, color3, b0, b1, b2);
        }
      }
      w0 += stepXW0;
      w1 += stepXW1;
      w2 += stepXW2;
    }

    w0Row += stepYW0;
    w1Row += stepYW1;
    w2Row += stepYW2;
    rowStartIndex += Framebuffer->Width;
  }
}

void drawModel(const struct Framebuffer* Framebuffer, const struct Model* Model,
               const struct Mat4 Transform) {
  for (size_t i = 0; i < Model->IndexCount; i += 3) {
    const uint32_t index0 = Model->Indices[i];
    const uint32_t index1 = Model->Indices[i + 1];
    const uint32_t index2 = Model->Indices[i + 2];
    const struct Vec4 obj0 =
        MAKE_VEC4(Model->Vertices[index0].X, Model->Vertices[index0].Y,
                  Model->Vertices[index0].Z, 1.0f);
    const struct Vec4 obj1 =
        MAKE_VEC4(Model->Vertices[index1].X, Model->Vertices[index1].Y,
                  Model->Vertices[index1].Z, 1.0f);
    const struct Vec4 obj2 =
        MAKE_VEC4(Model->Vertices[index2].X, Model->Vertices[index2].Y,
                  Model->Vertices[index2].Z, 1.0f);

    const struct Vec4 clip0 = mat4MulVec4(Transform, obj0);
    const struct Vec4 clip1 = mat4MulVec4(Transform, obj1);
    const struct Vec4 clip2 = mat4MulVec4(Transform, obj2);

    const struct Vec3 ndc0 =
        MAKE_VEC3(clip0.X / clip0.W, clip0.Y / clip0.W, clip0.Z / clip0.W);
    const struct Vec3 ndc1 =
        MAKE_VEC3(clip1.X / clip1.W, clip1.Y / clip1.W, clip1.Z / clip1.W);
    const struct Vec3 ndc2 =
        MAKE_VEC3(clip2.X / clip2.W, clip2.Y / clip2.W, clip2.Z / clip2.W);

    drawTriangle(Framebuffer, ndc0, ndc1, ndc2, Model->Colors[i / 3]);
  }
}

void drawModelMesh(const struct Framebuffer* Framebuffer,
                   const struct Model* Model, const struct Mat4 Transform,
                   const uint32_t Color) {
  for (size_t i = 0; i < Model->IndexCount; i += 3) {
    const uint32_t index0 = Model->Indices[i];
    const uint32_t index1 = Model->Indices[i + 1];
    const uint32_t index2 = Model->Indices[i + 2];
    const struct Vec4 obj0 =
        MAKE_VEC4(Model->Vertices[index0].X, Model->Vertices[index0].Y,
                  Model->Vertices[index0].Z, 1.0f);
    const struct Vec4 obj1 =
        MAKE_VEC4(Model->Vertices[index1].X, Model->Vertices[index1].Y,
                  Model->Vertices[index1].Z, 1.0f);
    const struct Vec4 obj2 =
        MAKE_VEC4(Model->Vertices[index2].X, Model->Vertices[index2].Y,
                  Model->Vertices[index2].Z, 1.0f);

    const struct Vec4 clip0 = mat4MulVec4(Transform, obj0);
    const struct Vec4 clip1 = mat4MulVec4(Transform, obj1);
    const struct Vec4 clip2 = mat4MulVec4(Transform, obj2);

    const struct Vec3 ndc0 =
        MAKE_VEC3(clip0.X / clip0.W, clip0.Y / clip0.W, clip0.Z / clip0.W);
    const struct Vec3 ndc1 =
        MAKE_VEC3(clip1.X / clip1.W, clip1.Y / clip1.W, clip1.Z / clip1.W);
    const struct Vec3 ndc2 =
        MAKE_VEC3(clip2.X / clip2.W, clip2.Y / clip2.W, clip2.Z / clip2.W);

    drawLine(Framebuffer, ndc0, ndc1, Color);
    drawLine(Framebuffer, ndc1, ndc2, Color);
    drawLine(Framebuffer, ndc2, ndc0, Color);
  }
}
